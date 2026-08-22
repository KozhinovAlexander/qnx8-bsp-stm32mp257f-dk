/*
 * Copyright (c) 2016, 2022-2023, BlackBerry Limited.
 * Copyright 2022-2023 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 */


#ifndef BOARD_H_
#define BOARD_H_

#ifndef __ASM__
#include "imx_startup.h"
#endif
#include <soc/nxp/imx8/mp/mx8mp.h>

/**
 * i.MX startup source file.
 *
 * @file       imx8mp/evk/board.h
 * @addtogroup startup
 * @{
 */

/* Peripheral PAD specific section */
/* USDHC */
#define IMX8MP_PAD_SETTINGS_USDHC (IMX_PAD_CTL_PE_PULL_ENABLED | \
                                   IMX_PAD_CTL_PUE_PULL_UP | \
                                   IMX_PAD_CTL_HYS_SCHMITT | \
                                   IMX_PAD_CTL_SRE_FAST | \
                                   IMX_PAD_CTL_DSE_2X)
/* I2C */
#define IMX8MP_PAD_SETTINGS_I2C   (IMX_PAD_CTL_ODE_OPEN_DRAIN | \
                                   IMX_PAD_CTL_PE_PULL_ENABLED | \
                                   IMX_PAD_CTL_PUE_PULL_UP | \
                                   IMX_PAD_CTL_HYS_SCHMITT | \
                                   IMX_PAD_CTL_SRE_SLOW | \
                                   IMX_PAD_CTL_DSE_6X)

/** Enable initialization of peripheral clocks and pads */
#define IMX_GPT_INIT_ENABLED               1
#define IMX_ENET_INIT_ENABLED              1
#define IMX_ENET_GET_MAC_ENABLED           1
#define IMX_UART_INIT_ENABLED              0
#define IMX_ISI_CSI_INIT_ENABLED           0
#define IMX_I2C_INIT_ENABLED               1
#define IMX_I2C1_INIT_ENABLED              1
#define IMX_I2C2_INIT_ENABLED              1
#define IMX_I2C3_INIT_ENABLED              1
#define IMX_I2C4_INIT_ENABLED              0     /* I2C4 is disabled - pins are used by PCIe */
#define IMX_I2C5_INIT_ENABLED              0     /* I2C5 is disabled - pins are shared with CAN1 */
#define IMX_ECSPI_INIT_ENABLED             1
#define IMX_QSPI_INIT_ENABLED              1
#define IMX_USDHC_INIT_ENABLED             1
#define IMX_NAND_INIT_ENABLED              0
#define IMX_USB_INIT_ENABLED               1
#define IMX_DC_INIT_ENABLED                1
#define IMX_AUDIO_INIT_ENABLED             1
#define IMX_FLEXCAN_INIT_ENABLED           1
#define IMX_GPIO_INIT_ENABLED              1
#define IMX_PCIE_INIT_ENABLED              1
#define IMX_MIPI_CSI_INIT_ENABLED          0
#define IMX_GPU_INIT_ENABLED               1
#define IMX_VPU_INIT_ENABLED               0
/* Note: CAN2 is in conflict with SAI5 and SPDIF which are not implemented
 *       Can be enabled here */
#define IMX_FLEXCAN2_ENABLED               1

/** i.MX MCU cores number */
#define IMX_MCU_CORES_NUMBER               4

/*!
 * @name QNX SDRAM memory configuration
 */
/*@{*/
/** SDRAM0 base address */
#define IMX_SDRAM0_BASE                    0x40000000
/** SDRAM0 size in kB */
#define IMX_SDRAM0_SIZE                    3072
/** SDRAM1 base address */
#define IMX_SDRAM1_BASE                    0x100000000
/** SDRAM1 size in kB */
#define IMX_SDRAM1_SIZE                    3072
/*@}*/

/** Base address for GPT driver */
#define IMX_GPT_DRV_BASE                   (IMX_GPT1_BASE)

/** GPT imx timer information (in MHz) */
#define IMX_GPT_CLOCK_FREQ                 24000000UL

/** Core counter input clock (in MHz) */
#define IMX_CNTV_CLOCK_FREQ                8000000


/* PCA6416 */
#define EXP_CAN_ADDR                       0x20

/* PCA6416 registers */
#define EXP_INPUT_PORT_0_REGISTER               0x00
#define EXP_INPUT_PORT_1_REGISTER               0x01
#define EXP_OUTPUT_PORT_0_REGISTER              0x02
#define EXP_OUTPUT_PORT_1_REGISTER              0x03
#define EXP_POLARITY_INVERSION_PORT_0_REGISTER  0x04
#define EXP_POLARITY_INVERSION_PORT_1_REGISTER  0x05
#define EXP_CONFIGURATION_PORT_0_REGISTER       0x06
#define EXP_CONFIGURATION_PORT_1_REGISTER       0x07

/* IO Expander output pins - port0 - Base Board */
#define BB_EXP_PWREN1                      0x01
#define BB_EXP_PWREN2                      0x02
#define BB_CAN_I2C5_SEL                    0x04
#define BB_PDM_CAN2_SEL                    0x08
#define BB_FAN_EN                          0x10
#define BB_PWR_MEAS_IO1                    0x20
#define BB_PWR_MEAS_IO2                    0x40
#define BB_EXP_P07                         0x80

/**
 * Board information:
 * -n = board name.
 */
#define IMX_BOARD_INFO                     "-n evk"

/*@{*/
/** WDOG device base address */
#define IMX_WDOG_BASE                       (IMX_WDOG1_BASE)
/** The watchdog timeout value to 7500 mili seconds */
#define IMX_WDOG_TIMEOUT                    7500
/** Enable debug information about source of the last reset */
#define IMX_CHECK_RESET                     0
/*@}*/

#ifndef __ASM__

void imx_init_wdog_clock(void);
void imx_wdg_reload(void);
void imx_wdg_enable(void);
unsigned char imx_get_soc_overdrive(void);

#if IMX_USB_INIT_ENABLED
void imx_usb3_otg_host_init(void);
#endif

#if IMX_I2C_INIT_ENABLED
int imx_init_i2c_pads(void);
#endif

void imx_init_syspll(void);
#if IMX_DC_INIT_ENABLED
int imx_init_dc_clock(void);
#endif

#if IMX_ECSPI_INIT_ENABLED
int imx_init_ecspi_clock(void);
#endif

#if IMX_UART_INIT_ENABLED
int imx_init_uart_clock(void);
#endif

#endif

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/board.h $ $Rev: 985114 $")
#endif
