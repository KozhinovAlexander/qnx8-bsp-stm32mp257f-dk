/*
 * Copyright (c) 2023 BlackBerry Limited.
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


#include "proto.h"

/**
 *  @brief              Driver initialization
 *  @param  argc        Argument count
 *  @param  argv        Argument strings
 *
 *  @return             device handle --success
 *                      NULL --failure
 */
void * imx_init(int argc, char *argv[])
{
    imx_dev_t      *dev;

    dev = malloc(sizeof(imx_dev_t));
    if (dev == NULL) return NULL;

    slogf(_SLOG_SETCODE(_SLOGC_I2C, 0), _SLOG_INFO, "Starting I2C driver (build date %s %s)", __DATE__, __TIME__);

    if (imx_options(dev, argc, argv) == -1) {
        i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: imx_options failed", __func__);
        free(dev);
        return NULL;
    }

    dev->regbase = mmap_device_io(dev->reglen, dev->physbase);
    if (dev->regbase == (uintptr_t)MAP_FAILED) {
        i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: mmap_device_io failed", __func__);
        free(dev);
        return NULL;
    }

    /* Disable I2C controller */
    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, CTRREG_DIS(dev->itype));

    /* Clear status */
    imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF, STSREG_CLRAL(dev->itype));

    delay(1);

    /* Set clock prescaler using default baud*/
    imx_set_bus_speed(dev, 100000, NULL);

    /* Enable I2C controller */
    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, CTRREG_IEN(dev->itype));

    /* Attach to I2C interrupt */
    SIGEV_INTR_INIT(&dev->intrevent);
    dev->iid = InterruptAttachEvent(dev->intr, &dev->intrevent, _NTO_INTR_FLAGS_TRK_MSK);
    if (dev->iid != -1) {
        /* Enable interrupts */
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF,
                (uint8_t)(imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF) | CTRREG_IIEN));

        return dev;
    }

    i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: InterruptAttachEvent failed: %s", __func__, strerror(errno));
    munmap_device_io(dev->regbase, dev->reglen);
    free(dev);

    return NULL;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/init.c $ $Rev: 979323 $")
#endif
