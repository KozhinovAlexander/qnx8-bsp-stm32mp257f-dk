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

enum opt_index {LOOPBACK, WAITSTATE, CSD, BURST, END};

/**
 *  @brief             Process board specific options which is set in SPI config file.
 *  @param  spi        SPI driver handle.
 *  @param  options    Board specific options.
 *
 *  @return            EOK --success otherwise fail.
 */
static int process_args(ecspi_t *spi, char *options)
{
    char       *value;
    int        opt = 0;
    const char *c;

    char *ecspi_opts[] = {
        [LOOPBACK]  =   "loopback",     /* loopback interface for test purposes*/
        [WAITSTATE] =   "waitstate",    /* waitstates between data transfers */
        [CSD]       =   "csdelay",      /* chip select delay between chip select active edge and first SPI clock edge */
        [BURST]     =   "burst",
        [END]       =   NULL
    };

    if (options == NULL) {
        spi_slogf(_SLOG_INFO, "%s: No bs args passed in", __func__);
        return EOK;
    }

    while ((options != NULL) && (*options != '\0')) {
        c = options;
        opt = getsubopt(&options, ecspi_opts, &value);
        if (opt == -1) {
            spi_slogf(_SLOG_ERROR, "%s: Unsupported SPI device driver args: %s", __func__, c);
            return EINVAL;
        }

        switch (opt) {
            case LOOPBACK:
                spi->loopback = (uint8_t)(value ? strtoul(value, NULL, 0) : 1);
                break;
            case WAITSTATE:
                spi->waitstates = (uint16_t)strtoul(value, NULL, 0);
                break;
            case CSD:
                spi->csdelay = (uint8_t)strtoul(value, NULL, 0);
                break;
            case BURST:
                spi->burst = (uint8_t)(value ? strtoul(value, NULL, 0) : 1);
                break;
            default:
                return EINVAL;
        }
    }

    return EOK;
}

/**
 *  @brief             Initialize SPI controller and devices.
 *  @param  spi        SPI driver handle.
 *
 *  @return            EOK --success otherwise fail.
 */
static int init_device(ecspi_t *spi)
{
    uintptr_t base;
    spi_bus_t *const bus = spi->bus_node;
    spi_dev_t *spi_dev = bus->devlist;
    int       status = EOK;

    /*
     * Map in SPI registers
     */
    base = mmap_device_io(ECSPI_SIZE, spi->pbase);
    if (base == (uintptr_t)MAP_FAILED) {
        spi_slogf(_SLOG_ERROR, "%s: Failed to map SPI registers(%s)",
                               __func__, strerror(errno));
        return errno;
    }
    spi->vbase = base;

    ecspi_spi_disable(spi);

    /*
     * Initial device configuration with defaults from config file
     */
    while (spi_dev != NULL) {
        status = ecspi_cfg(spi, spi_dev);
        if (status != EOK) {
            spi_slogf(_SLOG_ERROR, "%s: ecspi_cfg failed", __func__);
            goto fail0;
        }
        spi_dev = spi_dev->next;
    }

    /* Attach SPI interrupt */
    status = ecspi_attach_intr(spi);
    if (status != EOK) {
        spi_slogf(_SLOG_ERROR, "%s: ecspi_attach_intr failed", __func__);
        goto fail0;
    }

    return EOK;

fail0:
    munmap_device_io(spi->vbase, ECSPI_SIZE);
    free(spi);
    return status;
}

/**
 *  @brief             SPI initialization.
 *  @param  bus        The SPI bus structure.
 *
 *  @return            EOK --success otherwise fail.
 */
int spi_init(spi_bus_t *bus)
{
    ecspi_t *spi = NULL;
    int status = EOK;

    if (bus == NULL) {
        spi_slogf(_SLOG_ERROR, "%s: SPI bus structure is NULL!", __func__);
        return EINVAL;
    }

    spi = calloc(1, sizeof(ecspi_t));
    if (spi == NULL) {
        spi_slogf(_SLOG_ERROR, "%s: Failed to alloc memory", __func__);
        return ENOMEM;
    }

    /* Save spi_ctrl to driver structure */
    spi->spi_ctrl = bus->spi_ctrl;
    spi->bus_node = bus;

    /* Get other SPI driver functions */
    bus->funcs->spi_fini = ecspi_fini;
    bus->funcs->drvinfo  = ecspi_drvinfo;
    bus->funcs->devinfo  = ecspi_devinfo;
    bus->funcs->setcfg   = ecspi_setcfg;
    bus->funcs->xfer     = ecspi_xfer;

    /* Set defaults: config file overrides the defaults */
    spi->pbase = bus->pbase ? bus->pbase : ECSPI2_BASE;
    spi->irq = bus->irq ? (int)bus->irq : ECSPI2_IRQ;
    spi->input_clk = bus->input_clk ? (uint32_t)bus->input_clk : ECSPI_INPUT_CLK;

    /* Set defaults */
    spi->loopback = 0;
    spi->waitstates = 0;
    spi->csdelay = 0;
    spi->burst = 1;

    spi->iid = -1;

    /* Process args, override the defaults */
    if (bus->bs) {
        status = process_args(spi, bus->bs);
        if (status != EOK) {
            spi_slogf(_SLOG_ERROR, "%s: process_args failed", __func__);
            goto fail0;
        }
    }

    /* Init SPI device */
    status = init_device(spi);
    if (status != EOK) {
        spi_slogf(_SLOG_ERROR, "%s: init_device failed: %s",
                                __func__, strerror(status));
        goto fail0;
    }

    /*
     * Create SPI chip select devices
     */
    status = spi_create_devs(bus->devlist);
    if (status != EOK) {
        spi_slogf(_SLOG_ERROR, "%s: spi_create_devs for %s failed",
                               __func__, bus->devlist->devinfo.name);
        goto fail1;
    }

    /* Save driver structure to drvhdl */
    bus->drvhdl = spi;

    return EOK;

fail1:
    munmap_device_io(spi->vbase, ECSPI_SIZE);
    if (spi->iid != -1) {
        InterruptDetach(spi->iid);
    }
fail0:
    free(spi);
    return status;
}

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/spi/ecspi/ecspi.c $ $Rev: 980075 $")
#endif
