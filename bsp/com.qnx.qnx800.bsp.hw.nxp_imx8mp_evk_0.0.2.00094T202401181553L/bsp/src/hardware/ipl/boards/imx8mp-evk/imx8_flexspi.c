/*
 * $QNXLicenseC:
 * Copyright 2021, 2022 BlackBerry Limited.
 * Copyright 2017-2019 NXP
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

#include <hw/inout.h>
#include <soc/nxp/imx8/common/imx_flexspi.h>
#include "ipl.h"
#include "imx8_flexspi_ipl.h"

/**
 * i.MX IPL source file.
 *
 * @file       imx_flexspi.c
 * @addtogroup ipl
 * @{
 */

#if defined(__aarch64__)

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/**
 * Set Rx water-mark.
 *
 * @param wtmk  RX watermark value.
 */
static void flexspi_set_rx_watermark(flexspi_t * const flexspi, uint32_t wtmk)
{
    /* Data transfer size logic must be dword aligned */
    while ((wtmk % 8) != 0) {
        wtmk++;
    }
    /* Set watermark */
    out32(flexspi->pbase + IMX_FLEXSPI_IPRXFCR, (wtmk / 8 - 1) << IMX_FLEXSPI_IPRXFCR_RXWMRK_SHIFT);
}

/**
 * FLEXSPI initialization routine.
 */
void imx8_flexspi_init(flexspi_t *flexspi, const unsigned base, const int verbose)
{
    uint32_t mcr_reg;

    flexspi->pbase = base;
    flexspi->verbose = (unsigned int)verbose;

    /* Configure FLEXSPI to use IP access, COMBINATION mode and strobe from DQS pad */
    mcr_reg = in32(flexspi->pbase + IMX_FLEXSPI_MCR0);
    mcr_reg &= ~(IMX_FLEXSPI_MCR0_ARDFEN_MASK | IMX_FLEXSPI_MCR0_ATDFEN_MASK | IMX_FLEXSPI_MCR0_RXCLKSRC_MASK | IMX_FLEXSPI_MCR0_COMBINATIONEN_MASK);
    mcr_reg |= ((flexspi->conf->pads == 8) ? IMX_FLEXSPI_MCR0_COMBINATIONEN_MASK : 0) |
               (flexspi->conf->smpl << IMX_FLEXSPI_MCR0_RXCLKSRC_SHIFT);
    out32(flexspi->pbase + IMX_FLEXSPI_MCR0, mcr_reg);
    /* Enable delay line calibration */
    out32(flexspi->pbase + IMX_FLEXSPI_DLLACR, IMX_FLEXSPI_DLLACR_DLLEN_MASK);
    /* Clear RX and TX FIFOs */
    out32(flexspi->pbase + IMX_FLEXSPI_IPTXFCR, IMX_FLEXSPI_IPTXFCR_CLRIPTXF_MASK);
    out32(flexspi->pbase + IMX_FLEXSPI_IPRXFCR, IMX_FLEXSPI_IPRXFCR_CLRIPRXF_MASK);
    /* Disable all interrupts */
    out32(flexspi->pbase + IMX_FLEXSPI_INTEN, 0x0);
    /* Clear all interrupts */
    out32(flexspi->pbase + IMX_FLEXSPI_INTR, 0xFFF);
}

/**
 * Read data from given offset.
 *
 * @param offset  Memory offset.
 * @param buffer  Pointer to data read buffer.
 * @param size    Size of data to read.
 *
 * @return Data that left to be transfered. -1 if any error.
 */
int imx8_flexspi_read(flexspi_t * const flexspi, unsigned long offset, unsigned long buffer, unsigned size)
{
    const uint32_t lut_index = 0;             /* LUT read command configuration on index 0 */
    uint32_t ipr_reg;
    uint32_t data_size;
    uint32_t watermark;

    if (size == 0) {
        return -1;
    }
    /*
     * Read command (programmed by ROM boot (index 0))
     */
    while (size) {
        /* Send data up to FIFO size */
        data_size = MIN(size, flexspi->conf->rx_fifo_size);
        watermark = MIN(size, FLEXSPI_MAX_BURST_RX);
        /* Enable clock to module */
        out32(flexspi->pbase + IMX_FLEXSPI_MCR0, in32(flexspi->pbase + IMX_FLEXSPI_MCR0) &
                                                    ~IMX_FLEXSPI_MCR0_MDIS_MASK);
        /* Clear all interrupts */
        out32(flexspi->pbase + IMX_FLEXSPI_INTR, 0xFFF);
        /* Overwrite ROM read command to have boot-loader defined read command */
        out32(flexspi->pbase + IMX_FLEXSPI_LUTa(0), flexspi->conf->lut0);
        out32(flexspi->pbase + IMX_FLEXSPI_LUTa(1), flexspi->conf->lut1);
        out32(flexspi->pbase + IMX_FLEXSPI_LUTa(2), flexspi->conf->lut2);
        out32(flexspi->pbase + IMX_FLEXSPI_LUTa(3), flexspi->conf->lut3);
        /* Write address to controller */
        out32(flexspi->pbase + IMX_FLEXSPI_IPCR0, (unsigned)offset);
        /* Clear Rx FIFO */
        ipr_reg = in32(flexspi->pbase + IMX_FLEXSPI_IPRXFCR);
        ipr_reg |= IMX_FLEXSPI_IPRXFCR_CLRIPRXF_MASK;
        out32(flexspi->pbase + IMX_FLEXSPI_IPRXFCR, ipr_reg);
        flexspi_set_rx_watermark(flexspi, watermark);
        /* Clear IP and Rx flags */
        out32(flexspi->pbase + IMX_FLEXSPI_INTR, IMX_FLEXSPI_INTR_IPRXWA_MASK | IMX_FLEXSPI_INTR_IPCMDDONE_MASK);
        /* Set IP command sequence index */
        out32(flexspi->pbase + IMX_FLEXSPI_IPCR1, (lut_index << IMX_FLEXSPI_IPCR1_ISEQID_SHIFT) | data_size);
        /* Trigger IP command */
        out32(flexspi->pbase + IMX_FLEXSPI_IPCMD, 0x1);
        /* Wait for CMD completion */
        while (!(in32(flexspi->pbase + IMX_FLEXSPI_INTR) & IMX_FLEXSPI_INTEN_IPCMDDONEEN_MASK));
        /* Wait for FIFO fill */
        while (data_size > 0) {
            while (!(in32(flexspi->pbase + IMX_FLEXSPI_INTR) & IMX_FLEXSPI_INTEN_IPRXWAEN_MASK));
            /* Read data from hw */
            copy(buffer, (flexspi->pbase + IMX_FLEXSPI_RFDR0), watermark);
            /* Clear IP and Rx flags */
            out32(flexspi->pbase + IMX_FLEXSPI_INTR, IMX_FLEXSPI_INTR_IPRXWA_MASK | IMX_FLEXSPI_INTR_IPCMDDONE_MASK);
            size -= watermark;
            data_size -= watermark;
            buffer += watermark;
            offset += watermark;
            watermark = MIN(data_size, FLEXSPI_MAX_BURST_RX);
            flexspi_set_rx_watermark(flexspi, watermark);
        }
    }

    return size;
}
#endif /* __arch64__ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/boards/imx8mp-evk/imx8_flexspi.c $ $Rev: 980064 $")
#endif
