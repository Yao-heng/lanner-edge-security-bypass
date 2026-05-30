/**
 * @file lanner_edge_security_gateway.c
 * @brief Telemetry Fusion and Bare-Metal Hardware Actuation State Machine
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lanner_edge_security_gateway.h"

#ifdef RUNNING_ON_REAL_LANNER_HW
#include <sys/mmap.h>
#endif

/**
 * @brief 寫入實體板端的 GPIO 暫存器位元 (MMIO 物理操作)
 */
void set_physical_gpio(uint32_t pin, bool level) {
#ifdef RUNNING_ON_REAL_LANNER_HW
    // 【真實硬體落地】：直接透過記憶體映射 (mmap) 去強行硬體級操作實體引腳暫存器
    int mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) return;

    uint32_t *gpio_map = (uint32_t *)mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, AST2600_GPIO_BASE);
    if (gpio_map == MAP_FAILED) { close(mem_fd); return; }

    if (level) {
        gpio_map[0] |= (1 << pin);  
    } else {
        gpio_map[0] &= ~(1 << pin); // 拉低 GPIO 23, 5ms內實體短路網卡，啟動直通機制
    }

    munmap(gpio_map, getpagesize());
    close(mem_fd);
#else
    // 【本地 Windows 模擬展示版】：移除實體記憶體尋址依賴，只輸出狀態 log
    (void)pin;
    (void)level;
#endif
}

LannerSystemState_t execute_power_rail_guardian(TelemetryFusionBundle_t* packet) {
    if (packet->gpu_voltage_mv < VOLTAGE_SAG_THRESHOLD_MV && packet->pcie_retrain_count > MAX_PCIE_RETRAIN_ALLOWED) {
        return SYS_STATE_POWER_SAG_MITIGATION;
    }
    if (packet->cpu_power_w > 100) return SYS_STATE_INFERENCE_BURST;
    return SYS_STATE_SECURE_RUNNING;
}

LannerSystemState_t execute_oob_intrusion_blocker(TelemetryFusionBundle_t* packet) {
    if (!packet->is_authenticated) {
        set_physical_gpio(GPIO_PIN_ROT_LOCK, true);   // 鎖死 SPI Flash 後門
        set_physical_gpio(GPIO_PIN_HW_BYPASS_EN, false); // 聯動實體網卡直通控制線 (保證SLA)
        return SYS_STATE_HARDWARE_BYPASS_ACTIVE;
    }
    return SYS_STATE_SECURE_RUNNING;
}