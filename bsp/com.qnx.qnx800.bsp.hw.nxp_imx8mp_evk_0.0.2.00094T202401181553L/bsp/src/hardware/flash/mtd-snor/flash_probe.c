/*
 * Copyright (c) 2022, BlackBerry Limited.
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


/*
** File: f3s_flash_probe.c
**
** Description:
**
** This file contains the array probe function for the f3s flash file system
**
*/

/*
** Includes
*/

#include "f3s_snor.h"

static int _flash_build_dbase(f3s_access_t* const access, const f3s_dbase_t* const dbase, f3s_dbase_t* const cdbase, const int numc);

/**
 *  @brief             This function is what is it says it is, a probe function.
 *                     Unless the configuration has already been set by the user the
 *                     function will loop through all idents (trying every configuration)
 *                     passed in through the main function until it finds one that matches.
 *  @param access      Pointer to flash access structure.
 *  @param flash_ptr   Pointer to flash array.
 *  @param dbase       Pointer to flash data base.
 *
 *  @return            1 --success otherwise(negative number) fail.
 */
int32_t f3s_flash_probe(f3s_access_t *access, f3s_flash_t **flash_ptr, f3s_dbase_t *dbase)
{
    snor_ctrl_t     *ctrl = (snor_ctrl_t *)access->socket.memory;
    snor_chip_t     *chip;
    f3s_flash_v2_t  *flash;
    f3s_dbase_t     cdbase[SNOR_MAX_CS] = {{0,},};
    uint8_t         cs = 0;
    _int32          error = EOK;
    uint8_t         chip_total = 0;
    uint32_t        sidx, bw;
    int             found;
    const uint32_t  busproto[] = { SNOR_BUSPROTO_1_1_1,
                                   SNOR_BUSPROTO_2_2_2,
                                   SNOR_BUSPROTO_4_4_4,
                                   SNOR_BUSPROTO_8_8_8,
                                   SNOR_BUSPROTO_1_1_1_DTR,
                                   SNOR_BUSPROTO_2_2_2_DTR,
                                   SNOR_BUSPROTO_4_4_4_DTR,
                                   SNOR_BUSPROTO_8_8_8_DTR,
                                   SNOR_BUSPROTO_HYPER };

    if (ctrl->funcs.cfg_bus == NULL) return (-ENOTSUP);

    while ((cs < ctrl->ncs) && (error == EOK)) {
        chip = &ctrl->chip[cs];
        chip->cfg.cs = cs;
        ctrl->ccs = cs;

        found = 0;
        flash = (f3s_flash_v2_t *)(*flash_ptr);
        while (flash->struct_size == sizeof(f3s_flash_v2_t)) {
            for (sidx = 0; sidx < sizeof(busproto) / sizeof(busproto[0]); sidx++) {
                bw = (busproto[sidx] >> SNOR_BUSPROTO_CBSHIFT) & 0x0F;
                if (busproto[sidx] == SNOR_BUSPROTO_HYPER) {
                    if (!(ctrl->hcaps & SNOR_HCAPS_HYPER)) break;
                } else {
                    if (busproto[sidx] & SNOR_BUSPROTO_DTR_MODE) {
                        if (!(ctrl->hcaps & SNOR_HCAPS_DTR)) break;
                    }
                    if ((bw == 1) && !(ctrl->hcaps & SNOR_HCAPS_RD_1_1_1)) continue;
                    if ((bw == 2) && !(ctrl->hcaps & SNOR_HCAPS_RD_2_2_2)) continue;
                    if ((bw == 4) && !(ctrl->hcaps & SNOR_HCAPS_RD_4_4_4)) continue;
                    if ((bw == 8) && !(ctrl->hcaps & SNOR_HCAPS_RD_8_8_8)) continue;
                }

                chip->cfg.bus_proto = busproto[sidx];
                snor_slogf(_SLOG_INFO, ctrl->verbosity, 5, "%s: set bus proto to 0x%x", __func__, chip->cfg.bus_proto);
                if (ctrl->funcs.cfg_bus(ctrl, &chip->cfg) != EOK) break;

                flash->reset(&cdbase[cs], access, SNOR_FLG_FORCECS, cs);

                error = flash->ident(&cdbase[cs], access, 0, cs);
                if (error == EOK) {
                    chip->hcaps &= chip->hcmask;
                    if (!(chip->hcaps & SNOR_HCAPS_RD_MASK)) {
                        // can't read the flash, so not supported
                        break;
                    }
                    // one flash device per chip select
                    chip_total++;
                    chip->flash = flash;
                    found = 1;
                    (*flash_ptr) = (f3s_flash_t*)flash;
                    break;
                }
            }

            if (found != 0) break;
            // go to next flash chip
            flash = (f3s_flash_v2_t *)((uint8_t*)flash + (*flash_ptr)->struct_size);
        }

        cs++;
    }

    if ((error != EOK) || (chip_total == 0)) {
        snor_slogf(_SLOG_ERROR, 0, 0,
                "(devf  t%d::%s:%d) Unable to properly identify any flash devices",
                pthread_self(), __func__, __LINE__);
        return (-ENOTSUP);
    } else {
        snor_slogf(_SLOG_INFO, ctrl->verbosity, 2,
                "(devf  t%d::%s:%d) Total %d flash devices identified",
                pthread_self(), __func__, __LINE__, chip_total);
    }

    // chip count must be power of two
    if ((chip_total != 1) && (chip_total != 2) && (chip_total != 4) && (chip_total != 8)) {
        snor_slogf(_SLOG_ERROR, 0, 0,
                "(devf  t%d::%s) chip number %d is not power of 2", pthread_self(), __func__, chip_total);
        return (-ENOTSUP);
    }

    // Stripe mode needs more than one chip
    if ((ctrl->flags & SNOR_FLG_STRIPE) && (chip_total < 2)) {
        snor_slogf(_SLOG_ERROR, 0, 0,
                "(devf  t%d::%s:%d) Need 2, 4 or 8 chips to support stripe mode",
                pthread_self(), __func__, __LINE__);
        return (-ENOTSUP);
    }

    // need to build chip geometries if they are not set
    if (cdbase[0].geo_num == 0) {
        if (_flash_build_dbase(access, dbase, cdbase, (int)chip_total) != EOK) return (-ENOTSUP);
    }

    // post ident for each chip
    uint32_t    dcaps = 0xFFFFFFFF;
    chip = &ctrl->chip[0];
    for (cs = 0; cs < chip_total; cs++) {
        chip->hcaps &= ctrl->hcaps;
        dcaps &= chip->dcaps & chip->dcmask;
        error = chip->post_ident(chip);
        if (error != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0,
                "(devf  t%d::%s:%d) flash configure failed", pthread_self(), __func__, __LINE__);
            return (-ENOTSUP);
        }
        chip++;
    }

    if (dcaps & SNOR_DCAPS_ESR) {
        dbase->flags |= F3S_ERASE_FOR_READ;
    }

    // final setup
    dbase->chip_inter  = 1;  // To make the file system happy
    dbase->chip_width  = 1;  // To make the file system happy
    dbase->struct_size = sizeof(*dbase);
    dbase->buffer_size = ctrl->chip[0].pagesz;  // use chip 0 buffer size
    dbase->name        = cdbase[0].name;

    // copy the first device data base
    dbase->geo_num = cdbase[0].geo_num;
    for (uint32_t ng = 0; ng < dbase->geo_num; ng++) {
        dbase->geo_vect[ng].unit_num  = cdbase[0].geo_vect[ng].unit_num;
        dbase->geo_vect[ng].unit_pow2 = cdbase[0].geo_vect[ng].unit_pow2;
    }

    if (ctrl->flags & SNOR_FLG_STRIPE) {
        // stripe mode sanity check
        for (uint8_t nc = 1; (nc < chip_total) && (error == EOK); nc++) {
            if (cdbase[nc].geo_num != cdbase[0].geo_num) {
                error = -ENOTSUP;
                break;
            }
            for (uint32_t geo = 0; geo < cdbase[nc].geo_num; geo++) {
                if ((cdbase[nc].geo_vect[geo].unit_num != cdbase[0].geo_vect[geo].unit_num) ||
                    (cdbase[nc].geo_vect[geo].unit_pow2 != cdbase[0].geo_vect[geo].unit_pow2)) {
                    error = -ENOTSUP;
                    break;
                }
            }
        }
        if (error != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0,
                    "(devf  t%d::%s:%d) Need identical chip to support stripe mode",
                        pthread_self(), __func__, __LINE__);
            return (error);
        }

        snor_slogf(_SLOG_INFO, ctrl->verbosity, 2,
                "(devf  t%d::%s:%d) stripe mode selected", pthread_self(), __func__, __LINE__);

        // stripe mode tweak
        ctrl->chip[0].align  *= (uint16_t)chip_total;
        ctrl->chip[0].pagesz *= (uint16_t)chip_total;
        dbase->buffer_size   *= (uint16_t)chip_total;
        dbase->chip_inter    *= (uint16_t)chip_total;
        for (uint32_t ng = 0; ng < dbase->geo_num; ng++) {
            if (chip_total == 8) {
                dbase->geo_vect[ng].unit_pow2 += 3;
            } else {
                dbase->geo_vect[ng].unit_pow2 += (uint16_t)(chip_total >> 1);
            }
        }

        uint8_t stripe = 0;
        for (cs = 0; cs < SNOR_MAX_CS; cs++) {
            if (ctrl->chip[cs].chipsz != 0) {
                stripe |= 1 << cs;  // chip select bit field
            }
        }
        ctrl->chip[0].rdcfg.stripe = stripe;
        ctrl->chip[0].wrcfg.stripe = stripe;
        ctrl->chip[0].cfg.stripe   = stripe;
        ctrl->chip[0].rdcfg.ncs    = chip_total;
        ctrl->chip[0].wrcfg.ncs    = chip_total;
        ctrl->chip[0].cfg.ncs      = chip_total;
    } else {
        // we combine all chips to one
        for (uint8_t nc = 1; nc < chip_total; nc++) {
            for (uint32_t geo = 0; geo < cdbase[nc].geo_num; geo++) {
                dbase->geo_vect[dbase->geo_num].unit_num  = cdbase[nc].geo_vect[geo].unit_num;
                dbase->geo_vect[dbase->geo_num].unit_pow2 = cdbase[nc].geo_vect[geo].unit_pow2;
                if (++dbase->geo_num > SNOR_NELEMS(dbase->geo_vect)) return (-ENOTSUP);
            }
        }
    }

    // For now, we always return one chip
    return (1);
}

/**
 *  @brief             Build flash data base.
 *  @param access      Pointer to flash access structure.
 *  @param dbase       Pointer to flash data base.
 *  @param cdbase      Pointer to source flash data base.
 *  @param numc        Number of chips.
 *
 *  @return            EOK --success otherwise fail.
 */
static int _flash_build_dbase(f3s_access_t* const access, const f3s_dbase_t* const dbase, f3s_dbase_t* const cdbase, const int numc)
{
    snor_ctrl_t *ctrl = (snor_ctrl_t *)access->socket.memory;
    uint32_t    unit_size;
    int         chip_ok;
    int         unit_pow2 = 0;
    int         es, cs;

#if 0
    // we need to check whether top/bootm boot sector is supported when dynamic lock is supported.
    // unfortunately F3S_PROTECT_DYN/F3S_PROTECT_PERSISTENT bits are not visiable for now
    if (dbase->flags & (F3S_PROTECT_DYN | F3S_PROTECT_PERSISTENT) ) {
        // TODO, check whether all chips support lock
        unit_size = ctrl->chip[0].lksz_pow2 == 0 ? (1 << SNOR_SZPOW2_LOCK) : (1 << ctrl->chip[0].lksz_pow2);
        if ((access->socket.unit_size > 0) && (unit_size != access->socket.unit_size)) {
            snor_slogf(_SLOG_ERROR, 0, 0,
                    "(devf  t%d::%s:%d) sector size[%x] is not lockable sector size[%x]",
                        pthread_self(), __func__, __LINE__, access->socket.unit_size, unit_size);
        }
    } else {
        unit_size = access->socket.unit_size ?: (1 << SNOR_SZPOW2_MAX);
    }
#else
    if (access->socket.unit_size > 0) {
        unit_size = access->socket.unit_size;
    } else {
        unit_size = 1u << SNOR_SZPOW2_MAX;
    }
#endif

    // search a sector size that's supported by all the chips
    do {
        chip_ok = 0;
        for (cs = 0; cs < numc; cs++) {
            for (es = 0; es < SNOR_MAX_ERSCFG; es++) {
                if (unit_size == (1u << ctrl->chip[cs].blkers[es].blksz_pow2)) {
                    // found matching sector size
                    chip_ok++;
                    unit_pow2 = (int)ctrl->chip[cs].blkers[es].blksz_pow2;
                    ctrl->chip[cs].sectsz = unit_size;
                    break;      // move to next chip
                }
            }
        }
        if (chip_ok == numc) break;
        if (access->socket.unit_size != 0) break;
        unit_size >>= 1;
    } while (unit_size >= (1u << SNOR_SZPOW2_MIN));

    // sector size needs to be supported by all chips
    if (chip_ok != numc) {
        snor_slogf(_SLOG_ERROR, 0, 0,
            "(devf  t%d::%s:%d) sector size %d is not supported",
                pthread_self(), __func__, __LINE__, access->socket.unit_size);
        return (-ENOTSUP);
    }

    for (cs = 0; cs < numc; cs++) {
        cdbase[cs].geo_num = 1;
        cdbase[cs].geo_vect[0].unit_pow2 = (uint16_t)unit_pow2;
        cdbase[cs].geo_vect[0].unit_num  = (uint16_t)(ctrl->chip[cs].chipsz / unit_size);
#if 0
        // F3S_PROTECT_DYN bit is not visiable
        if (dbase->flags & F3S_PROTECT_DYN) {
                // dynamic lockable boot sector size is 4KB
            if (ctrl->chip[cs].flags & SNOR_CFLG_DLBBS) {
                cdbase[cs].geo_vect[1].unit_pow2 = cdbase[cs].geo_vect[0].unit_pow2;
                cdbase[cs].geo_vect[1].unit_num  = cdbase[cs].geo_vect[0].unit_num - 1;
                cdbase[cs].geo_vect[0].unit_pow2 = SNOR_SZPOW2_MIN;
                cdbase[cs].geo_vect[0].unit_num  = unit_size / (1 << SNOR_SZPOW2_MIN);
                cdbase[cs].geo_num++;
            }
            if (ctrl->chip[cs].flags & SNOR_CFLG_DLTBS) {
                cdbase[cs].geo_vect[cdbase->geo_num].unit_pow2 = SNOR_SZPOW2_MIN;
                cdbase[cs].geo_vect[cdbase->geo_num].unit_num  = unit_size / (1 << SNOR_SZPOW2_MIN);
                cdbase[cs].geo_vect[cdbase->geo_num - 1].unit_num--;
                cdbase[cs].geo_num++;
            }
        }
#endif
    }

    return (EOK);
}

/**
 *  @brief             Retrieve driver build information.
 *
 *  @return            Pointer to build information.
 */
const char *f3s_mtd_buildInfo(void)
{
    static const char build_info[] = "MTD Build: " __DATE__", " __TIME__;
    return (build_info);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
