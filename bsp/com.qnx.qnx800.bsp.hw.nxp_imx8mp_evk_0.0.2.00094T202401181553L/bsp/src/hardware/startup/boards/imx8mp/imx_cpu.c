/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017, 2022 NXP
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

#include <startup.h>
#include "imx_startup.h"
#include <soc/nxp/imx8/mp/imx_ccm_analog.h>
#include <soc/nxp/imx8/mp/imx_ocotp.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_cpu.c
 * @addtogroup startup
 * @{
 */

/* VPU disabled */
#define IMX_VPU_DISABLED        0x43000000UL
/* NPU disabled*/
#define IMX_NPU_DISABLED        0x08UL
/* ISP disabled */
#define IMX_ISP_DISABLED        0x03UL
/* GPU disabled */
#define IMX_GPU_DISABLED        0xC0UL
/* LVDS disabled */
#define IMX_LVDS_DISABLED       0x00180000UL
/* MIPI DSI disabled */
#define IMX_MIPI_DSI_DISABLED   0x00060000UL
#define CHIP_STRING_SIZE            30

/**
 * Get chip revision code.
 *
 * @return  Chip revision code.
 */
uint32_t imx_get_chip_rev(void)
{
    uint32_t chip_rev = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DIGPROG);

    return (chip_rev & IMX_CCM_ANALOG_DIGPROG_DIGPROG_MINOR_MASK);
}

/**
 * Get chip type code.
 *
 * @return  Chip type code.
 */
uint32_t imx_get_chip_type(void)
{
    uint32_t tester3_reg = in32(IMX_OCOTP_CTRL_BASE + IMX_HW_OCOTP_TESTER3);
    uint32_t tester4_reg = in32(IMX_OCOTP_CTRL_BASE + IMX_HW_OCOTP_TESTER4);
    uint32_t chip_type, type_flags = 0;

    if ((tester3_reg & 0xC0000) == 0x80000) {
        return IMX_IMX8MPD_MCU_TYPE;
    } else {
        /* VPU disabled */
        type_flags = ((tester4_reg & IMX_VPU_DISABLED) == IMX_VPU_DISABLED) ? (1 << 0) : 0;
        /* NPU disabled*/
        type_flags |= ((tester4_reg & IMX_NPU_DISABLED) == IMX_NPU_DISABLED) ? (1 << 1) : 0;
        /* ISP disabled */
        type_flags |= ((tester4_reg & IMX_ISP_DISABLED) == IMX_ISP_DISABLED) ? (1 << 2) : 0;
        /* GPU disabled */
        type_flags |= ((tester4_reg & IMX_GPU_DISABLED) == IMX_GPU_DISABLED) ? (1 << 3): 0;
        /* LVDS disabled */
        type_flags |= ((tester4_reg & IMX_LVDS_DISABLED) == IMX_LVDS_DISABLED) ? (1 << 4) : 0;
        /* MIPI DSI disabled */
        type_flags |= ((tester4_reg & IMX_MIPI_DSI_DISABLED) == IMX_MIPI_DSI_DISABLED) ? (1 << 5): 0;
    }

    switch (type_flags) {
    case 0x3F:
        chip_type = IMX_IMX8MPUL_MCU_TYPE;
        break;
    case 7:
        chip_type = IMX_IMX8MPL_MCU_TYPE;
        break;
    case 2:
        chip_type = IMX_IMX8MP6_MCU_TYPE;
        break;
    default:
        chip_type = IMX_IMX8MP_MCU_TYPE;
        break;
    }

    return chip_type;
}

/**
 * Get/print chip information: Type, revision.
 */
void print_chip_info(void)
{
    uint32_t chip_type = imx_get_chip_type();
    uint32_t chip_rev = imx_get_chip_rev();
    char chip_type_str[CHIP_STRING_SIZE];

    switch (chip_type) {
    case IMX_IMX8MP_MCU_TYPE:
    case IMX_IMX8MP6_MCU_TYPE:
        strcpy(chip_type_str, "");
        break;
    case IMX_IMX8MPL_MCU_TYPE:
        strcpy(chip_type_str, " Lite");
        break;
    case IMX_IMX8MPD_MCU_TYPE:
        strcpy(chip_type_str, " Dual");
        break;
    case IMX_IMX8MPUL_MCU_TYPE:
        strcpy(chip_type_str, " UltraLite");
        break;
    default:
        strcpy(chip_type_str, " Unknown variant");
        break;
    }
    kprintf("\nDetected i.MX8MPlus%s, revision %d.%d\n\n", chip_type_str,
                                                          ((chip_rev >> 4) & 0x0F),
                                                          (chip_rev & 0x0F));
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_cpu.c $ $Rev: 984580 $")
#endif
