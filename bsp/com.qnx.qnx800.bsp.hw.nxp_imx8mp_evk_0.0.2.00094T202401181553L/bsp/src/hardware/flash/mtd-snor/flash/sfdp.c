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

#include "f3s_snor.h"

#define SFDP_SIGNATURE          0x50444653U
#define SFDP_JESD216_MAJOR      1u

#define SFDP_PHDR_ID(p)         (((p)->pidm << 8) | (p)->pidl)
#define SFDP_PHDR_PTP(p)        (((p)->ptp[2] << 16) | ((p)->ptp[1] << 8) | ((p)->ptp[0] << 0))

#define SFDP_BFPT_ID            0xFF00u /* Basic Flash Parameter Table */
#define SFDP_4BAIT_ID           0xFF84u /* 4-byte Address Instruction Table */

#define SFDP_4BAIT_DWORD_MAX    2

#define SFDP_PID_MSB            0xFFu
#define SFDP_PID_LSB_BASIC      0x00u
#define SFDP_PID_LSB_SECMAP     0x81u
#define SFDP_PID_LSB_4BAI       0x84u

#define BFPT_DWORD_MAX_JESD216  9
#define BFPT_DWORD_MAX_JESD216B 16

#define SFDP_LEN                256                     // default SFDP size in byte
#define SFDP_HDRSZ              (sizeof(uint32_t) * 2)  // header size in byte
#define SFDP_TBLSZ              20                      // table size, in double word

typedef struct _sfdp_phdr_t {
    uint8_t     pidl;   // Parameter ID LSB
    uint8_t     minor;  // Minor revision
    uint8_t     major;  // Major revision
    uint8_t     pl;     // Parameter length in double word
    uint8_t     ptp[3]; // Parameter table pointer, byte address
    uint8_t     pidm;   // Parameter ID MSB
} sfdp_phdr_t;

typedef struct _sfdp_hdr_t {
    uint32_t    sfdp;
    uint8_t     minor;  // Minor revision
    uint8_t     major;  // Major revision
    uint8_t     nph;    // Number of parameter headers
    uint8_t     ap;     // Access Protocol
    sfdp_phdr_t bfpthdr;// Basic parameter table
} sfdp_hdr_t;


#define SFDP_PTP_OFFSET(p)  ((uint32_t)((p)->ptp[0]) | ((uint32_t)((p)->ptp[1]) << 8) | ((uint32_t)((p)->ptp[2] << 16)))

static int snor_read_sfdp(snor_chip_t* const chip, int off, uint8_t* buf, int len);
static int sfdp_parse_bfpt(snor_chip_t* const chip, sfdp_phdr_t* const phdr);
static int sfdp_parse_4bai(snor_chip_t* const chip, sfdp_phdr_t* const phdr);

/**
 *  @brief             Ident callout for serial NOR flash which supports SFDP.
 *  @param dbase       F3S data base handle.
 *  @param access      F3S access handle.
 *  @param flags       Ident flags.
 *  @param cs          Chip select
 *
 *  @return            EOK --success otherwise fail.
 */
int32_t f3s_sfdp_ident(f3s_dbase_t *dbase, f3s_access_t *access, uint32_t flags, uint32_t cs)
{
    snor_ctrl_t *ctrl;
    snor_chip_t *chip;
    int         status;
    size_t      sz;
    sfdp_hdr_t  hdr;
    sfdp_phdr_t *phdr;
    sfdp_phdr_t *bfpthdr;

    if (access == NULL) return (ENODEV);

    ctrl = (snor_ctrl_t *)access->socket.memory;
    chip = &ctrl->chip[cs];

    status = snor_read_sfdp(chip, 0, (uint8_t *)&hdr, sizeof(hdr));
    if (status != EOK) return (status);
    if ((hdr.sfdp != SFDP_SIGNATURE) || (hdr.major != SFDP_JESD216_MAJOR)) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "(devf  t%d::%s) SFDP signature incorrect: %08x : %02x",
                pthread_self(), __func__, hdr.sfdp, hdr.major);
        return (ENODEV);
    }

    if (hdr.nph == 0) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "(devf  t%d::%s) No parameter table available", pthread_self(), __func__);
        return (ENODEV);
    }

    /*
     * Verify that the first and only mandatory parameter header is a
     * Basic Flash Parameter Table header as specified in JESD216.
     */
    bfpthdr = &hdr.bfpthdr;
    if ((SFDP_PHDR_ID(bfpthdr) != SFDP_BFPT_ID) ||
            (bfpthdr->major != SFDP_JESD216_MAJOR)) {
        snor_slogf(_SLOG_ERROR, ctrl->verbosity, 0,
            "(devf  t%d::%s) Invalid parameter header", pthread_self(), __func__);
        return (ENODEV);
    }

    sz = (size_t)hdr.nph * sizeof(sfdp_phdr_t);
    phdr = malloc(sz);
    if (phdr == NULL) {
        return (ENOMEM);
    }

    status = snor_read_sfdp(chip, sizeof(hdr), (uint8_t *)phdr, (int)sz);
    if (status != EOK) return (status);

    /* Basic parameter table */
    sfdp_phdr_t *phdrtmp;
    for (uint8_t i = 0; i < hdr.nph; i++) {
        phdrtmp = &phdr[i];
        if ((SFDP_PHDR_ID(phdrtmp) == SFDP_BFPT_ID) && (phdrtmp->major == SFDP_JESD216_MAJOR) &&
            ((phdrtmp->minor > bfpthdr->minor) ||
            ((phdrtmp->minor == bfpthdr->minor) && (phdrtmp->pl> bfpthdr->pl)))) {

            bfpthdr = phdrtmp;
        }
    }

    if (sfdp_parse_bfpt(chip, bfpthdr) != EOK) return (ENODEV);

    /* Parse optional parameter tables. */
    for (uint8_t i = 0; i < hdr.nph; i++) {
        phdrtmp = &phdr[i];
        switch (SFDP_PHDR_ID(phdrtmp)) {
            case SFDP_4BAIT_ID:
                status = sfdp_parse_4bai(chip, phdrtmp);
                break;
            default:
                break;
        }
    }

    free(phdr);

    if ((status == EOK) && (dbase != NULL)) {
        dbase->name = "SFDP";
    }

    return (status);
}

/**
 *  @brief             Read SFDP data block.
 *  @param chip        Chip handle.
 *  @param off         Offset into SFDP data block.
 *  @param buf         SFDP buffer.
 *  @param len         Read length.
 *
 *  @return            EOK --success otherwise fail.
 */
static int snor_read_sfdp(snor_chip_t* const chip, int off, uint8_t* buf, int len)
{
    snor_ctrl_t* const ctrl = chip->ctrl;
    snor_cmd_t  cmd;
    snor_op_t   rdsfdp = { .opcode = SNOR_CMD_RDSFDP, .dcycle = 8, .adrlen = (chip->cfg.bus_proto == SNOR_BUSPROTO_8_8_8_DTR) ? 4 : 3 };
    int         rlen;

    if (ctrl->funcs.read != NULL) {
        do {
            SNOR_SET_CMD(cmd, &rdsfdp, &chip->cfg, (uint32_t)off);
            rlen = ctrl->funcs.read(ctrl, &cmd, buf, len);
            if (rlen <= 0) return (EIO);
            off += rlen;
            buf += rlen;
            len -= rlen;
        } while (len > 0);

        return (EOK);
    }


    return (ENODEV);
}

/*
 * JESD216D.01
 * Parse basic parameter table
 *  DWORD #1 : Uniform 4KB Sectors, Write Buffer Size, Volatile Status Register,
 *             Fast Read Support (1-1-2) (1-2-2) (1-4-4)(1-1-4), Number of Address Bytes, DTR Support
 *              bit 0~1   : Block/Sector Erase Sizes
 *              bit 2     : Write Granularity
 *              bit 3     : Volatile Status Register Block Protect bits
 *              bit 4     : Write Enable Instruction Select for Writing to Volatile Status Register
 *              bit 8~15  : 4 Kilobyte Erase Instruction
 *              bit 16    : Supports (1-1-2) Fast Read
 *              bit 17~18 : Address Bytes
 *                          00 : 3-Byte only addressing
 *                          01 : 3- or 4-Byte addressing
 *                          10 : 4-Byte only addressing
 *              bit 19    : Supports Double Transfer Rate (DTR) Clocking
 *              bit 20    : Supports (1-2-2) Fast Read
 *              bit 21    : Supports (1-4-4) Fast Read
 *              bit 22    : Supports (1-1-4) Fast Read
 *  DWORD #2 : Memory Density
 *              For densities 2 gigabits or less, b31 = 0, b0~30: size in bits
 *              For densities over 2 gigabits, b31 = 1, b0~30: 2^N bits
 *  DWORD #3 : Fast Read (1-4-4) (1-1-4): Wait States, Mode Bit Clocks, Instruction
 *              bit 0~4   : (1-4-4) Fast Read Number of Wait states (dummy clocks)
 *              bit 5~7   : (1-4-4) Fast Read Number of Mode Clocks
 *              bit 8~15  : (1-4-4) Fast Read Instruction
 *              bit 16~20 : (1-1-4) Fast Read Number of Wait states (dummy clocks)
 *              bit 21~23 : (1-1-4) Fast Read Number of Mode Clocks
 *              bit 24~31 : (1-1-4) Fast Read Instruction
 *
 *  DOWRD #4 : Fast Read (1-1-2) (1-2-2): Wait States, Mode Bit Clocks, Instruction
 *              bit 0~4   : (1-1-2) Fast Read Number of Wait states (dummy clocks)
 *              bit 5~7   : (1-1-2) Fast Read Number of Mode Clocks
 *              bit 8~15  : (1-1-2) Fast Read Instruction
 *              bit 16~20 : (1-2-2) Fast Read Number of Wait states (dummy clocks)
 *              bit 21~23 : (1-2-2) Fast Read Number of Mode Clocks
 *              bit 24~31 : (1-2-2) Fast Read Instruction
 *  DWORD #5 : Fast Read (2-2-2) (4-4-4) Support
 *              bit 0     : Supports (2-2-2) Fast Read
 *              bit 4     : Supports (4-4-4) Fast Read
 *  DWORD #6 : Fast Read (2-2-2): Wait States, Mode Bit Clocks, Instruction
 *              bit 16~20 : (2-2-2) Fast Read Number of Wait states (dummy clocks)
 *              bit 21~23 : (2-2-2) Fast Read Number of Mode Clocks
 *              bit 24~31 : (2-2-2) Fast Read Instruction
 *  DWORD #7 : Fast Read (4-4-4): Wait States, Mode Bit Clocks, Instruction
 *              bit 16~20 : (4-4-4) Fast Read Number of Wait states (dummy clocks)
 *              bit 21~23 : (4-4-4) Fast Read Number of Mode Clocks
 *              bit 24~31 : (4-4-4) Fast Read Instruction
 *  DWORD #8 : Erase Type 1 & 2 Size and Instruction
 *              bit 0~7   : Erase Type 1 Size, erase type size = 2^N bytes
 *              bit 8~15  : Erase Type 1 Instruction
 *              bit 16~23 : Erase Type 2 Size, erase type size = 2^N bytes
 *              bit 24~31 : Erase Type 2 Instruction
 *  DWORD #9 : Erase Type 3 & 4 Size and Instruction
 *              bit 0~7   : Erase Type 4 Size, erase type size = 2^N bytes
 *              bit 8~15  : Erase Type 4 Instruction
 *              bit 16~23 : Erase Type 3 Size, erase type size = 2^N bytes
 *              bit 24~31 : Erase Type 3 Instruction
 *  DWORD #10: Erase Type (1:4) Typical Erase Times and Multiplier Used To Derive Max Erase Times
 *              bit 0~3   : Multiplier from typical erase time to maximum erase time
 *                          Formula: Erase Type n (or Chip) erase maximum time = 2 * (count + 1) * Erase Type n (or Chip) erase typical time
 *              bit 4~10  : Erase Type 1 Erase, Typical time
 *                          10~9    units (00b: 1 ms, 01b: 16 ms, 10b: 128 ms, 11b: 1 s)
 *                          8~4     count
 *                          Formula: typical time = (count + 1)*units
 *              bit 11~17 : Erase Type 2 Erase, Typical time
 *              bit 18~24 : Erase Type 3 Erase, Typical time
 *              bit 25~31 : Erase Type 4 Erase, Typical time
 *  DWORD #11: Chip Erase Typical Time, Byte Program and Page Program Typical Times, Page Size
 *              bit 0~3   : Multiplier from typical time to max time for Page or byte program
 *                          Formula: maximum time = 2 * (count + 1)*typical time
 *              bit 4~7   : Page Size
 *              bit 8~13  : Page Program Typical time
 *                          13 units (0: 8 μs, 1: 64 μs)
 *                          12~8 count
 *                          Formula: typical page program time = (count + 1)*units
 *              bit 14~18 : Byte Program Typical time, first byte
 *              bit 19~23 : Byte Program Typical time, additional byte
 *              bit 24~30 : Chip Erase, Typical time
 *  DWORD #12: Erase/Program Suspend/Resume Support, Intervals, Latency, Keep Out Area Size
 *              bit 0~3   : Prohibited Operations During Program Suspend
 *              bit 4~7   : Prohibited Operations During Erase Suspend
 *              bit 9~12  : Program Resume to Suspend Interval
 *                          Formula: program resume to suspend interval = (count + 1)*64 μs
 *              bit 13~19 : Suspend in-progress program max latency
 *                          19~18 units (00b: 128ns, 01b: 1μs, 10b: 8μs, 11b: 64μs)
 *                          17~13 count
 *                          Formula: suspend in-progress program max latency = (count+1)*units
 *              bit 20~23 : Erase Resume to Suspend Interval
 *                          Formula: erase resume to suspend interval = (count + 1)*64 μs
 *              bit 24~30 : Suspend in-progress erase max latency
 *                          30:29 units (00b: 128ns, 01b: 1μs, 10b: 8μs, 11b: 64μs)
 *                          28:24 count
 *                          Formula: erase max latency = (count + 1)*units
 *              bit 31    : Suspend / Resume supported, 0: supported, 1: not supported
 *  DWORD #13: Program/Erase Suspend/Resume Instructions
 *              bit 0~7   : Program Resume Instruction
 *              bit 8~15  : Program Suspend Instruction
 *              bit 16~23 : Resume Instruction
 *              bit 24~31 : Suspend Instruction
 *              bit 8~15  : Program Suspend Instruction
 *  DWORD #14: Deep Powerdown and Status Register Polling Device Busy
 *              bit 2     : Use of legacy polling is supported by reading the Status Register
 *                          with 05h instruction and checking WIP bit[0] (0=ready; 1=busy).
 *              bit 3     : Bit 7 of the Flag Status Register may be polled any time a Program,
 *                          Erase, Suspend/Resume command is issued, or after a Reset command
 *                          while the device is busy. The read instruction is 70h.
 *                          Flag Status Register bit definitions: bit[7]: Program or erase controller status (0=busy; 1=ready)
 *  DWORD #15: Hold and WP Disable Function, Quad Enable Requirements, 4-4-4 Mode Enable/Disable Sequences, 0-4-4 Entry/Exit Methods and Support
 *  DWORD #16: 32-bit Address Entry/Exit Methods and Support, Soft Reset and Rescue Sequences, Volatile and Nonvolatile Status Register Support
 *  DWORD #17: Fast Read (1-8-8) (1-1-8): Wait States, Mode Bit Clocks, Instruction
 *              bit 0~4   : (1-8-8) Fast Read Number of Wait states (dummy clocks)
 *              bit 5~7   : (1-8-8) Fast Read Number of Mode Clocks
 *              bit 8~15  : (1-8-8) Fast Read Instruction
 *              bit 16~20 : (1-1-8) Fast Read Number of Wait states (dummy clocks)
 *              bit 21~23 : (1-1-8) Fast Read Number of Mode Clocks
 *              bit 24~31 : (1-1-8) Fast Read Instruction
 *  DWORD #18: Octal commands, Byte order, Data strobe, JEDEC SPI Protocol Reset
 *              bit 18~22 : Variable Output Driver Strength
 *              bit 23    : JEDEC SPI Protocol Reset (In-Band Reset)
 *              bit 24~25 : Data Strobe Waveforms in STR Mode
 *              bit 26    : Data Strobe support for QPI STR mode (4S-4S-4S)
 *              bit 27    : Data Strobe support for QPI DTR mode (4S-4D-4D)
 *              bit 29~30 : Octal DTR (8D-8D-8D) Command and Command Extension
 *              bit 31    : Byte Order in 8D-8D-8D mode
 *  DWORD #19: Octal Enable Requirements, 8-8-8 Mode Enable/Disable Sequences, 0-8-8 Entry/Exit Methods and Support
 *              bit 0~3   : 8s-8s-8s mode disable sequences
 *              bit 4~8   : 8s-8s-8s mode enable sequences
 *              bit 20~22 : Octal Enable Requirements:
 *                          000b: Device does not have an Octal Enable bit.
 *                          001b: Octal Enable is bit 3 of status register 2. It is set via Write status register 2
 *                          instruction 31h with one data byte where bit 3 is one.
 *                          It is cleared via Write status register 2 instruction 3Eh with one data byte where bit 3
 *                          is zero. The status register 2 is read using instruction 65h with address byte 02h and one dummy byte.
 *  DWORD #20: Maximum operating speeds
 *              bit 0~3   : Maximum operation speed of device in 4S-4S-4S mode when not utilizing Data Strobe
 *              bit 4~7   : Maximum operation speed of device in 4S-4S-4S mode when utilizing Data Strobe
 *              bit 8~11  : Maximum operation speed of device in 4S-4D-4D mode when not utilizing Data Strobe
 *              bit 12~15 : Maximum operation speed of device in 4S-4D-4D mode when utilizing Data Strobe
 *              bit 16~19 : Maximum operation speed of device in 8S-8S-8S mode when not utilizing Data Strobe
 *              bit 20~23 : Maximum operation speed of device in 8S-8S-8S mode when utilizing Data Strobe
 *              bit 24~27 : Maximum operation speed of device in 8D-8D-8D mode when not utilizing Data Strobe
 *              bit 28~31 : Maximum operation speed of device in 8D-8D-8D mode when utilizing Data Strobe
 *                          speed table :   1100b: 400 MHz
 *                                          1011b: 333 MHz
 *                                          1010b: 266 MHz
 *                                          1001b: 250 MHz
 *                                          1000b: 200 MHz
 *                                          0111b: 166 MHz
 *                                          0110b: 133 MHz
 *                                          0101b: 100 MHz
 *                                          0100b: 80 MHz
 *                                          0011b: 66 MHz
 *                                          0010b: 50 MHz
 *                                          0001b: 33 MHz
 */
static inline void sfdp_fill_rdop(snor_chip_t* const chip, const enum snor_busproto_idx proto,
                const uint64_t cap, const uint32_t* const wptr, const int flgw, const int flgb, const int opw, const int opo)
{
    if ((wptr[flgw] & (1 << (flgb)))) {
        chip->hcaps |= cap;
        chip->rdops[proto].opcode = (uint8_t)((wptr[opw] >> (opo + 8)) & 0xff);
        chip->rdops[proto].dcycle = (uint8_t)(((wptr[opw] >> opo) & 0x1F) + ((wptr[opw] >> (opo + 5)) & 0x07));
    }
}

/**
 *  @brief             Fill erase information.
 *  @param chip        Chip handle.
 *  @param wptr        Pointer to erase parameter words.
 *  @param idx         Erase type index.
 *  @param oc          Erase info offset.
 *  @param os          Erase info shift.
 *  @param to          Timeout info offset.
 *  @param ts          Timeout infoshift.
 *  @param tblen       Table length.
 *
 *  @return            EOK --success otherwise fail.
 */
static inline void sfdp_fill_einfo(snor_chip_t* chip, const uint32_t* const wptr,
                const int idx, const int oc, const int os, const uint8_t to, const uint8_t ts, const uint8_t tblen)
{
    chip->blkers[idx].blksz_pow2 = (uint8_t)((wptr[oc] >> (os)) & 0xFF);
    chip->blkers[idx].opcode     = (uint8_t)((wptr[oc] >> (os + 8)) & 0xFF);

    if (tblen > to) {
        chip->blkers[idx].met    = (uint16_t)((wptr[to]) & 0x0F);

        switch ((wptr[to] >> (5 + ts)) & 3) {
            case 0:
                chip->blkers[idx].tet = 1;
                break;
            case 1:
                chip->blkers[idx].tet = 16;
                break;
            case 2:
                chip->blkers[idx].tet = 128;
                break;
            case 3:
                chip->blkers[idx].tet = 1000;
                break;
            default:
                break;
        }
        chip->blkers[idx].tet *= (wptr[to] >> (ts)) & 0x1F;
    }
}

/**
 *  @brief             Parse basic flash parameter table.
 *  @param chip        Chip handle.
 *  @param phdr        Parameter header pointer.
 *
 *  @return            EOK --success otherwise fail.
 */
static int sfdp_parse_bfpt(snor_chip_t* const chip, sfdp_phdr_t* const phdr)
{
    uint32_t    dwords[SFDP_TBLSZ];
    int         status;
    uint8_t     tblen = phdr->pl;

    if (tblen < BFPT_DWORD_MAX_JESD216) return (EINVAL);
    if (tblen > SFDP_TBLSZ) {
        tblen = SFDP_TBLSZ;
    }

    status = snor_read_sfdp(chip, SFDP_PHDR_PTP(phdr), (uint8_t *)&dwords[0], (int)tblen * 4);
    if (status != EOK) return (status);

    static const uint32_t   adrm[4] = { 0, SNOR_DCAPS_ADDR_3B_4B, SNOR_DCAPS_ADDR_4B, 0 };
    chip->dcaps |= adrm[(dwords[0] >> 17) & 3];

    /* read OPs */
    sfdp_fill_rdop(chip, SNOR_BPI_1_1_2, SNOR_HCAPS_RD_1_1_2, dwords, 0, 16, 3,  0);
    sfdp_fill_rdop(chip, SNOR_BPI_1_2_2, SNOR_HCAPS_RD_1_2_2, dwords, 0, 20, 3, 16);
    sfdp_fill_rdop(chip, SNOR_BPI_1_4_4, SNOR_HCAPS_RD_1_4_4, dwords, 0, 21, 2,  0);
    sfdp_fill_rdop(chip, SNOR_BPI_1_1_4, SNOR_HCAPS_RD_1_1_4, dwords, 0, 22, 2, 16);
    sfdp_fill_rdop(chip, SNOR_BPI_2_2_2, SNOR_HCAPS_RD_2_2_2, dwords, 4,  0, 5, 16);
    sfdp_fill_rdop(chip, SNOR_BPI_4_4_4, SNOR_HCAPS_RD_4_4_4, dwords, 4,  4, 6, 16);

    /* DTR */
    if (dwords[0] & (1u << 19)) {
        chip->hcaps |= SNOR_HCAPS_DTR_CMD;
    }

    /* Chip density */
    if (dwords[1] & (1u << 31)) {
        chip->chipsz = (1u << ((dwords[1] & 0x7FFFFFFF) - 3));
    } else {
        chip->chipsz = (dwords[1] + 1) >> 3;
    }

    /* Fill erase types */
    sfdp_fill_einfo(chip, dwords, 0, 7,  0, 9,  4, tblen);
    sfdp_fill_einfo(chip, dwords, 1, 7, 16, 9, 11, tblen);
    sfdp_fill_einfo(chip, dwords, 2, 8,  0, 9, 18, tblen);
    sfdp_fill_einfo(chip, dwords, 3, 8, 16, 9, 25, tblen);

    if (tblen <= BFPT_DWORD_MAX_JESD216) return EOK;

    /* DWORD 11, program info */
    chip->pagesz = (uint16_t)(1u << ((dwords[10] >> 4) & 0x0F));
    chip->pptt   = (uint16_t)(((dwords[10] >> 8) & 0x1F) + 1);
    chip->pptt  *= (uint16_t)((dwords[10] & (1 << 13)) ? 64 : 8);   /* in us */
    chip->ppmt   = (uint16_t)((dwords[10] & 0x0F) + 1);  /* multiplier from typical time to max time */

    /* DWORD12/13 suspend/resume */
    /* DWORD12 bit 31, 0: supported, 1: not supported */
    if ((dwords[11] & (1 << 31)) == 0) {
        chip->dcaps |= SNOR_DCAPS_PSR | SNOR_DCAPS_ESR;
        chip->op_pr = (uint8_t)(dwords[12] & 0xFF);
        chip->op_ps = (uint8_t)((dwords[12] >> 8) & 0xFF);
        chip->op_er = (uint8_t)((dwords[12] >> 16) & 0xFF);
        chip->op_es = (uint8_t)((dwords[12] >> 24) & 0xFF);
    }

    static const uint32_t sml[4] = { 128, 1000, 8 * 1000, 64 * 1000 };
    chip->e2sl  = sml[(dwords[11] >> 29) & 3] * (((dwords[11] >> 24) & 0x1F) + 1);
    chip->er2si = (((dwords[11] >> 20) & 0x0F) + 1) * 64;
    chip->p2sl  = sml[(dwords[11] >> 18) & 3] * (((dwords[11] >> 13) & 0x1F) + 1);
    chip->pr2si = (((dwords[11] >> 9) & 0x0F) + 1) * 64;

    /* DWORD14 */
    if (dwords[13] & (1 << 2)) {
//        fprintf(stderr, "Legacy status polling\n");
    }
    if (dwords[13] & (1 << 3)) {
        chip->dcaps |= SNOR_DCAPS_FSTATUS;
    }

    /* DWORD15 quad mode related */
    chip->qer = (uint8_t)((dwords[14] >> 20) & 7);
    chip->qes = (uint8_t)((dwords[14] >> 4) & 0x1F);
    chip->qds = (uint8_t)((dwords[14] >> 0) & 0x0F);

    /* DWORD16, enter 4B address protocol etc */
    chip->e4ba = (uint8_t)((dwords[15] >> 24) & 0xFF);
    chip->x4ba = (uint16_t)((dwords[15] >> 14) & 0x3FF);
    chip->ssrs = (uint8_t)((dwords[15] >> 8) & 0x3F);
    chip->asr1 = (uint8_t)((dwords[15] >> 0) & 0x7F);

    if (tblen <= BFPT_DWORD_MAX_JESD216B) return (EOK);

    /* DWORD17, 1-1-8 and 1-8-8 support */
    chip->rdops[SNOR_BPI_1_1_8].opcode = (uint8_t)((dwords[16] >> 24) & 0xFF);
    if (chip->rdops[SNOR_BPI_1_1_8].opcode != 0) {
        chip->hcaps |= SNOR_HCAPS_RD_1_1_8;
        chip->rdops[SNOR_BPI_1_1_8].dcycle = (uint8_t)(((dwords[16] >> 16) & 0x1F) + ((dwords[16] >> 21) & 0x07));
    }

    chip->rdops[SNOR_BPI_1_8_8].opcode = (uint8_t)((dwords[16] >> 8) & 0xFF);
    if (chip->rdops[SNOR_BPI_1_8_8].opcode != 0) {
        chip->hcaps |= SNOR_HCAPS_RD_1_8_8;
        chip->rdops[SNOR_BPI_1_8_8].dcycle = (uint8_t)(((dwords[16] >> 0) & 0x1F) + ((dwords[16] >> 5) & 0x07));
    }

    // TODO! DWORD18
    // DQS etc
    // TODO! DWORD19
    // Octal mode etc
    // TODO! DWORD20
    // quad/octal bus speed limit

    return (EOK);
}

/**
 *  @brief             Parse 4-bytes address instruction parameter table.
 *  @param chip        Chip handle.
 *  @param phdr        Parameter header pointer.
 *
 *  @return            EOK --success otherwise fail.
 */
static int sfdp_parse_4bai(snor_chip_t* const chip, sfdp_phdr_t* const phdr)
{
    uint32_t    dwords[SFDP_4BAIT_DWORD_MAX];
    int         status;

    if ((phdr->major != SFDP_JESD216_MAJOR) || (phdr->pl < SFDP_4BAIT_DWORD_MAX)) return (EINVAL);

    status = snor_read_sfdp(chip, SFDP_PHDR_PTP(phdr), (uint8_t *)&dwords[0], (int)sizeof(dwords));
    if (status != EOK) return (status);

    chip->dcaps |= dwords[0] & 0x01F0E1FF;      /* 4B read/pp capabilities */

    /* sector erase */
    for (int i = 0; i <= 3; i++) {
        if (dwords[0] & (1 << (i + 9))) {
            chip->blkers[i].opcode_4b = (uint8_t)((dwords[1] >> (i * 8)) & 0xFF);
        }
    }

    return (EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
