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

struct _i2c_clk_div {
    uint16_t    div;
    uint16_t    val;
};

static const struct _i2c_clk_div imx_i2c_clk_div[] = {
    { .div = 22,  .val = 0x20 }, { .div = 24,  .val = 0x21 }, { .div = 26,  .val = 0x22 }, { .div = 28,  .val = 0x23 },
    { .div = 30,  .val = 0x00 }, { .div = 32,  .val = 0x24 }, { .div = 36,  .val = 0x25 }, { .div = 40,  .val = 0x26 },
    { .div = 42,  .val = 0x03 }, { .div = 44,  .val = 0x27 }, { .div = 48,  .val = 0x28 }, { .div = 52,  .val = 0x05 },
    { .div = 56,  .val = 0x29 }, { .div = 60,  .val = 0x06 }, { .div = 64,  .val = 0x2A }, { .div = 72,  .val = 0x2B },
    { .div = 80,  .val = 0x2C }, { .div = 88,  .val = 0x09 }, { .div = 96,  .val = 0x2D }, { .div = 104, .val = 0x0A },
    { .div = 112, .val = 0x2E }, { .div = 128, .val = 0x2F }, { .div = 144, .val = 0x0C }, { .div = 160, .val = 0x30 },
    { .div = 192, .val = 0x31 }, { .div = 224, .val = 0x32 }, { .div = 240, .val = 0x0F }, { .div = 256, .val = 0x33 },
    { .div = 288, .val = 0x10 }, { .div = 320, .val = 0x34 }, { .div = 384, .val = 0x35 }, { .div = 448, .val = 0x36 },
    { .div = 480, .val = 0x13 }, { .div = 512, .val = 0x37 }, { .div = 576, .val = 0x14 }, { .div = 640, .val = 0x38 },
    { .div = 768, .val = 0x39 }, { .div = 896, .val = 0x3A }, { .div = 960, .val = 0x17 }, { .div = 1024,.val = 0x3B },
    { .div = 1152,.val = 0x18 }, { .div = 1280,.val = 0x3C }, { .div = 1536,.val = 0x3D }, { .div = 1792,.val = 0x3E },
    { .div = 1920,.val = 0x1B }, { .div = 2048,.val = 0x3F }, { .div = 2304,.val = 0x1C }, { .div = 2560,.val = 0x1D },
    { .div = 3072,.val = 0x1E }, { .div = 3840,.val = 0x1F }
};

static const struct _i2c_clk_div s32_i2c_clk_div[] = {
    { .div = 20,  .val = 0x00 }, { .div = 22,  .val = 0x01 }, { .div = 24,  .val = 0x02 }, { .div = 26,  .val = 0x03 },
    { .div = 28,  .val = 0x04 }, { .div = 30,  .val = 0x05 }, { .div = 32,  .val = 0x09 }, { .div = 34,  .val = 0x06 },
    { .div = 36,  .val = 0x0A }, { .div = 40,  .val = 0x07 }, { .div = 44,  .val = 0x0C }, { .div = 48,  .val = 0x0D },
    { .div = 52,  .val = 0x43 }, { .div = 56,  .val = 0x0E }, { .div = 60,  .val = 0x45 }, { .div = 64,  .val = 0x12 },
    { .div = 68,  .val = 0x0F }, { .div = 72,  .val = 0x13 }, { .div = 80,  .val = 0x14 }, { .div = 88,  .val = 0x15 },
    { .div = 96,  .val = 0x19 }, { .div = 104, .val = 0x16 }, { .div = 112, .val = 0x1A }, { .div = 128, .val = 0x17 },
    { .div = 136, .val = 0x4F }, { .div = 144, .val = 0x1C }, { .div = 160, .val = 0x1D }, { .div = 176, .val = 0x55 },
    { .div = 192, .val = 0x1E }, { .div = 208, .val = 0x56 }, { .div = 224, .val = 0x22 }, { .div = 228, .val = 0x24 },
    { .div = 240, .val = 0x1F }, { .div = 256, .val = 0x23 }, { .div = 288, .val = 0x5C }, { .div = 320, .val = 0x25 },
    { .div = 384, .val = 0x26 }, { .div = 448, .val = 0x2A }, { .div = 480, .val = 0x27 }, { .div = 512, .val = 0x2B },
    { .div = 576, .val = 0x2C }, { .div = 640, .val = 0x2D }, { .div = 768, .val = 0x31 }, { .div = 896, .val = 0x32 },
    { .div = 960, .val = 0x2F }, { .div = 1024,.val = 0x33 }, { .div = 1152,.val = 0x34 }, { .div = 1280,.val = 0x35 },
    { .div = 1536,.val = 0x36 }, { .div = 1792,.val = 0x3A }, { .div = 1920,.val = 0x37 }, { .div = 2048,.val = 0x3B },
    { .div = 2304,.val = 0x3C }, { .div = 2560,.val = 0x3D }, { .div = 3072,.val = 0x3E }, { .div = 3584,.val = 0x7A },
    { .div = 3840,.val = 0x3F }, { .div = 4096,.val = 0x7B }, { .div = 5120,.val = 0x7D }, { .div = 6144,.val = 0x7E },
};

/**
 *  @brief              Find best bus speed divisor
 *  @param  hdl         Driver handle
 *  @param  i2c_div     Desired divisor
 *
 *  @return             Best divisor
 */
static uint8_t imx_find_best_ic(const imx_dev_t * const dev, const unsigned int i2c_div)
{
    uint32_t    ic;
    uint32_t    ndiv;

#define CLKDIV_ELEMENTS(s) (sizeof(s) / sizeof(s[0]))

    if (dev->itype == S32_I2C) {
        ndiv = CLKDIV_ELEMENTS(s32_i2c_clk_div);
        if (i2c_div < s32_i2c_clk_div[0].div) return 0;
        if (i2c_div > s32_i2c_clk_div[ndiv - 1].div) return ndiv - 1;

        for (ic = 0; s32_i2c_clk_div[ic].div < i2c_div; ic++) ;

        /* Store divider value */
        return (uint8_t)s32_i2c_clk_div[ic].val;
    } else {
        ndiv = CLKDIV_ELEMENTS(imx_i2c_clk_div);
        if (i2c_div < imx_i2c_clk_div[0].div) return 0;
        if (i2c_div > imx_i2c_clk_div[ndiv - 1].div) return ndiv - 1;

        for (ic = 0; imx_i2c_clk_div[ic].div < i2c_div; ic++) ;

        /* Store divider value */
        return (uint8_t)imx_i2c_clk_div[ic].val;
    }
}

/**
 *  @brief              Set I2C bus speed
 *  @param  hdl         Driver handle
 *  @param  speed       Bus speed to be set
 *  @param  ospeed      Pointer to speed been set
 *
 *  @return             0  -- success
 *                      -1 -- failure, unsupported bus speed
 */
int imx_set_bus_speed(void *hdl, unsigned int speed, unsigned int *ospeed)
{
    imx_dev_t      *dev = hdl;
    unsigned int   i2c_div;
    uint8_t        i2c_freq_val;

    if (speed > 400000) {
        errno = EINVAL;
        return -1;
    }

    if (speed != dev->speed) {
        i2c_div = (dev->input_clk + speed - 1) / speed;
        i2c_freq_val = imx_find_best_ic(dev, i2c_div);
        imx_i2c_wrr(dev, IMX_I2C_FRQREG_OFF, i2c_freq_val);

        /* Save the speed, next time we don't have to recalculate */
        dev->speed = speed;
        dev->i2c_freq_val = i2c_freq_val;
    }

    if (ospeed) {
        if (dev->itype == S32_I2C) {
            *ospeed = dev->input_clk / s32_i2c_clk_div[dev->i2c_freq_val].div;
        } else {
            *ospeed = dev->input_clk / imx_i2c_clk_div[dev->i2c_freq_val].div;
        }

    }

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/bus_speed.c $ $Rev: 979323 $")
#endif

