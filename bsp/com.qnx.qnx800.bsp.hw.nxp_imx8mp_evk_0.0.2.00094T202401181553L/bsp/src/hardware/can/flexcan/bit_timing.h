/*
 * Copyright (c) 2020, 2023, BlackBerry Limited.
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

#ifndef _FLEXCAN_BIT_TIMING_H_
#define _FLEXCAN_BIT_TIMING_H_

#include <stdint.h>

struct flexcan_btm_const {
    int     tseg1_min;      /* Time segment 1 = prop_seg + phase_seg1 */
    int     tseg1_max;
    int     tseg2_min;      /* Time segment 2 = phase_seg2 */
    int     tseg2_max;
    int     sjw_max;        /* Synchronisation jump width */
    int     brp_min;        /* Bit-rate prescaler */
    int     brp_max;
    int     brp_inc;
};

struct flexcan_btm {
    int     freq;           /* CAN controller frequency */
    int     bitrate;        /* Bit-rate in bits/second */
    int     sample_point;   /* Sample point in one-tenth of a percent */
    int     tq;             /* Time quanta (TQ) in nanoseconds */
    int     prop_seg;       /* Propagation segment in TQs */
    int     phase_seg1;     /* Phase buffer segment 1 in TQs.TSEG1 = Prop_Seg + Phase_Seg1  */
    int     phase_seg2;     /* Phase buffer segment 2 in TQs. TSEG2 = Phase_Seg2 */
    int     sjw;            /* Synchronisation jump width in TQs */
    int     brp;            /* Bit-rate prescaler */
    int     pad;            /* Pad to 8 bytes alignment */
};

int flexcan_calc_bittiming(const struct flexcan_btm_const* const btc, struct flexcan_btm* const bt);
int flexcan_validate_bittime_params(const struct flexcan_btm_const* const btc, struct flexcan_btm* const bt);

#endif /* FLEXCAN_BIT_TIMING_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/can/flexcan/bit_timing.h $ $Rev: 977147 $")
#endif
