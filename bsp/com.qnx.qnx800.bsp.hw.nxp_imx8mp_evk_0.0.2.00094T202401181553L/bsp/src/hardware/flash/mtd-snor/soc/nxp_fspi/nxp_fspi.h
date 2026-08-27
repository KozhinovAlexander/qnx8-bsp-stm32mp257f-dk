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

#ifndef _NXP_FSPI_H
#define _NXP_FSPI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <hw/inout.h>

#include "f3s_snor.h"

#define FSPI_BASE                0x5D120000
#define FSPI_SIZE                0x10000
#define FSPI_DEFAULT_IRQ         124
#define FSPI_AMBA_BASE           0x08000000

/* Register MCR0 - Module Configuration Register 0 */
#define FSPI_MCR0                0x0
 /* Field RESET - Software Reset */
 #define FSPI_MCR0_SWRESET_MASK         (0x1u << 0)
 /* Field MDIS - Module Disable */
 #define FSPI_MCR0_MDIS_MASK            (0x1u << 1)
 /* Field RXCLKSRC - Sample Clock source selection for Flash Reading */
 #define FSPI_MCR0_RXCLKSRC_MASK        (0x3u << 4)
 #define FSPI_MCR0_RXCLKSRC(v)          (((v) << 4) & FSPI_MCR0_RXCLKSRC_MASK)
 #define FSPI_MCR0_RXCLKSRC_BV_LPBK_INT 0u
 #define FSPI_MCR0_RXCLKSRC_BV_LPBK_DQS 1u
 #define FSPI_MCR0_RXCLKSRC_BV_FLSH_DQS 3u
 /* Field ARDFEN - Enable AHB bus Read Access to IP RX FIFO */
 #define FSPI_MCR0_ARDFEN_MASK          (0x1u << 6)
 /* Field ATDFEN - Enable AHB bus Write Access to IP TX FIFO */
 #define FSPI_MCR0_ATDFEN_MASK          (0x1u << 7)
 /* Field COMBINATIONEN - Flash Octal mode */
 #define FSPI_MCR0_COMBINATIONEN_MASK   (0x1u << 13)

/* Register MCR1 - Module Configuration Register 1 */
#define FSPI_MCR1                0x4
 /* Field SEQWAIT - Command Sequence Execution timeout */
 #define FSPI_MCR1_SEQWAIT_MASK         (0xFFFFu << 16)
 /* Field AHBBUSWAIT - AHB Read/Write access to Serial Flash Memory timeout */
 #define FSPI_MCR1_AHBBUSWAIT_MASK      (0xFFFFu << 0)

/* Register MCR2 - Module Configuration Register 2 */
#define FSPI_MCR2                0x8
 /* Field RESUMEWAIT - Wait cycle (in AHB clock cycle) - idle state before suspended command resumed */
 #define FSPI_MCR2_RESUMEWAIT_MASK      (0xFFu << 24)
 /* Field SCKBDIFFOPT - SCKB pad can be used as SCKA differential clock output (inverted clock to SCKA) */
 #define FSPI_MCR2_SCKBDIFFOPT_MASK     (0x1u << 19)
 /* Field SAMEDEVICEEN - All external devices are same devices (both in types and size) for A1/A2/B1/B2 */
 #define FSPI_MCR2_SAMEDEVICEEN_MASK    (0x1u << 15)
 /* Field CLRLEARNPHASE - Sampling clock phase selection reset */
 #define FSPI_MCR2_CLRLEARNPHASE_MASK   (0x1u << 14)
 /* Field CLRAHBBUFOPT - AHB buffers will be cleaned when FlexSPI return from STOP mode */
 #define FSPI_MCR2_CLRAHBBUFOPT_MASK    (0x1u << 11)

/* Register AHBCR - AHB Bus Control Register */
#define FSPI_AHBCR               0xC
 /* Field READADDROPT - AHB burst start address alignment */
 #define FSPI_AHBCR_READADDROPT_SHIFT   6
 #define FSPI_AHBCR_READADDROPT_MASK    (0x1u << 6)
 /* Field PREFETCHEN - AHB Read Prefetch Enable */
 #define FSPI_AHBCR_PREFETCHEN_SHIFT    5
 #define FSPI_AHBCR_PREFETCHEN_MASK     (0x1u << 5)
 /* Field BUFFERABLEEN - Enable AHB bus buffer-able write access support */
 #define FSPI_AHBCR_BUFFERABLEEN_SHIFT  4
 #define FSPI_AHBCR_BUFFERABLEEN_MASK   (0x1u << 4)
 /* Field CACHABLEEN - Enable AHB bus cache-able read access support */
 #define FSPI_AHBCR_CACHABLEEN_SHIFT    3
 #define FSPI_AHBCR_CACHABLEEN_MASK     (0x1u << 3)
 /* Field APAREN - Parallel mode enabled for AHB triggered Command (both read and write) */
 #define FSPI_AHBCR_APAREN_SHIFT        0
 #define FSPI_AHBCR_APAREN_MASK         (0x1u << 0)

/* Register INTEN - Interrupt Enable Register */
#define FSPI_INTEN               0x10

/* Register INTR - Interrupt Register */
#define FSPI_INTR                0x14
 /* Field IPCMDDONE - IP triggered Command Sequences Execution finished interrupt */
 #define FSPI_INTR_IPCMDDONE_MASK   (0x1u << 0)
 /* Field IPCMDGE - IP triggered Command Sequences Grant Timeout interrupt */
 #define FSPI_INTR_IPCMDGE_MASK     (0x1u << 1)
 /* Field IPCMDERR - IP triggered Command Sequences Error Detected interrupt */
 #define FSPI_INTR_IPCMDERR_MASK    (0x1u << 3)
 /* Field IPTXWE - IP TX FIFO */
 #define FSPI_INTR_IPTXWE_MASK      (0x1u << 6)
 /* Field IPRXWA - IP RX FIFO */
 #define FSPI_INTR_IPRXWA_MASK      (0x1u << 5)

/* Register LUTKEY - LUT Key Register */
#define FSPI_LUTKEY             0x18
 #define FSPI_LUT_KEY_VAL           0x5AF05AF0U
 /* Field KEY - The key is 0x5AF05AF0 */
 #define FSPI_LUTKEY_KEY_SHIFT      0
 #define FSPI_LUTKEY_KEY_MASK       (0xFFFFFFFFu << 0)

/* Register LUTCR - LUT Control Register */
#define FSPI_LUTCR              0x1C
 /* Field UNLOCK - Unlock LUT */
 #define FSPI_LUTCR_UNLOCK_SHIFT    1
 #define FSPI_LUTCR_UNLOCK_MASK     (0x1u << 1)
 /* Field LOCK - Lock LUT */
 #define FSPI_LUTCR_LOCK_SHIFT      0
 #define FSPI_LUTCR_LOCK_MASK       (0x1u << 0)

/* Register AHBRXBUFaCR0 - AHB RX Buffer a Control Register 0 */
#define FSPI_AHBRXBUFaCR0_NUM       8
#define FSPI_AHBRXBUF0CR0           0x20
 #define FSPI_AHBRXBUFaCR0(index)       (FSPI_AHBRXBUF0CR0 + ((index) * 4))
 /* Field MSTRID - This AHB RX Buffer is assigned according to AHB Master with ID (MSTR_ID) */
 #define FSPI_AHBRXBUFaCR0_MSTRID_SHIFT 16
 #define FSPI_AHBRXBUFaCR0_MSTRID_MASK  (0xFu << 16)
 /* Field BUFSZ - AHB RX Buffer Size in 64 bits */
 #define FSPI_AHBRXBUFaCR0_BUFSZ_SHIFT  0
 #define FSPI_AHBRXBUFaCR0_BUFSZ_MASK   (0x1FFu << 0)

/* Register FLSHA1CR0 - Flash A1 Control Register 0 */
#define FSPI_FLSHA1CR0          0x60
 /* Field FLSHSZ - Flash Size in KByte */
 #define FSPI_FLSHA1CR0_FLSHSZ_MASK     (0x7FFFFFu << 0)

/* Register FLSHA2CR0 - Flash A2 Control Register 0 */
#define FSPI_FLSHA2CR0          0x64
 /* Field FLSHSZ - Flash Size in KByte */
 #define FSPI_FLSHA2CR0_FLSHSZ_SHIFT    0
 #define FSPI_FLSHA2CR0_FLSHSZ_MASK     (0x7FFFFFu << 0)

/* Register FLSHB1CR0 - Flash B1 Control Register 0 */
#define FSPI_FLSHB1CR0          0x68
 /* Field FLSHSZ - Flash Size in KByte */
 #define FSPI_FLSHB1CR0_FLSHSZ_SHIFT    0
 #define FSPI_FLSHB1CR0_FLSHSZ_MASK     (0x7FFFFFu << 0)

/* Register FLSHB2CR0 - Flash B2 Control Register 0 */
#define FSPI_FLSHB2CR0          0x6C
 /* Field FLSHSZ - Flash Size in KByte */
 #define FSPI_FLSHB2CR0_FLSHSZ_SHIFT    0
 #define FSPI_FLSHB2CR0_FLSHSZ_MASK     (0x7FFFFFu << 0)

/* Register FLSHA1CR1 - Flash A1 Control Register 1 */
#define FLEXSPI_FLSHA1CR1       0x70
 /* Field CSINTERVAL - CS interval */
 #define FSPI_FLSHA1CR1_CSINTERVAL_SHIFT      16
 #define FSPI_FLSHA1CR1_CSINTERVAL_MASK       (0xFFFFu << FSPI_FLSHA1CR1_CSINTERVAL_SHIFT)
 /* Field CSINTERVALUNIT - CS interval unit */
 #define FSPI_FLSHA1CR1_CSINTERVALUNIT_SHIFT  15
 #define FSPI_FLSHA1CR1_CSINTERVALUNIT_MASK   (0x1u << FSPI_FLSHA1CR1_CSINTERVALUNIT_SHIFT)
 /* Field CAS - Column Address Size */
 #define FSPI_FLSHA1CR1_CAS_SHIFT             11
 #define FSPI_FLSHA1CR1_CAS_MASK              (0xFu << FSPI_FLSHA1CR1_CAS_SHIFT)
 /* Field WA - Word Addressable */
 #define FSPI_FLSHA1CR1_WA_SHIFT              10
 #define FSPI_FLSHA1CR1_WA_MASK               (0x1u << FSPI_FLSHA1CR1_WA_SHIFT)
 /* Field TCSH - Serial Flash CS Hold time */
 #define FSPI_FLSHA1CR1_TCSH_SHIFT            5
 #define FSPI_FLSHA1CR1_TCSH_MASK             (0x1Fu << FSPI_FLSHA1CR1_TCSH_SHIFT)
 /* Field TCSS - Serial Flash CS setup time */
 #define FSPI_FLSHA1CR1_TCSS_SHIFT            0
 #define FSPI_FLSHA1CR1_TCSS_MASK             (0x1Fu << FSPI_FLSHA1CR1_TCSS_SHIFT)

/* Register FLSHA1CR2 - Flash A1 Control Register 2 */
#define FSPI_FLSHA1CR2          0x80
 /* Field CLRINSTRPTR - Clear the instruction pointer */
 #define FSPI_FLSHA1CR2_CLRINSTRPTR_SHIFT     31
 #define FSPI_FLSHA1CR2_CLRINSTRPTR_MASK      (0x1u << 31)
 /* Field AWRWAITUNIT - Wait unit */
 #define FSPI_FLSHA1CR2_AWRWAITUNIT_SHIFT     28
 #define FSPI_FLSHA1CR2_AWRWAITUNIT_MASK      (0x7u << 28)
 /* Field AWRWAIT - Wait */
 #define FSPI_FLSHA1CR2_AWRWAIT_SHIFT         16
 #define FSPI_FLSHA1CR2_AWRWAIT_MASK          (0xFFFu << 16)
 /* Field AWRSEQNUM - Sequence Number for AHB Write triggered Command */
 #define FSPI_FLSHA1CR2_AWRSEQNUM_SHIFT       13
 #define FSPI_FLSHA1CR2_AWRSEQNUM_MASK        (0x7u << 13)
 /* Field AWRSEQID - Sequence Index for AHB Write triggered Command */
 #define FSPI_FLSHA1CR2_AWRSEQID_SHIFT        8
 #define FSPI_FLSHA1CR2_AWRSEQID_MASK         (0x1Fu << 8)
 /* Field ARDSEQNUM - Sequence Number for AHB Read triggered Command in LUT */
 #define FSPI_FLSHA1CR2_ARDSEQNUM_SHIFT       5
 #define FSPI_FLSHA1CR2_ARDSEQNUM_MASK        (0x7u << 5)
 /* Field ARDSEQID - Sequence Index for AHB Read triggered Command in LUT */
 #define FSPI_FLSHA1CR2_ARDSEQID_SHIFT        0
 #define FSPI_FLSHA1CR2_ARDSEQID_MASK         (0x1Fu << 0)

/* Register IPCR0 - IP Control Register 0 */
#define FSPI_IPCR0              0xA0
 /* Field SFAR - Serial Flash Address for IP command */
 #define FSPI_IPCR0_SFAR_SHIFT      0
 #define FSPI_IPCR0_SFAR_MASK       (0xFFFFFFFFu << 0)

/* Register IPCR1 - IP Control Register 1 */
#define FSPI_IPCR1              0xA4
 /* Field IPAREN - Parallel mode Enabled for IP command */
 #define FSPI_IPCR1_IPAREN_SHIFT    31
 #define FSPI_IPCR1_IPAREN_MASK     (0x1u << FSPI_IPCR1_IPAREN_SHIFT)
 /* Field ISEQNUM - Sequence Number for IP command */
 #define FSPI_IPCR1_ISEQNUM_MASK    (0x7u << 24)
 /* Field ISEQID - Sequence Index in LUT for IP command */
 #define FSPI_IPCR1_ISEQID_SHIFT    16
 #define FSPI_IPCR1_ISEQID_MASK     (0x1Fu << FSPI_IPCR1_ISEQID_SHIFT)
 /* Field IDATSZ - Flash Read/Program Data Size (in Byte) for IP command */
 #define FSPI_IPCR1_IDATSZ_MASK     (0xFFFFu << 0)

/* Register IPCMD - IP Command Register */
#define FSPI_IPCMD              0xB0
 /* Field TRG - IP command trigger */
 #define FSPI_IPCMD_TRG_MASK        (0x1u << 0)

/* Register IPRXFCR - IP RX FIFO Control Register */
#define FSPI_IPRXFCR            0xB8
 /* Field RXWMRK - Water-mark level is (RXWMRK+1)*64 Bits */
 #define FSPI_IPRXFCR_RXWMRK_MASK   (0x3Fu << 2)
 /* Field RXDMAEN - IP RX FIFO reading by DMA enabled */
 #define FSPI_IPRXFCR_RXDMAEN_MASK  (0x1u << 1)
 /* Field CLRIPRXF - Clear all valid data entries in IP RX FIFO */
 #define FSPI_IPRXFCR_CLRIPRXF_MASK (0x1u << 0)


/* Register IPTXFCR - IP TX FIFO Control Register */
#define FSPI_IPTXFCR            0xBC
 /* Field TXWMRK - Water-mark level is (TXWMRK+1)*64 Bits */
 #define FSPI_IPTXFCR_TXWMRK_MASK   (0x7Fu << 2)
 /* Field TXDMAEN - IP TX FIFO reading by DMA enabled */
 #define FSPI_IPTXFCR_TXDMAEN_MASK  (0x1u << 1)
 /* Field CLRIPTXF - Clear all valid data entries in IP TX FIFO */
 #define FSPI_IPTXFCR_CLRIPTXF_MASK (0x1u << 0)

/* Register DLLACR - DLL A Control Register */
#define FSPI_DLLACR             0xC0
 /* Field OVRDVAL - Slave clock delay line delay cell number selection override value */
 #define FSPI_DLLACR_OVRDVAL_SHIFT  9
 #define FSPI_DLLACR_OVRDVAL_MASK   (0x3Fu << 9)
 /* Field OVRDEN - Slave clock delay line delay cell number selection override enable */
 #define FSPI_DLLACR_OVRDEN_SHIFT   8
 #define FSPI_DLLACR_OVRDEN_MASK    (0x1u << 8)
 /* Field SLVDLYTARGET - The delay target for slave delay line */
 #define FSPI_DLLACR_SLVDLYTARGET_SHIFT 3
 #define FSPI_DLLACR_SLVDLYTARGET_MASK  (0xFu << 3)
 /* Field DLLRESET - Software could force a reset on DLL */
 #define FSPI_DLLACR_DLLRESET_SHIFT     1
 #define FSPI_DLLACR_DLLRESET_MASK      (0x1u << 1)
 /* Field DLLEN - DLL calibration enable */
 #define FSPI_DLLACR_DLLEN_SHIFT        0
 #define FSPI_DLLACR_DLLEN_MASK         (0x1u << 0)

/* Register DLLBCR - DLL B Control Register */
#define FSPI_DLLBCR             0xC4
 /* Field OVRDVAL - Slave clock delay line delay cell number selection override value */
 #define FSPI_DLLBCR_OVRDVAL_SHIFT  9
 #define FSPI_DLLBCR_OVRDVAL_MASK   (0x3Fu << 9)
 /* Field OVRDEN - Slave clock delay line delay cell number selection override enable */
 #define FSPI_DLLBCR_OVRDEN_SHIFT   8
 #define FSPI_DLLBCR_OVRDEN_MASK    (0x1u << 8)
 /* Field SLVDLYTARGET - The delay target for slave delay line */
 #define FSPI_DLLBCR_SLVDLYTARGET_SHIFT 3
 #define FSPI_DLLBCR_SLVDLYTARGET_MASK  (0xFu << 3)
 /* Field DLLRESET - Software could force a reset on DLL */
 #define FSPI_DLLBCR_DLLRESET_SHIFT 1
 #define FSPI_DLLBCR_DLLRESET_MASK  (0x1u << 1)
 /* Field DLLEN - DLL calibration enable */
 #define FSPI_DLLBCR_DLLEN_SHIFT    0
 #define FSPI_DLLBCR_DLLEN_MASK     (0x1u << 0)

/* Register STS0 - Status Register 0 */
#define FSPI_STS0               0xE0
 /* Field DATALEARNPHASEB - Indicate the sampling clock phase selection on Port B after Data Learning */
 #define FSPI_STS0_DATALEARNPHASEB_MASK (0xFu << 8)
 /* Field DATALEARNPHASEA - Indicate the sampling clock phase selection on Port A after Data Learning */
 #define FSPI_STS0_DATALEARNPHASEA_MASK (0xFu << 4)
 /* Field ARBCMDSRC - Indicates the trigger source of current command sequence granted by arbitrator */
 #define FSPI_STS0_ARBCMDSRC_MASK       (0x7u << 2)
 /* Field ARBIDLE - Indicates the state machine in ARB_CTL */
 #define FSPI_STS0_ARBIDLE_MASK         (0x1u << 1)
 /* Field SEQIDLE - Indicates the state machine in SEQ_CTL */
 #define FSPI_STS0_SEQIDLE_MASK         (0x1u << 0)

/* Register STS1 - Status Register 1 */
#define FSPI_STS1               0xE4

/* Register STS2 - Status Register 2 */
#define FSPI_STS2               0xE8

/* Register IPRXFSTS - IP RX FIFO Status Register */
#define FSPI_IPRXFSTS           0xF0
 /* Field RDCNTR - Total Read Data Counter: RDCNTR * 64 Bits */
 #define FSPI_IPRXFSTS_RDCNTR_MASK      (0xFFFFu << 16)
 /* Field FILL - Fill level of IP RX FIFO */
 #define FSPI_IPRXFSTS_FILL_MASK        (0xFFu << 0)

/* Register RFDRa - IP RX FIFO Data Register a */
#define FSPI_RFDRa_NUM          32
#define FSPI_RFDR0              0x100
 #define FSPI_RFDRa(index)      (FSPI_RFDR0 + ((index) * 4))

/* Register TFDRa - IP TX FIFO Data Register a */
#define FSPI_TFDRa_NUM          32
#define FSPI_TFDR0              0x180
 #define FSPI_TFDRa(index)      (FSPI_TFDR0 + ((index) * 4))

/* Register LUTa - Look-up table Register a */
#define FSPI_LUTa_NUM           128  /* 128 / 4 = 128 LUT records */
#define FSPI_LUT0               0x200
 #define FSPI_LUTa(index)           (FSPI_LUT0 + ((index) * 4))
 /* Field OPCODE1 */
 #define FSPI_LUT_OPCODE1_SHIFT     26
 #define FSPI_LUT_OPCODE1_MASK      (0x3Fu << 26)
 /* Field NUM_PADS1 */
 #define FSPI_LUT_NUM_PADS1_SHIFT   24
 #define FSPI_LUT_NUM_PADS1_MASK    (0x3u << 24)
 /* Field OPERAND1 */
 #define FSPI_LUT_OPERAND1_SHIFT    16
 #define FSPI_LUT_OPERAND1_MASK     (0xFFu << 16)
 /* Field OPCODE0 */
 #define FSPI_LUT_OPCODE0_SHIFT     10
 #define FSPI_LUT_OPCODE0_MASK      (0x3Fu << 10)
 /* Field NUM_PADS0 */
 #define FSPI_LUT_NUM_PADS0_SHIFT   8
 #define FSPI_LUT_NUM_PADS0_MASK    (0x3u << 8)
 /* Field OPERAND0 */
 #define FSPI_LUT_OPERAND0_SHIFT    0
 #define FSPI_LUT_OPERAND0_MASK     (0xFFu << 0)

#define FSPI_RX_BUF_DEPTH           (64 * 8)    /* (64 * 64 bits) */
#define FSPI_TX_BUF_DEPTH           (128 * 8)   /* (128 * 64 bits) */
#define FSPI_AHB_BUF_SIZE           (256 * 8)   /* (256 * 64 bits) */
#define DEFAULT_FLASH_MEM_SIZE      0x4000000   /* 64MB */
#define FSPI_MAX_NUM_OF_CHIPS       4
#define FSPI_MAX_NUM_OF_SEQUENCE    32

#define ROUNDDOWN(v, a)         ((v) & (~((a) - 1)))
/**
 * There are a total of 128 LUT registers. These 128 registers are
 * divided into groups of 4 registers that make a valid sequence.
 */
#define LUT_SEQUENCE_SIZE       4U

/* Instruction set for the LUT register. */
#define LUT_STOP            0x00
#define LUT_CMD             0x01
#define LUT_ADDR            0x02
#define LUT_CADDR           0x03
#define LUT_MODE1           0x04
#define LUT_MODE2           0x05
#define LUT_MODE4           0x06
#define LUT_MODE8           0X07
#define LUT_WRITE           0x08
#define LUT_READ            0x09
#define LUT_LEARN           0x0A
#define LUT_DATSZ           0x0B
#define LUT_DUMMY           0x0C
#define LUT_DUMMY_RWDS      0x0D
#define LUT_JMP_ON_CS       0x1F
#define LUT_DDR             0X20

#define ADDR_24BITS         0x18
#define ADDR_32BITS         0x20

/*
 * Calculate number of required PAD bits for LUT register.
 *
 * The pad stands for the number of IO lines [0:7].
 * For example, the octal read needs eight IO lines,
 * so you should use LUT_PAD(8). This macro
 * returns 3 i.e. use eight (2^3) IP lines for read.
 */
#define LUT_PAD(x) (fls(x) - 1)


/* FSPI controller specific */
typedef struct {
    snor_ctrl_t     ctrl;
    paddr_t         pbase;
    uintptr_t       vbase;
    uintptr_t       amba_vbase;
    uint32_t        amba_base;
    uint32_t        ahb_bufsz;
    uint32_t        amba_size;
    uint32_t        ip_only;          // IP command access only
    uint32_t        octcomb_en;       // Octal combination mode
    uint32_t        rxclksrc;         // sample clock source selection for flash reading
    uint16_t        seqs;             // command sequence size/step
    uint16_t        mseqs;            // maximum command sequence count
    uint16_t        rdp;              // Rx buffer depth
    uint16_t        tdp;              // Tx buffer depth
    uint32_t        *sigtbl;          // command signature table
    uint32_t        *ccount;          // command count
    int             irq;
    int             iid;
    sem_t           *sem;
} nxp_fspi_t;

extern int32_t f3s_nxp_fspi_open(f3s_socket_t *socket, const uint32_t flags);

#endif /* _NXP_FSPI_H */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
