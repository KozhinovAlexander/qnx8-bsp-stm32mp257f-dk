/*
 * Copyright (c) 2013, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, 2020, NXP
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
 *
 */

#ifndef _SIM_BS_H_INCLUDED
#define _SIM_BS_H_INCLUDED


/**
 * Board specific interface
 *
 * @file       sdmmc/aarch64/mx8x.le/sim_bs.h
 * @addtogroup sdmmc_bs
 * @{
 */

extern int sim_bs_args( SIM_HBA *hba, char *options );

#define SDMMC_CACHE_SUP
#define SIM_BS_DEVCTL

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devb/sdmmc/aarch64/mx8x.le/sim_bs.h $ $Rev: 976550 $")
#endif
