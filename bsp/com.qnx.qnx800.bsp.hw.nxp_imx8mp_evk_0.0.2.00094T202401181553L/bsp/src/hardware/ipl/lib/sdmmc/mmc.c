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
#include "mmc.h"
#include "sdmmc.h"

/* For MMC CMD6 */
#define MMC_SWITCH_MODE_OFF     24
#define MMC_SWITCH_INDEX_OFF    16
#define MMC_SWITCH_VALUE_OFF    8

static mmc_ecsd_t  ecsd;

static void mmc_parse_ext_csd(sdmmc_t *sdmmc, const uint8_t *const raw_ecsd)
{
    card_t  *card = &sdmmc->card;

    if (sdmmc->caps & HC_CAP_ACMD23) {
        card->caps |= DEV_CAP_CMD23;
    }

    ecsd.card_type = (uint8_t)(raw_ecsd[ECSD_CARD_TYPE] & ECSD_CARD_TYPE_MSK);
    ecsd.ext_csd_rev = raw_ecsd[ECSD_REV];
    ecsd.part_cfg = raw_ecsd[ECSD_PART_CONFIG];

    if (ecsd.ext_csd_rev >= ECSD_REV_V4_2) {
        card->blk_num = (uint32_t)((raw_ecsd[ECSD_SEC_CNT + 0] << 0) |
                                   (raw_ecsd[ECSD_SEC_CNT + 1] << 8) |
                                   (raw_ecsd[ECSD_SEC_CNT + 2] << 16) |
                                   (raw_ecsd[ECSD_SEC_CNT + 3] << 24));
        if (card->blk_num > ECSD_SEC_CNT_2GB) {
            card->caps   |= DEV_CAP_HC;
        }
    }

    if (ecsd.ext_csd_rev >= ECSD_REV_V4_5) {
        ecsd.driver_strength = raw_ecsd[ECSD_DRIVER_STRENGTH];
        if (!(ecsd.driver_strength & (1 << sdmmc->card.drv_type))) {
            sdmmc->card.drv_type = 0;   // fall back to default value
        }
    }

    if (ecsd.ext_csd_rev >= ECSD_REV_V5_1) {
        // Enhanced strobe in HS400 mode - HS400ES
        const uint8_t strobe = raw_ecsd[ECSD_STROBE_SUPPORT];
        if ((sdmmc->caps & HC_CAP_HS400ES) && strobe) {
            card->caps |= DEV_CAP_HS400ES;
        }
    }

    if ((sdmmc->caps & HC_CAP_HS400 ) && (ecsd.card_type & ECSD_CARD_TYPE_HS400)) {
        if (((sdmmc->caps & HC_CAP_SV_1_8V) && (ecsd.card_type & ECSD_CARD_TYPE_HS400_1_8V)) ||
            ((sdmmc->caps & HC_CAP_SV_1_2V) && (ecsd.card_type & ECSD_CARD_TYPE_HS400_1_2V))) {
            card->caps |= DEV_CAP_HS400 | DEV_CAP_HS;
            sdmmc->card.dtr_max_hs = DTR_MAX_HS400;
        }
    }

    if ((sdmmc->caps & HC_CAP_HS200) && (ecsd.card_type & ECSD_CARD_TYPE_HS200)) {
        if (((sdmmc->caps & HC_CAP_SV_1_8V) && (ecsd.card_type & ECSD_CARD_TYPE_HS200_1_8V)) ||
            ((sdmmc->caps & HC_CAP_SV_1_2V) && (ecsd.card_type & ECSD_CARD_TYPE_HS200_1_2V))) {
            card->caps |= DEV_CAP_HS200 | DEV_CAP_HS;
            sdmmc->card.dtr_max_hs = DTR_MAX_HS200;
        }
    }

    if (!(card->caps & DEV_CAP_HS) && (sdmmc->caps & HC_CAP_HS) && (ecsd.card_type & ECSD_CARD_TYPE_52MHZ)) {
        card->caps |= DEV_CAP_HS;
        sdmmc->card.dtr_max_hs = DTR_MAX_HS52;
    }

    if (!(card->caps & DEV_CAP_HS)) {
        sdmmc->card.dtr_max_hs = DTR_MAX_HS26;
    }

    if ((sdmmc->caps & HC_CAP_DDR50) && (ecsd.card_type & ECSD_CARD_TYPE_DDR)) {
        if (((sdmmc->caps & (HC_CAP_SV_3_0V | HC_CAP_SV_1_8V)) && (ecsd.card_type & ECSD_CARD_TYPE_DDR_1_8V)) ||
            ((sdmmc->caps & HC_CAP_SV_1_2V) && (ecsd.card_type & ECSD_CARD_TYPE_DDR_1_2V))) {
            card->caps |= DEV_CAP_DDR50;
        }
    }

    if (sdmmc->verbose) {
        kprintf("Ext CSD:\n");
        kprintf("    CARD_TYPE 0x%x\n", ecsd.card_type);
        kprintf("    EXT_CSD_REV 0x%x\n", ecsd.ext_csd_rev);
        kprintf("    sectors %d\n", card->blk_num);
    }
}

static int mmc_send_ext_csd(sdmmc_t *sdmmc, void *const ecsd_raw)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;
    void        *buf;

    if (sdmmc->sdmmc.alloc_dmabuf != 0) {
        buf = sdmmc->sdmmc.alloc_dmabuf(MMC_EXT_CSD_SIZE);
        if (buf == (void *)0) {
            kprintf("%s: failed to llocate DMA buffer\n");
            return SDMMC_ERROR;
        }
    } else {
        buf = ecsd_raw;
    }

    /* Get Ext CSD */
    CMD_CREATE(sdmmc, cmd, MMC_SEND_EXT_CSD, 0, NULL, MMC_EXT_CSD_SIZE, 1, buf, SCF_CTYPE_ADTC | SCF_DIR_IN | SCF_RSP_R1);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        kprintf("Failed to read ECSD register\n");
        return SDMMC_ERROR;
    }

    if (buf != ecsd_raw) {
        copy((paddr_t)ecsd_raw, (paddr_t)buf, MMC_EXT_CSD_SIZE);
    }

    return SDMMC_OK;
}

static int mmc_send_op_cond(sdmmc_t *sdmmc, const unsigned ocr, unsigned *rocr)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    card_t      *card = &sdmmc->card;
    uint32_t    rsp[4];
    int         retry;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, MMC_SEND_OP_COND, ocr, rsp, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R3);
    for (retry = 0; retry < POWER_UP_WAIT; retry++) {
        if (SDMMC_OK != hc->send_cmd(sdmmc)) {
            return SDMMC_ERROR;
        }

        if (ocr == 0) break;

        if (rsp[0] & OCR_PWRUP_CMP) break;
    }

    if (rocr) {
        *rocr = rsp[0];
    }

    /* check if time out */
    if (retry >= POWER_UP_WAIT) {
        kprintf("Failed to power up MMC card\n");
        return SDMMC_ERROR;
    }

    /* test for HC bit set */
    if (rsp[0] & OCR_HCS) {
        card->type = eMMC_HC;
    }

    return SDMMC_OK;
}

static int mmc_set_relative_addr(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    uint32_t    rsp[4];
    sdmmc_cmd_t cmd;

    sdmmc->card.rca = 1;
    CMD_CREATE(sdmmc, cmd, MMC_SET_RELATIVE_ADDR, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), rsp, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R6);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (SDMMC_OK != sdmmc_get_state(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static int mmc_switch(sdmmc_t *sdmmc, const int mode, const int index, const int value, const int cmdset)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;
    const uint32_t arg = (uint32_t) ((mode << MMC_SWITCH_MODE_OFF) |
                                     (index << MMC_SWITCH_INDEX_OFF) |
                                     (value << MMC_SWITCH_VALUE_OFF) |
                                     cmdset);

    CMD_CREATE(sdmmc, cmd, MMC_SWITCH, arg, NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1B);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) return SDMMC_ERROR;

    do {
        if (SDMMC_OK != sdmmc_get_state(sdmmc)) return SDMMC_ERROR;
    } while (CDS_CUR_STATE_PRG == (sdmmc->card.state & CDS_CUR_STATE_MSK));

    return SDMMC_OK;
}

static void mmc_parse_csd(sdmmc_t *sdmmc, const unsigned *const resp)
{
    static const uint32_t mmc_tran_speed_fu[] = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };
    static const uint32_t mmc_tran_speed_mf[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };
    card_t      *card = &sdmmc->card;
    sdmmc_csd_t *csd = &sdmmc->csd;
    int         bsize, csize, csizem;

    csd->csd_structure = (uint8_t)((resp[3] >> 30) & 0x03);
    csd->spec_vers     = (uint8_t)((resp[3] >> 26) & 0x0F);
    csd->tran_speed    = (uint8_t)(resp[3] & 0x0F);
    csd->ccc           = (resp[2] >> 20) & 0xFFF;
    csd->read_bl_len   = (uint8_t)((resp[2] >> 16) & 0x0F);
    csd->c_size        = (uint16_t)(((resp[2] & 0x3FF) << 2) | (resp[1] >> 30));
    csd->c_size_mult   = (uint8_t)((resp[1] >> 15) & 0x07);

    bsize  = 1 << csd->read_bl_len;
    csize  = (int)csd->c_size + 1;
    csizem = 1 << (csd->c_size_mult + 2);

    /* force to 512 byte block */
    if ((bsize > SDMMC_BLOCKSIZE) && ((bsize % SDMMC_BLOCKSIZE) == 0)) {
        const int ts = bsize / SDMMC_BLOCKSIZE;
        csize = csize * ts;
        bsize = SDMMC_BLOCKSIZE;
    }

    card->blk_size = (uint32_t)bsize;
    card->blk_num  = (uint32_t)(csize * csizem);
    csd->dtr_max   = mmc_tran_speed_fu[csd->tran_speed & 0x7] *
                        mmc_tran_speed_mf[(csd->tran_speed >> 3 ) & 0xf];

    if (sdmmc->verbose) {
        kprintf("CSD:\n");
        kprintf("    CSD_STRUCTURE %d\n", csd->csd_structure);

        if (IS_EMMC_CARD(card)) {
            kprintf("    SPEC_VERS %d\n", csd->spec_vers);
        }

        kprintf("    CCC 0x%x\n", csd->ccc);
        kprintf("    TRAN_SPEED %d\n", csd->tran_speed);
        kprintf("    READ_BL_LEN %x\n", csd->read_bl_len);
        kprintf("    C_SIZE %d\n", csd->c_size);
        kprintf("    C_SIZE_MULT %d\n", csd->c_size_mult);
        kprintf("    blksz %d\n", card->blk_size);
        kprintf("    sectors %d\n", card->blk_num);
        kprintf("    dtr_max %d\n", csd->dtr_max);
    }
}

static void mmc_parse_cid(sdmmc_t *sdmmc, const unsigned *const resp)
{
    sdmmc_cid_t *cid = &sdmmc->cid;
    const sdmmc_csd_t *const csd = &sdmmc->csd;

    cid->pnm[0] = (uint8_t)(resp[3] & 0x0F);
    cid->pnm[1] = (uint8_t)((resp[2] >> 24) & 0x0F) ;
    cid->pnm[2] = (uint8_t)((resp[2] >> 16) & 0x0F);
    cid->pnm[3] = (uint8_t)((resp[2] >> 8) & 0x0F);
    cid->pnm[4] = (uint8_t)(resp[2] & 0x0F);
    cid->pnm[5] = (uint8_t)((resp[1] >> 24) & 0x0F);
    cid->mdt = (resp[0] >> 8) & 0xFF;

    if (csd->spec_vers < 2) {
        cid->mid = (resp[3] >> 8) & 0xFFFFFF;
        cid->oid = 0;
        cid->prv = (resp[1] >> 8) & 0xFF;
        cid->psn = 0;
        cid->pnm[6] = (uint8_t)((resp[1] >> 16) & 0x0F);
        cid->pnm[7] = 0;
    } else {
        cid->mid = (resp[3] >> 24) & 0xFF;
        cid->oid = (resp[3] >> 8) & 0xFF;
        cid->prv = (resp[1] >> 16) & 0xFF;
        cid->psn = ((resp[1] & 0xFFFF) << 16) | (resp[0] >> 16);
        cid->pnm[6] = 0;
    }

    if (sdmmc->verbose) {
        kprintf("Card IDentification:\n");
        kprintf("    MID 0x%x\n", cid->mid);
        kprintf("    OID 0x%x\n", cid->oid);
        kprintf("    PNM %s\n", (const char *)cid->pnm);
        kprintf("    PRV 0x%x\n", cid->prv);
        kprintf("    PSN 0x%x\n", cid->psn);
    }
}

static int mmc_init_hs200_bus(sdmmc_t *const sdmmc, const int bus_width, const int timing)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    int         status = SDMMC_ERROR;
    uint8_t     sv12v, sv18v;

    if (hc->tuning == 0) return status;

    switch (timing) {
        case TIMING_HS200:
            sv12v = ECSD_CARD_TYPE_HS200_1_2V;
            sv18v = ECSD_CARD_TYPE_HS200_1_8V;
            break;
        case TIMING_HS400:
            sv12v = ECSD_CARD_TYPE_HS400_1_2V;
            sv18v = ECSD_CARD_TYPE_HS400_1_8V;
            break;
        default:
            return SDMMC_ERROR;
    }

    if ((ecsd.card_type & sv12v) && (sdmmc->caps & HC_CAP_SV_1_2V)) {
        status = hc->signal_voltage(sdmmc, SIGNAL_VOLTAGE_1_2);
        if (status != SDMMC_OK) {
            kprintf("%s: failed to switch to 1.2v signal voltage\n");
        }
    }
    if ((status != SDMMC_OK) && (ecsd.card_type & sv18v) && (sdmmc->caps & HC_CAP_SV_1_8V)) {
        status = hc->signal_voltage(sdmmc, SIGNAL_VOLTAGE_1_8);
        if (status != SDMMC_OK) {
            kprintf("%s: failed to switch to 1.8v signal voltage\n");
        }
    }

    if (status != SDMMC_OK) return (status);

    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_BUS_WIDTH, bus_width / 4, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    hc->set_bus_width(sdmmc, bus_width);

    return SDMMC_OK;
}

static int _mmc_init_hs200(sdmmc_t *const sdmmc, const int bus_width)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;

    if (hc->driver_strength != NULL) {
        sdmmc->card.drv_type = (uint32_t)hc->driver_strength(sdmmc, TIMING_HS200, sdmmc->card.drv_type);
    }
    const int val = (int)((sdmmc->card.drv_type << ECSD_HS_TIMING_DRV_TYPE_SHFT) |
                           ECSD_HS_TIMING_HS200);
    // set HS200
    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_HS_TIMING, val, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    hc->set_timing(sdmmc, TIMING_HS200);
    hc->set_clk(sdmmc, sdmmc->card.dtr_max_hs);

    if (hc->tuning(sdmmc, MMC_SEND_TUNING_BLOCK) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    return SDMMC_OK;
}

static int mmc_init_hs200(sdmmc_t *sdmmc, const int bus_width)
{
    if (mmc_init_hs200_bus(sdmmc, bus_width, TIMING_HS200) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (_mmc_init_hs200(sdmmc, bus_width) == SDMMC_OK) {
        sdmmc->card.flags |= DEV_FLAG_HS200;
        return SDMMC_OK;
    }

    return (SDMMC_ERROR);
}

static int mmc_init_hs400(sdmmc_t *sdmmc, const int bus_width, const int timing)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    const int val =  (int)((sdmmc->card.drv_type << ECSD_HS_TIMING_DRV_TYPE_SHFT) |
                           ECSD_HS_TIMING_HS400);


    if (mmc_init_hs200_bus(sdmmc, bus_width, TIMING_HS400) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (timing != TIMING_HS400ES) {
        if (_mmc_init_hs200(sdmmc, bus_width) != SDMMC_OK) {
            return SDMMC_ERROR;
        }
    }

    // set HS
    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_HS_TIMING, ECSD_HS_TIMING_HS, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    hc->set_timing(sdmmc, TIMING_HS);
    hc->set_clk(sdmmc, DTR_MAX_HS52);

    if (sdmmc_wait_card_state(sdmmc, CDS_READY_FOR_DATA, CDS_READY_FOR_DATA, 3000) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    // set buswidth to 8bit DDR / DDR ES
    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_BUS_WIDTH,
            ((timing == TIMING_HS400ES) ? ECSD_BUS_WIDTH_ES : 0) | ECSD_BUS_WIDTH_8 | ECSD_BUS_WIDTH_DDR, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    // set HS400
    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_HS_TIMING, val, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    hc->set_timing(sdmmc, timing);
    hc->set_clk(sdmmc, sdmmc->card.dtr_max_hs);

    if (sdmmc_wait_card_state(sdmmc, CDS_READY_FOR_DATA, CDS_READY_FOR_DATA, 3000) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    sdmmc->card.flags |= (timing == TIMING_HS400ES) ? DEV_FLAG_HS400ES : DEV_FLAG_HS400;

    return SDMMC_OK;
}

static int mmc_init_ddr(sdmmc_t *sdmmc, const int bus_width)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    const int  sv  = ((ecsd.card_type & ECSD_CARD_TYPE_DDR_1_2V ) && ( sdmmc->caps & HC_CAP_SV_1_2V ) ) ?
                            SIGNAL_VOLTAGE_1_2 : SIGNAL_VOLTAGE_1_8;

    // we only need to change voltage for 1_2V since 1_8V supports 1.8 to 3.3V
    if ((sv == SIGNAL_VOLTAGE_1_2) && (hc->signal_voltage(sdmmc, sv) != SDMMC_OK)) {
        return (SDMMC_ERROR);
    }

    /* switch to 8 bit mode, DDR */
    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_BUS_WIDTH,
                (bus_width / 4) | ECSD_BUS_WIDTH_DDR, MMC_SWITCH_CMDSET_DFLT) != SDMMC_OK) {
        kprintf("Failed to switch to DDR mode\n");
        return SDMMC_ERROR;
    }

    hc->set_bus_width(sdmmc, bus_width);
    hc->set_timing(sdmmc, TIMING_DDR50);

    sdmmc->card.flags |= DEV_FLAG_DDR;

    return SDMMC_OK;
}

static int mmc_bus_width(const sdmmc_t *const sdmmc, const int flag)
{
    int     bw;

    if (sdmmc->caps & HC_CAP_BW8) {
        bw = BUS_WIDTH_8;
    } else if (sdmmc->caps & HC_CAP_BW4) {
        bw = BUS_WIDTH_4;
    } else {
        bw = BUS_WIDTH_1;
    }

    if (!flag) return (bw);

#if 0
    for (; bw >= 4; bw >>= 1) {
        if ((status = mmc_switch( dev, MMC_SWITCH_CMDSET_DFLT, MMC_SWITCH_MODE_WRITE, ECSD_BUS_WIDTH, width / 4, SDIO_TIME_DEFAULT ) ) ) {
            continue;
        }

        sdio_bus_width( hc, width );

        if( ( status = mmc_bus_test( dev, width ) ) == EOK ) {
            return( width );
        }
    }
#endif

    return (BUS_WIDTH_1);
}

static int mmc_init_hs(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;

    if (sdmmc->card.dtr_max_hs > DTR_MAX_HS52) {
        sdmmc->card.dtr_max_hs = DTR_MAX_HS52;    // max dtr for HS is 52MHz
    }

    if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_HS_TIMING, ECSD_HS_TIMING_HS, MMC_SWITCH_CMDSET_DFLT)) {
        kprintf("Failed to switch to High Speed mode\n");
        return SDMMC_ERROR;
    }

    hc->set_timing(sdmmc, TIMING_HS);
    hc->set_clk(sdmmc, sdmmc->card.dtr_max_hs);

    sdmmc->card.flags |= DEV_FLAG_HS;

    return SDMMC_OK;
}

static int mmc_init_bus(sdmmc_t *const sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    const card_t     *const card = &sdmmc->card;
    const int  bus_width = mmc_bus_width(sdmmc, 0);

    if (sdmmc_set_block_length(sdmmc, SDMMC_BLOCKSIZE) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    // enable HS400 if supported
    if ((card->caps & DEV_CAP_HS400) && (card->dtr_max_hs > DTR_MAX_HS52) && bus_width >= BUS_WIDTH_8) {
        if ((card->caps & DEV_CAP_HS400ES)) {
            mmc_init_hs400(sdmmc, bus_width, TIMING_HS400ES);
        } else {
            mmc_init_hs400(sdmmc, bus_width, TIMING_HS400);
        }
    }

    // enable HS200 if supported
    if (!(card->flags & (DEV_FLAG_HS400 | DEV_FLAG_HS400ES)) && (card->caps & DEV_CAP_HS200) && (card->dtr_max_hs > DTR_MAX_HS52) && bus_width >= BUS_WIDTH_4) {
        mmc_init_hs200(sdmmc, bus_width);
    }

    // enable HS if supported
    if (!(card->flags & (DEV_FLAG_HS200 | DEV_FLAG_HS400 | DEV_FLAG_HS400ES)) && (card->caps & DEV_CAP_HS)) {
        mmc_init_hs(sdmmc);
    }

    if ((card->flags & DEV_FLAG_HS)) {
        if ((bus_width >= BUS_WIDTH_4) && (card->caps & DEV_CAP_DDR50)) { // enable DDR50 if supported
            mmc_init_ddr(sdmmc, bus_width);
        }

        if (!(card->flags & DEV_FLAG_DDR)) {    // enable 4/8 bit mode
            if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_BUS_WIDTH, bus_width / 4, MMC_SWITCH_CMDSET_DFLT) == SDMMC_OK) {
                hc->set_bus_width(sdmmc, bus_width);
            }
        }
    }

    if (!(card->flags & (DEV_FLAG_HS400ES | DEV_FLAG_HS400 | DEV_FLAG_HS200 | DEV_FLAG_HS))) {
        // we either have a device where dev->csd.spec_vers < CSD_SPEC_VER_4
        // or all higher modes have failed, so set set max ls timing
        hc->set_timing(sdmmc, TIMING_LS);
        hc->set_clk(sdmmc, sdmmc->csd.dtr_max);
    }

    return SDMMC_OK;
}

/* Get current boot partition configuration */
unsigned sdmmc_get_mmc_boot_partition(sdmmc_t *sdmmc)
{
        return ((ecsd.part_cfg & ECSD_PART_CONFIG_BOOT_PARTITION_ENABLE_MASK) >> ECSD_PART_CONFIG_BOOT_PARTITION_ENABLE_SHIFT);
}

/* Switch partition for io operations */
int sdmmc_set_mmc_partition(sdmmc_t *sdmmc, const unsigned partition)
{
        if (partition > 7) {
                kprintf("Partition index is out of bounds\n");
                return SDMMC_ERROR;
        }

        if (mmc_switch(sdmmc, MMC_SWITCH_MODE_WRITE, ECSD_PART_CONFIG, (int)(ecsd.part_cfg | partition), MMC_SWITCH_CMDSET_DFLT)) {
                kprintf("Failed to switch partition\n");
                return SDMMC_ERROR;
        }

        return SDMMC_OK;
}

int sdmmc_init_mmc(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t  *const hc = sdmmc->hc;
    card_t      *card = &sdmmc->card;
    const sdmmc_csd_t *const csd = &sdmmc->csd;
    uint32_t    ocr;
    uint32_t    cid_raw[4];
    uint32_t    csd_raw[4];

    card->type  = eMMC;
    card->rca   = 1;
    card->state = CDS_CUR_STATE_IDLE;
    card->flags = 0;
    card->caps  = 0;
    card->drv_type = DRV_TYPE_B;


    hc->set_bus_mode(sdmmc, BUS_MODE_OPEN_DRAIN);

    if (sdmmc_go_idle(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (mmc_send_op_cond(sdmmc, 0, &ocr) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (sdmmc_select_voltage(sdmmc, ocr) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (sdmmc_go_idle(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (mmc_send_op_cond(sdmmc, sdmmc->ocr | OCR_HCS, &ocr) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (sdmmc_all_send_cid(sdmmc, cid_raw) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (mmc_set_relative_addr(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    hc->set_bus_mode(sdmmc, BUS_MODE_PUSH_PULL);

    if (sdmmc_send_csd(sdmmc, csd_raw) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    mmc_parse_csd(sdmmc, csd_raw);
    mmc_parse_cid(sdmmc, cid_raw);

    if (sdmmc_select_card(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (csd->spec_vers >= CSD_SPEC_VER_4) {
        uint8_t raw_ecsd[MMC_EXT_CSD_SIZE];
        if (mmc_send_ext_csd(sdmmc, raw_ecsd) != SDMMC_OK) {
            return SDMMC_ERROR;
        }
        mmc_parse_ext_csd(sdmmc, raw_ecsd);
    }

    if (mmc_init_bus(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    sdmmc->sdmmc.blkrw   = sdmmc_read_write;
    sdmmc->sdmmc.verbose = sdmmc->verbose;

    return SDMMC_OK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/sdmmc/mmc.c $ $Rev: 980326 $")
#endif
