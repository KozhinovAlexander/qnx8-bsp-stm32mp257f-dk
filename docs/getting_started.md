# Getting Started Guide

This guide describes necessary tools installation needed for this project.

- [Getting Started Guide](#getting-started-guide)
  - [Preparing Host System](#preparing-host-system)
  - [Configuring and building U-Boot for smt32mp257f-dk](#configuring-and-building-u-boot-for-smt32mp257f-dk)
  - [Installing QNX 8](#installing-qnx-8)
  - [Installing stm32 tools](#installing-stm32-tools)

## Preparing Host System

Folloiwng steps shall be done from the [<root_path>](../) of this project to configure host system and install necessary dependencies for development process.

Execute following make commands from [<root_path>](../) to setup git-su7bmodules and install dependencies:

```bash
make git_submodules_configure
make install_dependencies
```

## Configuring and building U-Boot for smt32mp257f-dk

This documentation is based on ST's original documentation for [stm32mp u-boot build and configuration][_stm32mp_uboot_docs_].

To compile U-Boot for stm32mp257f-dk following make command shall be executed:

```bash
make uboot
```

##  Installing QNX 8

The QNX 8 installation is well described in the [original documenataion][_qnx8_install_host_].
Simpliest way ist to use default installation settings. After successful installation [QNX Momentics IDE][_qnx_momentics_install_] shall be installed too.

Install following packages:

- `QNX Momentics IDE`
- `QNX Software Development Platform 8.0`

## Installing stm32 tools

The only tool needed is [STM32CubeProgrammer][_stm32cubeprog_]. This tool allows flashing the board. Please use default installation path.
To use [`make flash_stm32_programmer`](../Makefile#54) operation the path update to [STM32CubeProgrammer][_stm32cubeprog_] installation may be necessary.


[_qnx8_install_host_]: https://www.qnx.com/developers/docs/8.0/com.qnx.doc.qnxsdp.quickstart/topic/install_host.html
[_qnx_momentics_install_]: https://www.qnx.com/developers/docs/qsc/com.qnx.doc.qsc.user_guide/topic/install_qnx_momentics.html
[_stm32cubeprog_]: https://www.st.com/en/development-tools/stm32cubeprog.html
[_qnx_license_]: https://www.qnx.com/products/everywhere/
[_stm32mp_uboot_docs_]: https://wiki.st.com/stm32mpu/wiki/U-Boot_overview#ARM_cross_compiler
