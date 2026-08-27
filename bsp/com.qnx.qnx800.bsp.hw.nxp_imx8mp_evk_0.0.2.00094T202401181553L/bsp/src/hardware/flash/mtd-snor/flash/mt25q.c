/*
 * Copyright (c) 2023, BlackBerry Limited.
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

#define MT25Q_ID_LEN        4

#define MT25Q_MANID         0x20    // JEDEC ID, Micron
#define MT25Q_TYPE_33v      0xBA    // 3.3v type
#define MT25Q_TYPE_18v      0xBB    // 1.8v type

#define MT25Q_OP_RDEVCR      0x65   // Read Enhanced Volatile Configuration Register
#define MT25Q_OP_WREVCR      0x61   // Write Enhanced Volatile Configuration Register
    #define DRV_STRGTH_MSK   (0x7)  // Output driver strength mask
    #define DIS_DTR          (1<<5) // 1: Disable DTR
    #define DIS_DUAL         (1<<6) // 1: Disable Dual
    #define DIS_QUAD         (1<<7) // 1: Disable Quad

static int _mt25q_set_protocol(struct _snor_chip_t *chip, uint32_t proto);

/**
 *  @brief             Ident callout for Micron MT25Q serial NOR flash.
 *  @param dbase       F3S data base handle.
 *  @param access      F3S access handle.
 *  @param flags       Ident flags.
 *  @param cs          Chip select
 *
 *  @return            EOK --success otherwise fail.
 */
int32_t f3s_mt25q_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs)
{
    snor_ctrl_t *ctrl;
    snor_chip_t *chip;
    uint8_t     ids[MT25Q_ID_LEN];

    if (access == NULL) return (ENODEV);

    ctrl = (snor_ctrl_t *)access->socket.memory;

    chip = &ctrl->chip[cs];

    if (((chip->cfg.bus_proto >> SNOR_BUSPROTO_CBSHIFT) & 0x0F) > 4) return (ENOTSUP);

    if ((chip->vid != 0) && (chip->did != 0)) {
        ids[0] = chip->vid;
        ids[1] = chip->did;
    } else {
        if (snor_read_id(chip, ids, MT25Q_ID_LEN) != EOK) return (EIO);
    }

    if ((ids[0] == MT25Q_MANID) && ((ids[1] == MT25Q_TYPE_33v) || (ids[1] == MT25Q_TYPE_18v))) {
        chip->hcaps = SNOR_HCAPS_RD_1_1_1 | SNOR_HCAPS_RD_1_1_1_FAST |
                      SNOR_HCAPS_RD_1_4_4 | SNOR_HCAPS_RD_1_4_4_DTR |
                      SNOR_HCAPS_RD_4_4_4 |
                      SNOR_HCAPS_PP_1_1_1 | SNOR_HCAPS_PP_1_4_4 | SNOR_HCAPS_PP_4_4_4 |
                      SNOR_HCAPS_DTR;

        if (f3s_sfdp_ident(dbase, access, flags, cs) != EOK) return (ENOTSUP);

        chip->flags |= SNOR_CFLG_DLTBS | SNOR_CFLG_DLBBS | SNOR_CFLG_PSLOCK | SNOR_CFLG_DNLOCK;
    } else {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "%s: unsupported ID[%02x:%02x]", __func__, ids[0], ids[1]);
        return (ENOTSUP);
    }

    if (dbase != NULL) {
        dbase->jedec_hi    = ids[0];
        dbase->jedec_lo    = (uint16_t)(ids[1] << 8);
        dbase->jedec_lo    = (uint16_t)ids[2];
        dbase->name        = "MT25Q";
    }

    chip->set_protocol = _mt25q_set_protocol;

    return (EOK);
}

/**
 *  @brief             Set protocol callout for Micron MT25Q serial NOR flash.
 *                     Use Enhanced Volatile Configuration Register to configure bus
 *                     Quad, Dual and DTR, as well as driver strength.
 *  @param chip        Chip handle.
 *  @param proto       Protocol.
 *
 *  @return            EOK --success otherwise fail.
 */
static int _mt25q_set_protocol(struct _snor_chip_t* const chip, const uint32_t proto)
{
    const snor_ctrl_t* const ctrl = chip->ctrl;
    int32_t     err;
    uint8_t     evcr[2];
    uint8_t     reg;
    uint32_t    cflag = 0;

    if (ctrl->verbosity > 3) {
        snor_slogf(_SLOG_INFO, ctrl->verbosity, 1, "%s: set protocol %x%s", __func__,
            proto & SNOR_BUSPROTO_BUS_MASK,
            (proto & SNOR_BUSPROTO_DTR_MODE) ? "-DTR" : "");
    }

    /* Read EVCR */
    err = snor_read_register(chip, MT25Q_OP_RDEVCR, 0, 0, evcr, sizeof(evcr));
    if (err != EOK) {
        return (err);
    }

    /* Bus mode */
    reg = (uint8_t)(evcr[0] | (DIS_QUAD | DIS_DUAL | DIS_DTR));
    switch (proto & SNOR_BUSPROTO_BUS_MASK) {
        case SNOR_BUSPROTO_4_4_4:      /* Enable QUAD */
            reg &= ~DIS_QUAD;
            cflag = SNOR_CFLG_QUAD;
            break;
        case SNOR_BUSPROTO_2_2_2:      /* Enable DUAL */
            reg &= ~DIS_DUAL;
            cflag = SNOR_CFLG_DUAL;
            break;
        default:
            break;
    }

    /* Enable DTR */
    if (proto & SNOR_BUSPROTO_DTR_MODE) {
        reg &= ~DIS_DTR;
    }

    /* Set driver strength */
    if ((chip->drv_type != 0) && (chip->drv_type != DRV_STRGTH_MSK)) {
        reg &= ~DRV_STRGTH_MSK;
        reg |= chip->drv_type & DRV_STRGTH_MSK;
    }

    if (reg != evcr[0]) {
        if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
        if (snor_write_register(chip, MT25Q_OP_WREVCR, 0, 0, &reg, 1) != EOK) return (EIO);
    }

    chip->flags |= cflag;

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
