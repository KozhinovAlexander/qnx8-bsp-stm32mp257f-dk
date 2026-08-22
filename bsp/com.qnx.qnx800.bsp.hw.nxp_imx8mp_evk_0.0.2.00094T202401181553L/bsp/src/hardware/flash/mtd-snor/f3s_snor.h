/*
 * Copyright (c) 2022, BlackBerry Limited.
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


/*
 * This file contains definitions for SPI serial NOR flash driver callouts.
 */

#ifndef __F3S_SNOR_H_INCLUDED
#define __F3S_SNOR_H_INCLUDED

#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <hw/inout.h>
#include <sys/neutrino.h>
#include <libc.h>
#include <fs/f3s_api.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>

/* Flash commands */
#define SNOR_CMD_WRSR           0x01u   // Write status register
#define SNOR_CMD_PP             0x02u   // Page program
#define SNOR_CMD_READ           0x03u   // Read data
#define SNOR_CMD_WRDI           0x04u   // Write disable
#define SNOR_CMD_RDSR           0x05u   // Read status register
#define SNOR_CMD_WREN           0x06u   // Write enable
#define SNOR_CMD_READ_FAST      0x0Bu   // Read data fast
#define SNOR_CMD_BE_4K          0x20u   // Block(4KB) erase
#define SNOR_CMD_PP_114         0x32u   // Page program 1-1-4
#define SNOR_CMD_RDCR           0x35u   // Read configuration register
#define SNOR_CMD_PP_144         0x38u   // Page program 1-4-4
#define SNOR_CMD_READ_112       0x3Bu   // Read data 1-1-2
#define SNOR_CMD_WRSR2          0x3Eu   // Write status register 2
#define SNOR_CMD_RDSR2          0x3Fu   // Read status register 2
#define SNOR_CMD_CLFSR          0x50u   // Clear flag status register
#define SNOR_CMD_BE_32K         0x52u   // Block(32KB) erase
#define SNOR_CMD_RDSFDP         0x5Au   // Read SFDP
#define SNOR_CMD_RDAR           0x65u   // Read any register(Cypress specific)
#define SNOR_CMD_SRSTEN         0x66u   // Software reset enable
#define SNOR_CMD_READ_114       0x6Bu   // Read data 1-1-4
#define SNOR_CMD_RDFSR          0x70u   // Read flag status register
#define SNOR_CMD_WRAR           0x71u   // Write any register(Cypress specific)
#define SNOR_CMD_PP_118         0x82u   // Page program 1-1-8
#define SNOR_CMD_READ_118       0x8Bu   // Read data 1-1-8
#define SNOR_CMD_GBU            0x98u   // Global block Unlock
#define SNOR_CMD_SRST           0x99u   // Software reset
#define SNOR_CMD_RDID           0x9Fu   // Read JEDEC ID
#define SNOR_CMD_PP_112         0xA2u   // Page program 1-1-2
#define SNOR_CMD_WRNVCR         0xB1u   // Write 16 bit nonvolatile configuration register
#define SNOR_CMD_RDNVCR         0xB5u   // Read 16 bit nonvolatile configuration register
#define SNOR_CMD_ETR_4B         0xB7u   // Enter 4-byte address mode
#define SNOR_CMD_READ_122       0xBBu   // Read data 1-2-2
#define SNOR_CMD_PP_188         0xC2u   // Page program 1-8-8
#define SNOR_CMD_WREAR          0xC5u   // Write extended address register
#define SNOR_CMD_CE             0xC7u   // Chip erase
#define SNOR_CMD_RDEAR          0xC8u   // Read extended address register
#define SNOR_CMD_READ_188       0xCBu   // Read data 1-8-8
#define SNOR_CMD_PP_122         0xD2u   // Page program 1-2-2
#define SNOR_CMD_SE             0xD8u   // Sector erase
#define SNOR_CMD_4DYBRD         0xE0u   // Read dynamic lock bits, 4B address
#define SNOR_CMD_4DYBWR         0xE1u   // Write dynamic lock bits, 4B address
#define SNOR_CMD_4PPBRD         0xE2u   // Read persistent protection lock bits, 4B address
#define SNOR_CMD_4PPBP          0xE3u   // Write persistent protection lock bits, 4B address
#define SNOR_CMD_PPBE           0xE4u   // Persistent protection block erase
#define SNOR_CMD_READ_144       0xEBu   // Read data 1-4-4
#define SNOR_CMD_EXT_4B         0xE9u   // Exit 4-byte address mode

/* 4-byte address commands */
#define SNOR_CMD_READ_4B        0x13u   // Read data
#define SNOR_CMD_READ_FAST_4B   0x0Cu   // Read data fast
#define SNOR_CMD_READ_112_4B    0x3Cu   // Read data 1-1-2
#define SNOR_CMD_READ_122_4B    0xBCu   // Read data 1-2-2
#define SNOR_CMD_READ_114_4B    0x6Cu   // Read data 1-1-4
#define SNOR_CMD_READ_144_4B    0xECu   // Read data 1-4-4
#define SNOR_CMD_READ_118_4B    0x7Cu   // Read data 1-1-8
#define SNOR_CMD_READ_188_4B    0xCCu   // Read data 1-8-8
#define SNOR_CMD_PP_4B          0x12u   // Page program
#define SNOR_CMD_PP_114_4B      0x34u   // page program 1-1-4
#define SNOR_CMD_PP_144_4B      0x3Eu   // Page program 1-4-4
#define SNOR_CMD_PP_118_4B      0x84u   // Page program 1-1-8
#define SNOR_CMD_PP_188_4B      0x8Eu   // Page program 1-8-8
#define SNOR_CMD_BE_4K_4B       0x21u   // Block(4KB) erase
#define SNOR_CMD_BE_32K_4B      0x5Cu   // Block(32KB) erase
#define SNOR_CMD_SE_4B          0xDCu   // Sector erase

/* DTR commands */
#define SNOR_CMD_READ_111_DTR   0x0Du
#define SNOR_CMD_READ_112_DTR   0x3Du
#define SNOR_CMD_READ_122_DTR   0xBDu
#define SNOR_CMD_READ_114_DTR   0x6Du
#define SNOR_CMD_READ_144_DTR   0xEDu
#define SNOR_CMD_READ_118_DTR   0x9Du
#define SNOR_CMD_READ_188_DTR   0xFDu

#define SNOR_CMD_READ_111_DTR_4B    0x0Eu
#define SNOR_CMD_READ_122_DTR_4B    0xBEu
#define SNOR_CMD_READ_144_DTR_4B    0xEEu

/* Status register bits */
#define SNOR_STS_WEL            (1u << 1)   // write enable latch
#define SNOR_STS_WIP            (1u << 0)   // write in progress

/* Flag status register bits */
#define SNOR_FLGSTS_RDY         (1u << 7)   // ready
#define SNOR_FLGSTS_ESUS        (1u << 6)   // erase suspend
#define SNOR_FLGSTS_EFAIL       (1u << 5)   // erase failure
#define SNOR_FLGSTS_PFAIL       (1u << 4)   // program failure
#define SNOR_FLGSTS_PSUS        (1u << 2)   // program suspend
#define SNOR_FLGSTS_EPROCT      (1u << 1)   // protection error
#define SNOR_FLGSTS_ADDR4B      (1u << 0)   // 4 bytes address mode enabled

/*
 * Definition for f3s_snor
 */
#define SNOR_SZPOW2_MIN         12          // minimum sector size, 4KB
#define SNOR_SZPOW2_LOCK        16          // lock sector size, 64KB
#define SNOR_SZPOW2_MAX         20          // maximum sector size, 1MB

/* Bus protocol */
#define SNOR_BUSPROTO_CBSHIFT   8
#define SNOR_BUSPROTO_ABSHIFT   4
#define SNOR_BUSPROTO_DBSHIFT   0
#define SNOR_BUSPROTO(c, a, d)  (((c) << 8) | ((a) << 4) | (d))
#define SNOR_BUSPROTO_NONE      0
#define SNOR_BUSPROTO_BUS_MASK  (SNOR_BUSPROTO(0xF, 0xF, 0xF))
#define SNOR_BUSPROTO_1_1_1     (SNOR_BUSPROTO(1, 1, 1))
#define SNOR_BUSPROTO_1_1_2     (SNOR_BUSPROTO(1, 1, 2))
#define SNOR_BUSPROTO_1_2_2     (SNOR_BUSPROTO(1, 2, 2))
#define SNOR_BUSPROTO_2_2_2     (SNOR_BUSPROTO(2, 2, 2))
#define SNOR_BUSPROTO_1_1_4     (SNOR_BUSPROTO(1, 1, 4))
#define SNOR_BUSPROTO_1_4_4     (SNOR_BUSPROTO(1, 4, 4))
#define SNOR_BUSPROTO_4_4_4     (SNOR_BUSPROTO(4, 4, 4))
#define SNOR_BUSPROTO_1_1_8     (SNOR_BUSPROTO(1, 1, 8))
#define SNOR_BUSPROTO_1_8_8     (SNOR_BUSPROTO(1, 8, 8))
#define SNOR_BUSPROTO_8_8_8     (SNOR_BUSPROTO(8, 8, 8))
/* DTR commands, no need to enable device DTR, command byte is shifted in single mode */
#define SNOR_BUSPROTO_DTR_CMD   (1u << 12)
/* DTR mode, need to enable device DTR, command byte is shifted out in DTR mode */
#define SNOR_BUSPROTO_DTR_MODE  (1u << 13)
#define SNOR_BUSPROTO_HPF_MODE  (1u << 14)
#define SNOR_BUSPROTO_1_1_1_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_1_1)
#define SNOR_BUSPROTO_1_1_2_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_1_2)
#define SNOR_BUSPROTO_1_2_2_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_2_2)
#define SNOR_BUSPROTO_2_2_2_DTR (SNOR_BUSPROTO_DTR_MODE | SNOR_BUSPROTO_2_2_2)
#define SNOR_BUSPROTO_1_1_4_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_1_4)
#define SNOR_BUSPROTO_1_4_4_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_4_4)
#define SNOR_BUSPROTO_4_4_4_DTR (SNOR_BUSPROTO_DTR_MODE | SNOR_BUSPROTO_4_4_4)
#define SNOR_BUSPROTO_1_1_8_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_1_8)
#define SNOR_BUSPROTO_1_8_8_DTR (SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_1_8_8)
#define SNOR_BUSPROTO_8_8_8_DTR (SNOR_BUSPROTO_DTR_MODE | SNOR_BUSPROTO_8_8_8)
#define SNOR_BUSPROTO_HYPER     (SNOR_BUSPROTO_HPF_MODE | SNOR_BUSPROTO_8_8_8)
#define SNOR_BUSPROTO_MASK      (SNOR_BUSPROTO_BUS_MASK | SNOR_BUSPROTO_DTR_CMD | SNOR_BUSPROTO_DTR_MODE | SNOR_BUSPROTO_HPF_MODE)

/* DQS supported */
#define SNOR_BUSPROTO_DQS       (1u << 17)

/* Extract command/address/data phase bus width from bus protocol */
#define SNOR_BUS_PROTO_TO_WIDTH(p, c, a, d)     { (c) = ((p) >> 8) & 0x0F, (a) = ((p) >> 4) & 0x0F, (d) = (p) & 0x0F; }

/*
 * Flag, offset is CS
 * flash file system will never set all flag bits
 */
#define SNOR_FLG_FORCECS        0xFFFFFFFF

/* Bus protocol index */
enum snor_busproto_idx {
    SNOR_BPI_1_1_1 = 0,
    SNOR_BPI_1_1_2,
    SNOR_BPI_1_2_2,
    SNOR_BPI_2_2_2,
    SNOR_BPI_1_1_4,
    SNOR_BPI_1_4_4,
    SNOR_BPI_4_4_4,
    SNOR_BPI_1_1_8,
    SNOR_BPI_1_8_8,
    SNOR_BPI_8_8_8,
    SNOR_BPI_1_1_1_DTR,
    SNOR_BPI_1_1_2_DTR,
    SNOR_BPI_1_2_2_DTR,
    SNOR_BPI_1_1_4_DTR,
    SNOR_BPI_1_4_4_DTR,
    SNOR_BPI_4_4_4_DTR,
    SNOR_BPI_1_1_8_DTR,
    SNOR_BPI_1_8_8_DTR,
    SNOR_BPI_MAX
};

typedef struct _snor_cfg_t {
    uint32_t    clk;        // bus clock
    uint32_t    bus_proto;  // bus protocol
    uint8_t     cs;         // chip select the device connected to
    uint8_t     ncs;        // number of chip for stripe mode
    uint16_t    stripe;     // stripe mode CS bit field
} snor_cfg_t;

typedef struct _snor_op_t {
    uint8_t     opcode;     // op code
    uint8_t     dcycle;     // dummy cycles
    uint8_t     adrlen;     // address length
    uint8_t     reserved;
} snor_op_t;

typedef struct _snor_cmd_t {
    snor_op_t   *op;
    snor_cfg_t  *cfg;
    uint32_t    addr;       // address
} snor_cmd_t;

/* Block erase */
typedef struct _snor_blkers {
    uint8_t     opcode;     // OP code
    uint8_t     opcode_4b;  // 4B address OP code
    uint8_t     blksz_pow2; // power of two
    uint16_t    tet;        // typical erase time in ms
    uint16_t    met;        // maximum erase time in ms
} snor_blkers;

/*
 * Hardware Capability, shared between controller and flash device
 */
/* Memory read */
#define SNOR_HCAPS_RD_MASK          0x0FFFFFul
#define SNOR_HCAPS_RD_1_1_1         (1ul << 0)
#define SNOR_HCAPS_RD_1_1_1_FAST    (1ul << 1)      // controller doesn't care, but device might
#define SNOR_HCAPS_RD_1_1_1_DTR     (1ul << 2)

#define SNOR_HCAPS_RD_DUAL          (0x1Ful << 3)
#define SNOR_HCAPS_RD_1_1_2         (1ul << 3)
#define SNOR_HCAPS_RD_1_2_2         (1ul << 4)
#define SNOR_HCAPS_RD_2_2_2         (1ul << 5)
#define SNOR_HCAPS_RD_1_1_2_DTR     (1ul << 6)
#define SNOR_HCAPS_RD_1_2_2_DTR     (1ul << 7)

#define SNOR_HCAPS_RD_QUAD          (0x1Ful << 8)
#define SNOR_HCAPS_RD_1_1_4         (1ul << 8)
#define SNOR_HCAPS_RD_1_4_4         (1ul << 9)
#define SNOR_HCAPS_RD_4_4_4         (1ul << 10)
#define SNOR_HCAPS_RD_1_1_4_DTR     (1ul << 11)
#define SNOR_HCAPS_RD_1_4_4_DTR     (1ul << 12)

#define SNOR_HCAPS_RD_OCTAL         (0x1Ful << 13)
#define SNOR_HCAPS_RD_1_1_8         (1ul << 13)
#define SNOR_HCAPS_RD_1_8_8         (1ul << 14)
#define SNOR_HCAPS_RD_8_8_8         (1ul << 15)
#define SNOR_HCAPS_RD_1_1_8_DTR     (1ul << 16)
#define SNOR_HCAPS_RD_1_8_8_DTR     (1ul << 17)

/* Page program */
#define SNOR_HCAPS_PP_MASK          (0x7FFul << 18)
#define SNOR_HCAPS_PP_1_1_1         (1ul << 18)

#define SNOR_HCAPS_PP_DUAL          (7ul << 19)
#define SNOR_HCAPS_PP_1_1_2         (1ul << 20)
#define SNOR_HCAPS_PP_1_2_2         (1ul << 21)
#define SNOR_HCAPS_PP_2_2_2         (1ul << 22)

#define SNOR_HCAPS_PP_QUAD          (7ul << 23)
#define SNOR_HCAPS_PP_1_1_4         (1ul << 23)
#define SNOR_HCAPS_PP_1_4_4         (1ul << 24)
#define SNOR_HCAPS_PP_4_4_4         (1ul << 25)

#define SNOR_HCAPS_PP_OCTAL         (7ul << 26)
#define SNOR_HCAPS_PP_1_1_8         (1ul << 26)
#define SNOR_HCAPS_PP_1_8_8         (1ul << 27)
#define SNOR_HCAPS_PP_8_8_8         (1ul << 28)

#define SNOR_HCAPS_DTR_CMD          (1ul << 29)     // DTR command supported
#define SNOR_HCAPS_DTR              (1ul << 30)     // DTR mode supported
#define SNOR_HCAPS_DQS              (1ul << 31)     // DQS supported
#define SNOR_HCAPS_HYPER            (1ul << 32)     // Hyper flash supported

typedef struct _snor_chip_t {
    void        *ctrl;      // pointer to SPI controller
    uint64_t    hcaps;      // flash hardware capability
    uint64_t    hcmask;     // flash hardware capability mask
    uint32_t    dcaps;      // flash device capability
    uint32_t    dcmask;     // flash device capability mask
#define SNOR_DCAPS_PSR          (1u << 31)  // Program suspend resume supported
#define SNOR_DCAPS_ESR          (1u << 30)  // Erase suspend resume supported

#define SNOR_DCAPS_ADDR_4B      (1u << 29)  // 4 bytes address command supported
#define SNOR_DCAPS_ADDR_3B_4B   (1u << 28)  // default to 3 bytes address, 4 bytes address supported

#define SNOR_DCAPS_FSTATUS      (1u << 27)  // use flag status register for status polling

// match SFDP DWORD #17
#define SNOR_DCAPS_4BPP_188     (1u << 24)  // 4B address page program(188) supported(0x8E)
#define SNOR_DCAPS_4BPP_118     (1u << 23)  // 4B address page program(118) supported(0x84)
#define SNOR_DCAPS_4BRD_188DTR  (1u << 22)  // 4B address read(188DTR) supported(0xFD)
#define SNOR_DCAPS_4BRD_188     (1u << 21)  // 4B address read(188) supported(0xCC)
#define SNOR_DCAPS_4BRD_118     (1u << 20)  // 4B address read(118) supported(0x7C)
#define SNOR_DCAPS_4BRD_144DTR  (1u << 15)  // 4B address read(144DTR) supported(0xEE)
#define SNOR_DCAPS_4BRD_122DTR  (1u << 14)  // 4B address read(122DTR) supported(0xBE)
#define SNOR_DCAPS_4BRD_111DTR  (1u << 13)  // 4B address read(111DTR) supported(0x0E)
#define SNOR_DCAPS_4BPP_144     (1u <<  8)  // 4B address page program(144) supported(0x3E)
#define SNOR_DCAPS_4BPP_114     (1u <<  7)  // 4B address page program(114) supported(0x34)
#define SNOR_DCAPS_4BPP_111     (1u <<  6)  // 4B address page program(111) supported(0x12)
#define SNOR_DCAPS_4BRD_144     (1u <<  5)  // 4B address read(144) supported(0xEC)
#define SNOR_DCAPS_4BRD_114     (1u <<  4)  // 4B address read(114) supported(0x6C)
#define SNOR_DCAPS_4BRD_122     (1u <<  3)  // 4B address read(122) supported(0xBC)
#define SNOR_DCAPS_4BRD_112     (1u <<  2)  // 4B address read(112) supported(0x3C)
#define SNOR_DCAPS_4BRD_111F    (1u <<  1)  // 4B address read(111FAST) supported(0x0C)
#define SNOR_DCAPS_4BRD_111     (1u <<  0)  // 4B address read(111) supported(0x13)

    uint32_t    flags;
#define SNOR_CFLG_PRESENT       (1u << 31)  // flash present
#define SNOR_CFLG_4B_ADDR       (1u << 30)  // currently in 4 bytes address mode
#define SNOR_CFLG_DUAL          (1u << 29)  // currently in dual mode
#define SNOR_CFLG_QUAD          (1u << 28)  // currently in quad mode
#define SNOR_CFLG_OCTAL         (1u << 27)  // currently in octal mode
#define SNOR_CFLG_HYPER         (1u << 26)  // hyper flash

// more capabilities
#define SNOR_CFLG_DLTBS         (1u << 11)  // top boot sector, for dynamic lock
#define SNOR_CFLG_DLBBS         (1u << 10)  // bottom boot sector, for persistent lock
#define SNOR_CFLG_PSLOCK        (1u <<  9)  // support persistent lock
#define SNOR_CFLG_DNLOCK        (1u <<  8)  // support dynamic lock
#define SNOR_CFLG_DOP           (1u <<  7)  // need double OP code, set at chip set_protocol

    snor_cfg_t  rdcfg;      // bus configuration for memory read operation
    snor_cfg_t  wrcfg;      // bus configuration for page program operation
    snor_cfg_t  cfg;        // bus configuration for none memory access operation
    snor_op_t   rdops[SNOR_BPI_MAX];
    snor_op_t   ppops[SNOR_BPI_MAX];
    snor_op_t   op_rd;
    snor_op_t   op_wr;
    uint8_t     op_ps;      // program suspend
    uint8_t     op_pr;      // program resume
    uint8_t     op_es;      // erase suspend
    uint8_t     op_er;      // erase resume
    uint8_t     qer;        // enter quad mode requirement
    uint8_t     qes;        // quad mode enable sequence
    uint8_t     qds;        // quad mode disable sequence
    uint8_t     e4ba;       // enter 4-byte addressing
    uint16_t    x4ba;       // exit 4-byte addressing
    uint8_t     ssrs;       // software reset and rescue sequence
    uint8_t     asr1;       // access status register 1
    uint8_t     asr2;       // access status register 2
    uint8_t     sr2_esbit;  // erase suspend bit
    uint8_t     sr2_psbit;  // program suspend bit
    uint8_t     op_eor;     // enter octal mode requirement

    uint8_t     op_ppbe;    // persistent protection block erase
    uint8_t     lksz_pow2;  // protect sector size
    uint8_t     lock_v;     // lock value
    uint8_t     lock_m;     // lock mask
    snor_op_t   op_rdlock;  // read dynamic lock bits
    snor_op_t   op_dlock;   // dynamic lock
    snor_op_t   op_rplock;  // read persistent lock bits
    snor_op_t   op_plock;   // persistent lock

    uint8_t     vid;
    uint8_t     did;
    uint8_t     drv_type;   // output driver strength

    uint32_t    e2sl;       // erase to suspend latency, in ns
    uint32_t    er2si;      // erase resume to suspend interval, in us
    uint32_t    p2sl;       // program to suspend latency, in ns
    uint32_t    pr2si;      // program resume to suspend interval, in us
#define SNOR_MAX_ERSCFG     4
    snor_blkers blkers[SNOR_MAX_ERSCFG];    // block erase
    uint32_t    chipsz;     // chip size
    uint32_t    sectsz;     // sector size
    uint16_t    pagesz;     // page size
    uint16_t    addrsz;     // address length
    uint16_t    pptt;       // page program typical time
    uint16_t    ppmt;       // page program maximum time
    uint16_t    align;      // access alignment requirement

    uint32_t    offset;     // offset within chip, filled by f3s_snor_page()

    f3s_flash_v2_t *flash;

    int         (*set_protocol)(struct _snor_chip_t* const chip, const uint32_t bus_proto);
    int         (*enter_quad)(struct _snor_chip_t* const chip);
    int         (*enter_4b_address)(struct _snor_chip_t* const chip);
    int         (*post_ident)(struct _snor_chip_t* const chip);
} snor_chip_t;

typedef struct _snor_ctrl_t snor_ctrl_t;
typedef struct _snor_func_t snor_func_t;

/* Host specific SPI flash calls */
struct _snor_func_t
{
    /**
     *  @brief             Host controller cleanup function.
     *  @param  hdl        Host controller handler.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*dinit)(void *const hdl);
    /**
     *  @brief             Host controller configure bus function.
     *  @param snor        SNOR driver handle.
     *  @param cfg         Serial NOR flash configure structure.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*cfg_bus)(snor_ctrl_t *const snor, snor_cfg_t *const cfg);
    /**
     *  @brief             Host controller post ident function.
     *  @param snor        SNOR driver handle.
     *  @param cs          Chip select.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*post_ident)(snor_ctrl_t *snor, const int cs);
    /**
     *  @brief             Destripe function.
     *  @param snor        SNOR driver handle.
     *  @param buf         Buffer.
     *  @param cnt         Counter.
     *  @param construct   construct.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*dstripe)(snor_ctrl_t *snor, uint8_t *buf, int cnt, int construct);
    /**
     *  @brief             SNOR read register function.
     *  @param snor        SNOR driver handle.
     *  @param cmd         Pointer to command structure.
     *  @param regs        Read register data buffer.
     *  @param len         Read register data length.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*read_reg)(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const regs, const uint32_t len);
    /**
     *  @brief             SNOR write register function.
     *  @param snor        SNOR driver handle.
     *  @param cmd         Pointer to command structure.
     *  @param regs        Write register data buffer.
     *  @param len         Write register data length.
     *
     *  @return            EOK --success otherwise fail.
     */
    int         (*write_reg)(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const regs, const uint32_t len);
    /**
     *  @brief             SNOR read function.
     *  @param snor        SNOR driver handle.
     *  @param cmd         Pointer to command structure.
     *  @param buf         Read data buffer.
     *  @param len         Read data length.
     *
     *  @return            Data length --success
     *                     -1 --fail with errno set.
     */
    int         (*read)(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const buf, const uint32_t len);
    /**
     *  @brief             SNOR write function.
     *  @param snor        SNOR driver handle.
     *  @param cmd         Pointer to command structure.
     *  @param buf         Write data buffer.
     *  @param len         Write data length.
     *
     *  @return            Data length --success
     *                     -1 --fail with errno set.
     */
    int         (*write)(snor_ctrl_t *const snor, const snor_cmd_t *const cmd, uint8_t *const buf, const uint32_t len);
};

struct _snor_ctrl_t
{
    snor_func_t funcs;

    uint32_t    flags;
#define SNOR_FLG_STRIPE     (1u << 0)   // stripe mode
#define SNOR_FLG_HYPER      (1u << 1)   // hyper flash
    int32_t     verbosity;
    char        *soc_opts;

    uint64_t    hcaps;
    uint64_t    ccaps;
#define SNOR_CCAPS_PPAWREN          (1ul << 0)  // auto WREN for page program
#define SNOR_CCAPS_PPASP            (1ul << 1)  // auto status polling for page program

    int         clkmax;

    uint8_t     ncs;        // number of chip select
    uint8_t     ccs;        // current chip select?
#define SNOR_MAX_CS     8
    snor_chip_t chip[SNOR_MAX_CS];  // flash chip array
};

#define SNOR_SET_CMD(cmd, o, c, a)  { (cmd).op = (o), (cmd).cfg = (c), (cmd).addr = (a); }

// Generic
extern int32_t f3s_snor_read(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset, int32_t size, uint8_t *buffer);
extern int32_t f3s_snor_program(f3s_dbase_t *dbase, f3s_access_t *access,
                    uint32_t flags, uint32_t offset, int32_t size, uint8_t *buffer);
extern uint8_t *f3s_snor_page(f3s_socket_t *socket, uint32_t flags, uint32_t offset, int32_t *size);
extern int32_t f3s_snor_status(f3s_socket_t *socket, uint32_t flags);
extern void    f3s_snor_close(f3s_socket_t *socket, uint32_t flags);
extern void    f3s_snor_reset(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int32_t f3s_snor_sync(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int     f3s_snor_erase(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int32_t f3s_snor_suspend(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int32_t f3s_snor_suspend_sr2(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int32_t f3s_snor_resume(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int     f3s_snor_unlockall(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int     f3s_snor_islock(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int     f3s_snor_unlock(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int     f3s_snor_lock(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);

extern int     snor_write_register(snor_chip_t* const chip,
                    const uint8_t opcode, const uint32_t addr, const uint8_t adrlen, uint8_t* const buf, const int len);
extern int     snor_post_ident(struct _snor_chip_t* const chip);
extern int     snor_read_register(snor_chip_t* const chip, const uint8_t opcode,
                    const uint32_t addr, const uint8_t adrlen, uint8_t* const buf, const int len);
extern int     snor_read_sr(snor_chip_t* const chip, uint8_t *sr);
extern int     snor_read_fsr(snor_chip_t* const chip, uint8_t *fsr);
extern int     snor_write_cmd(snor_chip_t* const chip, const uint8_t opcode);
extern int     snor_reset(snor_chip_t* const chip);
extern int     snor_read_id(snor_chip_t* const chip, uint8_t* const ids, const int len);

extern int     snor_slogf(const int severity, const int verbosity, const int vlevel, const char* const fmt, ...);

extern int     snor_soc_getsubopt(char **optionp, char* const *tokens, char **valuep);
extern void   *snor_alloc_handle(f3s_socket_t* const socket, const size_t nbytes);

// Flash devices
extern int32_t f3s_mt35x_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs);
extern int32_t f3s_sfdp_ident (f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs);
extern int32_t f3s_s28hx_ident(f3s_dbase_t *dbase, f3s_access_t *access, const uint32_t flags, const uint32_t cs);
extern void f3s_s28hx_reset(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t offset);
extern int32_t f3s_gd25q_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs);
extern int32_t f3s_mt25q_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs);

static inline int snor_options_arg_value(const char* const fn, const char* const opts, const char* const vp)
{
    if ((vp == NULL) || (*vp == '\0')) {
        snor_slogf(_SLOG_ERROR, 0, 0, "%s:  Missing argument for '%s'", fn, opts);
        return (EINVAL);
    }
    return (EOK);
}

static inline int snor_options_arg_novalue(const char* const fn, const char* const opts, const char* const vp)
{
    if (vp != NULL) {
        snor_slogf(_SLOG_ERROR, 0, 0, "%s:  Unexpected argument for '%s'", fn, opts);
        return (EINVAL);
    }
    return (EOK);
}

#if 0
#define SNOR_OPTIONS_ARG_VAL(_o, _v, _s) {  \
            if (((_v) == NULL) || (*(_v) == '\0')) {    \
                snor_slogf(_SLOG_ERROR, 0, 0, "%s:  Missing argument for '%s'", __func__, _o); \
                (_s) = EINVAL;  \
                break;      \
            }   \
        }

#define SNOR_OPTIONS_ARG_NOVAL(_o, _v) {    \
            if ((_v) != NULL) { \
                snor_slogf(_SLOG_ERROR, 0, 0, "%s:  Unexpected argument for '%s'", __func__, _o);  \
                break;  \
            }   \
        }
#endif

#define SNOR_NELEMS(_x)     (sizeof((_x)) / sizeof((_x)[0]))

// debug
static inline void dump_buf(const uint8_t* const buf, const int len)
{
    fprintf(stderr, "= = = = = = =\n");
    for (int i = 0; i < len; i++) {
        fprintf(stderr, "%02x", buf[i]);
        if (i == (len - 1)) {
            fprintf(stderr, "\n");
            break;
        } else if ((i % 16) == 15) {
            fprintf(stderr, "\n");
        } else {
            fprintf(stderr, ":");
        }
    }
}


#endif /* __F3S_SNOR_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif

