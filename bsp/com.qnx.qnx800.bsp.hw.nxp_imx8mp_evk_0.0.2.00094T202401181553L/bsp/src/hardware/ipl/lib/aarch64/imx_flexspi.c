/*
 * $QNXLicenseC:
 * Copyright 2018, 2023, QNX Software Systems.
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

/**
 * i.MX IPL source file.
 *
 * @file       imx_flexspi.c
 * @addtogroup ipl
 * @{
 */

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define FLEXSPI_MAX_BURST_RX    128
#define FLEXSPI_RXFIFO_SIZE     512

#define FLEXSPI_REG_DUMP    0

static const unsigned dflt_seqs[] = {
    0x0B2004CCU,        /* instruction 1/0 */
    0x27043310U,        /* instruction 3/2 */
    0U,                 /* instruction 5/4 */
    0U,                 /* instruction 7/6 */
};

#if FLEXSPI_REG_DUMP
 /**
 * Flexspi register dump.
 *
 * @param base Peripheral base address.
 */
static void flexspi_reg_dump(unsigned long base)
{
    kprintf("IMX_FLEXSPI_MCR0: 0x%x\n", in32(base + IMX_FLEXSPI_MCR0));
    kprintf("IMX_FLEXSPI_MCR1: 0x%x\n", in32(base + IMX_FLEXSPI_MCR1));
    kprintf("IMX_FLEXSPI_MCR2: 0x%x\n", in32(base + IMX_FLEXSPI_MCR2));
    kprintf("IMX_FLEXSPI_AHBCR: 0x%x\n", in32(base + IMX_FLEXSPI_AHBCR));
    kprintf("IMX_FLEXSPI_INTEN: 0x%x\n", in32(base + IMX_FLEXSPI_INTEN));
    kprintf("IMX_FLEXSPI_INTR: 0x%x\n", in32(base + IMX_FLEXSPI_INTR));
    kprintf("IMX_FLEXSPI_FLSHA1CR0: 0x%x\n", in32(base + IMX_FLEXSPI_FLSHA1CR0));
    kprintf("IMX_FLEXSPI_FLSHA2CR0: 0x%x\n", in32(base + IMX_FLEXSPI_FLSHA2CR0));
    kprintf("IMX_FLEXSPI_FLSHB1CR0: 0x%x\n", in32(base + IMX_FLEXSPI_FLSHB1CR0));
    kprintf("IMX_FLEXSPI_FLSHB2CR0: 0x%x\n", in32(base + IMX_FLEXSPI_FLSHB2CR0));
    kprintf("IMX_FLEXSPI_IPCR0: 0x%x\n", in32(base + IMX_FLEXSPI_IPCR0));
    kprintf("IMX_FLEXSPI_IPCR1: 0x%x\n", in32(base + IMX_FLEXSPI_IPCR1));
    kprintf("IMX_FLEXSPI_IPCMD: 0x%x\n", in32(base + IMX_FLEXSPI_IPCMD));
    kprintf("IMX_FLEXSPI_DLLACR: 0x%x\n", in32(base + IMX_FLEXSPI_DLLACR));
    kprintf("IMX_FLEXSPI_DLLBCR: 0x%x\n", in32(base + IMX_FLEXSPI_DLLBCR));
    kprintf("IMX_FLEXSPI_STS0: 0x%x\n", in32(base + IMX_FLEXSPI_STS0));
    kprintf("IMX_FLEXSPI_STS1: 0x%x\n", in32(base + IMX_FLEXSPI_STS1));
    kprintf("IMX_FLEXSPI_STS2: 0x%x\n", in32(base + IMX_FLEXSPI_STS2));
    kprintf("IMX_FLEXSPI_IPRXFSTS: 0x%x\n", in32(base + IMX_FLEXSPI_IPRXFSTS));
    kprintf("IMX_FLEXSPI_IPTXFSTS: 0x%x\n", in32(base + IMX_FLEXSPI_IPTXFSTS));

    for (int i = 0; i < IMX_FLEXSPI_LUTa_NUM; i++) {
        kprintf("IMX_FLEXSPI_LUT: %d 0x%x\n", in32(base + IMX_FLEXSPI_LUTa(i)));
    }
}
#endif

/**
 * Set Rx water-mark.
 *
 * @param wtmk  RX watermark value.
 */
static void flexspi_set_rx_watermark(const unsigned long base, const uint32_t wtmk)
{
    /* Set watermark */
    /* Data transfer size logic must be dword aligned */
    out32(base + IMX_FLEXSPI_IPRXFCR, ((wtmk + 7) / 8 - 1) << IMX_FLEXSPI_IPRXFCR_RXWMRK_SHIFT);
}

/**
 * FLEXSPI initialization routine.
 */
void imx_flexspi_init(const unsigned long base, const unsigned octal, const unsigned smpl, const unsigned *seqs)
{
    uint32_t mcr_reg;

    if (seqs == 0) {
        seqs = dflt_seqs;
    }

    /* Reset controller and ensure that module clock is disabled */
    out32(base + IMX_FLEXSPI_MCR0, in32(base + IMX_FLEXSPI_MCR0) |
                                                  IMX_FLEXSPI_MCR0_MDIS_MASK);
    /* Configure FLEXSPI to use IP access, COMBINATION mode and strobe from DQS pad */
    mcr_reg = in32(base + IMX_FLEXSPI_MCR0);
    mcr_reg &= ~(IMX_FLEXSPI_MCR0_ARDFEN_MASK | IMX_FLEXSPI_MCR0_ATDFEN_MASK | IMX_FLEXSPI_MCR0_RXCLKSRC_MASK | IMX_FLEXSPI_MCR0_COMBINATIONEN_MASK);
    mcr_reg |= (octal ? IMX_FLEXSPI_MCR0_COMBINATIONEN_MASK : 0) | (smpl << IMX_FLEXSPI_MCR0_RXCLKSRC_SHIFT);
    out32(base + IMX_FLEXSPI_MCR0, mcr_reg);
    /* Enable delay line calibration */
    out32(base + IMX_FLEXSPI_DLLACR, IMX_FLEXSPI_DLLACR_DLLEN_MASK);
    /* Clear RX and TX FIFOs */
    out32(base + IMX_FLEXSPI_IPTXFCR, IMX_FLEXSPI_IPTXFCR_CLRIPTXF_MASK);
    out32(base + IMX_FLEXSPI_IPRXFCR, IMX_FLEXSPI_IPRXFCR_CLRIPRXF_MASK);
    /* Disable all interrupts */
    out32(base + IMX_FLEXSPI_INTEN, 0x0);
    /* Clear all interrupts */
    out32(base + IMX_FLEXSPI_INTR, 0xFFF);
    /* Enable clock to module */
    out32(base + IMX_FLEXSPI_MCR0, in32(base + IMX_FLEXSPI_MCR0) &
                                                ~IMX_FLEXSPI_MCR0_MDIS_MASK);
    /* Unlock LUT */
    out32(base + IMX_FLEXSPI_LUTKEY, IMX_FLEXSPI_LUT_KEY_VAL);
    out32(base + IMX_FLEXSPI_LUTCR, IMX_FLEXSPI_LUTCR_UNLOCK_MASK);
    /* Overwrite ROM read command to have boot-loader defined read command */
    out32(base + IMX_FLEXSPI_LUTa(0), seqs[0]);
    out32(base + IMX_FLEXSPI_LUTa(1), seqs[1]);
    out32(base + IMX_FLEXSPI_LUTa(2), seqs[2]);
    out32(base + IMX_FLEXSPI_LUTa(3), seqs[3]);
    /* Lock LUT */
    out32(base + IMX_FLEXSPI_LUTKEY, IMX_FLEXSPI_LUT_KEY_VAL);
    out32(base + IMX_FLEXSPI_LUTCR, IMX_FLEXSPI_LUTCR_LOCK_MASK);
    /* Reset controller */
    out32(base + IMX_FLEXSPI_MCR0, in32(base + IMX_FLEXSPI_MCR0) |
                                                  IMX_FLEXSPI_MCR0_SWRESET_MASK);
#if FLEXSPI_REG_DUMP
    flexspi_reg_dump(base);
#endif
}

/**
 * Read data from given offset.
 *
 * @param seqs    Command sequence for flash read (optional).
 * @param offset  Memory offset.
 * @param buffer  Address of data read buffer.
 * @param size    Size of data to read.
 *
 * @return Data that left to be transfered. -1 if any error.
 */
int imx_flexspi_read(const unsigned long base, const unsigned *seqs, unsigned offset, unsigned long buffer, unsigned size)
{
    uint32_t ipr_reg;
    uint32_t data_size;
    uint32_t watermark;

    if (size == 0) {
        return -1;
    }
    if (seqs == 0) {
        seqs = dflt_seqs;
    }
    /*
     * Read data from flash device by LUT command at index 0
     */
    while (size) {
        /* Send data up to FIFO size */
        data_size = MIN(size, FLEXSPI_RXFIFO_SIZE);
        watermark = MIN(size, FLEXSPI_MAX_BURST_RX);
        /* Clear all interrupts */
        out32(base + IMX_FLEXSPI_INTR, 0xFFF);
        /* Write address to controller */
        out32(base + IMX_FLEXSPI_IPCR0, offset);
        /* Clear Rx FIFO */
        ipr_reg = in32(base + IMX_FLEXSPI_IPRXFCR);
        ipr_reg |= IMX_FLEXSPI_IPRXFCR_CLRIPRXF_MASK;
        out32(base + IMX_FLEXSPI_IPRXFCR, ipr_reg);
        flexspi_set_rx_watermark(base, watermark);
        /* Clear IP and Rx flags */
        out32(base + IMX_FLEXSPI_INTR, IMX_FLEXSPI_INTR_IPRXWA_MASK | IMX_FLEXSPI_INTR_IPCMDDONE_MASK);
        /* Set IP command sequence index, 0 */
        out32(base + IMX_FLEXSPI_IPCR1, (0 << IMX_FLEXSPI_IPCR1_ISEQID_SHIFT) | data_size);
        /* Trigger IP command */
        out32(base + IMX_FLEXSPI_IPCMD, 0x1);
        /* Wait for CMD completion */
        while (!(in32(base + IMX_FLEXSPI_INTR) & IMX_FLEXSPI_INTEN_IPCMDDONEEN_MASK)) {
        }
        /* Wait for FIFO fill */
        while (data_size > 0) {
            while (!(in32(base + IMX_FLEXSPI_INTR) & IMX_FLEXSPI_INTEN_IPRXWAEN_MASK)) {
            }
            /* Read data from hw */
            copy(buffer, (base + IMX_FLEXSPI_RFDR0), watermark);
            /* Clear IP and Rx flags */
            out32(base + IMX_FLEXSPI_INTR, IMX_FLEXSPI_INTR_IPRXWA_MASK | IMX_FLEXSPI_INTR_IPCMDDONE_MASK);
            size -= watermark;
            data_size -= watermark;
            buffer += watermark;
            offset += watermark;
            watermark = MIN(data_size, FLEXSPI_MAX_BURST_RX);
            flexspi_set_rx_watermark(base, watermark);
        }
    }

    return size;
}

/** @} */ /* End of imx8_flexspi */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/aarch64/imx_flexspi.c $ $Rev: 975396 $")
#endif
