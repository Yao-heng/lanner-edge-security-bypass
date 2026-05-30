# Edge-AI Infrastructure Obsv & Bypass Coherence Block Diagram

```mermaid
graph TD
    %% Base Styling
    classDef hardware fill:#1a237e,stroke:#303f9f,stroke-width:2px,color:#fff;
    classDef software fill:#004d40,stroke:#00796b,stroke-width:2px,color:#fff;
    classDef security fill:#b71c1c,stroke:#d32f2f,stroke-width:2px,color:#fff;
    classDef telemetry fill:#e65100,stroke:#f57c00,stroke-width:2px,color:#fff;

    %% Hardware Layers
    subg_Host[Host Compute Subsystem]
        CPU[Intel/AMD CPU Core<br/>Complex Schedule]:::hardware
        GPU[NVIDIA GPU / Jetson SoM<br/>Heavy Inference Burst]:::hardware
        PCIe[PCIe Gen 5/6 Bus Topology<br/>Dynamic Link Training]:::hardware
    end

    subg_Lanner[Lanner Proprietary Network I/O]
        NIC[Network Security Card<br/>Hardware Bypass Relay]:::hardware
    end

    %% Management & Security Control Tiers
    subg_OOB[Out-of-Band Management & Automation Control]
        BMC[ASPEED AST2600 BMC Engine<br/>Tony's Closed-Loop AIOps Engine]:::telemetry
        RoT[Hardware Root of Trust<br/>Cryptographic Anchor / SPDM 1.2]:::security
    end

    %% Interconnections & Telemetry
    CPU <=>|PCIe Gen 5 Data Pipeline| GPU
    CPU -.->| PE Bus Telemetry | BMC
    GPU -.->| Sub-ms Voltage Sag Telemetry | BMC
    PCIe -.->| 100μs AER Link Polling | BMC

    BMC ==>| GPIO Pin 14 Lock Flash | RoT
    BMC ==>| GPIO Pin 23 Active Low | NIC

    class BMC,RoT,NIC telemetry,security,hardware;