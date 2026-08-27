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


#ifndef IMX_I2C_DRV_H_
#define IMX_I2C_DRV_H_

typedef struct {
    unsigned int base;
    unsigned short div;
    unsigned short slave;
} imx_i2c_dev_t;


#define I2C_AR                      0x0
#define I2C_IFDR                    0x4
#define I2C_I2CR                    0x8
#define I2C_I2SR                    0xC
#define I2C_I2DR                    0x10

#define I2C_I2CR_IEN                (1 << 7)
#define I2C_I2CR_IIEN               (1 << 6)
#define I2C_I2CR_MSTA               (1 << 5)
#define I2C_I2CR_MTX                (1 << 4)
#define I2C_I2CR_TXAK               (1 << 3)
#define I2C_I2CR_RSTA               (1 << 2)

#define I2C_I2SR_ICF                (1 << 7)
#define I2C_I2SR_IAAS               (1 << 6)
#define I2C_I2SR_IBB                (1 << 5)
#define I2C_I2SR_IAL                (1 << 4)
#define I2C_I2SR_SRW                (1 << 2)
#define I2C_I2SR_IIF                (1 << 1)
#define I2C_I2SR_RXAK               (1 << 0)

#define I2C_WRITE                   0
#define I2C_READ                    1
#define WAIT_RXAK_LOOPS             1000000

#define I2C_WAIT_CNT                10000

int i2c_write(imx_i2c_dev_t * const dev, const unsigned char reg, unsigned char* const val);
int i2c_read(imx_i2c_dev_t * const dev, const unsigned char reg, unsigned char* const val);
void init_i2c_bus(imx_i2c_dev_t * const dev);

#endif  /* IMX_I2C_DRV_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx_i2c_drv.h $ $Rev: 958436 $")
#endif
