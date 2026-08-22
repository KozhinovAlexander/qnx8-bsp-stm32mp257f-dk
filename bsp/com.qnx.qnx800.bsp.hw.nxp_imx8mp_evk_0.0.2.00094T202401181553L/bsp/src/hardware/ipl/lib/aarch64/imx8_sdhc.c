/*
 * Copyright (c) 2022-2023, BlackBerry Limited.
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

#include "ipl.h"
#include <mmc.h>
#include <sdmmc.h>
#include <sd.h>
#include <hw/inout.h>
#include <string.h>
#include "imx8_sdhc.h"

static uint32_t imx8_clkmax;
static uint32_t imx8_clock;
static uint32_t imx8_version;
static uint32_t imx8_flags;
static uint32_t imx8_chip_type;
static uint32_t imx8_chip_rev;
static uint32_t imx8_drv_type = 0;


static imx8_adma32_t adma32[ADMA_DESC_MAX];
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define TUNING_8BIT_BLK_SIZE    128
#define FRQ_100M                100000000

#ifdef IM8_MMC_REG_DUMP
static void reg_dump(paddr_t base)
{
    int i;
    kprintf("reg_dump\n");
    for ( i = 0; i <= 0xcc; i = i + 4) {
        if (!(i % 32)) {
            kprintf("\n0x%x  ", base + i);
        }
        kprintf("0x%x  ", in32(base + i));
    }
    kprintf("\n");
}
#endif /* IM8_MMC_REG_DUMP */

static int imx8_waitmask(const sdmmc_t *const sdmmc, const uint32_t reg, const uint32_t mask, const uint32_t val, const uint32_t usec)
{
    uint32_t cnt, iter;
    int sts = SDMMC_ERROR;

    const paddr_t base = sdmmc->hc->sdmmc_pbase;

    /* Fast poll for 1ms - 1us intervals */
    for (cnt = MIN(usec, 1000); cnt; cnt--) {
        if (((in32(base + reg)) & mask) == val) {
            sts = SDMMC_OK;
            break;
        }
        udelay(1);
    }

    if ((usec > 1000) && sts) {
        iter = usec / 1000;
        for (cnt = iter; cnt; cnt--) {
            if (((in32(base + reg)) & mask) == val) {
                sts = SDMMC_OK;
                break;
            }
            udelay(1000);
        }
    }

    if (sts != SDMMC_OK) {
        kprintf("Timeout to wait for mask:0x%x, reg:0x%x\n", mask, in32(base + reg));
    }
    return sts;
}

static void imx8_reset_tuning(const sdmmc_t *const sdmmc, const uint32_t flags)
{
    uint32_t mix_ctrl;
    const paddr_t base = sdmmc->hc->sdmmc_pbase;

    if (flags & IMX_USDHC_TUNING_MANUAL) {
        mix_ctrl = in32(base + IMX_USDHC_MIX_CTRL);
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK;
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK;
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
        out32(base + IMX_USDHC_MIX_CTRL, mix_ctrl);
        out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS, 0);
    }

    if (flags & IMX_USDHC_TUNING_STANDARD) {
        mix_ctrl = in32(base + IMX_USDHC_MIX_CTRL);
        if (mix_ctrl & IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK) {
            mix_ctrl &= ~IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
            out32(base + IMX_USDHC_MIX_CTRL, mix_ctrl);
        }
        uint32_t acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
        acmd12 &= ~IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK;
        acmd12 &= ~IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
        out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);

        const uint32_t mask = IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
        imx8_waitmask(sdmmc, IMX_USDHC_AUTOCMD12_ERR_STATUS, mask, 0, 50);

        /* Clear Buffer Read Ready interrupt for cmd19 */
        uint32_t intr_sts = in32(base + IMX_USDHC_INT_STATUS);
        intr_sts |= IMX_USDHC_INT_STATUS_BRR_MASK;
        out32(base + IMX_USDHC_INT_STATUS, intr_sts);
    }
}

static int imx8_reset(const sdmmc_t *const sdmmc, const uint32_t rst)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t sctl;

    sctl = in32(base + IMX_USDHC_SYS_CTRL);

    /* Wait up to 100 ms for reset to complete */
    out32(base + IMX_USDHC_SYS_CTRL, sctl | rst);
    return imx8_waitmask(sdmmc, IMX_USDHC_SYS_CTRL, rst, 0, 100000);

}

static void imx8_voltage_reset(const sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t sctl;

    sctl = in32(base + IMX_USDHC_SYS_CTRL);

    sctl &= ~IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);
    udelay(10000); /* Delay for card reset */
    sctl |= IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);

}

static int imx8_set_strobe_dll(const sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t strdll_ctrl, vspec;
    uint32_t lmask;
    int sts = SDMMC_ERROR;

    /* disable clock before seting sdll  */
    vspec = in32(base + IMX_USDHC_VEND_SPEC);
    vspec &= ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    out32(base + IMX_USDHC_VEND_SPEC, vspec);

    /* wait for SD clock gate off */
    sts = imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE,
                         IMX_USDHC_PRES_STATE_SDOFF_MASK,
                         IMX_USDHC_PRES_STATE_SDOFF_MASK,
                         100);
    if (sts != SDMMC_OK) {
        kprintf("SD clock is still not gate off in 100us!\n");
        return SDMMC_ERROR;
    }

    /* set reset bit */
    out32(base + IMX_USDHC_STROBE_DLL_CTRL, IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK);

    /* clr reset bit and other bits */
    out32(base + IMX_USDHC_STROBE_DLL_CTRL, 0);

    /* enable strobe delayed-line lock */
    strdll_ctrl = ( IMX_USDHC_STROBE_DLL_CTRL_ENABLE_MASK |
                    IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT(IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_DEFAULT_VALUE) |
                    IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET(IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_DEFAULT_VALUE));
    out32(base + IMX_USDHC_STROBE_DLL_CTRL, strdll_ctrl);

    lmask = (IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK);
    if ((imx8_waitmask(sdmmc, IMX_USDHC_STROBE_DLL_STATUS, lmask, lmask, 50)) != SDMMC_OK) {
        kprintf("sdll is not locked!\n");
        return SDMMC_ERROR;
    }

    /* Force SD clock active */
    vspec = in32(base + IMX_USDHC_VEND_SPEC);
    vspec |= IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    out32(base + IMX_USDHC_VEND_SPEC, vspec);

    return SDMMC_OK;
}

static int imx8_set_dll(const sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t dll_ctrl, vspec;
    uint32_t lmask;
    int sts = SDMMC_ERROR;

    /* disable clock before seting dll  */
    vspec = in32(base + IMX_USDHC_VEND_SPEC);
    vspec &= ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    out32(base + IMX_USDHC_VEND_SPEC, vspec);

    /* wait for SD clock gate off */
    sts = imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE,
                         IMX_USDHC_PRES_STATE_SDOFF_MASK,
                         IMX_USDHC_PRES_STATE_SDOFF_MASK,
                         100);
    if (sts != SDMMC_OK) {
        kprintf("SD clock is still not gate off in 100us!\n");
        return SDMMC_ERROR;
    }
    /* set reset bit */
    out32(base + IMX_USDHC_DLL_CTRL, IMX_USDHC_DLL_CTRL_RESET_MASK);

    /* clr reset bit and other bits */
    out32(base + IMX_USDHC_DLL_CTRL, 0);

    /* enable delayed-line lock */
    dll_ctrl = IMX_USDHC_DLL_CTRL_ENABLE_MASK;
    out32(base + IMX_USDHC_DLL_CTRL, dll_ctrl);

    lmask = (IMX_USDHC_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_DLL_STATUS_SLV_LOCK_MASK);
    if ((imx8_waitmask(sdmmc, IMX_USDHC_DLL_STATUS, lmask, lmask, 50)) != SDMMC_OK) {
        kprintf("dll is not locked in 50us!\n");
        return SDMMC_ERROR;
    }

    /* Force SD clock active */
    vspec = in32(base + IMX_USDHC_VEND_SPEC);
    vspec |= IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    out32(base + IMX_USDHC_VEND_SPEC, vspec);

    return SDMMC_OK;
}

/* sets the sdmmc clock frequency */
static void imx8_set_frq(sdmmc_t *const sdmmc, unsigned frq)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t div = 1, pre_div = 1, ddr_mode = 0;
    uint32_t sctl;

    /* Stop clock */
    sctl = in32(base + IMX_USDHC_SYS_CTRL);
    sctl &= ~(IMX_USDHC_SYS_CTRL_DTOCV_MASK | IMX_USDHC_SYS_CTRL_SDCLKFS_MASK);
    sctl |= IMX_USDHC_SYS_CTRL_DTOCV_MASK | IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);

    if ((sdmmc->timing == TIMING_DDR50) || (sdmmc->timing == TIMING_HS400) || (sdmmc->timing == TIMING_HS400ES)) {
        ddr_mode = 1;
        pre_div = 2;
    }

    if ((uint32_t)frq > imx8_clkmax) {
        frq = imx8_clkmax;
    }

    while ((imx8_clkmax / (pre_div * 16) > (uint32_t)frq) && (pre_div < 256)) {
        pre_div *= 2;
    }

    while ((imx8_clkmax / (pre_div * div) > (uint32_t)frq) && (div < 16)) {
        div++;
    }

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_0) {
        kprintf("desired clock: %d Hz ref clock: %d Hz actual clock: %d Hz, pre_div=%d, div=%d\n",
                frq,
                imx8_clkmax,
                imx8_clkmax / pre_div / div, pre_div, div);
    }
    pre_div >>= (1 + ddr_mode);
    div --;

    sctl = ((0xE << IMX_USDHC_SYS_CTRL_DTOCV_SHIFT) | (pre_div << IMX_USDHC_SYS_CTRL_SDCLKFS_SHIFT) |
            (div << IMX_USDHC_SYS_CTRL_DVS_SHIFT) | IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK | 0xF);

    out32(base + IMX_USDHC_SYS_CTRL, sctl);

    /* Wait for clock to stabilize */
    imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE,
                         IMX_USDHC_PRES_STATE_SDSTB_MASK,
                         IMX_USDHC_PRES_STATE_SDSTB_MASK,
                         IMX_USDHC_CLOCK_TIMEOUT);


    if ((sdmmc->timing == TIMING_HS200) && (frq > FRQ_100M)) {
        if ((imx8_set_dll(sdmmc)) != SDMMC_OK) {
            kprintf("failed to set dll!\n");
        }
    } else if (((sdmmc->timing == TIMING_HS400) || (sdmmc->timing == TIMING_HS400ES)) && (frq > FRQ_100M)) {
        if ((imx8_set_strobe_dll(sdmmc)) != SDMMC_OK) {
            kprintf("failed to set sdll\n");
        }
    } else {
        out32(base + IMX_USDHC_DLL_CTRL, 0);
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, 0);
    }

    imx8_clock = frq;
}

static int imx8_pwr(sdmmc_t *const sdmmc, const int vdd)
{
    const paddr_t    base = sdmmc->hc->sdmmc_pbase;
    const sdmmc_hc_t *const hc = sdmmc->hc;
    uint32_t   reg;

    if (vdd) {
        reg = in32(base + IMX_USDHC_VEND_SPEC) & ~IMX_USDHC_VEND_SPEC_VSELECT_MASK;
        switch (vdd) {
            case OCR_VDD_17_195:
                reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8);
                break;
            case OCR_VDD_29_30:
            case OCR_VDD_30_31:
            case OCR_VDD_32_33:
            case OCR_VDD_33_34:
                reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0);
                break;
            default:
                kprintf("unrecognized voltage level vdd: %d\n", vdd);
                return SDMMC_ERROR;
        }

        out32(base + IMX_USDHC_VEND_SPEC, reg);
        out32(base + IMX_USDHC_INT_SIGNAL_EN, IMX_USDHC_INT_SIGNAL_EN_DFLTS);
    } else {
        if (!(hc->flags & HC_FLAG_DEV_MMC)) {
            imx8_voltage_reset(sdmmc);
        }
        /* Reset core */
        if (imx8_reset(sdmmc, IMX_USDHC_SYS_CTRL_RSTA_MASK) != SDMMC_OK) {
            kprintf("Failed to reset controller\n");
            return (SDMMC_ERROR);
        }
        /* Reset both tuning methods */
        imx8_reset_tuning(sdmmc, (IMX_USDHC_TUNING_MANUAL | IMX_USDHC_TUNING_STANDARD));
        /* Make a clean environment */
        out32(base + IMX_USDHC_MIX_CTRL, 0);
        out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS, 0);
    }

    return SDMMC_OK;
}

/* sets the data bus width */
static void imx8_set_bus_width(sdmmc_t *sdmmc, const int width)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t hctl = in32(base + IMX_USDHC_PROT_CTRL) & ~(IMX_USDHC_PROT_CTRL_DTW_MASK);

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_0) {
        kprintf("Bus width %d\n", width);
    }

    switch (width) {
        case 8:
            hctl |= IMX_USDHC_PROT_CTRL_DTW(IMX_USDHC_PROT_CTRL_DTW_BV_8);
            break;
        case 4:
            hctl |= IMX_USDHC_PROT_CTRL_DTW(IMX_USDHC_PROT_CTRL_DTW_BV_4);
            break;
        case 1:
        default:
            break;
    }

    out32(base + IMX_USDHC_PROT_CTRL, hctl);
    sdmmc->bus_width = width;
}

static void imx8_set_bus_mode(sdmmc_t *const sdmmc, const int mode)
{
    return;
}

static void imx8_set_timing(sdmmc_t *sdmmc, const int timing)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t mix_ctl;
    static const char *const name[11] = { "INVALID", "LS", "HS", "HS DDR50", "SDR12", "SDR25", "SDR50", "SDR104", "HS200", "HS400", "HS400ES" };

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_0) {
        kprintf("Timing %s\n", name[timing]);
    }

    sdmmc->timing = timing;
    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);

    if (timing == TIMING_DDR50) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_HS400) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_HS400ES) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_LS) {
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
    } else {
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    }

    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);

    if (timing == TIMING_DDR50) {
        if (imx8_clock > DTR_MAX_HS52 ) {
            imx8_clock = DTR_MAX_HS52;      // max dtr for HS is 52MHz
        }
        imx8_set_frq(sdmmc, imx8_clock);
    }

    return;
}

static int imx8_signal_voltage(sdmmc_t *const sdmmc, const int signal_voltage)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t reg, dlsl;

    if (imx8_version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        return SDMMC_OK;
    }

    if ((signal_voltage == SIGNAL_VOLTAGE_3_3) || (signal_voltage == SIGNAL_VOLTAGE_3_0)) {
        reg = in32(base + IMX_USDHC_VEND_SPEC);
        reg &= ~IMX_USDHC_VEND_SPEC_VSELECT_MASK;
        reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0);
        out32(base + IMX_USDHC_VEND_SPEC, reg);

    } else if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
        reg = in32(base + IMX_USDHC_VEND_SPEC);
        reg &= ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;

        /* Stop sd clock */
        out32(base + IMX_USDHC_VEND_SPEC, reg);

        dlsl = IMX_USDHC_PRES_STATE_DLSL0_MASK |
                IMX_USDHC_PRES_STATE_DLSL1_MASK |
                IMX_USDHC_PRES_STATE_DLSL2_MASK |
                IMX_USDHC_PRES_STATE_DLSL3_MASK;

        if (sdmmc->card.type != eMMC_HC) {
            if (imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE, dlsl, 0, IMX_USDHC_CLOCK_TIMEOUT)) {
                kprintf("Failed to switch to 1.8V, can't stop SD Clock \n ");
                return SDMMC_ERROR;
            }
        }

        reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8);
        out32(base + IMX_USDHC_VEND_SPEC, reg);

        udelay(5000);

       /* Restore sd clock status */
        out32(base + IMX_USDHC_VEND_SPEC, reg | IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK);

        udelay(1000);

        reg = in32(base + IMX_USDHC_VEND_SPEC);

        /* Data lines should be high now */
        if (!(reg & IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8)) ||
            !(in32(base + IMX_USDHC_PRES_STATE) & dlsl)) {
            kprintf(" Failed to switch to 1.8V, DATA lines remain in low reg=0x%x\n ", reg);
            return SDMMC_ERROR;
        }
    } else {
        kprintf("unsupported voltage %d\n", signal_voltage);
        return SDMMC_ERROR;
    }

    return (SDMMC_OK);
}

/* clean up the SDMMC controller */
static int imx8_fini(sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;

    imx8_pwr(sdmmc, 0);
    out32(base + IMX_USDHC_INT_SIGNAL_EN, 0);
    out32(base + IMX_USDHC_INT_STATUS_EN, 0);
    out32(base + IMX_USDHC_INT_STATUS, in32(base + IMX_USDHC_INT_STATUS));
    out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, 0);

    return (SDMMC_OK);
}

/* initializes the SDMMC controller */
static int imx8_init_ctrl(sdmmc_t *sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t cap, reg, tuning_ctrl;
    const uint32_t imx8_tuning_std_step = IMX_USDHC_TUNING_CTRL_TUNING_STEP_DEFAULT;
    const uint32_t imx8_tuning_std_start_step = IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_DEFAULT;

    /* Cleanup some registers first */
    imx8_fini(sdmmc);

    imx8_version = in32(base + IMX_USDHC_HOST_CTRL_VER) & IMX_USDHC_HOST_CTRL_VER_SVN_MASK;
    cap = in32(base + IMX_USDHC_HOST_CTRL_CAP);

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
        kprintf("version:0x%x, cap=0x%x\n", imx8_version, cap);
    }

    imx8_clkmax = (sdmmc->hc->clock) ? sdmmc->hc->clock : IMX_USDHC_CLOCK_DEFAULT;

    if (imx8_version >= IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
            kprintf("using ADMA\n");
        }
        imx8_flags |= IMX_USDHC_USE_ADMA;
        sdmmc->caps |= HC_CAP_ACMD23;
    } else {
        if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
            kprintf("using SDMA\n");
        }
        imx8_flags |= IMX_USDHC_USE_SDMA;
    }

    sdmmc->caps |= HC_CAP_BUSY | HC_CAP_ACMD12 | HC_CAP_BW4 | HC_CAP_BW8;
    sdmmc->caps |= HC_CAP_200MA | HC_CAP_DRV_TYPE_B;

    if (cap & IMX_USDHC_HOST_CTRL_CAP_HSS_MASK) {
        sdmmc->caps |= HC_CAP_HS;
    }

    if (cap & IMX_USDHC_HOST_CTRL_CAP_SDR50_SUPPORT_MASK) {
        sdmmc->caps |= HC_CAP_SDR50 | HC_CAP_SDR25 | HC_CAP_SDR12;
    }

    if (cap & IMX_USDHC_HOST_CTRL_CAP_SDR104_SUPPORT_MASK) {
        sdmmc->caps |= HC_CAP_SDR104;
    }

    if (cap & IMX_USDHC_HOST_CTRL_CAP_DDR50_SUPPORT_MASK) {
        sdmmc->caps |= HC_CAP_DDR50;
    }

    /* TKT357500  - In enhance HS400 mode, usdhc can not report command timeout when emmc does not send response. */
    if ((imx8_chip_type == IMX_CHIP_TYPE_QUAD_MAX) && (imx8_chip_rev == IMX_CHIP_REV_A)) {
        sdmmc->caps |= HC_CAP_HS200 | HC_CAP_HS400;
    } else {
        sdmmc->caps |= HC_CAP_HS200 | HC_CAP_HS400 | HC_CAP_HS400ES;
    }

    if (cap & IMX_USDHC_HOST_CTRL_CAP_VS18_MASK) {
        sdmmc->ocr = OCR_VDD_17_195;
        sdmmc->caps |= HC_CAP_SV_1_8V;
    }

    if ((cap & IMX_USDHC_HOST_CTRL_CAP_VS30_MASK)) {
        sdmmc->ocr = OCR_VDD_30_31 | OCR_VDD_29_30;
        sdmmc->caps |= HC_CAP_SV_3_0V;
    }
    if ((cap & IMX_USDHC_HOST_CTRL_CAP_VS33_MASK)) {
        sdmmc->ocr = OCR_VDD_32_33 | OCR_VDD_33_34;
        sdmmc->caps |= HC_CAP_SV_3_3V;
    }

    // 3.3v signal voltage
    reg = in32(base + IMX_USDHC_VEND_SPEC);
    reg &= ~IMX_USDHC_VEND_SPEC_VSELECT_MASK;
    reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0);
    out32(base + IMX_USDHC_VEND_SPEC, reg);

    imx8_set_bus_width(sdmmc, 1);
    imx8_set_frq(sdmmc, 400000);
    imx8_set_timing(sdmmc, TIMING_LS);

    /* Configure watermark value */
    out32(base + IMX_USDHC_WTMK_LVL,
          (IMX_USDHC_WTMK_LVL_WR_WML_BV_MAX_VAL << IMX_USDHC_WTMK_LVL_WR_WML_SHIFT) |
          (IMX_USDHC_WTMK_LVL_RD_WML_BV_MAX_VAL << IMX_USDHC_WTMK_LVL_RD_WML_SHIFT));

    out32(base + IMX_USDHC_INT_SIGNAL_EN, IMX_USDHC_INT_SIGNAL_EN_DFLTS);

    if (imx8_flags & IMX_USDHC_TUNING_STANDARD) {
        /* Enable STD tuning */
        tuning_ctrl = in32(base + IMX_USDHC_TUNING_CTRL);
        tuning_ctrl &= ~(IMX_USDHC_TUNING_CTRL_TUNING_STEP_MASK | IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_MASK);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_TUNING_STEP(imx8_tuning_std_step);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_TUNING_START_TAP(imx8_tuning_std_start_step);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_DIS_CMD_CHK_FOR_STD_TUNING_MASK;
        out32(base + IMX_USDHC_TUNING_CTRL, tuning_ctrl);
    } else {
        /* Disable STD tuning */
        tuning_ctrl = in32(base + IMX_USDHC_TUNING_CTRL);
        tuning_ctrl &= ~IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        out32(base + IMX_USDHC_TUNING_CTRL, tuning_ctrl);
    }
    return SDMMC_OK;
}

/* setup DMA for data read */
static int imx8_setup_dma(sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    sdmmc_cmd_t *cmd = sdmmc->cmd;
    paddr_t     addr = (paddr_t)cmd->dbuf;
    int         idx, clen;
    int         tlen = (int)(cmd->bsize * cmd->bcnt);
    uint32_t    pctl;

    pctl = in32(base + IMX_USDHC_PROT_CTRL) & ~IMX_USDHC_PROT_CTRL_DMASEL_MASK;

    if (addr == 0) {
        out32(base + IMX_USDHC_PROT_CTRL, pctl);
        return SDMMC_ERROR;
    }

    if (imx8_flags & IMX_USDHC_USE_ADMA) {
        for (idx = 0; (tlen > 0) && (idx < ADMA_DESC_MAX); idx++) {
            clen             = (tlen < IMX_USDHC_ADMA2_MAX_XFER) ? tlen : IMX_USDHC_ADMA2_MAX_XFER;
            adma32[idx].attr = IMX_USDHC_ADMA2_VALID | IMX_USDHC_ADMA2_TRAN;
            adma32[idx].len  = (uint16_t)clen;
            adma32[idx].addr = (uint32_t)addr;

            addr            += (paddr_t)clen;
            tlen            -= clen;
        }

        if (idx > 0) {
            adma32[--idx].attr  |= IMX_USDHC_ADMA2_END;
        }

        cmd->bcnt -= (uint32_t)tlen / cmd->bsize;

        out32(base + IMX_USDHC_ADMA_SYS_ADDR, (uint32_t)((paddr_t)adma32));
        pctl |= IMX_USDHC_PROT_CTRL_DMASEL(IMX_USDHC_PROT_CTRL_DMASEL_BV_ADMA2);
    } else {
        /* SDMA */
        cmd->bcnt = (cmd->bcnt > SDMA_MAX_BCNT) ? SDMA_MAX_BCNT : cmd->bcnt;
        out32(base + IMX_USDHC_DS_ADDR, (uint32_t)addr);
        pctl |= IMX_USDHC_PROT_CTRL_DMASEL(IMX_USDHC_PROT_CTRL_DMASEL_BV_SDMA);
    }

    out32(base + IMX_USDHC_PROT_CTRL, pctl);

    return SDMMC_OK;
}

/* issues a command on the SDMMC bus */
static int imx8_send_cmd(sdmmc_t *const sdmmc)
{
    const paddr_t     base = sdmmc->hc->sdmmc_pbase;
    sdmmc_cmd_t *cmd = sdmmc->cmd;
    uint32_t    command, imask, pmask, intsts;
    uint32_t    mix_ctrl = 0;
    int         sts = SDMMC_OK;

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
        kprintf("CMD:%d, ARG:0x%x\n", cmd->cmd, cmd->arg);
    }

    /* Command inhibit (CMD) */
    pmask = IMX_USDHC_PRES_STATE_CIHB_MASK;


    command = (uint32_t)cmd->cmd << 24;

    if ((cmd->cmd == MMC_STOP_TRANSMISSION) || (cmd->cmd == SD_STOP_TRANSMISSION)) {
        command |= IMX_USDHC_CMD_XFR_TYP_CMDTYP(IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_ABORT);
    }

    imask = IMX_USDHC_INT_STATUS_EN_DFLTS;

    /* check if need data transfer */
    if ((cmd->flgsts & SCF_DATA_MSK)) {
        pmask |= IMX_USDHC_PRES_STATE_CDIHB_MASK;

        /* Data present */
        command |= IMX_USDHC_CMD_XFR_TYP_DPSEL_MASK;

        if (cmd->flgsts & SCF_DIR_IN) {
            mix_ctrl |=  IMX_USDHC_MIX_CTRL_DTDSEL_MASK;
        }

        imask |=  IMX_USDHC_INT_STATUS_EN_DTOESEN_MASK |
                  IMX_USDHC_INT_STATUS_EN_DCESEN_MASK |
                  IMX_USDHC_INT_STATUS_EN_DEBESEN_MASK |
                  IMX_USDHC_INT_STATUS_EN_TCSEN_MASK;

        if (imx8_setup_dma(sdmmc) == SDMMC_OK) {
            imask |= IMX_USDHC_INT_STATUS_EN_DMAESEN_MASK;
            mix_ctrl |= IMX_USDHC_MIX_CTRL_DMAEN_MASK;
        } else {
            /* use PIO */
            imask |= (cmd->flgsts & SCF_DIR_IN) ? IMX_USDHC_INT_STATUS_EN_BRRSEN_MASK : IMX_USDHC_INT_STATUS_EN_BWRSEN_MASK;
        }

        if (cmd->flgsts & SCF_MULTIBLK) {
            mix_ctrl |= IMX_USDHC_MIX_CTRL_MSBSEL_MASK | IMX_USDHC_MIX_CTRL_BCEN_MASK;
            if ((sdmmc->caps & HC_CAP_ACMD23) && (cmd->flgsts & SCF_SBC)) {
                /* Auto CMD23 need to use sdma address register to store the argument */
                out32(base + IMX_USDHC_DS_ADDR, cmd->bcnt);
                mix_ctrl |= IMX_USDHC_MIX_CTRL_AC23EN_MASK;
            } else if ((sdmmc->caps & HC_CAP_ACMD12)) {
                mix_ctrl |= IMX_USDHC_MIX_CTRL_AC12EN_MASK;
            } else {
                /* intended to be empty! */
            }
        }
        out32(base + IMX_USDHC_BLK_ATT, cmd->bsize | (cmd->bcnt << IMX_USDHC_BLK_ATT_BLKCNT_SHIFT));
    } else {
        /* Enable command complete intr */
        imask |= IMX_USDHC_INT_STATUS_EN_CCSEN_MASK;
    }

    if (cmd->flgsts & SCF_RSP_PRESENT) {
        if (cmd->flgsts & SCF_RSP_136) {
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_136);
        } else if (cmd->flgsts & SCF_RSP_BUSY) {
            /* Response with busy check */
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48B);
            /* Command 12 can be asserted even if data lines are busy */
            if ((cmd->cmd != MMC_STOP_TRANSMISSION) && (cmd->cmd != SD_STOP_TRANSMISSION)) {
                pmask |= IMX_USDHC_PRES_STATE_CDIHB_MASK;
            }
        } else {
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48);
        }

        /* Index check */
        if (cmd->flgsts & SCF_RSP_OPCODE) {
            command |= IMX_USDHC_CMD_XFR_TYP_CICEN_MASK;
        }

        /* CRC check */
        if (cmd->flgsts & SCF_RSP_CRC) {
            command |= IMX_USDHC_CMD_XFR_TYP_CCCEN_MASK;
        }
    }

    /* Wait till card is ready to handle next command */
    if (imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE, pmask, 0, IMX_USDHC_COMMAND_TIMEOUT) != SDMMC_OK) {
        kprintf("Timeout to wait for the ready state of next command at CMD%d\n", cmd->cmd);
        return SDMMC_ERROR;
    }

    /* IDLE state, need to initialize clock */
    if (cmd->cmd == MMC_GO_IDLE_STATE) {
        out32(base + IMX_USDHC_SYS_CTRL, (in32(base + IMX_USDHC_SYS_CTRL) | IMX_USDHC_SYS_CTRL_INITA_MASK));
        if (imx8_waitmask(sdmmc, IMX_USDHC_SYS_CTRL, IMX_USDHC_SYS_CTRL_INITA_MASK, 0,
                                         IMX_USDHC_COMMAND_TIMEOUT) != SDMMC_OK) {
            kprintf("Timeout IMX_USDHC_SYS_CTRL at CMD%d", cmd->cmd);
            return SDMMC_ERROR;
        }
    }

    mix_ctrl |= (in32(base + IMX_USDHC_MIX_CTRL) &
                 (IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK |
                  IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK |
                  IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK |
                  IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK |
                  IMX_USDHC_MIX_CTRL_HS400_MODE_MASK |
                  IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK |
                  IMX_USDHC_MIX_CTRL_DDR_EN_MASK));

    out32(base + IMX_USDHC_MIX_CTRL, mix_ctrl);

    /* Enable interrupts */
    out32(base + IMX_USDHC_INT_STATUS, IMX_USDHC_INT_STATUS_INTR_CLR_ALL);
    out32(base + IMX_USDHC_INT_STATUS_EN, imask);
    out32(base + IMX_USDHC_CMD_ARG, cmd->arg);
    out32(base + IMX_USDHC_CMD_XFR_TYP, command);

    imask = (IMX_USDHC_INT_STATUS_CC_MASK |
             IMX_USDHC_INT_STATUS_TC_MASK |
             IMX_USDHC_INT_STATUS_BRR_MASK |
             IMX_USDHC_INT_STATUS_BWR_MASK |
             IMX_USDHC_INT_STATUS_ERRI);

    /* wait for command finish */
    do {
        intsts = in32(base + IMX_USDHC_INT_STATUS);
    } while (!(intsts & imask));

    if (intsts & IMX_USDHC_INT_STATUS_ERRI) {
        /* filter the tunning command error print */
        if ((cmd->cmd != SD_SEND_TUNING_BLOCK) && (cmd->cmd != MMC_SEND_TUNING_BLOCK)) {
            kprintf("Command %x, send error(%x), CMD%d\n", command, intsts, cmd->cmd);
        } else {
            if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
                const uint32_t cmd_errs_mask = (IMX_USDHC_INT_STATUS_CCE_MASK |
                                            IMX_USDHC_INT_STATUS_CTOE_MASK |
                                            IMX_USDHC_INT_STATUS_CEBE_MASK |
                                            IMX_USDHC_INT_STATUS_CIE_MASK);
                if (intsts & cmd_errs_mask) {
                    kprintf("Command errors(%x), CMD%d\n", intsts, cmd->cmd);
                }
            }
        }

        if (imx8_reset(sdmmc, IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK) != SDMMC_OK) {
            kprintf("Failed to reset controller\n");
        }
        return SDMMC_ERROR;
    } else {
        if (intsts & IMX_USDHC_INT_STATUS_CC_MASK) {
            /* Errata ENGcm03648 */
            if (cmd->flgsts & SCF_RSP_BUSY) {
                const uint32_t timeout = 0x200000;
                sts = imx8_waitmask(sdmmc, IMX_USDHC_PRES_STATE,
                                    IMX_USDHC_PRES_STATE_DLSL0_MASK,
                                    IMX_USDHC_PRES_STATE_DLSL0_MASK,
                                    timeout);
                if (sts != SDMMC_OK) {
                    if (imx8_reset(sdmmc,
                            IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK) != SDMMC_OK) {
                        kprintf("Failed to reset controller\n");
                    }
                    kprintf("PRES_STATE_DLSL0 wait timeout for CMD%d\n", cmd->cmd);
                    return SDMMC_ERROR;
                }
            }

            if ((cmd->rsp != NULL) &&
                (cmd->flgsts & SCF_RSP_PRESENT)) {
                cmd->rsp[0] = in32(base + IMX_USDHC_CMD_RSP0);
                if (cmd->flgsts & SCF_RSP_136) {
                    cmd->rsp[1] = in32(base + IMX_USDHC_CMD_RSP1);
                    cmd->rsp[2] = in32(base + IMX_USDHC_CMD_RSP2);
                    cmd->rsp[3] = in32(base + IMX_USDHC_CMD_RSP3);
                    /*
                    * CRC is not included in the response register,
                    * we have to left shift 8 bit to match the 128 CID/CSD structure
                    */
                    cmd->rsp[3] = (cmd->rsp[3] << 8) | (cmd->rsp[2] >> 24);
                    cmd->rsp[2] = (cmd->rsp[2] << 8) | (cmd->rsp[1] >> 24);
                    cmd->rsp[1] = (cmd->rsp[1] << 8) | (cmd->rsp[0] >> 24);
                    cmd->rsp[0] = (cmd->rsp[0] << 8);
                }
            }
        }

        if ((command & IMX_USDHC_CMD_XFR_TYP_DPSEL_MASK)) {
            if ((mix_ctrl & IMX_USDHC_MIX_CTRL_DMAEN_MASK)) {
                sts = imx8_waitmask(sdmmc, IMX_USDHC_INT_STATUS,
                                    IMX_USDHC_INT_STATUS_TC_MASK,
                                    IMX_USDHC_INT_STATUS_TC_MASK,
                                    IMX_USDHC_COMMAND_TIMEOUT);
            } else {
                /* PIO */
                if ((cmd->flgsts & SCF_DIR_IN)) {
                    sts = imx8_waitmask(sdmmc, IMX_USDHC_INT_STATUS,
                                        IMX_USDHC_INT_STATUS_BRR_MASK,
                                        IMX_USDHC_INT_STATUS_BRR_MASK,
                                        IMX_USDHC_COMMAND_TIMEOUT);
                } else {
                    sts = imx8_waitmask(sdmmc, IMX_USDHC_INT_STATUS,
                                        IMX_USDHC_INT_STATUS_BWR_MASK,
                                        IMX_USDHC_INT_STATUS_BWR_MASK,
                                        IMX_USDHC_COMMAND_TIMEOUT);
                }

            }
        }
    }

    return sts;
}

/* Pre-tuning functionality. */
static void imx8_pre_tuning(const sdmmc_t *const sdmmc, const int tuning)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t mix_ctl;

    /* IC suggest to reset controller before every tuning command */
    if (imx8_reset(sdmmc, IMX_USDHC_SYS_CTRL_RSTA_MASK) != SDMMC_OK) {
        kprintf("Failed to reset controller\n");
    }
    /* Keep this delay comented until we confirm this is actualy not required */
    //udelay(1000);

    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl |= (IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK |
                IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK |
                IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK);
    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);
    out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS,
                (uint32_t)tuning << IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE_SHIFT);
}

/* Send tuning command. */
static int imx8_send_tune_cmd(sdmmc_t *sdmmc, const int op)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t buf[TUNING_8BIT_BLK_SIZE / sizeof(uint32_t)] = {0};
    sdmmc_cmd_t cmd;
    const uint8_t *tuning_block_pattern;
    int tlen;

    tlen = (sdmmc->bus_width == BUS_WIDTH_8) ? 128 : 64;
    CMD_CREATE(sdmmc, cmd, op, 0, NULL, (uint32_t)tlen, 1, NULL, SCF_CTYPE_ADTC | SCF_RSP_R1 | SCF_DIR_IN);
    if (imx8_send_cmd(sdmmc) != SDMMC_OK) {
        if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
            kprintf("Send tuning command error!\n");
        }
        sdmmc->cmd = NULL;
        return SDMMC_ERROR;
    }
    sdmmc->cmd = NULL;
    in32s(buf, (unsigned)tlen >> 2, base + IMX_USDHC_DATA_BUFF_ACC_PORT);

    tuning_block_pattern = (sdmmc->bus_width == BUS_WIDTH_8) ? sdio_tbp_8bit : sdio_tbp_4bit;
    /* Tuning block pattern error */
    if (memcmp((void *)buf, (void *)tuning_block_pattern, (size_t)tlen)) {
        if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
            kprintf("Tuning block pattern error!\n");
        }
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static void imx8_select_auto_tuning_mode(const sdmmc_t *const sdmmc)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    const uint32_t bus_width = (uint32_t)sdmmc->bus_width;
    uint32_t atune_bw, vspec2;

    switch (bus_width) {
    case BUS_WIDTH_8:
        atune_bw = IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN(1);
        break;
    case BUS_WIDTH_4:
        atune_bw = (IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN(0) | IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN(0));
        break;
    default:    /* 1BITBUS */
        atune_bw = IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN(1);
        break;
    }
    vspec2 = in32(base  + IMX_USDHC_VEND_SPEC2);
    vspec2 &= ~(IMX_USDHC_VEND_SPEC2_TUNING_8bit_EN_MASK | IMX_USDHC_VEND_SPEC2_TUNING_1bit_EN_MASK);
    vspec2 |= (atune_bw | IMX_USDHC_VEND_SPEC2_TUNING_CMD_EN(1));
    out32(base + IMX_USDHC_VEND_SPEC2, vspec2);
}

/* Standard Tuning */
static int imx8_std_tune(sdmmc_t *sdmmc, const int op)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    const sdmmc_hc_t *const hc = sdmmc->hc;
    uint32_t    acmd12, mix_ctl;
    uint32_t    buf[TUNING_8BIT_BLK_SIZE / sizeof(uint32_t)];
    int         sts = SDMMC_OK;
    int         tlen, idx;
    sdmmc_cmd_t cmd;

    tlen = (sdmmc->bus_width == BUS_WIDTH_8) ? 128 : 64;

    if (imx8_version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        return SDMMC_OK;
    }

    if (!(hc->flags & HC_FLAG_DEV_MMC) && (sdmmc->card.type != eMMC_HC)) {
        if (imx8_reset(sdmmc, IMX_USDHC_SYS_CTRL_RSTA_MASK) != SDMMC_OK) {
            kprintf("Failed to reset controller\n");
        }
    }

    /* It is recommended to use internal clock as the tuning clock. */
    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl |= IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK;
    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);

    /* Enable tuning execution */
    acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
    acmd12 |= IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
    out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);

    for (idx = 0; idx < IMX_USDHC_TUNING_RETRIES; idx++) {
        CMD_CREATE(sdmmc, cmd, op, 0, NULL, (uint32_t)tlen, 1, NULL, SCF_CTYPE_ADTC | SCF_RSP_R1 | SCF_DIR_IN);
        sts = imx8_send_cmd(sdmmc);
        sdmmc->cmd = NULL;
        if (sts != SDMMC_OK) {
            break;
        }

        /* Empty buffer */
        in32s(buf, (unsigned)tlen >> 2 , base + IMX_USDHC_DATA_BUFF_ACC_PORT);

        /* Check if tuning was successful or not. */
        acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
        if (!(acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK)) {
            if (acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK) {
                if (sdmmc->verbose > SDMMC_VERBOSE_LVL_0) {
                    kprintf("std tuning is passed within %d iteration(s)\n", idx);
                    kprintf("IMX_USDHC_CLK_TUNE_CTRL_STATUS:0x%x\n",
                            in32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS));
                }
                imx8_select_auto_tuning_mode(sdmmc);
                mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
                mix_ctl |= IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
                out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);
                udelay(1000); /* Make sure sdmmc finish handling of tuning data */
                break;
            }
        }
        udelay(1000);
    }

    if (sts || !(acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK)) {
        sts = SDMMC_OK;
        acmd12 &= ~(IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK |
                    IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK);
        out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);
        kprintf("failed tuning, using the fixed clock\n");
    }

    return sts;
}

/* manual tuning */
static int imx8_man_tune(sdmmc_t *const sdmmc, const int op)
{
    const paddr_t base = sdmmc->hc->sdmmc_pbase;
    uint32_t    mix_ctl;
    int         min, max;
    int         sts = SDMMC_ERROR;
    int         pass_steps = 0;

    if (imx8_version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        return (SDMMC_OK);
    }

    /* Minimum good value */
    min = IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MIN;
    while (min < IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MAX) {
        imx8_pre_tuning(sdmmc, min);
        if (SDMMC_OK == imx8_send_tune_cmd(sdmmc, op)) {
            /* we noticed, in our experiment of imx8qm mek board, some singular good values
             * are extremmely marginal and unstable. minimum good value is defined only if we see
             * IMX_USDHC_MANUAL_TUNING_ESS consecutive successes. This will increase the confidence
             * of finding a stable minimum vlaue which can pass tuning test.
             */
            if (pass_steps > IMX_USDHC_MANUAL_TUNING_ESS) {
                min -= pass_steps;
                if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
                    kprintf("Found the minimum good value:%d\n", min);
                }
                break;
            }
            pass_steps++;
        } else {
            pass_steps = 0;
        }

        min += IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    }

    /* Maximum not-good value */
    max = min + pass_steps + IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    while (max < IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MAX) {
        imx8_pre_tuning(sdmmc, max);
        if (SDMMC_OK != imx8_send_tune_cmd(sdmmc, op)) {
            if (sdmmc->verbose > SDMMC_VERBOSE_LVL_2) {
                kprintf("Found the maximum not-good value:%d\n", max);
            }
            max -= IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
            break;
        }
        max += IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    }

    imx8_pre_tuning(sdmmc, (min + max) / 2);
    sts = imx8_send_tune_cmd(sdmmc, op);

    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl &= ~IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK;

    /* Use the fixed clock if failed */
    if (sts != SDMMC_OK) {
        sts = SDMMC_OK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        kprintf("failed tuning, using the fixed clock(%d)\n", (min + max) / 2);
    } else {
        if (sdmmc->verbose > SDMMC_VERBOSE_LVL_0) {
            kprintf("manual tuning is passed with DLY_CELL_SET_PRE:%d\n", (min + max) / 2);
        }
    }

    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);

    return sts;
}

static int imx8_tune(sdmmc_t *const sdmmc, const int op)
{
    int sts = SDMMC_ERROR;

    if (imx8_flags & IMX_USDHC_TUNING_STANDARD) {
        sts = imx8_std_tune(sdmmc, op);
    } else {
        sts = imx8_man_tune(sdmmc, op);
    }

    return sts;
}

static int imx8_driver_strength(sdmmc_t *const sdmmc, const int timing, const int type)
{
    /* Override the drv_type for high speed protocols */
    if (((timing == TIMING_HS200) || (timing == TIMING_HS400) || (timing == TIMING_HS400ES)) && imx8_drv_type) {
        return imx8_drv_type;
    } else {
        return type;
    }
}

void imx8_init_hc(sdmmc_t *sdmmc, const paddr_t base, const uint32_t clock, const int verbose,
                            const sdmmc_ext_t *const ext)
{
    uint32_t tflags = 0;
    /*
    *--------------------------------------
    * IMX8 Host Controller
    *---------------------------------------
    */
    static sdmmc_hc_t imx8_hc = {
    .set_clk = imx8_set_frq,
    .set_bus_width = imx8_set_bus_width,
    .set_bus_mode = imx8_set_bus_mode,
    .set_timing = imx8_set_timing,
    .send_cmd = imx8_send_cmd,
    .dinit_hc = imx8_fini,
    .signal_voltage = imx8_signal_voltage,
    .tuning = imx8_tune,
    .power = imx8_pwr,
    .drv_type = NULL,
    .driver_strength = NULL,
    };

    sdmmc->hc = &imx8_hc;
    sdmmc->verbose = verbose;
    sdmmc->sdmmc.alloc_dmabuf = NULL;

    imx8_hc.sdmmc_pbase = base;
    imx8_hc.clock = clock;

    imx8_chip_type = ext->chip_type;
    imx8_chip_rev  = ext->chip_rev;

    if (ext->dev_flags & HC_FLAG_DEV_SD) {
        sdmmc->hc->flags |= HC_FLAG_DEV_SD;
    } else {
        sdmmc->hc->flags |= HC_FLAG_DEV_MMC;
        /* driver_strength function is only used for eMMC chips to overrride
         * the default value set in mmc codes.
         */
        imx8_hc.driver_strength = imx8_driver_strength;
        if ((ext->drv_type & DRV_TYPE_MSK)) {
            imx8_drv_type = (ext->drv_type & DRV_TYPE_MSK);
        }
    }

    tflags = ext->tuning_flags & IMX_USDHC_TUNING_MASK;

    if (tflags == IMX_USDHC_TUNING_MASK) {
        kprintf("Error choosing tuning flags:0x%x\n", tflags);
    } else {
        imx8_flags = tflags;
        imx8_init_ctrl(sdmmc);
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/imx8_sdhc.c $ $Rev: 979772 $")
#endif
