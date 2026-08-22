/*
 * Some modifications Copyright 2014, 2016, 2021, 2022 BlackBerry Limited.
 ************************************************************************************
 *  Copyright (C) 2010-2011 Renesas Electronics Corporation. All rights reserved.   *
 ************************************************************************************
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

#define _BITFIELD32L(__start_bit,__value) ((__value) << (__start_bit))
#define _BITFIELD16L(__start_bit,__value) ((__value) << (__start_bit))
#define _BITFIELD8L(__start_bit,__value)  ((__value) << (__start_bit))

#if defined(VARIANT_rzg2l)
/* RZG2L base addresses */
#define RZG2L_SCIFA0_BASE           0x1004B800
#define RZG2L_SCIFA1_BASE           0x1004BC00
#define RZG2L_SCIFA2_BASE           0x1004C000
#define RZG2L_SCIFA3_BASE           0x1004C400
#define RZG2L_SCIFA4_BASE           0x1004C800

/* RZG2L SCIFA register addresses */
#define SCIFA_SCASMR_OFF            0x0
#define SCIFA_SCABRR_OFF            0x2
#define SCIFA_SCASCR_OFF            0x4
#define SCIFA_SCAFTDR_OFF           0x6
#define SCIFA_SCASSR_OFF            0x8
#define SCIFA_SCAFRDR_OFF           0xA
#define SCIFA_SCAFCR_OFF            0xC
#define SCIFA_SCAFDR_OFF            0xE
#define SCIFA_SCASPTR_OFF           0x10
#define SCIFA_SCALSR_OFF            0x12
#define SCIFA_SCASEMR_OFF           0x14
#define SCIFA_SCAFTCR_OFF           0x16

/* RZG2L SCIFA Specific FDR Bits */
#define SCIFA_FIFO_LEN         16
#define SCIFA_SCAFDR_TX(x)     ((x>>8)&0x1f)
#define SCIFA_SCAFDR_RX(x)     (x&0x1f)

#define SCIFA_SCASSR_ORER      (1<<9)

#define SCIFA_SCASCR_EXT_CLK   (1<<1)
#define SCIFA_SCASCR_TEIE      (2<<1)

/* Some RZG2L SCIFA Specific FCR Bits */
#define SCIFA_SCAFCR_RSTRG_M   (7 << 8)
#define SCIFA_SCAFCR_RSTRG_15  (0 << 8)
#define SCIFA_SCAFCR_RSTRG_1   (1 << 8)
#define SCIFA_SCAFCR_RSTRG_4   (2 << 8)
#define SCIFA_SCAFCR_RSTRG_6   (3 << 8)
#define SCIFA_SCAFCR_RSTRG_8   (4 << 8)
#define SCIFA_SCAFCR_RSTRG_10  (5 << 8)
#define SCIFA_SCAFCR_RSTRG_12  (6 << 8)
#define SCIFA_SCAFCR_RSTRG_14  (7 << 8)
#define SCIFA_SCAFCR_RTRG_M    (3 << 6)
#define SCIFA_SCAFCR_RTRG_1    (0 << 6)
#define SCIFA_SCAFCR_RTRG_4    (1 << 6)
#define SCIFA_SCAFCR_RTRG_8    (2 << 6)
#define SCIFA_SCAFCR_RTRG_14   (3 << 6)
#define SCIFA_SCAFCR_TTRG_M    (3 << 4)
#define SCIFA_SCAFCR_TTRG_8    (0 << 4)
#define SCIFA_SCAFCR_TTRG_4    (1 << 4)
#define SCIFA_SCAFCR_TTRG_2    (2 << 4)
#define SCIFA_SCAFCR_TTRG_0    (3 << 4)
#define SCIFA_SCAFCR_TFRST     (1 << 2)
#define SCIFA_SCAFCR_RFRST     (1 << 1)

#else

/* RZG1H base addresses */
#define RZG1H_SCIFA0_BASE           0xE6C40000
#define RZG1H_SCIFA1_BASE           0xE6C50000
#define RZG1H_SCIFA2_BASE           0xE6C60000
#define RZG1H_SCIFA3_BASE           0xE6C70000
#define RZG1H_SCIFA4_BASE           0xE6C78000
#define RZG1H_SCIFA5_BASE           0xE6C80000

/* RZG1H SCIFA register addresses */
#define SCIFA_SCASMR_OFF              0x0
#define SCIFA_SCABRR_OFF              0x4
#define SCIFA_SCASCR_OFF              0x8
#define SCIFA_SCATDSR_OFF             0xC
#define SCIFA_SCAFER_OFF             0x10
#define SCIFA_SCASSR_OFF             0x14
#define SCIFA_SCAFCR_OFF             0x18
#define SCIFA_SCAFDR_OFF             0x1c
#define SCIFA_SCAFTDR_OFF            0x20
#define SCIFA_SCAFRDR_OFF            0x24
#define SCIFA_SCAPCR_OFF             0x30
#define SCIFA_SCAPDR_OFF             0x34

/* RZG1H SCIFA Specific FDR Bits */
#define SCIFA_FIFO_LEN         64
#define SCIFA_SCAFDR_TX(x)     ((x>>8)&0x7f)
#define SCIFA_SCAFDR_RX(x)     (x&0x7f)

/* Some RZG1H SCIFA Specific SCASSR Bits that are
   different from  RCAR Common SSR Bits */
#define SCIFA_SCASSR_ORER           (1<<9)

/* Some RZG1H SCIFA Specific SCASCR Bits that are
   different from  RCAR Common SCR Bits */
#define SCIFA_SCASCR_ERIE       (1<<10)
#define SCIFA_SCASCR_DRIE       (1<<8)
#define SCIFA_SCASCR_CKE0       (1<<0)

/* Some RZG1H SCIFA Specific PCR Bits */
#define SCIFA_SCAPCR_RTSC      (1 << 4)
#define SCIFA_SCAPCR_CTSC      (1 << 3)
#define SCIFA_SCAPCR_SCKC      (1 << 2)
#define SCIFA_SCAPCR_RXDC      (1 << 1)
#define SCIFA_SCAPCR_TXDC      (1 << 0)

/* Some RZG1H SCIFA Specific SMR Bits */
#define SCIFA_SCASMR_CKEDG     (1 << 12)
#define SCIFA_SCASMR_SRC_M     (7 << 8)
#define SCIFA_SCASMR_SRC_1_16  (0 << 8)
#define SCIFA_SCASMR_SRC_1_5   (1 << 8)
#define SCIFA_SCASMR_SRC_1_7   (2 << 8)
#define SCIFA_SCASMR_SRC_1_11  (3 << 8)
#define SCIFA_SCASMR_SRC_1_13  (4 << 8)
#define SCIFA_SCASMR_SRC_1_17  (5 << 8)
#define SCIFA_SCASMR_SRC_1_19  (6 << 8)
#define SCIFA_SCASMR_SRC_1_27  (7 << 8)

/* Some RZG1H SCIFA Specific FCR Bits */
#define SCIFA_SCAFCR_RSTRG_M   (7 << 8)
#define SCIFA_SCAFCR_RSTRG_63  (0 << 8)
#define SCIFA_SCAFCR_RSTRG_1   (1 << 8)
#define SCIFA_SCAFCR_RSTRG_8   (2 << 8)
#define SCIFA_SCAFCR_RSTRG_16  (3 << 8)
#define SCIFA_SCAFCR_RSTRG_32  (4 << 8)
#define SCIFA_SCAFCR_RSTRG_48  (5 << 8)
#define SCIFA_SCAFCR_RSTRG_54  (6 << 8)
#define SCIFA_SCAFCR_RSTRG_60  (7 << 8)
#define SCIFA_SCAFCR_RTRG_M    (3 << 6)
#define SCIFA_SCAFCR_RTRG_1    (0 << 6)
#define SCIFA_SCAFCR_RTRG_16   (1 << 6)
#define SCIFA_SCAFCR_RTRG_32   (2 << 6)
#define SCIFA_SCAFCR_RTRG_48   (3 << 6)
#define SCIFA_SCAFCR_TTRG_M    (3 << 4)
#define SCIFA_SCAFCR_TTRG_32   (0 << 4)
#define SCIFA_SCAFCR_TTRG_16   (1 << 4)
#define SCIFA_SCAFCR_TTRG_2    (2 << 4)
#define SCIFA_SCAFCR_TTRG_0    (3 << 4)
#define SCIFA_SCAFCR_TFRST     (1 << 2)
#define SCIFA_SCAFCR_RFRST     (1 << 1)

#endif

/* R_CAR_H3 base addresses */
#define R_CAR_H3_SCIF_BASE0         0xE6E60000
#define R_CAR_H3_SCIF_BASE1         0xE6E68000
#define R_CAR_H3_SCIF_BASE2         0xE6E88000

/* RCAR SCIF register addresses */
#define SCIF_SCSMR_OFF              0x0
#define SCIF_SCBRR_OFF              0x4
#define SCIF_SCSCR_OFF              0x8
#define SCIF_SCFTDR_OFF             0xC
#define SCIF_SCFSR_OFF              0x10
#define SCIF_SCFRDR_OFF             0x14
#define SCIF_SCFCR_OFF              0x18
#define SCIF_SCFDR_OFF              0x1c
#define SCIF_SCSPTR_OFF             0x20
#define SCIF_SCLSR_OFF              0x24
#define SCIF_DL_OFF                 0x30
#define SCIF_CKS_OFF                0x34

/* RCAR SCIFB Registers */
#define SCIFB_SCBSMR_OFF            0x0         //  Serial mode register B
#define SCIFB_SCBBRR_OFF            0x4         //  Bit rate register B
#define SCIFB_SCBSCR_OFF            0x8         //  Serial control register B
#define SCIFB_SCBTDSR_OFF           0xC         //  Transmit data stop register B
#define SCIFB_SCBFER_OFF            0x10        //  FIFO error count register B
#define SCIFB_SCBSSR_OFF            0x14        //  Serial status register B
#define SCIFB_SCBFCR_OFF            0x18        //  FIFO control register B
#define SCIFB_SCBRCER_OFF           0x28        //  Receive data count compare register B
#define SCIFB_SCBMBR_OFF            0x2C        //  Multibyte set register B
#define SCIFB_SCBPCR_OFF            0x30        //  Serial port control register B
#define SCIFB_SCBPDR_OFF            0x34        //  Serial port data register B
#define SCIFB_SCBTFDR_OFF           0x38        //  Transmit FIFO data count register B
#define SCIFB_SCBRFDR_OFF           0x3C        //  Receive FIFO data count register B
#define SCIFB_SCBFTDR_OFF           0x40        //  Transmit FIFO data register B
#define SCIFB_SCBFRDR_OFF           0x60        //  Receive FIFO data register B
#define SCIFB_SCBSC2R_OFF           0x80        //  Serial control register2 B
#define SCIFB_SCBSS2R_OFF           0x84        //  Serial status register2 B
#define SCIFB_SCBRRCHR_OFF          0x90        //  Receive reading count H register B
#define SCIFB_SCBRRCLR_OFF          0x94        //  Receive reading count L register B
#define SCIFB_SCBRRCCHR_OFF         0x98        //  Receive reading count compare H register B
#define SCIFB_SCBRRCCLR_OFF         0x9C        //  Receive reading count compare L register B
#define SCIFB_SCBFIFO1_OFF          0xA8        //  Receive FIFO1 register B
#define SCIFB_SCBFIFO0_OFF          0xAC        //  Receive FIFO0 register B

/* RCAR HSCIF base addresses */
#define R_CAR_H3_HSCIF_BASE0        0xE6540000
#define R_CAR_H3_HSCIF_BASE1        0xE6550000
#define R_CAR_H3_HSCIF_BASE2        0xE6560000
#define R_CAR_H3_HSCIF_BASE3        0xE66A0000
#define R_CAR_H3_HSCIF_BASE4        0xE66B0000

/* RCAR HSCIF Registers */
#define HSCIF_HSSMR_OFF             (0x0)
#define HSCIF_HSBRR_OFF             (0x4)
#define HSCIF_HSSCR_OFF             (0x8)
#define HSCIF_HSFTDR_OFF            (0xC)
#define HSCIF_HSFSR_OFF             (0x10)
#define HSCIF_HSFRDR_OFF            (0x14)
#define HSCIF_HSFCR_OFF             (0x18)
#define HSCIF_HSFDR_OFF             (0x1c)
#define HSCIF_HSSPTR_OFF            (0x20)
#define HSCIF_HSLSR_OFF             (0x24)
#define HSCIF_DL_OFF                (0x30)
#define HSCIF_CKS_OFF               (0x34)
#define HSCIF_HSSRR_OFF             (0x40)      // Sampling rate register
#define HSCIF_HSRER_OFF             (0x44)      // Serial error register
#define HSCIF_HSRTGR_OFF            (0x50)      // RTS output active trigger count register
#define HSCIF_HSRTRGR_OFF           (0x54)      // Receive FIFO data count trigger register
#define HSCIF_HSTTRGR_OFF           (0x58)      // Transmit FIFO data count trigger register

/* RCAR Common SMR Bits */
#define SCIF_SCSMR_CA          (1<<7)
#define SCIF_SCSMR_CHR         (1<<6)
#define SCIF_SCSMR_PE          (1<<5)
#define SCIF_SCSMR_OE          (1<<4)
#define SCIF_SCSMR_STOP        (1<<3)
#define SCIF_SCSMR_CKS_M       (3<<0)
#define SCIF_SCSMR_CKS_1       (0<<0)
#define SCIF_SCSMR_CKS_4       (1<<0)
#define SCIF_SCSMR_CKS_16      (2<<0)
#define SCIF_SCSMR_CKS_64      (3<<0)

/* RCAR SCIFB Specific SMR Bits */
#define SCIFB_SCBSMR_CKEDG     (1 << 12)
#define SCIFB_SCBSMR_SRC_M     (7 << 8)
#define SCIFB_SCBSMR_SRC_1_16  (0 << 8)
#define SCIFB_SCBSMR_SRC_1_5   (1 << 8)
#define SCIFB_SCBSMR_SRC_1_7   (2 << 8)
#define SCIFB_SCBSMR_SRC_1_11  (3 << 8)
#define SCIFB_SCBSMR_SRC_1_13  (4 << 8)
#define SCIFB_SCBSMR_SRC_1_17  (5 << 8)
#define SCIFB_SCBSMR_SRC_1_19  (6 << 8)
#define SCIFB_SCBSMR_SRC_1_27  (7 << 8)

/* RCAR Common SCR Bits */
#define SCIF_SCSCR_TIE         (1<<7)
#define SCIF_SCSCR_RIE         (1<<6)
#define SCIF_SCSCR_TE          (1<<5)
#define SCIF_SCSCR_RE          (1<<4)
#define SCIF_SCSCR_TOIE        (1<<2)
#define SCIF_SCSCR_CKE_M       (0x3)
#define SCIF_SCSCR_CKE_1       (1<<1)

/* RCAR SCIF Specific SCR Bits */
#define SCIF_SCSCR_TEIE        (1<<11)
#define SCIF_SCSCR_REIE        (1<<3)

/* RCAR SCIFB Specific SCR Bits */
#define SCIFB_SCBSCR_TDRQE     (1 << 15)
#define SCIFB_SCBSCR_RDRQE     (1 << 14)
#define SCIFB_SCBSCR_RCEE      (1 << 13)
#define SCIFB_SCBSCR_TENDPOSE  (1 << 12)
#define SCIFB_SCBSCR_TSIE      (1 << 11)
#define SCIFB_SCBSCR_ERIE      (1 << 10)
#define SCIFB_SCBSCR_BRIE      (1 << 9)
#define SCIFB_SCBSCR_DRIE      (1 << 8)

/* RCAR Common FCR Bits */
#define SCIF_SCFCR_RTRG_M      (3 << 6)	/* RX Fifo trigger mask */
#define SCIF_SCFCR_TTRG_M      (3 << 4)	/* TX Fifo trigger mask */
#define SCIF_SCFCR_MCE         (1<<3)
#define SCIF_SCFCR_TFRST       (1<<2)
#define SCIF_SCFCR_RFRST       (1<<1)
#define SCIF_SCFCR_LOOP        (1<<0)

/* RCAR SCIF Specific FCR Bits */
#define SCIF_SCFCR_RSTRG_15    _BITFIELD16L(8, 0)
#define SCIF_SCFCR_RSTRG_1     _BITFIELD16L(8, 1)
#define SCIF_SCFCR_RSTRG_4     _BITFIELD16L(8, 2)
#define SCIF_SCFCR_RSTRG_6     _BITFIELD16L(8, 3)
#define SCIF_SCFCR_RSTRG_8     _BITFIELD16L(8, 4)
#define SCIF_SCFCR_RSTRG_10    _BITFIELD16L(8, 5)
#define SCIF_SCFCR_RSTRG_12    _BITFIELD16L(8, 6)
#define SCIF_SCFCR_RSTRG_14    _BITFIELD16L(8, 7)

#define SCIF_FIFO_LEN          16
#define SCIF_SCFCR_RTRG_1      _BITFIELD16L(6,0)
#define SCIF_SCFCR_RTRG_4      _BITFIELD16L(6,1)
#define SCIF_SCFCR_RTRG_8      _BITFIELD16L(6,2)
#define SCIF_SCFCR_RTRG_E      _BITFIELD16L(6,3)
#define SCIF_SCFCR_TTRG_E      _BITFIELD16L(4,3)
#define SCIF_SCFCR_TTRG_0      _BITFIELD16L(4,3)
#define SCIF_SCFCR_TTRG_2      _BITFIELD16L(4,2)
#define SCIF_SCFCR_TTRG_4      _BITFIELD16L(4,1)
#define SCIF_SCFCR_TTRG_8      _BITFIELD16L(4,0)

/* RCAR SCIFB Specific FCR Bits */
#define SCIFB_SCBFCR_TSE       (1 << 15)
#define SCIFB_SCBFCR_TCRST     (1 << 14)
#define SCIFB_SCBFCR_RCE       (1 << 13)
#define SCIFB_SCBFCR_RCRST     (1 << 12)

#define SCIFB_SCBFCR_RSTRG_M   (7 << 8)
#define SCIFB_SCBFCR_RSTRG_255 (0 << 8)
#define SCIFB_SCBFCR_RSTRG_1   (1 << 8)
#define SCIFB_SCBFCR_RSTRG_32  (2 << 8)
#define SCIFB_SCBFCR_RSTRG_64  (3 << 8)
#define SCIFB_SCBFCR_RSTRG_128 (4 << 8)
#define SCIFB_SCBFCR_RSTRG_192 (5 << 8)
#define SCIFB_SCBFCR_RSTRG_224 (6 << 8)
#define SCIFB_SCBFCR_RSTRG_240 (7 << 8)

#define SCIFB_FIFO_LEN         256
#define SCIFB_SCBFCR_RTRG_M    (3 << 6)
#define SCIFB_SCBFCR_RTRG_1    (0 << 6)
#define SCIFB_SCBFCR_RTRG_16   (1 << 6)
#define SCIFB_SCBFCR_RTRG_32   (2 << 6)
#define SCIFB_SCBFCR_RTRG_48   (3 << 6)
#define SCIFB_SCBFCR_TTRG_M    (3 << 4)
#define SCIFB_SCBFCR_TTRG_32   (0 << 4)
#define SCIFB_SCBFCR_TTRG_16   (1 << 4)
#define SCIFB_SCBFCR_TTRG_2    (2 << 4)
#define SCIFB_SCBFCR_TTRG_0    (3 << 4)

/* RCAR HSCIF Specific FCR Bits */
#define HSCIF_FIFO_LEN         128

/* RCAR Common SSR/FSR Bits (The FSR was renamed to SSR for the SCIFA and SCIFB) */
#define SCIF_SCSSR_ER          (1<<7)
#define SCIF_SCSSR_TEND        (1<<6)
#define SCIF_SCSSR_TDFE        (1<<5)
#define SCIF_SCSSR_BRK         (1<<4)
#define SCIF_SCSSR_FER         (1<<3)
#define SCIF_SCSSR_PER         (1<<2)
#define SCIF_SCSSR_RDF         (1<<1)
#define SCIF_SCSSR_DR          (1<<0)

/* RCAR SCIF Specific SSR/FSR Bits
 * Note: The FSR was renamed to SSR for the SCIFA and SCIFB
 */
#define SH_SCIF_SCSSR_PERF(x)  ((x>>12)&0xf)
#define SH_SCIF_SCSSR_FERF(x)  ((x>>8)&0xf)

/* RCAR SCIFB Specific SSR/FSR Bits
 * Note: The FSR was renamed to SSR for the SCIFA and SCIFB
 */
#define SCIFB_SCBSSR_RCEF      (1 << 13)
#define SCIFB_SCBSSR_TENDPOS   (1 << 12)
#define SCIFB_SCBSSR_ORER      (1 << 9)
#define SCIFB_SCBSSR_TSF       (1 << 8)

/* RCAR SCIF Specific FDR Bits */
#define SCIF_SCFDR_TX(x)       ((x>>8)&0x1f)
#define SCIF_SCFDR_RX(x)       (x&0x1f)

/* RCAR SCIFB Specific FRDR/FTFR Bits 
 * Note: The FDR from the SCIF was split into separate TX and RX
 *       count registers for the SCIFA and SCIFB
 */
#define SCIFB_SCBFDR(x)        (x&0x1ff)

/* RCAR HSCIF Specific FDR Bits */
#define HSCIF_HSFDR_TX(x)      ((x >> 8) & 0xFF)
#define HSCIF_HSFDR_RX(x)      (x & 0xFF)

/* RCAR SCIF Specific SCLSR Bits
 * Note: This register was removed from the SCIFA and SCIFB,
 *       and the ORER bit was moved to the SCSSR register
 */
#define SCIF_SCLSR_ORER        (1<<0)

/* RCAR SCIF Specific PTR Bits
 * Note: The PCR and PDR registers are the SCIFB functional equivalant of the SCIF PTR
 */
#define SCIF_SCSPTR_RTSIO      (1<<7)
#define SCIF_SCSPTR_RTSDT      (1<<6)
#define SCIF_SCSPTR_CTSIO      (1<<5)
#define SCIF_SCSPTR_CTSDT      (1<<4)
#define SCIF_SCSPTR_SPB2IO     (1<<1)
#define SCIF_SCSPTR_SPB2DT     (1<<0)

/* RCAR SCIFB Specific PCR Bits 
 * Note: The PTR is the equivalant register on the SCIF
 */
#define SCIFB_SCBPCR_RTSC      (1 << 4)
#define SCIFB_SCBPCR_CTSC      (1 << 3)
#define SCIFB_SCBPCR_SCKC      (1 << 2)
#define SCIFB_SCBPCR_RXDC      (1 << 1)
#define SCIFB_SCBPCR_TXDC      (1 << 0)

/* RCAR SCIFB Specific PDR Bits
 * Note: The PTR is the equivalant register on the SCIF
 */
#define SCIFB_SCBPDR_RTSD      (1 << 4)
#define SCIFB_SCBPDR_SCKD      (1 << 2)
#define SCIFB_SCBPDR_TXDD      (1 << 0)

/*HSSRR*/
#define HSCIF_HSSRR_SRE        (1<<15)
#define HSCIF_HSSRR_SRCYC      (0x1F << 0)
