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
 *  @brief              Reset I2C controller
 *  @param  dev         Driver handle
 *
 *  @return             None
 */
static void imx_i2c_reset(imx_dev_t *dev)
{
    /* Disable controller */
    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, CTRREG_DIS(dev->itype));

    /* Clear status */
    imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF, STSREG_CLRAL(dev->itype));

    delay(1);

    /* Enable controller */
    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, CTRREG_IEN(dev->itype));

    imx_i2c_wrr(dev, IMX_I2C_FRQREG_OFF, dev->i2c_freq_val);

    dev->restart = 0;

    delay(1);
}

/**
 *  @brief              Wait for I2C bus not busy
 *  @param  dev         Driver handle
 *
 *  @return             I2C status
 *
 */
i2c_status_t imx_wait_bus_not_busy(imx_dev_t * const dev)
{
    unsigned        tries = 1000000;

    if (dev->restart) return I2C_STATUS_DONE;

    while (imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF) & STSREG_IBB) {
        if (tries-- == 0) {
            i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: wait bus idle failed (%x %x)",
                __func__, imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF), imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF));

            /* Reset the controller to see if it's able to recover */
            imx_i2c_reset(dev);

            /* Try again to see if it's OK now after reset */
            if (!(imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF) & STSREG_IBB)) break;

            delay(1);
            return I2C_STATUS_ERROR;
        }
    }

    return I2C_STATUS_DONE;
}

/**
 *  @brief              Wait for I2C transaction complete
 *  @param  dev         Driver handle
 *
 *  @return             I2C status register(IMX_I2C_STSREG_OFF)'s value
 */
uint8_t imx_wait_complete(imx_dev_t * const dev)
{
    uint8_t         status;
    const uint64_t  ntime = 500000000ULL;
    int             interr = EOK;

    while (interr != ETIMEDOUT) {
        TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_INTR, NULL, &ntime, NULL);
        interr = InterruptWait_r(0, NULL);
        if (interr == ETIMEDOUT) break;

        status = imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF);
        if (status & STSREG_IIF) {
            /* Clear the interrupt status bits */
            if (dev->itype == S32_I2C) {
                imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF,
                    (uint8_t)(imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF) | STSREG_IIF));
            } else {
                imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF,
                    (uint8_t)(imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF) & ~(STSREG_IIF)));
            }
            InterruptUnmask(dev->intr, dev->iid);
            return status;
        }
        InterruptUnmask(dev->intr, dev->iid);
    }

    i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: timedout (%x %x)",
        __func__, imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF), imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF));

    /* Timeout case, we need reset */
    imx_i2c_reset(dev);

    return 0;
}

/**
 *  @brief              Receive one byte
 *  @param  dev         Driver handle
 *  @param  byte        Receive byte pointer
 *  @param  nack        Send NACK
 *  @param  stop        Send stop condition
 *
 *  @return             I2C status
 *
 */
i2c_status_t imx_recvbyte(imx_dev_t* const dev, uint8_t* const byte, const int nack, const int stop)
{
    uint8_t status;

    status = imx_wait_complete(dev);

    if (!(status & STSREG_ICF)) {
        if (status) {
            imx_i2c_reset(dev);
        }
        return I2C_STATUS_ERROR;
    }

    if (nack) {
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, (uint8_t)(CTRREG_IEN(dev->itype) | CTRREG_IIEN | CTRREG_MSTA | CTRREG_TXAK));
    } else if (stop) {
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, (uint8_t)(CTRREG_IEN(dev->itype) | CTRREG_TXAK));
    }

    *byte = imx_i2c_rdr(dev, IMX_I2C_DATREG_OFF);

    return I2C_STATUS_DONE;
}

/**
 *  @brief              Send one byte
 *  @param  dev         Driver handle
 *  @param  byte        Byte to be sent
 *
 *  @return             I2C status
 *
 */
i2c_status_t imx_sendbyte(imx_dev_t * const dev, const uint8_t byte)
{
    uint8_t status;

    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF, (uint8_t)(imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF) | CTRREG_MTX));
    imx_i2c_wrr(dev, IMX_I2C_DATREG_OFF, byte);

    status = imx_wait_complete(dev);

    if (!(status & STSREG_ICF)) {
        if (status) {
            imx_i2c_reset(dev);
        }
        return I2C_STATUS_ERROR;
    }

    if (!(imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF) & CTRREG_MSTA)) {
        if (status & STSREG_IAL) {
            if (dev->itype == S32_I2C) {
                imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF, status | STSREG_IAL);
            } else {
                imx_i2c_wrr(dev, IMX_I2C_STSREG_OFF, status & ~STSREG_IAL);
            }
            imx_i2c_reset(dev);

            return I2C_STATUS_ARBL;
        }

        if (status & STSREG_IAAS) {
            imx_i2c_reset(dev);
            return I2C_STATUS_ERROR;
        }
    }

    if (status & STSREG_RXAK) {
        /* Send Stop to stop I2C transaction */
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF,
             (uint8_t)(imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF) & ~(CTRREG_MSTA | CTRREG_MTX)));
        return I2C_STATUS_NACK;
    }

    return I2C_STATUS_DONE;
}

/**
 *  @brief              Wait for I2C bus busy
 *  @param  dev         Driver handle
 *
 *  @return             I2C status
 *
 */
static i2c_status_t imx_wait_bus_busy(imx_dev_t * const dev)
{
    /* Wait for 500us */
    int timeout = 5000;

    /* When Start is detected, IBB is set */
    while (!(imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF) & STSREG_IBB)) {
        if (--timeout <= 0) {
            i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: timedout (%x %x)",
                        __func__,
                        imx_i2c_rdr(dev, IMX_I2C_CTRREG_OFF),
                        imx_i2c_rdr(dev, IMX_I2C_STSREG_OFF));
            imx_i2c_reset(dev);
            return I2C_STATUS_ERROR;
        }

        nanospin_ns(100);
    }

    return I2C_STATUS_DONE;
}

/**
 *  @brief              Send I2C 7-bit address
 *  @param  addr        7-bit address
 *  @param  rw          Read or write operation flag
 *  @param  restart     Restart condition flag
 *
 *  @return             I2C status
 *
 */
i2c_status_t imx_sendaddr7(imx_dev_t * const dev, const unsigned addr, const uint8_t rw, const unsigned restart)
{
    i2c_status_t    status;

    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF,
                (uint8_t)(CTRREG_IEN(dev->itype) | CTRREG_IIEN | CTRREG_MSTA |
                (restart ? CTRREG_RSTA : 0u) | CTRREG_MTX));

    /* Didn't detect Start */
    status = imx_wait_bus_busy(dev);
    if (status != I2C_STATUS_DONE) return status;

    return imx_sendbyte(dev, (uint8_t)((addr << 1) | rw));
}

/**
 *  @brief              Send I2C 10-bit address
 *  @param  addr        10-bit address
 *  @param  rw          Read or write operation flag
 *  @param  restart     Restart condition flag
 *
 *  @return             I2C status
 *
 */
i2c_status_t imx_sendaddr10(imx_dev_t * const dev, const unsigned addr, const uint8_t rw, const unsigned restart)
{
    i2c_status_t    status;

    imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF,
                (uint8_t)(CTRREG_IEN(dev->itype) |
                CTRREG_IIEN | CTRREG_MSTA |
                (restart ? CTRREG_RSTA : 0) | CTRREG_MTX));

    /* Didn't detect Start */
    status = imx_wait_bus_busy(dev);
    if (status != I2C_STATUS_DONE) return status;

    status = imx_sendbyte(dev, IMX_I2C_XADDR1(addr));
    if (status != I2C_STATUS_DONE) return status;
    status = imx_sendbyte(dev, IMX_I2C_XADDR2(addr));
    if (status != I2C_STATUS_DONE) return status;

    if (rw == IMX_I2C_ADDR_RD) {
        imx_i2c_wrr(dev, IMX_I2C_CTRREG_OFF,
                    (uint8_t)(CTRREG_IEN(dev->itype) |
                    CTRREG_IIEN | CTRREG_MSTA |
                    CTRREG_RSTA | CTRREG_MTX));
        status = imx_sendbyte(dev, (uint8_t)(IMX_I2C_XADDR1(addr) | rw));
        if (status != I2C_STATUS_DONE) return status;
    }

    return status;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/wait.c $ $Rev: 979323 $")
#endif
