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
 *  @brief              Parse I2C command line options
 *  @param  dev         I2C device handle
 *  @param  argc        Argument count
 *  @param  argv        Argument strings
 *
 *  @return             0  -- success
 *                      -1 -- failure
 */
int imx_options(imx_dev_t *dev, int argc, char *argv[])
{
    int      c;
    int      prev_optind;
    int      done = 0;
    int      tcmd = _NTO_TCTL_IO;

    /* Set defaults */
    dev->physbase   = 0;
    dev->reglen     = IMX_I2C_REGLEN;
    dev->intr       = -1;
    dev->iid        = -1;
    dev->itype      = IMX_I2C;
    dev->restart    = 0;
    dev->slave_addr = 0;
    dev->input_clk  = IMX_I2C_INPUT_CLOCK;
    dev->verbosity  = _SLOG_ERROR;

    while (!done) {
        prev_optind = optind;
        c = getopt(argc, argv, "c:i:lp:s:t:v");
        switch (c) {
        case 'c':
            dev->input_clk = (unsigned)strtoul(optarg, &optarg, 0);
            break;

        case 'i':
            dev->intr = (int)strtol(optarg, &optarg, 0);
            break;

        case 'l':
            tcmd = _NTO_TCTL_IO_PRIV;
            break;

        case 'p':
            dev->physbase = strtoul(optarg, &optarg, 0);
            break;

        case 's':
            dev->slave_addr = (unsigned)strtoul(optarg, &optarg, 0);
            break;

        case 't':
            dev->itype = (uint8_t)strtol(optarg, &optarg, 0);
            if ((dev->itype != IMX_I2C) && ((dev->itype != S32_I2C))) {
                i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: Invalid itype. Has to be 0 or 1", __func__);
                errno = EINVAL;
                return -1;
            }
            break;

        case 'v':
            dev->verbosity++;
            break;

        case '?':
            if (optopt == '-') {
                ++optind;
                break;
            }
            return -1;

        case -1:
            if (prev_optind < optind) { /* -- */
                return -1;
            }

            if (argv[optind] == NULL) {
                done = 1;
                break;
            }
            if (*argv[optind] != '-') {
                ++optind;
                break;
            }
            return -1;

        case ':':
        default:
            return -1;
        }
    }

    /* I2C Base addresss or irq was not provided by command line opts */
    if ((dev->physbase == 0) || (dev->intr == -1)) {
        i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: must specify both base address and IRQ", __func__);
        errno = EINVAL;
        return -1;
    }

    if (ThreadCtl(tcmd, NULL) == -1) {
        i2c_slogf(dev->verbosity, _SLOG_ERROR, "%s: ThreadCtl failed: (%s)", __func__, strerror(errno));
        return -1;
    }

    return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/i2c/imx/options.c $ $Rev: 981005 $")
#endif
