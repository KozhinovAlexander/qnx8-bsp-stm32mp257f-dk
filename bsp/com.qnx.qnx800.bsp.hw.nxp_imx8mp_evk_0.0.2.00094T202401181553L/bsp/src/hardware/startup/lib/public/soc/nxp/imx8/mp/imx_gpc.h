/*
 * $QNXLicenseC:
 * Copyright 2022 BlackBerry Limited.
 * Copyright 2022 NXP
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

#ifndef IMX_GPC_H_
#define IMX_GPC_H_

/* GPC registers, offset from base address */
#define IMX_GPC_LPCR_A53_BSC                        0x00    /* Basic Low power control register of A53 platform */
#define IMX_GPC_LPCR_A53_AD                         0x04    /* Advanced Low power control register of A53 platform */
#define IMX_GPC_LPCR_M7                             0x08    /* Low power control register of CPU1 */
#define IMX_GPC_SLPCR                               0x14    /* System low power control register */
#define IMX_GPC_MST_CPU_MAPPING                     0x18    /* MASTER LPM Handshake */
#define IMX_GPC_MLPCR                               0x20    /* Memory low power control register */
#define IMX_GPC_PGC_ACK_SEL_A53                     0x24    /* PGC acknowledge signal selection of A53 platform */
#define IMX_GPC_PGC_ACK_SEL_M7                      0x28    /* PGC acknowledge signal selection of M7 platform */
#define IMX_GPC_MISC                                0x2C    /* GPC Miscellaneous register */
#define IMX_GPC_IMR1_CORE0_A53                      0x30    /* IRQ masking register 1 of A53 core0 */
#define IMX_GPC_IMR2_CORE0_A53                      0x34    /* IRQ masking register 2 of A53 core0 */
#define IMX_GPC_IMR3_CORE0_A53                      0x38    /* IRQ masking register 3 of A53 core0 */
#define IMX_GPC_IMR4_CORE0_A53                      0x3C    /* IRQ masking register 4 of A53 core0 */
#define IMX_GPC_IMR5_CORE0_A53                      0x40    /* IRQ masking register 5 of A53 core0 */
#define IMX_GPC_IMR1_CORE1_A53                      0x44    /* IRQ masking register 1 of A53 core1 */
#define IMX_GPC_IMR2_CORE1_A53                      0x48    /* IRQ masking register 2 of A53 core1 */
#define IMX_GPC_IMR3_CORE1_A53                      0x4C    /* IRQ masking register 3 of A53 core1 */
#define IMX_GPC_IMR4_CORE1_A53                      0x50    /* IRQ masking register 4 of A53 core1 */
#define IMX_GPC_IMR5_CORE1_A53                      0x54    /* IRQ masking register 5 of A53 core1 */
#define IMX_GPC_IMR1_M7                             0x58    /* IRQ masking register 1 of M7 */
#define IMX_GPC_IMR2_M7                             0x5C    /* IRQ masking register 2 of M7 */
#define IMX_GPC_IMR3_M7                             0x60    /* IRQ masking register 3 of M7 */
#define IMX_GPC_IMR4_M7                             0x64    /* IRQ masking register 4 of M7 */
#define IMX_GPC_IMR5_M7                             0x68    /* IRQ masking register 5 of M7 */
#define IMX_GPC_ISR1_A53                            0x80    /* IRQ status register 1 of A53 */
#define IMX_GPC_ISR2_A53                            0x84    /* IRQ status register 2 of A53 */
#define IMX_GPC_ISR3_A53                            0x88    /* IRQ status register 3 of A53 */
#define IMX_GPC_ISR4_A53                            0x8C    /* IRQ status register 4 of A53 */
#define IMX_GPC_ISR5_A53                            0x90    /* IRQ status register 5 of A53 */
#define IMX_GPC_ISR1_M7                             0x94    /* IRQ status register 1 of M7 */
#define IMX_GPC_ISR2_M7                             0x98    /* IRQ status register 2 of M7 */
#define IMX_GPC_ISR3_M7                             0x9C    /* IRQ status register 3 of M7 */
#define IMX_GPC_ISR4_M7                             0xA0    /* IRQ status register 4 of M7 */
#define IMX_GPC_ISR5_M7                             0xA4    /* IRQ status register 5 of M7 */
#define IMX_GPC_CPU_PGC_SW_PUP_REQ                  0xD0    /* CPU PGC software power up trigger */
#define IMX_GPC_MIX_PGC_SW_PUP_REQ                  0xD4    /* MIX PGC software power up trigger */
#define IMX_GPC_PU_PGC_SW_PUP_REQ                   0xD8    /* PU PGC software up trigger */
#define IMX_GPC_CPU_PGC_SW_PDN_REQ                  0xDC    /* CPU PGC software down trigger */
#define IMX_GPC_MIX_PGC_SW_PDN_REQ                  0xE0    /* MIX PGC software power down trigger */
#define IMX_GPC_PU_PGC_SW_PDN_REQ                   0xE4    /* PU PGC software down trigger */
#define IMX_GPC_CPU_PGC_PUP_STATUS1                 0x108   /* CPU PGC software up trigger status1 */
#define IMX_GPC_A53_MIX_PGC_PUP_STATUS(n)           (0x10C + (4 * (n))) /* A53 MIX software up trigger status register */
#define IMX_GPC_M7_MIX_PGC_PUP_STATUS(n)            (0x118 + (4 * (n))) /* M7 MIX software up trigger status register */
#define IMX_GPC_A53_PU_PGC_PUP_STATUS(n)            (0x124 + (4 * (n))) /* A53 PU software up trigger status register */
#define IMX_GPC_M7_PU_PGC_PUP_STATUS(n)             (0x130 + (4 * (n))) /* M7 PU PGC software up trigger status register */
#define IMX_PC_CPU_PGC_PDN_STATUS1                  0x13C   /* CPU PGC software dn trigger status1 */
#define IMX_GPC_A53_MIX_PGC_PDN_STATUS(n)           (0x140 + (4 * (n))) /* A53 MIX software down trigger status register */
#define IMX_GPC_M7_MIX_PGC_PDN_STATUS(n)            (0x14C + (4 * (n))) /* M7 MIX PGC software power down trigger status register */
#define IMX_GPC_A53_PU_PGC_PDN_STATUS(n)            (0x158 + (4 * (n))) /* A53 PU PGC software down trigger status */
#define IMX_GPC_M7_PU_PGC_PDN_STATUS(n)             (0x164 + (4 * (n))) /* M7 PU PGC software down trigger status */
#define IMX_GPC_A53_MIX_PDN_FLG                     0x170   /* A53 MIX PDN FLG */
#define IMX_GPC_A53_PU_PDN_FLG                      0x174   /* A53 PU PDN FLG */
#define IMX_GPC_M7_MIX_PDN_FLG                      0x178   /* M7 MIX PDN FLG */
#define IMX_GPC_M7_PU_PDN_FLG                       0x17C   /* M7 PU PDN FLG */
#define IMX_GPC_LPCR_A53_BSC2                       0x180   /* Basic Low power control register of A53 platform */
#define IMX_GPC_PU_PWRHSK                           0x190   /* Power handshake register */
#define IMX_GPC_IMR1_CORE2_A53                      0x194   /* IRQ masking register 1 of A53 core2 */
#define IMX_GPC_IMR2_CORE2_A53                      0x198   /* IRQ masking register 2 of A53 core2 */
#define IMX_GPC_IMR3_CORE2_A53                      0x19C   /* IRQ masking register 3 of A53 core2 */
#define IMX_GPC_IMR4_CORE2_A53                      0x1A0   /* IRQ masking register 4 of A53 core2 */
#define IMX_GPC_IMR5_CORE2_A53                      0x1A4   /* IRQ masking register 5 of A53 core2 */
#define IMX_GPC_IMR1_CORE3_A53                      0x1A8   /* IRQ masking register 1 of A53 core3 */
#define IMX_GPC_IMR2_CORE3_A53                      0x1AC   /* IRQ masking register 2 of A53 core3 */
#define IMX_GPC_IMR3_CORE3_A53                      0x1B0   /* IRQ masking register 3 of A53 core3 */
#define IMX_GPC_IMR4_CORE3_A53                      0x1B4   /* IRQ masking register 4 of A53 core3 */
#define IMX_GPC_IMR5_CORE3_A53                      0x1B8   /* IRQ masking register 5 of A53 core3 */
#define IMX_GPC_ACK_SEL_A53_PU                      0x1BC   /* PGC acknowledge signal selection of A53 platform for PUs */
#define IMX_GPC_ACK_SEL_A53_PU1                     0x1C0   /* PGC acknowledge signal selection of A53 platform for PUs */
#define IMX_GPC_ACK_SEL_M7_PU                       0x1C4   /* PGC acknowledge signal selection of M7 platform for PUs */
#define IMX_GPC_ACK_SEL_M7_PU1                      0x1C8   /* PGC acknowledge signal selection of M7 platform for PUs */
#define IMX_GPC_PGC_CPU_A53_MAPPING                 0x1CC   /* PGC CPU A53 mapping */
#define IMX_GPC_PGC_CPU_M7_MAPPING                  0x1D0   /* PGC CPU M7 mapping */
#define IMX_GPC_SLT_CFG(n)                          (0x200 + (4 * (n))) /* Slot configure register for CPUs */
#define IMX_GPC_SLTn_CFG_PU(n)                      (0x280 + (8 * (n))) /* Slot configure register for PGC PUs */
#define IMX_GPC_SLTn_CFG_PU1(n)                     (0x284 + (8 * (n))) /* Extended slot configure register for PGC PUs */

/* Requests: PUP, PDN */
#define IMX_GPC_PU_PGC_SW_REQ_MIPI_DSI_SHIFT        (0)
#define IMX_GPC_PU_PGC_SW_REQ_MIPI_DSI_MASK         (1 << IMX_GPC_PU_PGC_SW_REQ_MIPI_DSI_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_PCIE_SHIFT            (1)
#define IMX_GPC_PU_PGC_SW_REQ_PCIE_MASK             (1 << IMX_GPC_PU_PGC_SW_REQ_PCIE_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_USB_OTG1_SHIFT        (2)
#define IMX_GPC_PU_PGC_SW_REQ_USB_OTG1_MASK         (1 << IMX_GPC_PU_PGC_SW_REQ_USB_OTG1_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_USB_OTG2_SHIFT        (3)
#define IMX_GPC_PU_PGC_SW_REQ_USB_OTG2_MASK         (1 << IMX_GPC_PU_PGC_SW_REQ_USB_OTG2_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_DDR1_SHIFT            (5)
#define IMX_GPC_PU_PGC_SW_REQ_DDR1_MASK             (1 << IMX_GPC_PU_PGC_SW_REQ_DDR1_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_GPU_2D_SHIFT          (6)
#define IMX_GPC_PU_PGC_SW_REQ_GPU_2D_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_GPU_2D_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_GPUMIX_SHIFT          (7)
#define IMX_GPC_PU_PGC_SW_REQ_GPUMIX_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_GPUMIX_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_VPUMIX_SHIFT          (8)
#define IMX_GPC_PU_PGC_SW_REQ_VPUMIX_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_VPUMIX_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_GPU_3D_SHIFT          (9)
#define IMX_GPC_PU_PGC_SW_REQ_GPU_3D_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_GPU_3D_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_DISPMIX_SHIFT         (10)
#define IMX_GPC_PU_PGC_SW_REQ_DISPMIX_MASK          (1 << IMX_GPC_PU_PGC_SW_REQ_DISPMIX_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_G1_SHIFT          (11)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_G1_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_VPU_G1_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_G2_SHIFT          (12)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_G2_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_VPU_G2_SHIFT)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_H1_SHIFT          (13)
#define IMX_GPC_PU_PGC_SW_REQ_VPU_H1_MASK           (1 << IMX_GPC_PU_PGC_SW_REQ_VPU_H1_SHIFT)

/* GPC PGC registers, offset from base address */
#define IMX_GPC_PGC_nCTRL(n)                        (0x800 + (64 * (n))) /* GPC PGC Control Register */
#define IMX_GPC_PGC_nPUPSCR(n)                      (0x804 + (64 * (n))) /* GPC PGC Up Sequence Control Register */
#define IMX_GPC_PGC_nPDNSCR(n)                      (0x808 + (64 * (n))) /* GPC PGC Down Sequence Control Register */
#define IMX_GPC_PGC_nSR(n)                          (0x80C + (64 * (n))) /* GPC PGC Status Register */

/*
 * IMX_GPC_PU_PWRHSK Control Divider Register bits *
 */
#define IMX_GPC_PU_PWRHSK_DDR1_CORE_CSYSREQ_SHIFT           (0)          /* DDR1 controller Hardware Low-Power Request */
#define IMX_GPC_PU_PWRHSK_DDR1_CORE_CSYSREQ_MASK            (0x01 << 0)
#define IMX_GPC_PU_PWRHSK_DDR1_AXI_CSYSREQ_SHIFT            (1)          /* DDR1 AXI Low-Power Request */
#define IMX_GPC_PU_PWRHSK_DDR1_AXI_CSYSREQ_MASK             (0x01 << 1)
#define IMX_GPC_PU_PWRHSK_NOC2AUDIOMIX_PWRDNREQN_SHIFT      (4)          /* Main noc 2 audiomix power down request */
#define IMX_GPC_PU_PWRHSK_NOC2AUDIOMIX_PWRDNREQN_MASK       (0x01 << 4)
#define IMX_GPC_PU_PWRHSK_NOC2SUPERMIX_PWRDNREQN_SHIFT      (5)          /* DISPMIX ADB400 power down request. */
#define IMX_GPC_PU_PWRHSK_NOC2SUPERMIX_PWRDNREQN_MASK       (0x01 << 5)
#define IMX_GPC_PU_PWRHSK_SUPERMIX2NOC_PWRDNREQN_SHIFT      (6)          /* Supermix 2 noc adbs power down request. */
#define IMX_GPC_PU_PWRHSK_SUPERMIX2NOC_PWRDNREQN_MASK       (0x01 << 6)
#define IMX_GPC_PU_PWRHSK_MLMIX_ADBS_PWRDNREQN_SHIFT        (7)          /* Mlmix adbs power down request. */
#define IMX_GPC_PU_PWRHSK_MLMIX_ADBS_PWRDNREQN_MASK         (0x01 << 7)
#define IMX_GPC_PU_PWRHSK_NOC2MLMIX_PWRDNREQN_SHIFT         (8)          /* Main noc 2 mlmix power down request. */
#define IMX_GPC_PU_PWRHSK_NOC2MLMIX_PWRDNREQN_MASK          (0x01 << 8)
#define IMX_GPC_PU_PWRHSK_GPUMIX_NOC_ADBS_PWRDNREQN_SHIFT   (9)          /* Gpumix noc and adbs power down request. */
#define IMX_GPC_PU_PWRHSK_GPUMIX_NOC_ADBS_PWRDNREQN_MASK    (0x01 << 9)
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOC_PWRDNREQN_SHIFT        (10)         /* Vpumix noc power down request. */
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOC_PWRDNREQN_MASK         (0x01 << 10)
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOC_PWRDNREQN_SHIFT        (10)         /* Vpumix noc power down request. */
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOC_PWRDNREQN_MASK         (0x01 << 10)
#define IMX_GPC_PU_PWRHSK_NOC2DDRMIX_PWRDNREQN_SHIFT        (11)         /* Main noc 2 ddrmix power down request. */
#define IMX_GPC_PU_PWRHSK_NOC2DDRMIX_PWRDNREQN_MASK         (0x01 << 11)
#define IMX_GPC_PU_PWRHSK_NOC2HSIO_ADBS_PWRDNREQN_SHIFT     (12)         /* Main noc 2 hsio and adbs power down request. */
#define IMX_GPC_PU_PWRHSK_NOC2HSIO_ADBS_PWRDNREQN_MASK      (0x01 << 12)
#define IMX_GPC_PU_PWRHSK_HDMIMIX_NOC_PWRDNREQN_SHIFT       (13)         /* Hdmimix noc power down request. */
#define IMX_GPC_PU_PWRHSK_HDMIMIX_NOC_PWRDNREQN_MASK        (0x01 << 13)
#define IMX_GPC_PU_PWRHSK_MEDIAMIX_NOC_ADBS_PWRDNREQN_SHIFT (14)         /* Mediamix noc and adbs power down request. */
#define IMX_GPC_PU_PWRHSK_MEDIAMIX_NOC_ADBS_PWRDNREQN_MASK  (0x01 << 14)
#define IMX_GPC_PU_PWRHSK_AUDIOMIX_NOC_PWRDNREQN_SHIFT      (15)         /* Audiomix noc power down request. */
#define IMX_GPC_PU_PWRHSK_AUDIOMIX_NOC_PWRDNREQN_MASK       (0x01 << 15)
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_LWPWACKN_SHIFT          (16)         /* DDR1 controller Hardware Low_Power ack. */
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_LWPWACKN_MASK           (0x01 << 16)
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_CLKACTIVE_SHIFT         (17)         /* DDR1 controller Hardware Low-Power Clock active. */
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_CLKACTIVE_MASK          (0x01 << 17)
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_REQACK_SHIFT            (18)         /* DDR1 AXI Low-Power Request ack. */
#define IMX_GPC_PU_PWRHSK_DDR1_CTRL_REQACK_MASK             (0x01 << 18)
#define IMX_GPC_PU_PWRHSK_DDR1_CACTIVE_SHIFT                (19)         /* DDR1 AXI Clock Active. */
#define IMX_GPC_PU_PWRHSK_DDR1_CACTIVE_MASK                 (0x01 << 19)
#define IMX_GPC_PU_PWRHSK_NOC2AUDIOMIX_PWDWNACKN_SHIFT      (20)         /* Main noc 2 audiomix power down ackn. */
#define IMX_GPC_PU_PWRHSK_NOC2AUDIOMIX_PWDWNACKN_MASK       (0x01 << 20)
#define IMX_GPC_PU_PWRHSK_NOC2SUPERMIX_ADBS_PWDWNACKN_SHIFT (21)         /* Main noc 2 Supermix adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_NOC2SUPERMIX_ADBS_PWDWNACKN_MASK  (0x01 << 21)
#define IMX_GPC_PU_PWRHSK_SUPERMIX2NOC_ADBS_PWDWNACKN_SHIFT (22)         /* Supermix 2 noc adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_SUPERMIX2NOC_ADBS_PWDWNACKN_MASK  (0x01 << 22)
#define IMX_GPC_PU_PWRHSK_MLMIX_ADBS_PWRDNACKN_SHIFT        (23)         /* Mlmix adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_MLMIX_ADBS_PWRDNACKN_MASK         (0x01 << 23)
#define IMX_GPC_PU_PWRHSK_NOC2MLMIX_PWDWNACKN_SHIFT         (24)         /* Main noc 2 mlmix power down ackn. */
#define IMX_GPC_PU_PWRHSK_NOC2MLMIX_PWDWNACKN_MASK          (0x01 << 24)
#define IMX_GPC_PU_PWRHSK_GPUMIX_NOC_ADBS_PWRDNACKN_SHIFT   (25)         /* Gpumix noc and adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_GPUMIX_NOC_ADBS_PWRDNACKN_MASK    (0x01 << 25)
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOX_PWDWNACKN_SHIFT        (26)         /* Vpumix noc power down ackn. */
#define IMX_GPC_PU_PWRHSK_VPUMIX_NOX_PWDWNACKN_MASK         (0x01 << 26)
#define IMX_GPC_PU_PWRHSK_NOC2DDRMIX_PWRDNACKN_SHIFT        (27)         /* Main noc 2 ddrmix power down ackn. */
#define IMX_GPC_PU_PWRHSK_NOC2DDRMIX_PWRDNACKN_MASK         (0x01 << 27)
#define IMX_GPC_PU_PWRHSK_NOC2HSIO_ADBS_PWDWNACKN_SHIFT     (28)         /* Main noc 2 hsio and adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_NOC2HSIO_ADBS_PWDWNACKN_MASK      (0x01 << 28)
#define IMX_GPC_PU_PWRHSK_HDMIMIX_NOC_PWRDNACKN_SHIFT       (29)         /* Hdmimix noc power down ackn. */
#define IMX_GPC_PU_PWRHSK_HDMIMIX_NOC_PWRDNACKN_MASK        (0x01 << 29)
#define IMX_GPC_PU_PWRHSK_MEDIAMIX_NOC_ADBS_PWRDNACKN_SHIFT (30)         /* Mediamix noc and adbs power down ackn. */
#define IMX_GPC_PU_PWRHSK_MEDIAMIX_NOC_ADBS_PWRDNACKN_MASK  (0x01 << 30)
#define IMX_GPC_PU_PWRHSK_AUDIOMIX_PWRDNACKN_SHIFT          (31)         /* Audiomix noc power down ackn. */
#define IMX_GPC_PU_PWRHSK_AUDIOMIX_PWRDNACKN_MASK           (0x01 << 31)

/* PU type power domain */
#define IMX_GPC_PGC_PU_nCTRL(n)                     (0xB00 + (64 * (n))) /* GPC PGC Control Register */
#define IMX_GPC_PGC_PU_nPUPSCR(n)                   (0xB04 + (64 * (n))) /* GPC PGC Up Sequence Control Register */
#define IMX_GPC_PGC_PU_nPDNSCR(n)                   (0xB08 + (64 * (n))) /* GPC PGC Down Sequence Control Register */
#define IMX_GPC_PGC_PU_nSR(n)                       (0xB0C + (64 * (n))) /* GPC PGC Status Register */
#define IMX_GPC_PGC_PU_nCTRL_PCR_SHIFT              (0)
#define IMX_GPC_PGC_PU_nCTRL_PCR_MASK               (0x01)

/* PU power domain indexes - see IMX_GPC_PGC_PU_nCTRL(n) */
#define IMX_GPC_PGC_PU_MIPI_DSI_PHY                 0
#define IMX_GPC_PGC_PU_PCIE1_PHY                    1
#define IMX_GPC_PGC_PU_USB_OTG1                     2
#define IMX_GPC_PGC_PU_USB_OTG2                     3
#define IMX_GPC_PGC_PU_ML                           4
#define IMX_GPC_PGC_PU_AUDIO                        5
#define IMX_GPC_PGC_PU_GPU_2D                       6
#define IMX_GPC_PGC_PU_GPUMIX                       7
#define IMX_GPC_PGC_PU_VPUMIX                       8
#define IMX_GPC_PGC_PU_GPU_3D                       9
#define IMX_GPC_PGC_PU_MEDIMIX                     10
#define IMX_GPC_PGC_PU_VPU_G1                      11
#define IMX_GPC_PGC_PU_VPU_G2                      12
#define IMX_GPC_PGC_PU_VPU_H1                      13

/* PU A53 and M7 mapping - see GPC_PGC_CPU_A53_MAPPING and GPC_PGC_CPU_M7_MAPPING regs */
#define IMX_GPC_PGC_CPU_MIX0_SUPERMIXM7_DOMAIN_SHIFT (0)
#define IMX_GPC_PGC_CPU_MIX0_SUPERMIXM7_DOMAIN_MASK  (0x01 << 0)
#define IMX_GPC_PGC_CPU_MIX1_NOC_DOMAIN_SHIFT        (1)
#define IMX_GPC_PGC_CPU_MIX1_NOC_DOMAIN_MASK         (0x01 << 1)
#define IMX_GPC_PGC_CPU_MIPI_PHY1_DOMAIN_SHIFT       (2)
#define IMX_GPC_PGC_CPU_MIPI_PHY1_DOMAIN_MASK        (0x01 << 2)
#define IMX_GPC_PGC_CPU_PCIE_PHY_DOMAIN_SHIFT        (3)
#define IMX_GPC_PGC_CPU_PCIE_PHY_DOMAIN_MASK         (0x01 << 3)
#define IMX_GPC_PGC_CPU_USB1_PHY_DOMAIN_SHIFT        (4)
#define IMX_GPC_PGC_CPU_USB1_PHY_DOMAIN_MASK         (0x01 << 4)
#define IMX_GPC_PGC_CPU_USB2_PHY_DOMAIN_SHIFT        (5)
#define IMX_GPC_PGC_CPU_USB2_PHY_DOMAIN_MASK         (0x01 << 5)
#define IMX_GPC_PGC_CPU_MLMIX_DOMAIN_SHIFT           (6)
#define IMX_GPC_PGC_CPU_MLMIX_DOMAIN_MASK            (0x01 << 6)
#define IMX_GPC_PGC_CPU_AUDIOMIX_DOMAIN_SHIFT        (7)
#define IMX_GPC_PGC_CPU_AUDIOMIX_DOMAIN_MASK         (0x01 << 7)
#define IMX_GPC_PGC_CPU_GPU_2D_DOMAIN_SHIFT          (8)
#define IMX_GPC_PGC_CPU_GPU_2D_DOMAIN_MASK           (0x01 << 8)
#define IMX_GPC_PGC_CPU_GPU_SHARE_LOGIC_DOMAIN_SHIFT (9)
#define IMX_GPC_PGC_CPU_GPU_SHARE_LOGIC_DOMAIN_MASK  (0x01 << 9)
#define IMX_GPC_PGC_CPU_VPUMIX_SHARE_LOGIC_DOMAIN_SHIFT (10)
#define IMX_GPC_PGC_CPU_VPUMIX_SHARE_LOGIC_DOMAIN_MASK  (0x01 << 10)
#define IMX_GPC_PGC_CPU_GPU3D_DOMAIN_SHIFT           (11)
#define IMX_GPC_PGC_CPU_GPU3D_DOMAIN_MASK            (0x01 << 11)
#define IMX_GPC_PGC_CPU_MEDIAMIX_DOMAIN_SHIFT        (12)
#define IMX_GPC_PGC_CPU_MEDIAMIX_DOMAIN_MASK         (0x01 << 12)
#define IMX_GPC_PGC_CPU_VPU_G1_DOMAIN_SHIFT          (13)
#define IMX_GPC_PGC_CPU_VPU_G1_DOMAIN_MASK           (0x01 << 13)
#define IMX_GPC_PGC_CPU_VPU_G2_DOMAIN_SHIFT          (14)
#define IMX_GPC_PGC_CPU_VPU_G2_DOMAIN_MASK           (0x01 << 14)
#define IMX_GPC_PGC_CPU_VPU_VC8K_DOMAIN_SHIFT        (15)
#define IMX_GPC_PGC_CPU_VPU_VC8K_DOMAIN_MASK         (0x01 << 15)
#define IMX_GPC_PGC_CPU_HDMIMIX_DOMAIN_SHIFT         (16)
#define IMX_GPC_PGC_CPU_HDMIMIX_DOMAIN_MASK          (0x01 << 16)
#define IMX_GPC_PGC_CPU_HDMI_PHY_DOMAIN_SHIFT        (17)
#define IMX_GPC_PGC_CPU_HDMI_PHY_DOMAIN_MASK         (0x01 << 17)
#define IMX_GPC_PGC_CPU_MIPI_PHY2_DOMAIN_SHIFT       (18)
#define IMX_GPC_PGC_CPU_MIPI_PHY2_DOMAIN_MASK        (0x01 << 18)
#define IMX_GPC_PGC_CPU_HSIOMIX_DOMAIN_SHIFT         (19)
#define IMX_GPC_PGC_CPU_HSIOMIX_DOMAIN_MASK          (0x01 << 19)
#define IMX_GPC_PGC_CPU_MEDIA_ISP_DWP_DOMAIN_SHIFT   (20)
#define IMX_GPC_PGC_CPU_MEDIA_ISP_DWP_DOMAIN_MASK    (0x01 << 20)
#define IMX_GPC_PGC_CPU_DDRMIX_DOMAIN_SHIFT          (21)
#define IMX_GPC_PGC_CPU_DDRMIX_DOMAIN_MASK           (0x01 << 21)

/* ATF PU power domain indexes - imx_sec_firmware_psci function */
/* hsio ss */
#define ATF_PU_HSIOMIX                              0UL
#define ATF_PU_PCIE_PHY                             1UL
#define ATF_PU_USB1_PHY                             2UL
#define ATF_PU_USB2_PHY                             3UL
#define ATF_PU_MLMIX                                4UL
#define ATF_PU_AUDIOMIX                             5UL
/* gpu ss */
#define ATF_PU_GPUMIX                               6UL
#define ATF_PU_GPU2D                                7UL
#define ATF_PU_GPU3D                                8UL
/* vpu ss */
#define ATF_PU_VPUMIX                               9UL
#define ATF_PU_VPU_G1                               10UL
#define ATF_PU_VPU_G2                               11UL
#define ATF_PU_VPU_H1                               12UL
/* media ss */
#define ATF_PU_MEDIAMIX                             13UL
#define ATF_PU_MEDIAMIX_ISPDWP                      14UL
#define ATF_PU_MIPI_PHY1                            15UL
#define ATF_PU_MIPI_PHY2                            16UL
/* HDMI ss */
#define ATF_PU_HDMIMIX                              17UL
#define ATF_PU_HDMI_PHY                             18UL
#define ATF_PU_DDRMIX                               19UL

#endif
