# Real-Time Telemetry Closed-Loop Actuation Workflow

```mermaid
sequenceDiagram
    autonumber
    actor Attacker as 黑客/邊緣極端負載
    participant HW as CPU / GPU 算力層
    participant BMC as AST2600 AIOps 狀態機
    participant RoT as 硬體信任根 (Root of Trust)
    participant Relay as Lanner 實體 Bypass 網卡

    %% Scenario A: Telemetry Sag Protection
    Note over HW, BMC: [情境 A: 突發 AI 推理引發電軌抽電過載]
    HW->>BMC: 突發重載矩陣運算 (Inference Burst)
    HW->>BMC: 電軌瞬降 (Voltage Sag < 1056mV) & PCIe Gen5 重訓
    rect rgb(45, 15, 15)
        BMC->>HW: [OS Layer Bypass] 繞過作業系統，由韌體直接限流限頻 (Predictive Throttling)
    end
    Note over HW, BMC: 暫態錯誤排除，保護晶片免於 Reset 殘留毒污值

    %% Scenario B: Backdoor Intrusion Prevention
    Note over Attacker, Relay: [情境 B: 黑客試圖透過 OOB 漏洞劫持 BMC 大後門]
    Attacker->>BMC: 注入非授權遠端 IPMI/Redfish 暫存器修改
    BMC->>RoT: 觸發 SPDM 1.2 執行階段代碼完整性校驗
    RoT-->>BMC: 驗證失敗 (Cryptographic Attestation Failed)
    
    rect rgb(15, 45, 15)
        critical 5ms 內跨層數據互鎖驅動 (5ms Strict Fail-Safe Actuation)
            BMC->>RoT: 拉高 GPIO 14 (SPI Flash 物理鎖定，防止韌體惡意寫入)
            BMC->>Relay: 拉低 GPIO 23 (網卡繼電器實體短路導通)
        end
    end
    
    Note over Relay: 網絡流量物理隔離直通：100% 保證客戶 SLA 防線，大後門就地封死！