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


#include "flexcan.h"
#include "bit_timing.h"

#ifdef DEBUG_DRVR
static void print_bittime_params(struct can_bittiming *bt);
#endif

#define CAN_CALC_MAX_ERROR 50 /* in one-tenth of a percent */

static int flexcan_update_spt(const struct flexcan_btm_const* const btc,
                const int sampl_pt, const int tseg, int* const tseg1, int* const tseg2)
{
    *tseg2 = tseg + 1 - (sampl_pt * (tseg + 1)) / 1000;

    if (*tseg2 < btc->tseg2_min) {
        *tseg2 = btc->tseg2_min;
    }

    if (*tseg2 > btc->tseg2_max) {
        *tseg2 = btc->tseg2_max;
    }

    *tseg1 = tseg - *tseg2;
    if (*tseg1 > btc->tseg1_max) {
        *tseg1 = btc->tseg1_max;
        *tseg2 = tseg - *tseg1;
    }

    return 1000 * (tseg + 1 - *tseg2) / (tseg + 1);
}

int flexcan_calc_bittiming(const struct flexcan_btm_const* const btc, struct flexcan_btm* const bt)
{
    long        rate = 0;
    long        best_error = 1000000000l, error = 0;
    int         best_tseg = 0, best_brp = 0, brp = 0;
    int         tsegall, tseg = 0, tseg1 = 0, tseg2 = 0;
    long        spt_error = 1000l, spt = 0, sampl_pt;
    uint64_t    v64;

    /* Use CIA recommended sample points */
    if (bt->sample_point) {
        sampl_pt = bt->sample_point;
    } else {
        if (bt->bitrate > 800000) {
            sampl_pt = 750;
        } else if (bt->bitrate > 500000) {
            sampl_pt = 800;
        } else {
            sampl_pt = 875;
        }
    }

    /* tseg even = round down, odd = round up */
    for (tseg = (btc->tseg1_max + btc->tseg2_max) * 2 + 1;
            tseg >= (btc->tseg1_min + btc->tseg2_min) * 2; tseg--) {
        tsegall = 1 + tseg / 2;
        /* Compute all possible tseg choices (tseg=tseg1+tseg2) */
        brp = bt->freq / (tsegall * bt->bitrate) + tseg % 2;
        /* chose brp step which is possible in system */
        brp = (brp / btc->brp_inc) * btc->brp_inc;
        if ((brp < btc->brp_min) || (brp > btc->brp_max)) continue;
        rate = bt->freq / (brp * tsegall);
        error = bt->bitrate - rate;
        /* tseg brp biterror */
        if (error < 0) {
            error = -error;
        }
        if (error > best_error) continue;
        best_error = error;
        if (error == 0) {
            spt = (long)flexcan_update_spt(btc, (int)sampl_pt, tseg / 2, &tseg1, &tseg2);
            error = sampl_pt - spt;
            if (error < 0) {
                error = -error;
            }
            if (error > spt_error) continue;
            spt_error = error;
        }
        best_tseg = tseg / 2;
        best_brp = brp;
        if (error == 0) break;
    }

    if (best_error) {
        /* Error in one-tenth of a percent */
        error = (best_error * 1000l) / (long)bt->bitrate;
        if (error > CAN_CALC_MAX_ERROR) {
            can_slogf(_SLOG_ERROR, "%s():bitrate error %ld.%ld%% too hig", __func__, error / 10, error % 10 );
            return -1;
        } else {
            can_slogf(_SLOG_INFO, "%s():bitrate error %ld.%ld%%", __func__, error / 10, error % 10 );
        }
    }

    /* real sample point */
    bt->sample_point = flexcan_update_spt(btc, (int)sampl_pt, best_tseg, &tseg1, &tseg2);

    v64 = best_brp * 1000000000UL;
    v64 /= (uint64_t)bt->freq;
    bt->tq = (int)v64;
    bt->prop_seg = tseg1 / 2;
    bt->phase_seg1 = tseg1 - bt->prop_seg;
    bt->phase_seg2 = tseg2;

    /* check for SJW user settings */
    if (!bt->sjw || !btc->sjw_max) {
        bt->sjw = 1;
    } else {
        /* bt->sjw is at least 1 -> sanitize upper bound to sjw_max */
        if (bt->sjw > btc->sjw_max) {
            bt->sjw = btc->sjw_max;
        }
        /* bt->sjw must not be higher than tseg2 */
        if (tseg2 < bt->sjw) {
            bt->sjw = tseg2;
        }
    }

    bt->brp = best_brp;

    /* real bit-rate */
    bt->bitrate = bt->freq / (bt->brp * (tseg1 + tseg2 + 1));

#ifdef DEBUG_DRVR
    fprintf(stderr, "result:\n");
    print_bittime_params(bt);
#endif

    return 0;
}
/*
 * Checks the validity of the specified bit-timing parameters tseg1,
 * tseg2 and sjw and tries to determine the bitrate prescaler value brp.
 * Input:  tseg1, tseg2, sjw, frequency, time_quantum
 * Output: brp, bitrate, sample_point
 */
int flexcan_validate_bittime_params(const struct flexcan_btm_const* const btc, struct flexcan_btm* const bt)
{
    int         tseg1, alltseg;
    uint64_t    v64;

    tseg1 = bt->prop_seg + bt->phase_seg1;

    /* Check incoming parameters range */
    if (bt->sjw == 0) {
        bt->sjw = 1;
    }
    if ((tseg1 > btc->tseg1_max) || (tseg1 < btc->tseg1_min)
            || (bt->phase_seg2 > btc->tseg2_max) || (bt->phase_seg2 < btc->tseg2_min)
            || (bt->sjw > btc->sjw_max)) return -1;

    if ((bt->brp > btc->brp_max) || (bt->brp < btc->brp_min)) return -1;

    v64 = (uint64_t)bt->brp * 1000000000UL;
    v64 /= (uint64_t)bt->freq;
    bt->tq = (int)v64;

    alltseg = bt->prop_seg + bt->phase_seg1 + bt->phase_seg2 + 1;
    bt->bitrate = bt->freq / (bt->brp * alltseg);
    bt->sample_point = (1000 * (tseg1 + 1)) / alltseg;

    return 0;
}

#ifdef DEBUG_DRVR
static void print_bittime_params(struct can_bittiming *bt)
{
    fprintf(stderr, "bt->freq = %d\n", bt->freq);
    fprintf(stderr, "bt->bitrate = %d\n", bt->bitrate);
    fprintf(stderr, "bt->sample_point = 0x%x\n", bt->sample_point);
    fprintf(stderr, "bt->tq = 0x%x\n", bt->tq);
    fprintf(stderr, "bt->prop_seg = 0x%x\n", bt->prop_seg);
    fprintf(stderr, "bt->phase_seg1 = 0x%x\n", bt->phase_seg1);
    fprintf(stderr, "bt->phase_seg2 = 0x%x\n", bt->phase_seg2);
    fprintf(stderr, "bt->sjw = 0x%x\n", bt->sjw);
    fprintf(stderr, "bt->brp = 0x%x\n", bt->brp);
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/can/flexcan/bit_timing.c $ $Rev: 977147 $")
#endif
