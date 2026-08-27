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


#ifndef __I2C_IMX_PROTO_H_INCLUDED
#define __I2C_IMX_PROTO_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <hw/i2c.h>
#include <sys/hwinfo.h>
#include <drvr/hwinfo.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>


/* I2C controller type */
enum imx_i2c_type {
    IMX_I2C,   /* i.MX: iMX6/iMX7/iMX8 */
    S32_I2C,   /* S32: LS10XX, LS20XX, S32V, S32G */
};

#define IMX_I2C_REGLEN         0x18

#define IMX_I2C_ADRREG_OFF     0x00
#define IMX_I2C_FRQREG_OFF     0x04
#define IMX_I2C_CTRREG_OFF     0x08
    /* bit 7:
     * IMX_I2C: 1: Enable controller; 0: Disable controller
     * S32_I2C: 1: Disable controller; 0: Enable controller
     */
    #define CTRREG_IEN(itype)       ((uint8_t)(((itype) == S32_I2C) ? 0 : (1 << 7)))
    #define CTRREG_DIS(itype)       ((uint8_t)(((itype) == S32_I2C) ? (1 << 7) : 0))
    #define CTRREG_IIEN             ((uint8_t)(1 << 6))  /* Interrupt Enable */
    #define CTRREG_MSTA             ((uint8_t)(1 << 5))  /* Master/Slave mode select */
    #define CTRREG_MTX              ((uint8_t)(1 << 4))  /* Transmit/Receive mode select */
    #define CTRREG_TXAK             ((uint8_t)(1 << 3))  /* Transmit acknowledge enable */
    #define CTRREG_RSTA             ((uint8_t)(1 << 2))  /* Repeat start */
#define IMX_I2C_STSREG_OFF     0x0C
    #define STSREG_ICF              ((uint8_t)(1 << 7))  /* Transfer complete */
    #define STSREG_IAAS             ((uint8_t)(1 << 6))  /* Addressed as a slave bit */
    #define STSREG_IBB              ((uint8_t)(1 << 5))  /* Bus busy */
    #define STSREG_IAL              ((uint8_t)(1 << 4))  /* Arbitration lost: IMX_I2C: w0c; S32_I2C: w1c */
    #define STSREG_SRW              ((uint8_t)(1 << 2))  /* Slave Read/Write */
    #define STSREG_IIF              ((uint8_t)(1 << 1))  /* Interrupt Flag: IMX_I2C: w0c; S32_I2C: w1c */
    #define STSREG_RXAK             ((uint8_t)(1 << 0))  /* Received Acknowledge */
    #define STSREG_CLRAL(itype)     ((uint8_t)(((itype) == S32_I2C) ? (STSREG_IAL | STSREG_IIF) : 0))
#define IMX_I2C_DATREG_OFF     0x10

typedef struct _imx_dev {
    uintptr_t           regbase;
    paddr_t             physbase;
    unsigned            reglen;
    int                 intr;
    int                 iid;
    struct sigevent     intrevent;
    unsigned            slave_addr;
    i2c_addrfmt_t       slave_addr_fmt;
    uint8_t             restart;
    unsigned            input_clk;
    unsigned            speed;
    uint8_t             i2c_freq_val;
    int                 verbosity;
    uint8_t             itype;       /* I2C controller type */
} imx_dev_t;

#define IMX_I2C_XADDR1(addr)       ((uint8_t)(0xf0 | (((addr) >> 7) & 0x6)))
#define IMX_I2C_XADDR2(addr)       ((uint8_t)((addr) & 0xff))
#define IMX_I2C_ADDR_RD            1
#define IMX_I2C_ADDR_WR            0

#define IMX_I2C_INPUT_CLOCK        66500000

/* I2C write register function */
static inline void imx_i2c_wrr(const imx_dev_t* const dev, const uint8_t offset, const uint8_t val)
{
    if (dev->itype == S32_I2C) {
        out8(dev->regbase + (offset >> 2u), val);
    } else {
        out16(dev->regbase + offset, (uint16_t)val);
    }
}

/* I2C read register function */
static inline uint8_t imx_i2c_rdr(const imx_dev_t* const dev, const uint8_t offset)
{
    if (dev->itype == S32_I2C) {
        return in8(dev->regbase + (offset >> 2u));
    } else {
        return (uint8_t)in16(dev->regbase + offset);
    }
}

/* Function prototypes */
void *imx_init(int argc, char *argv[]);
void imx_fini(void *hdl);
i2c_status_t imx_recv(void *hdl, void *buf, unsigned int len, unsigned int stop);
i2c_status_t imx_send(void *hdl, void *buf, unsigned int len, unsigned int stop);
int imx_set_slave_addr(void *hdl, unsigned int addr, i2c_addrfmt_t fmt);
int imx_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed);
int imx_version_info(i2c_libversion_t *version);
int imx_driver_info(void *hdl, i2c_driver_info_t *info);

int imx_options(imx_dev_t *dev, int argc, char *argv[]);
i2c_status_t imx_wait_bus_not_busy(imx_dev_t *dev);
uint8_t imx_wait_complete(imx_dev_t *dev);
i2c_status_t imx_sendaddr7(imx_dev_t *dev, const unsigned addr, const uint8_t rw, const unsigned restart);
i2c_status_t imx_sendaddr10(imx_dev_t *dev, const unsigned addr, const uint8_t rw, const unsigned restart);
i2c_status_t imx_sendbyte(imx_dev_t *dev, uint8_t byte);
i2c_status_t imx_recvbyte(imx_dev_t *dev, uint8_t *byte, int nack, int stop);

#endif /* __I2C_IMX_PROTO_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/proto.h $ $Rev: 979323 $")
#endif
