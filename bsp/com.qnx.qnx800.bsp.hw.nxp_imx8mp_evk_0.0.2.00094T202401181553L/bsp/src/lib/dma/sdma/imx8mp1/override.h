/*
 * Copyright 2020, 2022-2023, BlackBerry Limited.
 * Copyright 2019, 2022 NXP
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


#ifndef OVERRIDE_H
#define OVERRIDE_H

#include <soc/nxp/imx8/common/imx_sdma.h>

#define SDMA_REGISTER_MAP_VERSION2
#define SDMA_BASE 0x30BD0000
#define SDMA_SIZE (IMX_SDMA_SIZE)
#define SDMA_IRQ  (2 + 32)
#define SDRAM_BASE    0x40000000
#define SDMA_MUTEX_PATH "/SDMA1_MUTEX"
#define SDMA_DESCRIPTION_STR "i.MX SDMA1 Controller"
#define SDMA_INSTANCE   0

#define IMX8
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/sdma/imx8mp1/override.h $ $Rev: 980222 $")
#endif
