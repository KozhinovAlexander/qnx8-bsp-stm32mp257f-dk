/*
 * Copyright (c) 2020, 2021, 2023, BlackBerry Limited.
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

#ifndef _FLEXCAN_H_INCLUDED
#define _FLEXCAN_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <devctl.h>
#include <atomic.h>
#include <unistd.h>
#include <assert.h>
#include <gulliver.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <hw/libcan.h>

#include "bit_timing.h"


#define RAW_MODE_RX_NUM_MBOX        1
#define RAW_MODE_TX_NUM_MBOX        1
#define RAW_MODE_RX_IRQ             (0x01u << 0)
#define RAW_MODE_TX_IRQ             (0x01u << 1)

#define FLEXCANFD_TX_INT_MASK       0x3FFFu
#define FLEXCANFD_TX_INT_SHIFT      14

#define FLEXCAN_NUM_MB              64
#define FLEXCANFD_NUM_MB            14

/* FlexCAN */
#define FLEXCAN_SIZE                0x1000
#define FLEXCAN_MEM_OFFSET          0x80
#define FLEXCAN_MEM_SIZE            0x400

#define FLEXCAN_MAILBOX_SIZE        4
#define FLEXCAN_SET_MODE_RETRIES    255

/*
 * The crystal oscillator clock should be selected whenever a tight
 * tolerance (up to 0.1%) is required in the CAN bus timing.
 * The crystal oscillator clock has better jitter performance
 * than PLL generated clocks.
 */
#define FLEXCAN_CLK_EXTAL           24576000
#define FLEXCAN_CLK_PLL             80000000
/* Predefined bitrates */
#define CAN_BITRATE_50K             50000
#define CAN_BITRATE_125K            125000
#define CAN_BITRATE_250K            250000
#define CAN_BITRATE_500K            500000
#define CAN_BITRATE_1000K           1000000
#define CAN_BITRATE_5000K           5000000
#define CAN_BITRATE_INVALID         (-1)

/* FlexCAN Register Offsets */
#define FLEXCAN_MCR                 0x00
#define FLEXCAN_CTRL                0x04
#define FLEXCAN_TIMER               0x08
#define FLEXCAN_RXGMASK             0x10
#define FLEXCAN_RX14MASK            0x14
#define FLEXCAN_RX15MASK            0x18
#define FLEXCAN_ECR                 0x1C
#define FLEXCAN_ESR                 0x20
#define FLEXCAN_IMASK2              0x24
#define FLEXCAN_IMASK1              0x28
#define FLEXCAN_IFLAG2              0x2C
#define FLEXCAN_IFLAG1              0x30
#define FLEXCAN_CTRL2               0x34
#define FLEXCAN_ESR2                0x38
#define FLEXCAN_CRCR                0x44
#define FLEXCAN_RXFGMASK            0x48
#define FLEXCAN_RXFIR               0x4C
#define FLEXCAN_CBT                 0x50
#define FLEXCAN_IMASK4              0x68
#define FLEXCAN_IMASK3              0x6C
#define FLEXCAN_IFLAG4              0x70
#define FLEXCAN_IFLAG3              0x74

/* Message Buffers MB0-MB15 */
#define FLEXCAN_MB0                 0x80
/* Rx Individual Mask Registers RXIMR0-RXIMR15 */
#define FLEXCAN_RXIMR0              0x880

#define FLEXCAN_MECR                0xAE0
#define FLEXCAN_FDCTRL              0xC00
#define FLEXCAN_FDCBT               0xC04

/* Bit definitions for FlexCAN Module Configuration (CANMCR) Register */
#define MCR_MDIS                    (0x01u << 31)
#define MCR_FRZ                     (0x01u << 30)
#define MCR_FEN                     (0x01u << 29)
#define MCR_HALT                    (0x01u << 28)
#define MCR_NOTRDY                  (0x01u << 27)
#define MCR_WAKEMSK                 (0x01u << 26)
#define MCR_SOFTRST                 (0x01u << 25)
#define MCR_FRZACK                  (0x01u << 24)
#define MCR_SUPV                    (0x01u << 23)
#define MCR_SELFWAKE                (0x01u << 22)
#define MCR_WRN_EN                  (0x01u << 21)
#define MCR_LPM_ACK                 (0x01u << 20)
#define MCR_WAK_SRC                 (0x01u << 19)
#define MCR_DOZE                    (0x01u << 18)
#define MCR_SRX_DIS                 (0x01u << 17)
#define MCR_IRMQ                    (0x01u << 16)
#define MCR_LPRIO_EN                (0x01u << 13)
#define MCR_AEN                     (0x01u << 12)
#define MCR_FDEN                    (0x01u << 11)
#define MCR_IDAM_FormatA            (0x00u << 8)
#define MCR_IDAM_FormatB            (0x01u << 8)
#define MCR_IDAM_FormatC            (0x10u << 8)
#define MCR_IDAM_FormatD            (0x11u << 8)
#define MCR_MAXMB_MASK              (0x0000007Fu)
#define MCR_MAXMB_MAXMB(x)          ((x) & 0x7f)

/* Bit definitions for FlexCAN Control (CTRL) Register */
#define CTRL_PRESDIV(x)             (((x) & 0xff) << 24)
#define CTRL_RJW(x)                 (((x) & 0x03) << 22)
#define CTRL_PSEG1(x)               (((x) & 0x07) << 19)
#define CTRL_PSEG2(x)               (((x) & 0x07) << 16)
#define CTRL_BOFFMSK                (0x01u << 15)
#define CTRL_ERRMASK                (0x01u << 14)
#define CTRL_CLKSRC                 (0x01u << 13)
#define CTRL_LPB                    (0x01u << 12)
#define CTRL_TWRNMSK                (0x01u << 11)
#define CTRL_RWRNMSK                (0x01u << 10)
#define CTRL_SAMP                   (0x01u << 7)
#define CTRL_BOFFREC                (0x01u << 6)
#define CTRL_TSYNC                  (0x01u << 5)
#define CTRL_LBUF                   (0x01u << 4)
#define CTRL_LOM                    (0x01u << 3)
#define CTRL_PROPSEG(x)             ((x) & 0x07)
#define CTRL_PRESDIV_MAXVAL         0xFFu
#define CTRL_RJW_MAXVAL             0x3u
#define CTRL_PSEG1_MAXVAL           0x7u
#define CTRL_PSEG2_MAXVAL           0x7u
#define CTRL_PRESDIV_SAM_MIN        5u

/* Bit definitions for FlexCAN Control 2 (CTRL2) Register */
#define CTRL2_ISOCANFDEN            (0x01 << 12)
#define CTRL2_EACEN                 (0x01 << 16)
#define CTRL2_RRS                   (0x01 << 17)
#define CTRL2_MRP                   (0x01 << 18)
#define CTRL2_TASD(x)               (((x) & 0x1f) << 19)
#define CTRL2_RFFN(x)               (((x) & 0x0f) << 24)
#define CTRL2_ECRWRE                (0x01 << 29)
#define CTRL2_WRMFRZ                (0x01 << 28)

/* FlexCAN Error Counter Register */
#define ECR_RXECTR                  0xFF00
#define ECR_TXECTR                  0x00FF

/* Bit definitions for FlexCAN Error and Status (CANES) Register */
#define ESR_TWRNINT                 (0x01u << 17)
#define ESR_RWRNINT                 (0x01u << 16)
#define ESR_BITERR_RECESSIVE_DOMINANT   (0x01u << 15)
#define ESR_BITERR_DOMINANT_RECESSIVE   (0x01u << 14)
#define ESR_ACKERR                  (0x01u << 13)
#define ESR_CRCERR                  (0x01u << 12)
#define ESR_FORMERR                 (0x01u << 11)
#define ESR_STUFFERR                (0x01u << 10)
#define ESR_TXWARN                  (0x01u << 9)
#define ESR_RXWARN                  (0x01u << 8)
#define ESR_IDLE                    (0x01u << 7)
#define ESR_TX_RX                   (0x01u << 6)
#define ESR_FCS_SHIFT               4
#define ESR_FCS_MASK                (0x03u << ESR_FCS_SHIFT)
#define ESR_FCS_ERROR_ACTIVE        (0x00u << ESR_FCS_SHIFT)
#define ESR_FCS_ERROR_PASSIVE       (0x01u << ESR_FCS_SHIFT)
#define ESR_BOFFINT                 (0x01u << 2)
#define ESR_ERRINT                  (0x01u << 1)
#define ESR_WAKEINT                 (0x01u << 0)

/* FLEXCAN Bit Timing register (CBT) bits */
#define CBT_BTF                     (0x1u << 31)
#define CBT_EPRESDIV(x)             (((x) & 0x3ff) << 21)
#define CBT_ERJW(x)                 (((x) & 0x1f) << 16)
#define CBT_EPROPSEG(x)             (((x) & 0x3f) << 10)
#define CBT_EPSEG1(x)               (((x) & 0x1f) << 5)
#define CBT_EPSEG2(x)               ((x) & 0x1f)

/* FLEXCAN memory error control register (MECR) bits */
#define MECR_ECRWRDIS               (0x01u << 31)
#define MECR_HANCEI_MSK             (0x01u << 19)
#define MECR_FANCEI_MSK             (0x01u << 18)
#define MECR_CEI_MSK                (0x01u << 16)
#define MECR_HAERRIE                (0x01u << 15)
#define MECR_FAERRIE                (0x01u << 14)
#define MECR_EXTERRIE               (0x01u << 13)
#define MECR_RERRDIS                (0x01u << 9)
#define MECR_ECCDIS                 (0x01u << 8)
#define MECR_NCEFAFRZ               (0x01u << 7)

/* FLEXCAN FD control register (FDCTRL) bits */
#define FDCTRL_FDRATE               (0x1u << 31)
#define FDCTRL_MBDSR3(x)            (((x) & 0x3) << 25)
#define FDCTRL_MBDSR2(x)            (((x) & 0x3) << 22)
#define FDCTRL_MBDSR1(x)            (((x) & 0x3) << 19)
#define FDCTRL_MBDSR0(x)            (((x) & 0x3) << 16)
#define FDCTRL_TDCEN                (0x01u << 15)
#define FDCTRL_TDCFAIL              (0x01u << 14)
#define FDCTRL_TDCOFF(x)            (((x) & 0x1f) << 8)

/* FLEXCAN FD Bit Timing register (FDCBT) bits */
#define FDCBT_FPRESDIV(x)           (((uint32_t)(x) & 0x3ff) << 20)
#define FDCBT_FRJW(x)               (((uint32_t)(x) & 0x07) << 16)
#define FDCBT_FPROPSEG(x)           (((uint32_t)(x) & 0x1f) << 10)
#define FDCBT_FPSEG1(x)             (((uint32_t)(x) & 0x07) << 5)
#define FDCBT_FPSEG2(x)             ((uint32_t)(x) & 0x07)

/* Interrupt Mask Register */
#define IMASK_BUFnM(x)              (0x1u << (x))
#define IMASK_BUFF_ENABLE_ALL       (0xFFFFFFFFu)
#define IMASK_BUFF_DISABLE_ALL      (0x00000000)

/* Interrupt Flag Register */
#define IFLAG_BUFF_SET_ALL          (0xFFFFFFFFu)
#define IFLAG_BUFF_CLEAR_ALL        (0x00000000)
#define IFLAG_BUFnM(x)              (0x1u << (x))

/* Bit definitions for FlexCAN Rx Global Mask (CANRXGMASK) Register */
#define RXGMASK_MASK                0xFFFFFFFFu
#define FLEXCAN_LAM_MASK            0xFFFFFFFFu

/* Message Buffers */
#define MB_CNT_EDL                  (0x1u << 31)
#define MB_CNT_BRS                  (0x1u << 30)
#define MB_CNT_ESI                  (0x1u << 29)
#define MB_CNT_CODE(x)              (((x) & 0x0F) << 24)
#define MB_CNT_SRR                  (0x1u << 22)
#define MB_CNT_IDE                  (0x1u << 21)
#define MB_CNT_RTR                  (0x1u << 20)
#define MB_CNT_LENGTH(x)            (((x) & 0x0F) << 16)
#define MB_CNT_TIMESTAMP(x)         ((x) & 0xFFFF)

/* Definitions for FlexCAN Message Control Field (32bit)*/
/** Message Buffer Code **/
#define MSG_BUF_CODE_SHIFT          24
#define MSG_BUF_CODE_MASK           (0xFu << MSG_BUF_CODE_SHIFT)
/*** Receive message object codes ***/
#define REC_CODE_NOT_ACTIVE         0x0     /* Message buffer is not active */
#define REC_CODE_BUSY               0x1     /* Message buffer is busy, not access */
#define REC_CODE_FULL               0x2     /* Message buffer is full */
#define REC_CODE_EMPTY              0x4     /* Message buffer is active and empty */
#define REC_CODE_OVERRUN            0x6     /* Message buffer is overrun */
#define REC_CODE_RANSWER            0xA     /* Message buffer is now being filled with a new receive frame.
                                            This condition will be cleared within 20 cycles.*/
/*** Transmit message object codes ***/
#define TRANS_CODE_NOT_READY        0x8     /* Message buffer not ready for transmit */
#define TRANS_CODE_ABORT            0x9     /* Message buffer is Tx and ARM aborted the transmission */
#define TRANS_CODE_TRANSMIT_ONCE    0xC     /* Data frame to be transmitted once, unconditionally */
#define TRANS_CODE_TRANSMIT_RTR_ONCE    0xC /* Remote frame to be transmitted once, and message buffer becomes
                                               an RX message buffer for data frames; RTR bit must be set */
#define TRANS_CODE_TRANSMIT_ONLY_RTR    0xA /* Data frame to be transmitted only as a response to a remote frame*/
#define TRANS_CODE_TRANSMIT_ONCE_RTR_ALWAYS 0xE /* Data frame to be transmitted once, unconditionally and then only as a response to remote frame always */
/** Data Length Code **/
#define MSG_BUF_DLC_MASK            0x000F0000u
#define MSG_BUF_DLC_SHIFT           16
#define MCF_RTR                     (0x1u << 20)    /* RTR */
#define MCF_IDE                     (0x1u << 21)    /* IDE */

/* Definitions for FlexCAN Message ID Field (32bit) */
#define MCF_TPL_SHIFT               29      /* PRIO */
#define MCF_TPL_MAXVAL              0x7u
#define MCF_TPL_MASK                (MCF_TPL_MAXVAL << MCF_TPL_SHIFT)
/* Definitions for FlexCAN Message Identifier */
#define MID_MASK_STD                0x1FFC0000u
#define MID_MASK_EXT                0x1FFFFFFFu


/* Device initialization flags */
#define FLEXCAN_FLAGS_LOOPBACK      (0x1u << 0)     /* Enable self-test/loopback mode */
#define FLEXCAN_FLAGS_EXTENDED_MID  (0x1u << 1)     /* Enable 29 bit extended message ID */
#define FLEXCAN_FLAGS_AUTOBUS       (0x1u << 2)     /* Disable auto bus on */
#define FLEXCAN_FLAGS_TIMESTAMP     (0x1u << 3)     /* Set initial local network time */
#define FLEXCAN_FLAGS_RX_FULL_MSG   (0x1u << 4)     /* Receiver should store message ID, timestamp, etc. */
#define FLEXCAN_FLAGS_BITRATE_SAM   (0x1u << 5)     /* Enable Bitrate Triple Sample Mode */
#define FLEXCAN_FLAGS_MDRIVER_INIT  (0x1u << 6)     /* Initialize from mini-driver (if it exists and is running) */
#define FLEXCAN_FLAGS_MDRIVER_SORT  (0x1u << 7)     /* Sort buffered mdriver message based on MID */
#define FLEXCAN_FLAGS_CLKSRC        (0x1u << 8)     /* External reference clock source */
#define FLEXCAN_FLAGS_TSYN          (0x1u << 9)     /* Enable Timer Sync feature */
#define FLEXCAN_FLAGS_LOM           (0x1u << 10)    /* Listen Only Mode */
#define FLEXCAN_FLAGS_LBUF          (0x1u << 11)    /* Lowest number buffer is transmitted first */
#define FLEXCAN_FLAGS_BRS           (0x1u << 12)    /* bit rate switch (second bitrate for payload data) */
#define FLEXCAN_FLAGS_TDCEN         (0x1u << 14)    /* Enable TDC for CAN-FD */
#define FLEXCAN_FLAGS_CANFD         (0x1u << 15)    /* Enable CAN-FD */
#define FLEXCAN_FLAGS_MTIMING       (0x1u << 16)    /* Manual timing, need validation */
#define FLEXCAN_FLAGS_MDTIMING      (0x1u << 17)    /* Manual data timing for CANFD, need validation */
#define FLEXCAN_FLAGS_ENDIAN_SWAP   (0x1u << 18)    /* Need data endian swap */
#define FLEXCAN_FLAGS_DUALMB        (0x1u << 19)    /* Dual message buffer */
#define FLEXCAN_FLAGS_ISO           (0x1u << 20)    /* CAN FD ISO */
#define FLEXCAN_FLAGS_IOPRIV        (0x1u << 21)    /* Need IO privity */
#define FLEXCAN_FLAGS_DISABLE_MECR  (0x1u << 22)    /* Disable MECR */

/* Memory Control Error Status Register */
#define FLEXCAN_ERRSR               0x1C

/* Bit definitions for Error Status Register */
#define FLEXCAN_ERRSR_CEIOF         (1 << 0)        /* Correctable Error Interrupt Overrun Flag */
#define FLEXCAN_ERRSR_FANCEIOF      (1 << 2)        /* FlexCAN Access With Non-Correctable Error Interrupt Overrun Flag */
#define FLEXCAN_ERRSR_HANCEIOF      (1 << 3)        /* Host Access With Non-Correctable Error Interrupt Overrun Flag */
#define FLEXCAN_ERRSR_CEIF          (1 << 16)       /* Correctable Error Interrupt Flag */
#define FLEXCAN_ERRSR_FANCEIF       (1 << 18)       /* FlexCAN Access With Non-Correctable Error Interrupt Flag */
#define FLEXCAN_ERRSR_HANCEIF       (1 << 19)       /* Host Access With Non-Correctable Error Interrupt Flag */

#ifndef NELEMENTS
#define NELEMENTS(x)  ((int)(sizeof((x)) / sizeof((x)[0])))
#endif    /* NELEMENTS */

typedef struct {
    paddr_t     regbase;
    int         numirq;
#define FLEXCAN_MAX_IRQ 3
    int         irqvector[FLEXCAN_MAX_IRQ];
} flexcan_hwinfo_t;

/* CAN payload length and DLC definitions according to ISO 11898-1 */
#define CAN_MAX_DLC     8
#define CAN_MAX_DLEN    8
/* CAN Message Object Data Structure */
typedef struct can_msg_obj
{
    volatile uint32_t    canmcf;         /* Message Control Field :32 bit CAN_ID + EFF/RTR/ERR flags */
    volatile uint32_t    canmid;         /* Message Identifier */
    uint32_t    canmdl;         /* Message Data Low Word */
    uint32_t    canmdh;         /* Message Data High Word */
} can_msg_obj_t;

/* CAN FD payload length and DLC definitions according to ISO 11898-7 */
#define CANFD_MAX_DLC   15
#define CANFD_MAX_DLEN  64
/* CANFD Message Object Data Structure */
typedef struct canfd_msg_obj
{
    volatile uint32_t    canmcf;         /* Message Control Field: 32 bit CAN_ID + EFF/RTR/ERR flags */
    volatile uint32_t    canmid;         /* Message Identifier */
    uint8_t     canmd[CANFD_MAX_DLEN];    /* Message Data */
} canfd_msg_obj_t;


/* Initialization and Options Structure */
typedef struct
{
    CANDEV_INIT         cinit;      /* Generic CAN Device Init Params */
    paddr_t             port;       /* Device Physical Register Address */
    struct flexcan_btm  bt;         /* Bitrate related parameters */
    struct flexcan_btm  dbt;        /* FD data bitrate related parameters */
    int                 irqsys[FLEXCAN_MAX_IRQ];    /* Device Message System Vector */
    int                 numirqs;
    uint32_t            flags;      /* Initialization flags */
    uint32_t            numtx;      /* Number of TX Mailboxes */
    uint32_t            numrx;      /* Number of RX Mailboxes */
    uint32_t            midrx;      /* RX Message ID */
    uint32_t            midtx;      /* TX Message ID */
    uint32_t            timestamp;  /* Initial value for local network time */
    uint8_t             verbosity;  /* Verbosity */
    uint8_t             tstampcap;  /* CAN hardware timestamp capture point */
    uint8_t             tdcoff;     /* TDC offset */
} flexcan_init_t;

typedef struct
{
    char                description[64];/* CAN device description */
    uint32_t            msgq_size;  /* Number of messages */
    uint32_t            waitq_size; /* Length of CAN message data */
    struct flexcan_btm  bt;         /* Bitrate related parameters */
    struct flexcan_btm  dbt;        /* FD Bitrate related parameters */
} flexcan_init_info_t;

struct flexcan_entry;
/* General Device Information Structure */
typedef struct
{
    uintptr_t           base;       /* Device Virtual Register Mapping */
    uintptr_t           base_fd;    /* Device FD Virtual Register Mapping */
    uintptr_t           canlam;     /* Array of CAN message object local area masks */
    can_msg_obj_t      *canmsg;     /* Array of CAN message objects */
    struct flexcan_entry *devlist;  /* Array of all device mailboxes */
    flexcan_init_info_t initinfo;   /* Initialization info */
    struct can_devctl_stats     stats;
    CANDEV_MODE         mode;       /* Driver mode:  raw frames or I/O */
    uint32_t            timer;      /* CAN message free running timestamp */
    uint32_t            canestat;   /* Keep track of CANESR register status for devctl */
    int                 numirq;
    int                 irqsys[FLEXCAN_MAX_IRQ];    /* IRQs */
    int                 iidsys[FLEXCAN_MAX_IRQ];    /* Return iid from InterruptAttachEvent */
    uint32_t            numtx;      /* Number of TX Mailboxes */
    uint32_t            numrx;      /* Number of RX Mailboxes */
    uint32_t            flags;
    uint32_t            num_mailboxes;
} flexcan_info_t;

/* Device specific extension of CANDEV struct */
typedef struct flexcan_entry
{
    CANDEV          cdev;       /* CAN Device - MUST be first entry */
    flexcan_info_t  *devinfo;   /* Common device information */
    uint32_t        mbxid;      /* Index into mailbox memory */
#define CANDEV_TX_ENABLED           0x00000001u
    uint32_t        dflags;     /* Device specific flags */
} flexcan_t;

/* Driver implemented CAN library function prototypes */
void flexcan_transmit(CANDEV *cdev);
int  flexcan_devctl(CANDEV* const cdev, io_devctl_t *msg);
CANDEV* flexcan_event_handler(void* const hdl, const int pulse_code);

/* Function prototypes */
int  flexcan_init_hw(flexcan_info_t* const devinfo, const flexcan_init_t* const devinit);
int  flexcan_init_intr(flexcan_info_t* const devinfo, const flexcan_init_t* const devinit);

#endif  // _FLEXCAN_H_INCLUDED

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/can/flexcan/flexcan.h $ $Rev: 985670 $")
#endif
