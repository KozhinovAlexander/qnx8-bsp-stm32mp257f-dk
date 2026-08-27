/*
 * Copyright (c) 2013, 2023, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017-2022, NXP.
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
 *
 */

#include <drvr/hwinfo.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/mman.h>
#include <internal.h>
#include <sys/syspage.h>
#include <inttypes.h>
#include <fcntl.h>

#ifdef SDIO_HC_IMX8

#include <soc/nxp/imx8/common/imx_usdhc.h>

#include "imx8_hc.h"

//#define IMX8_SDHCX_DEBUG

/**
 * Host controller interface
 *
 * @file       hc/imx8_hc.c
 * @addtogroup sdmmc_hc
 * @{
 */
#ifdef IMX8_SDHCX_DEBUG
static int imx_sdhcx_reg_dump(sdio_hc_t *const hc, const char *const func, const int line)
{
    const imx_sdhcx_hc_t    *sdhc;
    uintptr_t               base;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: line %d", func, line);
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: HOST_CTRL_VER %x, HOST_CTRL_CAP %x",
               __FUNCTION__,
               in32(base + IMX_USDHC_HOST_CTRL_VER),
               in32(base + IMX_USDHC_HOST_CTRL_CAP));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: SDMA_ARG2 %x, BLK %x, ARG %x, CMD %x, RSP10 %x, RSP32 %x, RSP54 %x,  RSP76 %x",
               __FUNCTION__,
               in32(base + IMX_USDHC_DS_ADDR),
               in32(base + IMX_USDHC_BLK_ATT),
               in32(base + IMX_USDHC_CMD_ARG),
               in32(base + IMX_USDHC_CMD_XFR_TYP),
               in32(base + IMX_USDHC_CMD_RSP0),
               in32(base + IMX_USDHC_CMD_RSP1),
               in32(base + IMX_USDHC_CMD_RSP2),
               in32(base + IMX_USDHC_CMD_RSP3));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: PSTATE %x, PCTL %x, SYSCTL %x, IS %x, IE %x, ISE %x, MIX %x, ADMA_ES %x, ADMA_ADDR %x",
               __FUNCTION__,
               in32(base + IMX_USDHC_PRES_STATE),
               in32(base + IMX_USDHC_PROT_CTRL),
               in32(base + IMX_USDHC_SYS_CTRL),
               in32(base + IMX_USDHC_INT_STATUS),
               in32(base + IMX_USDHC_INT_STATUS_EN),
               in32(base + IMX_USDHC_INT_SIGNAL_EN),
               in32(base + IMX_USDHC_MIX_CTRL),
               in32(base + IMX_USDHC_ADMA_ERR_STATUS),
               in32(base + IMX_USDHC_ADMA_SYS_ADDR));

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: ACMD12 %x, DLLCTRL %x, DLLSTAT %x, CTCS %x, SDC %x, SDS %x, VSPEC %x, MBOOT %x, VSPEC2 %x, TUN %x",
               __FUNCTION__,
               in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS),
               in32(base + IMX_USDHC_DLL_CTRL),
               in32(base + IMX_USDHC_DLL_STATUS),
               in32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS),
               in32(base + IMX_USDHC_STROBE_DLL_CTRL),
               in32(base + IMX_USDHC_STROBE_DLL_STATUS),
               in32(base + IMX_USDHC_VEND_SPEC),
               in32(base + IMX_USDHC_MMC_BOOT),
               in32(base + IMX_USDHC_VEND_SPEC2),
               in32(base + IMX_USDHC_TUNING_CTRL));

    return (EOK);
}
#endif

/**
 * Voltage reset using IPP_RST_N bit.
 *
 * @param hc   Host controller handle.
 *
 */
static void imx_sdhcx_voltage_reset(const sdio_hc_t *const hc)
{
#if SDHC_IPP_RESET_CTL != 0
    const imx_sdhcx_hc_t    *sdhc;
    uintptr_t               base;
    uint32_t                sctl;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    sctl = in32(base + IMX_USDHC_SYS_CTRL);

    sctl &= ~IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);
    delay(10); /* Delay for card reset */
    sctl |= IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);
#endif
}

/**
 * Common wait routine.
 *
 * @param hc   Host controller handle.
 * @param reg  Register.
 * @param mask Mask.
 * @param val  Value to check against.
 * @param usec Delay time.
 *
 * @return Execution status.
 */
static int imx_sdhcx_waitmask(const sdio_hc_t *const hc, const uint32_t reg, const uint32_t mask, const uint32_t val, const uint32_t usec)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    uint32_t                 cnt;
    int                      status;
    uint32_t                 iter;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    status = ETIMEDOUT;

    /* Fast poll for 1ms - 1us intervals */
    for (cnt = min(usec, 1000); cnt; cnt--) {
        if (((in32(base + reg)) & mask) == val) {
            status = EOK;
            break;
        }
        nanospin_ns(1000L);
    }

    if ((usec > 1000U) && status) {
        iter = usec / 1000U;
        for (cnt = iter; cnt; cnt--) {
            if (((in32(base + reg)) & mask) == val) {
                status = EOK;
                break;
            }
            delay(1);
        }
    }

    return (status);
}

/**
 * Reset tuning circuit.
 *
 * @param hc  Host controller handle.
 *
 */
static void imx_sdhcx_reset_tuning(const sdio_hc_t *const hc)
{
    const imx_sdhcx_hc_t    *const sdhc = hc->cs_hdl;
    const uintptr_t         base = sdhc->base;

    if (sdhc->tuning_mode == IMX_USDHC_TUNING_MANUAL) {
        uint32_t mix_ctrl = in32(base + IMX_USDHC_MIX_CTRL);
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        mix_ctrl &= ~IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK;
        out32(base + IMX_USDHC_MIX_CTRL, mix_ctrl);
        out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS, 0);
    } else if (sdhc->tuning_mode == IMX_USDHC_TUNING_STANDARD) {
        uint32_t acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
        acmd12 &= ~IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK;
        acmd12 &= ~IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
        out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);

        const uint32_t mask = IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
        const int status = imx_sdhcx_waitmask(hc, IMX_USDHC_AUTOCMD12_ERR_STATUS, mask, 0, 50);
        if (status == ETIMEDOUT) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                       "Failed to clear EXECUTE_TUNING bit");
        }
        /* Clear Buffer Read Ready interrupt for cmd19 */
        uint32_t intr_sts = in32(base + IMX_USDHC_INT_STATUS);
        intr_sts |= IMX_USDHC_INT_STATUS_BRR_MASK;
        out32(base + IMX_USDHC_INT_STATUS, intr_sts);
    } else {
        /* no operation. */
    }
}

/**
 * Reset.
 *
 * @param hc  Host controller handle.
 * @param rst Reset mask (type). IMX_USDHC_SYS_CTRL_RSTA_MASK - Reset for ALL.
 *                               IMX_USDHC_SYS_CTRL_RSTD_MASK - Reset DATA Line.
 *                               IMX_USDHC_SYS_CTRL_RSTC_MASK - Reset CMD Line.
 *                               IMX_USDHC_SYS_CTRL_RSTT_MASK - Reset tuning.
 *
 * @return Execution status.
 */
static int imx_sdhcx_reset(const sdio_hc_t *const hc, const uint32_t rst)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    uint32_t                 sctl;
    int                      status;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    sctl = in32(base + IMX_USDHC_SYS_CTRL);

    /* Wait up to 100 ms for reset to complete */
    out32(base + IMX_USDHC_SYS_CTRL, sctl | rst);
    status = imx_sdhcx_waitmask(hc, IMX_USDHC_SYS_CTRL, rst, 0, 100000);

    return (status);
}

/**
 * PIO transfer.
 *
 * @param hc  Host controller handle.
 * @param cmd Command.
 *
 * @return Execution status.
 */
static int imx_sdhcx_pio_xfer(sdio_hc_t *const hc, const sdio_cmd_t *const cmd)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    int                      len;
    uint32_t                 msk;
    int                      blksz;
    uint8_t                  *addr;
    int                      blks = (int)cmd->blks;
    int                      xfer_len;

#ifdef IMX8_SDHCX_DEBUG
    uint64_t cps, cycle1, cycle2 = 0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    msk = (cmd->flags & SCF_DIR_IN) ? (uint32_t)IMX_USDHC_PRES_STATE_BREN_MASK : (uint32_t)IMX_USDHC_PRES_STATE_BWEN_MASK;

    /* Multiple blocks */
    while (blks--) {
        blksz = (int)cmd->blksz;
        while (blksz) {
            if (sdio_sg_nxt(hc, &addr, &len, blksz)) {
                break;
            }
            blksz -= len;
            len /= 4;

            /* BRE or BWE is asserted when the available buffer is more than the watermark level */
            while (len) {
                xfer_len = min(len, IMX_USDHC_WTMK_LVL_WR_WML_BV_MAX_VAL);
                len -= xfer_len;

                /* Wait until the read or write buffer is ready */
                if (imx_sdhcx_waitmask(hc, IMX_USDHC_PRES_STATE, msk, msk, IMX_USDHC_TRANSFER_TIMEOUT)) {
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                               "pio read: timedout for BREN in present state register");
                    return ETIMEDOUT;
                }

                if ((cmd->flags & SCF_DIR_IN)) {
                    in32s(addr, (uint32_t)xfer_len, base + IMX_USDHC_DATA_BUFF_ACC_PORT);
                } else {
                    out32s(addr, (uint32_t)xfer_len, base + IMX_USDHC_DATA_BUFF_ACC_PORT);
                }
                addr += xfer_len * 4;
            }
        }
    }

#ifdef IMX8_SDHCX_DEBUG
    cycle2 = ClockCycles();
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: CMD %d  flags:%x, cmd->blks: %d, cmd->blksz: %d, it took %d us",
               __FUNCTION__, cmd->opcode, cmd->flags, cmd->blks, cmd->blksz,
               (int)((cycle2 - cycle1) * 1000 / (cps / 1000)));
#endif

    return (EOK);
}

/**
 * Interrupt event.
 *
 * @param hc Host controller handle.
 *
 * @return Execution status.
 */
static int imx_sdhcx_intr_event(sdio_hc_t *const hc)
{
    const imx_sdhcx_hc_t     *sdhc;
    sdio_cmd_t         *cmd;
    uint32_t            sts;
    int                 cs;
    int                 idx;
    uint32_t            rsp;
    uintptr_t           base;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    cs = CS_CMD_INPROG;
    sts = in32(base + IMX_USDHC_INT_STATUS);

#ifdef IMX8_SDHCX_DEBUG
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s: is:%x ie:%x ise:%x", __FUNCTION__,
               in32(base + IMX_USDHC_INT_STATUS),
               in32(base + IMX_USDHC_INT_STATUS_EN),
               in32(base + IMX_USDHC_INT_SIGNAL_EN));
#endif

    /*
     * Errata TKT070753, Rev 1.0, IMX6DQCE
     * on mx6q, there is low possibility that DATA END interrupt comes earlier than DMA
     * END interrupt which is conflict with standard host controller spec. In this case, read the
     * status register again will workaround this issue.
     */
    if ((sts & IMX_USDHC_INT_STATUS_TC_MASK) && !(sts & IMX_USDHC_INT_STATUS_DINT_MASK)) {
        sts = in32(base + IMX_USDHC_INT_STATUS);
    }
    /* Debounce delay when card inserted or removed. */
    if ((sts & (IMX_USDHC_INT_STATUS_CINS_MASK | IMX_USDHC_INT_STATUS_CRM_MASK))) {
        delay(100);
    }
    /* Clear interrupt events. CINS, CREM flags cleared always since sts variable
     * is loaded before debounce delay. */
    out32(base + IMX_USDHC_INT_STATUS, sts | (IMX_USDHC_INT_STATUS_CINS_MASK | IMX_USDHC_INT_STATUS_CRM_MASK));

    /* Card insertion and card removal events */
    if ((sts & (IMX_USDHC_INT_STATUS_CINS_MASK | IMX_USDHC_INT_STATUS_CRM_MASK))) {
        sdio_hc_event(hc, HC_EV_CD);
    }

    cmd = hc->wspc.cmd;
    if (cmd == NULL) {
        return (EOK);
    }

    /* Tuning commands */
    if ((cmd->opcode == SD_SEND_TUNING_BLOCK) || (cmd->opcode == MMC_SEND_TUNING_BLOCK)) {
        if ((sdhc->tuning_mode == IMX_USDHC_TUNING_MANUAL) && (sts & IMX_USDHC_INT_STATUS_BRR_MASK)) {
        /* Though we don't need the data, we need to clear the buffer */
            imx_sdhcx_pio_xfer(hc, cmd);
        }
        if (sts & IMX_USDHC_INT_STATUS_ERRI) {
            cs = CS_CMD_CMP_ERR;
        } else {
            cs = CS_CMD_CMP;
        }
        sdio_cmd_cmplt(hc, cmd, cs);
        return EOK;
    }

    /* Check of errors */
    if (sts & IMX_USDHC_INT_STATUS_ERRI) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                   "%s, ERROR in HC, CMD%d, sts: 0x%x:  is 0x%x, ac12 0x%x, IMX_USDHC_INT_STATUS_ERRI: 0x%x",
                   __FUNCTION__, cmd->opcode, sts, in32(base + IMX_USDHC_INT_STATUS),
                   in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS), IMX_USDHC_INT_STATUS_ERRI);
#ifdef IMX8_SDHCX_DEBUG
        imx_sdhcx_reg_dump(hc, __FUNCTION__, __LINE__);
#endif
        if (sts & IMX_USDHC_INT_STATUS_DTOE_MASK) {
            cs  = CS_DATA_TO_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_DCE_MASK) {
            cs = CS_DATA_CRC_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_DEBE_MASK) {
            cs  = CS_DATA_END_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_CTOE_MASK) {
            cs  = CS_CMD_TO_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_CCE_MASK) {
            cs = CS_CMD_CRC_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_CEBE_MASK) {
            cs  = CS_CMD_END_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_CIE_MASK) {
            cs  = CS_CMD_IDX_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_DMAE_MASK) {
            cs = CS_DATA_TO_ERR;
        }
        if (sts & IMX_USDHC_INT_STATUS_AC12E_MASK) {
            cs  = CS_DATA_TO_ERR;
        }
        if (!cs) {
            cs = CS_CMD_CMP_ERR;
        }
        imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK);
    } else {
        /* End of command */
        if ((sts & IMX_USDHC_INT_STATUS_CC_MASK)) {
            cs = CS_CMD_CMP;

            /* Errata ENGcm03648 */
            if (cmd->flags & SCF_RSP_BUSY) {
                int timeout = 16 * 1024 * 1024;

                while (!(in32(base + IMX_USDHC_PRES_STATE) & IMX_USDHC_PRES_STATE_DLSL0_MASK) && timeout) {
                    nanospin_ns(100L);
                    timeout--;
                }
                if (timeout <= 0) {
                    cs = CS_CMD_TO_ERR;
                    imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK);
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                               "%s: busy done wait timeout for cmd: %d", __func__, cmd->opcode);
                }
            }

            if ((cmd->flags & SCF_RSP_136)) {
                /*
                 * CRC is not included in the response reg, left
                 * shift 8 bits to match the 128 CID/CSD structure
                 */
                uint8_t rbyte = 0;
                for (idx = 0; idx < 4; idx++) {
                    rsp = in32(base + IMX_USDHC_CMD_RSP0 + (uint32_t)idx * 4);
                    cmd->rsp[3 - idx] = (rsp << 8) + rbyte;
                    rbyte = (uint8_t)(rsp >> 24);
                }
            } else if ((cmd->flags & SCF_RSP_PRESENT)) {
                cmd->rsp[0] = in32(base + IMX_USDHC_CMD_RSP0);
            } else {
                /* no operation */
            }
        }

        /* End of data transfer */
        if (sts & IMX_USDHC_INT_STATUS_TC_MASK) {
            cs = CS_CMD_CMP;
            cmd->rsp[0] = in32(base + IMX_USDHC_CMD_RSP0);
        }

        /* Doesn't need to do anything for DMA interrupt */
        if ((sts & IMX_USDHC_INT_STATUS_DINT_MASK)) {
            cs = CS_CMD_CMP;
        }

        if ((sts & (IMX_USDHC_INT_STATUS_BWR_MASK | IMX_USDHC_INT_STATUS_BRR_MASK))) {
            if (EOK == imx_sdhcx_pio_xfer(hc, cmd)) {
                cs = CS_CMD_CMP;
            } else {
                cs = CS_DATA_TO_ERR;
            }
        }
    }

    if (cs != CS_CMD_INPROG) {
        if ((cs == CS_DATA_TO_ERR) || (cs == CS_CMD_TO_ERR)) {
            /* Timeout error case, check if card removed */
            if (!(hc->entry.cd(hc) & CD_INS)) {
                cs = CS_CARD_REMOVED;
            }
        }
        sdio_cmd_cmplt(hc, cmd, cs);
    }

    return (EOK);
}

/**
 * USDHC Interrupt/Board (GPIO) specific event.
 *
 * @param hc Host controller handle.
 * @param ev Event.
 *
 * @return Execution status.
 */
static int imx_sdhcx_event(sdio_hc_t *const hc, sdio_event_t *const ev)
{
    int status;

    switch (ev->code) {
        case HC_EV_INTR:
            status = imx_sdhcx_intr_event(hc);
            InterruptUnmask(hc->cfg.irq[0], hc->hc_iid);
            break;
        default:
            status = bs_event(hc, ev);
            break;
    }

    return (status);
}

/**
 * ADMA configuration
 * @param hc Host controller handle
 * @param cmd Command
 *
 * @return Execution status
 */
static int imx_sdhcx_adma_setup(sdio_hc_t *const hc, sdio_cmd_t *const cmd)
{
    imx_sdhcx_hc_t      *sdhc;
    imx_usdhc_adma32_t  *adma;
    sdio_sge_t          *sgp;
    int                 sgc;
    int                 sgi;
    int                 acnt = 0;
    int                 alen;
    int                 sg_count;
    paddr_t             paddr;

    sdhc = hc->cs_hdl;
    adma = sdhc->adma;

    sgc = (int)cmd->sgc;
    sgp = cmd->sgl;

    if (!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    for (sgi = 0; sgi < sgc; sgi++) {
        paddr = sgp->sg_address;
        sg_count = (int)sgp->sg_count;
        while (sg_count) {
            alen = min(sg_count, IMX_USDHC_ADMA2_MAX_XFER);
            adma->attr = IMX_USDHC_ADMA2_VALID | IMX_USDHC_ADMA2_TRAN;
            adma->addr = (uint32_t)paddr;
            adma->len = (uint16_t)alen;
            sg_count -= alen;
            paddr += (paddr_t)alen;
            adma++;
            if (++acnt > ADMA_DESC_MAX) {
                return (ENOTSUP);
            }
        }
        sgp++;
    }

    adma--;
    adma->attr |= IMX_USDHC_ADMA2_END;

    out32(sdhc->base + IMX_USDHC_ADMA_SYS_ADDR, sdhc->admap);

    return (EOK);
}

/**
 * SDMA configuration.
 *
 * @param hc  Host controller handle.
 * @param cmd Command.
 *
 * @return EOK always.
 */
static int imx_sdhcx_sdma_setup(sdio_hc_t *const hc, sdio_cmd_t *const cmd)
{
    imx_sdhcx_hc_t     *sdhc;
    sdio_sge_t          *sgp;
    int                 sgc;

    sdhc = hc->cs_hdl;

    sgc = (int)cmd->sgc;
    sgp = cmd->sgl;

    if (!(cmd->flags & SCF_DATA_PHYS)) {
        sdio_vtop_sg(sgp, sdhc->sgl, sgc, cmd->mhdl);
        sgp = sdhc->sgl;
    }

    out32(sdhc->base + IMX_USDHC_DS_ADDR, (uint32_t)sgp->sg_address);

    return (EOK);
}

/**
 * Transfer setup.
 *
 * @param hc      Host controller handle.
 * @param cmd     Command.
 * @param command Command.
 * @param imask   Bits mask of USDHC_INT_STATUS_EN register.
 *
 * @return Execution status
 */
static int imx_sdhcx_xfer_setup(sdio_hc_t *const hc, sdio_cmd_t *const cmd, uint32_t *command, uint32_t *imask)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            pctl, mix_ctrl;
    int                 status = EOK;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    pctl = in32(base + IMX_USDHC_PROT_CTRL) & ~IMX_USDHC_PROT_CTRL_DMASEL_MASK;
    mix_ctrl = 0;

    /* Data present */
    *command |= IMX_USDHC_CMD_XFR_TYP_DPSEL_MASK;

    if (cmd->flags & SCF_DIR_IN) {
        mix_ctrl |=  IMX_USDHC_MIX_CTRL_DTDSEL_MASK;
    }

    *imask |= IMX_USDHC_INT_STATUS_EN_DTOESEN_MASK |
              IMX_USDHC_INT_STATUS_EN_DCESEN_MASK |
              IMX_USDHC_INT_STATUS_EN_DEBESEN_MASK |
              IMX_USDHC_INT_STATUS_EN_TCSEN_MASK;

    status = sdio_sg_start(hc, cmd->sgl, (int)cmd->sgc);

    if (cmd->sgc && (hc->caps & HC_CAP_DMA)) {
        if ((sdhc->flags & SF_USE_ADMA)) {
            status = imx_sdhcx_adma_setup(hc, cmd);
            if (status == EOK) {
                pctl |= IMX_USDHC_PROT_CTRL_DMASEL(IMX_USDHC_PROT_CTRL_DMASEL_BV_ADMA2);
            }
        } else {
            status = imx_sdhcx_sdma_setup(hc, cmd);
            if ( status == EOK) {
                pctl |= IMX_USDHC_PROT_CTRL_DMASEL(IMX_USDHC_PROT_CTRL_DMASEL_BV_SDMA);
            }
        }

        if (status == EOK) {
            *imask |= IMX_USDHC_INT_STATUS_EN_DMAESEN_MASK;
            mix_ctrl |= IMX_USDHC_MIX_CTRL_DMAEN_MASK;
        }
    }

    /* Use PIO */
    if (status || !(hc->caps & HC_CAP_DMA)) {
        if ((cmd->flags & SCF_DATA_PHYS)) {
            return (status);
        }
        status = EOK;

        *imask |= (cmd->flags & SCF_DIR_IN) ? IMX_USDHC_INT_STATUS_EN_BRRSEN_MASK : IMX_USDHC_INT_STATUS_EN_BWRSEN_MASK;
    }

    if (cmd->blks > 1) {
        mix_ctrl |= IMX_USDHC_MIX_CTRL_MSBSEL_MASK | IMX_USDHC_MIX_CTRL_BCEN_MASK;
        if ((hc->caps & HC_CAP_ACMD23) && (cmd->flags & SCF_SBC)) {
            /* Auto CMD23 need to use sdma address register to store the argument */
            out32(base + IMX_USDHC_DS_ADDR, cmd->blks);
            mix_ctrl |= IMX_USDHC_MIX_CTRL_AC23EN_MASK;
        } else if ((hc->caps & HC_CAP_ACMD12)) {
            mix_ctrl |= IMX_USDHC_MIX_CTRL_AC12EN_MASK;
        } else {
            /* no operation */
        }
    }

    sdhc->mix_ctrl = mix_ctrl;

    out32(base + IMX_USDHC_PROT_CTRL, pctl);
    out32(base + IMX_USDHC_BLK_ATT,
          cmd->blksz | (cmd->blks << IMX_USDHC_BLK_ATT_BLKCNT_SHIFT));

    return (status);
}

/**
 * Process command.
 *
 * @param hc  Host controller handle.
 * @param cmd Command.
 *
 * @return Execution status.
 */
static int imx_sdhcx_cmd(sdio_hc_t *const hc, sdio_cmd_t *const cmd)
{
    imx_sdhcx_hc_t     *sdhc;
    uintptr_t           base;
    uint32_t            pmask, pval;
    uint32_t            imask;
    int                 status;
    uint32_t            command, reg;
    const sdio_dev_t    *const dev = &hc->device;

#ifdef IMX8_SDHCX_DEBUG
    uint64_t cps, cycle1, cycle2 = 0;
    cps = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    cycle1 = ClockCycles();
#endif

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    /* Command inhibit (CMD) and CMD line signal level state */
    pmask = IMX_USDHC_PRES_STATE_CIHB_MASK | IMX_USDHC_PRES_STATE_CLSL_MASK;

    command = cmd->opcode << 24;

    if ((cmd->opcode == MMC_STOP_TRANSMISSION) || (cmd->opcode == SD_STOP_TRANSMISSION)) {
        command |= IMX_USDHC_CMD_XFR_TYP_CMDTYP(IMX_USDHC_CMD_XFR_TYP_CMDTYP_BV_ABORT);
    }

    imask = IMX_USDHC_INT_STATUS_EN_DFLTS;

    if ((cmd->flags & SCF_DATA_MSK)) {
        pmask |= IMX_USDHC_PRES_STATE_CDIHB_MASK | IMX_USDHC_PRES_STATE_DLSL0_MASK;

        if (cmd->blks) {
            status = imx_sdhcx_xfer_setup(hc, cmd, &command, &imask);
            if (status != EOK) {
                return status;
            }
        }
    } else {
        /* Enable command complete intr */
        imask |= IMX_USDHC_INT_STATUS_EN_CCSEN_MASK;
    }

    if ((cmd->flags & SCF_RSP_PRESENT)) {
        if (cmd->flags & SCF_RSP_136) {
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_136);
        } else if (cmd->flags & SCF_RSP_BUSY) {
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48B);

            /* Command 12 can be asserted even if data lines are busy */
            if ((cmd->opcode != MMC_STOP_TRANSMISSION) && (cmd->opcode != SD_STOP_TRANSMISSION)) {
                pmask |= IMX_USDHC_PRES_STATE_CDIHB_MASK | IMX_USDHC_PRES_STATE_DLSL0_MASK;
            }
        } else {
            command |= IMX_USDHC_CMD_XFR_TYP_RSPTYP(IMX_USDHC_CMD_XFR_TYP_RSPTYP_BV_48);
        }
        /* Index check */
        if (cmd->flags & SCF_RSP_OPCODE) {
            command |= IMX_USDHC_CMD_XFR_TYP_CICEN_MASK;
        }
        /* CRC check */
        if (cmd->flags & SCF_RSP_CRC) {
            command |= IMX_USDHC_CMD_XFR_TYP_CCCEN_MASK;
        }
    }

    reg = in32(base + IMX_USDHC_PRES_STATE);

    /* In some scenarios, espcially the std tuning circuit is active, usdhc controller tends
     * to set the RTR bit even when a HS sd card is inserted. Adding more restriction to
     * prevent triggering re-tune event for those cards which don't require tuning.
     */
    if (dev->flags & (DEV_FLAG_UHS | DEV_FLAG_HS200 | DEV_FLAG_HS400)) {
        /* If host requires tuning */
        if (reg & (IMX_USDHC_PRES_STATE_RTR_MASK)) {
            sdio_hc_event(hc, HC_EV_TUNE);
        }
    }

    /* Wait till card is ready to handle next command */
    pval = pmask & (IMX_USDHC_PRES_STATE_CLSL_MASK | IMX_USDHC_PRES_STATE_DLSL0_MASK);

    if ((reg & pmask) != pval) {
        status = imx_sdhcx_waitmask(hc, IMX_USDHC_PRES_STATE, pmask, pval, IMX_USDHC_COMMAND_TIMEOUT);
        if (status != EOK) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                    "%s: Timeout CMD_INHIBIT at CMD%d", __FUNCTION__, cmd->opcode);
            return (status);
        }
    }

    /* IDLE state, need to initialize clock */
    if (cmd->opcode == 0) {
        out32(base + IMX_USDHC_SYS_CTRL, (in32(base + IMX_USDHC_SYS_CTRL) | IMX_USDHC_SYS_CTRL_INITA_MASK));
        if ((status = imx_sdhcx_waitmask(hc, IMX_USDHC_SYS_CTRL, IMX_USDHC_SYS_CTRL_INITA_MASK, 0,
                                         IMX_USDHC_COMMAND_TIMEOUT))) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                       "%s: Timeout IMX_USDHC_SYS_CTRL at CMD%d", __FUNCTION__, cmd->opcode);
            return (status);
        }
    }

    sdhc->mix_ctrl |= (in32(base + IMX_USDHC_MIX_CTRL) & \
                       (IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK | \
                        IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK | \
                        IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK | \
                        IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK |
                        IMX_USDHC_MIX_CTRL_HS400_MODE_MASK |
                        IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK |
                        IMX_USDHC_MIX_CTRL_DDR_EN_MASK));

    out32(base + IMX_USDHC_MIX_CTRL, sdhc->mix_ctrl);
    sdhc->mix_ctrl = 0;

    /* Enable interrupts */
    out32(base + IMX_USDHC_INT_STATUS, IMX_USDHC_INT_STATUS_INTR_CLR_ALL);
    out32(base + IMX_USDHC_INT_STATUS_EN, imask);
    out32(base + IMX_USDHC_CMD_ARG, cmd->arg);
#if SDHC_BUS_SYNC != 0
    /* Ensure that all data are in RAM when cmd is triggered. */
    dsb();
#endif
    out32(base + IMX_USDHC_CMD_XFR_TYP, command);

#ifdef IMX8_SDHCX_DEBUG
    cycle2 = ClockCycles();
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
               "%s(), Issuing CMD%d,  cmd_arg:0x%x command:0x%x blks: %d, blksz: %d, mix_ctrl: 0x%x, it took %d us",
               __FUNCTION__, hc->wspc.cmd->opcode, cmd->arg, command, cmd->blks, cmd->blksz,
               in32(base + IMX_USDHC_MIX_CTRL), (int)((cycle2 - cycle1) * 1000 / (cps / 1000)));
#endif

    return (EOK);
}

/**
 * Abort function. Present to fulfill specification of caller layer.
 *
 * @param hc  Host controller handle.
 * @param cmd Command structure.
 *
 * @return EOK always.
 */
static int imx_sdhcx_abort(__attribute__((unused)) sdio_hc_t *const hc, __attribute__((unused)) sdio_cmd_t *const cmd)
{
    return (EOK);
}

/**
 * Configure power capabilities.
 *
 * @param hc  Host controller handle.
 * @param vdd Power.
 *
 * @return EOK always.
 */
static int imx_sdhcx_pwr(sdio_hc_t *const hc, const int vdd)
{
    const imx_sdhcx_hc_t    *sdhc;
    uintptr_t               base;
    uint32_t                reg;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    if (!hc->vdd || !vdd) {
        if (!((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC))) {
            imx_sdhcx_voltage_reset(hc);
        }
        /* Reset core */
        imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTA_MASK);
        /* reset tuning circuit (not affected by core reset) otherwise the active
         * tuning circuit tends to set the RTR bit for those cards which don't
         * require tuning.
         */
        imx_sdhcx_reset_tuning(hc);
        /* Make a clean environment */
        out32(base + IMX_USDHC_MIX_CTRL, 0);
        out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS, 0);
    }

    if (vdd) {
        reg = in32(base + IMX_USDHC_VEND_SPEC) & ~IMX_USDHC_VEND_SPEC_VSELECT_MASK;
        switch (vdd) {
            case OCR_VDD_17_195:
                reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8);
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): setting power to 1.8v", __FUNCTION__);
                break;
            case OCR_VDD_29_30:
            case OCR_VDD_30_31:
            case OCR_VDD_32_33:
            case OCR_VDD_33_34:
                reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0);
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): setting power to 3.3v", __FUNCTION__);
                break;
            default:
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                           "%s(): unrecognized voltage level. vdd: 0x%x", __FUNCTION__, vdd);
                return (EINVAL);
        }

        out32(base + IMX_USDHC_VEND_SPEC, reg);
        out32(base + IMX_USDHC_INT_SIGNAL_EN, IMX_USDHC_INT_SIGNAL_EN_DFLTS);
    }

    hc->vdd = (uint32_t)vdd;
    return (EOK);
}

/**
 * Bus mode.
 *
 * @param hc       Host controller handle.
 * @param bus_mode Bus mode.
 *
 * @return EOK always.
 */
static int imx_sdhcx_bus_mode(sdio_hc_t *const hc, const int bus_mode)
{
    hc->bus_mode = (uint32_t)bus_mode;
    return (EOK);
}

/**
 * Configure bus width.
 *
 * @param hc    Host controller handle.
 * @param width Bus width.
 *
 * @return EOK always.
 */
static int imx_sdhcx_bus_width(sdio_hc_t *const hc, const int width)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    uint32_t                 hctl;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    hctl = in32(base + IMX_USDHC_PROT_CTRL) & ~(IMX_USDHC_PROT_CTRL_DTW_MASK);

    switch (width) {
        case BUS_WIDTH_8:
            hctl |= IMX_USDHC_PROT_CTRL_DTW(IMX_USDHC_PROT_CTRL_DTW_BV_8);
            break;
        case BUS_WIDTH_4:
            hctl |= IMX_USDHC_PROT_CTRL_DTW(IMX_USDHC_PROT_CTRL_DTW_BV_4);
            break;
        case BUS_WIDTH_1:
        default:
            break;
    }

    out32(base + IMX_USDHC_PROT_CTRL, hctl);
    hc->bus_width = (uint32_t)width;
    return (EOK);
}

/**
 * Configure sdll.
 *
 * @param hc Host controller handle.
 * @param val Requested strobe delay line value.
 *
 * @return EOK if sdll set correctly.
 * @return EIO if PLL lock fail.
 */
int imx_sdhcx_host_sdll(sdio_hc_t *const hc, const unsigned val)
{
    const imx_sdhcx_hc_t    *sdhc;
    uintptr_t               base;
    uint32_t                strdll_ctrl_reg, reg;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    /* Disable clock prior to sdll manipulation */
    reg = in32(base + IMX_USDHC_VEND_SPEC);
    reg &= ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;
    out32(base + IMX_USDHC_VEND_SPEC, reg);

    if (((hc->timing == TIMING_HS400) || (hc->timing == TIMING_HS400ES))) {
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, (IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK |
                                                 IMX_USDHC_STROBE_DLL_CTRL_ENABLE_MASK |
                                                 (IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_DEFAULT_VALUE << IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_SHIFT) |
                                                 ((val << IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_SHIFT) &
                                                  IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_MASK)));
        strdll_ctrl_reg = in32(base + IMX_USDHC_STROBE_DLL_CTRL);
        strdll_ctrl_reg &= ~IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK;
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, strdll_ctrl_reg);
        nanospin_ns(100000L);
        if ((in32(base + IMX_USDHC_STROBE_DLL_STATUS) & (IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK |
                                                          IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK))
            != (IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "Strobe delay line not locked");
            return (EIO);
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "SDLL status 0x%x", in32(base + IMX_USDHC_STROBE_DLL_STATUS));
    } else {
        return ENOTSUP;
    }

    return (EOK);
}

/**
 * Configure clock frequency.
 *
 * @param hc  Host controller handle.
 * @param clk Clock value.
 *
 * @return EOK always.
 */
static int imx_sdhcx_clk(sdio_hc_t *hc, int clk)
{
    imx_sdhcx_hc_t         *sdhc;
    uintptr_t               base;
    uint32_t                sctl;
    uint32_t                pre_div = 1, divd = 1;
    int                     ddr_mode = 0;
    uint32_t                dll_ctrl_reg, strdll_ctrl_reg;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    sctl = in32(base + IMX_USDHC_SYS_CTRL);

    /* Stop clock */
    sctl &= ~(IMX_USDHC_SYS_CTRL_DTOCV_MASK | IMX_USDHC_SYS_CTRL_SDCLKFS_MASK);
    sctl |= IMX_USDHC_SYS_CTRL_DTOCV_MASK | IMX_USDHC_SYS_CTRL_RSTC_MASK | IMX_USDHC_SYS_CTRL_RSTD_MASK;
    out32(base + IMX_USDHC_SYS_CTRL, sctl);

    if ((hc->timing == TIMING_DDR50) || (hc->timing == TIMING_HS400) || (hc->timing == TIMING_HS400ES)) {
        ddr_mode = 1;
        pre_div = 2;
    }
    if ((uint32_t)clk > hc->clk_max) {
        clk = (int)hc->clk_max;
    }

    while ((hc->clk_max / (pre_div * 16) > (uint32_t)clk) && (pre_div < 256)) {
        pre_div *= 2;
    }

    while ((hc->clk_max / (pre_div * divd) > (uint32_t)clk) && (divd < 16)) {
        divd++;
    }

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 5, "desired SD clock: %d, actual: %d",
               clk, hc->clk_max / pre_div / divd);

    pre_div >>= (uint32_t)(1 + ddr_mode);
    divd--;

    if ((hc->timing == TIMING_HS200) && (clk > 100000000)) {
        out32(base + IMX_USDHC_DLL_CTRL, (IMX_USDHC_DLL_CTRL_RESET_MASK | IMX_USDHC_DLL_CTRL_ENABLE_MASK));
    } else {
        out32(base + IMX_USDHC_DLL_CTRL, 0);
    }
    if (((hc->timing == TIMING_HS400) || (hc->timing == TIMING_HS400ES)) && (clk > 100000000)) {
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, (uint32_t)(IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK |
                                                           IMX_USDHC_STROBE_DLL_CTRL_ENABLE_MASK |
                                                           (IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_DEFAULT_VALUE << IMX_USDHC_STROBE_DLL_CTRL_SLV_UPDATE_INT_SHIFT) |
                                                           ((sdhc->sdll << IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_SHIFT) &
                                                           IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_MASK)));
    } else {
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, 0);
    }

    sctl = ((0xE << IMX_USDHC_SYS_CTRL_DTOCV_SHIFT) | (pre_div << IMX_USDHC_SYS_CTRL_SDCLKFS_SHIFT) |
            (divd << IMX_USDHC_SYS_CTRL_DVS_SHIFT) | IMX_USDHC_SYS_CTRL_IPP_RST_N_MASK | 0xF);

    /* Enable clock to the card */
    out32(base + IMX_USDHC_SYS_CTRL, sctl);

    /* Wait for clock to stabilize */
    imx_sdhcx_waitmask(hc, IMX_USDHC_PRES_STATE, IMX_USDHC_PRES_STATE_SDSTB_MASK, IMX_USDHC_PRES_STATE_SDSTB_MASK, IMX_USDHC_CLOCK_TIMEOUT);

    if ((hc->timing == TIMING_HS200) && (clk > 100000000)) {
        dll_ctrl_reg = in32(base + IMX_USDHC_DLL_CTRL);
        dll_ctrl_reg &= ~IMX_USDHC_DLL_CTRL_RESET_MASK;
        out32(base + IMX_USDHC_DLL_CTRL, dll_ctrl_reg);
        IMX_INIT_DELAY;
        if ((in32(base + IMX_USDHC_DLL_STATUS) & (IMX_USDHC_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_DLL_STATUS_SLV_LOCK_MASK))
            != (IMX_USDHC_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_DLL_STATUS_SLV_LOCK_MASK)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "Delay line not locked");
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 5, "DLL status: 0x%x", in32(base + IMX_USDHC_DLL_STATUS));
    }
    if (((hc->timing == TIMING_HS400) || (hc->timing == TIMING_HS400ES)) && (clk > 100000000)) {
        strdll_ctrl_reg = in32(base + IMX_USDHC_STROBE_DLL_CTRL);
        strdll_ctrl_reg &= ~IMX_USDHC_STROBE_DLL_CTRL_RESET_MASK;
        out32(base + IMX_USDHC_STROBE_DLL_CTRL, strdll_ctrl_reg);
        IMX_INIT_DELAY;
        if ((in32(base + IMX_USDHC_STROBE_DLL_STATUS) & (IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK |
                                                          IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK))
            != (IMX_USDHC_STROBE_DLL_STATUS_REF_LOCK_MASK | IMX_USDHC_STROBE_DLL_STATUS_SLV_LOCK_MASK)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "Strobe delay line not locked");
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 5, "SDLL status 0x%x", in32(base + IMX_USDHC_STROBE_DLL_STATUS));
    }

    nanospin_ns(1000L);

    hc->clk = (uint32_t)clk;

    return (EOK);
}

/**
 * Configure timing.
 *
 * @param hc Host controller handle.
 * @param timing Timing value.
 *
 * @return EOK always.
 */
static int imx_sdhcx_timing(sdio_hc_t *hc, const int timing)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    uint32_t                 mix_ctl;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    hc->timing = (uint32_t)timing;
    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);

    if (timing == TIMING_DDR50) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_HS400) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_HS400ES) {
        mix_ctl |= IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl |= IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    } else if (timing == TIMING_LS) {
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
    } else {
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_DDR_EN_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_HS400_MODE_MASK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_EN_HS400_MODE_MASK;
    }

    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);

    if (timing == TIMING_DDR50) {
        if( hc->clk > DTR_MAX_HS52 ) {
            hc->clk = DTR_MAX_HS52;		// max dtr for HS is 52MHz
        }

        imx_sdhcx_clk( hc, (int)hc->clk );
    }

    return (EOK);
}

/**
 * Signal voltage configuration.
 *
 * @param hc             Host controller handle.
 * @param signal_voltage Signal voltage value.
 *
 * @return Execution status.
 */
static int imx_sdhcx_signal_voltage(sdio_hc_t *hc, const int signal_voltage)
{
    const imx_sdhcx_hc_t      *sdhc;
    uintptr_t                 base;
    uint32_t                  reg, dlsl;

    sdhc = hc->cs_hdl;
    base = sdhc->base;


    if ((hc->version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) || (hc->signal_voltage == (uint32_t)signal_voltage)) {
        return (EOK);
    }

    /* Its expected that eMMC operates in 1.8 V by default. In this case we skip
     * code to switch to 1.8 V */
    if ((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC)) {
        if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
            hc->signal_voltage = (uint32_t)signal_voltage;
            return (EOK);
        }
    }

    if ((signal_voltage == SIGNAL_VOLTAGE_3_3) || (signal_voltage == SIGNAL_VOLTAGE_3_0)) {
        reg = in32(base + IMX_USDHC_VEND_SPEC);
        reg &= ~IMX_USDHC_VEND_SPEC_VSELECT_MASK;
        reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_3V0);
        out32(base + IMX_USDHC_VEND_SPEC, reg);

        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: switched to  3.3V ", __FUNCTION__);
    } else if (signal_voltage == SIGNAL_VOLTAGE_1_8) {
        reg = in32(base + IMX_USDHC_VEND_SPEC);
        reg &= ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK;

        /* Stop sd clock */
        out32(base + IMX_USDHC_VEND_SPEC, reg);

        dlsl = IMX_USDHC_PRES_STATE_DLSL0_MASK |
               IMX_USDHC_PRES_STATE_DLSL1_MASK |
               IMX_USDHC_PRES_STATE_DLSL2_MASK |
               IMX_USDHC_PRES_STATE_DLSL3_MASK;
        if (imx_sdhcx_waitmask(hc, IMX_USDHC_PRES_STATE, dlsl, 0, IMX_USDHC_CLOCK_TIMEOUT)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                       "%s() : Failed to switch to 1.8V, can't stop SD Clock", __FUNCTION__);
            return (EIO);
        }
        reg |= IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8);
        out32(base + IMX_USDHC_VEND_SPEC, reg);

        /* Sleep at least 5ms */
        delay(5);

        /* Restore sd clock status */
        out32(base + IMX_USDHC_VEND_SPEC, reg | IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK);
        IMX_INIT_DELAY;

        reg = in32(base + IMX_USDHC_VEND_SPEC);

        /* Data lines should be high now */
        if (!(reg & IMX_USDHC_VEND_SPEC_VSELECT(IMX_USDHC_VEND_SPEC_VSELECT_BV_1V8)) ||
            !(in32(base + IMX_USDHC_PRES_STATE) & dlsl)) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,
                       "%s(): Failed to switch to 1.8V, DATA lines remain in low", __FUNCTION__);
            return (EIO);
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s: switched to 1.8V ", __FUNCTION__);
    } else {
        return (EINVAL);
    }

    hc->signal_voltage = (uint32_t)signal_voltage;

    /* The board specific code may need to do something  */
#ifdef BS_PAD_CONF
    bs_pad_conf(hc, SDIO_TRUE);
#endif

    return (EOK);
}

/**
 * Pre-tuning functionality.
 *
 * @param hc     Host controller handle.
 * @param tuning Tuning value.
 */
static void imx_sdhcx_pre_tuning(sdio_hc_t *const hc, const int tuning)
{
    const imx_sdhcx_hc_t     *sdhc;
    uintptr_t                base;
    uint32_t                 mix_ctl;

    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s: tuning at %d", __FUNCTION__, tuning);

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    if (!((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC))) {
        /* Reset controller before tuning otherwise tuning fails on some cards. */
        imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTA_MASK);
    }
    IMX_INIT_DELAY;

    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl |= (IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK |
                IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK |
                IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK);
    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);

    out32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS, (uint32_t)(tuning << IMX_USDHC_CLK_TUNE_CTRL_STATUS_DLY_CELL_SET_PRE_SHIFT));
}

/**
 * Send tuning command.
 *
 * @param hc Host controller handle.
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC).
 *
 * @return Execution status.
 */
static int imx_sdhcx_send_tune_cmd(sdio_hc_t *hc, const int op)
{
    struct sdio_cmd     cmd;
    int                 tlen;
    int                 status;
    sdio_sge_t          sge;
    uint32_t            *buf;

    tlen = (hc->bus_width == BUS_WIDTH_8) ? 128 : 64;

    buf = sdio_alloc((size_t)tlen);
    if (buf == NULL) {
        sdio_free_cmd(&cmd);
        return (ENOMEM);
    }

    /* Clear buffer for tuning data */
    memset(buf, 0, (size_t)tlen);

    sge.sg_count = (uint32_t)tlen;
    sge.sg_address = (paddr_t)buf;

    memset((void *)&cmd, 0, sizeof(struct sdio_cmd));
    sdio_setup_cmd(&cmd, SCF_CTYPE_ADTC | SCF_RSP_R1, (uint32_t)op, 0);

    /* Seems if not read the data out of the buffer, it will be some problems */
    sdio_setup_cmd_io(&cmd, SCF_DIR_IN, 1, tlen, &sge, 1, NULL);
    status = sdio_issue_cmd(&hc->device, &cmd, SDIO_TIME_DEFAULT);

    /* Command error */
    if ((cmd.status > CS_CMD_CMP) || (memcmp(buf, (hc->bus_width == BUS_WIDTH_8) ? sdio_tbp_8bit : sdio_tbp_4bit, (size_t)tlen))) {
        status = EIO;
    }

    sdio_free(buf, (size_t)tlen);

    return (status);
}

/**
 * Tuning routine for standard tuning execution.
 *
 * @param hc Host controller handle.
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC).
 *
 * @return Execution status.
 */
static int imx_sdhcx_std_tune(sdio_hc_t *hc, const int op)
{
    const imx_sdhcx_hc_t     *sdhc;
    uint32_t                 *buf;
    struct sdio_cmd          *cmd;
    uintptr_t                base;
    uint32_t                 acmd12, mix_ctl;
    uint16_t                 i;
    int                      tlen = 0;
    int                      status = EIO;
    unsigned                 sigen, stsen;

    sdhc = hc->cs_hdl;
    base = sdhc->base;
    tlen = (hc->bus_width == BUS_WIDTH_8) ? 128 : 64;

    if (hc->version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        return (EOK);
    }

    /* Return if not HS200 or SDR104, and not SDR50 that requires tuning */
    if ((hc->timing != TIMING_SDR104) &&
        (hc->timing != TIMING_HS200) &&
        ((hc->timing == TIMING_SDR50) &&
         !(sdhc->flags & SF_TUNE_SDR50))) {
        return (EOK);
    }

    buf = sdio_alloc((size_t)tlen);
    if (buf == NULL) {
        return (ENOMEM);
    }

    cmd = sdio_alloc_cmd();
    if (cmd == NULL) {
        sdio_free(buf, (size_t)tlen);
        return (ENOMEM);
    }

    if (!((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC))) {
        /* Reset controller before tuning otherwise tuning fails on some cards. */
        imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTA_MASK);
    }
    /* Backup interrupt settings */
    sigen = in32(base + IMX_USDHC_INT_SIGNAL_EN);
    stsen = in32(base + IMX_USDHC_INT_STATUS_EN);
    /* As per the Host Controller spec v3.00, tuning command generates Buffer Read Ready interrupt. Enable only this interrupt. */
    out32(base + IMX_USDHC_INT_SIGNAL_EN, IMX_USDHC_INT_SIGNAL_EN_BRRIEN_MASK);
    out32(base + IMX_USDHC_INT_STATUS_EN, IMX_USDHC_INT_STATUS_EN_BRRSEN_MASK);
    /* It is recommended to use internal clock as the tuning clock. */
    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl |= IMX_USDHC_MIX_CTRL_FBCLK_SEL_MASK;
    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);
    /* Enable tuning execution */
    acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
    acmd12 |= IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK;
    out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);

    for (i = 0; i < IMX_USDHC_TUNING_RETRIES; i++) {
        cmd->status = CS_CMD_INPROG;
        sdio_setup_cmd(cmd, SCF_CTYPE_ADTC | SCF_RSP_R1, (uint32_t)op, 0);
        sdio_setup_cmd_io(cmd, SCF_DIR_IN, 1, tlen, NULL, 0, NULL);
        /* Send tuning command */
        if ((status = sdio_issue_cmd(&hc->device, cmd, IMX_USDHC_TUNING_TIMEOUT))) {
            break;
        }
        /* Empty buffer */
        in32s(buf, (unsigned)(tlen >> 2), base + IMX_USDHC_DATA_BUFF_ACC_PORT);
        /* Check if tuning was successful or not. */
        acmd12 = in32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS);
        if (!(acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK)) {
            if (acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK) {
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s: Tuning pass within %d iteration.", __func__, i);
                sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s: IMX_USDHC_CLK_TUNE_CTRL_STATUS 0x%x", __func__, in32(base + IMX_USDHC_CLK_TUNE_CTRL_STATUS));
                mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
                mix_ctl |= IMX_USDHC_MIX_CTRL_AUTO_TUNE_EN_MASK;
                out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);
                delay(1); /* Make sure sdmmc finish handling of tuning data */
                break;
            }
        }
        delay(1);
    }

    if (status || !(acmd12 & IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK)) {
        status = EOK;
#ifdef IMX8_SDHCX_DEBUG
        imx_sdhcx_reg_dump(hc, __FUNCTION__, __LINE__);
#endif
        acmd12 &= ~(IMX_USDHC_AUTOCMD12_ERR_STATUS_SMP_CLK_SEL_MASK | IMX_USDHC_AUTOCMD12_ERR_STATUS_EXECUTE_TUNING_MASK);
        out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, acmd12);
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s(), failed tuning, using the fixed clock", __func__);
    }
    /* Free used buffers */
    sdio_free(buf, (size_t)tlen);
    sdio_free_cmd(cmd);
    /* Restore interrupt settings */
    out32(base + IMX_USDHC_INT_SIGNAL_EN, sigen);
    out32(base + IMX_USDHC_INT_STATUS_EN, stsen);

    return (status);
}

/**
 * Tuning routine for manual tuning execution.
 *
 * @param hc Host controller handle.
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC).
 *
 * @return Execution status.
 */
static int imx_sdhcx_man_tune(sdio_hc_t *const hc, const int op)
{
    const imx_sdhcx_hc_t    *sdhc;
    uintptr_t               base;
    uint32_t                mix_ctl;
    int                     status = EIO;
    int                     min, max;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    if (hc->version < IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
        return (EOK);
    }

    /* Return if not HS200 or SDR104, and not SDR50 that requires tuning */
    if ((hc->timing != TIMING_SDR104) &&
        (hc->timing != TIMING_HS200) &&
        ((hc->timing == TIMING_SDR50) &&
         !(sdhc->flags & SF_TUNE_SDR50))) {
        return (EOK);
    }
    /* Minimum good value */
    min = IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MIN;
    while (min < IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MAX) {
        imx_sdhcx_pre_tuning(hc, min);
        status = imx_sdhcx_send_tune_cmd(hc, op);
        if (status == EOK) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s() Found the minimum good value: %d",
                       __func__, min);
            break;
        }

        min += IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    }
    /* Maximum not-good value */
    max = min + IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    while (max < IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_MAX) {
        imx_sdhcx_pre_tuning(hc, max);
        status = imx_sdhcx_send_tune_cmd(hc, op);
        if (status != EOK) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s() Found the maximum not-good value: %d",
                       __func__, max);
            max -= IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
            break;
        }
        max += IMX_USDHC_CLK_TUNE_CTRL_STATUS_PRE_STEP;
    }

    imx_sdhcx_pre_tuning(hc, (min + max) / 2);
    if ((status = imx_sdhcx_send_tune_cmd(hc, op) != EOK)) {
        status = EIO;
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 2, "%s(), failed tuning", __func__);
    }

    mix_ctl = in32(base + IMX_USDHC_MIX_CTRL);
    mix_ctl &= ~IMX_USDHC_MIX_CTRL_EXE_TUNE_MASK;

    /* Use the fixed clock if failed */
    if (status) {
        status = EOK;
        mix_ctl &= ~IMX_USDHC_MIX_CTRL_SMP_CLK_SEL_MASK;
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1, "%s(), failed tuning, using the fixed clock",
                   __func__);
    } else {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 2, "%s(), tuned DLY_CELL_SET_PRE at: %d",
                   __func__, (min + max) / 2);
#ifdef IMX8_SDHCX_DEBUG
        imx_sdhcx_reg_dump(hc, __FUNCTION__, __LINE__);
#endif
    }

    out32(base + IMX_USDHC_MIX_CTRL, mix_ctl);
    return (status);
}

/**
 * Tuning routine select.
 *
 * @param hc Host controller handle.
 * @param op Device operand (CMD19 - SD, CMD21 - eMMC).
 *
 * @return Execution status.
 */
static int imx_sdhcx_tune(sdio_hc_t *const hc, const int op)
{
    int status = EOK;
    const imx_sdhcx_hc_t *const sdhc = hc->cs_hdl;

    if (sdhc->tuning_mode == IMX_USDHC_TUNING_STANDARD) {
        status = imx_sdhcx_std_tune(hc, op);
    } else {
        status = imx_sdhcx_man_tune(hc, op);
    }

    return (status);
}

/**
 * Card detect routine.
 *
 * @param hc Host controller handle.
 *
 * @retval SDIO_TRUE Card inserted.
 * @retval SDIO_FALSE Card removed.
 */
static int imx_sdhcx_cd(sdio_hc_t *const hc)
{
    const imx_sdhcx_hc_t      *sdhc;
    uintptr_t                 base;
    int                       status;

    sdhc = hc->cs_hdl;
    base = sdhc->base;

    status = imx_sdhcx_waitmask(hc, IMX_USDHC_PRES_STATE,
                                  IMX_USDHC_PRES_STATE_CARD_STABLE,
                                  IMX_USDHC_PRES_STATE_CARD_STABLE,
                                  100000);

    if (!(in32(base + IMX_USDHC_PRES_STATE) & IMX_USDHC_PRES_STATE_CINST_MASK)) {
        return (SDIO_FALSE);
    }

    return ((status == EOK) ? SDIO_TRUE : SDIO_FALSE);
}

/**
 * Host controller de-initialization.
 *
 * @param hc Host controller handle.
 *
 * @return EOK always.
 */
int imx_sdhcx_dinit(sdio_hc_t *hc)
{
    imx_sdhcx_hc_t *sdhc;

    if (!hc || !hc->cs_hdl) {
        return (EOK);
    }

    sdhc = hc->cs_hdl;

    if (sdhc->base) {
        imx_sdhcx_pwr(hc, 0);
        imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTA_MASK);
        out32(sdhc->base + IMX_USDHC_INT_SIGNAL_EN, 0);
        out32(sdhc->base + IMX_USDHC_INT_STATUS_EN, 0);

        if (hc->hc_iid != -1) {
            InterruptDetach(hc->hc_iid);
        }
        munmap_device_memory((void *)sdhc->base, (size_t)hc->cfg.base_addr_size[0]);
    }

    if (sdhc->adma) {
        sdio_free(sdhc->adma, sizeof(imx_usdhc_adma32_t) * ADMA_DESC_MAX);
    }
    if (sdhc->fd) {
        close(sdhc->fd);
    }

    free(sdhc);
    hc->cs_hdl = NULL;

    return (EOK);
}

static int imx_sdhcx_hs_args(sdio_hc_t *hc)
{
    sdio_hc_cfg_t   *const cfg = &hc->cfg;
    imx_sdhcx_hc_t  *const sdhc = hc->cs_hdl;
    int             opt;
    int             status = EOK;
    char            *value;
    char            *options = cfg->hsoptions;

    sdhc->sdll = IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_DEFAULT_VALUE;
    sdhc->tuning_mode = IMX_USDHC_TUNING_MANUAL;
    sdhc->std_step = IMX_USDHC_TUNING_CTRL_TUNING_STEP_DEFAULT;
    sdhc->std_start_tap = IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_DEFAULT;

    static char *const opts[] = {
#define SDLL                0
        "sdll",
#define TUNING_MODE         1
        "tuning_mode",
#define STD_TUNING_CFG      2
        "std_tuning_cfg",
        NULL
    };

    while (options && (*options != '\0')) {
        opt = sdio_hc_getsubopt( &options, opts, &value );
        if (opt == -1) {
            sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,
                       "%s: invalid HS options %s", __func__, value);
            return EINVAL;
        }

        switch (opt) {
            case SDLL:
				if ((value == NULL) || (*value == '\0')) {
					sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 0, 0, "%s: Missing argument for '%s'", __FUNCTION__, opts[ opt ] );
					status = EINVAL;
					break;
				}
                sdhc->sdll = (int)strtoull(value, NULL, 0);
                if ((sdhc->sdll << IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_SHIFT) > IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_MASK) {
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,
                               "Strobe DLL Control Slave Delay Target too high.  Resetting to %d.",
                               IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_DEFAULT_VALUE);
                    sdhc->sdll = IMX_USDHC_STROBE_DLL_CTRL_SLV_DLY_TARGET_DEFAULT_VALUE;
                }
                break;
            case TUNING_MODE:
				if ((value == NULL) || (*value == '\0')) {
					sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 0, 0, "%s: Missing argument for '%s'", __FUNCTION__, opts[ opt ] );
					status = EINVAL;
					break;
				}
                if (strncasecmp(value, "manual", sizeof("manual")) == 0) {
                    sdhc->tuning_mode = IMX_USDHC_TUNING_MANUAL;
                } else if (strncasecmp(value, "standard", sizeof("standard")) == 0) {
                    sdhc->tuning_mode = IMX_USDHC_TUNING_STANDARD;
                } else {
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,
                               "Tuning mode is invalid(%s).  Defaulting to manual.", value);
                    sdhc->tuning_mode = IMX_USDHC_TUNING_MANUAL;
                }
                break;
            case STD_TUNING_CFG:
				if ((value == NULL) || (*value == '\0')) {
					sdio_slogf( _SLOGC_SDIODI, _SLOG_ERROR, 0, 0, "%s: Missing argument for '%s'", __FUNCTION__, opts[ opt ] );
					status = EINVAL;
					break;
				}
                sdhc->std_step = (uint16_t)strtoull(value, &value, 0);
                if ((sdhc->std_step << IMX_USDHC_TUNING_CTRL_TUNING_STEP_SHIFT) > IMX_USDHC_TUNING_CTRL_TUNING_STEP_MASK) {
                    sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,
                               "Tuning Step is too high.  Resetting to %d.",
                               IMX_USDHC_TUNING_CTRL_TUNING_STEP_DEFAULT);
                    sdhc->std_step = IMX_USDHC_TUNING_CTRL_TUNING_STEP_DEFAULT;
                }
                if (*value == '^') {
                    sdhc->std_start_tap = (uint16_t)strtol(value + 1, &value, 0);
                    if ((sdhc->std_start_tap << IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_SHIFT) > IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_MASK) {
                        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 0,
                                   "Tuning Start Tap is too high. Resetting to %d.",
                                   IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_DEFAULT);
                        sdhc->std_start_tap = IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_DEFAULT;
                    }
                }
                break;
            default:
                break;
        }
    }
    return (status);
}

/**
 * Host controller initialization.
 *
 * @param hc Host controller handle.
 *
 * @return Execution status.
 */
int imx_sdhcx_init(sdio_hc_t *hc)
{
    sdio_hc_cfg_t      *cfg;
    imx_sdhcx_hc_t     *sdhc;
    uint32_t            cap;
    uintptr_t           base;
    struct sigevent     event;
    unsigned            hwi_off, mcu = 0, mcu_rev = 0;
    const hwi_tag       *tag_hwversion = NULL;
    uint32_t            tuning_ctrl;
    int                 status = EOK;


    /** Controller entry configuration (some overrides done in bs.c file) */
    static const sdio_hc_entry_t imx_sdhcx_hc_entry = {
                        .nentries           = SDIO_HC_ENTRY_NFUNCS,
                        .dinit              = imx_sdhcx_dinit,
                        .pm                 = NULL,
                        .cmd                = imx_sdhcx_cmd,
                        .abort              = imx_sdhcx_abort,
                        .event              = imx_sdhcx_event,
                        .cd                 = imx_sdhcx_cd,
                        .pwr                = imx_sdhcx_pwr,
                        .clk                = imx_sdhcx_clk,
                        .bus_mode           = imx_sdhcx_bus_mode,
                        .bus_width          = imx_sdhcx_bus_width,
                        .timing             = imx_sdhcx_timing,
                        .signal_voltage     = imx_sdhcx_signal_voltage,
                        .drv_type           = NULL,
                        .driver_strength    = NULL,
                        .tune               = imx_sdhcx_tune,
                        .preset             = NULL
                        };

    hc->hc_iid = -1;
    cfg = &hc->cfg;

    memcpy(&hc->entry, &imx_sdhcx_hc_entry, sizeof(sdio_hc_entry_t));

    if (!cfg->base_addr[0]) {
        return (ENODEV);
    }
    if (!cfg->base_addr_size[0]) {
        cfg->base_addr_size[0] = IMX_USDHC_SIZE;
    }
    sdhc = calloc(1, sizeof(imx_sdhcx_hc_t));
    if (sdhc == NULL) {
        return (ENOMEM);
    }
    hc->cs_hdl = sdhc;

    status = imx_sdhcx_hs_args(hc);
    if (status != EOK) {
        return status;
    }

    base = mmap_device_io((size_t)cfg->base_addr_size[0], cfg->base_addr[0]);
    if (base == (uintptr_t)MAP_FAILED) {
        sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1,
                   "%s: SDHCI base mmap_device_memory (0x%lx) %s",
                   __FUNCTION__, cfg->base_addr[0], strerror(errno));
        imx_sdhcx_dinit(hc);
        return (errno);
    }
    sdhc->base = base;
    sdhc->usdhc_addr = cfg->base_addr[0];
#ifdef IMX8_SDHCX_DEBUG
    imx_sdhcx_reg_dump(hc, __FUNCTION__, __LINE__);
#endif

    /* The CD_B pin mux configuration from startup routine can cause a spike on CD_B pin and this
     * will be interpreted incorrectly as a transition from "0" to "1" to cause the CRM bit is
     * being set incorretly from usdhc controller. This eventually causes the first initialization
     * process is always failed. So, all pending bits of INT_STATUS register should be clear before
     * software reset.
     */
    out32(base + IMX_USDHC_INT_STATUS, in32(base + IMX_USDHC_INT_STATUS));


    /* Device voltage reset. Needed for SDR104 cards for correct re-initialization.
     * This routine is executed for SD cards only since reset signal is ignored by device by default
     * according to the eMMC specification. */
    if (!((hc->caps & HC_CAP_SLOT_TYPE_EMBEDDED) && (hc->flags & HC_FLAG_DEV_MMC))) {
        imx_sdhcx_voltage_reset(hc);
    }
#if SDHC_CTRL_RESET_CTL != 0
    /* RM: During initialization, the Host Driver shall set this bit to 1 to reset the uSDHC */
    imx_sdhcx_reset(hc, IMX_USDHC_SYS_CTRL_RSTA_MASK);
#endif
#if SDHC_REG_RESET_CTL != 0
    /* Reset tuning circuit. It is necessary when booting from uBoot.
     * Otherwise driver will not pass initialization. */
    out32(base + IMX_USDHC_AUTOCMD12_ERR_STATUS, 0x0);
    /* iMXMP - when boot from eMMC, ROM left its configuration.
     * Reset MIX, ACMD12, DLLCTRL, VSPEC to after reset state for
     * correct driver functionality. */
    out32(base + IMX_USDHC_DLL_CTRL, IMX_USDHC_DLL_CTRL_RESET_MASK);
    out32(base + IMX_USDHC_DLL_CTRL, 0);
    out32(base + IMX_USDHC_VEND_SPEC, in32(base + IMX_USDHC_VEND_SPEC) & ~IMX_USDHC_VEND_SPEC_FRC_SDCLK_ON_MASK);
    out32(base + IMX_USDHC_MIX_CTRL, IMX_USDHC_MIX_CTRL_RESERVED31_MASK);
#endif

    if (!hc->version) {
        hc->version = in32(base + IMX_USDHC_HOST_CTRL_VER) & IMX_USDHC_HOST_CTRL_VER_SVN_MASK;
    }

    cap = in32(base + IMX_USDHC_HOST_CTRL_CAP);

    if (cfg->clk) {
        hc->clk_max = (uint32_t)cfg->clk;
    } else {
        hc->clk_max = IMX_USDHC_CLOCK_DEFAULT;
    }
    sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,"%s: Base: 0x%"PRIX64", Clk: %d Hz, Irq: %d",
               __FUNCTION__, sdhc->usdhc_addr, hc->clk_max, cfg->irq[0]);

    /* TKT357500 - Look for board and silicon version */
    hwi_off = hwi_find_device("board", 0);
    if (hwi_off != HWI_NULL_OFF) {
        tag_hwversion = hwi_tag_find(hwi_off, HWI_TAG_NAME_hwversion, NULL);
        if (tag_hwversion != NULL) {
            mcu = tag_hwversion->hwversion.hclass;
            mcu_rev = tag_hwversion->hwversion.version;
        }
        sdio_slogf(_SLOGC_SDIODI, _SLOG_INFO, hc->cfg.verbosity, 1,"%s: Mcu: %d, Mcu rev: %d", __FUNCTION__, mcu, mcu_rev);
    }

    hc->caps |= HC_CAP_BSY | HC_CAP_BW4 | HC_CAP_BW8 | HC_CAP_CD_INTR | HC_CAP_CD_WP;
    hc->caps |= HC_CAP_ACMD12 | HC_CAP_200MA | HC_CAP_DRV_TYPE_B;

    if (cap & IMX_USDHC_HOST_CTRL_CAP_HSS_MASK) {
        hc->caps |= HC_CAP_HS;
    }
    if (cap & IMX_USDHC_HOST_CTRL_CAP_DMAS_MASK) {
        hc->caps |= HC_CAP_DMA;
    }

    hc->caps |= HC_CAP_SDR104;
    hc->caps |= HC_CAP_SDR50 | HC_CAP_SDR25 | HC_CAP_SDR12;
    /* TKT357500  - In enhance HS400 mode, usdhc can not report command timeout when emmc does not send response. */
    if ((tag_hwversion != NULL) && (mcu == IMX_CHIP_TYPE_QUAD_MAX) && (mcu_rev == IMX_CHIP_REV_A)) {
        hc->caps |= HC_CAP_DDR50 | HC_CAP_HS200 | HC_CAP_HS400;
    } else {
        hc->caps |= HC_CAP_DDR50 | HC_CAP_HS200 | HC_CAP_HS400 | HC_CAP_HS400ES;
    }
    sdhc->flags |= SF_TUNE_SDR50;

    if (cap & IMX_USDHC_HOST_CTRL_CAP_VS18_MASK) {
        hc->ocr = OCR_VDD_17_195;
        hc->caps |= HC_CAP_SV_1_8V;
    }
    if ((cap & IMX_USDHC_HOST_CTRL_CAP_VS30_MASK)) {
        hc->ocr = OCR_VDD_30_31 | OCR_VDD_29_30;
        hc->caps |= HC_CAP_SV_3_0V;
    }
    if ((cap & IMX_USDHC_HOST_CTRL_CAP_VS33_MASK)) {
        hc->ocr = OCR_VDD_32_33 | OCR_VDD_33_34;
        hc->caps |= HC_CAP_SV_3_3V;
    }

#ifdef ADMA_SUPPORTED
    if ((cap & IMX_USDHC_HOST_CTRL_CAP_DMAS_MASK)) {
        if (hc->version >= IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
            hc->cfg.sg_max = ADMA_DESC_MAX;
            sdhc->adma = sdio_alloc(sizeof(imx_usdhc_adma32_t) * ADMA_DESC_MAX);
            if (sdhc->adma == NULL) {
                sdio_slogf(_SLOGC_SDIODI, _SLOG_ERROR, hc->cfg.verbosity, 1, "%s: ADMA mmap %s",
                               __FUNCTION__,
                               strerror(errno));
                imx_sdhcx_dinit(hc);
                return (errno);
            }
            sdhc->flags |= SF_USE_ADMA;
            /* Get physical address of the ADMA descriptor location */
            sdhc->admap = (uint32_t)sdio_vtop(sdhc->adma);
            if (hc->version >= IMX_USDHC_HOST_CTRL_VER_SVN_BV_VER_3) {
                hc->caps |= HC_CAP_ACMD23;
            }
        } else {
            hc->cfg.sg_max = 1;
            sdhc->flags |= SF_USE_SDMA;
        }
    }
#endif /* ADMA_SUPPORTED */
    hc->caps &= cfg->caps; /* Reconcile command line options */

    SIGEV_PULSE_INIT(&event, hc->hc_coid, (short)hc->priority, HC_EV_INTR, NULL);
    hc->hc_iid = InterruptAttachEvent(cfg->irq[0], &event,
                                           _NTO_INTR_FLAGS_TRK_MSK);
    if (hc->hc_iid == -1) {
        imx_sdhcx_dinit(hc);
        return (errno);
    }

    /* Configure watermark value */
    out32(base + IMX_USDHC_WTMK_LVL, (IMX_USDHC_WTMK_LVL_WR_WML_BV_MAX_VAL << IMX_USDHC_WTMK_LVL_WR_WML_SHIFT) |
                                     (IMX_USDHC_WTMK_LVL_RD_WML_BV_MAX_VAL << IMX_USDHC_WTMK_LVL_RD_WML_SHIFT));

    out32(base + IMX_USDHC_INT_SIGNAL_EN, IMX_USDHC_INT_SIGNAL_EN_DFLTS);
    /* Only enable the card insertion and removal interrupts */
    out32(base + IMX_USDHC_INT_STATUS_EN, IMX_USDHC_INT_STATUS_EN_CINSSEN_MASK |
                                          IMX_USDHC_INT_STATUS_EN_CRMSEN_MASK);

    if (sdhc->tuning_mode == IMX_USDHC_TUNING_STANDARD) {
        /* Enable STD tuning */
        tuning_ctrl = in32(base + IMX_USDHC_TUNING_CTRL);
        tuning_ctrl &= ~(IMX_USDHC_TUNING_CTRL_TUNING_STEP_MASK | IMX_USDHC_TUNING_CTRL_TUNING_START_TAP_MASK);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_TUNING_STEP(sdhc->std_step);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_TUNING_START_TAP(sdhc->std_start_tap);
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        tuning_ctrl |= IMX_USDHC_TUNING_CTRL_DIS_CMD_CHK_FOR_STD_TUNING_MASK;
        out32(base + IMX_USDHC_TUNING_CTRL, tuning_ctrl);
    } else {
        /* Disable STD tuning */
        tuning_ctrl = in32(base + IMX_USDHC_TUNING_CTRL);
        tuning_ctrl &= ~IMX_USDHC_TUNING_CTRL_STD_TUNING_EN_MASK;
        out32(base + IMX_USDHC_TUNING_CTRL, tuning_ctrl);
    }

    return (EOK);
}

#endif

/** @} */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devb/sdmmc/sdiodi/hc/imx8_hc.c $ $Rev: 982571 $")
#endif
