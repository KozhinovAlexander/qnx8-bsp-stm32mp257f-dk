/*
 * Copyright (c) 2019, 2022-2023, BlackBerry Limited.
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


#ifndef __CPU_IPL_H_INCLUDED
#define __CPU_IPL_H_INCLUDED

/* Fast memcpy for large bulk transfers */
extern void    memcpy_bulk(void *dst, const void *src, size_t size);

extern void    armv8gt_init(const uint64_t clock);

/* iMX8 specific drivers */
extern void    imx_init_lpuart(const unsigned long port, const unsigned baud, const unsigned clk, unsigned osr);
extern void    imx_flexspi_init(unsigned long base, unsigned octal, unsigned smpl, const unsigned *seqs);
extern int     imx_flexspi_read(unsigned long base, const unsigned *seqs, unsigned offset, unsigned long buffer, unsigned size);


#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/cpu_ipl.h $ $Rev: 975396 $")
#endif
