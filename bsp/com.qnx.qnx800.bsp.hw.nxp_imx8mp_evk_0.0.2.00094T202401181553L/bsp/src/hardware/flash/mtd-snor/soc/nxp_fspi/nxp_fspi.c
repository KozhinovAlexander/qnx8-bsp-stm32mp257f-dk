/*
 * Copyright (c) 2023, BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nxp_fspi.h"

#define NXP_FSPI_DATA_NONE  0
#define NXP_FSPI_DATA_READ  1
#define NXP_FSPI_DATA_WRITE 2
/* SNOR signature for this OP code plus bus protocol */
#define SNOR_LUT_SIG(op, proto)      (((proto) << 8) | (op))

typedef struct {
    uint8_t op;
    uint8_t pad;
    uint8_t cmd;
} lut_t;

static int nxp_fspi_options(nxp_fspi_t *dev)
{
    char    *value, *freeptr, *options;
    int     opt;
    int     ret = EOK;
    static char *fspi_opts[] = {
        "base",
#define OPT_BASE        0
        "irq",
#define OPT_IRQ         1
        "abma_base",
#define OPT_AMBA_BASE   2
        "octcomb_en",
#define OPT_OCTCOMB_EN  3
        "rxclksrc",
#define OPT_RXCLKSRC    4
        NULL
    };

    freeptr = dev->ctrl.soc_opts;
    if (freeptr == NULL) return (ret);

    options = freeptr;
    while ((options) && (*options != '\0') && (ret == EOK)) {
        opt = snor_soc_getsubopt(&options, fspi_opts, &value);
        switch (opt) {
            case OPT_BASE:
                ret = snor_options_arg_value(__func__, fspi_opts[opt], value);
                if (ret == EOK) {
                    dev->pbase = (paddr_t)strtoul(value, NULL, 0);
                }
                break;
            case OPT_IRQ:
                ret = snor_options_arg_value(__func__, fspi_opts[opt], value);
                if (ret == EOK) {
                    dev->irq = (int)strtoul(value, NULL, 0);
                    snor_slogf(_SLOG_INFO, dev->ctrl.verbosity, _SLOG_INFO, "%s: irq=%d", __func__, dev->irq);
                }
                break;
            case OPT_AMBA_BASE:
                ret = snor_options_arg_value(__func__, fspi_opts[opt], value);
                if (ret == EOK) {
                    dev->amba_base = (uint32_t)strtoul(value, NULL, 0);
                }
                break;
            case OPT_OCTCOMB_EN:
                ret = snor_options_arg_value(__func__, fspi_opts[opt], value);
                if (ret == EOK) {
                    dev->octcomb_en = (uint32_t)strtoul(value, NULL, 0);
                }
                break;
            case OPT_RXCLKSRC:
                ret = snor_options_arg_value(__func__, fspi_opts[opt], value);
                if (ret == EOK) {
                    dev->rxclksrc = (uint32_t)strtoul(value, NULL, 0);
                }
                break;
            default:
                snor_slogf(_SLOG_CRITICAL, dev->ctrl.verbosity, 0,
                        "%s: unknown option %s", __func__, options);
                ret = EINVAL;
                break;
        }

        if (ret != EOK) break;
    }

    free(freeptr);
    dev->ctrl.soc_opts = NULL;

    return (ret);
}

static void dump_registers(const nxp_fspi_t *const dev)
{
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_MCR0=0x%.8x", __func__, in32(dev->vbase + FSPI_MCR0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_MCR1=0x%.8x", __func__, in32(dev->vbase + FSPI_MCR1));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_MCR2=0x%.8x", __func__, in32(dev->vbase + FSPI_MCR2));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_INTEN=0x%.8x", __func__, in32(dev->vbase + FSPI_INTEN));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_INTR=0x%.8x", __func__, in32(dev->vbase + FSPI_INTR));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_STS0=0x%.8x", __func__, in32(dev->vbase + FSPI_STS0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_STS1=0x%.8x", __func__, in32(dev->vbase + FSPI_STS1));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_STS2=0x%.8x", __func__, in32(dev->vbase + FSPI_STS2));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_IPRXFCR=0x%.8x", __func__, in32(dev->vbase + FSPI_IPRXFCR));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_IPTXFCR=0x%.8x", __func__, in32(dev->vbase + FSPI_IPTXFCR));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_IPCR0=0x%.8x", __func__, in32(dev->vbase + FSPI_IPCR0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_IPCR1=0x%.8x", __func__, in32(dev->vbase + FSPI_IPCR1));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_IPCMD=0x%.8x", __func__, in32(dev->vbase + FSPI_IPCMD));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_FLSHA1CR0=0x%.8x", __func__, in32(dev->vbase + FSPI_FLSHA1CR0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_FLSHA2CR0=0x%.8x", __func__, in32(dev->vbase + FSPI_FLSHA2CR0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_FLSHB1CR0=0x%.8x", __func__, in32(dev->vbase + FSPI_FLSHB1CR0));
    snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_FLSHB2CR0=0x%.8x", __func__, in32(dev->vbase + FSPI_FLSHB2CR0));
    const uint32_t sid = (in32(dev->vbase + FSPI_IPCR1) & FSPI_IPCR1_ISEQID_MASK) >> FSPI_IPCR1_ISEQID_SHIFT;
    for (uint32_t i = 0; i < dev->seqs; i++) {
        snor_slogf(_SLOG_INFO, 0, 0, "%s: FSPI_LUTa(%d)=0x%.8x", __func__,
                   sid * dev->seqs + i, in32(dev->vbase + FSPI_LUTa(sid * dev->seqs + i)));
    }
}

static int fspi_wait_mask(const nxp_fspi_t *const dev, const int offset, const uint32_t mask, const uint32_t value, uint32_t timeout)
{
    const uintptr_t addr = dev->vbase + (uintptr_t)offset;
    uint32_t  reg = 0;

    while (timeout--) {
        reg = in32(addr);
        if ((reg & mask) == value) return (EOK);
        nanospin_ns(500);
    }

    snor_slogf(_SLOG_ERROR, 0, 0, "%s: timeout to poll 0x%" PRIXPTR "=0x%.8x (0x%.8x)",
               __func__, addr, reg, value);

    return (ETIMEDOUT);
}

/*
 * If the slave device content being changed by Write/Erase, need to
 * invalidate the AHB buffer. This can be achieved by doing the reset
 * of controller after setting MCR0[SWRESET] bit.
 */
static inline int fspi_invalid(const nxp_fspi_t *const dev)
{
    uint32_t reg;

    reg = in32(dev->vbase + FSPI_MCR0);
    reg |= FSPI_MCR0_SWRESET_MASK;
    out32(dev->vbase + FSPI_MCR0, reg);

    if (fspi_wait_mask(dev, FSPI_MCR0,
                            FSPI_MCR0_SWRESET_MASK,
                            0,
                            1000) != EOK) {
        return (-1);
    }
    return 0;
}

static int fspi_mkseq(lut_t *const seq, const snor_cmd_t *const cmd, const int data_dir)
{
    uint8_t     idx = 0;
    uint32_t    cb, ab, db;

    SNOR_BUS_PROTO_TO_WIDTH(cmd->cfg->bus_proto, cb, ab, db);

    /* command */
    seq[idx].op  = cmd->op->opcode;
    seq[idx].pad = (uint8_t)LUT_PAD((int)cb);
    seq[idx].cmd = LUT_CMD;
    if (cmd->cfg->bus_proto & SNOR_BUSPROTO_DTR_MODE) {
        seq[0].cmd |= LUT_DDR;
    }
    ++idx;

    /* address if exists */
    if (cmd->op->adrlen > 0) {
        seq[idx].op  = (uint8_t)((cmd->op->adrlen == 3) ? ADDR_24BITS : ADDR_32BITS);
        seq[idx].pad = (uint8_t)LUT_PAD((int)ab);
        seq[idx].cmd = LUT_ADDR;
        if (cmd->cfg->bus_proto & SNOR_BUSPROTO_DTR_MODE) {
            seq[idx].cmd |= LUT_DDR;
        }
        ++idx;
    }

    /* dummy cycles if exists */
    if (cmd->op->dcycle > 0) {
        seq[idx].op  = cmd->op->dcycle;
        /* Due to FlexSPI controller limitation number of PAD for dummy
         * buswidth needs to be programmed as equal to data buswidth.
         */
        seq[idx].pad = (uint8_t)LUT_PAD((int)db);
        seq[idx].cmd = LUT_DUMMY;
        if (cmd->cfg->bus_proto & SNOR_BUSPROTO_DTR_MODE) {
            seq[idx].cmd |= LUT_DDR;
        }
        ++idx;
    }

    /* data if exists */
    if (data_dir != NXP_FSPI_DATA_NONE) {
        seq[idx].op  = 0;
        seq[idx].pad = (uint8_t)LUT_PAD((int)db);
        seq[idx].cmd = (uint8_t)((data_dir == NXP_FSPI_DATA_READ) ? LUT_READ : LUT_WRITE);
        if (cmd->cfg->bus_proto & SNOR_BUSPROTO_DTR_MODE) {
            seq[idx].cmd |= LUT_DDR;
        }
        ++idx;
    }

    /* STOP command is "0", so we don't need to construct the STOP command */
    return (++idx);
}

static uint32_t fspi_least_lut(const nxp_fspi_t *const dev)
{
    uint32_t sid, lsid = 0;
    uint32_t count = dev->ccount[lsid];

    for (sid = 1; sid < dev->mseqs; sid++) {
        if (count > dev->ccount[sid]) {
            count = dev->ccount[sid];
            lsid = sid;
        }
    }

    return (lsid);
}

/* function for constructing the LUT register */
static inline uint32_t get_lut(const lut_t *const table, const uint32_t idx)
{
    /*
    * The definition of the LUT register shows below:
    *
    *  ---------------------------------------------------
    *  | INSTR1 | PAD1 | OPRND1 | INSTR0 | PAD0 | OPRND0 |
    *  ---------------------------------------------------
    */
    return  ((((table[idx + 1].cmd) & 0x3f) << 26) |
             (((table[idx + 1].pad) & 3) << 24) |
             (((table[idx + 1].op)  & 0xff) << 16) |
             (((table[idx].cmd) & 0x3f) << 10) |
             (((table[idx].pad) & 3) << 8) |
             ((table[idx].op) & 0xff));

}

static int fspi_add_lut(const nxp_fspi_t *const dev, const snor_cmd_t *const cmd, const int data_dir)
{
    uint32_t sid;
    lut_t *const cmds = calloc(dev->seqs * 2, sizeof(lut_t));

    if (cmds == NULL) return (-1);

    /* find a slot */
    for (sid = 0; sid < dev->mseqs; sid++) {
        /* empty slot, or the slot with the same OP code */
        if ((dev->sigtbl[sid] == 0) ||
            ((dev->sigtbl[sid] & 0xFF) == cmd->op->opcode)) break;
    }

    /* table is full, re-use the lease used command sequence */
    if (sid >= dev->mseqs) {
        sid = fspi_least_lut(dev);
    }

    fspi_mkseq(cmds, cmd, data_dir);

    /* update the command signature and count */
    dev->sigtbl[sid] = SNOR_LUT_SIG(cmd->op->opcode, cmd->cfg->bus_proto);
    dev->ccount[sid] = 0;

    if (dev->ctrl.verbosity > 6) {
        snor_slogf(_SLOG_INFO, 0, 0,
            "%s: add LUT[%x] to slot #%d of %d",
            __func__, cmd->op->opcode, dev->sigtbl[sid], sid);
        for (uint32_t idx = 0; idx < dev->seqs; idx++) {
            snor_slogf(_SLOG_INFO, 0, 0, "%s: seqs[%d]=0x%x",
                            __func__, idx, get_lut(cmds, (idx * 2)));
        }
    }

    /* Unlock the LUT */
    out32(dev->vbase + FSPI_LUTKEY, FSPI_LUT_KEY_VAL);
    out32(dev->vbase + FSPI_LUTCR, FSPI_LUTCR_UNLOCK_MASK);

    /* write sequence to LUT */
    for (uint32_t idx = 0; idx < dev->seqs; idx++) {
        out32(dev->vbase + FSPI_LUTa(sid * dev->seqs + idx), get_lut(cmds, (idx * 2)));
    }

    free(cmds);

    /* Lock the LUT */
    out32(dev->vbase + FSPI_LUTKEY, FSPI_LUT_KEY_VAL);
    out32(dev->vbase + FSPI_LUTCR, FSPI_LUTCR_LOCK_MASK);

    return (sid);
}

static int fspi_lut_lookup(const nxp_fspi_t *const dev, const snor_cmd_t *const cmd, const int data_dir)
{
    const uint32_t sig = SNOR_LUT_SIG(cmd->op->opcode, cmd->cfg->bus_proto);

    for (int sid = 0; dev->sigtbl[sid] != 0; sid++) {
        if ((dev->sigtbl[sid]) == sig) {
            dev->ccount[sid]++;
            return (sid);
        }
    }
    /*
     * new command or old command but with different bus protocol,
     * add to table and hardware LUT
     */
    return fspi_add_lut(dev, cmd, data_dir);
}

static int fspi_setup(const nxp_fspi_t *const dev)
{
    uint32_t reg;

    /* Disable the module */
    reg =  in32(dev->vbase + FSPI_MCR0);
    reg |= FSPI_MCR0_MDIS_MASK;
    out32(dev->vbase + FSPI_MCR0, reg);

    /* Configure MCR0 */
    reg = in32(dev->vbase + FSPI_MCR0);
    reg &= ~(FSPI_MCR0_ARDFEN_MASK |
             FSPI_MCR0_ATDFEN_MASK |
             FSPI_MCR0_RXCLKSRC_MASK |
             FSPI_MCR0_COMBINATIONEN_MASK);
    if (dev->octcomb_en) {
        reg |= FSPI_MCR0_COMBINATIONEN_MASK;
    }
    reg |= FSPI_MCR0_RXCLKSRC(dev->rxclksrc);
    out32(dev->vbase + FSPI_MCR0, reg);

    /* Configure MCR1, set max SEQ and AHB timeout */
    reg = FSPI_MCR1_SEQWAIT_MASK | FSPI_MCR1_AHBBUSWAIT_MASK;
    out32(dev->vbase + FSPI_MCR1, reg);

    /* Configure MCR2
     * Disable SAMEDEVICEEN bit and configure all slave devices independdently
     */
    reg = FSPI_MCR2_RESUMEWAIT_MASK | FSPI_MCR2_CLRLEARNPHASE_MASK;
    /* Disable SCKB so that port B flash is not accessible */
    if (dev->ctrl.ncs <= 2) {
        reg |= FSPI_MCR2_SCKBDIFFOPT_MASK;
    }
    out32(dev->vbase + FSPI_MCR2, reg);

    /* Flash memory space configuraiton */
    out32(dev->vbase + FSPI_FLSHA1CR0, 0);
    out32(dev->vbase + FSPI_FLSHA2CR0, 0);
    out32(dev->vbase + FSPI_FLSHB1CR0, 0);
    out32(dev->vbase + FSPI_FLSHB2CR0, 0);

    /* Configure DLL control register DLLxCR */
    reg = FSPI_DLLACR_SLVDLYTARGET_MASK | FSPI_DLLACR_DLLEN_MASK;
    out32(dev->vbase + FSPI_DLLACR, reg);

    reg = FSPI_DLLBCR_SLVDLYTARGET_MASK | FSPI_DLLBCR_DLLEN_MASK;
    out32(dev->vbase + FSPI_DLLBCR, reg);

    /* Enable module */
    reg =  in32(dev->vbase + FSPI_MCR0);
    reg &= ~FSPI_MCR0_MDIS_MASK;
    out32(dev->vbase + FSPI_MCR0, reg);

    /* AHB configuration for access buffer 0~7. */
    for (uint32_t i = 0; i < 7; i++) {
        out32(dev->vbase + FSPI_AHBRXBUFaCR0(i), 0);
    }

    /*
     * Set BUFSZ with the maximum AHB buffer size to improve the read
     * performance.
     */
    out32(dev->vbase + FSPI_AHBRXBUFaCR0(7), (dev->ahb_bufsz / 8));

    /* prefetch and no start address alignment limitation */
    reg = FSPI_AHBCR_PREFETCHEN_MASK | FSPI_AHBCR_READADDROPT_MASK;
    if (dev->ctrl.flags & SNOR_FLG_STRIPE) {
        reg |= FSPI_AHBCR_APAREN_MASK;
    }
    out32(dev->vbase + FSPI_AHBCR, reg);

    /* Reset controller */
    return fspi_invalid(dev);
}

static int wait_for_completion(const nxp_fspi_t *const dev, const uint32_t len)
{
    const uint32_t timeout = (len + 1) * 2 * 1000;
    int status;

    status = fspi_wait_mask(dev, FSPI_STS0,
                                 FSPI_STS0_ARBIDLE_MASK,
                                 FSPI_STS0_ARBIDLE_MASK,
                                 timeout);
    if (status != EOK) {
        errno = status;
        return (-1);
    }
    return 0;
}

static int fspi_process_intr(const nxp_fspi_t *const dev)
{
    uint32_t    intr;
    int         ret = 0;

    intr = in32(dev->vbase + FSPI_INTR);
    out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPCMDDONE_MASK |
                                  FSPI_INTR_IPCMDERR_MASK |
                                  FSPI_INTR_IPCMDGE_MASK);

    if ((intr & FSPI_INTR_IPCMDERR_MASK) ||
        (intr & FSPI_INTR_IPCMDGE_MASK)) {
        snor_slogf(_SLOG_ERROR, 0, 0, "%s: Error found in INTR=%#x", __func__, intr);
        out32(dev->vbase + FSPI_INTEN, 0);
        /* reset controller */
        fspi_invalid(dev);
        if (dev->ctrl.verbosity > 8) {
            dump_registers(dev);
        }
        ret = -1;
    } else if (intr & FSPI_INTR_IPCMDDONE_MASK) {
        out32(dev->vbase + FSPI_INTEN, 0);
        ret = 1;
    } else {
        /* no operation */
    }
    InterruptUnmask(dev->irq, dev->iid);

    return (ret);
}

static int wait_for_completion_intr(const nxp_fspi_t *const dev, const uint32_t len)
{
    int process_result = 0;
    int semwait_result = 0;
    struct timespec ts;
    uint64_t timestamp;

    while (process_result == 0) {
        clock_gettime(CLOCK_MONOTONIC, &ts);
        timestamp = timespec2nsec(&ts);
        timestamp += (uint64_t)(len + 1) * 1000000;     /* 1ms per byte */
        nsec2timespec(&ts, timestamp);
        semwait_result = sem_timedwait_monotonic(dev->sem, &ts);

        if (semwait_result == -1) {
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: %s(%d)", __func__, strerror(errno), errno);
            if (errno == EINTR) continue;
            out32(dev->vbase + FSPI_INTEN, 0);
            fspi_invalid(dev);
            return (-1);
        }
        process_result = fspi_process_intr(dev);
    }

    return (process_result < 0) ? (-1) : (0);
}

static void fspi_set_sfar(const nxp_fspi_t *const dev, const snor_cmd_t *const cmd)
{
    uint32_t    sfar = 0;
    const snor_chip_t *const chip = &dev->ctrl.chip[cmd->cfg->cs];

    /* pre-identification process,  we only fill the default size for
     * the chip select passed from flash layer.
     */
    if (!(chip->flags & SNOR_CFLG_PRESENT)) {
        out32(dev->vbase + FSPI_FLSHA1CR0, 0);
        out32(dev->vbase + FSPI_FLSHA2CR0, 0);
        out32(dev->vbase + FSPI_FLSHB1CR0, 0);
        out32(dev->vbase + FSPI_FLSHB2CR0, 0);
        const uint32_t size_kb = DEFAULT_FLASH_MEM_SIZE >> 10;
        out32(dev->vbase + FSPI_FLSHA1CR0 +  (uint32_t)(cmd->cfg->cs * 4), size_kb);
        if (cmd->op->adrlen) {
            sfar += cmd->addr;
        }
    } else {
        if (cmd->op->adrlen == 0) {
            for (uint32_t cidx = 0; cidx < cmd->cfg->cs; cidx++) {
                sfar += dev->ctrl.chip[cidx].chipsz;
            }
        } else {
            sfar += cmd->addr;
        }
    }

    out32(dev->vbase + FSPI_IPCR0, sfar);
}

static int fspi_read_rxfifo(const nxp_fspi_t *const dev, uint8_t *const buf, const uint32_t nbytes)
{
    uint32_t val, idx;

    /* Default value of watermark level is 8 bytes, hence in single
     * read request controller can read max 8 bytes of data.
     */
    for (idx = 0; idx < ROUNDDOWN(nbytes, 8); idx += 8) {
        if (fspi_wait_mask(dev, FSPI_INTR,
                                FSPI_INTR_IPRXWA_MASK,
                                FSPI_INTR_IPRXWA_MASK,
                                1000) != EOK) {
            return (-1);
        }
        *(uint32_t *) (buf + idx) = in32(dev->vbase + FSPI_RFDR0);
        *(uint32_t *) (buf + idx + 4) = in32(dev->vbase + FSPI_RFDR0 + 4);
        /* Move RX FIFO pointer */
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPRXWA_MASK);
    }

    if (idx < nbytes) {
        if (fspi_wait_mask(dev, FSPI_INTR,
                                FSPI_INTR_IPRXWA_MASK,
                                FSPI_INTR_IPRXWA_MASK,
                                1000) != EOK) {
            return (-1);
        }

        uint32_t data = 0;
        uint32_t rnbytes = nbytes - idx; /* remaining number of bytes */
        uint32_t cpsize = 0;
        for (uint32_t j = 0; j < (nbytes - idx); j += 4) {
            data = in32(dev->vbase + FSPI_RFDR0 + j);
            cpsize = min(rnbytes, 4);
            memcpy(buf + idx + j, &data, (size_t)cpsize);
            rnbytes -= cpsize;
        }
        /* Invalid the RXFIFO */
        val = in32(dev->vbase + FSPI_IPRXFCR) | FSPI_IPRXFCR_CLRIPRXF_MASK;
        out32(dev->vbase + FSPI_IPRXFCR, val);
        /* Move the FIFO pointer */
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPRXWA_MASK);
    }

    return 0;
}

static int fspi_fill_txfifo(const nxp_fspi_t *const dev, const uint8_t *const buf, const uint32_t nbytes)
{
    uint32_t idx;

    /* Clear the TX FIFO and set wartmark to 8 bytes */
    out32(dev->vbase + FSPI_IPTXFCR, FSPI_IPTXFCR_CLRIPTXF_MASK);

    for (idx = 0; idx < ROUNDDOWN(nbytes, 8); idx += 8) {
        if (fspi_wait_mask(dev, FSPI_INTR,
                                FSPI_INTR_IPTXWE_MASK,
                                FSPI_INTR_IPTXWE_MASK,
                                1000) != EOK) {
            return (-1);
        }
        out32(dev->vbase + FSPI_TFDR0, *(uint32_t *) (buf + idx));
        out32(dev->vbase + FSPI_TFDR0 + 4u, *(uint32_t *) (buf + idx + 4u));
        /* Push data to Tx FIFO */
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPTXWE_MASK);
    }

    if (idx < nbytes) {
        if (fspi_wait_mask(dev, FSPI_INTR,
                            FSPI_INTR_IPTXWE_MASK,
                            FSPI_INTR_IPTXWE_MASK,
                            1000) != EOK) {
            return (-1);
        }
        uint32_t data;
        uint32_t rnbytes = nbytes - idx; /* remaining number of bytes */
        uint32_t cpsize = 0;
        for (uint32_t j = 0; j < (nbytes - idx); j += 4) {
            data = 0;
            cpsize = min(rnbytes, 4);
            memcpy(&data, buf + idx + j, (size_t)cpsize);
            out32(dev->vbase + FSPI_TFDR0 + j, data);
            rnbytes -= cpsize;
        }
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPTXWE_MASK);
    }

    return 0;
}

static int fspi_read_ahb(const snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const buf, uint32_t len)
{
    nxp_fspi_t *const dev = (nxp_fspi_t *)snor;
    int tid;

    tid = fspi_lut_lookup(dev, cmd, NXP_FSPI_DATA_READ);
    if (tid < 0) {
        return (-1);
    }

    /* AHB buffer size is read buffer depth
     * FIXME!! The size limit might only matter if DMA is used.
     */
    len = (len > dev->ahb_bufsz) ? dev->ahb_bufsz : len;

    const uint32_t seqid = (tid << FSPI_FLSHA1CR2_ARDSEQID_SHIFT) & FSPI_FLSHA1CR2_ARDSEQID_MASK;
    out32(dev->vbase + FSPI_FLSHA1CR2 + (uint32_t)(cmd->cfg->cs * 4) , seqid);

    memcpy(buf, (void *)(dev->amba_vbase + cmd->addr), (size_t)len);

    return (len);
}

static int nxp_fspi_read(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const buf, const uint32_t len)
{
    const nxp_fspi_t  *const dev = (nxp_fspi_t *)snor;
    int tid;
    uint32_t val, xlen = 0;

    if (fspi_wait_mask(dev, FSPI_STS0,
                            FSPI_STS0_ARBIDLE_MASK,
                            FSPI_STS0_ARBIDLE_MASK,
                            1000) != EOK) {
         return (-1);
    }

    if (len == 0) return (len);
    if ((len > (dev->rdp - 4u)) &&
        (cmd->op->adrlen >= 3u) &&
        (dev->amba_size > 0u) &&
        !dev->ip_only) {
        return fspi_read_ahb(snor, cmd, buf, len);
    }

    tid = fspi_lut_lookup(dev, cmd, NXP_FSPI_DATA_READ);
    if (tid < 0) {
        return (-1);
    }

    /* FSPI_RX_BUF_DEPTH is read buffer depth */
    xlen = (len > dev->rdp) ? dev->rdp : len;

    /* Set RX water mark level to 8 bytes so that a read request
     * can read max 8 bytes of data.
     */
    val = in32(dev->vbase + FSPI_IPRXFCR) & ~FSPI_IPRXFCR_RXWMRK_MASK;

    /* Check if the RX FIFO is actually empty right now */
    if (in32(dev->vbase + FSPI_IPRXFSTS) & FSPI_IPRXFSTS_FILL_MASK) {
        /* Clear RX FIFO from stale values */
        val |= FSPI_IPRXFCR_CLRIPRXF_MASK;
    }
    out32(dev->vbase + FSPI_IPRXFCR, val);

    fspi_set_sfar(dev, cmd);

    if (dev->irq) {
        /* Clear interrupt flags */
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPCMDDONE_MASK |
                                      FSPI_INTR_IPCMDERR_MASK |
                                      FSPI_INTR_IPCMDGE_MASK);
        /* Enable interrupts */
        out32(dev->vbase + FSPI_INTEN, FSPI_INTR_IPCMDDONE_MASK |
                                       FSPI_INTR_IPCMDERR_MASK |
                                       FSPI_INTR_IPCMDGE_MASK);
    }

    /* IPCR: Assert Read CMD */
    uint32_t ipcr1 = (tid << FSPI_IPCR1_ISEQID_SHIFT) & FSPI_IPCR1_ISEQID_MASK;
    if (dev->ctrl.flags & SNOR_FLG_STRIPE) {
        ipcr1 |= FSPI_IPCR1_IPAREN_MASK;
    }
    /* override the data size in command sequences */
    ipcr1 |= (xlen & FSPI_IPCR1_IDATSZ_MASK);
    out32(dev->vbase + FSPI_IPCR1, ipcr1);

    /* Trigger IP command */
    out32(dev->vbase + FSPI_IPCMD, FSPI_IPCMD_TRG_MASK);

    /* Wait for cmd to be sent */
    if (dev->irq) {
        if (wait_for_completion_intr(dev, xlen) == -1) return (-1);
    } else {
        if (wait_for_completion(dev, xlen) == -1) return (-1);
    }

    const int status = fspi_read_rxfifo(dev, buf, xlen);

    return (status == -1) ? (-1) : (xlen);
}

static int nxp_fspi_write(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const buf, const uint32_t len)
{
    const nxp_fspi_t *const dev = (nxp_fspi_t *)snor;
    const int data_dir = (len > 0) ? NXP_FSPI_DATA_WRITE : NXP_FSPI_DATA_NONE;
    int tid, status = 0;
    uint32_t xlen = 0;

    if (fspi_wait_mask(dev, FSPI_STS0,
                            FSPI_STS0_ARBIDLE_MASK,
                            FSPI_STS0_ARBIDLE_MASK,
                            1000) != EOK) {
         return (-1);
    }

    tid = fspi_lut_lookup(dev, cmd, data_dir);
    if (tid < 0) return (-1);

    fspi_set_sfar(dev, cmd);

    /* Handle addressing write */
    if (len > 0) {
        /* Set the transfer segment/burst byte counter
         * QSPI_TX_BUF_DEPTH is the write buffer depth */
        xlen = (len > dev->tdp) ? dev->tdp : len;

        status = fspi_fill_txfifo(dev, buf, xlen);
        if (status == -1) {
            return (status);
        }

    }

    if (dev->irq) {
        /* Clear interrrupt flags */
        out32(dev->vbase + FSPI_INTR, FSPI_INTR_IPCMDDONE_MASK |
                                      FSPI_INTR_IPCMDERR_MASK |
                                      FSPI_INTR_IPCMDGE_MASK);
        /* Enable interrupts */
        out32(dev->vbase + FSPI_INTEN, FSPI_INTR_IPCMDDONE_MASK |
                                       FSPI_INTR_IPCMDERR_MASK |
                                       FSPI_INTR_IPCMDGE_MASK);
    }

    /* Set IP command sequence index */
    uint32_t ipcr1 = (tid << FSPI_IPCR1_ISEQID_SHIFT) & FSPI_IPCR1_ISEQID_MASK;
    ipcr1 |= (dev->ctrl.flags & SNOR_FLG_STRIPE) ? FSPI_IPCR1_IPAREN_MASK : 0u;
    /* override the data size in command sequence */
    ipcr1 |= xlen & FSPI_IPCR1_IDATSZ_MASK;
    out32(dev->vbase + FSPI_IPCR1, ipcr1);
    /* Trigger IP command */
    out32(dev->vbase + FSPI_IPCMD, FSPI_IPCMD_TRG_MASK);

    /* Wait for cmd to be sent */
    if (dev->irq) {
        status = wait_for_completion_intr(dev, xlen);
    } else {
        status = wait_for_completion(dev, xlen);
    }

    /* AHB buffer need reset */
    fspi_invalid(dev);

    return (status == -1) ? (-1) : (xlen);
}

static int nxp_fspi_dinit(void *const hdl)
{
    nxp_fspi_t *const dev = hdl;

    if (dev == NULL) return (EOK);

    /* Disable function */
    out32(dev->vbase + FSPI_MCR0, FSPI_MCR0_MDIS_MASK);

    if (dev->irq) {
        if ((dev->sem != SEM_FAILED) || (dev->sem != NULL)) {
            sem_close(dev->sem);
        }
        if (dev->iid != -1) {
            InterruptDetach(dev->iid);
        }
    }

    if (dev->vbase) {
        munmap_device_io(dev->vbase, FSPI_SIZE);
    }

    if (dev->sigtbl != NULL) {
        free(dev->sigtbl);
    }

    free(dev);

    return (EOK);
}

static int nxp_fspi_post_ident(snor_ctrl_t *snor, int cs)
{
    nxp_fspi_t  *dev = (nxp_fspi_t *)snor;
    snor_chip_t *chip = &snor->chip[cs];
    uint32_t    total_size = 0;

    /* for the access of non-memory content, we stick with 1-1-1 bus protocol for none Octal, Quad and Dual modes.
     */
    if (((chip->cfg.bus_proto & SNOR_BUSPROTO_BUS_MASK) != SNOR_BUSPROTO_8_8_8) &&
        ((chip->cfg.bus_proto & SNOR_BUSPROTO_BUS_MASK) != SNOR_BUSPROTO_4_4_4) &&
        ((chip->cfg.bus_proto & SNOR_BUSPROTO_BUS_MASK) != SNOR_BUSPROTO_2_2_2)) {
        chip->cfg.bus_proto &= ~SNOR_BUSPROTO_BUS_MASK;
        chip->cfg.bus_proto |= SNOR_BUSPROTO_1_1_1;
    }

    if (chip->cfg.bus_proto & SNOR_BUSPROTO_DQS) {
        uint32_t reg = in32(dev->vbase + FSPI_MCR0);
        reg |= FSPI_MCR0_RXCLKSRC(FSPI_MCR0_RXCLKSRC_BV_FLSH_DQS);
        out32(dev->vbase + FSPI_MCR0, reg);
    }

    /* update flash size of this chip select */
    const uint32_t size_kb = chip->chipsz >> 10;
    out32(dev->vbase + FSPI_FLSHA1CR0 + (uint32_t)(cs * 4), size_kb);

    /* alignment requirement */
    if (chip->pagesz > dev->tdp) {
        chip->pagesz = dev->tdp;    /* cap the page size to write buffer size */
    }

    if (dev->amba_size != 0) {
        munmap_device_io(dev->amba_vbase, dev->amba_size);
        dev->amba_size = 0;
    }

    /* calculate total size up to this chip select */
    for (int cidx = 0; cidx <= cs; cidx++) {
        total_size += chip[cidx].chipsz;
    }

    dev->amba_size = total_size;
    dev->amba_vbase = mmap_device_io(dev->amba_size, dev->amba_base);
    if (dev->amba_vbase == (uintptr_t)MAP_FAILED) {
        snor_slogf(_SLOG_ERROR, 0, 0,
                "%s: mmap_device_io error %s", __func__, strerror(errno));
        return (errno);
    } else {
        dev->ip_only = 0;
    }

    return (EOK);
}

static int nxp_fspi_cfg_bus(snor_ctrl_t *const snor, snor_cfg_t *const cfg)
{
    return (EOK);
}

static int nxp_fspi_init(nxp_fspi_t *dev)
{
    int status = EOK;

    static const snor_func_t nxp_fspi_func = {
        .dinit = nxp_fspi_dinit,
        .cfg_bus = nxp_fspi_cfg_bus,
        .post_ident = nxp_fspi_post_ident,
        .dstripe = NULL,     /* no de-stripe */
        .read_reg = NULL,    /* no register spicific read function */
        .write_reg = NULL,   /* no regsiter specific write function */
        .read = nxp_fspi_read,
        .write = nxp_fspi_write
    };

    dev->amba_base  = FSPI_AMBA_BASE;
    dev->amba_size  = 0;
    dev->pbase      = FSPI_BASE;
    dev->seqs       = LUT_SEQUENCE_SIZE;
    dev->rdp        = FSPI_RX_BUF_DEPTH;
    dev->tdp        = FSPI_TX_BUF_DEPTH;
    dev->mseqs      = FSPI_MAX_NUM_OF_SEQUENCE;
    dev->ahb_bufsz  = FSPI_AHB_BUF_SIZE;
    dev->irq        = FSPI_DEFAULT_IRQ;
    dev->rxclksrc   = FSPI_MCR0_RXCLKSRC_BV_LPBK_INT;
    dev->octcomb_en = 1;
    dev->ip_only    = 1;

    /* check command line options */
    if (nxp_fspi_options(dev) != EOK) {
        nxp_fspi_dinit(dev);
        return (ENODEV);
    }

    /* Octal combination mode only support two chipselects */
    if ((dev->octcomb_en == 1) && (dev->ctrl.ncs > 2)) {
        snor_slogf(_SLOG_ERROR, 0, 0,
                "Octal combination mode only support two chipselects(%u)", dev->ctrl.ncs);
        nxp_fspi_dinit(dev);
        return (ENODEV);
    }

    /* Command signature table and Command execution count table */
    dev->sigtbl = calloc(dev->mseqs * 2, sizeof(uint32_t));
    if (dev->sigtbl == NULL) {
        status = errno;
        nxp_fspi_dinit(dev);
        return (status);
    }

    dev->ccount = dev->sigtbl + dev->mseqs;

    dev->vbase = mmap_device_io(FSPI_SIZE, dev->pbase);
    if (dev->vbase == (uintptr_t)MAP_FAILED) {
        status = errno;
        snor_slogf(_SLOG_ERROR, 0, 0, "mmap_device_io error %s", strerror(errno));
        nxp_fspi_dinit(dev);
        return (status);
    }

    /* Setup FSPI */
    if (fspi_setup(dev) != 0) {
        nxp_fspi_dinit(dev);
        return (ENODEV);

    }

    /* Clear RX and TX FIFOs */
    out32(dev->vbase + FSPI_IPTXFCR, FSPI_IPTXFCR_CLRIPTXF_MASK);
    out32(dev->vbase + FSPI_IPRXFCR, FSPI_IPRXFCR_CLRIPRXF_MASK);

    /* Disable all interrupts */
    out32(dev->vbase + FSPI_INTEN, 0x0u);
    /* Clear all interrupts */
    out32(dev->vbase + FSPI_INTR, 0xFFFu);

    if (dev->irq) {
        struct sigevent event;

        dev->sem = sem_open(SEM_ANON, 0, 0, 0);
        if ((dev->sem == SEM_FAILED) || (dev->sem == NULL)) {
            status = errno;
            snor_slogf(_SLOG_ERROR, 0, 0, "%s: sem-open failed %s", __func__, strerror (errno));
            nxp_fspi_dinit(dev);
            return (status);
        }
        SIGEV_SEM_INIT(&event, dev->sem);

        /* Attach interrupt */
        dev->iid = InterruptAttachEvent(dev->irq, &event, _NTO_INTR_FLAGS_TRK_MSK);
        if (dev->iid == -1) {
            status = errno;
            nxp_fspi_dinit(dev);
            return (status);

        }
    }

    memcpy(&dev->ctrl.funcs, &nxp_fspi_func, sizeof(snor_func_t));

    dev->ctrl.hcaps |= SNOR_HCAPS_RD_1_1_1 |
                       SNOR_HCAPS_RD_1_1_1_FAST |
                       SNOR_HCAPS_RD_1_4_4 |
                       SNOR_HCAPS_RD_4_4_4 |
                       SNOR_HCAPS_RD_1_8_8 |
                       SNOR_HCAPS_PP_1_8_8 |
                       SNOR_HCAPS_PP_1_4_4 |
                       SNOR_HCAPS_PP_4_4_4 |
                       SNOR_HCAPS_PP_1_1_1;

    return (EOK);
}

int32_t f3s_nxp_fspi_open(f3s_socket_t *socket, uint32_t flags)
{
    nxp_fspi_t  *dev;

    if (socket->memory) return (EOK);

    /* Allocate driver handle */
    dev = snor_alloc_handle(socket, sizeof(nxp_fspi_t));
    if (dev == NULL) return (ENOMEM);

    socket->name = (unsigned char *)"NXP FlexSPI";

    /* Initialize FSPI controller */
    return nxp_fspi_init(dev);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
