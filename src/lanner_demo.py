import time
import random
import os
import sys

# ==========================================================================
# LANNER SYSTEM SPEC DEFINITIONS (與 C 語言代碼完全等價的頂層規格)
# ==========================================================================
VOLTAGE_NORMAL_MV = 1200
VOLTAGE_SAG_THRESHOLD_MV = 1056  # -12% 臨界線
TOTAL_DEMO_CYCLES = 40

class LannerEdgeAIAppliance:
    def __init__(self):
        # 初始硬體狀態 (Telemetry Fusion Bundle)
        self.cpu_power_w = 45
        self.gpu_voltage_mv = VOLTAGE_NORMAL_MV
        self.pcie_retrain_count = 0
        self.is_authenticated = True
        self.hw_bypass_active = False
        self.rot_lock_active = False
        self.system_state_desc = "[SYS_STATE_SECURE_RUNNING] Core forwarding network packets."

    def execute_arbitration_loop(self, cycle):
        """
        模擬動態工作負載波動 (Inference Burst) 與安全攻擊事件
        """
        # 階段 1：正常網安封包轉發
        if cycle <= 10:
            self.cpu_power_w = 40 + random.randint(0, 5)
            self.gpu_voltage_mv = VOLTAGE_NORMAL_MV + random.randint(-5, 10)
            self.pcie_retrain_count = 0
            self.is_authenticated = True
            self.hw_bypass_active = False
            self.rot_lock_active = False
            self.system_state_desc = "\033[1;32m[SYS_STATE_SECURE_RUNNING] Core forwarding network packets.\033[0m"

        # 階段 2：Edge AI 模型推理開始突發重載
        elif 10 < cycle <= 18:
            self.cpu_power_w = 115 + (cycle * 2)
            self.gpu_voltage_mv = 1180 - (cycle * 3)
            self.pcie_retrain_count = cycle - 10
            self.system_state_desc = "\033[1;33m[SYS_STATE_INFERENCE_BURST] Edge AI Matrix compute peaked.\033[0m"

        # 階段 3：發生二線廠抄 CRB 最頭痛的 Power-rail Stealing 與暫態錯誤
        elif 18 < cycle <= 26:
            self.cpu_power_w = 145
            self.gpu_voltage_mv = 1040 - (cycle % 3)  # 跌破 1056mV 臨界點！
            self.pcie_retrain_count = 8
            self.system_state_desc = "\033[1;35m[SYS_STATE_POWER_SAG_CRITICAL] Rail Voltage drop detected! Overriding OS via Sideband...\033[0m"
            
            # 韌體預測性調流護欄 (Predictive Throttling Actuation) 在後半段發揮效果，拉回電軌
            if cycle >= 23:
                self.gpu_voltage_mv = 1200
                self.pcie_retrain_count = 1
                self.system_state_desc = "\033[1;32m[SYS_STATE_SECURE_RUNNING] Adaptive Throttling Succeeded. Power rail restored.\033[0m"

        # 階段 4：遭受黑客對 BMC 頻外管理通道(OOB)的 Zero-Day 漏洞探測
        elif 26 < cycle <= 32:
            self.cpu_power_w = 45
            self.gpu_voltage_mv = VOLTAGE_NORMAL_MV
            self.pcie_retrain_count = 0
            self.is_authenticated = False  # 🛑 信任根(RoT) 完整性校驗失敗！
            self.system_state_desc = "\033[1;41m[SYS_STATE_OOB_ATTACK_DETECTED] Untrusted Injection on Management Port! Locking Flash layer...\033[0m"

        # 階段 5：終極安全機制觸發：GPIO 5ms 內強行聯動 Hardware Bypass 電路
        else:
            self.cpu_power_w = 15  # 主板強行進入安全鎖定 (Lockdown) 狀態
            self.gpu_voltage_mv = 0
            self.pcie_retrain_count = 0
            self.is_authenticated = False
            self.rot_lock_active = True      # SPI Flash 實體鎖定
            self.hw_bypass_active = True     # ⚡ GPIO Pin 23 拉低成功：網卡實體電路短路直通！
            self.system_state_desc = "\033[1;42m[SYS_STATE_HARDWARE_BYPASS_ACTIVE] LAN Port Shunted. SLA Assured. Backdoor Contained.\033[0m"

    def render_tui_dashboard(self, cycle):
        """
        實作 Linux htop 級別的終端機動態著色看板
        """
        # Windows PowerShell 清除畫面字元
        os.system('cls' if os.name == 'nt' else 'clear')
        
        sys.stdout.write("\033[1;34m========================================================================\033[0m\n")
        sys.stdout.write("\033[1;36m   LANNER EDGE-AI COMPUTE APPLIANCE - LOCAL TELEMETRY & SECURITY DEMO\033[0m\n")
        sys.stdout.write("\033[1;34m========================================================================\033[0m\n")
        sys.stdout.write(f" [Simulation Cycle: {cycle:02d}/{TOTAL_DEMO_CYCLES}]  |  Frequency Layer: OOB Polling Loop 100 microseconds\n")
        sys.stdout.write("------------------------------------------------------------------------\n")
        
        # 輸出 Telemetry Fusion 指標
        sys.stdout.write(f" 🔹 x86 Core CPU Power Draw  : \033[1;33m{self.cpu_power_w:3d} Watts\033[0m\n")
        
        sys.stdout.write(" 🔹 NVIDIA Rail Voltage     : ")
        if self.gpu_voltage_mv >= 1200:
            sys.stdout.write(f"\033[1;32m{self.gpu_voltage_mv:4d} mV [Stable Nom]\033[0m\n")
        elif self.gpu_voltage_mv >= VOLTAGE_SAG_THRESHOLD_MV:
            sys.stdout.write(f"\033[1;33m{self.gpu_voltage_mv:4d} mV [Inference Load Transient]\033[0m\n")
        else:
            sys.stdout.write(f"\033[1;31m{self.gpu_voltage_mv:4d} mV [CRITICAL SINK] - Power-Rail Stealing Triggered!\033[0m\n")
            
        sys.stdout.write(" 🔹 PCIe Gen5 Link LinkErr   : ")
        if self.pcie_retrain_count == 0:
            sys.stdout.write(f"\033[1;32m{self.pcie_retrain_count} (No Jitter)\033[0m\n")
        elif self.pcie_retrain_count <= 4:
            sys.stdout.write(f"\033[1;33m{self.pcie_retrain_count} (Link Quality Degraded)\033[0m\n")
        else:
            sys.stdout.write(f"\033[1;31m{self.pcie_retrain_count} (Link Retraining Boundary Exceeded!)\033[0m\n")
            
        sys.stdout.write("------------------------------------------------------------------------\n")
        
        # 輸出晶片級安全防線狀態
        sys.stdout.write(" 🛡️ Secure Boot Chain Anchor: ")
        if self.is_authenticated:
            sys.stdout.write("\033[1;32m[PASS] SPDM 1.2 Encrypted Attestation Coherent\033[0m\n")
        else:
            sys.stdout.write("\033[1;31m[FAIL] UNTRUSTED INJECTION DETECTED ON OOB MANAGEMENT PORT\033[0m\n")
            
        sys.stdout.write(" ⚡ Hardware Bypass Relay    : ")
        if not self.hw_bypass_active:
            sys.stdout.write("\033[1;32m[DEACTIVE] Normal Operational Pipeline Controlled by OS\033[0m\n")
        else:
            sys.stdout.write("\033[1;31m\033[5m[ACTIVE] 5ms Shunt Relay short-circuited. SLA Assured. Door Sealed.\033[0m\n")
            
        sys.stdout.write("------------------------------------------------------------------------\n")
        sys.stdout.write(f" 🚦 REG_SYSTEM_CORE_STATE   : {self.system_state_desc}\n")
        sys.stdout.write("\033[1;34m========================================================================\033[0m\n")
        sys.stdout.flush()

if __name__ == "__main__":
    appliance = LannerEdgeAIAppliance()
    
    for cycle in range(1, TOTAL_DEMO_CYCLES + 1):
        appliance.execute_arbitration_loop(cycle)
        appliance.render_tui_dashboard(cycle)
        # 每 400ms 動態遞進，形成完美的實時觀測視覺效果
        time.sleep(0.4)
        
    print("\n \033[1;32m✔ [LOCAL DEMO SUCCESSFUL]: 邊緣算力穩定度治理與安全狀態機仿真測試全數跑通！\033[0m\n")