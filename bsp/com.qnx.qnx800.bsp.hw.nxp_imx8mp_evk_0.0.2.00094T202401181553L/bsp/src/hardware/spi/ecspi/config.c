/*
 * Copyright (c) 2010,2023, BlackBerry Limited.
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


#include "ecspi.h"

static spi_dev_params_t dev_params[SPI_MAX_BUS][SPI_MAX_DEV];

spi_dev_params_t *const
get_dev_params(const uint32_t busid, const uint32_t ssid)
{
    return &dev_params[busid][ssid];
}

/**
 *  @brief             SPI configuration function.
 *  @param  hdl        SPI driver handler.
 *  @param  spi_dev    SPI device structure.
 *
 *  @return            EOK --success otherwise fail.
 */
int ecspi_cfg(const ecspi_t *const spi, spi_dev_t *spi_dev)
{
    spi_cfg_t *const cfg = &spi_dev->devinfo.cfg;
    const uint32_t ssid  = spi_dev->devinfo.devno;
    const uint32_t busid = spi->bus_node->busno;
    uint32_t post_div, pre_div, drate, post_drate;

    if (cfg == NULL) {
        return EOK;
    }

    if ((ssid > SPI_MAX_SSID) || (busid > SPI_MAX_BUSID)) {
        spi_slogf(_SLOG_ERROR, "%s: busid(%u) or ssid(%u) is not supported",
                                __func__, busid, ssid);
        return EINVAL;
    }

    const uint32_t word_width = cfg->mode & SPI_MODE_WORD_WIDTH_MASK;
    if ((word_width != SPI_MODE_WORD_WIDTH_32) &&
        (word_width != SPI_MODE_WORD_WIDTH_16) &&
        (word_width != SPI_MODE_WORD_WIDTH_8)) {
        spi_slogf(_SLOG_ERROR, "%s: %d-bit word width is not supported by this controller",
                                __func__, word_width);
        return EINVAL;
    }

    /* Assign the datarate if calculated rate <= desired rate;
     * OR assign lowest possible rate last time through the loop
     */
    for (post_div = 0; post_div < 16 ; post_div++) {
        post_drate = spi->input_clk >> post_div;
        for (pre_div = 0; pre_div < 16 ;pre_div++) {
            drate = post_drate / (pre_div + 1);
            if (drate <= cfg->clock_rate) {
                break;
            }
        }
        if (drate <= cfg->clock_rate) {
            break;
        }
    }

    cfg->clock_rate = drate;
    /* update current clock rate */
    spi_dev->devinfo.current_clkrate = cfg->clock_rate;

    spi_dev_params_t *const ctrl_params = get_dev_params(busid, ssid);
    ctrl_params->ctrl_reg = (post_div << ECSPI_CONREG_POSTDIVIDR_POS) | (pre_div << ECSPI_CONREG_PREDIVIDR_POS);

    switch (cfg->mode & SPI_MODE_RDY_MASK) {
        case SPI_MODE_RDY_EDGE:
            ctrl_params->ctrl_reg |= ECSPI_CONTROLREG_DRCTL_EDGE << ECSPI_CONTROLREG_DRCTL_POS;
            break;
        case SPI_MODE_RDY_LEVEL:
            ctrl_params->ctrl_reg |= ECSPI_CONTROLREG_DRCTL_LEVEL << ECSPI_CONTROLREG_DRCTL_POS;
            break;
        default:
            break;
    }

    /* set SCLK polarity, CPOL: assume SPI_MODE_CPOL_1 same as CPHA */
    if (cfg->mode & SPI_MODE_CPOL_1) {
        ctrl_params->cfg_reg &= ~(1 << (ssid + ECSPI_CONFIGREG_POL_POS));    // set CPOL=0
        ctrl_params->cfg_reg &= ~(1 << (ssid + ECSPI_CONFIGREG_CLKCTL_POS));
    } else {
        ctrl_params->cfg_reg |= (1 << (ssid + ECSPI_CONFIGREG_POL_POS));     // set CPOL=1
        ctrl_params->cfg_reg |= (1 << (ssid + ECSPI_CONFIGREG_CLKCTL_POS));
    }

    /* set SCLK phase, CPHA: assume SPI_MODE_CPHA_1 same as CPHA */
    if (cfg->mode & SPI_MODE_CPHA_1) {
        ctrl_params->cfg_reg &= ~(1 << (ssid + ECSPI_CONFIGREG_PHA_POS));    // set CPHA=0
    } else {
        ctrl_params->cfg_reg |= (1 << (ssid + ECSPI_CONFIGREG_PHA_POS));     // set CPHA=1
    }

    /* set slave select (SS) polarity, 0 = active low, 1 = active high
     * note that SCLK, SS polarity bit settings are different */
    if (cfg->mode & SPI_MODE_CSPOL_HIGH) {
        ctrl_params->cfg_reg |= (1 << (ssid + ECSPI_CONFIGREG_SSPOL_POS));   // SS active high
    } else {
        ctrl_params->cfg_reg &= ~(1 << (ssid + ECSPI_CONFIGREG_SSPOL_POS));  // SS active low
    }

    return EOK;
}

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/spi/ecspi/config.c $ $Rev: 980075 $")
#endif
