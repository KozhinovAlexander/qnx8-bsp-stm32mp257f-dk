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
** File: snor_post_ident.c
**
** Description:
**
** This file contains the post identification function for the f3s flash file system
**
** Ident: $Id: snor_post_ident.c $
**
*/

/*
** Includes
*/

#include "f3s_snor.h"

static int32_t snor_select_octal(snor_chip_t *chip);
static int32_t snor_select_quad(snor_chip_t *chip);
static int32_t snor_select_dual(snor_chip_t *chip);
static int32_t snor_select_single(snor_chip_t *chip);
static int snor_enter_4b_address(struct _snor_chip_t * const chip);
static int snor_set_4b_cmds(snor_chip_t *chip);

#define SNOR_NEED_4B_COMMAND(chip)  (!((chip)->flags & SNOR_CFLG_4B_ADDR) && ((chip)->chipsz > 16 * 1024 * 1024))

/**
 *  @brief             Post ident configuration.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
int snor_post_ident(struct _snor_chip_t* const chip)
{
    snor_ctrl_t* const ctrl = chip->ctrl;

    /* default read/write commands */
    chip->rdcfg.bus_proto = SNOR_BUSPROTO_NONE;
    chip->wrcfg.bus_proto = SNOR_BUSPROTO_NONE;
    chip->op_rd.opcode    = 0;
    chip->op_rd.dcycle    = 255u;
    chip->op_rd.adrlen    = 3;
    chip->op_wr.opcode    = 0;
    chip->op_wr.adrlen    = 3;

    /* support 4B address mode */
    if (chip->dcaps & SNOR_DCAPS_ADDR_3B_4B) {
        if (chip->enter_4b_address == NULL) {
            chip->enter_4b_address = snor_enter_4b_address;
        }
        if (chip->enter_4b_address(chip) == EOK) {
            /* now in 4 bytes address mode */
            chip->flags |= SNOR_CFLG_4B_ADDR;
            chip->op_rd.adrlen = 4;
            chip->op_wr.adrlen = 4;
        }
    }

    /*
     * We try Octal mode first, then quad, dual, and single
     * In theory, 4-4-4 DTR is faster than 1-1-8 STR, 2-2-2 DTR
     * is faster than 1-1-4 STR
     */
    if (chip->hcaps & SNOR_HCAPS_RD_OCTAL) {
        if (snor_select_octal(chip) != EOK) {
            chip->hcaps &= ~SNOR_HCAPS_RD_OCTAL;
        }
    }

    if ((chip->hcaps & SNOR_HCAPS_RD_QUAD) && !(chip->hcaps & SNOR_HCAPS_RD_OCTAL)) {
        if (snor_select_quad(chip) != EOK) {
            chip->hcaps &= ~SNOR_HCAPS_RD_QUAD;
        }
    }

    if (chip->hcaps & SNOR_HCAPS_RD_DUAL) {
        if (!(chip->hcaps & (SNOR_HCAPS_RD_QUAD | SNOR_HCAPS_RD_OCTAL))) {
            if (snor_select_dual(chip) != EOK) {
                chip->hcaps &= ~SNOR_HCAPS_RD_DUAL;
            }
        }
    }

    if (!(chip->hcaps &
            (SNOR_HCAPS_RD_QUAD | SNOR_HCAPS_RD_OCTAL | SNOR_HCAPS_RD_DUAL))) {
        if (snor_select_single(chip) != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: snor_select_single failed", __func__);
            return (ENOTSUP);
        }
    }

    if (SNOR_NEED_4B_COMMAND(chip)) {
        if (snor_set_4b_cmds(chip) != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: snor_set_4b_cmds failed", __func__);
            return (ENOTSUP);
        }
        chip->op_rd.adrlen = 4;
        chip->op_wr.adrlen = 4;
    }

    // re-configure bus protocol
    ctrl->funcs.cfg_bus(ctrl, &chip->cfg);

    if (ctrl->funcs.post_ident != NULL) {
        if (ctrl->funcs.post_ident(ctrl, chip->cfg.cs) != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: ctrl->funcs.post_ident failed", __func__);
            return (EIO);
        }
    }

    if (chip->hcaps & SNOR_HCAPS_DQS) {
        chip->cfg.bus_proto |= SNOR_BUSPROTO_DQS;
    }

    // in case bus protocol has been changed
    ctrl->funcs.cfg_bus(ctrl, &chip->cfg);

    /* Update read configuration */
    chip->rdcfg.cs  = chip->cfg.cs;     // chip select won't change
    if (chip->rdcfg.clk == 0) {
        chip->rdcfg.clk = chip->cfg.clk;
    }
    if (chip->rdcfg.bus_proto == 0) {
        chip->rdcfg.bus_proto = chip->cfg.bus_proto;
    }

    /* Update page program configuration */
    chip->wrcfg.cs  = chip->cfg.cs;     // chip select won't change
    if (chip->wrcfg.clk == 0) {
        chip->wrcfg.clk = chip->cfg.clk;
    }
    if (chip->wrcfg.bus_proto == 0) {
        chip->wrcfg.bus_proto = chip->cfg.bus_proto;
    }

    if (ctrl->verbosity > 2) {
        snor_slogf(_SLOG_INFO, ctrl->verbosity, 2,
                "(devf  t%d:%s bus[%x], read[bus-%x:op-0x%02X:alen-%d:dc-%d], write[bus-%x:op-0x%02X:alen-%d])",
                pthread_self(), __func__, chip->cfg.bus_proto,
                chip->rdcfg.bus_proto, chip->op_rd.opcode, chip->op_rd.adrlen, chip->op_rd.dcycle,
                chip->wrcfg.bus_proto, chip->op_wr.opcode, chip->op_rd.adrlen);
    }

    chip->flags |= SNOR_CFLG_PRESENT;

    return (EOK);
}

#define SNOR_POST_RDOP(prot, dfop, dfdc) {  \
        rdproto = SNOR_BUSPROTO_##prot;     \
        proto   = rdproto;  \
        rd_op   = (uint8_t)(chip->rdops[SNOR_BPI_##prot].opcode ? chip->rdops[SNOR_BPI_##prot].opcode : dfop); \
        rd_dc   = (uint8_t)(chip->rdops[SNOR_BPI_##prot].opcode ? chip->rdops[SNOR_BPI_##prot].dcycle : dfdc); \
    }
#define SNOR_POST_WROP(prot, dfop) {   \
        wrproto = SNOR_BUSPROTO_##prot;    \
        pp_op   = (uint8_t)(chip->ppops[SNOR_BPI_##prot].opcode ?: dfop); \
    }

/**
 *  @brief             Select octal mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int32_t snor_select_octal(snor_chip_t *chip)
{
    uint32_t    proto = SNOR_BUSPROTO_NONE;
    uint32_t    rdproto = SNOR_BUSPROTO_NONE;
    uint32_t    wrproto = SNOR_BUSPROTO_NONE;
    uint8_t     rd_op = 0, rd_dc = 16, pp_op = 0;

    /* DTR mode has the highest priority
     * FIXME! currently not sure there is a generic way to enable octal or DTR mode
     * For now it only works if chip specific protocol function is implemented
     */
    if (chip->hcaps & SNOR_HCAPS_DTR) {
        if (chip->hcaps & SNOR_HCAPS_RD_8_8_8) {
            /* no need to check PP capability, everything is in octal mode */
            /* set_protocol needs to be able to switch to Octal DTR mode */
            /* use default read/write commands */
            proto = SNOR_BUSPROTO_8_8_8 | SNOR_BUSPROTO_DTR_MODE;
            rdproto = SNOR_BUSPROTO_8_8_8 | SNOR_BUSPROTO_DTR_MODE;
            wrproto = SNOR_BUSPROTO_8_8_8 | SNOR_BUSPROTO_DTR_MODE;
            rd_op = SNOR_CMD_READ_FAST;
            pp_op = SNOR_CMD_PP;
        } else {
            /* read operation protocol, OP code etc, dummy cycle */
            if (chip->hcaps & SNOR_HCAPS_RD_1_8_8) {
                SNOR_POST_RDOP(1_8_8, SNOR_CMD_READ_188, 16);
            } else {
                SNOR_POST_RDOP(1_1_8, SNOR_CMD_READ_118, 16);
            }
            rdproto |= SNOR_BUSPROTO_DTR_MODE;

            /* write operation protocol, OP code */
            if (chip->hcaps & SNOR_HCAPS_PP_1_8_8) {
                SNOR_POST_WROP(1_8_8, SNOR_CMD_PP_188);
            } else {
                SNOR_POST_WROP(1_1_8, SNOR_CMD_PP_118);
            }
            wrproto |= SNOR_BUSPROTO_DTR_MODE;

            proto |= SNOR_BUSPROTO_DTR_MODE;
        }
    }

    /* STR commands have the second priority */
    if (proto == SNOR_BUSPROTO_NONE) {
        proto = SNOR_BUSPROTO_1_1_1;
        /* read operation protocol, OP code etc, dummy cycle */
        if (chip->hcaps & SNOR_HCAPS_RD_1_8_8_DTR) {
            SNOR_POST_RDOP(1_8_8_DTR, SNOR_CMD_READ_188_DTR, 16);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_1_8_DTR) {
            SNOR_POST_RDOP(1_1_8_DTR, SNOR_CMD_READ_118_DTR, 16);
        } else if (chip->hcaps & SNOR_HCAPS_RD_8_8_8) {
            SNOR_POST_RDOP(8_8_8, SNOR_CMD_READ_FAST, 16);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_8_8) {
            SNOR_POST_RDOP(1_8_8, SNOR_CMD_READ_188, 16);
        } else {
            SNOR_POST_RDOP(1_1_8, SNOR_CMD_READ_118, 16);
        }

        /* write operation protocol, OP code */
        if (rdproto == SNOR_BUSPROTO_8_8_8) {
            SNOR_POST_WROP(8_8_8, SNOR_CMD_PP);
            proto = SNOR_BUSPROTO_8_8_8;
        } else if (chip->hcaps & SNOR_HCAPS_PP_1_8_8) {
            SNOR_POST_WROP(1_8_8, SNOR_CMD_PP_188);
        } else {
            SNOR_POST_WROP(1_1_8, SNOR_CMD_PP_118);
        }
    }

    if (proto == SNOR_BUSPROTO_NONE) return (ENOTSUP);

    if (chip->hcaps & SNOR_HCAPS_DQS) {
        proto   |= SNOR_BUSPROTO_DQS;
        rdproto |= SNOR_BUSPROTO_DQS;
        wrproto |= SNOR_BUSPROTO_DQS;
    }

    if (chip->set_protocol != NULL) {
        if (chip->set_protocol(chip, proto) != EOK) return (ENOTSUP);
    }

    if (chip->rdcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->rdcfg.bus_proto = rdproto;
    }
    if (chip->wrcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->wrcfg.bus_proto = wrproto;
    }
    if (chip->op_rd.opcode == 0) {
        chip->op_rd.opcode = rd_op;
    }
    if (chip->op_rd.dcycle == 255u) {
        chip->op_rd.dcycle = rd_dc;
    }
    if (chip->op_wr.opcode == 0) {
        chip->op_wr.opcode = pp_op;
    }

    chip->cfg.bus_proto = proto;

    return (EOK);
}

/**
 *  @brief             Configure device to quad mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int snor_enter_quad(struct _snor_chip_t* const chip)
{
    uint8_t     reg;

    if (chip->qes & (1u << 0)) {
        /* TODO, check qer to determine what to do */
        return (ENOTSUP);
    } else if (chip->qes & (1u << 1)) {
        return snor_write_cmd(chip, 0x38u);
    } else if (chip->qes & (1u << 2)) {
        return snor_write_cmd(chip, 0x35u);
    } else if (chip->qes & (1u << 3)) {
        /* TODO */
        /*
         * read configuration using instruction 65h followed by address 800003h, set bit 6,
         * write configuration using instruction 71h followed by address 800003h. This configuration is volatile.
         */
        return (ENOTSUP);
    } else if (chip->qes & (1u << 4)) {
        if (snor_read_register(chip, 0x65, 0, 0, &reg, 1) != EOK) return (EIO);
        reg &= ~(1u << 7);
        return snor_write_register(chip, 0x61, 0, 0, &reg, 1);
    }

    /* there has to be a device specific quad enable callback */
    if (chip->set_protocol != NULL) return EOK;

    return (ENOTSUP);
}

/**
 *  @brief             Select quad mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int32_t snor_select_quad(snor_chip_t *chip)
{
    uint32_t    proto = SNOR_BUSPROTO_NONE;
    uint32_t    rdproto = SNOR_BUSPROTO_NONE;
    uint32_t    wrproto = SNOR_BUSPROTO_NONE;
    uint8_t     rd_op = 0, rd_dc = 10, pp_op = 0;

    if (chip->enter_quad == NULL) {
        chip->enter_quad = snor_enter_quad;
    }

    /*
     * DTR mode has the highest priority
     * NOTE! need set_protocol to set DTR mode
     */
    if ((chip->hcaps & SNOR_HCAPS_DTR) && (chip->hcaps & SNOR_HCAPS_RD_4_4_4)) {
        SNOR_POST_RDOP(4_4_4_DTR, SNOR_CMD_READ_144_DTR, 8);

        wrproto = SNOR_BUSPROTO_4_4_4;
        proto   = SNOR_BUSPROTO_4_4_4;
        pp_op   = SNOR_CMD_PP;
    } else {
        /* read operation protocol, OP code, dummy cycle */
        if (chip->hcaps & SNOR_HCAPS_RD_1_4_4_DTR) {
            SNOR_POST_RDOP(1_4_4_DTR, SNOR_CMD_READ_144_DTR, 8);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_1_4_DTR) {
            SNOR_POST_RDOP(1_1_4_DTR, SNOR_CMD_READ_114_DTR, 8);
        } else if (chip->hcaps & SNOR_HCAPS_RD_4_4_4) {
            SNOR_POST_RDOP(4_4_4, SNOR_CMD_READ_FAST, 10);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_4_4) {
            SNOR_POST_RDOP(1_4_4, SNOR_CMD_READ_144, 10);
        } else {
            SNOR_POST_RDOP(1_1_4, SNOR_CMD_READ_114, 10);
        }

        /* write operation protocol, OP code */
        if (rdproto == SNOR_BUSPROTO_4_4_4) {
            SNOR_POST_WROP(4_4_4, SNOR_CMD_PP);
            proto = SNOR_BUSPROTO_4_4_4;
        } else if (chip->hcaps & SNOR_HCAPS_PP_1_4_4) {
            SNOR_POST_WROP(1_4_4, SNOR_CMD_PP_144);
        } else {
            SNOR_POST_WROP(1_1_4, SNOR_CMD_PP_114);
        }
    }

    if (proto == SNOR_BUSPROTO_NONE) return (ENOTSUP);

    if (chip->set_protocol != NULL) {
        if (chip->set_protocol(chip, proto) != EOK) return (ENOTSUP);
    }

    if ((proto & SNOR_BUSPROTO_BUS_MASK) == SNOR_BUSPROTO_4_4_4) {
        /* If quad is not set by chip set_protocol */
        if (!(chip->flags & SNOR_CFLG_QUAD)) {
            if (chip->enter_quad(chip) != EOK) return (ENOTSUP);
            chip->flags |= SNOR_CFLG_QUAD;
        }
    }

    if (chip->rdcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->rdcfg.bus_proto = rdproto;
    }
    if (chip->wrcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->wrcfg.bus_proto = wrproto;
    }
    if (chip->op_rd.opcode == 0) {
        chip->op_rd.opcode = rd_op;
    }
    if (chip->op_rd.dcycle == 255u) {
        chip->op_rd.dcycle = rd_dc;
    }
    if (chip->op_wr.opcode == 0) {
        chip->op_wr.opcode = pp_op;
    }

    chip->cfg.bus_proto = proto;

    return (EOK);
}

/**
 *  @brief             Select dual mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int32_t snor_select_dual(snor_chip_t *chip)
{
    uint32_t    proto = SNOR_BUSPROTO_NONE;
    uint32_t    rdproto = SNOR_BUSPROTO_NONE;
    uint32_t    wrproto = SNOR_BUSPROTO_NONE;
    uint8_t     rd_op = 0, rd_dc = 8, pp_op = 0;

    /* Dual DTR mode has the highest priority
     */
    if ((chip->hcaps & SNOR_HCAPS_DTR) && (chip->hcaps & SNOR_HCAPS_RD_2_2_2)) {
        proto = SNOR_BUSPROTO_2_2_2;
        rdproto = SNOR_BUSPROTO_2_2_2 | SNOR_BUSPROTO_DTR_MODE;
        wrproto = SNOR_BUSPROTO_2_2_2;
        rd_op = SNOR_CMD_READ_FAST;
        rd_dc = 6;
        pp_op = SNOR_CMD_PP;
    } else {
        /* read operation protocol, OP code etc, dummy cycle */
        if (chip->hcaps & SNOR_HCAPS_RD_1_2_2_DTR) {
            SNOR_POST_RDOP(1_2_2_DTR, SNOR_CMD_READ_122_DTR, 6);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_1_2_DTR) {
            SNOR_POST_RDOP(1_1_2_DTR, SNOR_CMD_READ_112_DTR, 6);
        } else if ((chip->hcaps & SNOR_HCAPS_RD_2_2_2) && (chip->set_protocol != NULL)) {
            SNOR_POST_RDOP(2_2_2, SNOR_CMD_READ_FAST, 6);
        } else if (chip->hcaps & SNOR_HCAPS_RD_1_2_2) {
            SNOR_POST_RDOP(1_2_2, SNOR_CMD_READ_122, 6);
        } else {
            SNOR_POST_RDOP(1_1_2, SNOR_CMD_READ_112, 6);
        }

        /* write operation protocol, OP code*/
        if (rdproto == SNOR_BUSPROTO_2_2_2) {
            SNOR_POST_WROP(8_8_8, SNOR_CMD_PP);
            proto = SNOR_BUSPROTO_2_2_2;
        } else if (chip->hcaps & SNOR_HCAPS_PP_1_2_2) {
            SNOR_POST_WROP(1_2_2, SNOR_CMD_PP_122);
        } else {
            SNOR_POST_WROP(1_1_2, SNOR_CMD_PP_112);
        }
    }

    if (proto == SNOR_BUSPROTO_NONE) return (ENOTSUP);

    if (chip->set_protocol != NULL) {
        if (chip->set_protocol(chip, proto) != EOK) return (ENOTSUP);
        if (chip->cfg.bus_proto != proto) return (ENOTSUP);
    }

    if (chip->rdcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->rdcfg.bus_proto = rdproto;
    }
    if (chip->wrcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->wrcfg.bus_proto = wrproto;
    }
    if (chip->op_rd.opcode == 0) {
        chip->op_rd.opcode = rd_op;
    }
    if (chip->op_rd.dcycle == 255u) {
        chip->op_rd.dcycle = rd_dc;
    }
    if (chip->op_wr.opcode == 0) {
        chip->op_wr.opcode = pp_op;
    }

    chip->cfg.bus_proto = proto;

    return (EOK);
}

/**
 *  @brief             Select single mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int32_t snor_select_single(snor_chip_t *chip)
{
    uint32_t    proto = SNOR_BUSPROTO_1_1_1;
    const uint32_t    rdproto = SNOR_BUSPROTO_1_1_1;
    const uint32_t    wrproto = SNOR_BUSPROTO_1_1_1;
    const uint8_t     pp_op = SNOR_CMD_PP;
    uint8_t     rd_op = SNOR_CMD_READ_FAST;     // Fast read hs to be supported
    uint8_t     rd_dc = 8;

    /*
     * Rely on set_protocol to enable DTR
     */
    if ((chip->hcaps & SNOR_HCAPS_DTR) && (chip->set_protocol != NULL)) {
        proto |= SNOR_BUSPROTO_DTR_MODE;
    } else if (!(chip->hcaps & SNOR_HCAPS_RD_1_1_1_FAST)) {
        rd_op = SNOR_CMD_READ;
        rd_dc = 0;
    }

    if (chip->set_protocol != NULL) {
        if (chip->set_protocol(chip, proto) != EOK) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: chip->set_protocol failed", __func__);
            return (ENOTSUP);
        }
        if (chip->cfg.bus_proto != proto) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: chip->cfg.bus_proto is 0x%x while proto is 0x%x", __func__, chip->cfg.bus_proto, proto);
            return (ENOTSUP);
        }
    }

    if (chip->rdcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->rdcfg.bus_proto = rdproto;
    }
    if (chip->wrcfg.bus_proto == SNOR_BUSPROTO_NONE) {
        chip->wrcfg.bus_proto = wrproto;
    }
    if (chip->op_rd.opcode == 0) {
        chip->op_rd.opcode = rd_op;
    }
    if (chip->op_rd.dcycle == 255u) {
        chip->op_rd.dcycle = rd_dc;
    }
    if (chip->op_wr.opcode == 0) {
        chip->op_wr.opcode = pp_op;
    }

    chip->cfg.bus_proto = proto;

    return (EOK);
}

/**
 *  @brief             Configure device to 4 byte commands mode.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int snor_enter_4b_address(struct _snor_chip_t* const chip)
{
    const snor_ctrl_t* const ctrl = chip->ctrl;

    if (chip->e4ba & (1 << 6)) {
        /* always in 4B address mode, no further action required */
        return (EOK);
    } else if (chip->e4ba & (3 << 0)) {
        /* Write-enable is optional */
        if ((chip->e4ba & (1 << 1)) && !(ctrl->ccaps & SNOR_CCAPS_PPAWREN)) {
            if (snor_write_cmd(chip, SNOR_CMD_WREN) != EOK) return (EIO);
        }
        /* Enter 4B address command */
        return snor_write_cmd(chip, SNOR_CMD_ETR_4B);
#if 0
    } else if (chip->e4ba & (1 << 5)) {
        /* 4B address instruction */
        return (EOK);
#endif
    } else if (chip->e4ba & (1 << 4)) {
        /* A 16-bit nonvolatile configuration register controls
         * 3-Byte/4-Byte address mode. Read instruction is B5h.
         * Bit[0] controls address mode [0=3-Byte; 1=4-Byte].
         * Write configuration register instruction is B1h, data length is 2 bytes
         */
        uint8_t nvcr[2];
        if (snor_read_register(chip, SNOR_CMD_RDNVCR, 0, 0, nvcr, sizeof(nvcr)) != EOK) return (EIO);
        if ((nvcr[0] & 0x01) == 0) {
            nvcr[0] |= 0x01;
            return snor_write_register(chip, SNOR_CMD_WRNVCR, 0, 0, nvcr, sizeof(nvcr));
        }
#if 0
    } else if (chip->e4ba & (1 << 3)) {
        /*
         * 8-bit volatile bank register used to define A[30:24] bits.
         * MSB (bit[7]) is used to enable/disable 4-byte address mode.
         * When MSB is set to ‘1’, 4-byte address mode is active and
         * A[30:24] bits are don’t care. Read with instruction 16h.
         * Write instruction is 17h with 1 byte of data. When MSB is
         * cleared to ‘0’, select the active 128 Mbit segment by setting
         * the appropriate A[30:24] bits and use 3-Byte addressing.
         */
        /* TODO!!, We don't support this for now, probably never will */
    } else if (chip->e4ba & (1 << 2)) {
        /*
         * 8-bit volatile extended address register used to define A[31:24]
         * bits. Read with instruction C8h. Write instruction is C5h with
         * 1 byte of data. Select the active 128 Mbit memory segment by
         * setting the appropriate A[31:24] bits and use 3-Byte addressing.
         */
        /* TODO!!, We don't support this for now, probably never will */
#endif
    }

    return (ENOTSUP);
}

typedef struct _cmd_4b_t {
    uint32_t    proto;
    uint32_t    capbit;
    uint8_t     opcode;
} cmd_4b_t;

/**
 *  @brief             Set up 4 byte commands.
 *  @param chip        Pointer to chip structure.
 *
 *  @return            EOK --success otherwise fail.
 */
static int snor_set_4b_cmds(snor_chip_t *chip)
{
    int     idx;

    static const cmd_4b_t rtbl[] = {
        { .proto = SNOR_BUSPROTO_8_8_8_DTR, .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_8_8_8,     .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_4_4_4_DTR, .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_4_4_4,     .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_2_2_2_DTR, .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_2_2_2,     .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_1_1_1_DTR, .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_111_DTR_4B },
        { .proto = SNOR_BUSPROTO_1_1_1,     .capbit = SNOR_DCAPS_4BRD_111F,   .opcode = SNOR_CMD_READ_FAST_4B },
        { .proto = SNOR_BUSPROTO_1_1_1,     .capbit = SNOR_DCAPS_4BRD_111,    .opcode = SNOR_CMD_READ_4B },
        { .proto = SNOR_BUSPROTO_1_8_8_DTR, .capbit = SNOR_DCAPS_4BRD_188DTR, .opcode = SNOR_CMD_READ_188_DTR },
        { .proto = SNOR_BUSPROTO_1_8_8,     .capbit = SNOR_DCAPS_4BRD_188,    .opcode = SNOR_CMD_READ_188_4B },
        { .proto = SNOR_BUSPROTO_1_1_8,     .capbit = SNOR_DCAPS_4BRD_118,    .opcode = SNOR_CMD_READ_118_4B },
        { .proto = SNOR_BUSPROTO_1_4_4_DTR, .capbit = SNOR_DCAPS_4BRD_144DTR, .opcode = SNOR_CMD_READ_144_DTR_4B },
        { .proto = SNOR_BUSPROTO_1_4_4,     .capbit = SNOR_DCAPS_4BRD_144,    .opcode = SNOR_CMD_READ_144_4B },
        { .proto = SNOR_BUSPROTO_1_1_4,     .capbit = SNOR_DCAPS_4BRD_114,    .opcode = SNOR_CMD_READ_114_4B },
        { .proto = SNOR_BUSPROTO_1_2_2_DTR, .capbit = SNOR_DCAPS_4BRD_122DTR, .opcode = SNOR_CMD_READ_122_DTR_4B },
        { .proto = SNOR_BUSPROTO_1_2_2,     .capbit = SNOR_DCAPS_4BRD_122,    .opcode = SNOR_CMD_READ_122_4B },
        { .proto = SNOR_BUSPROTO_1_1_2,     .capbit = SNOR_DCAPS_4BRD_112,    .opcode = SNOR_CMD_READ_112_4B },
    };

    static const cmd_4b_t wtbl[] = {
        { .proto = SNOR_BUSPROTO_8_8_8_DTR, .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_8_8_8,     .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_4_4_4_DTR, .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_4_4_4,     .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_2_2_2_DTR, .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_2_2_2,     .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_1_1_1_DTR, .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_1_1_1,     .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_1_1_1,     .capbit = SNOR_DCAPS_4BPP_111, .opcode = SNOR_CMD_PP_4B },
        { .proto = SNOR_BUSPROTO_1_8_8_DTR, .capbit = SNOR_DCAPS_4BPP_188, .opcode = SNOR_CMD_PP_188_4B },
        { .proto = SNOR_BUSPROTO_1_8_8,     .capbit = SNOR_DCAPS_4BPP_188, .opcode = SNOR_CMD_PP_188_4B },
        { .proto = SNOR_BUSPROTO_1_1_8,     .capbit = SNOR_DCAPS_4BPP_118, .opcode = SNOR_CMD_PP_118_4B },
        { .proto = SNOR_BUSPROTO_1_4_4_DTR, .capbit = SNOR_DCAPS_4BPP_144, .opcode = SNOR_CMD_PP_144_4B },
        { .proto = SNOR_BUSPROTO_1_4_4,     .capbit = SNOR_DCAPS_4BPP_144, .opcode = SNOR_CMD_PP_144_4B },
        { .proto = SNOR_BUSPROTO_1_1_4,     .capbit = SNOR_DCAPS_4BPP_114, .opcode = SNOR_CMD_PP_114_4B },
    };

    for (idx = 0; idx < sizeof(rtbl) / sizeof(rtbl[0]); idx++) {
        if ((chip->rdcfg.bus_proto & rtbl[idx].proto) && (chip->dcaps & rtbl[idx].capbit)) {
            chip->op_rd.opcode = rtbl[idx].opcode;
            break;
        }
    }

    if (idx >= sizeof(rtbl) / sizeof(rtbl[0])) return (ENOTSUP);

    for (idx = 0; idx < sizeof(wtbl) / sizeof(wtbl[0]); idx++) {
        if ((chip->wrcfg.bus_proto & wtbl[idx].proto) && (chip->dcaps & wtbl[idx].capbit)) {
            chip->op_wr.opcode = wtbl[idx].opcode;
            return (EOK);
        }
    }

    return (ENOTSUP);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
