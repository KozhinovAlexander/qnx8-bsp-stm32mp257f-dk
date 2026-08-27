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

#include "f3s_snor.h"

/*
 * This is the lock related callouts for SPI serial NOR flash.
 */

/**
 *  @brief             Unlock all callout for SPI serial NOR flash.
 *                     Only valid for persistent protection
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Unlock flags.
 *  @param offset      Unlock offset.
 *
 *  @return            EOK --success otherwise fail.
 */
int f3s_snor_unlockall(f3s_dbase_t *dbase,
                       f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    int         tmc;
    int         err = EOK;
    int         cs;

    /* check that this chip supports Persistent protection. */
    if (!(dbase->flags & F3S_PROTECT_PERSISTENT)) return (ENOTSUP);

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    ctrl = chip->ctrl;

    for (cs = 0; cs < SNOR_MAX_CS; cs++) {
        chip = &ctrl->chip[cs];
        if (chip->chipsz > 0) {
            if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
            if (snor_write_cmd(chip, chip->op_ppbe ? chip->op_ppbe : (uint8_t)SNOR_CMD_PPBE) != EOK) return (EIO);

            for (tmc = 0; tmc < 1000; tmc++) {
                err = chip->flash->v2sync(dbase, access, flags, offset);
                if (err != EAGAIN) break;
                delay(1);
            }
            if (err != EOK) break;
            if (ctrl->flags & SNOR_FLG_STRIPE) break;
        }
    }

    return (err);
}

/**
 *  @brief             Lock status check callout for SPI serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Islock flags.
 *  @param offset      Islock offset.
 *
 *  @return            EOK -- flash not locked
 *                     EROFS -- flash locked(read only)
 *                     Otherwise errors
 */
int f3s_snor_islock(f3s_dbase_t *dbase,
                    f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    uint32_t    lsize;
    uint8_t     opcode;
    uint8_t     adrlen, nsr, sr;
    uint8_t     locks[SNOR_MAX_CS];

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    ctrl = chip->ctrl;
    if (ctrl->flags & SNOR_FLG_STRIPE) {
        nsr = chip->cfg.ncs;
    } else {
        nsr = 1;
    }

    lsize = (chip->lksz_pow2 == 0) ? (1u << SNOR_SZPOW2_LOCK) : (1u << chip->lksz_pow2);
    if ((chip->offset % lsize) != 0) return (EINVAL);

    /* check that this chip supports dynamic protection. */
    if (dbase->flags & F3S_PROTECT_DYN) {
        if (chip->op_rdlock.opcode == 0) {
            opcode = SNOR_CMD_4DYBRD;
            adrlen = 4;
        } else {
            opcode = chip->op_rdlock.opcode;
            adrlen = chip->op_rdlock.adrlen;
        }
        // TODO
        // support boot sector
        if (snor_read_register(chip, opcode, chip->offset, adrlen, locks, (int)nsr) != EOK) return (EIO);
        if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
            ctrl->funcs.dstripe(ctrl, locks, nsr, 0);
        }
        for (sr = 0; sr < nsr; sr++) {
            if ((uint8_t)(locks[sr] & chip->lock_m) == chip->lock_v) return (EROFS);
        }
    }

    /* The DYB bits aren't locked, check the PPB bits */
    if (dbase->flags & F3S_PROTECT_PERSISTENT) {
        if (chip->op_rplock.opcode == 0) {
            opcode = SNOR_CMD_4PPBRD;
            adrlen = 4;
        } else {
            opcode = chip->op_rplock.opcode;
            adrlen = chip->op_rplock.adrlen;
        }
        if (snor_read_register(chip, opcode, chip->offset, adrlen, locks, (int)nsr) != EOK) return (EIO);
        if ((nsr > 1) && (ctrl->funcs.dstripe != NULL)) {
            ctrl->funcs.dstripe(ctrl, locks, nsr, 0);
        }
        for (sr = 0; sr < nsr; sr++) {
            if (!(locks[sr] & 0x01)) return (EROFS);
        }
    }

    return (EOK);
}

/**
 *  @brief             Write dynamic lock register
 *  @param chip        Pointer to chip structure.
 *  @param offset      Write offset.
 *  @param lb          Buffer pointer to lock bytes.
 *  @param nb          Number of lock bytes.
 *
 *  @return            EOK --success otherwise fail.
 */
static int _snor_dyb_write(snor_chip_t* const chip, const uint32_t offset, uint8_t* const lb, const uint8_t nb)
{
    uint8_t opcode, adrlen;

    if (chip->op_dlock.opcode == 0) {
        opcode = SNOR_CMD_4DYBWR;
        adrlen = 4;
    } else {
        opcode = chip->op_dlock.opcode;
        adrlen = chip->op_dlock.adrlen;
    }

    if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
    return snor_write_register(chip, opcode, offset, adrlen, lb, (int)nb);
}

/**
 *  @brief             Unlock callout for SPI serial NOR flash.
 *                     Only valid for dynamic protection
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Unlock flags.
 *  @param offset      Unlock offset.
 *
 *  @return            EOK --success otherwise fail.
 */
int f3s_snor_unlock(f3s_dbase_t *dbase,
                    f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    uint32_t    lsize, ssize;
    uint8_t     nc, n;
    uint8_t     ul[SNOR_MAX_CS];

    if (!(dbase->flags & F3S_PROTECT_DYN)) return (ENOTSUP);

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    lsize = (chip->lksz_pow2 == 0) ? (1u << SNOR_SZPOW2_LOCK) : (1u << chip->lksz_pow2);
    if ((chip->offset % lsize) != 0) return (EINVAL);

    ctrl = chip->ctrl;
    if (ctrl->flags & SNOR_FLG_STRIPE) {
        nc = chip->cfg.ncs;
    } else {
        nc = 1;
    }
    for (n = 0; n < nc; n++) {
        ul[n] = (uint8_t)((~chip->lock_v) & chip->lock_m);
    }
    if ((nc > 1) && (ctrl->funcs.dstripe != NULL)) {
        ctrl->funcs.dstripe(ctrl, ul, nc, 1);
    }

    // File system only supports uniformed sector, so we need to go through all sub-sectors
    if (((chip->flags & SNOR_CFLG_DLBBS) && (chip->offset == 0)) ||
        ((chip->flags & SNOR_CFLG_DLTBS) && (chip->offset == (chip->chipsz - lsize)))) {
        ssize = 1u << SNOR_SZPOW2_MIN;
    } else {
        ssize = lsize;
    }
    for (uint32_t soff = 0; soff < lsize; soff += ssize) {
        if (_snor_dyb_write(chip, chip->offset + soff, ul, nc) != EOK) return (EIO);
    }

    return (EOK);
}

/**
 *  @brief             Lock callout for SPI serial NOR flash.
 *                     Valid for both persistent and dynamic protection
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Lock flags.
 *  @param offset      Lock offset.
 *
 *  @return            EOK --success otherwise fail.
 */
int f3s_snor_lock(f3s_dbase_t *dbase,
                  f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;
    uint32_t    lsize, ssize;
    uint8_t     opcode, adrlen, nc;
    uint8_t     locks[SNOR_MAX_CS];

    if (!(dbase->flags & (F3S_PROTECT_DYN | F3S_PROTECT_PERSISTENT))) return (ENOTSUP);

    chip = (snor_chip_t *)access->service->page(&access->socket, 0, offset, NULL);
    if (chip == NULL) return (ERANGE);

    lsize = (chip->lksz_pow2 == 0) ? (1u << SNOR_SZPOW2_LOCK) : (1u << chip->lksz_pow2);
    if ((chip->offset % lsize) != 0) return (EINVAL);

    // persistent protection
    if (dbase->flags & F3S_PROTECT_PERSISTENT) {
        if (chip->op_dlock.opcode == 0) {
            opcode = SNOR_CMD_4PPBP;
            adrlen = 4;
        } else {
            opcode = chip->op_dlock.opcode;
            adrlen = chip->op_dlock.adrlen;
        }
        if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
        return snor_write_register(chip, opcode, chip->offset, adrlen, NULL, 0);
    }

    ctrl = chip->ctrl;
    if (ctrl->flags & SNOR_FLG_STRIPE) {
        nc = chip->cfg.ncs;
    } else {
        nc = 1;
    }

    // Dynamic protection
    // File system only supports uniformed sector, so we need to go through all sube-sectors
    if (((chip->flags & SNOR_CFLG_DLBBS) && (chip->offset == 0)) ||
        ((chip->flags & SNOR_CFLG_DLTBS) && (chip->offset == (chip->chipsz - lsize)))) {
        ssize = 1 << SNOR_SZPOW2_MIN;
    } else {
        ssize = lsize;
    }
    for (uint8_t n = 0; n < nc; n++) {
        locks[n] = chip->lock_v;
    }
    if ((nc > 1) && (ctrl->funcs.dstripe != NULL)) {
        ctrl->funcs.dstripe(ctrl, locks, nc, 1);
    }
    for (uint32_t soff = 0; soff < lsize; soff += ssize) {
        if (_snor_dyb_write(chip, chip->offset + soff, locks, nc) != EOK) return (EIO);
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
