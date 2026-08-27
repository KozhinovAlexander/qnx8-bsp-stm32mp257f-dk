/*
 * $QNXLicenseC:
 * Copyright 2012, 2022 BlackBerry Limited.
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


#include "imx_i2c_drv.h"
#include "ipl.h"
#include <hw/inout.h>

void init_i2c_bus(imx_i2c_dev_t * const dev)
{
    // init I2C clock rate
    out16(dev->base + I2C_IFDR, dev->div);
}

static int wait_op_done(const unsigned int base, const int is_tx)
{
    volatile unsigned short v;
    int i = WAIT_RXAK_LOOPS;

    while ((((v = in16(base + I2C_I2SR)) & (I2C_I2SR_IIF)) == 0 ||
            (v & (I2C_I2SR_ICF)) == 0) && --i > 0) {

        if (v & I2C_I2SR_IAL) {
            kprintf("Error: Arbitration lost\n");
            return -1;
        }
    }

    if (i <= 0) {
        kprintf("Error: timeout unexpected\n");
        return -1;
    }
    if (is_tx) {
        if (v & I2C_I2SR_IAL) {
            kprintf("Error: Arbitration lost\n");
            return -1;
        }
        if (v & I2C_I2SR_RXAK) {
            kprintf("Error: no ack received\n");
            return -1;
        }
    }
    return 0;
}

// For master TX, always expect a RXAK signal to be set!
static int tx_byte(unsigned char * const data, const unsigned int base)
{
    // clear both IAL and IIF bits
    out16(base + I2C_I2SR, 0);

    // transmit the data
    out16(base + I2C_I2DR, *data);

    if (wait_op_done(base, 1) != 0) {
        return -1;
    }

    return 0;
}

// For master RX
static int rx_byte(unsigned char * const data, const unsigned int base)
{
    unsigned short i2cr;

    if (wait_op_done(base, 0) != 0) {
        return -1;
    }

    // clear both IAL and IIF bits
    out16(base + I2C_I2SR, 0);

    // last byte --> generate STOP
    i2cr = in16(base + I2C_I2CR);
    out16(base + I2C_I2CR, (i2cr & ~(I2C_I2CR_MSTA | I2C_I2CR_MTX)));

    *data = (unsigned char)in16(base + I2C_I2DR);

    return 0;
}

static inline int is_bus_free(const unsigned int base)
{
    return ((in16(base + I2C_I2SR) & I2C_I2SR_IBB) == 0);
}

static inline int wait_till_busy(const unsigned int base)
{
    int i = I2C_WAIT_CNT;

    while (((in16(base + I2C_I2SR) & (I2C_I2SR_IBB)) == 0) && (--i > 0)) {
        if (in16(base + I2C_I2SR) & I2C_I2SR_IAL) {
            kprintf("Error: arbitration lost!\n");
            return -1;
        }
    }

    if (i <= 0) {
        kprintf("Error: timeout unexpected\n");
        return -1;
    }

    return 0;
}

static int i2c_xfer(imx_i2c_dev_t * const dev, const unsigned char reg, unsigned char* val, const int dir)
{
    volatile int i;
    unsigned char data;
    unsigned short i2cr;
    int ret = 0;

    // reset and enable I2C1
    out16(dev->base + I2C_I2CR, 0);
    out16(dev->base + I2C_I2CR, I2C_I2CR_IEN);

    // wait at least 2 cycles of per_clk, declare i as volatile to avoid optimization
    for (i = 0; i < 100; i++) ;

    // Step 1: generate START signal
    // 1.1 make sure bus is free
    if (!is_bus_free(dev->base)) {
        kprintf("Error: Bus is not free\n");
        return -1;
    }

    // 1.2 clear both IAL and IIF bits
    out16(dev->base + I2C_I2SR, 0);

    // 1.3 assert START signal and also indicate TX mode
    i2cr = I2C_I2CR_IEN | I2C_I2CR_MSTA | I2C_I2CR_MTX;
    out16(dev->base + I2C_I2CR, i2cr);

    // 1.4 make sure bus is busy after the START signal
    if (wait_till_busy(dev->base) != 0) {
        kprintf("Error: Bus is not busy\n");
        return -1;
    }

    // Step 2: send slave address + read/write at the LSB
    data = (unsigned char)(((dev->slave << 1) | I2C_WRITE) & 0xFF);
    if (tx_byte(&data, dev->base) != 0) {
        kprintf("Error: Failed to transmit slave address\n");
        return -1;
    }

    // Step 3: send I2C device register address
    data = reg & 0xFF;
    if (tx_byte(&data, dev->base) != 0) {
        kprintf("Error: Failed to transmit device register address\n");
        return -1;
    }

    // Step 4: read/write data
    if (dir == I2C_READ) {
        // do repeat-start
        i2cr = in16(dev->base + I2C_I2CR);
        out16(dev->base + I2C_I2CR, (i2cr | I2C_I2CR_RSTA));

        // send slave address again, but indicate read operation
        data = (unsigned char)((dev->slave << 1) | I2C_READ);
        if (tx_byte(&data, dev->base) != 0) {
            kprintf("Error: Failed to transmit slave address\n");
            return -1;
        }

        // change to receive mode
        i2cr = in16(dev->base + I2C_I2CR);

        // read only one byte, make sure don't send ack
        i2cr |= I2C_I2CR_TXAK;
        i2cr &= ~I2C_I2CR_MTX;
        out16(dev->base + I2C_I2CR, i2cr);

        // dummy read
        in16(dev->base + I2C_I2DR);

        // now reading ...
        if (rx_byte(&data, dev->base) != 0) {
            kprintf("Error: Failed to receive data\n");
            return -1;
        }
        *val = data;

    } else {    // I2C_WRITE
        data = (unsigned char)(*val & 0xFF);
        if ((ret = tx_byte(&data, dev->base)) != 0) {
            kprintf("Error: Failed to transmit data\n");
        }

        // generate STOP by clearing MSTA bit
        out16(dev->base + I2C_I2CR, (I2C_I2CR_IEN | I2C_I2CR_MTX));
    }

    return ret;
}

int i2c_write(imx_i2c_dev_t * const dev, const unsigned char reg, unsigned char* const val)
{
    return i2c_xfer(dev, reg, val, I2C_WRITE);
}

int i2c_read(imx_i2c_dev_t * const dev, const unsigned char reg, unsigned char* const val)
{
    return i2c_xfer(dev, reg, val, I2C_READ);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_i2c_drv.c $ $Rev: 980064 $")
#endif
