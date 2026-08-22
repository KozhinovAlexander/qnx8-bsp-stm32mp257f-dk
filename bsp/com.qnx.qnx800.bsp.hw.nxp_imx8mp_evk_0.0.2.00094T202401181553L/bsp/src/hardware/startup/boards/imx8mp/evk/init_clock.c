/*
 * Copyright (c) 2016, 2023, BlackBerry Limited.
 * Copyright 2022 NXP
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
 * $
 */

#include <startup.h>
#include "mp_evk_cpu.h"
#include "imx_startup.h"
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_gpc.h>
#include <soc/nxp/imx8/common/imx_smc_call.h>
#include "board.h"

void imx_init_syspll(void)
{
    /* nothing to do yet */
}

#if IMX_DC_INIT_ENABLED
/**
 * Initialize DC clock.
 *
 * @return Execution status.
 */
int imx_init_dc_clock(void)
{
/* Initialize display AXI and APB clocks */
    {
        /* Disable CCGR93(IMX_CCM_CCGR_DISPLAY) common for both DISPLAY_AXI_CLK_ROOT & DISPLAY_APB_CLK_ROOT clock roots */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_MEDIA), 0x03);
        if (soc_overdrive) {
            /* Set DISPLAY_AXI_CLK_ROOT clock root to SYSTEM_PLL2_CLK => 1000MHz enable clock */
            out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_MEDIA_AXI), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));    /* Set MUX to SYSTEM_PLL2_CLK */
        } else {
            /* Set DISPLAY_AXI_CLK_ROOT clock root to SYSTEM_PLL1_CLK => 800MHz enable clock */
            out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_MEDIA_AXI), IMX_CCM_TARGET_ROOT_MUX_VALUE(2));    /* Set MUX to SYSTEM_PLL1_CLK */
        }
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_MEDIA_AXI), IMX_CCM_TARGET_ROOT_PRE_PODF(1)); /* Set PRE_PODF post divider to /2 */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_MEDIA_AXI), IMX_CCM_TARGET_ROOT_ENABLE_MASK); /* Enable clock root */

        /* Set DISPLAY_APB_CLK_ROOT clock root to SYSTEM_PLL1_CLK => 800MHz enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_MEDIA_APB), IMX_CCM_TARGET_ROOT_MUX_VALUE(2));    /* Set MUX to SYSTEM_PLL1_CLK */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_MEDIA_APB), IMX_CCM_TARGET_ROOT_PRE_PODF(3)); /* Set PRE_PODF post divider to /4 to reach 200MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_MEDIA_APB), IMX_CCM_TARGET_ROOT_ENABLE_MASK); /* Enable clock root */
        /* Enable CCGR93(IMX_CCM_CCGR_DISPLAY) common for both DISPLAY_AXI_CLK_ROOT & DISPLAY_APB_CLK_ROOT clock roots */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_MEDIA), 0x03);

    }
    /* Enable power domain for MIPI-DSI and DISPMIX */
    {
        /* Enable DISPMIX power domain */
        (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_MEDIAMIX, 0x01, 0x00);
        /* Enable MIPI-DSI power domain */
        (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_MIPI_PHY1, 0x01, 0x00);
    }
    /* Enable power domain for HDMI */
    {
        /* Enable HDMIMIX power domain */
        (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_HDMIMIX, 0x01, 0x00);
        /* Enable HDMI_PHY power domain */
        (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_HDMI_PHY, 0x01, 0x00);
    }

    return 0;
}
#endif


#if IMX_ECSPI_INIT_ENABLED
/**
 * Initialize LPSPI clock.
 *
 * @return Execution status.
 */
int imx_init_ecspi_clock(void)
{
    /* Set ECSPI2 input clock to 40MHz (SYSTEM_PLL1) */
    {
        /* Disable ECSPI2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_ECSPI2), 0x03);
        /* Set ECSPI2 clock root to SYSTEM_PLL1_DIV20 => 800MHz / 20 =  40MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ECSPI2), IMX_CCM_TARGET_ROOT_MUX_VALUE(2));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_ECSPI2), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable ECSPI2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_ECSPI2), 0x03);
    }

    return 0;
}
#endif

#if IMX_UART_INIT_ENABLED
/**
 * Initialize LPUART clock.
 *
 * @return Execution status.
 */
static int imx_init_uart_clock(void)
{
    return 0;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/init_clock.c $ $Rev: 984580 $")
#endif
