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
 */

#include "ecspi.h"

/**
 *  @brief             SPI transfer function.
 *  @param hdl         SPI driver handle.
 *  @param spi_dev     SPI device structure pointer.
 *  @param buf         The buffer which stores the transfer data.
 *  @param tnbytes     The number of transmit bytes.
 *  @param rnbytes     The number of receive bytes.
 *
 *  @return            EOK --success otherwise fail.
 */
int ecspi_xfer(void *const hdl, spi_dev_t *const spi_dev, uint8_t *const buf, const uint32_t tnbytes, const uint32_t rnbytes)
{
    ecspi_t           *spi = hdl;
    const uintptr_t   base = spi->vbase;
    uint8_t           txfifo = 0;
    uint32_t          ctrl_reg, data, cfg_reg, period_reg;
    const uint32_t ssid = spi_dev->devinfo.devno;
    const uint32_t busid = spi->bus_node->busno;
    const spi_cfg_t *const cfg = &spi_dev->devinfo.cfg;
    int ret = EOK;

    if (ssid >= SPI_MAX_DEV) {
        spi_slogf(_SLOG_ERROR, "%s: Unexpected device (id=%d)\n", __func__, ssid);
        return EINVAL;
    }

    spi->xlen = max(tnbytes, rnbytes);       // total exchange length
    spi->rlen = 0;                           // totall recive length
    spi->tlen = 0;                           // totall tranmit length
    spi->bxlen = 0;                          // exchange length for the current burst
    spi->lsb_adjust = 0;
    spi->pbuf = buf;

    spi->dwidth = ((cfg->mode & SPI_MODE_WORD_WIDTH_MASK) >> 3);

    if (spi->xlen < spi->dwidth) {
        spi_slogf(_SLOG_ERROR,"%s: Unexpected exchange data length %d (word length is %d)\n",
                               __func__, spi->xlen, spi->dwidth);
        return EINVAL;
    }

    /* Estimate transfer time in us...
     * The calculated dtime is only used for the timeout, so it doesn't have to be that accurate.
     * The dtime value should round up to the next integer value; and at higher clock rates,
     * a calcuated dtime of 0 would mess-up the timeout calculation.
     * So always add up 1us here.
     */
    spi->dtime = spi->dwidth * 8 * 1000 * 1000 / cfg->clock_rate;
    spi->dtime++;

    /* Disable Interrupts */
    out32(base + ECSPI_INTREG, 0x0);

    const spi_dev_params_t *const ctrl_params = get_dev_params(busid, ssid);
    cfg_reg = ctrl_params->cfg_reg;
    cfg_reg |= (0x1 << (ssid + ECSPI_CONFIGREG_SSCTL_POS));        // multiply burst mode

    /* It seems that all channels have to be set as master mode */
    ctrl_reg = ctrl_params->ctrl_reg;
    ctrl_reg |= (ssid << ECSPI_CONTROLREG_CSEL_POS) | ECSPI_CONTROLREG_CH_MODE_MASK | ECSPI_CONTROLREG_ENABLE;

    /* Enable SPI and set the configuration register */
    out32(base + ECSPI_CONTROLREG, ctrl_reg);
    out32(base + ECSPI_CONFIGREG, cfg_reg);

    /* clean up the RXFIFO, it should be no data here */
    while ((in32(base + ECSPI_TESTREG) & ECSPI_TESTREG_RXCNT_MASK) != 0) {
        in32(base + ECSPI_RXDATA);
    }

    if (spi->loopback == 1) {
        out32(base + ECSPI_TESTREG, (unsigned int)ECSPI_TESTREG_LOOPBACK);
    }

    /* set wait states and chip select delay */
    period_reg = (uint32_t)((spi->csdelay << ECSPI_PERIODREG_CSD_CTRL_POS) & ECSPI_PERIODREG_CSD_CTRL_MASK)
                         | (spi->waitstates & ECSPI_PERIODREG_SP_MASK);
    out32(base + ECSPI_PERIODREG, period_reg);

    /* set the RX water mark for RXFIFO data request interrupt */
    out32(base + ECSPI_DMAREG, (ECSPI_FIFO_RXMARK << 16));

    while ((spi->rlen < spi->xlen) && (spi->tlen < spi->xlen)) {

        spi->brlen = 0;
        spi->btlen = 0;
        txfifo = 0;

        if (spi->burst) {           // for SPI burst transmit mode, support 8, 16 and 32 bits word
            /* get the next burst size in byte */
            if ((spi->xlen - spi->tlen) >= ECSPI_BURST_MAX) {
                spi->bxlen = ECSPI_BURST_MAX;
                spi->lsb_adjust = 0;
            } else {
                spi->bxlen = spi->xlen - spi->tlen;

                /* check if the burst align on word size */
                spi->btlen = spi->bxlen % 4;
                switch (spi->btlen) {
                    case 1:   // only write 8 bits in TXFIFO
                        if (1 == spi->dwidth) {
                            out32(base + ECSPI_TXDATA, buf[spi->tlen]);
                            spi->tlen++;
                            txfifo = 1;
                            spi->lsb_adjust = 1;        // 1 LSB byte in first TXFIFO
                        } else {
                            spi_slogf(_SLOG_ERROR, "%s: Unexpected tranfer length %d for %d bits word\n",
                                                    __func__, spi->xlen, (8 * spi->dwidth));
                            ret = EIO;
                            goto fail;
                        }
                        break;

                    case 2:   // only write 16 bits in TXFIFO
                        if (1 == spi->dwidth) {
                            data = (uint32_t)(buf[spi->tlen] << 8) | (buf[spi->tlen + 1]);
                            out32(base + ECSPI_TXDATA, data);
                            spi->tlen += 2;
                            txfifo = 1;
                            spi->lsb_adjust = 2;        // 2 LSB byte in first TXFIFO
                        } else if (2 == spi->dwidth) {
                            data = *(uint16_t *)(&buf[spi->tlen]);
                            out32(base + ECSPI_TXDATA, data);
                            spi->tlen += 2;
                            txfifo = 1;
                            spi->lsb_adjust = 2;        // 2 LSB byte in first TXFIFO
                        } else {
                            spi_slogf(_SLOG_ERROR, "%s: Unexpected tranfer length %d for %d bits word\n",
                                                    __func__, spi->xlen, (8 * spi->dwidth));
                            ret = EIO;
                            goto fail;
                        }
                        break;

                    case 3:   // only write 24 bits in TXFIFO
                        if (1 == spi->dwidth) {
                            data = (uint32_t)(buf[spi->tlen] << 16) | (buf[spi->tlen + 1] << 8) | (buf[spi->tlen + 2]);
                            out32(base + ECSPI_TXDATA, data);
                            spi->tlen += 3;
                            txfifo = 1;
                            spi->lsb_adjust = 3;        // 3 LSB byte in first TXFIFO
                        } else {
                            spi_slogf(_SLOG_ERROR, "%s: Unexpected tranfer length %d for %d bits word\n",
                                                    __func__, spi->xlen, (8 * spi->dwidth));
                            ret = EIO;
                            goto fail;
                        }
                        break;

                    default:
                        // no data adjustment in TXFIFO
                        txfifo = 0;
                        spi->lsb_adjust = txfifo;
                        break;
                }
            }

            /* write rest of data to TXFIFO */
            while ((spi->btlen < spi->bxlen) && (txfifo < ECSPI_FIFO_SIZE)) {
                switch (spi->dwidth) {
                    case 1:
                        data = (uint32_t)(buf[spi->tlen] << 24) | (buf[spi->tlen + 1] << 16)
                                | (buf[spi->tlen + 2] << 8) | (buf[spi->tlen + 3]);
                        break;
                    case 2:
                        data = (uint32_t)(*(uint16_t *)(&buf[spi->tlen]) << 16)
                                | (*(uint16_t *)(&buf[spi->tlen + 2]));
                        break;
                    case 4:
                        data = *(uint32_t *)(&buf[spi->tlen]);
                        break;
                    default:
                        spi_slogf(_SLOG_ERROR, "%s: Unsupport word length (support 8, 16 and 32 bits word in burst mode)\n", __func__);
                        ret = EIO;
                        goto fail;
                        break;
                }
                out32(base + ECSPI_TXDATA, data);
                spi->tlen += 4;
                spi->btlen += 4;
                txfifo++;
            }

            /* set SPI burst length */
            ctrl_reg = (in32(base + ECSPI_CONTROLREG) & ~ECSPI_CONTROLREG_BCNT_MASK)
                            | ((spi->bxlen * 8 - 1) << ECSPI_CONTROLREG_BCNT_POS);
            out32(base + ECSPI_CONTROLREG, ctrl_reg);

        } else {
            /* the normal mode will support any word length (from 1 to 32 bits) data transfer
             * in one word per burst mode
             */
            for (txfifo = 0; txfifo < ECSPI_FIFO_SIZE; txfifo++) {

                if (spi->tlen >= spi->xlen) {
                    break;
                }

                switch (spi->dwidth) {
                    case 1:
                        out32(base + ECSPI_TXDATA, buf[spi->tlen]);
                        spi->tlen++;
                        break;
                    case 2:
                        out32(base + ECSPI_TXDATA, *(uint16_t *)(&buf[spi->tlen]));
                        spi->tlen += 2;
                        break;
                    case 3:
                    case 4:
                        out32(base + ECSPI_TXDATA, *(uint32_t *)(&buf[spi->tlen]));
                        spi->tlen += 4;
                        break;
                    default:
                        break;
                }
            }

            /* set SPI burst length */
            ctrl_reg = (in32(base + ECSPI_CONTROLREG) & ~ECSPI_CONTROLREG_BCNT_MASK)
                       | (((cfg->mode & SPI_MODE_WORD_WIDTH_MASK) - 1) << ECSPI_CONTROLREG_BCNT_POS);
            out32(base + ECSPI_CONTROLREG, ctrl_reg);
        }

        /* enable tx complete interrupt and RXFIFO data request interrupt */
        out32(base + ECSPI_INTREG, ECSPI_INTREG_TCEN | ECSPI_INTREG_RDREN );

        /* Start exchange */
        out32(base + ECSPI_CONTROLREG, in32(base + ECSPI_CONTROLREG) | ECSPI_CONTROLREG_XCH);

        /*
         * Wait for exchange to finish
         */
        ret = ecspi_wait(spi, spi->xlen);
        if (ret != EOK) {
            spi_slogf(_SLOG_ERROR, "%s: XFER Timeout!!!\n", __func__);
            spi->rlen = ~0U;
        }
    }

fail:
    /* disable SPI and interrupt */
    out32(base + ECSPI_INTREG, 0x0);
    return ret;
}

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
