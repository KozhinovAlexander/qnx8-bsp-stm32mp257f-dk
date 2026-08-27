/*
 * Copyright (c) 2022-2023, BlackBerry Limited.
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

#include <stddef.h>
#include "ipl.h"
#include "sd.h"
#include "mmc.h"
#include "sdmmc.h"

/* tuning block pattern for 4 bit mode */
const uint8_t sdio_tbp_4bit[] = {
    0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
    0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
    0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
    0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
    0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
    0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
    0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
    0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

/* tuning block pattern for 8 bit mode */
const uint8_t sdio_tbp_8bit[] = {
    0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00,
    0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc, 0xcc,
    0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff, 0xff,
    0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee, 0xff,
    0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd, 0xdd,
    0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff, 0xbb,
    0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff, 0xff,
    0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee, 0xff,
    0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00,
    0x00, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0x33, 0xcc,
    0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0xcc, 0xff,
    0xff, 0xff, 0xee, 0xff, 0xff, 0xff, 0xee, 0xee,
    0xff, 0xff, 0xff, 0xdd, 0xff, 0xff, 0xff, 0xdd,
    0xdd, 0xff, 0xff, 0xff, 0xbb, 0xff, 0xff, 0xff,
    0xbb, 0xbb, 0xff, 0xff, 0xff, 0x77, 0xff, 0xff,
    0xff, 0x77, 0x77, 0xff, 0x77, 0xbb, 0xdd, 0xee,
};

static int ffs(const uint32_t val)
{
    uint32_t ret = 0x00000001;
    int idx = 1;

    while (ret) {
        if (val & ret) return(idx);
        ret <<= 1;
        idx += 1;
    }
    return(0);
}

int sdmmc_select_voltage(sdmmc_t *sdmmc, uint32_t ocr)
{
    int     bpos;
    int     status = SDMMC_OK;

    ocr &= sdmmc->ocr;

    bpos = ffs(ocr);
    if (bpos) {
        const sdmmc_hc_t *const hc = sdmmc->hc;
        ocr = (uint32_t)(1 << (bpos - 1));
        if (hc->power) {
            status = hc->power(sdmmc, ocr);
        }
    } else {
        ocr     = 0;
        status  = SDMMC_ERROR;
    }

    sdmmc->ocr = ocr;

    return (status);
}

int sdmmc_wait_card_state(sdmmc_t *const sdmmc, const uint32_t mask, const uint32_t val, uint32_t msec)
{
    msec *= 1000;
    if (msec == 0) {
        msec = 1;
    }

    for (; msec; msec--) {
        if ((sdmmc_get_state(sdmmc)) != SDMMC_OK) {
            return (SDMMC_ERROR);
        }

        if ((sdmmc->card.state & mask) == val) {
            return SDMMC_OK;
        }
    }

    kprintf("%s timeout, status = %x, mask = %x value = %x\n", sdmmc->card.state, mask, val);

    return (SDMMC_ERROR);
}

/* Gets the card state */
int sdmmc_get_state(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;
    uint32_t    rsp[4];

    CMD_CREATE(sdmmc, cmd, MMC_SEND_STATUS, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), &rsp[0],
                    0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (rsp[0] & CDS_ERROR_MSK) {
        return SDMMC_ERROR;
    }

    /* Bits 9-12 define the CURRENT_STATE */
    sdmmc->card.state = rsp[0];// & CDS_CUR_STATE_MSK;

    return SDMMC_OK;
}

int sdmmc_all_send_cid(sdmmc_t *sdmmc, uint32_t *const cid)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_ALL_SEND_CID, 0, cid, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R2);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

int sdmmc_set_block_length(sdmmc_t *sdmmc, const int blklen)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_SET_BLOCKLEN, (uint32_t)blklen, NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

int sdmmc_go_idle(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_GO_IDLE_STATE, 0, NULL, 0, 0, NULL, SCF_CTYPE_BC | SCF_RSP_NONE);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }
    return SDMMC_OK;
}

int sdmmc_send_csd(sdmmc_t *sdmmc, uint32_t *const csd)
{
    sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_SEND_CSD, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), csd, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R2);

    return (hc->send_cmd(sdmmc));
}

static int sdmmc_stop_transmission(sdmmc_t *sdmmc)
{
    sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_STOP_TRANSMISSION, 0, NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1B);

    return (hc->send_cmd(sdmmc));
}

int sdmmc_select_card(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    /* Select the Card */
    CMD_CREATE(sdmmc, cmd, MMC_SEL_DES_CARD, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (SDMMC_OK != sdmmc_get_state(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

int sdmmc_read_write(void *const ext, void *const buf, uint32_t blkno, uint32_t blkcnt, const int rd)
{
    int         op;
    uint32_t    arg;
    uint32_t    flags;
    paddr_t     addr;
    sdmmc_t     *sdmmc = ext;
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    if (sdmmc->verbose > SDMMC_VERBOSE_LVL_1) {
        kprintf("%s: blkno: %d, blkcnt: %d\n", __func__, blkno, blkcnt);
    }

    if ((blkno >= sdmmc->card.blk_num) || ((blkno + blkcnt) > sdmmc->card.blk_num) || ((blkno + blkcnt) < blkno)) {
        return SDMMC_ERROR;
    }

    addr = (paddr_t)buf;

    while (blkcnt > 0) {
        flags = (uint32_t)((rd ? SCF_DIR_IN : SCF_DIR_OUT) | SCF_CTYPE_ADTC | SCF_RSP_R1);
        if (blkcnt > 1) {
            op = MMC_READ_MULTIPLE_BLOCK;
            flags |= SCF_MULTIBLK;
            if (sdmmc->card.caps & DEV_CAP_CMD23) {
                flags |= SCF_SBC;
            }
        } else {
            op = MMC_READ_SINGLE_BLOCK;
        }

        arg = blkno;

        if ((sdmmc->card.type == eMMC) || (sdmmc->card.type == eSDC)) {
            arg *= SDMMC_BLOCKSIZE;
        }

        CMD_CREATE(sdmmc, cmd, op, arg, NULL, SDMMC_BLOCKSIZE, blkcnt, (void *)addr, flags);
        if (SDMMC_OK != hc->send_cmd(sdmmc)) {
            return SDMMC_ERROR;
        }

        if ((cmd.bcnt > 1) && !(sdmmc->card.caps & DEV_CAP_CMD23) && !(sdmmc->caps & HC_CAP_ACMD12)) {
            if (sdmmc_stop_transmission(sdmmc) != SDMMC_OK) {
                return SDMMC_ERROR;
            }
        }

        if (sdmmc_wait_card_state(sdmmc, CDS_READY_FOR_DATA, CDS_READY_FOR_DATA, 3000) != SDMMC_OK) {
            return SDMMC_ERROR;
        }

        blkcnt -= cmd.bcnt;
        blkno  += cmd.bcnt;
        addr   += cmd.bcnt * SDMMC_BLOCKSIZE;
    }

    return SDMMC_OK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/sdmmc/sdmmc.c $ $Rev: 975396 $")
#endif
