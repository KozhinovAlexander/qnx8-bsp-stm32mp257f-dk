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

#include <startup.h>
#include "imx_startup.h"
#include "board.h"
#include <soc/nxp/imx8/mp/imx_ccm_analog.h>
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_gpc.h>
#include <soc/nxp/imx8/mp/imx_src.h>
#include <soc/nxp/imx8/common/imx_smc_call.h>
#include <soc/nxp/imx8/common/imx_aipstz.h>
#include <soc/nxp/imx8/mp/imx_iomuxc_gpr.h>
#include "imx_i2c_drv.h"
/**
 * i.MX startup source file.
 *
 * @file       imx_init_clocks.c
 * @addtogroup startup
 * @{
 */

unsigned char soc_overdrive = 0;

/**
 * Initialize AUDIO and VIDEO PLLs.
 *
 * @return Execution status.
 */
static int imx_init_pll(void)
{
    uint32_t val_cfg0, val;

    /** Configure AUDIO 1 PLL = 393216000 Hz */
    {
        /* Bypass the AUDIO PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_BYPASS_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_RST_MASK)));
        /* Configure AUDIO 1 PLL to 393216000 Hz */
        val = IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_MAIN_DIV(262) |
              IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_PRE_DIV(2) |
              IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_POST_DIV(3);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0, val);
        val = IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL1_PLL_DSM(9437);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL1, val);
        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_RST_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL, val_cfg0);
        /* Wait for PLL lock */
        while(!(in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL) & IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_LOCK_MASK));
        /* Disable bypass */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL, val_cfg0);
    }
    /** Configure AUDIO 2 PLL = 361267200 Hz */
    {
        /* Bypass the AUDIO PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL_PLL_BYPASS_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL_PLL_RST_MASK)));
        /* Configure AUDIO 2 PLL to 361267200 Hz */
        val = IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL0_PLL_MAIN_DIV(361) |
              IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL0_PLL_PRE_DIV(3) |
              IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL0_PLL_POST_DIV(3);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL0, val);
        val = IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL1_PLL_DSM(17511);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_FDIV_CTL1, val);
        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL_PLL_RST_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL, val_cfg0);
        /* Wait for PLL lock */
        while(!(in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL) & IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL_PLL_LOCK_MASK));
        /* Disable bypass */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL2_GEN_CTRL, val_cfg0);
    }
    /** Configure VIDEO PLL = 650MHz */
    {
        /* Bypass the VIDEO PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL_PLL_BYPASS_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL_PLL_RST_MASK)));
        /* Configure VIDEO PLL to 650MHz */
        val = IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL0_PLL_MAIN_DIV(325) |
              IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL0_PLL_PRE_DIV(3) |
              IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL0_PLL_POST_DIV(2);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL0, val);
        val = IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL1_PLL_DSM(0);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL1, val);
        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL_PLL_RST_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL, val_cfg0);
        /* Wait for PLL lock */
        while(!(in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL) & IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL_PLL_LOCK_MASK));
        /* Disable bypass */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL, val_cfg0);
    }
    /** Configure GPU PLL = 1000MHz */
    {
        /* Bypass the GPU PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Set the GPU PLL value: MainDiv=250, PreDiv=3, PostDiv=1 */
        val = IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_MAIN_DIV(250) |
                IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_PRE_DIV(3) |
                IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_POST_DIV(1);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0, val);
        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL, val_cfg0);
        /* Wait for GPU_PLL lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL) & IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_LOCK_MASK) == 0) {}
        /* Clear bypass the GPU PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL, val_cfg0);
        /* GPU PLL output clock gating enable */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_CLKE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL, val_cfg0);
    }
    /** Configure VPU PLL = 600MHz */
    {
        /* Bypass the VPU PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Set the VPU PLL value: MainDiv=300, PreDiv=3, PostDiv=2 */
        val = IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_MAIN_DIV(300) |
                IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_PRE_DIV(3) |
                IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_POST_DIV(2);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0, val);
        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL, val_cfg0);
        /* Wait for VPU_PLL lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL) & IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_LOCK_MASK) == 0) {}
        /* Clear bypass the VPU PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL, val_cfg0);
        /* VPU PLL output clock gating enable */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_CLKE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL, val_cfg0);
    }
    return 0;
}

/**
 * Initialize WDOG clocks.
 */
void imx_init_wdog_clock(void)
{
    /* Disable WDOG1 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_WDOG1), 0x03);
    /* Disable WDOG2 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_WDOG2), 0x03);
    /* Disable WDOG3 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_WDOG3), 0x03);
    /* Set WDOG clock root to 24 MHz, CLK slice 115, 24M_REF_CLK => mux 0 */
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_WDOG), (IMX_CCM_TARGET_ROOT_MUX_VALUE(0) |
                                                                         IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                         IMX_CCM_TARGET_ROOT_POST_PODF(1)));
    /* Enable WDOG clock root */
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_WDOG), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    /* Enable WDOG1 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_WDOG1), 0x03);
    /* Enable WDOG2 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_WDOG2), 0x03);
    /* Enable WDOG3 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_WDOG3), 0x03);
}

#if IMX_USB_INIT_ENABLED
/**
 * Initialize USB clocks.
 *
 * @return Execution status.
 */
static int imx_init_usb_clock(void)
{
    /* Disable USB clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USB), 0x00);
    /* Disable USB PHY clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USB_PHY), 0x00);

    /* Set IMX_CCM_TARGET_USB_CORE_REF clock root to 100 MHz, enable clock */
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USB_CORE_REF), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                             IMX_CCM_TARGET_ROOT_ENABLE_MASK));

    /* Set IMX_CCM_TARGET_USB_PHY_REF root to 100 MHz, enable clock */
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USB_PHY_REF), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                            IMX_CCM_TARGET_ROOT_ENABLE_MASK));

    /* Enable USB clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USB), 0x03);
    /* Enable USB PHY clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USB_PHY), 0x03);

    return 0;
}
#endif

#if IMX_ENET_INIT_ENABLED
/**
 * Initialize ENET clock.
 *
 * @return Execution status.
 */
static int imx_init_enet_clock(void)
{
    { /* ENET_QOS */
        uint32_t gpr1;

        gpr1 = in32(IMX_IOMUXC_GPR_BASE + IMX_IOMUXC_GPR1);

        gpr1 |= IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_RGMII_EN_MASK
                | IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_GEN_EN_MASK
                | IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_INTF_SEL(0x01);
        gpr1 &= ~IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_TX_CLK_SEL_MASK;

        out32((IMX_IOMUXC_GPR_BASE + IMX_IOMUXC_GPR1), gpr1);

        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_QOS_ENET), 0x03);
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_ENET_QOS), 0x03);

        /* Set ENET_QOS_CLK_ROOT clock root to 125 MHz, CLK slice 81, SYS_PLL2_DIV8 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_QOS), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                | IMX_CCM_TARGET_ROOT_PRE_PODF(1)
                                                                                | IMX_CCM_TARGET_ROOT_POST_PODF(1) |
                                                                                IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        /* Set IMX_CCM_TARGET_ENET_QOS_TIMER clock root to 100 MHz, CLK slice 83, SYS_PLL2_DIV10 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_QOS_TIMER), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                    | IMX_CCM_TARGET_ROOT_PRE_PODF(1)
                                                                                    | IMX_CCM_TARGET_ROOT_POST_PODF(1) |
                                                                                    IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_QOS_ENET), 0x03);
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_ENET_QOS), 0x03);
    }

    { /* ENET1 */
        out32((IMX_IOMUXC_GPR_BASE + IMX_IOMUXC_GPR1), (in32(IMX_IOMUXC_GPR_BASE + IMX_IOMUXC_GPR1)) |
                                                            IMX_IOMUXC_GPR_IOMUXC_GPR_ENET1_RGMII_EN_MASK);

        /* Disable ENET1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_ENET1), 0x03);
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_SIM_ENET), 0x03);

        /* Set ENET_AXI clock root to 266 MHz, CLK slice 17, SYS_PLL1_DIV3 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_AXI), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                | IMX_CCM_TARGET_ROOT_PRE_PODF(1)
                                                                                | IMX_CCM_TARGET_ROOT_POST_PODF(1) |
                                                                                IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        /* Set ENET_REF clock root to 125 MHz, CLK slice 83, SYS_PLL2_DIV8 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_REF), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                | IMX_CCM_TARGET_ROOT_PRE_PODF(1)
                                                                                | IMX_CCM_TARGET_ROOT_POST_PODF(1) |
                                                                                IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        /* Set ENET_TIMER clock root to 100 MHz, CLK slice 84, SYS_PLL2_DIV10 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_TIMER), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                | IMX_CCM_TARGET_ROOT_PRE_PODF(1)
                                                                                | IMX_CCM_TARGET_ROOT_POST_PODF(4) |
                                                                                IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        /* Set ENET_PHY clock root to 50 MHz, CLK slice 85, SYS_PLL2_DIV20 => mux 1, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ENET_PHY_REF), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1)
                                                                                    | IMX_CCM_TARGET_ROOT_POST_PODF(1) |
                                                                                    IMX_CCM_TARGET_ROOT_ENABLE_MASK));

        /* Enable ENET1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_SIM_ENET), 0x03);
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_ENET1), 0x03);
    }
    return 0;
}
#endif

#if IMX_GPIO_INIT_ENABLED
/**
 * Initialize GPIO clock.
 *
 * @param ipc IPC handle.
 *
 * @return    Execution status.
 */
static int imx_init_gpio_clock(void)
{
    /* Enable GPIO2 clock root */
     out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_GPIO2), 0x03);
    /* Enable GPIO4 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_GPIO4), 0x03);
    /* Enable GPIO5 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_GPIO5), 0x03);

    return 0;
}
#endif

#if IMX_GPT_INIT_ENABLED
/**
 * Initialize GPT clock.
 *
 * @return Execution status.
 */
static int imx_init_gpt_clock(void)
{
    /* Set GPT1 input clock to 24MHz */
    {
        /* Disable GPT1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_GPT1), 0x03);
        /* Set GPT1 clock root to 24 MHz oscillator, CLK slice 107, 24M_REF_CLK => mux 0, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPT1), 0x00);
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPT1), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable GPT1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_GPT1), 0x03);
    }

    return 0;
}
#endif

#if IMX_I2C_INIT_ENABLED
/**
 * Initialize I2C clock.
 *
 * @return Execution status.
 */
static int imx_init_i2c_clock(void)
{
#if IMX_I2C1_INIT_ENABLED
    /* Set I2C1 input clock to 66MHz (SYSTEM_PLL1) */
    {
        /* Disable I2C1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_I2C1), 0x03);
        /* Set I2C1 clock root to SYSTEM_PLL1_DIV6 => 133MHz / 2 =  66MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C1), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C1), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable I2C1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C1), 0x03);
    }
#endif

#if IMX_I2C2_INIT_ENABLED
    /* Set I2C2 input clock to 66MHz (SYSTEM_PLL1) */
    {
        /* Disable I2C2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_I2C2), 0x03);
        /* Set I2C2 clock root to SYSTEM_PLL1_DIV6 => 133MHz / 2 =  66MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C2), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C2), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable I2C2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C2), 0x03);
    }
#endif

#if IMX_I2C3_INIT_ENABLED
    /* Set I2C3 input clock to 66MHz (SYSTEM_PLL1) */
    {
        /* Disable I2C3 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_I2C3), 0x03);
        /* Set I2C3 clock root to SYSTEM_PLL1_DIV6 => 133MHz / 2 =  66MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C3), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C3), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable I2C3 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C3), 0x03);
    }
#endif

#if IMX_I2C4_INIT_ENABLED
    /* Set I2C4 input clock to 66MHz (SYSTEM_PLL1) */
    {
        /* Disable I2C4 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_I2C4), 0x03);
        /* Set I2C4 clock root to SYSTEM_PLL1_DIV6 => 133MHz / 2 =  66MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C4), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C4), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable I2C4 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C4), 0x03);
    }
#endif

#if IMX_I2C5_INIT_ENABLED
    /* Set I2C5 input clock to 66MHz (SYSTEM_PLL1) */
    {
        /* Disable I2C5 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_I2C5), 0x03);
        /* Set I2C5 clock root to SYSTEM_PLL1_DIV6 => 133MHz / 2 =  66MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C5), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C5), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable I2C5 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C5), 0x03);
    }
#endif

    return 0;
}
#endif

#if IMX_QSPI_INIT_ENABLED
/**
 * Initialize FLEXSPI clock.
 *
 * @return Execution status.
 */
static int imx_init_qspi_clock(void)
{
    /* Set QSPI input clock to 100MHz (SYSTEM_PLL1) */
    {
        /* Disable QSPI clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_FLEXSPI), 0x03);
        /* Set QSPI clock root to SYSTEM_PLL1_DIV2 => 800MHz / 8 =  100MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_QSPI), IMX_CCM_TARGET_ROOT_MUX_VALUE(7));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_QSPI), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable QSPI clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_FLEXSPI), 0x03);
    }

    return 0;
}
#endif

#if IMX_USDHC_INIT_ENABLED
/**
 * Initialize USDHC clock.
 *
 * @return Execution status.
 */
static int imx_init_usdhc_clock(imx_startup_data_t * startup_data)
{
#if 0
    /* Set USDHC1 input clock to 400MHz (SYSTEM_PLL1_DIV2) */
    {
        /* Disable USDHC1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USDHC1), 0x03);
        /* Set USDHC1 clock root to SYSTEM_PLL1_DIV2 => 800MHz / 2 =  400MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USDHC1), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_USDHC1), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable USDHC1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USDHC1), 0x03);
        /* Save clock source frequency for HWI table */
        startup_data->imx_usdhc_clk[0] = 400000000UL;
    }
#endif
    /* Set USDHC2 input clock to 400MHz (SYSTEM_PLL1_DIV2) */
    {
        /* Disable USDHC2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USDHC2), 0x03);
        /* Set USDHC2 clock root to SYSTEM_PLL1_DIV2 => 800MHz / 2 =  400MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USDHC2), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_USDHC2), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable USDHC2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USDHC2), 0x03);
        /* Save clock source frequency for HWI table */
        startup_data->imx_usdhc_clk[1] = 400000000UL;
    }
    /* Set USDHC3 input clock to 400MHz (SYSTEM_PLL1_DIV2) */
    {
        /* Disable USDHC3 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USDHC3), 0x03);
        /* Set USDHC3 clock root to SYSTEM_PLL1_DIV2 => 800MHz / 2 =  400MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USDHC3), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_USDHC3), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable USDHC3 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USDHC3), 0x03);
        /* Save clock source frequency for HWI table */
        startup_data->imx_usdhc_clk[2] = 400000000UL;
    }

    return 0;
}
#endif

#if IMX_NAND_INIT_ENABLED
/**
 * Initialize NAND clock.
 *
 * @return Execution status.
 */
static int imx_init_nand_clock(void)
{
    return 0;
}
#endif

#if IMX_AUDIO_INIT_ENABLED
/**
 * Initialize Audio clock.
 *
 * @return Execution status.
 */
static int imx_init_audio_clock(void)
{
    /* Enable AUDIOMIX power domain, ID = 0x05 */
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_AUDIOMIX, 0x01, 0x00);
    /* Disable clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_AUDIO), 0x03);
    /* Configure SAI3_CLK_ROOT to Audio PLL1 clock source and divide it down to 24.576 MHz */
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_SAI3), IMX_CCM_TARGET_ROOT_MUX_VALUE(1) | IMX_CCM_TARGET_ROOT_POST_PODF(4) | IMX_CCM_TARGET_ROOT_PRE_PODF(4));
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_SAI3), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    /* Configure Audio AHB to System PLL1 an divide by 2 thus get 400 MHz */
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_AUDIO_AHB), IMX_CCM_TARGET_ROOT_MUX_VALUE(2) | IMX_CCM_TARGET_ROOT_PRE_PODF(2));
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_AUDIO_AHB), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    /* Configure Audio AXI to System PLL1 (800 MHz) */
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_AUDIO_AXI), IMX_CCM_TARGET_ROOT_MUX_VALUE(2));
    out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_AUDIO_AXI), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    /* Enable clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_AUDIO), 0x03);

    return 0;
}
#endif

#if IMX_FLEXCAN_INIT_ENABLED
/**
 * Initialize FlexCAN clock.
 *
 * @return Execution status.
 */
static int imx_init_flexcan_clock(void)
{
    /* Set FlexCAN1 input clock to 80 MHz (SYSTEM_PLL1) */
    {
        /* Disable FlexCAN1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_CAN1), 0x03);
        /* Set FlexCAN1 clock root to SYSTEM_PLL1_DIV5 => 800 MHz / 5 / 2 =  80 MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_CAN1), IMX_CCM_TARGET_ROOT_MUX_VALUE(3));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_CAN1), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable FlexCAN1 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_CAN1), 0x03);
    }
#if IMX_FLEXCAN2_ENABLED
    /* Set FlexCAN2 input clock to 80 MHz (SYSTEM_PLL1) */
    {
        /* Disable FlexCAN2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_CAN2), 0x03);
        /* Set FlexCAN2 clock root to SYSTEM_PLL1_DIV5 => 800 MHz / 5 / 2 =  80 MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_CAN2), IMX_CCM_TARGET_ROOT_MUX_VALUE(3));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_CAN2), IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                             IMX_CCM_TARGET_ROOT_POST_PODF(2));
        /* Enable FlexCAN2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_CAN2), 0x03);
    }
#endif

    return 0;
}
#endif

#if IMX_PCIE_INIT_ENABLED
/**
 * Initialize PCI express clock.
 *
 * @return Execution status.
 */
static int imx_init_pcie_clock(void)
{
    /* Enable HSIOMIX power domain, ID = 0x00 */
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, 0x00, 0x01, 0x00);
    /* Enable PCIe power domain, ID = 0x01 */
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, 0x01, 0x01, 0x00);

    /* Disable PCIE_CTRL clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_PCIE), 0x03);
    /* Set PCIe PHy input clock to 100MHz (SYSTEM_PLL2_DIV10) */
    {
        /* Set PCIE_PHy root to SYSTEM_PLL2_DIV10 => 1000MHz / 10 = 100MHz, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_PCIE_PHY), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_PCIE_PHY), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    }
    /* Set PCIe AUX input clock to 10MHz (SYSTEM_PLL2_DIV20 / 5) */
    {
        /* Set PCIE_AUX root to SYSTEM_PLL2_DIV20 => 1000MHz / 20 / 5 = 10MHz, enable clock */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_PCIE_AUX), IMX_CCM_TARGET_ROOT_MUX_VALUE(2));
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_PCIE_AUX), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                                IMX_CCM_TARGET_ROOT_POST_PODF(5)));
    }
    /* Enable PCIE_CTRL clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_PCIE), 0x03);
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_HSIO), 0x03);

    return 0;
}
#endif

#if IMX_GPU_INIT_ENABLED
/**
 * Initialize GPU clock.
 *
 * @return Execution status.
 */
static int imx_init_gpu_clock(void)
{
    /* GPUMIX power off */
    /* Power down request to ADB */
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPU2D, 0x00, 0x00);
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPU3D, 0x00, 0x00);
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPUMIX, 0x00, 0x00);
    /* Disable GPU2D clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU2D), 0x00);
    /* Disable GPU3D clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU3D), 0x00);
    /* Disable GPU bus clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU), 0x00);

    if (soc_overdrive) {
    /* VDD_SOC=0.95V */
        /* Set GPU3D Core clock root to GPU_PLL_CLK, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU3D_CORE), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU3D_CORE), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU3D Core clock root to GPU_PLL_CLK, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU3D_SHADER), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU3D_SHADER), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU2D clock root to GPU_PLL_CLK, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU2D), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU2D), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU_AXI clock root to SYSTEM_PLL1 = 800MHz, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU_AXI), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU_AXI), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU_AHB clock root to SYSTEM_PLL1PLL = 800Mz, post div = 2, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU_AHB), (IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(2)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU_AHB), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    } else {
        /* Set GPU3D Core clock root to SYSTEM_PLL1 = 800MHz, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU3D_CORE), (IMX_CCM_TARGET_ROOT_MUX_VALUE(2) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU3D_CORE), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU3D Core clock root to SYSTEM_PLL1 = 800MHz, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU3D_SHADER), (IMX_CCM_TARGET_ROOT_MUX_VALUE(2) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU3D_SHADER), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU2D clock root to SYSTEM_PLL1 = 800MHz, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU2D), (IMX_CCM_TARGET_ROOT_MUX_VALUE(2) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU2D), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU_AXI clock root to SYSTEM_PLL3 = 600MHz, post div = 1, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU_AXI), (IMX_CCM_TARGET_ROOT_MUX_VALUE(3) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(1)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU_AXI), IMX_CCM_TARGET_ROOT_ENABLE_MASK);

        /* Set GPU_AHB clock root to SYSTEM_PLL3 = 600Mz, post div = 2, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GPU_AHB), (IMX_CCM_TARGET_ROOT_MUX_VALUE(3) |
                                                                           IMX_CCM_TARGET_ROOT_PRE_PODF(1) |
                                                                           IMX_CCM_TARGET_ROOT_POST_PODF(2)));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_GPU_AHB), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    }

    /* Enable GPU bus clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU), 0x02);
    /* Enable GPU2D clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU2D), 0x02);
    /* Enable GPU3D clock */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn(IMX_CCM_CCGR_GPU3D), 0x02);

    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPUMIX, 0x01, 0x00);
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPU2D, 0x01, 0x00);
    (void)imx_sec_firmware_psci(IMX_FSL_SIP_GPC, IMX_FSL_SIP_CONFIG_GPC_PM_DOMAIN, ATF_PU_GPU3D, 0x01, 0x00);

    return 0;
}
#endif

#if IMX_VPU_INIT_ENABLED
/**
 * Initialize VPU clock.
 *
 * @return Execution status.
 */
static int imx_init_vpu_clock(void)
{
    return 0;
}
#endif

#if IMX_ISI_CSI_INIT_ENABLED
/**
 * Initialize Imaging subsystem clocks.
 *
 * @return Execution status.
 */
static int imx_init_isi_csi_clock(void)
{
    return 0;
}
#endif

/**
 * Initialize peripheral clocks.
 *
 * @return Execution status.
 */
int imx_init_clocks(imx_startup_data_t * startup_data)
{
    do {
        imx_init_syspll();

#if IMX_I2C_INIT_ENABLED
        if (imx_init_i2c_clock() != 0) {
            break;
        }
        /* Configure I2C pads here */
        if (imx_init_i2c_pads() != 0) {
            break;
        }
        soc_overdrive = imx_get_soc_overdrive();
#endif
        if (imx_init_pll() != 0) {
            break;
        }
#if IMX_GPIO_INIT_ENABLED
        if (imx_init_gpio_clock() != 0) {
            break;
        }
#endif
#if IMX_GPT_INIT_ENABLED
        if (imx_init_gpt_clock() != 0) {
            break;
        }
#endif
#if IMX_ENET_INIT_ENABLED
        if (imx_init_enet_clock() != 0) {
            break;
        }
#endif
#if IMX_UART_INIT_ENABLED
        if (imx_init_uart_clock() != 0) {
             break;
        }
#endif
#if IMX_ISI_CSI_INIT_ENABLED
        /* The ISI and MIPI-CSI clocks needs to be enabled so the i2c devices owned be the CSI2
         * can be setup. */
        if (imx_init_isi_csi_clock() != 0) {
             break;
         }
#endif
#if IMX_DC_INIT_ENABLED
        if (imx_init_dc_clock() != 0) {
            break;
        }
#endif
#if IMX_ECSPI_INIT_ENABLED
        if (imx_init_ecspi_clock() != 0) {
            break;
        }
#endif
#if IMX_QSPI_INIT_ENABLED
        if (imx_init_qspi_clock() != 0) {
            break;
        }
#endif
#if IMX_USDHC_INIT_ENABLED
        if (imx_init_usdhc_clock(startup_data) != 0) {
            break;
        }
#endif
#if IMX_NAND_INIT_ENABLED
        if (nand_enable) {
            if (imx_init_nand_clock() != 0) {
                break;
            }
        }
#endif
#if IMX_USB_INIT_ENABLED
        if (imx_init_usb_clock() != 0) {
            break;
        }
#endif
#if IMX_AUDIO_INIT_ENABLED
        if (imx_init_audio_clock() != 0) {
            break;
        }
#endif
#if IMX_FLEXCAN_INIT_ENABLED
        if (imx_init_flexcan_clock() != 0) {
             break;
        }
#endif
#if IMX_PCIE_INIT_ENABLED
        if (imx_init_pcie_clock() != 0) {
             break;
        }
#endif
#if IMX_GPU_INIT_ENABLED
        if (imx_init_gpu_clock() != 0) {
             break;
        }
#endif
#if IMX_VPU_INIT_ENABLED
        if (imx_init_vpu_clock() != 0) {
             break;
        }
#endif
        /* Enable SDMA1 */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_SDMA1), 0x03);
        return 0;
    } while (0);
    return -1;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_init_clocks.c $ $Rev: 985114 $")
#endif
