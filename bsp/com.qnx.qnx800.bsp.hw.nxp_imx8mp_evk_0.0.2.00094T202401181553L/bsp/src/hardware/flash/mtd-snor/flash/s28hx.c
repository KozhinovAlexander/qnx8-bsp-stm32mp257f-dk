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

#define SPINOR_ID_SIZE      4

#define S28Hx_MANID         0x34            // JEDEC ID, Cypress
#define S28Hx_TYPE_33v      0x5A            // 3.3v type (HL-T)
#define S28Hx_TYPE_18v      0x5B            // 1.8v type (HS-T)

#define WRITE_REG_TIMEOUT   (600 * 1000)

#define S28Hx_SOFT_RESET_DELAY_US 100

/* Configuration register (Volatile) */
#define S28Hx_CFR1V          0x00800002
#define S28Hx_CFR2V          0x00800003
#define S28Hx_CFR3V          0x00800004
#define S28Hx_CFR4V          0x00800005
#define S28Hx_CFR5V          0x00800006

/* Configuration register (Nonvolatile) */
#define S28Hx_CFR1N          0x00000002
#define S28Hx_CFR2N          0x00000003
#define S28Hx_CFR3N          0x00000004
#define S28Hx_CFR4N          0x00000005
#define S28Hx_CFR5N          0x00000006

/* CFR1x bit definitions */
#define S28Hx_CFR1X_TB4KBS       (1 << 2)  // 0 = 4KB Sector Block is in the bottom of the memory address space
                                           // 1 = 4KB Sector Block is in the top of the memory address space
#define S28Hx_CFR1X_SP4KBS       (1 << 6)  // 0 = 4KB Sectors are grouped together
                                           // 1 = 4KB Sectors are split between High and Low Addresses

/* CFR2x bit definitions */
#define S28Hx_CFR2X_MEMLAT_200M  (0xB)     // Single mode: dummy cycle=11; Octal mode: dummy cycle=24

/* CFR3x bit definitions */
#define S28Hx_CFR3X_UNHYSA       (1 << 3)  // 1: Uniform Sector Architecture (all 256KB sectors); 0: Hybrid Sector Architecture
#define S28Hx_CFR3X_PGMBUF_512   (1 << 4)  // 1: 512 Byte Write Buffer Size; 0: 256 Byte Write Buffer Size

/* CFR4x bit definitions */
#define S28Hx_CFR4X_ECC12S_1     (0xA0)    // 1-bit ECC Error Detection/Correction

/* CFR5x bit definitions */
#define S28Hx_CFR5X_DDR          (1 << 1)  // 1: DDR enabled; 0 = SDR enabled
#define S28Hx_CFR5X_OCTAL        (1 << 0)  // 1: Data Width set to 8 wide (8x)-Octal Protocol; 0: Legacy Single SPI Protocol


/*  s28hx_set_device_config
 *
 *  Set device specific configurations and set I/O mode to Octal DDR for bus proto 888/888DTR.
 *
 *  Assumption: The device is currently in default configuration in standard SPI mode.
 */
static int32_t s28hx_set_device_config(struct _snor_chip_t* const chip, const uint32_t proto)
{
    const snor_ctrl_t* const ctrl = chip->ctrl;
    int32_t       err;
    const uint8_t alen = 3;
    uint8_t       buf;

    /* Check sector architecture. We do not support 'hybrid' non-uniform architecture.
     * When CFR3V[3]: UNHYSA - Uniform or Hybrid Sector Architecture Selection
     * 0 = Hybrid Sector Architecture (combination of 4KB sectors and 256KB sectors)
     * 1 = Uniform Sector Architecture (all 256KB sectors)
     */
    err = snor_read_register(chip, SNOR_CMD_RDAR, S28Hx_CFR3V, alen, &buf, 1);
    if (err != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Read S28Hx_CFR3V failed", __func__);
        return err;
    }

    if (!(buf & S28Hx_CFR3X_UNHYSA)) {
        snor_slogf(_SLOG_WARNING, ctrl->verbosity, 3, "%s: Hybrid Sector Architecture is not supported: CFR3V: 0x%x", __func__, buf);

        /* Try to read S28Hx_CFR1V */
        buf = 0;
        err = snor_read_register(chip, SNOR_CMD_RDAR, S28Hx_CFR1V, alen, &buf, 1);
        if (err != EOK) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Read S28Hx_CFR1V failed", __func__);
            return err;
        }

        if (buf & S28Hx_CFR1X_TB4KBS) {
            snor_slogf(_SLOG_WARNING, ctrl->verbosity, 3, "%s: 4KB Sector Block is in the top of the memory address space. Avoid to use", __func__);
        } else {
            snor_slogf(_SLOG_WARNING, ctrl->verbosity, 3, "%s: 4KB Sector Block is in the bottom of the memory address space. Avoid to use", __func__);
        }

        if (buf & S28Hx_CFR1X_SP4KBS) {
            snor_slogf(_SLOG_WARNING, ctrl->verbosity, 3, "%s: 4KB Sectors are split between High and Low Addresses", __func__);
        } else {
            snor_slogf(_SLOG_WARNING, ctrl->verbosity, 3, "%s: 4KB Sectors are grouped together", __func__);
        }
    }

    /* Set Latency Code for 200 MHz Reference Clock (WRARG_C_1 CFR2) */
    if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write SNOR_CMD_WREN failed", __func__);
        return (EIO);
    }
    buf = S28Hx_CFR2X_MEMLAT_200M;
    if (snor_write_register(chip, SNOR_CMD_WRAR, S28Hx_CFR2V, alen, &buf, 1) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write S28Hx_CFR2V failed", __func__);
        return (ENOTSUP);
    }

    /*
    * Disable 2-bit ECC Error Detection (WRARG_C_1 CFR4)
    * According to Cypress S28Hx Document Number: 002-18216, Section 4.1 Error Detection and Correction,
    * when 2-bit error detection is enabled, byte-programming/bit-walking/multiple page program operation
    * (without an erase) will result in Program Error (PRGERR)
    */
    if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write SNOR_CMD_WREN failed", __func__);
        return (EIO);
    }
    buf = S28Hx_CFR4X_ECC12S_1;
    if (snor_write_register(chip, SNOR_CMD_WRAR, S28Hx_CFR4V, alen, &buf, 1) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write S28Hx_CFR4V failed", __func__);
        return (ENOTSUP);
    }

    /* Enable/Disable Octal DTR (WRARG_C_1 CFR5) */
    buf = 0;
    if ((proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_8_8_8) {
        buf = S28Hx_CFR5X_OCTAL;
        if (proto & SNOR_BUSPROTO_DTR_MODE) {
            buf |= S28Hx_CFR5X_DDR;
        }
    }

    if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write SNOR_CMD_WREN failed", __func__);
        return (EIO);
    }

    if (snor_write_register(chip, SNOR_CMD_WRAR, S28Hx_CFR5V, alen, &buf, 1) != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Write S28Hx_CFR5V failed", __func__);
        return (ENOTSUP);
    }

    /* Enable 4B address support */
    chip->flags |= SNOR_CFLG_4B_ADDR;
    chip->op_rd.adrlen = 4;
    chip->op_wr.adrlen = 4;

    /* Overwrite chip opcode */
    if ((proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_8_8_8) {
        chip->op_rd.opcode = SNOR_CMD_READ_144_4B;
        chip->op_rd.dcycle = 24;
        chip->op_wr.opcode = SNOR_CMD_PP_4B;
        if (proto & SNOR_BUSPROTO_DTR_MODE) {
            chip->op_rd.opcode = SNOR_CMD_READ_144_DTR_4B;
        }
    } else {
        chip->op_rd.opcode = SNOR_CMD_READ_4B;
        chip->op_rd.dcycle = 0;
        chip->op_wr.opcode = SNOR_CMD_PP_4B;
    }

    return EOK;
}

static void s28hx_set_dopflgs(struct _snor_chip_t* const chip, const uint32_t proto)
{
    if ((proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_8_8_8) {
        chip->flags |= SNOR_CFLG_DOP;
    } else {
        chip->flags &= ~SNOR_CFLG_DOP;
    }
}

/**
 *  @brief             Set protocol callout for Cypress S28Hx serial NOR flash.
 *  @param chip        Chip handle.
 *  @param proto       Protocol.
 *
 *  @return            EOK --success otherwise fail.
 */
static int _s28hx_set_protocol(struct _snor_chip_t* const chip, const uint32_t proto)
{
    const snor_ctrl_t* const ctrl = chip->ctrl;

    if (ctrl->verbosity > 3) {
        snor_slogf(_SLOG_INFO, ctrl->verbosity, 1, "%s: set protocol %x%s%s", __func__,
            proto & SNOR_BUSPROTO_BUS_MASK,
            (proto & SNOR_BUSPROTO_DTR_MODE) ? "-DTR" : "",
            (proto & SNOR_BUSPROTO_DQS) ? "-DQS" : "");
    }

    const int32_t err = s28hx_set_device_config(chip, proto);
    if (err != EOK) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: s28hx_set_device_config failed", __func__);
        return err;
    }

    s28hx_set_dopflgs(chip, proto);

    return err;
}

/**
 *  @brief             Ident callout for Cypress S28Hx serial NOR flash.
 *  @param dbase       F3S data base handle.
 *  @param access      F3S access handle.
 *  @param flags       Ident flags.
 *  @param cs          Chip select
 *
 *  @return            EOK --success otherwise fail.
 */
int32_t f3s_s28hx_ident(f3s_dbase_t *dbase, f3s_access_t *access, const uint32_t flags, const uint32_t cs)
{
    snor_ctrl_t   *ctrl;
    snor_chip_t   *chip;
    uint8_t       ids[SPINOR_ID_SIZE];

    if (access == NULL) return ENODEV;

    ctrl = (snor_ctrl_t *)access->socket.memory;
    chip = &ctrl->chip[cs];

    if ((chip->cfg.bus_proto != SNOR_BUSPROTO_1_1_1) &&
        (chip->cfg.bus_proto != SNOR_BUSPROTO_8_8_8) &&
        (chip->cfg.bus_proto != SNOR_BUSPROTO_8_8_8_DTR)) return (ENOTSUP);

    if ((chip->vid != 0) && (chip->did != 0)) {
        ids[0] = chip->vid;
        ids[1] = chip->did;
    } else {
        if (snor_read_id(chip, ids, SPINOR_ID_SIZE) != EOK) return EIO;
    }

    if ((ids[0] == S28Hx_MANID) && ((ids[1] == S28Hx_TYPE_33v) || (ids[1] == S28Hx_TYPE_18v))) {
        chip->hcaps = SNOR_HCAPS_RD_1_1_1 | SNOR_HCAPS_RD_1_1_1_FAST | SNOR_HCAPS_RD_8_8_8 | SNOR_HCAPS_RD_OCTAL |
                      SNOR_HCAPS_PP_1_1_1 | SNOR_HCAPS_PP_8_8_8 |
                      SNOR_HCAPS_DTR | SNOR_HCAPS_DQS;

        if (f3s_sfdp_ident(dbase, access, flags, cs) != EOK) return ENOTSUP;

        uint8_t cr;
        int32_t err;

        err = snor_read_register(chip, SNOR_CMD_RDAR, S28Hx_CFR3V, 3, &cr, sizeof(cr));
        if (err != EOK) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: Read of S28Hx_CFR3V failed", __func__);
            return (err);
        }

        if (cr & S28Hx_CFR3X_PGMBUF_512) {
            chip->pagesz = 512;
        } else {
            chip->pagesz = 256;
        }

        chip->op_pr = 0x30;
        chip->op_ps = 0xB0;
        chip->op_er = 0x30;
        chip->op_es = 0xB0;

        // Add chip specific erase suspend calllout, generic resume callout should work.
        chip->asr2  = 0x07;
        chip->sr2_esbit = 1;
    } else {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "%s: unsupported ID[%02x:%02x]", __func__, ids[0], ids[1]);
        return ENOTSUP;
    }

    if (dbase != NULL) {
        dbase->jedec_hi    = ids[0];
        dbase->jedec_lo    = (uint16_t)(ids[1] << 8);
        dbase->jedec_lo    = (uint16_t)ids[2];
        dbase->name        = "S28Hx";
    }

    chip->set_protocol = _s28hx_set_protocol;

    return EOK;
}


/**
 *  @brief             Reset callout for Cypress S28Hx serial NOR flash.
 *  @param dbase       Pointer to F3S data base.
 *  @param access      Pointer to flash access structure.
 *  @param flags       Reset  flags.
 *  @param offset      Reset offset.
 *
 *  @return            None
 */
void f3s_s28hx_reset(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset)
{
    snor_chip_t *chip;
    snor_ctrl_t *ctrl;

    chip = (snor_chip_t *)access->service->page(&access->socket, flags, offset, NULL);
    if (chip == NULL) return;

    ctrl = chip->ctrl;

    s28hx_set_dopflgs(chip, chip->cfg.bus_proto);

    /* Reconfig the bus */
    if (ctrl->funcs.cfg_bus != NULL) {
        ctrl->funcs.cfg_bus(ctrl, &chip->cfg);
    }

    if (snor_reset(chip) == EOK) {
        /* s28hx does not provide a status bit to verify reset completion.
         * Need a delay to ensure the volatile registers have been loaded from
         * non-volatile regs.
         */
        usleep(S28Hx_SOFT_RESET_DELAY_US);

        /* Reconfig the bus */
        if (ctrl->funcs.cfg_bus != NULL) {
            chip->cfg.bus_proto = SNOR_BUSPROTO_1_1_1;
            ctrl->funcs.cfg_bus(ctrl, &chip->cfg);
        }

        if (s28hx_set_device_config(chip, chip->cfg.bus_proto) != EOK) {
            snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: s28hx_set_device_config failed", __func__);
        }

        s28hx_set_dopflgs(chip, chip->cfg.bus_proto);
    } else {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0, "%s: snor_reset failed", __func__);
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif

