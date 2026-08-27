/*
 * Copyright (c) 2010,2023, BlackBerry Limited.
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

#ifndef _ECSPI_H_INCLUDED
#define _ECSPI_H_INCLUDED

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <hw/io-spi.h>

/*
 * Enhanced Configurable Serial Peripheral Interface (eCSPI)
 */
#define ECSPI1_BASE                 0x30820000
#define ECSPI2_BASE                 0x30830000
#define ECSPI3_BASE                 0x30840000

#define ECSPI1_IRQ                  63
#define ECSPI2_IRQ                  64
#define ECSPI3_IRQ                  65

#define ECSPI_SIZE                  0x10000
#define ECSPI_COUNT                 3

#define ECSPI_BURST_MAX             0x200
#define ECSPI_FIFO_SIZE             0x40
#define ECSPI_FIFO_RXMARK           (ECSPI_FIFO_SIZE >> 1)

#define SPI_MAX_BUS                 3
#define SPI_MAX_DEV                 3
#define SPI_MAX_SSID                (SPI_MAX_DEV - 1)
#define SPI_MAX_BUSID               (SPI_MAX_BUS - 1)

#define ECSPI_INPUT_CLK             80000000

#define ECSPI_RXDATA                0x00    /* Receive data register */
#define ECSPI_TXDATA                0x04    /* Transmit data register */
#define ECSPI_CONTROLREG            0x08    /* Control register */
#define ECSPI_CONFIGREG             0x0C    /* Config register */
#define ECSPI_INTREG                0x10    /* Interrupt control register */
#define ECSPI_DMAREG                0x14    /* DMA control register */
#define ECSPI_STATREG               0x18    /* Status register */
#define ECSPI_PERIODREG             0x1C    /* Sample period control register */
#define ECSPI_TESTREG               0x20    /* Test register */
#define ECSPI_MSGDATAREG            0x40    /* Message data register */

// CONTROLREG BIT Definitions
#define ECSPI_CONTROLREG_ENABLE                 0x1
#define ECSPI_CONTROLREG_HW                     0x2
#define ECSPI_CONTROLREG_XCH                    0x4
#define ECSPI_CONTROLREG_SMC                    0x8
#define ECSPI_CONTROLREG_CH_MODE_POS            4
#define ECSPI_CONTROLREG_CH_MODE_MASK           0x000000f0
#define ECSPI_CONTROLREG_POST_DVDR              0x00000f00
#define ECSPI_CONTROLREG_PRE_DVDR               0x0000f000
#define ECSPI_CONTROLREG_DRCTL_MASK             0x00030000
#define ECSPI_CONTROLREG_DRCTL_POS              16
#define ECSPI_CONTROLREG_DRCTL_EDGE             1
#define ECSPI_CONTROLREG_DRCTL_LEVEL            2
#define ECSPI_CONTROLREG_CSEL_MASK              0x000C0000
#define ECSPI_CONTROLREG_CSEL_POS               18
#define ECSPI_CONTROLREG_BCNT_MASK              0xFFF00000
#define ECSPI_CONTROLREG_BCNT_POS               20
#define ECSPI_CONREG_PREDIVIDR_POS              12
#define ECSPI_CONREG_POSTDIVIDR_POS             8

// CONFIGREG BIT Definitions
#define ECSPI_CONFIGREG_PHA_MASK                0x0000000f
#define ECSPI_CONFIGREG_PHA_POS                 0
#define ECSPI_CONFIGREG_POL_MASK                0x000000f0
#define ECSPI_CONFIGREG_POL_POS                 4
#define ECSPI_CONFIGREG_SSCTL_MASK              0x00000f00
#define ECSPI_CONFIGREG_SSCTL_POS               8
#define ECSPI_CONFIGREG_SSPOL_MASK              0x0000f000
#define ECSPI_CONFIGREG_SSPOL_POS               12
#define ECSPI_CONFIGREG_DATACTL_MASK            0x000f0000
#define ECSPI_CONFIGREG_DATACTL_POS             16
#define ECSPI_CONFIGREG_CLKCTL_MASK             0x00f00000
#define ECSPI_CONFIGREG_CLKCTL_POS              20

// INTREG BIT Definitions
#define ECSPI_INTREG_TEEN                       0x1
#define ECSPI_INTREG_TDREN                      0x2
#define ECSPI_INTREG_TFEN                       0x4
#define ECSPI_INTREG_RREN                       0x8
#define ECSPI_INTREG_RDREN                      0x10
#define ECSPI_INTREG_RFEN                       0x20
#define ECSPI_INTREG_ROEN                       0x40
#define ECSPI_INTREG_TCEN                       0x80

// STATREG (Status Reg) BIT Definitions
#define ECSPI_STATREG_TE                        0x1
#define ECSPI_STATREG_TDR                       0x2
#define ECSPI_STATREG_TF                        0x4
#define ECSPI_STATREG_RR                        0x8
#define ECSPI_STATREG_RDR                       0x10
#define ECSPI_STATREG_RF                        0x20
#define ECSPI_STATREG_RO                        0x40
#define ECSPI_STATREG_TC                        0x80

// PERIODREG Definitions
#define ECSPI_PERIODREG_SP_MASK                 0x00007fff
#define ECSPI_PERIODREG_32K_CLK                 0x00008000
#define ECSPI_PERIODREG_CSD_CTRL_MASK           0x003f0000
#define ECSPI_PERIODREG_CSD_CTRL_POS            16

// TESTREG BIT Definitions
#define ECSPI_TESTREG_LOOPBACK                  (1 << 31)
#define ECSPI_TESTREG_RXCNT                     8
#define ECSPI_TESTREG_RXCNT_MASK                0x00007f00
#define ECSPI_TESTREG_TXCNT_MASK                0x0000007F

typedef struct {
        uint32_t ctrl_reg;
        uint32_t cfg_reg;
} spi_dev_params_t;

/* SPI Low level driver structure */
typedef struct {
    uint64_t         pbase;                     /* Peripheral physical address */
    uintptr_t        vbase;                     /* Peripheral virtual address */
    int              irq;                       /* SPI interrupt HW number */
    int              iid;
    struct sigevent  spievent;                  /* SPI Interrupt event */
    uint32_t         input_clk;                 /* SPI source clock frequency */
    uint32_t         fifo_size;                 /* FIFO size (In size of SPI words) */
    uint32_t         word_len;                  /* SPI word length in bytes */
    uint32_t         xlen;                      /* Transfer length */
    uint32_t         tlen;                      /* Transmit counter */
    uint32_t         rlen;                      /* Receive counter */
    uint32_t         dwidth;                    /* Data width in bytes */
    uint32_t         bxlen;                     /* used for mutiply burst, exchange length for the current burst*/
    uint32_t         btlen;                     /* used for mutiply burst, transmit length for the current burst*/
    uint32_t         brlen;                     /* used for mutiply burst, recive length for the current burst*/
    uint64_t         dtime;                     /* nsec per data, for time out use */
    uint8_t          *pbuf;                     /* Pointer to data in interrupt mode */
    spi_ctrl_t       *spi_ctrl;                 /* The address of spi_ctrl structure */
    spi_bus_t        *bus_node;                 /* The address of bus structure which is passed in by spi_init() */
    uint16_t         waitstates;
    uint8_t          burst;
    uint8_t          csdelay;
    uint8_t          loopback;
    uint8_t          lsb_adjust;
} ecspi_t;

int spi_init(spi_bus_t *bus);
void ecspi_devinfo(const void *const hdl, const spi_dev_t *const spi_dev, spi_devinfo_t *const info);
void ecspi_drvinfo(const void *const hdl, spi_drvinfo_t *info);
int ecspi_cfg(const ecspi_t *const spi, spi_dev_t *spi_dev);
int ecspi_setcfg(const void *const hdl, spi_dev_t *spi_dev, const spi_cfg_t *const cfg);
int ecspi_attach_intr(ecspi_t *spi);
void ecspi_fini(void *const hdl);
int ecspi_xfer(void *const hdl, spi_dev_t *const spi_dev, uint8_t *const buf, const uint32_t tnbytes, const uint32_t rnbytes);
int ecspi_wait(ecspi_t *const spi, const uint32_t len);
int process_interrupts(ecspi_t *const dev);
spi_dev_params_t *const get_dev_params(const uint32_t busid, const uint32_t ssid);
void ecspi_spi_disable(ecspi_t *const spi);

#endif

#if defined(QNXNTO) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/spi/ecspi/ecspi.h $ $Rev: 980075 $")
#endif
