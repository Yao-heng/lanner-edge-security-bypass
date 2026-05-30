# High-Density Edge-AI Telemetry & Cybersecurity Actuation Register Specification

This document defines the Memory-Mapped I/O (MMIO) address boundaries and register bitmasks utilized by the Zero-Trust Bypass Coherence Engine on the ASPEED AST2600 management processor.

## 1. MMIO Controller Base Addresses
* **AST2600_GPIO_REG_BASE**: `0x1E780000` (Hardware pin actuation multiplexer)
* **HOST_PECI_INTERFACE_BASE**: `0x1E78A000` (CPU sub-millisecond telemetry link)
* **PCIE_AER_CONTROLLER_BASE**: `0x1E6E2000` (PCIe Advanced Error Reporting & link monitoring)

## 2. Ingestion Register Bit Allocation (亞毫秒級硬體遙測輸入)

### 0x1E780020: REG_HOST_POWER_TRAIL_STATUS (100μs Polling Loop)
* `Bit [31:16]`: CPU_PACKAGE_POWER_RAW (unsigned 16-bit vector reflecting instant SoC wattage draw).
* `Bit [15:08]`: GPU_RAIL_VOLTAGE_RAW (dynamic voltage feedback from board-level VRM, resolution 1mV per LSB).
* `Bit [07:00]`: CURRENT_CSTATE_RESIDUE (indicates core power state residency to prevent clock-gating drops during inference bursts).

### 0x1E6E204C: REG_PCIE_GEN5_LINK_INTEGRITY
* `Bit [07:00]`: HARD_LINK_RETRAIN_CNT (triggers `VOLTAGE_SAG_MITIGATION` if count > 5 under continuous dynamic ripple).

## 3. Actuation Register Bit Allocation (5ms 實體防線驅動)

### 0x1E780000: REG_GPIO_DATA_ROUTING (Output Control)
* `Bit 23` (`GPIO_PIN_HW_BYPASS_EN`): Active-Low control for Lanner proprietary bypass hardware relays. 
  * `0`: Actuate physical shunt relay immediately. Shuts down untrusted interface pipelines and bridges network interfaces via short-circuit to preserve client SLA.
  * `1`: Regular software-defined data path controlled by host operating system (Default).
* `Bit 14` (`GPIO_PIN_ROT_LOCK`): Active-High root-of-trust hardware lockpin.
  * `1`: Forcibly isolate management SPI Flash layer to contain out-of-band zero-day injections.