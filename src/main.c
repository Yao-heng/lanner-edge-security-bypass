/**
 * @file main.c
 * @brief Bare-Metal Pipeline Simulation Core with TUI Dashboard Output
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lanner_edge_security_gateway.h"

extern LannerSystemState_t execute_power_rail_guardian(TelemetryFusionBundle_t* packet);
extern LannerSystemState_t execute_oob_intrusion_blocker(TelemetryFusionBundle_t* packet);

int main() {
    // 強制開啟 ANSI 色彩協議支援（相容 Windows PowerShell 看板顯示）
    #ifndef RUNNING_ON_REAL_LANNER_HW
    system(""); 
    #endif

    TelemetryFusionBundle_t telemetry = {45, 1200, 0, true};
    LannerSystemState_t current_state = SYS_STATE_SECURE_RUNNING;

    for (int cycle = 1; cycle <= TOTAL_DEMO_CYCLES; cycle++) {
        /* 動態模擬環境變數跳擺 */
        if (cycle <= 10) {
            telemetry.cpu_power_w = 42 + (cycle % 2);
            telemetry.gpu_voltage_mv = 1200;
            telemetry.pcie_retrain_count = 0;
            telemetry.is_authenticated = true;
            current_state = execute_power_rail_guardian(&telemetry);
        } else if (cycle > 10 && cycle <= 18) {
            telemetry.cpu_power_w = 120;
            telemetry.gpu_voltage_mv = 1120;
            telemetry.pcie_retrain_count = cycle - 10;
            current_state = execute_power_rail_guardian(&telemetry);
        } else if (cycle > 18 && cycle <= 26) {
            telemetry.gpu_voltage_mv = 1030; // 跌破 1056mV 臨界！
            telemetry.pcie_retrain_count = 7;
            current_state = execute_power_rail_guardian(&telemetry);
            if (cycle >= 23) { telemetry.gpu_voltage_mv = 1200; current_state = SYS_STATE_SECURE_RUNNING; }
        } else {
            telemetry.is_authenticated = false; // 🛑 偵測到惡意探測 BMC 後門行為！
            current_state = execute_oob_intrusion_blocker(&telemetry);
        }

        /* 輸出美商大廠級 TUI 實時維運看板 */
        printf("\033[H\033[J");
        printf("\033[1;34m========================================================================\033[0m\n");
        printf("\033[1;36m   LANNER INDUSTRIAL APPLIANCE - BARE-METAL TELEMETRY INTERLOCK DEMO\033[0m\n");
        printf("\033[1;34m========================================================================\033[0m\n");
        printf(" [Cycle: %02d/%d] | Execution Mode: %s\n", cycle, TOTAL_DEMO_CYCLES, 
#ifdef RUNNING_ON_REAL_LANNER_HW
        "\033[1;31m[PRODUCTION HARDWARE CHIP ENGAGED]\033[0m"
#else
        "\033[1;32m[WINDOWS POWERSHELL VIRTUAL SIMULATOR]\033[0m"
#endif
        );
        printf("------------------------------------------------------------------------\n");
        printf(" 🔹 CPU Ingestion Tracker    : \033[1;33m%3d Watts\033[0m\n", telemetry.cpu_power_w);
        printf(" 🔹 GPU Rail Voltage Sensor  : \033[1;32m%4d mV\033[0m\n", telemetry.gpu_voltage_mv);
        printf(" 🔹 PCIe Gen5 Link LinkErr   : %d\n", telemetry.pcie_retrain_count);
        printf("------------------------------------------------------------------------\n");
        printf(" 🛡️ SPDM 1.2 Attestation Chain: %s\n", telemetry.is_authenticated ? "\033[1;32m[PASS]\033[0m" : "\033[1;31m[COMPROMISED!]\033[0m");
        printf(" ⚡ Hardware Bypass Relay Link: %s\n", (current_state == SYS_STATE_HARDWARE_BYPASS_ACTIVE) ? "\033[1;31m\033[5m[ENGAGED (5ms FAIL-SAFE ACTIVE)]\033[0m" : "\033[1;32m[NORMAL PIPELINE]\033[0m");
        printf("------------------------------------------------------------------------\n");
        printf(" 🚦 ARCHITECTURE STATE       : ");
        if (current_state == SYS_STATE_HARDWARE_BYPASS_ACTIVE) printf("\033[1;42m[STATE_HARDWARE_BYPASS_ACTIVE]\033[0m\n");
        else if (current_state == SYS_STATE_POWER_SAG_MITIGATION) printf("\033[1;35m[STATE_POWER_SAG_MITIGATION]\033[0m\n");
        else printf("\033[1;32m[STATE_OPERATIONAL_NORMAL]\033[0m\n");
        printf("\033[1;34m========================================================================\033[0m\n");
        
        fflush(stdout);
        usleep(300000); 
    }
    return 0;
}