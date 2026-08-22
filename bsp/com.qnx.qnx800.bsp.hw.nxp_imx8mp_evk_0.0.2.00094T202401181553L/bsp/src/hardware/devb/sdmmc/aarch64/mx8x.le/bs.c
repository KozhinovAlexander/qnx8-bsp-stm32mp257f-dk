/*
 * Copyright (c) 2013, 2023, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, 2019-2021, NXP.
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
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <unistd.h>


#include <soc/nxp/imx8/common/imx_gpio.h>
#include <soc/nxp/imx8/common/imx_usdhc.h>

#include "imx8_hc.h"
#include "bs.h"

/**
 * Board specific interface
 *
 * @file       bs.c
 * @addtogroup sdmmc_bs
 * @{
 */

/**
 * Update write protect status
 * @param hc Host controller handle
 * @param cstate Card detect state
 */
static void imx_update_wp_status(sdio_hc_t *const hc, int *cstate)
{
    const imx_sdhcx_hc_t    *sdhc;
    imx_ext_t               *ext;
    uint32_t                status;
    uintptr_t               base;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    ext = hc->bs_hdl;

    /* Write protect using GPIO */
    if (ext->wp_pbase) {
        if (in32(ext->wp_base + IMX_GPIO_PSR) & (1 << ext->wp_pin)) {
            *cstate |= CD_WP;
        } else {
            *cstate &= ~CD_WP;
        }
    } else {
        /* Check write protect using USDHC_PRES_STATE register */
        status = in32(base + IMX_USDHC_PRES_STATE);

        /* When WPSPL bit is 0 write protect is enabled */
        if (!(status & IMX_USDHC_PRES_STATE_WPSPL_MASK)) {
            *cstate |= CD_WP;
        } else {
            *cstate &= ~CD_WP;
        }
    }
}

/**
 * Handle CD/WP if no card detection, or through uSDHC interrupt
 * @param hc Host controller handle
 *
 * @return Card detect state
 */
static int imx_cd(sdio_hc_t *const hc)
{
    const imx_sdhcx_hc_t    *sdhc;
    const imx_ext_t         *ext;
    uint32_t                status;
    uintptr_t               base;
    int                     cstate = CD_RMV;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    ext = hc->bs_hdl;

    /* Assuming card is always inserted */
    if (ext->nocd) {
        return CD_INS;
    }

    /* SDx_CD and SDx_WP pins are connected so that USDHC_PRES_STATE register can tell CD/WP status */
    status = in32(base + IMX_USDHC_PRES_STATE);

    if (status & IMX_USDHC_PRES_STATE_CDPL_MASK) {
        cstate |= CD_INS;
        imx_update_wp_status(hc, &cstate);
    }
    return (cstate);
}

/**
 * CD/WP through GPIO pins, interrupt may or may not be used
 * @param hc Host controller handle
 *
 * @return Card detect state
 */
static int imx_gpio_cd(sdio_hc_t *const hc)
{
    imx_ext_t    *const ext = hc->bs_hdl;
    int          cstate = CD_RMV;
    unsigned     val;

    /* Debounce delay when card inserted or removed. */
    delay(10);

    /*
     * CD pin low to indicate card inserted
     * This may not be true for all targets, so another
     * "cdpolarity" option may be needed
     */
    if (!(in32(ext->cd_base + IMX_GPIO_PSR) & (1 << ext->cd_pin))) {
        cstate |= CD_INS;

        if (ext->cd_irq) {
            /* Clear GPIO interrupt */
            out32(ext->cd_base + IMX_GPIO_ISR, (1u << ext->cd_pin));

            /* Change gpio interrupt to rising edge sensitive to catch card removal event */
            if (ext->cd_pin < 16) {
                val = in32(ext->cd_base + IMX_GPIO_ICR1) & ~(0x3 << (2 * ext->cd_pin));
                val |= (0x2 << (2 * ext->cd_pin));
                out32(ext->cd_base + IMX_GPIO_ICR1, val);
            } else {
                val = in32(ext->cd_base + IMX_GPIO_ICR2) & ~(0x3 << (2 * (ext->cd_pin - 16)));
                val |= (0x2 << (2 * (ext->cd_pin - 16)));
                out32(ext->cd_base + IMX_GPIO_ICR2, val);
            }

            InterruptUnmask(ext->cd_irq, ext->cd_iid);
        }
        imx_update_wp_status(hc, &cstate);
    } else {
        if (ext->cd_irq) {
            /* Card removed, reconfigure gpio interrupt to falling edge sensitive */
            if (ext->cd_pin < 16) {
                val = in32(ext->cd_base + IMX_GPIO_ICR1) | (0x3 << (2 * ext->cd_pin));
                out32(ext->cd_base + IMX_GPIO_ICR1, val);
            } else {
                val = in32(ext->cd_base + IMX_GPIO_ICR2) | (0x3 << (2 * (ext->cd_pin - 16)));
                out32(ext->cd_base + IMX_GPIO_ICR2, val);
            }
            InterruptUnmask(ext->cd_irq, ext->cd_iid);
        }
    }

    return cstate;
}

/**
 * Board Specific options can be passed through command line arguments or syspage optstr attribute,
 * but the syspage way is recommended since it can pass different options to different uSDHC controllers.
 * Example of the BS options: bs=cd_irq=165:cd_base=0x0209C000:cd_pin=5:vdd1_8
 *        -- CD pin is GPIO1[5] (GPIO1 Base: 0x0209C000)
 *        -- IRQ for GPIO1[5] is 165
 *        -- 1.8v is supported
 * Also please note that the optstr passed from syspage can be overwritten by the SDMMC command line arguments
 *
 * Notes:
 *        CD_BASE=base, CD_PIN=pin, CD_IRQ=irq can be replaced by one single option: CD=base^pin^irq
 *        WP_BASE=base, WP_PIN=pin can be replaced by one single option: WP=base^pin
 *
 * For example:
 *        cd=0x020a0000^0^192 is equivalent to cd_base=0x020a0000:cd_pin=0:cd_irq=192
 *        wp=0x020a0000^1 is equivalent to wp_base=0x020a0000:wp_pin=1
 *
 * @param hc Host controller handle
 * @param options driver cmd line options
 *
 * @return Execution status
 */
static int imx_bs_args(sdio_hc_t *const hc, char *options)
{
    char        *value;
    int         opt;
    imx_ext_t   *ext = hc->bs_hdl;

    /* Default values */
    ext->nocd = 0;
    ext->cd_irq = 0;
    ext->cd_pbase = 0;
    ext->cd_base = 0;
    ext->cd_pin = 0;
    ext->wp_pbase = 0;
    ext->wp_base = 0;
    ext->wp_pin = 0;
    ext->vdd1_8 = 0;
    ext->rs_pbase = 0;
    ext->rs_base = 0;
    ext->rs_pin = 0;

    static char     *opts[] = {
#define NOCD            0
        "nocd",             /* No card detection capability */
#define CD_IRQ          1
        "cd_irq",           /* Interrupt vector for GPIO CD pin */
#define CD_BASE         2
        "cd_base",          /* GPIO base address for the CD pin */
#define CD_PIN          3
        "cd_pin",
#define CD              4   /* This option covers CD_IRQ, CD_BASE, CD_PIN */
        "cd",
#define WP_BASE         5
        "wp_base",          /* GPIO base address for the WP pin */
#define WP_PIN          6
        "wp_pin",
#define WP              7   /* This option covers WP_BASE, WP_PIN */
        "wp",
#define VDD1_8          8
        "vdd1_8",           /* 1.8 V support capability */
#define RS_BASE         9
        "rs_base",          /* GPIO base address for the Reset pin */
#define RS_PIN          10
        "rs_pin",
#define RS              11  /* This option covers RS_BASE, RS_PIN */
        "rs",
        NULL
    };

    while (options && (*options != '\0')) {
        opt = sdio_hc_getsubopt(&options, opts, &value);
        if (opt == -1) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: invalid BS option %s", __func__, value);
            return EINVAL;
        }

        switch (opt) {
            case NOCD:
                ext->nocd = 1;
                break;
            case CD_IRQ:
                if (value != NULL) {
                    ext->cd_irq = (int)strtol(value, NULL, 0);
                }
                break;
            case CD_BASE:
                 if (value != NULL) {
                    ext->cd_pbase = strtoull(value, NULL, 0);
                 }
                break;
            case CD_PIN:
                if (value != NULL) {
                    ext->cd_pin = (int)strtol(value, NULL, 0);
                }
                break;
            case CD:
                if (value != NULL) {
                    ext->cd_pbase = strtoull(value, &value, 0);
                    if (*value == '^') {
                        ext->cd_pin = (int)strtol(value + 1, &value, 0);
                    }
                    if (*value == '^') {
                        ext->cd_irq = (int)strtol(value + 1, &value, 0);
                    }
                }
                break;
            case WP_BASE:
                if (value != NULL) {
                    ext->wp_pbase = strtoull(value, NULL, 0);
                }
                break;
            case WP_PIN:
                if (value != NULL) {
                    ext->wp_pin = (int)strtol(value, NULL, 0);
                }
                break;
            case WP:
                if (value != NULL) {
                    ext->wp_pbase = strtoull(value, &value, 0);
                    if (*value == '^') {
                        ext->wp_pin = (int)strtol(value + 1, &value, 0);
                    }
                }
                break;
            case VDD1_8:
                ext->vdd1_8 = 1;
                break;
            case RS_BASE:
                if (value != NULL) {
                    ext->rs_pbase = strtoull(value, NULL, 0);
                }
                break;
            case RS_PIN:
                if (value != NULL) {
                    ext->rs_pin = (int)strtol(value, NULL, 0);
                }
                break;
            case RS:
                if (value != NULL) {
                    ext->rs_pbase = strtoull(value, &value, 0);
                    if (*value == '^') {
                        ext->rs_pin = (int)strtol(value + 1, &value, 0);
                    }
                }
                break;
            default:
                break;
        }
    }

    return EOK;
}

/**
 * De-initialization of host controller (board specific part)
 * @param hc Host controller
 *
 * @return EOK always
 */
static int imx_dinit(sdio_hc_t *const hc)
{
    imx_ext_t *const ext = hc->bs_hdl;

    if (ext) {
        if (ext->cd_iid != -1) {
            InterruptDetach(ext->cd_iid);
        }

        if (ext->cd_base != MAP_DEVICE_FAILED) {
            munmap_device_io(ext->cd_base, IMX_GPIO_SIZE);
        }

        if (ext->wp_base != MAP_DEVICE_FAILED) {
            munmap_device_io(ext->wp_base, IMX_GPIO_SIZE);
        }

        free(ext);
    }

    return (imx_sdhcx_dinit(hc));
}

/**
 * Initialization of host controller (board specific part)
 * @param hc Host controller handle
 *
 * @return Execution status
 */
static int imx_init(sdio_hc_t *hc)
{

    imx_ext_t           *ext;
    sdio_hc_cfg_t       *const cfg = &hc->cfg;
    int                 status;
    uint32_t            rs_gpio_gdir, rs_gpio_dr;

    if (!(ext = calloc(1, sizeof(imx_ext_t)))) {
        return ENOMEM;
    }

    /* By default 1.8V is not enabled */
    hc->bs_hdl = ext;
    memset(ext, 0, sizeof(imx_ext_t));
    ext->cd_base = MAP_DEVICE_FAILED;
    ext->wp_base = MAP_DEVICE_FAILED;
    ext->cd_iid = -1;

    if (imx_bs_args(hc, cfg->options)) {
        return EINVAL;
    }

    if ((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC)) {
        ext->vdd1_8 = 1;
        ext->nocd = 1;
    }

    if (ext->rs_pbase) {
        ext->rs_base = mmap_device_io(IMX_GPIO_SIZE, ext->rs_pbase);
        if (MAP_DEVICE_FAILED == ext->rs_base) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: GPIO mmap_device_io failed.", __func__);
            return ENOMEM;
        }
    }

    if (ext->cd_pbase) {
        ext->cd_base = mmap_device_io(IMX_GPIO_SIZE, ext->cd_pbase);
        if (MAP_DEVICE_FAILED == ext->cd_base) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: GPIO mmap_device_io failed.", __func__);
            return ENOMEM;
        }
    }

    if (ext->wp_pbase) {
        ext->wp_base = mmap_device_io(IMX_GPIO_SIZE, ext->wp_pbase);
        if (MAP_DEVICE_FAILED == ext->wp_base) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0, "%s: GPIO mmap_device_io failed.", __func__);
            imx_dinit(hc);
            return ENOMEM;
        }
    }

    if (ext->rs_pbase) {
        rs_gpio_gdir = in32(ext->rs_base + IMX_GPIO_GDIR);
        rs_gpio_dr = in32(ext->rs_base + IMX_GPIO_GDIR);
        out32(ext->rs_base + IMX_GPIO_GDIR, rs_gpio_gdir | (1 << ext->rs_pin)); /* Set the pin as output */
        out32(ext->rs_base, rs_gpio_dr & ~((1 << ext->rs_pin))); /* Set the pin to "0" */
        delay(10); /* Delay for card reset */
        out32(ext->rs_base + IMX_GPIO_GDIR, rs_gpio_gdir);
    }

    status = imx_sdhcx_init(hc);
    if (EOK != status) {
        imx_dinit(hc);
        return status;
    }

    if (ext->cd_irq) {
        struct sigevent    event;

        SIGEV_PULSE_INIT(&event, hc->hc_coid, (short)hc->priority, HC_EV_CD, NULL);

        /* Disable and clear interrupt status */
        out32(ext->cd_base + IMX_GPIO_IMR, in32(ext->cd_base + IMX_GPIO_IMR) & ~(1 << ext->cd_pin));
        out32(ext->cd_base + IMX_GPIO_ISR, (1u << ext->cd_pin));

        ext->cd_iid = InterruptAttachEvent(ext->cd_irq, &event, _NTO_INTR_FLAGS_TRK_MSK);
        if (ext->cd_iid == -1) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, 1, 0, "%s: InterruptAttachEvent(%d) failure - %s", __FUNCTION__, ext->cd_irq,
                       strerror(errno));
            imx_dinit(hc);
            return errno;
        }

        hc->caps |= HC_CAP_CD_INTR;
    }

    /*
     * Overwrite cd/dinit functions  by default,
     * CD is detected through uSDHCx_PRES_STATE register and interrupt (HC_CAP_CD_INTR is set)
     */
    hc->entry.dinit = imx_dinit;
    if (ext->cd_pbase) {
        hc->entry.cd = imx_gpio_cd;
    } else {
        hc->entry.cd = imx_cd;
    }

    /* Overwrite some of the capabilities that are set by imx_sdhcx_init() */
    if (ext->nocd) {
        hc->caps &= ~HC_CAP_CD_INTR;
    }

    /* "bs=vdd1_8" must be set in order to enable 1.8v operations */
    if (!ext->vdd1_8) {
        hc->ocr     &= ~OCR_VDD_17_195;
        hc->caps    &= ~HC_CAP_SV_1_8V;
        hc->caps    &= ~(HC_CAP_SDR12 | HC_CAP_SDR25 | HC_CAP_SDR50 | HC_CAP_SDR104 | HC_CAP_DDR50 | HC_CAP_HS200 |
                      HC_CAP_HS400 | HC_CAP_HS400ES);
    } else {
        hc->ocr     |= OCR_VDD_17_195;
    }

    /* Enable GPIO interrupt */
    if (ext->cd_irq) {
        out32(ext->cd_base + IMX_GPIO_IMR, in32(ext->cd_base + IMX_GPIO_IMR) | (1 << ext->cd_pin));
    }

    return EOK;
}

/**
 * Card detect event
 * @param hc Host controller handle
 *
 * @return EOK always
 */
static int mx_cd_event(sdio_hc_t *const hc)
{
    sdio_hc_event(hc, HC_EV_CD);
    return EOK;
}

/**
 * Board specific event
 * @param hc Host controller handle
 * @param ev Event
 *
 * @return Execution status
 */
int bs_event(sdio_hc_t *const hc, sdio_event_t *const ev)
{
    int    status;

    switch (ev->code) {
        case HC_EV_CD:
            status = mx_cd_event(hc);
            break;

        default:
            status = ENOTSUP;
            break;
    }

    return status;
}

static sdio_product_t   sdio_fs_products[] = {
    { .did = SDIO_DEVICE_ID_WILDCARD, .class = 0, .aflags = 0, .name = "imx", .init = imx_init },
};

sdio_vendor_t    sdio_vendors[] = {
    { .vid = SDIO_VENDOR_ID_WILDCARD, .name = "NXP", .chipsets = sdio_fs_products },
    { .vid = 0, .name = NULL, .chipsets = NULL }
};

/** @} */


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devb/sdmmc/aarch64/mx8x.le/bs.c $ $Rev: 982571 $")
#endif
