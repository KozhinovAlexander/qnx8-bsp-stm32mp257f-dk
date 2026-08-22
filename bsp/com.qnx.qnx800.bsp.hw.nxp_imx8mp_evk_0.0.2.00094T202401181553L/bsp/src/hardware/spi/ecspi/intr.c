/*
 * Copyright (c) 2017,2023, BlackBerry Limited.
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

/*
 * We use the same buffer for transmit and receive
 * For exchange, that's exactly what we wanted
 * For Read, it doesn't matter what we write to SPI, so we are OK.
 * For transmit, the receive data is put at the buffer we just transmitted, we are still OK.
 */

/**
 *  @brief             Process SPI interrupts.
 *  @param  bus        The SPI bus structure
 *
 *  @return            0: Transfer is completed; -1: Transfer is not completed
 */
int process_interrupts(ecspi_t *const dev)
{
    uintptr_t   const base = dev->vbase;
    uint32_t    data;
    unsigned int tmp, count;

    /* check which interrupt */
    tmp = in32(base + ECSPI_STATREG);
    if (tmp & ECSPI_STATREG_TC) {
        /* clear TC interrupt source */
        out32(base + ECSPI_STATREG, ECSPI_STATREG_TC);

        /* how many words received in RXFIFO */
        count = ((in32(base + ECSPI_TESTREG) & ECSPI_TESTREG_RXCNT_MASK) >> ECSPI_TESTREG_RXCNT);
    } else if (tmp & ECSPI_STATREG_RDR) {
        count = ((in32(base + ECSPI_TESTREG) & ECSPI_TESTREG_RXCNT_MASK) >> ECSPI_TESTREG_RXCNT);

        /* RX FIFO interrupt, at least ECSPI_FIFO_RXMARK bytes in RXFIFO */
        if (count > ECSPI_FIFO_RXMARK) {
            count = ECSPI_FIFO_RXMARK;
        }
    } else {                          // wrong interrupt
        /* Disable interrupt and return */
        out32(base + ECSPI_INTREG, 0);
        return 0;
    }

    if (dev->rlen < 0) {
        return 0;
    }

    if (dev->burst) {
        /* read the data out */
        /* check if the first word in RXFIFO need data adjustment */
        if (0 != dev->lsb_adjust) {
            data = in32(base + ECSPI_RXDATA);
            count--;                // read one word data from RXFIFO

            switch (dev->lsb_adjust) {
                case 1:             // only read 8 bits in RXFIFO
                    if (1 == dev->dwidth) {
                        dev->pbuf[dev->rlen++] = (uint8_t)data;
                        dev->brlen += 1;
                    }
                    break;
                case 2:             // only read 16 bits in RXFIFO
                    if (1 == dev->dwidth) {
                        dev->pbuf[dev->rlen++] = (uint8_t)(data >> 8);
                        dev->pbuf[dev->rlen++] = (uint8_t)data;
                        dev->brlen += 2;
                    } else if (2 == dev->dwidth) {
                        *(uint16_t *)(&dev->pbuf[dev->rlen]) = (uint16_t)(data >> 0);
                        dev->rlen += 2;
                        dev->brlen += 2;
                    } else {
			    /* do nothing */
		    }
                    break;
                case 3:             // only read 24 bits in RXFIFO
                    if (1 == dev->dwidth) {
                        dev->pbuf[dev->rlen++] = (uint8_t)(data >> 16);
                        dev->pbuf[dev->rlen++] = (uint8_t)(data >> 8);
                        dev->pbuf[dev->rlen++] = (uint8_t)data;
                        dev->brlen += 3;
                    }
                    break;
                default:
                    break;
            }
            dev->lsb_adjust = 0;    // clear data adjustment flag
        }

        /* read the rest RX buffer */
        while (count && (dev->rlen < dev->xlen) && (dev->rlen < dev->tlen) && (dev->brlen < dev->bxlen)) {
            data = in32(base + ECSPI_RXDATA);
            count--;

            switch (dev->dwidth) {
                case 1:
                    dev->pbuf[dev->rlen++] = (uint8_t)(data >> 24);
                    dev->pbuf[dev->rlen++] = (uint8_t)(data >> 16);
                    dev->pbuf[dev->rlen++] = (uint8_t)(data >> 8);
                    dev->pbuf[dev->rlen++] = (uint8_t)data;
                    break;
                case 2:
                    *(uint16_t *)(&dev->pbuf[dev->rlen]) = (uint16_t)(data >> 16);
                    dev->rlen += 2;
                    *(uint16_t *)(&dev->pbuf[dev->rlen]) = (uint16_t)data;
                    dev->rlen += 2;
                    break;
                case 4:
                    *(uint32_t *)(&dev->pbuf[dev->rlen]) = data;
                    dev->rlen += 4;
                    break;
                default:
                    break;
            }
            dev->brlen += 4;
        }

        /* fill TXFIFO:
         * 1: more data need to be transmitted for the current burst
         * 2: less that ECSPI_FIFO_RXMARK word has been write to TXFIFO
         * Note: The data left here must be align on 32bits word.
         * Because only the first word of every burst may need the data adjustment
         * that has been handled in mx51_xfer()
         */
        count = ECSPI_FIFO_SIZE - (in32(base + ECSPI_TESTREG) & ECSPI_TESTREG_TXCNT_MASK);
        while ((dev->btlen < dev->bxlen) && (count > 0)) {
            switch (dev->dwidth) {
                case 1:
                    data = (uint32_t)((dev->pbuf[dev->tlen] << 24) | (dev->pbuf[dev->tlen + 1] << 16)
                            | (dev->pbuf[dev->tlen + 2] << 8) | (dev->pbuf[dev->tlen + 3]));
                    break;
                case 2:
                    data = (uint32_t)((*(uint16_t *)(&dev->pbuf[dev->tlen]) << 16)
                            | (*(uint16_t *)(&dev->pbuf[dev->tlen + 2])));
                    break;
                case 4:
                    data = *(uint32_t *)(&dev->pbuf[dev->tlen]);
                    break;
                default:
                    data = 0;
                    break;
            }
            out32(base + ECSPI_TXDATA, data);
            dev->tlen += 4;
            dev->btlen += 4;
            count--;
        }

        if ((dev->brlen >= dev->bxlen) || (dev->rlen >= dev->xlen)) {
            /* Disable interrupt and return */
            out32(base + ECSPI_INTREG, 0);
            return 0;
        } else {
            /* Start next burst */
            out32(base + ECSPI_CONTROLREG,
                in32(base + ECSPI_CONTROLREG) | ECSPI_CONTROLREG_XCH);
        }
    } else {
        /* empty RX buffer and fill tx buffer(if required) */
        while (count) {
            data = in32(base + ECSPI_RXDATA);
            count--;

            switch (dev->dwidth) {
                case 1:
                    dev->pbuf[dev->rlen++] = (uint8_t)data;

                    /* More to Xmit */
                    if (dev->tlen < dev->xlen) {
                        out32(base + ECSPI_TXDATA, dev->pbuf[dev->tlen++]);
                    }
                    break;
                case 2:
                    *(uint16_t *)(&dev->pbuf[dev->rlen]) = (uint16_t)data;
                    dev->rlen += 2;

                    /* More to Xmit */
                    if (dev->tlen < dev->xlen) {
                        out32(base + ECSPI_TXDATA, *(uint16_t *)(&dev->pbuf[dev->tlen]));
                        dev->tlen += 2;
                    }
                    break;
                case 3:
                case 4:
                    *(uint32_t *)(&dev->pbuf[dev->rlen]) = data;
                    dev->rlen += 4;

                    /* More to Xmit */
                    if (dev->tlen < dev->xlen) {
                        out32(base + ECSPI_TXDATA, *(uint32_t *)(&dev->pbuf[dev->tlen]));
                        dev->tlen += 4;
                    }
                    break;
                default:
                    break;
            }
        }

        if (dev->rlen >= dev->xlen) {
            /* Disable interrupt and return */
            out32(base + ECSPI_INTREG, 0);
            return 0;
        } else {
            /* Start next burst */
            out32(base + ECSPI_CONTROLREG,
                in32(base + ECSPI_CONTROLREG) | ECSPI_CONTROLREG_XCH);
        }
    }

    return -1;
}

/**
 *  @brief             Attach SPI interrupts.
 *  @param  spi        The SPI driver handle.
 *
 *  @return            EOK --success otherwise fail.
 */
int ecspi_attach_intr(ecspi_t *spi)
{
    const spi_bus_t *const bus = spi->bus_node;

    /*
     * Attach SPI interrupt
     */
    spi->iid = InterruptAttachEvent(spi->irq, &bus->evt, _NTO_INTR_FLAGS_TRK_MSK);
    if (spi->iid == -1) {
        spi_slogf(_SLOG_ERROR, "%s: InterruptAttachEvent failed: %s", __func__, strerror(errno));
        return errno;
    }

    return EOK;
}

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/spi/ecspi/intr.c $ $Rev: 980075 $")
#endif
