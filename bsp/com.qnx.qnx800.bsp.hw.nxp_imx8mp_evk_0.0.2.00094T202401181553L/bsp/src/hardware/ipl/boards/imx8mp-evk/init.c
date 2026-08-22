/*
  * Copyright (c) 2022-2023, BlackBerry Limited. All rights reserved.
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
  */

#include <stdint.h>
#include "ipl.h"
#include <sys/srcversion.h>
#include <hw/inout.h>
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <soc/nxp/imx8/mp/mx8mp_iomux.h>
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_ccm_analog.h>
#include <soc/nxp/imx8/common/imx_aipstz.h>
#include <soc/nxp/imx8/common/imx_scntr.h>
#include <soc/nxp/imx8/mp/imx_src.h>
#include <soc/nxp/imx8/mp/imx_gpc.h>
#include <soc/nxp/imx8/mp/imx_iomuxc_gpr.h>
#include "board.h"
#include "imx_ipl.h"
#include <soc/nxp/imx8/common/imx_smc_call.h>
#include "imx_i2c_drv.h"

#if defined(IMX_SPL_BOOT)
#if IMX_PMIC_OVERDRIVE_ENABLED
static void imx_init_pmic_i2c(imx_i2c_dev_t * dev) {

    /* I2C1 pinmux */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SDA), IMX_MUX_CTL_SION | IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SDA), IMX_PAD_CTL_PUE_PULL_UP | IMX_PAD_CTL_PE_PULL_ENABLED | IMX_PAD_CTL_SRE_FAST | IMX_PAD_CTL_HYS_SCHMITT | IMX_PAD_CTL_DSE_6X);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_I2C1_SDA_IN_SELECT_INPUT), 2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SCL), IMX_MUX_CTL_SION | IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SCL), IMX_PAD_CTL_PUE_PULL_UP | IMX_PAD_CTL_PE_PULL_ENABLED | IMX_PAD_CTL_SRE_FAST | IMX_PAD_CTL_HYS_SCHMITT | IMX_PAD_CTL_DSE_6X);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_I2C1_SCL_IN_SELECT_INPUT), 2);

    /* Set I2C1 input clock to 24MHz */
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_I2C1), IMX_CCM_TARGET_ROOT_MUX_VALUE(0));
    out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_I2C1), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    /* Enable I2C1 clock root */
    out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_I2C1), 0x03);

    /* I2C1 base */
    dev->base = 0x30A20000;
    dev->div = 0x10;
    /* PCA9450 slave address */
    dev->slave = 0x25;

    init_i2c_bus(dev);
}

void imx_init_pmic(void) {
    imx_i2c_dev_t dev;
    unsigned char value;

    imx_init_pmic_i2c(&dev);
    /* Write BUCK123_DVS. Set BUCK1 and BUCK2 to 0.95V.
     * BUCK3 is in dual phase with BUCK1 thus no need to set it. */
    value = (1 << 7) | (1 << 5) | (3 << 3) | (3);
    if (i2c_write(&dev, 0x0C, &value) != 0) {
         kprintf("Failed to set PMIC register 0x0C\n");
    }
}
#endif
/**
 * Initialize system counter.
 */
void imx_init_system_counter(void)
{
    uint32_t val;

    val = in32(IMX_SCNTR_BASE + IMX_SCNTR_CR_OFF);
    val &= ~(IMX_SCNTR_CR_FREQ0 | IMX_SCNTR_CR_FREQ1);
    val |= (IMX_SCNTR_CR_FREQ0 | IMX_SCNTR_CR_ENABLE | IMX_SCNTR_CR_HDBG);
    out32(IMX_SCNTR_BASE + IMX_SCNTR_CR_OFF, val);
}

/**
 * Enable clock gate and configure clock path for devices used by IPL.
 */
void imx_init_clocks(void)
{
    uint32_t val_cfg0, val;

    /* Initialize ARM_A53 input clock to 24MHz */
    {
        /* Set ARM_A53 clock root to 24 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ARM_A53), (
                IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                IMX_CCM_TARGET_ROOT_MUX_VALUE(0) |          /* 24 MHz source */
                IMX_CCM_TARGET_ROOT_POST_PODF(1)));         /* PODF div by 1 */
    }
    /* ********************************* ARM PLL = 1200/1600MHz ********************************* */
    {
        /* Bypass the ARM PLL clock and set lock to PLL output lock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
#if IMX_PMIC_OVERDRIVE_ENABLED
        /* Set the ARM PLL value: MainDiv=200, PreDiv=3, PostDiv=0 -> 1600 MHz */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0, (IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_MAIN_DIV(200) |
                                                                      IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_PRE_DIV(3) |
                                                                      IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_POST_DIV(0)));
#else
        /* Set the ARM PLL value: MainDiv=200, PreDiv=2, PostDiv=1 -> 1200 MHz */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0, (IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_MAIN_DIV(200) |
                                                                      IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_PRE_DIV(2) |
                                                                      IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_POST_DIV(1)));
#endif
        delay(100);
        /* Disable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL, (val_cfg0 |
                (IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Wait for ARM_PLL lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL) & IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_LOCK_MASK) == 0)
        {
        }
        /* Clear bypass the ARM PLL clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL, val_cfg0);
        /* PLL output clock gating enable */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_CLKE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL, val_cfg0);
    }
    /* Configure ARM_A53 input clock to 1200/1600MHz (ARM PLL) */
    {
        /* Bypass CCM A53 ROOT, Switch to ARM PLL->MUX->CPU */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_CORE_SEL_CFG), (
                IMX_CCM_TARGET_ROOT_MUX_VALUE(1) |
                IMX_CCM_TARGET_ROOT_POST_PODF(1)));         /* PODF div by 1 */
    }
    {
        /* Enable system counter clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_SCTR), 0x03);
    }
    /* ********************************* SYSTEM_PLL1 = 800MHz ********************************* */
    {
        /* Bypass the SYSTEM PLL1 clock and set lock to PLL output lock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Set the SYSTEM PLL1 value: MainDiv=400, PreDiv=2, PostDiv=3 */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0, (IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_MAIN_DIV(400) |
                                                                       IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_PRE_DIV(3) |
                                                                       IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_POST_DIV(2)));
        delay(100);
        /* Disable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL, (val_cfg0 |
                (IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Wait for SYSTEM_PLL1 lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL) & IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_LOCK_MASK) == 0)
        {
        }
        /* Clear bypass the SYSTEM PLL1 clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL, val_cfg0);
        /* PLL output clock gating enable */
        val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL);
        val |= (IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_CLKE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV2_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV3_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV4_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV5_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV6_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV8_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV10_CLKE_OVERRIDE_MASK|
                IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV20_CLKE_OVERRIDE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL, val);
    }
    /* ********************************* SYSTEM_PLL2 = 1000MHz ********************************* */
    {
        /* Bypass the SYSTEM PLL2 clock and set lock to PLL output lock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Set the SYSTEM PLL2 value: MainDiv=250, PreDiv=3, PostDiv=1 */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_FDIV_CTL0, (IMX_CCM_ANALOG_SYS_PLL2_FDIV_CTL0_PLL_MAIN_DIV(250) |
                                                                       IMX_CCM_ANALOG_SYS_PLL2_FDIV_CTL0_PLL_PRE_DIV(3) |
                                                                       IMX_CCM_ANALOG_SYS_PLL2_FDIV_CTL0_PLL_POST_DIV(1)));
        delay(100);
        /* Disable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL, (val_cfg0 |
                (IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Wait for SYSTEM_PLL2 lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL) & IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_LOCK_MASK) == 0)
        {
        }
        /* Clear bypass the SYSTEM PLL2 clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL, val_cfg0);
        val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL);
        val |= (IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_CLKE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV2_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV3_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV4_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV5_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV6_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV8_CLKE_OVERRIDE_MASK |
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV10_CLKE_OVERRIDE_MASK|
                IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL_PLL_DIV20_CLKE_OVERRIDE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL2_GEN_CTRL, val);
    }
    /* GIC clock configuration */
    {
#if IMX_PMIC_OVERDRIVE_ENABLED
        /* SYSTEM_PLL2_DIV2 -> 500 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GIC), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(5)));
#else
        /* SYSTEM_PLL1_CLK /2 -> 400 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_GIC), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(4) |
                                                                       IMX_CCM_TARGET_ROOT_PRE_PODF(2)));
#endif
    }
    /* ********************************* SYSTEM_PLL3 = 600MHz ********************************* */
    {
        /* Bypass the SYSTEM_PLL3 clock and set lock to PLL output lock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_BYPASS_MASK | IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_LOCK_SEL_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Set the SYSTEM_PLL3 value: MainDiv=300, PreDiv=3, PostDiv=2 */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_FDIV_CTL0, (IMX_CCM_ANALOG_SYS_PLL3_FDIV_CTL0_PLL_MAIN_DIV(300) |
                                                                       IMX_CCM_ANALOG_SYS_PLL3_FDIV_CTL0_PLL_PRE_DIV(3) |
                                                                       IMX_CCM_ANALOG_SYS_PLL3_FDIV_CTL0_PLL_POST_DIV(2)));
        delay(100);
        /* Disable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL, (val_cfg0 |
                (IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_RST_OVERRIDE_MASK)));
        /* Wait for PLL lock */
        while ((in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL) & IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_LOCK_MASK) == 0)
        {
        }
        /* Clear bypass the SYSTEM_PLL3 clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL, val_cfg0);
        /* PLL output clock clock gating enable */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL);
        val_cfg0 |= (IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_CLKE_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL, val_cfg0);
    }
    {
#if IMX_PMIC_OVERDRIVE_ENABLED
        /* PLL2 1000 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_NOC), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(3)));
        /* PLL1 800 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_NOC_IO), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
#else
        /* PLL1 800 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_NOC), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
        /* PLL3 600 MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_NOC_IO), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                       IMX_CCM_TARGET_ROOT_MUX_VALUE(2)));
#endif
    }
    /* ********************************* DRAM PLL = 1000MHz ********************************* */
    {
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_DRAM1), 0x03);
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_ALT), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(1)));
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_DRAM1), 0x03);
    }
    {
        /* Reset DDR controller */
        out32(IMX_SRC_BASE + 0x1000, 0x8F00001F);
        out32(IMX_SRC_BASE + 0x1000, 0x8F00000F);
        delay(100);

        /* Change the clock source of dram_apb_clk_root: SYSTEM_PLL1 / 4 = 200MHz */
        out32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_DRAM_APB), (IMX_CCM_TARGET_ROOT_ENABLE_MASK |
                                                                            IMX_CCM_TARGET_ROOT_MUX_VALUE(4) |
                                                                            IMX_CCM_TARGET_ROOT_PRE_PODF(4)));

        /* Disable ISO */
        out32(IMX_GPC_BASE + 0x00EC, 0x0000FFFF);       /* PGC_CPU_MAPPING */
        val = (in32(IMX_GPC_BASE + 0xF8) | (0x01 << 5));
        out32(IMX_GPC_BASE + 0xF8 , val);               /* PU_PGC_SW_PUP_REQ */
    }
    {
        out32(IMX_GPC_BASE + 0xEC, in32(IMX_GPC_BASE + 0xEC) | (1 << 7));
        out32(IMX_GPC_BASE + 0xF8, in32(IMX_GPC_BASE + 0xF8) | (1 << 5));   /* Software power up trigger for DDR1 */
        /* Enable DDR domain */
        out32(IMX_SRC_BASE + 0x1004, 0x8F000000);

        /* Bypass the DRAM PLL1,2 clock */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL_PLL_BYPASS_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL, val_cfg0);
        /* Enable reset */
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL, (val_cfg0 &
                ~(IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL_PLL_RST_MASK)));

        /* Configure DRAM PLL to 1000MHz */
        val = IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL0_PLL_MAIN_DIV(250) |
              IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL0_PLL_PRE_DIV(3) |
              IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL0_PLL_POST_DIV(1);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL0, val);
        val = IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL1_PLL_DSM(0);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL1, val);
        delay(100);

        /* Disable reset */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL);
        val_cfg0 |= IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL_PLL_RST_MASK;
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL, val_cfg0);
        /* Wait for lock */
        while(!(in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL) & IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL_PLL_LOCK_MASK));
        /* Disable PLL bypass */
        val_cfg0 = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL);
        val_cfg0 &= ~(IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL_PLL_BYPASS_MASK);
        out32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL, val_cfg0);
    }
    /* Set USDHC2 input clock to 400MHz (SYSTEM_PLL1) */
    {
        /* Disable USDHC2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_USDHC2), 0x03);
        /* Set USDHC2 clock root to SYSTEM_PLL1_DIV2 => 800MHz / 2 =  400MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_USDHC2), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_USDHC2), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable USDHC2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_USDHC2), 0x03);
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
    }
#if IMX_QSPI_BOOT
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
#endif
#if (IMX_CM7_CORE_BOOT == 1)
    /* Set Cortex-M7 clock root to 200MHz (SYSTEM_PLL2_DIV5) */
    {
        /* Set Cortex-M7 clock root to SYSTEM_PLL2_DIV5 => 200 MHz, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_ARM_M7), IMX_CCM_TARGET_ROOT_MUX_VALUE(1));
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_ARM_M7), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
    }
    /* Release reset, enable TCM */
    out32(IMX_SRC_BASE + IMX_SRC_M7RCR, 0xAA);
#endif
}

#ifdef IMX_QSPI_BOOT
/**
 * Initialize FlexSPI electric properties of PINs for NOR device.
 */
static void imx_init_fspi(void)
{
    /* QSPIA_SCLK */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_ALE), IMX_MUX_CTL_MUX_MODE_ALT1);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_ALE), (IMX_PAD_CTL_PE_PULL_ENABLED |
                                                                                            IMX_PAD_CTL_PUE_PULL_UP));
    /* QSPIA_SS0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE0_B), IMX_MUX_CTL_MUX_MODE_ALT1);
    /* QSPIA_DAT0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00), IMX_MUX_CTL_MUX_MODE_ALT1);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA00), (IMX_PAD_CTL_PE_PULL_ENABLED |
                                                                                               IMX_PAD_CTL_PUE_PULL_UP));
    /* QSPIA_DAT1 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01), IMX_MUX_CTL_MUX_MODE_ALT1);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA01), (IMX_PAD_CTL_PE_PULL_ENABLED |
                                                                                               IMX_PAD_CTL_PUE_PULL_UP));
    /* QSPIA_DAT2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02), IMX_MUX_CTL_MUX_MODE_ALT1);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA02), (IMX_PAD_CTL_PE_PULL_ENABLED |
                                                                                               IMX_PAD_CTL_PUE_PULL_UP));
    /* QSPIA_DAT3 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03), IMX_MUX_CTL_MUX_MODE_ALT1);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA03), (IMX_PAD_CTL_PE_PULL_ENABLED |
                                                                                               IMX_PAD_CTL_PUE_PULL_UP));
}
#endif
#endif /* IMX_SPL_BOOT */

/**
 * Initialize serial console.
 */
void imx_init_console(void)
{
    /* Initialize UART2 input clock to 24MHz */
    {
        /* Disable UART2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_CLR(IMX_CCM_CCGR_UART2), 0x03);
        /* Set UART2 clock root to 24 MHz oscillator, clk slice 94, 24M_REF_CLK => mux 0, enable clock */
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_UART2), 0x00);
        out32(IMX_CCM_BASE +  IMX_CCM_TARGET_ROOTn_SET(IMX_CCM_TARGET_UART2), IMX_CCM_TARGET_ROOT_ENABLE_MASK);
        /* Enable UART2 clock root */
        out32(IMX_CCM_BASE + IMX_CCM_CCGRn_SET(IMX_CCM_CCGR_UART2), 0x03);
    }
    /* Configure UART2 pads */
    {
        /* UART2_RX: mux 0*/
        out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_RXD),
                IMX_MUX_CTL_MUX_MODE_ALT0);
        /* UART2_RX: input */
        out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_UART2_UART_RXD_MUX_SELECT_INPUT), 6);
        /* UART2_TX: mux 0 */
        out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_TXD),
                IMX_MUX_CTL_MUX_MODE_ALT0);
    }
    /*
     * Initialize serial interface,
     * 115200bps, 24MHz input clock */
    imx_init_uart(IMX_CONSOLE_UART_BASE, IMX_CONSOLE_UART_BAUD_RATE, 24000000UL);
}

#if defined(IMX_SPL_BOOT)
/**
 * Initialize uSDHC3 electric properties of PINs for eMMC device.
 */
static void imx_init_usdhc3(void)
{
    /* USDHC3_CLK: mux 2*/
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WE_B), (IMX_MUX_CTL_MUX_MODE_ALT2 |
                                                                                             IMX_MUX_CTL_SION));
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WE_B), (IMX_PAD_CTL_SRE_FAST |
                                                                                             IMX_PAD_CTL_DSE_2X));
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT), 1);

    /* USDHC3_CMD: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WP_B), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_CMD_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA0: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT0_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA1: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA05), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT1_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA2: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA06), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT2_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA3: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA07), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT3_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA4: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_RE_B), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_RE_B), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT4_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA5: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE2_B), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE2_B), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT5_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA6: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE3_B), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE3_B), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT6_IN_SELECT_INPUT), 1);
    /* USDHC3_DATA7: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CLE), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CLE), IMX8MP_PAD_SETTINGS_USDHC);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_DAT7_IN_SELECT_INPUT), 1);
    /* USDHC3_STROBE: mux 2 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B), IMX_MUX_CTL_MUX_MODE_ALT2);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE1_B), (IMX_PAD_CTL_SRE_FAST |
                                                                                              IMX_PAD_CTL_DSE_2X));
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_INPUTx(IMX_IOMUXC_USDHC3_STROBE_SELECT_INPUT), 1);
}

/**
 * Initialize uSDHC2 electric properties of PINs for SD card.
 */
static void imx_init_usdhc2(void)
{
    /* USDHC2_CLK: mux 0*/
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CLK), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_CMD: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CMD), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_DATA0: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA0), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_DATA1: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA1), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_DATA2: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA2), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_DATA3: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA3), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_RESET_B: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_RESET_B), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_RESET_B), IMX8MP_PAD_SETTINGS_USDHC);
    /* USDHC2_CD_B: mux 0 */
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_MUX_CTL_PADx(IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CD_B), IMX_MUX_CTL_MUX_MODE_ALT0);
    out32(IMX_IOMUXC_BASE + IMX_IOMUXC_SW_PAD_CTL_PADx(IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CD_B), IMX8MP_PAD_SETTINGS_USDHC);
}

/**
 * Initialize SD2 (SD card), SD3 (eMMC) PINs routing and electric PIN properties.
 */
void imx_init_pinmux(void)
{
#ifdef IMX_QSPI_BOOT
    imx_init_fspi();
#endif
    imx_init_usdhc2();
    imx_init_usdhc3();
}
#endif /* IMX_SPL_BOOT */

#if (IMX_CM7_CORE_BOOT == 1)
/**
 * Start Cortex-M7 core.
 */
void imx_start_cortex_m7_core(void)
{
    /* Release reset, enable TCM */
    imx_smc_status_t status;

    status = imx_sec_firmware_psci(IMX_FSL_SIP_SRC, IMX_FSL_SIP_SRC_M4_START, 0x00, 0x00, 0x00);
    if (status != IMX_PSCI_SUCCESS) {
        kprintf("Cortex-M7 core start failed!\n");
    }
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/init.c $ $Rev: 980064 $")
#endif
