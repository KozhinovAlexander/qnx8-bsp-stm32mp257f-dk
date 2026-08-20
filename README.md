# QNX 8 BSP for STM32MP257F-DK

Board Support Package for **QNX Neutrino RTOS 8.0** targeting the
**STM32MP257F-DK Discovery Kit** (STMicroelectronics).

Please refer first to [**Getting Started Guide**](./docs/getting_started.md) to provide necessary manual installation steps.

- [QNX 8 BSP for STM32MP257F-DK](#qnx-8-bsp-for-stm32mp257f-dk)
  - [Configure project](#configure-project)
  - [Hardware](#hardware)
  - [Boot Flow](#boot-flow)
  - [Repository Layout](#repository-layout)
  - [Prerequisites](#prerequisites)
  - [Building](#building)
    - [Individual targets](#individual-targets)
  - [Flashing \& Booting](#flashing--booting)
    - [Development — TFTP boot (recommended for bring-up)](#development--tftp-boot-recommended-for-bring-up)
    - [Production — boot from eMMC](#production--boot-from-emmc)
  - [Driver Bring-up Priority](#driver-bring-up-priority)
  - [Key Addresses (STM32MP257)](#key-addresses-stm32mp257)
  - [References](#references)
  - [License](#license)


## Configure project

After cloning this project refer to [Getting Started](./docs/getting_started.md) Guide to setup the development environment.

## Hardware

| Component        | Details                                           |
|------------------|---------------------------------------------------|
| **SoC**          | STM32MP257FAK3                                    |
| **CPU**          | Dual Cortex-A35 @ 1.5 GHz (AArch64) + Cortex-M33 |
| **RAM**          | 4 GB LPDDR4                                       |
| **Storage**      | 8 GB eMMC 5.1 + microSD                           |
| **Debug UART**   | UART4 @ `0x40010000`, 115200 8N1 (via ST-LINK/V3) |
| **GIC**          | GIC-600 (GICv3), GICD @ `0x58000000`              |
| **Ethernet**     | Gigabit RGMII                                     |
| **USB**          | USB 2.0 HS x2, USB 3.0 SuperSpeed Type-C          |

---

## Boot Flow

Please refer to [U-Boot STM32 documentation][_stm32_u_boot_] for more information.

```
Power-on
  └─> STM32MP2 ROM Boot
        └─> TF-A (BL2 / BL31)  — EL3, DDR init, TrustZone
              └─> U-Boot (EL1)
                    └─> FIT image (eMMC / SD / TFTP)
                          ├─> QNX IFS  (loaded @ 0xC0100000)
                          └─> DTB      (passed in x0)
                                └─> startup-stm32mp257f-dk
                                      └─> procnto-smp  (QNX kernel)
```

> **Note:** TF-A handles DDR training and EL3 setup before handing off to
> U-Boot at EL1. QNX startup therefore runs at EL1 and must **not**
> re-initialise DDR.

---

## Repository Layout

```
qnx8-bsp-stm32mp257f-dk/
├── src/
│   ├── startup/
│   │   └── startup-stm32mp257f-dk/   # AArch64 QNX startup
│   │       ├── main.c
│   │       ├── init_raminfo.c
│   │       ├── init_intrinfo.c
│   │       ├── init_smp.c
│   │       ├── init_timing.c
│   │       ├── board_smp.c
│   │       ├── callout_debug_stm32_uart.S
│   │       └── Makefile
│   └── hardware/
│       ├── devc/
│       │   └── devc-ser-stm32/       # UART serial driver
│       │       ├── stm32_uart.c
│       │       ├── stm32_uart.h
│       │       └── Makefile
│       ├── i2c/
│       │   └── i2c-stm32mp2/         # I2C driver stub
│       │       └── Makefile
│       └── spi/
│           └── spi-stm32mp2/         # SPI driver stub
│               └── Makefile
├── images/
│   ├── stm32mp257f-dk.build          # mkifs IFS build script
│   └── stm32mp257f-dk.its            # U-Boot FIT image source
├── install/
│   └── qnx-uboot-env.txt             # U-Boot environment variables
├── Makefile                          # Top-level build
└── README.md
```

---

## Prerequisites

| Tool / Package         | Notes                                    |
|------------------------|------------------------------------------|
| QNX SDP 8.0            | Provides toolchain, `mkifs`, `procnto`   |
| `mkimage`              | U-Boot tools — build FIT image           |
| TF-A + U-Boot binaries | Pre-built for STM32MP257F-DK from ST     |
| STM32MP257F-DK DTB     | From Linux kernel or TF-A device tree    |

---

## Building

```bash
# 1. Source the QNX SDP environment
source /path/to/qnx800/qnxsdp-env.sh

# 2. Build startup + drivers + IFS image + FIT image
make all

# Outputs:
#   images/stm32mp257f-dk.ifs   — QNX Image File System
#   images/fitImage-qnx         — U-Boot FIT image
```

### Individual targets

```bash
make startup   # build startup-stm32mp257f-dk only
make drivers   # build all hardware drivers
make image     # run mkifs → stm32mp257f-dk.ifs
make fit       # run mkimage → fitImage-qnx
make clean
```

---

## Flashing & Booting

### Development — TFTP boot (recommended for bring-up)

```bash
# On the host — copy FIT image to TFTP root
cp images/fitImage-qnx /tftpboot/

# In U-Boot console on the board
setenv bootcmd 'run qnxboot_tftp'
run qnxboot_tftp
```

### Production — boot from eMMC

```bash
# Write FIT image to eMMC (offset 2048 sectors = 1 MiB)
sudo dd if=images/fitImage-qnx of=/dev/sdX bs=512 seek=2048

# In U-Boot console
setenv bootcmd 'run qnxboot'
saveenv
reset
```

---

## Driver Bring-up Priority

| Priority | Driver               | Interface              |
|----------|----------------------|------------------------|
| 1        | `devc-ser-stm32`     | UART4 debug console    |
| 2        | `devnp-stm32-gmac`   | Gigabit Ethernet       |
| 3        | `devb-sdmmc-stm32`   | eMMC / microSD         |
| 4        | `i2c-stm32mp2`       | PMIC, sensors          |
| 5        | `spi-stm32mp2`       | SPI NOR flash          |

---

## Key Addresses (STM32MP257)

| Peripheral         | Base Address   |
|--------------------|----------------|
| UART4              | `0x40010000`   |
| GIC Distributor    | `0x58000000`   |
| GIC Redistributor  | `0x580A0000`   |
| RCC                | `0x44200000`   |
| DDR Controller     | `0x5A000000`   |
| DDR PHY            | `0x5A010000`   |

---

## References

- [STM32MP257F-DK Product Page](https://www.st.com/en/evaluation-tools/stm32mp257f-dk.html)
- [RM0457 STM32MP23/25xx Reference Manual](https://www.st.com/resource/en/reference_manual/rm0457-stm32mp2325xx-advanced-armbased-3264bit-mpus-stmicroelectronics.pdf)
- [QNX SDP 8.0 Documentation](https://www.qnx.com/developers/docs/)
- [joexue/qemu-virt — minimal public QNX 8 AArch64 BSP](https://github.com/joexue/qemu-virt)
- [QNX RPi4 Quick-Start Image](https://gitlab.com/qnx/quick-start-images/raspberry-pi-qnx-8.0-quick-start-image)

---

## License

This BSP skeleton is provided as a starting point for porting QNX 8 to the
STM32MP257F-DK. Adapt and extend as required for your project.


[_stm32_u_boot_]: https://wiki.st.com/stm32mpu/wiki/U-Boot
