# QNX 8 BSP for STM32MP257F-DK

Board Support Package for **QNX Neutrino RTOS 8.0** targeting the
**STM32MP257F-DK Discovery Kit** (STMicroelectronics).

Please refer first to [**Getting Started Guide**](./docs/getting_started.md) to provide necessary manual installation steps.

- [QNX 8 BSP for STM32MP257F-DK](#qnx-8-bsp-for-stm32mp257f-dk)
	- [Configure project](#configure-project)
	- [Hardware](#hardware)
		- [COM Port configuration](#com-port-configuration)
	- [SD Card Provision](#sd-card-provision)
	- [Building the Project](#building-the-project)
	- [Booting QNX from U-Boot](#booting-qnx-from-u-boot)
	- [References](#references)


## Configure project

After cloning this project refer to [Getting Started](./docs/getting_started.md) Guide to setup the development environment.

## Hardware

| Component        | Details                                           |
|------------------|---------------------------------------------------|
| **SoC**          | STM32MP257FAK3                                    |
| **CPU**          | Dual Cortex-A35 @ 1.5 GHz (AArch64) + Cortex-M33  |
| **RAM**          | 4 GB LPDDR4                                       |
| **Storage**      | 8 GB eMMC 5.1 + microSD                           |
| **Debug UART**   | UART4 @ `0x40010000`, 115200 8N1 (via ST-LINK/V3) |
| **GIC**          | GIC-600 (GICv3), GICD @ `0x58000000`              |
| **Ethernet**     | Gigabit RGMII                                     |
| **USB**          | USB 2.0 HS x2, USB 3.0 SuperSpeed Type-C          |

---

### COM Port configuration

Connect to the COM port (`ls /dev/ttyACM*`) using your favorite terminal program with these settings:

    Baud rate: 115200
    Data: 8 bit
    Parity: none
    Stop: 1 bit
    Flow control: none

## SD Card Provision

A one-time action shall be done to make your SD-Card bootable and populate it with first- and second-stage bootloaders. Insert a Micro-SD card delivered with STM32MP257F-DK or any compatible one into SD-Card reader of your computer and find it with `lsblk` command.

**NOTE:** Currently only linux host is supported for SD-Card provisioning.

Run make command to provision the card:

```sh
make sdcard_provision SD_CARD_DEV=/dev/sdX
```

**ATTENTION:** ENSURE your're flashing correct sd-card found with `lsblk`, otherwise you may destroy your host file-system!

## Building the Project

```sh
make all
```

**NOTE:** Please verify your TFTP configuration - it shall have `/etc/default/tftpd-hpa` config similiar to this one:

```sh
# /etc/default/tftpd-hpa

TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/srv/tftp"
TFTP_ADDRESS=":69"
TFTP_OPTIONS="--secure"
```

## Booting QNX from U-Boot

Setting up U-Boot and booting QNX8 is described in [Manual U-Boot Setup][_man_uboot_setup_]

## References

- [STM32MP257F-DK Product Page](https://www.st.com/en/evaluation-tools/stm32mp257f-dk.html)
- [RM0457 STM32MP23/25xx Reference Manual](https://www.st.com/resource/en/reference_manual/rm0457-stm32mp2325xx-advanced-armbased-3264bit-mpus-stmicroelectronics.pdf)
- [QNX SDP 8.0 Documentation](https://www.qnx.com/developers/docs/)
- [joexue/qemu-virt — minimal public QNX 8 AArch64 BSP](https://github.com/joexue/qemu-virt)
- [QNX RPi4 Quick-Start Image](https://gitlab.com/qnx/quick-start-images/raspberry-pi-qnx-8.0-quick-start-image)


[_man_uboot_setup_]: ./bsp/manual_uboot_setup.md
[_stm32_u_boot_]: https://wiki.st.com/stm32mpu/wiki/U-Boot
