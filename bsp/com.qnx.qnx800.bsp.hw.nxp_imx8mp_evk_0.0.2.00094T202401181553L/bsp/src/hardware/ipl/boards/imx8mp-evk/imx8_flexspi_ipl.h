/*
 * $QNXLicenseC:
 * Copyright 2021, 2022 BlackBerry Limited.
 * Copyright 2017-2019 NXP
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

#ifndef IMX_FLEXSPI_IPL_H_
#define IMX_FLEXSPI_IPL_H_

#include <stdint.h>

#define FLEXSPI_MAX_BURST_RX            128

#define FLEXSPI_OK                      0

typedef struct _flexspi_conf {
    unsigned        rx_fifo_size;       /* Receive FIFO size */
    unsigned        pads;               /* PADs number */
    unsigned        smpl;               /* Sample clock source */
    uint32_t        lut0;               /* Lookup table 0 */
    uint32_t        lut1;               /* Lookup table 1 */
    uint32_t        lut2;               /* Lookup table 2 */
    uint32_t        lut3;               /* Lookup table 3 */
} flexspi_conf_t;

typedef struct _flexspi {
    unsigned        pbase;              /* Base address */
    flexspi_conf_t  *conf;
    unsigned        verbose;            /* Verbose level */
} flexspi_t;

/* API */
void imx8_flexspi_init(flexspi_t *flexspi, unsigned base, int verbose);
int imx8_flexspi_read(flexspi_t *flexspi, unsigned long offset, unsigned long buffer, unsigned size);

#endif /* IMX_FLEXSPI_IPL_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx8_flexspi_ipl.h $ $Rev: 958436 $")
#endif
