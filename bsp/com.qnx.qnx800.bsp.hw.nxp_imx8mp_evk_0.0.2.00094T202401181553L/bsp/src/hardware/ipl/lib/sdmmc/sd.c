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

static sd_scr_t         scr;
static sd_switch_cap_t  swcap;

static inline int fls(const int val)
{
    if (val == 0) return 0;
    return(sizeof(val)*8 - __builtin_clz(val));
}

static int sd_voltage_switch(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;

    CMD_CREATE(sdmmc, cmd, SD_VOLTAGE_SWITCH, 0, NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1);
    if (SDMMC_OK != hc->send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static void sd_parse_cid(sdmmc_t *sdmmc, const unsigned *const resp)
{
    sdmmc_cid_t *cid = &sdmmc->cid;

    cid->mid = (resp[3] >> 24) & 0xFF;
    cid->oid = (resp[3] >> 8) & 0xFFFF;
    cid->pnm[0] = (uint8_t)(resp[3] & 0x0F);
    cid->pnm[1] = (uint8_t)((resp[2] >> 24) & 0x0F);
    cid->pnm[2] = (uint8_t)((resp[2] >> 16) & 0x0F);
    cid->pnm[3] = (uint8_t)((resp[2] >> 8) & 0x0F);
    cid->pnm[4] = (uint8_t)(resp[2] & 0x0F);
    cid->pnm[5] = 0;
    cid->prv = (resp[1] >> 24) & 0xFF;
    cid->psn = (resp[1] << 8) | (resp[0] >> 24);
    cid->mdt = (resp[0] >> 8) & 0xFFF;

    if (sdmmc->verbose) {
        kprintf("Card IDentification:\n");
        kprintf("    MID 0x%x\n", cid->mid);
        kprintf("    OID 0x%x\n", cid->oid);
        kprintf("    PNM %s\n", (const char *)cid->pnm);
        kprintf("    PRV 0x%x\n", cid->prv);
        kprintf("    PSN 0x%x\n", cid->psn);
    }
}

static int sd_parse_csd(sdmmc_t *sdmmc, const unsigned *const resp)
{
    card_t      *card = &sdmmc->card;
    sdmmc_csd_t *csd = &sdmmc->csd;
    int         bsize, csize, csizem;
    static const uint32_t sd_tran_speed_fu[] = { 10000, 100000, 1000000, 10000000, 0, 0, 0, 0 };
    static const uint32_t sd_tran_speed_mf[] = { 0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80 };

    csd->csd_structure = (uint8_t)((resp[3] >> 30) & 0x03);
    csd->tran_speed    = (uint8_t)(resp[3] & 0xFF);
    csd->ccc           = (resp[2] >> 20) & 0xFFF;
    csd->read_bl_len   = (uint8_t)((resp[2] >> 16) & 0x0F);

    switch (csd->csd_structure) {
        case CSD_STRUCT_VER_10:         // Standard Capacity
            csd->c_size      = (uint16_t)(((resp[2] & 0x3FF) << 2) | (resp[1] >> 30));
            csd->c_size_mult = (uint8_t)((resp[1] >> 15) & 0x07);
            bsize            = 1 << csd->read_bl_len;
            csize            = csd->c_size + 1;
            csizem           = 1 << (csd->c_size_mult + 2);
            break;
        case CSD_STRUCT_VER_20:
            csd->c_size      = (uint16_t)(((resp[2] & 0x3F) << 16) | (resp[1] >> 16));
            bsize            = SDMMC_BLOCKSIZE;
            csize            = csd->c_size + 1;
            csizem           = 1024;
            card->caps      |= DEV_CAP_HC;
            break;
        case CSD_STRUCT_VER_30:         // Ultra Capacity
        default:
            return SDMMC_ERROR;
    }

    /* force to 512 byte block */
    if ((bsize > SDMMC_BLOCKSIZE) && ((bsize % SDMMC_BLOCKSIZE) == 0)) {
        const int ts = bsize / SDMMC_BLOCKSIZE;
        csize = csize * ts;
        bsize = SDMMC_BLOCKSIZE;
    }

    card->blk_size = (uint32_t)bsize;
    card->blk_num  = (uint32_t)(csize * csizem);
    csd->dtr_max   = sd_tran_speed_fu[csd->tran_speed & 0x7] * sd_tran_speed_mf[(csd->tran_speed >> 3 ) & 0xf];

    return SDMMC_OK;
}

static int sd_send_cmd(sdmmc_t *sdmmc)
{
    sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t appcmd, *cmd;

    if (sdmmc->cmd->flgsts & SCF_APP_CMD) {
        cmd = sdmmc->cmd;
        CMD_CREATE(sdmmc, appcmd, SD_APP_CMD, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), NULL, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R1);
        if (SDMMC_OK != hc->send_cmd(sdmmc)) {
            return SDMMC_ERROR;
        }
        cmd->flgsts &= ~SCF_APP_CMD;
        sdmmc->cmd = cmd;
    }

    return hc->send_cmd(sdmmc);
}

static int sd_send_if_cond(sdmmc_t *sdmmc, const unsigned vhs)
{
    sdmmc_cmd_t cmd;
    unsigned    rsp[4];

    CMD_CREATE(sdmmc, cmd, SD_SEND_IF_COND, (vhs << 8) | SD_SIC_TEST_PATTERN, rsp, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R7);
    if (SDMMC_OK != sd_send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    return (((rsp[0] & 0xff ) == SD_SIC_TEST_PATTERN) ? SDMMC_OK : SDMMC_ERROR);
}

static int sd_send_op_cond(sdmmc_t *sdmmc, const unsigned ocr, unsigned *rocr)
{
    card_t      *card = &sdmmc->card;
    sdmmc_cmd_t cmd;
    unsigned    rsp[4];
    int         i;

    for (i = 0; i < POWER_UP_WAIT; i++) {
        CMD_CREATE(sdmmc, cmd, SD_AC_SEND_OP_COND, ocr, rsp, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R3 | SCF_APP_CMD);
        if (SDMMC_OK != sd_send_cmd(sdmmc)) {
            return SDMMC_ERROR;
        }

        if (!ocr) {
            break;
        }

        if (rsp[0] & OCR_PWRUP_CMP) {
            break;
        }
    }

    /* check if time out */
    if (i >= POWER_UP_WAIT) {
        kprintf("Failed to power up SD card\n");
        return SDMMC_ERROR;
    }

    if (rocr) {
        *rocr = rsp[0];
    }

    /* test for HC bit set */
    if (rsp[0] & OCR_HCS) {
        card->type = eSDC_HC;
    } else {
        card->type = eSDC;
    }

    return SDMMC_OK;
}

static int sd_read_parse_csd(sdmmc_t *const sdmmc)
{
    const sdmmc_csd_t *const csd = &sdmmc->csd;
    const card_t      *const card = &sdmmc->card;
    unsigned    rsp[4];

    if (sdmmc_send_csd(sdmmc, rsp) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (sd_parse_csd(sdmmc, rsp) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (sdmmc->verbose) {
        kprintf("CSD:\n");
        kprintf("    CSD_STRUCTURE %d\n", csd->csd_structure);
        kprintf("    CCC 0x%x\n", csd->ccc);
        kprintf("    TRAN_SPEED %d\n", csd->tran_speed);
        kprintf("    READ_BL_LEN %x\n", csd->read_bl_len);
        kprintf("    C_SIZE %d\n", csd->c_size);
        kprintf("    C_SIZE_MULT %d\n", csd->c_size_mult);
        kprintf("    blksz %d\n", card->blk_size);
        kprintf("    sectors %d\n", card->blk_num);
        kprintf("    dtr_max %d\n", csd->dtr_max);
    }

    return SDMMC_OK;
}

static int sd_set_relative_addr(sdmmc_t *sdmmc)
{
    card_t      *card = &sdmmc->card;
    sdmmc_cmd_t cmd;
    unsigned    rsp[4];

    card->rca = 0x0001;
    CMD_CREATE(sdmmc, cmd, SD_SEND_RELATIVE_ADDR, (uint32_t)RELATIVE_CARD_ADDR(sdmmc), rsp, 0, 0, NULL, SCF_CTYPE_BCR | SCF_RSP_R6);
    if (SDMMC_OK != sd_send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    card->rca = (uint16_t)(rsp[0] >> 16);

    if (SDMMC_OK != sdmmc_get_state(sdmmc)) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static int sd_switch(sdmmc_t *sdmmc, const int mode, const int group, const int value, unsigned char *const buf)
{
    sdmmc_cmd_t cmd;
    unsigned    arg;
    void        *tbuf;

    if (sdmmc->sdmmc.alloc_dmabuf != 0) {
        tbuf = sdmmc->sdmmc.alloc_dmabuf(SD_SF_STATUS_SIZE);
        if (tbuf == (void *)0) {
            kprintf("%s: failed to llocate DMA buffer\n");
            return SDMMC_ERROR;
        }
    } else {
        tbuf = buf;
    }

    /* By default group function is all "1" */
    arg = (uint32_t)((mode << SD_SF_MODE_OFF) | 0x00ffffff);
    arg &= ~(SD_SF_CUR_FCN << (group * SD_SF_GRP_WIDTH));
    arg |= (value << (group * SD_SF_GRP_WIDTH));

    CMD_CREATE(sdmmc, cmd, SD_SWITCH_FUNC, arg, NULL, SD_SF_STATUS_SIZE, 1, tbuf, SCF_CTYPE_ADTC | SCF_DIR_IN | SCF_RSP_R1);
    if (SDMMC_OK != sd_send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (buf != tbuf) {
        copy((paddr_t)buf, (paddr_t)tbuf, MMC_EXT_CSD_SIZE);
    }

    if (sdmmc_wait_card_state(sdmmc, CDS_READY_FOR_DATA, CDS_READY_FOR_DATA, 3000) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static int sd_read_parse_scr(sdmmc_t *sdmmc)
{
    sdmmc_cmd_t cmd;
    uint32_t    dbuf[4];
    uint8_t     *buf;

    if (sdmmc->sdmmc.alloc_dmabuf != 0) {
        buf = sdmmc->sdmmc.alloc_dmabuf(SD_SF_STATUS_SIZE);
        if (buf == (void *)0) {
            kprintf("%s: failed to llocate DMA buffer\n");
            return SDMMC_ERROR;
        }
    } else {
        buf = (uint8_t *)&dbuf[0];
    }

    CMD_CREATE(sdmmc, cmd, SD_AC_SEND_SCR, 0, NULL, SD_SCR_SIZE, 1, buf, SCF_CTYPE_ADTC | SCF_DIR_IN | SCF_RSP_R1 | SCF_APP_CMD);
    if (SDMMC_OK != sd_send_cmd(sdmmc)) {
        return SDMMC_ERROR;
    }

    scr.scr_structure = (uint8_t)((buf[0] >> 4) & 0xF);
    scr.sd_spec       = (uint8_t)(buf[0] & 0xF);
    scr.sd_bus_widths = (uint8_t)(buf[1] & 0xF);
    scr.sd_spec3      = (uint8_t)((buf[2] >> 7) & 0x1);

    if (scr.sd_spec < CSD_SPEC_VER_1) {
        kprintf("CSR: SD version < 1, no high speed support\n");
    }

    if (sdmmc->verbose) {
        kprintf("SD Card Configuration:\n");
        kprintf("    SCR_STRUCTURE: %d\n", scr.scr_structure);
        kprintf("    SD_SPEC: %d\n", scr.sd_spec);
        kprintf("    SD_BUS_WIDTHS: %d\n", scr.sd_bus_widths);
        kprintf("    SD_SPEC3: %d\n", scr.sd_spec3);
    }
    return SDMMC_OK;
}

static int sd_read_switch(sdmmc_t *sdmmc)
{
    card_t      *card = &sdmmc->card;
    uint32_t    dbuf[16];
    uint8_t     *buf = (uint8_t *)&dbuf[0];

    if (!(sdmmc->csd.ccc & CCC_SWITCH)) {
        return SDMMC_OK;
    }

    buf[13] = 0;
    buf[9]  = 0;
    buf[7]  = 0;
    /* Get the current bus speed */
    if (sd_switch(sdmmc, SD_SF_MODE_CHECK, SD_SF_GRP_BUS_SPD, SD_SF_CUR_FCN, buf)) {
        kprintf("sd_capabilities: check mode failed\n");
        return SDMMC_ERROR;
    }

    /* SDHC card */
    if ((buf[13] & 0x2 ) ) {
        swcap.dtr_max_hs  = 50000000;
        if ((sdmmc->caps & HC_CAP_HS)) {
            card->caps |= DEV_CAP_HS;
        }
    }

    if (scr.sd_spec3 ) {
        swcap.bus_mode   = (uint32_t)(buf[13] & SD_BUS_MODE_MSK);
        swcap.drv_type   = (uint32_t)(buf[9] & SD_DRV_TYPE_MSK);
        swcap.curr_limit = (uint32_t)(buf[7] & SD_CURR_LIMIT_MSK);

        if ((swcap.bus_mode & SD_BUS_MODE_UHS)) {
            card->caps |= DEV_CAP_UHS;
        }
    }

    return SDMMC_OK;
}

static int sd_switch_hs(sdmmc_t *const sdmmc)
{
    uint32_t dbuf[16];
    uint8_t  *const buf = (uint8_t *)&dbuf[0];

    /* switch to HS */
    if (sd_switch(sdmmc, SD_SF_MODE_SET, SD_SF_GRP_BUS_SPD, 1, buf) == SDMMC_OK) {
        if ((buf[16] & 0xF) == 0x1) {
            return SDMMC_OK;
        }
    }

    return SDMMC_ERROR;
}

static int sd_set_bus_width(sdmmc_t *sdmmc, const int width)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    sdmmc_cmd_t cmd;
    uint32_t bus_width = 0;

    switch (width) {
        case BUS_WIDTH_1:
            bus_width = SD_BUS_WIDTH_1;
            break;

        case BUS_WIDTH_4:
            if ((sdmmc->caps & HC_CAP_BW4) && (scr.sd_bus_widths & SCR_BUS_WIDTH_4)) {
                bus_width = SD_BUS_WIDTH_4;
            } else {
                return SDMMC_ERROR;
            }
            break;
        case BUS_WIDTH_8:
        default:
            return SDMMC_ERROR;
    }

    /* switch bus width */
    CMD_CREATE(sdmmc, cmd, SD_AC_SET_BUS_WIDTH, bus_width, NULL, 0, 0, NULL, SCF_CTYPE_AC | SCF_RSP_R1 | SCF_APP_CMD);
    if (SDMMC_OK != sd_send_cmd(sdmmc)) {
        kprintf("Failed to switch to bus width 4\n");
        return SDMMC_ERROR;
    }

    hc->set_bus_width(sdmmc, width);

    return SDMMC_OK;
}

static int sd_capabilities(sdmmc_t *const sdmmc)
{
    if (sd_read_parse_scr(sdmmc)) {
        kprintf("sd_capabilities: read SCR failed\n");
        return SDMMC_ERROR;
    }

    if (sd_read_switch(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    return SDMMC_OK;
}

static int sd_set_bus_speed_mode(sdmmc_t *const sdmmc, const int bus_spd)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    int         idx;
    int         status;
    static const int  dtr[5]      = { DTR_MAX_SDR12, DTR_MAX_SDR25, DTR_MAX_SDR50,
                                DTR_MAX_SDR104, DTR_MAX_DDR50 };
    static const int  timing[5]   = { TIMING_SDR12, TIMING_SDR25, TIMING_SDR50,
                                TIMING_SDR104, TIMING_DDR50 };
    uint8_t     sw_status[64] = { 0 };

    if (!bus_spd) {
        return SDMMC_OK;
    }

    idx	= fls(bus_spd) - 1;   // get index to fastest speed

    if (idx >= NELEMS(timing)) {
        return SDMMC_ERROR;
    }

    status = sd_switch(sdmmc, SD_SF_MODE_SET, SD_SF_GRP_BUS_SPD, idx, sw_status);
    if (status == SDMMC_OK) {
        if ((sw_status[16] & 0xF) == idx) {
            hc->set_timing(sdmmc, timing[idx]);
            hc->set_clk(sdmmc, dtr[idx]);
        }
        else {
            status = SDMMC_ERROR;
        }
    }

    if (status) {
        kprintf("Error Switching speed %d\n", idx);
    }

    return status;
}

static int sd_set_current_limit(sdmmc_t *const sdmmc, const int bus_spd, int curr_limit)
{
    int        idx = 0;
    int        status;
    uint8_t    sw_status[64] = { 0 };

    // reconcile hc and device current limits
    curr_limit &= HC_CAP_CURRENT(sdmmc->caps);

    if (curr_limit && (bus_spd & (SD_BUS_MODE_SDR50 | SD_BUS_MODE_SDR104 | SD_BUS_MODE_DDR50))) {
        idx = fls(curr_limit) - 1;	// get index to highest current
    }

    status = sd_switch(sdmmc, SD_SF_MODE_SET, SD_SF_GRP_CUR_LMT, idx, sw_status);
    if (status == SDMMC_OK) {
        if (((sw_status[15] >> 4) & 0x0F) != idx) {
            status = SDMMC_ERROR;
        }
    }

    if (status) {
        kprintf("Error Switching speed %d, current %d\n", bus_spd, idx);
    }

    return status;
}

static int sd_set_drv_type(sdmmc_t *const sdmmc, const int bus_spd, int drv_type)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    int        idx;
    int        status;
    int        driver_strength;
    static  const int  timing[5]	= { TIMING_SDR12, TIMING_SDR25, TIMING_SDR50,
                                TIMING_SDR104, TIMING_DDR50 };
    uint8_t    sw_status[64] = { 0 };

    if (!HC_CAP_DRV_TYPES(sdmmc->caps) ||
        !hc->driver_strength) {
        return SDMMC_OK;
    }

    idx = (bus_spd) ? (fls(bus_spd) - 1) : 0;

    if (idx >= NELEMS(timing)) {
        return SDMMC_ERROR;
    }

    drv_type = hc->driver_strength(sdmmc, timing[idx], drv_type | SD_DRV_TYPE_B);
    driver_strength = fls(drv_type) - 1;

    status = sd_switch(sdmmc, SD_SF_MODE_SET, SD_SF_GRP_DRV_STR, driver_strength, sw_status);
    if (status == SDMMC_OK) {
        if (((sw_status[15] & 0xF) == driver_strength) &&
            (hc->drv_type != NULL)) {
            hc->drv_type(sdmmc, drv_type);
        } else {
            status = SDMMC_ERROR;
        }
    }

    if (status) {
        kprintf("Error Switching drive strength %d\n", driver_strength);
    }

    return status;
}

static int sd_select_bus_mode(const sdmmc_t *const sdmmc, const int bus_mode)
{
    int        bus_spd_mode;

    if ((bus_mode & SD_BUS_MODE_SDR104) && (sdmmc->caps & HC_CAP_SDR104)) {
        bus_spd_mode = SD_BUS_MODE_SDR104;
    } else if ((bus_mode & SD_BUS_MODE_DDR50) && (sdmmc->caps & HC_CAP_DDR50)) {
        bus_spd_mode = SD_BUS_MODE_DDR50;
    } else if ((bus_mode & SD_BUS_MODE_SDR50) && (sdmmc->caps & HC_CAP_SDR50)) {
        bus_spd_mode = SD_BUS_MODE_SDR50;
    } else if ((bus_mode & SD_BUS_MODE_SDR25) && (sdmmc->caps & HC_CAP_SDR25)) {
        bus_spd_mode = SD_BUS_MODE_SDR25;
    } else {
        bus_spd_mode = SD_BUS_MODE_SDR12;
    }
    return bus_spd_mode;
}

static int sd_init_uhs(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    card_t     *card = &sdmmc->card;
    int        status;
    int        bus_spd_mode;

    status = sd_set_bus_width(sdmmc, BUS_WIDTH_4);
    if (status != SDMMC_OK) {
        return status;
    }

    bus_spd_mode = sd_select_bus_mode(sdmmc, (int)swcap.bus_mode);

    status = sd_set_drv_type(sdmmc, bus_spd_mode, (int)swcap.drv_type);
    if (status != SDMMC_OK) {
        return status;
    }

    status = sd_set_current_limit(sdmmc, bus_spd_mode, (int)swcap.curr_limit);
    if (status != SDMMC_OK) {
        return status;
    }

    status = sd_set_bus_speed_mode(sdmmc, bus_spd_mode);
    if (status != SDMMC_OK) {
        return status;
    }

    status = hc->tuning(sdmmc, SD_SEND_TUNING_BLOCK);
    if (status != SDMMC_OK) {
            // tuning may have failed, but we should should be able to continue.
            // ie hc will use defaults an we will re-tune if we get a CRC error
        kprintf("tuning failure (%d)\n", status);
    }
    card->flags |= DEV_FLAG_UHS;
    return SDMMC_OK;
}

static int sd_init_bus(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    card_t      *card = &sdmmc->card;
    uint32_t    dtr;
    int         timing;

    dtr     = sdmmc->csd.dtr_max;
    timing  = TIMING_LS;

    if (sdmmc->signal_voltage == SIGNAL_VOLTAGE_1_8) {
         if ((sd_init_uhs(sdmmc)) != SDMMC_OK) {
            return SDMMC_ERROR;
         }
    } else {
        if ((card->caps & DEV_CAP_HS)) {    // HS card
            if ((sd_switch_hs(sdmmc)) != SDMMC_OK ) {
                return SDMMC_ERROR;
            }

            timing      = TIMING_HS;
            dtr         = swcap.dtr_max_hs;
            card->flags |= DEV_FLAG_HS;
        }

        if ((sdmmc->caps & HC_CAP_BW4) && (scr.sd_bus_widths & SCR_BUS_WIDTH_4)) {
            if ((sd_set_bus_width(sdmmc, BUS_WIDTH_4)) != SDMMC_OK) {
                return SDMMC_ERROR;
            }
        }

        hc->set_timing(sdmmc, timing);
        hc->set_clk(sdmmc, dtr);
    }
    return SDMMC_OK;
}

int sdmmc_init_sd(sdmmc_t *sdmmc)
{
    const sdmmc_hc_t *const hc = sdmmc->hc;
    card_t *card = &sdmmc->card;
    unsigned ocr, rocr;
    unsigned cid_raw[4];
    int status = SDMMC_ERROR;
    uint32_t retries = 5;

    card->type  = NONE;
    card->rca   = 0;
    card->state = CDS_CUR_STATE_IDLE;
    card->flags = 0;
    card->caps  = 0;

    hc->set_bus_mode(sdmmc, BUS_MODE_OPEN_DRAIN);

    if (sdmmc_go_idle(sdmmc)) {
        return SDMMC_ERROR;
    }

    /* Some LS cards have extremely long setup time and they tend
     * to fail in the first SD_SEND_IF_COND command.
     */
    do {
        status = sd_send_if_cond(sdmmc, SD_SIC_VHS_27_36V);
        retries --;
    } while ((retries != 0) && (status == SDMMC_ERROR));
    if (status == SDMMC_OK) {
        card->type = eSDC_V200;
    } else {
        kprintf("V1 card not supported.\n");
        return SDMMC_ERROR;
    }

    if (sd_send_op_cond(sdmmc, 0, &ocr)) {
        return SDMMC_ERROR;
    }

    if (sdmmc_select_voltage(sdmmc, ocr) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    if (SDMMC_OK != sd_send_if_cond(sdmmc, SD_SIC_VHS_27_36V)) {
        return SDMMC_ERROR;
    }

    if (sdmmc_go_idle(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (SDMMC_OK != sd_send_if_cond(sdmmc, SD_SIC_VHS_27_36V)) {
        return SDMMC_ERROR;
    }

    ocr = sdmmc->ocr;
    // HC supports 1.8V signalling
    if (HC_CAP_UHS(sdmmc->caps) && (sdmmc->caps & HC_CAP_SV_1_8V)) {
        ocr |= OCR_S18R;
    }

    // verify HC can supply > 150ma
    if (HC_CAP_XPC(sdmmc->caps)) {
        ocr |= OCR_XPC;
    }

    while (1) {
        if (sd_send_op_cond(sdmmc, ocr | OCR_HCS | OCR_PWRUP_CMP, &rocr)) {
            return SDMMC_ERROR;
        }

        // Switch signal voltage
        if (((rocr & (OCR_HCS | OCR_S18A)) == (OCR_HCS | OCR_S18A)) &&
            (hc->signal_voltage != 0)) {
            if (sd_voltage_switch(sdmmc)) {
                ocr &= ~OCR_S18R;
                continue;
            }

            if (hc->signal_voltage(sdmmc, SIGNAL_VOLTAGE_1_8)) {
                 return SDMMC_ERROR;
            }
            sdmmc->signal_voltage = SIGNAL_VOLTAGE_1_8;
        }
        break;
    }

    if (sdmmc_all_send_cid(sdmmc, cid_raw)) {
        return SDMMC_ERROR;
    }

    if (sd_set_relative_addr(sdmmc)) {
        return SDMMC_ERROR;
    }

    hc->set_bus_mode(sdmmc, BUS_MODE_PUSH_PULL);

    if (sd_read_parse_csd(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (!(sdmmc->caps & HC_CAP_SKIP_IDENT)) {
        sd_parse_cid(sdmmc, cid_raw);
    }

    if (sdmmc_select_card(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (sd_capabilities(sdmmc)) {
        return SDMMC_ERROR;
    }

    if (sdmmc_set_block_length(sdmmc, SDMMC_BLOCKSIZE) != SDMMC_OK) {
        return (SDMMC_ERROR);
    }

    if (sd_init_bus(sdmmc) != SDMMC_OK) {
        return SDMMC_ERROR;
    }

    sdmmc->sdmmc.blkrw   = sdmmc_read_write;
    sdmmc->sdmmc.verbose = sdmmc->verbose;

    return SDMMC_OK;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/sdmmc/sd.c $ $Rev: 979772 $")
#endif
