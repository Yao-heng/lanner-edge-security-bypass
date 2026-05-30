/**
 * @file lanner_edge_security_gateway.h
 * @brief Zero-Trust Edge-AI Security & Bypass Infrastructure Specs
 * @author Tony Lee (FW Director Candidate)
 */

#ifndef LANNER_EDGE_SECURITY_GATEWAY_H
#define LANNER_EDGE_SECURITY_GATEWAY_H

#include <stdint.h>
#include <stdbool.h>

/* 信驊 AST2600 暫存器實實體 Memory-Mapped I/O 基底地址 */
#define AST2600_GPIO_BASE            0x1E780000 
#define REG_GPIO_DATA_ROUTING        (AST2600_GPIO_BASE + 0x00)
#define REG_GPIO_DIRECTION           (AST2600_GPIO_BASE + 0x04)

/* 閾值指標設定 (System Threshold Boundaries) */
#define TELEMETRY_POLLING_US         100        // 100微秒超高頻遙測採樣 (Sub-millisecond)
#define VOLTAGE_SAG_THRESHOLD_MV     1056       // -12% 電壓瞬間下陷臨界值 (Voltage Sag)
#define MAX_PCIE_RETRAIN_ALLOWED     5          // PCIe 訊號抖動重訓容忍上限
#define TOTAL_DEMO_CYCLES            40         // 動態面板展示總週期

#define GPIO_PIN_HW_BYPASS_EN        23         // 控制立瑞網卡實體短路的 GPIO 引腳 (Active Low)
#define GPIO_PIN_ROT_LOCK            14         // 聯動硬體信任根 (Root of Trust) 實體鎖定引腳

typedef struct {
    uint32_t cpu_power_w;
    uint32_t gpu_voltage_mv;
    uint32_t pcie_retrain_count;
    bool     is_authenticated;
} TelemetryFusionBundle_t;

typedef enum {
    SYS_STATE_SECURE_RUNNING = 0,
    SYS_STATE_INFERENCE_BURST,
    SYS_STATE_POWER_SAG_MITIGATION,
    SYS_STATE_OOB_ATTACK_DETECTED,
    SYS_STATE_HARDWARE_BYPASS_ACTIVE
} LannerSystemState_t;

#endif // LANNER_EDGE_SECURITY_GATEWAY_H