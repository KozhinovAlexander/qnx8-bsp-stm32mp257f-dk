/*
 * Copyright (c) 2007, 2008, 2023, BlackBerry Limited.
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

#ifndef SERMX1_PROTO_H_
#define SERMX1_PROTO_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/dcmd_chr.h>
#include <atomic.h>
#include <hw/inout.h>
#include <sys/io-char.h>
#include <drvr/hwinfo.h>
#include <sys/dispatch.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

#include "externs.h"

#define MX1_UART_SIZE           0xE0

#define MX1_UART_RXDATA         0x00        /* Receiver Register */
#define MX1_UART_TXDATA         0x40        /* Transmitter Register */
#define MX1_UART_CR1            0x80        /* Control Register 1 */
#define MX1_UART_CR2            0x84        /* Control Register 2 */
#define MX1_UART_CR3            0x88        /* Control Register 3 */
#define MX1_UART_CR4            0x8C        /* Control Register 4 */
#define MX1_UART_FCR            0x90        /* FIFO Control Register */
#define MX1_UART_SR1            0x94        /* Status Register 1 */
#define MX1_UART_SR2            0x98        /* Status Register 2 */
#define MX1_UART_ESC            0x9C        /* Escape Character Register */
#define MX1_UART_TIM            0xA0        /* Escape Timer Register */
#define MX1_UART_BIR            0xA4        /* BRM Incremental Register */
#define MX1_UART_BMR            0xA8        /* BRM Modulator Register */
#define MX1_UART_BRC            0xAC        /* Baud Rate Count Register */
#define MX1_UART_BIPR1          0xB0        /* BRM Incremental Preset Register 1 */
#define MX1_UART_BIPR2          0xB4        /* BRM Incremental Preset Register 2 */
#define MX1_UART_BIPR3          0xB8        /* BRM Incremental Preset Register 3 */
#define MX1_UART_BIPR4          0xBC        /* BRM Incremental Preset Register 4 */
#define MX1_UART_BMPR1          0xC0        /* BRM Modulator Preset Register */
#define MX1_UART_BMPR2          0xC4        /* BRM Modulator Preset Register */
#define MX1_UART_BMPR3          0xC8        /* BRM Modulator Preset Register */
#define MX1_UART_BMPR4          0xCC        /* BRM Modulator Preset Register */
#define MX1_UART_TS             0xD0        /* Test Register */

/*
 * Receiver Register bits
 */
#define MX1_URXD_CHARRDY        (1 << 15)   /* Character Ready */
#define MX1_URXD_ERR            (1 << 14)   /* Error Detect */
#define MX1_URXD_OVERRUN        (1 << 13)   /* Receiver Overrun */
#define MX1_URXD_FRMERR         (1 << 12)   /* Frame Error */
#define MX1_URXD_BRK            (1 << 11)   /* BREAK detect */
#define MX1_URXD_PRERR          (1 << 10)   /* Parity Error */

/*
 * Control Register 1 bits
 */
#define MX1_UCR1_ADEN           (1 << 15)   /* Automatic Baud Rate Detection Interrupt Enable */
#define MX1_UCR1_ADBR           (1 << 14)   /* Automatic Detection of Baud Rate */
#define MX1_UCR1_TRDYEN         (1 << 13)   /* Transmitter Ready Interrupt Enable */
#define MX1_UCR1_IDEN           (1 << 12)   /* Idle Condition Detected Interrupt */
#define MX1_UCR1_ICD_MASK       (3 << 10)   /* Idle Condition Detect Mask */
#define MX1_UCR1_RRDYEN         (1 << 9)    /* Receiver Ready Interrupt Enable */
#define MX1_UCR1_RDMAEN         (1 << 8)    /* Receive Ready DMA Enable */
#define MX1_UCR1_IREN           (1 << 7)    /* Infrared Interface Enable */
#define MX1_UCR1_TXMPTYEN       (1 << 6)    /* Transmitter Empty Interrupt Enable */
#define MX1_UCR1_RTSDEN         (1 << 5)    /* RTS Delta Interrupt Enable */
#define MX1_UCR1_SNDBRK         (1 << 4)    /* Send BREAK */
#define MX1_UCR1_TDMAEN         (1 << 3)    /* Transmitter Ready DMA Enable */
#define MX1_UCR1_ATDMAEN        (1 << 2)    /* Aging DMA timer enable */
#define MX1_UCR1_DOZE           (1 << 1)    /* UART DOZE State Control */
#define MX1_UCR1_UARTEN         (1 << 0)    /* UART Enable */

/*
 * Control Register 2 bits
 */
#define MX1_UCR2_ESCI           (1 << 15)   /* Escape Sequence Interrupt Enable */
#define MX1_UCR2_IRTS           (1 << 14)   /* Ignore UART RTS pin */
#define MX1_UCR2_CTSC           (1 << 13)   /* UART CTS pin Control */
#define MX1_UCR2_CTS            (1 << 12)   /* Clear To Send */
#define MX1_UCR2_ESCEN          (1 << 11)   /* Escape Enable */
#define MX1_UCR2_RTEC_MASK      (3 << 9)    /* Request to Send Edge Control Mask */
#define MX1_UCR2_PREN           (1 << 8)    /* Parity Enable */
#define MX1_UCR2_PROE           (1 << 7)    /* Parity Odd/Even */
#define MX1_UCR2_STPB           (1 << 6)    /* Stop Bit */
#define MX1_UCR2_WS             (1 << 5)    /* Word Size */
#define MX1_UCR2_RTSEN          (1 << 4)    /* Request to Send Interrupt Enable */
#define MX1_UCR2_ATEN           (1 << 3)    /* Ageing Timer Interrupt Enable */
#define MX1_UCR2_TXEN           (1 << 2)    /* Transmitter Enable */
#define MX1_UCR2_RXEN           (1 << 1)    /* Receiver Enable */
#define MX1_UCR2_SRST           (1 << 0)    /* Software Reset */

/*
 * Control Register 3 bits
 */
#define MX1_UCR3_DPEC_MASK      (3 << 14)   /* DTR Interrupt Edge Control */
#define MX1_UCR3_DTREN          (1 << 13)   /* Data Terminal Ready Interrupt Enable */
#define MX1_UCR3_PARERREN       (1 << 12)   /* Parity Error Interrupt Enable */
#define MX1_UCR3_FRAERREN       (1 << 11)   /* Frame Error Interrupt Enable */
#define MX1_UCR3_DSR            (1 << 10)   /* Data Set Ready */
#define MX1_UCR3_DCD            (1 << 9)    /* Data Carrier Detect */
#define MX1_UCR3_RI             (1 << 8)    /* Ring Indicator */
#define MX1_UCR3_ADNIMP         (1 << 7)    /* Autobaud Detection Not Improved */
#define MX1_UCR3_RXDSEN         (1 << 6)    /* Receive Status Interrupt Enable */
#define MX1_UCR3_AIRINTEN       (1 << 5)    /* Asynchronous IR WAKE Interrupt Enable */
#define MX1_UCR3_AWAKEN         (1 << 4)    /* Asynchronous WAKE Interrupt Enable */
#define MX1_UCR3_DTRDEN         (1 << 3)    /* Data Terminal Ready Delta Enable */
#define MX1_UCR3_RXDMUXSEL      (1 << 2)    /* RXD Muxed Input Selected */
#define MX1_UCR3_INVT           (1 << 1)    /* Inverted Infrared Transmission */
#define MX1_UCR3_ACIEN          (1 << 0)    /* Autobaud Counter Interrupt Enable */

/*
 * Control Register 4 bits
 */
#define MX1_UCR4_CTSTL_MASK     (0x3F << 10)/* CTS Trigger Level Mask */
#define MX1_UCR4_INVR           (1 << 9)    /* Inverted Infrared Reception */
#define MX1_UCR4_ENIRI          (1 << 8)    /* Serial Infrared Interrupt Enable */
#define MX1_UCR4_WKEN           (1 << 7)    /* WAKE Interrupt Enable */
#define MX1_UCR4_IDDMAEN        (1 << 6)    /* DMA IDLE Condition Detected Interrupt Enable */
#define MX1_UCR4_IRSC           (1 << 5)    /* IR Special Case */
#define MX1_UCR4_LPBYP          (1 << 4)    /* Low Power Bypass */
#define MX1_UCR4_TCEN           (1 << 3)    /* Transmit Complete Interrupt Enable */
#define MX1_UCR4_BKEN           (1 << 2)    /* BREAK Condition Detected Interrupt Enable */
#define MX1_UCR4_OREN           (1 << 1)    /* Receive Overrun Interrupt Enable */
#define MX1_UCR4_DREN           (1 << 0)    /* Receive Data Ready Interrupt Enable */

/*
 * FIFO Control Register bits
 */
#define MX1_UFCR_TXTL_MASK      (0x3F << 10)/* Transmitter Trigger Level Mask*/
#define MX1_UFCR_RFDIV_MASK     (0x7 << 7)  /* Reference Frequency Divider Mask */
#define MX1_UFCR_DCEDTE         (1 << 6)    /* DCE/DTE mode select */
#define MX1_UFCR_RXTL_MASK      (0x3F << 0) /* Receive Trigger Level Mask */

/*
 * Status Register 1 bits
 */
#define MX1_USR1_PARITYERR      (1 << 15)   /* Parity Error Interrupt Flag */
#define MX1_USR1_RTSS           (1 << 14)   /* RTS Pin Status */
#define MX1_USR1_TRDY           (1 << 13)   /* Transmitter Ready Interrupt/DMA Flag */
#define MX1_USR1_RTSD           (1 << 12)   /* RTS Delta */
#define MX1_USR1_ESCF           (1 << 11)   /* Escape Sequence Interrupt Flag */
#define MX1_USR1_FRAMERR        (1 << 10)   /* Frame Error Interrupt Flag */
#define MX1_USR1_RRDY           (1 << 9)    /* Receiver Ready Interrupt/DMA Flag */
#define MX1_USR1_AGTIM          (1 << 8)    /* Ageing Timer Interrupt Flag */
#define MX1_USR1_DTRD           (1 << 7)    /* DTR Delta */
#define MX1_USR1_RXDS           (1 << 6)    /* Receiver IDLE Interrupt Flag */
#define MX1_USR1_AIRINT         (1 << 5)    /* Asynchronous IR WAKE Interrupt Flag */
#define MX1_USR1_AWAKE          (1 << 4)    /* Asynchronous WAKE Interrupt Flag */
#define MX1_USR1_SAD            (1 << 3)    /* RS-485 Slave Address Detected Interrupt Flag */

/*
 * Status Register 2 bits
 */
#define MX1_USR2_ADET           (1 << 15)   /* Automatic Baud Rate Detect Complete */
#define MX1_USR2_TXFE           (1 << 14)   /* Transmit Buffer FIFO Empty */
#define MX1_USR2_DTRF           (1 << 13)   /* DTR Edge Triggered Interrupt Flag */
#define MX1_USR2_IDLE           (1 << 12)   /* IDLE Condition */
#define MX1_USR2_ACST           (1 << 11)   /* Autobaud Counter Stopped */
#define MX1_USR2_RIDLET         (1 << 10)   /* Ring Indicator Delta */
#define MX1_USR2_RIIN           (1 << 9)    /* Ring Indicator Input */
#define MX1_USR2_IRINT          (1 << 8)    /* Serial Infrared Interrupt Flag */
#define MX1_USR2_WAKE           (1 << 7)    /* WAKE */
#define MX1_USR2_DCDDELT        (1 << 6)    /* Data Carrier Detect delta */
#define MX1_USR2_DCDIN          (1 << 5)    /* Data Carrier Detect input */
#define MX1_USR2_RTSF           (1 << 4)    /* RTS Edge Triggered Interrupt Flag */
#define MX1_USR2_TXDC           (1 << 3)    /* Transmitter Complete */
#define MX1_USR2_BRCD           (1 << 2)    /* BREAK Condition Detected */
#define MX1_USR2_ORE            (1 << 1)    /* Overrun Error */
#define MX1_USR2_RDR            (1 << 0)    /* Receive Data Ready */

#define MX1_UART_FIFO_SIZE      32
#define MX1_UART_MIN_TX_FIFO    2

typedef struct dev_mx1 {
    TTYDEV          tty;
    int             intr;
    int             iid;
    unsigned        clk;
    unsigned        div;
    unsigned        fifo;
    uintptr_t       base;
    unsigned        fcr;
    unsigned        cr2;
    unsigned        bir;
} DEV_MX1;

void        create_device(const TTYINIT *const dip);
void        ser_stty(DEV_MX1 *dev);
void        ser_ctrl(DEV_MX1 *dev, unsigned flags);
void        ser_attach_intr(DEV_MX1 *dev);
void        ser_detach_intr(DEV_MX1 *dev);
unsigned    ser_options(int argc, char *argv[]);
int         edit(TTYDEV *dev, unsigned ch);

#endif /* SERMX1_PROTO_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devc/sermx1/proto.h $ $Rev: 987518 $")
#endif
