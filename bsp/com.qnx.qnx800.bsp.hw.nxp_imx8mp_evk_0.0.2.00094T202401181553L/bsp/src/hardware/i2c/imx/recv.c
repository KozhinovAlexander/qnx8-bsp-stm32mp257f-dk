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
 *  @brief              I2C receive function
 *  @param  hdl         Driver handle
 *  @param  buf         Receive buffer pointer
 *  @param  len         Receive buffer length
 *  @param  stop        STOP condition flag
 *
 *  @return             I2C receive status
 */
i2c_status_t imx_recv(void* const hdl, void *buf, unsigned int len, unsigned int stop)
{
    imx_dev_t       *dev = hdl;
    i2c_status_t    status;

    if (len <= 0) return I2C_STATUS_DONE;

    status = imx_wait_bus_not_busy(dev);
    if (status != I2C_STATUS_DONE) return status;

    /* Send slave address */
    if (dev->slave_addr_fmt == I2C_ADDRFMT_7BIT) {
        status = imx_sendaddr7(dev, dev->slave_addr, IMX_I2C_ADDR_RD, dev->restart);
    } else  {
        status = imx_sendaddr10(dev, dev->slave_addr, IMX_I2C_ADDR_RD, dev->restart);
    }

    if (status != I2C_STATUS_DONE) return status;

    if (len > 1) {
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, (uint8_t)(CTRREG_IEN(dev->itype) | CTRREG_IIEN | CTRREG_MSTA));
    } else {
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, (uint8_t)(CTRREG_IEN(dev->itype) | CTRREG_IIEN | CTRREG_MSTA | CTRREG_TXAK));
    }

    imx_i2c_rdr(dev, IMX_I2C_DATREG_OFF);

    while (len > 0) {
        status = imx_recvbyte(dev, buf, (len == 2), (len == 1) && stop);
        if (status != I2C_STATUS_DONE) return status;
        ++buf;
        --len;
    }

    if (!stop) {
        imx_wait_complete(dev);
    }

    dev->restart = !stop;

    return I2C_STATUS_DONE;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/recv.c $ $Rev: 979323 $")
#endif
