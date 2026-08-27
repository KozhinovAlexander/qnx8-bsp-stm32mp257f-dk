/*
 * $QNXLicenseC:
 * Copyright 2017, QNX Software Systems.
 * Copyright 2021, Renesas Electronics Corporation.
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
 * Renesas R-Car processor with ARMv8
 * Gen4 shared definitions: V3U, S4
 */

#ifndef __AARCH64_RCAR_GEN4_H_INCLUDED
#define __AARCH64_RCAR_GEN4_H_INCLUDED

/*
 * Product Register (PRR)
 */
#define RCAR_PRODUCT_REGISTER      0xFFF00044
#define RCAR_PRODUCT_ID(x)         (((x) & 0x7F00) >> 8)
 #define PRODUCT_ID_RCAR_S4             0x5A
 #define PRODUCT_ID_RCAR_V4H            0x5C
 #define PRODUCT_ID_RCAR_UNKNOWN        (~0)

#define RCAR_PRODUCT_REV(x)        ((x) & 0xFF)
 #define PRODUCT_S4_REV_1_0             0x00
 #define PRODUCT_S4_REV_1_1             0x01
 #define PRODUCT_S4_REV_1_2             0x02
 #define PRODUCT_V4H_REV_1_0            0x00
 #define PRODUCT_V4H_REV_1_1            0x01
 #define PRODUCT_V4H_REV_2_0            0x10
 #define PRODUCT_V4H_REV_2_1            0x11
 #define PRODUCT_REV_UNKNOWN           (~0)
/*
* Below PRODUCT_REV_xxx are deprecated, they are legacy from Rcar Gen3 and
* these definitions might not work for Rcar Gen4.
*/
 #define PRODUCT_REV_1_0                0x00
 #define PRODUCT_REV_1_1                0x01
 #define PRODUCT_REV_1_2                0x10
 #define PRODUCT_REV_1_3                0x11
 #define PRODUCT_REV_2_0                0x10
 #define PRODUCT_REV_3_0                0x20


#define MPIDR_A76_CLUSTER_ID(x)         (((x) >> 16) & 0xFF)
#define MPIDR_CLUSTER_ID(x)             (((x) >>  8) & 0xFF)
#define MIDR_CPU_ID(x)                  (((x) >>  4) & 0xFFF)
#define MIDR_CPU_A53_ID                 0xD03
#define MIDR_CPU_A55_ID                 0xD05
#define MIDR_CPU_A57_ID                 0xD07
#define MIDR_CPU_A76_ID                 0xD0B
#define RCAR_GEN4_CLUSTERS_NUM_MAX      4
#define RCAR_GEN4_CORES_NUM_MAX         8
#define RCAR_GEN4_MAX_CORE_PER_CLUSTER  2
#define RCAR_GEN4_PRODUCT_CAEN(x,y)     (((((x) >> 20) & 0xFFF) >> ((y) * 3)) & 3)
#define RCAR_V4H_CLUSTERS_NUM_MAX       2


/*--------------------------------------------------
 *
 * Macros is for Gen4 (V3U, S4, V4H)
 *
 *--------------------------------------------------*/

/*
 * PFC/GPIO
 */

/* Domain base address */
#define RCAR_GEN4_GPIO_DM0_BASE          0xE6050000
#define RCAR_GEN4_GPIO_DM1_BASE          0xE6052000
#define RCAR_GEN4_GPIO_DM2_BASE          0xE6054000
#define RCAR_GEN4_GPIO_DM3_BASE          0xE6056000
#define RCAR_GEN4_GPIO_SIZE              0x2000
#define RCAR_GEN4_GPIO_GP_SIZE           0x200

/* Group offset */
#define RCAR_V3U_GPIO_GP0                0x08000
#define RCAR_V3U_GPIO_GP1                0x00000
#define RCAR_V3U_GPIO_GP2                0x00800
#define RCAR_V3U_GPIO_GP3                0x08800
#define RCAR_V3U_GPIO_GP4                0x10000
#define RCAR_V3U_GPIO_GP5                0x10800
#define RCAR_V3U_GPIO_GP6                0x18000
#define RCAR_V3U_GPIO_GP7                0x18800
#define RCAR_V3U_GPIO_GP8                0x19000
#define RCAR_V3U_GPIO_GP9                0x19800
#define RCAR_V3U_GPIO_SYSGP              0x28000

#define RCAR_S4_GPIO_GP0                 0x00000
#define RCAR_S4_GPIO_GP1                 0x00800
#define RCAR_S4_GPIO_GP2                 0x01000
#define RCAR_S4_GPIO_GP3                 0x01800
#define RCAR_S4_GPIO_SYSGP0              0x28000
#define RCAR_S4_GPIO_GP4                 0x19D40000
#define RCAR_S4_GPIO_GP5                 0x19D40800
#define RCAR_S4_GPIO_GP6                 0x19D41000
#define RCAR_S4_GPIO_GP7                 0x19D41800
#define RCAR_S4_GPIO_SYSGP1              0x19D48000

#define RCAR_V4H_GPIO_GP0                0x00000
#define RCAR_V4H_GPIO_GP1                0x00800
#define RCAR_V4H_GPIO_GP2                0x08000
#define RCAR_V4H_GPIO_GP3                0x08800
#define RCAR_V4H_GPIO_GP4                0x10000
#define RCAR_V4H_GPIO_GP5                0x10800
#define RCAR_V4H_GPIO_GP6                0x11000
#define RCAR_V4H_GPIO_GP7                0x11800
#define RCAR_V4H_GPIO_GP8                0x18000
#define RCAR_V4H_GPIO_SYSGP              0x28000


/* Group operation offset */
#define RCAR_GEN4_GPIO_RW                0x0000
#define RCAR_GEN4_GPIO_SET               0x0200
#define RCAR_GEN4_GPIO_CLR               0x0400

/* Register offset for Group n */
#define RCAR_GEN4_GPIO_PMMR              0x000
#define RCAR_GEN4_GPIO_PMMER             0x004
#define RCAR_GEN4_GPIO_DM0PR             0x020
#define RCAR_GEN4_GPIO_DM1PR             0x024
#define RCAR_GEN4_GPIO_DM2PR             0x028
#define RCAR_GEN4_GPIO_DM3PR             0x02C
#define RCAR_GEN4_GPIO_GPSR              0x040
#define RCAR_GEN4_GPIO_IP0SR             0x060
#define RCAR_GEN4_GPIO_IP1SR             0x064
#define RCAR_GEN4_GPIO_IP2SR             0x068
#define RCAR_GEN4_GPIO_IP3SR             0x06C
#define RCAR_GEN4_GPIO_DRV0CTRL          0x080
#define RCAR_GEN4_GPIO_DRV1CTRL          0x084
#define RCAR_GEN4_GPIO_DRV2CTRL          0x088
#define RCAR_GEN4_GPIO_DRV3CTRL          0x08C
#define RCAR_GEN4_GPIO_POC               0x0A0
#define RCAR_GEN4_GPIO_PUEN              0x0C0
#define RCAR_GEN4_GPIO_PUD               0x0E0
#define RCAR_GEN4_GPIO_MODSEL            0x100
#define RCAR_GEN4_GPIO_TD0SEL            0x120
#define RCAR_GEN4_GPIO_TD1SEL            0x124
#define RCAR_GEN4_GPIO_BIP0SR            0x140
#define RCAR_GEN4_GPIO_BIP1SR            0x144
#define RCAR_GEN4_GPIO_BIP2SR            0x148
#define RCAR_GEN4_GPIO_BIP3SR            0x14C
#define RCAR_GEN4_GPIO_PSER              0x160
#define RCAR_GEN4_GPIO_PS0SR             0x164
#define RCAR_GEN4_GPIO_PS1SR             0x168
#define RCAR_GEN4_GPIO_IOINTSEL          0x180
#define RCAR_GEN4_GPIO_INOUTSEL          0x184
#define RCAR_GEN4_GPIO_OUTDT             0x188
#define RCAR_GEN4_GPIO_INDT              0x18C
#define RCAR_GEN4_GPIO_INTDT             0x190
#define RCAR_GEN4_GPIO_INTCLR            0x194
#define RCAR_GEN4_GPIO_INTMSK            0x198
#define RCAR_GEN4_GPIO_MSKCLR            0x19C
#define RCAR_GEN4_GPIO_POSNEG            0x1A0
#define RCAR_GEN4_GPIO_EDGLEVEL          0x1A4
#define RCAR_GEN4_GPIO_FILONOFF          0x1A8
#define RCAR_GEN4_GPIO_FILCLKSEL         0x1AC
#define RCAR_GEN4_GPIO_OUTDTSEL          0x1C0
#define RCAR_GEN4_GPIO_OUTDTH            0x1C4
#define RCAR_GEN4_GPIO_OUTDTL            0x1C8
#define RCAR_GEN4_GPIO_BOTHEDGE          0x1CC
#define RCAR_GEN4_GPIO_INEN              0x1D0

/* Register offset for SYS Group */
#define RCAR_GEN4_GPIO_PMMRSYS           0x000
#define RCAR_GEN4_GPIO_PMMERSYS          0x004
#define RCAR_GEN4_GPIO_DM0PRSYS          0x020
#define RCAR_GEN4_GPIO_DM1PRSYS          0x024
#define RCAR_GEN4_GPIO_DM2PRSYS          0x028
#define RCAR_GEN4_GPIO_DM3PRSYS          0x02C
#define RCAR_GEN4_GPIO_DRV0CTRLSYS       0x080
#define RCAR_GEN4_GPIO_DRV1CTRLSYS       0x084
#define RCAR_GEN4_GPIO_DRV2CTRLSYS       0x088
#define RCAR_GEN4_GPIO_PUENSYS           0x0C0
#define RCAR_GEN4_GPIO_PUDSYS            0x0E0
#define RCAR_GEN4_GPIO_FUSE_MON0         0x0C0
#define RCAR_GEN4_GPIO_FUSE_MON1         0x0C4
#define RCAR_GEN4_GPIO_FUSE_MON2         0x0C8
#define RCAR_GEN4_GPIO_FUSE_MON3         0x0CC
#define RCAR_GEN4_GPIO_FUSE_MON4         0x0D0
#define RCAR_GEN4_GPIO_FUSE_MON5         0x0D4
#define RCAR_GEN4_GPIO_FUSE_MON6         0x0D8
#define RCAR_GEN4_GPIO_FUSE_MON7         0x0DC
#define RCAR_GEN4_GPIO_FUSE_MON8         0x0E0
#define RCAR_GEN4_GPIO_FUSE_MON9         0x0E4
#define RCAR_GEN4_GPIO_FUSE_CTR          0x080
#define RCAR_GEN4_GPIO_LTM0_CTR          0x100
#define RCAR_GEN4_GPIO_LTM0_MON0         0x140
#define RCAR_GEN4_GPIO_LTM0_MON1         0x144
#define RCAR_GEN4_GPIO_LTM0_MON2         0x148
#define RCAR_GEN4_GPIO_LTM0_MON3         0x14C
#define RCAR_GEN4_GPIO_LTM0_MON4         0x150
#define RCAR_GEN4_GPIO_LTM1_CTR          0x180
#define RCAR_GEN4_GPIO_LTM1_MON0         0x1C0
#define RCAR_GEN4_GPIO_LTM1_MON1         0x1C4
#define RCAR_GEN4_GPIO_LTM1_MON2         0x1C8
#define RCAR_GEN4_GPIO_LTM1_MON3         0x1CC
#define RCAR_GEN4_GPIO_LTM1_MON4         0x1D0

/*
 * CPG
 */

/* Domain base address */
#define RCAR_GEN4_CPG_DM0_BASE      0xE6150000
#define RCAR_GEN4_CPG_DM1_BASE      0xE6154000
#define RCAR_GEN4_CPG_DM2_BASE      0xE6158000
#define RCAR_GEN4_CPG_DM3_BASE      0xE615C000
#define RCAR_GEN4_CPG_SIZE          0x4000

/* Group offset */
#define RCAR_GEN4_CPG_GP0            0x0000
#define RCAR_GEN4_CPG_GP1            0x0400
#define RCAR_GEN4_CPG_GP2            0x0800
#define RCAR_GEN4_CPG_GP3            0x0C00
#define RCAR_GEN4_CPG_GP4            0x1000
#define RCAR_GEN4_CPG_GP5            0x1400
#define RCAR_GEN4_CPG_GP6            0x1800
#define RCAR_GEN4_CPG_GP7            0x1C00
#define RCAR_GEN4_CPG_GP8            0x2000
#define RCAR_GEN4_CPG_GP9            0x2400
#define RCAR_GEN4_CPG_GPA            0x2800
#define RCAR_GEN4_CPG_GPB            0x2C00
#define RCAR_GEN4_CPG_GPC            0x3000
#define RCAR_GEN4_CPG_GPD            0x3400
#define RCAR_GEN4_CPG_GPE            0x3800
#define RCAR_GEN4_CPG_GPF            0x3C00

/* Register offset */
#define RCAR_GEN4_CPG_CPGWPR              0x0000
#define RCAR_GEN4_CPG_CPGWPCR             0x0004
#define RCAR_GEN4_CPG_RSSDER              0x0008
#define RCAR_GEN4_CPG_CLSTAT              0x0108
#define RCAR_GEN4_CPG_ATSTPOFFR           0x0278
#define RCAR_GEN4_CPG_RTSRAMCR            0x027C
#define RCAR_GEN4_CPG_BKBAPR              0x0280
#define RCAR_GEN4_CPG_FSRCHKRA(n)        (0x0400 + ((n) * 4))
#define RCAR_GEN4_CPG_FSRCHKRB(n)        (0x0480 + ((n) * 4))
#define RCAR_GEN4_CPG_FSRCHKSETR(n)      (0x0500 + ((n) * 4))
#define RCAR_GEN4_CPG_FSRCHKCLRR(n)      (0x0580 + ((n) * 4))
#define RCAR_GEN4_CPG_ECMERRINJR          0x0740
#define RCAR_GEN4_CPG_FRQCRA              0x0800
#define RCAR_GEN4_CPG_FRQCRB              0x0804
#define RCAR_GEN4_CPG_FRQCRC              0x0808
#define RCAR_GEN4_CPG_FRQCRD0             0x080C
#define RCAR_GEN4_CPG_FRQCRD1             0x0810
#define RCAR_GEN4_CPG_PLLECR              0x0820
#define RCAR_GEN4_CPG_PLL1CR0             0x0830
#define RCAR_GEN4_CPG_PLL1CR1             0x08B0
#define RCAR_GEN4_CPG_PLL1CR2             0x08B4
#define RCAR_GEN4_CPG_PLL2CR0             0x0834
#define RCAR_GEN4_CPG_PLL2CR1             0x08B8
#define RCAR_GEN4_CPG_PLL2CR2             0x08BC
#define RCAR_GEN4_CPG_PLL20CR             0x0834
#define RCAR_GEN4_CPG_PLL21CR             0x0838
#define RCAR_GEN4_CPG_PLL3CR0             0x083C
#define RCAR_GEN4_CPG_PLL3CR1             0x08C0
#define RCAR_GEN4_CPG_PLL3CR2             0x08C0
#define RCAR_GEN4_CPG_PLL30CR             0x083C
#define RCAR_GEN4_CPG_PLL31CR             0x0840
#define RCAR_GEN4_CPG_PLL4CR              0x0844
#define RCAR_GEN4_CPG_PLL5CR0             0x0848
#define RCAR_GEN4_CPG_PLL5CR1             0x08D0
#define RCAR_GEN4_CPG_PLL5CR2             0x08D4
#define RCAR_GEN4_CPG_PLL6CR0             0x084C
#define RCAR_GEN4_CPG_PLL6CR1             0x08D8
#define RCAR_GEN4_CPG_PLL6CR2             0x08DC
#define RCAR_GEN4_CPG_PLL1STPCR           0x0850
#define RCAR_GEN4_CPG_PLL20STPCR          0x0854
#define RCAR_GEN4_CPG_PLL21STPCR          0x0858
#define RCAR_GEN4_CPG_PLL30STPCR          0x085C
#define RCAR_GEN4_CPG_PLL31STPCR          0x0860
#define RCAR_GEN4_CPG_PLL4STPCR           0x0864
#define RCAR_GEN4_CPG_PLL5STPCR           0x0868
#define RCAR_GEN4_CPG_SD0CKCR             0x0870
#define RCAR_GEN4_CPG_SD0CKCR1            0x08A4
#define RCAR_GEN4_CPG_RPCCKCR             0x0874
#define RCAR_GEN4_CPG_CANFDCKCR           0x0878
#define RCAR_GEN4_CPG_MSOCKCR             0x087C
#define RCAR_GEN4_CPG_CSI0CKCR            0x0880
#define RCAR_GEN4_CPG_DSICKCR             0x0884
#define RCAR_GEN4_CPG_IPCCKCR             0x0888
#define RCAR_GEN4_CPG_FRAYCKCR            0x088C
#define RCAR_GEN4_CPG_POSTCKCR            0x0890
#define RCAR_GEN4_CPG_POST2CKCR           0x0894
#define RCAR_GEN4_CPG_POST3CKCR           0x0898
#define RCAR_GEN4_CPG_POST4CKCR           0x089C
#define RCAR_GEN4_CPG_OSCCKCR             0x08A0
#define RCAR_GEN4_CPG_CPGMTCR             0x08A4
#define RCAR_GEN4_CPG_Z0CKKSCR            0x08A8
#define RCAR_GEN4_CPG_Z1CKKSCR            0x08AC
#define RCAR_GEN4_CPG_RSW2CKCR            0x08E8
#define RCAR_GEN4_CPG_PLL1FBCKMCSR        0x0C00
#define RCAR_GEN4_CPG_PLL1FBCKMECR        0x0C04
#define RCAR_GEN4_CPG_PLL1FBCKMLCH        0x0C08
#define RCAR_GEN4_CPG_PLL1FBCKMLCL        0x0C0C
#define RCAR_GEN4_CPG_PLL1FBCKMCNT        0x0C10
#define RCAR_GEN4_CPG_PLL1FBCKMCNTE       0x0C14
#define RCAR_GEN4_CPG_PLL1FBCKMMDR        0x0C18
#define RCAR_GEN4_CPG_PLL2_0FBCKMCSR      0x0C20
#define RCAR_GEN4_CPG_PLL2_0FBCKMECR      0x0C24
#define RCAR_GEN4_CPG_PLL2_0FBCKMLCH      0x0C28
#define RCAR_GEN4_CPG_PLL2_0FBCKMLCL      0x0C2C
#define RCAR_GEN4_CPG_PLL2_0FBCKMCNT      0x0C30
#define RCAR_GEN4_CPG_PLL2_0FBCKMCNTE     0x0C34
#define RCAR_GEN4_CPG_PLL2_0FBCKMMDR      0x0C38
#define RCAR_GEN4_CPG_PLL2_1FBCKMCSR      0x0C40
#define RCAR_GEN4_CPG_PLL2_1FBCKMECR      0x0C44
#define RCAR_GEN4_CPG_PLL2_1FBCKMLCH      0x0C48
#define RCAR_GEN4_CPG_PLL2_1FBCKMLCL      0x0C4C
#define RCAR_GEN4_CPG_PLL2_1FBCKMCNT      0x0C50
#define RCAR_GEN4_CPG_PLL2_1FBCKMCNTE     0x0C54
#define RCAR_GEN4_CPG_PLL2_1FBCKMMDR      0x0C58
#define RCAR_GEN4_CPG_PLL3_0FBCKMCSR      0x0C60
#define RCAR_GEN4_CPG_PLL3_0FBCKMECR      0x0C64
#define RCAR_GEN4_CPG_PLL3_0FBCKMLCH      0x0C68
#define RCAR_GEN4_CPG_PLL3_0FBCKMLCL      0x0C6C
#define RCAR_GEN4_CPG_PLL3_0FBCKMCNT      0x0C70
#define RCAR_GEN4_CPG_PLL3_0FBCKMCNTE     0x0C74
#define RCAR_GEN4_CPG_PLL3_0FBCKMMDR      0x0C78
#define RCAR_GEN4_CPG_PLL3_1FBCKMCSR      0x0C80
#define RCAR_GEN4_CPG_PLL3_1FBCKMECR      0x0C84
#define RCAR_GEN4_CPG_PLL3_1FBCKMLCH      0x0C88
#define RCAR_GEN4_CPG_PLL3_1FBCKMLCL      0x0C8C
#define RCAR_GEN4_CPG_PLL3_1FBCKMCNT      0x0C90
#define RCAR_GEN4_CPG_PLL3_1FBCKMCNTE     0x0C94
#define RCAR_GEN4_CPG_PLL3_1FBCKMMDR      0x0C98
#define RCAR_GEN4_CPG_PLL4FBCKMCSR        0x0CA0
#define RCAR_GEN4_CPG_PLL4FBCKMECR        0x0CA4
#define RCAR_GEN4_CPG_PLL4FBCKMLCH        0x0CA8
#define RCAR_GEN4_CPG_PLL4FBCKMLCL        0x0CAC
#define RCAR_GEN4_CPG_PLL4FBCKMCNT        0x0CB0
#define RCAR_GEN4_CPG_PLL4FBCKMCNTE       0x0CB4
#define RCAR_GEN4_CPG_PLL4FBCKMMDR        0x0CB8
#define RCAR_GEN4_CPG_PLL5FBCKMCSR        0x0CC0
#define RCAR_GEN4_CPG_PLL5FBCKMECR        0x0CC4
#define RCAR_GEN4_CPG_PLL5FBCKMLCH        0x0CC8
#define RCAR_GEN4_CPG_PLL5FBCKMLCL        0x0CCC
#define RCAR_GEN4_CPG_PLL5FBCKMCNT        0x0CD0
#define RCAR_GEN4_CPG_PLL5FBCKMCNTE       0x0CD4
#define RCAR_GEN4_CPG_PLL5FBCKMMDR        0x0CD8
#define RCAR_GEN4_CPG_CPCKMCSR            0x0CE0
#define RCAR_GEN4_CPG_CPCKMECR            0x0CE4
#define RCAR_GEN4_CPG_CPCKMLCH            0x0CE8
#define RCAR_GEN4_CPG_CPCKMLCL            0x0CEC
#define RCAR_GEN4_CPG_CPCKMCNT            0x0CF0
#define RCAR_GEN4_CPG_CPCKMCNTE           0x0CF4
#define RCAR_GEN4_CPG_CPCKMMDR            0x0CF8
#define RCAR_GEN4_CPG_CBFUSACKMCSR        0x0D00
#define RCAR_GEN4_CPG_CBFUSACKMECR        0x0D04
#define RCAR_GEN4_CPG_CBFUSACKMLCH        0x0D08
#define RCAR_GEN4_CPG_CBFUSACKMLCL        0x0D0C
#define RCAR_GEN4_CPG_CBFUSACKMCNT        0x0D10
#define RCAR_GEN4_CPG_CBFUSACKMCNTE       0x0D14
#define RCAR_GEN4_CPG_CBFUSACKMMDR        0x0D18
#define RCAR_GEN4_CPG_CPGACKMSR           0x0D1C
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMCSR          0x0D20
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMECR          0x0D24
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMLCH          0x0D28
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMLCL          0x0D2C
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMCNT          0x0D30
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMCNTE         0x0D34
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2CKMMDR          0x0D38
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMCSR     0x0D40
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMECR     0x0D44
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMLCH     0x0D48
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMLCL     0x0D4C
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMCNT     0x0D50
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMCNTE    0x0D54
#define RCAR_GEN4_CPG_CKMDDR0ZB3D2DDRL0CKMMDR     0x0D58
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMCSR          0x0D60
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMECR          0x0D64
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMLCH          0x0D68
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMLCL          0x0D6C
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMCNT          0x0D70
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMCNTE         0x0D74
#define RCAR_GEN4_CPG_CKMDDR0CL16MCKMMDR          0x0D78
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMCSR          0x0D80
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMECR          0x0D84
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMLCH          0x0D88
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMLCL          0x0D8C
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMCNT          0x0D90
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMCNTE         0x0D94
#define RCAR_GEN4_CPG_CKMDDR0ZB3D1CKMMDR          0x0D98
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMCSR          0x0DA0
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMECR          0x0DA4
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMLCH          0x0DA8
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMLCL          0x0DAC
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMCNT          0x0DB0
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMCNTE         0x0DB4
#define RCAR_GEN4_CPG_CKMDDR0ZB3D4CKMMDR          0x0DB8
#define RCAR_GEN4_CPG_CKMDDR0ACKMSR               0x0DBC
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMCSR          0x0DC0
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMECR          0x0DC4
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMLCH          0x0DC8
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMLCL          0x0DCC
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMCNT          0x0DD0
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMCNTE         0x0DD4
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2CKMMDR          0x0DD8
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMCSR     0x0DE0
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMECR     0x0DE4
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMLCH     0x0DE8
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMLCL     0x0DEC
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMCNT     0x0DF0
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMCNTE    0x0DF4
#define RCAR_GEN4_CPG_CKMDDR1ZB3D2DDRL0CKMMDR     0x0DF8
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMCSR          0x0E00
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMECR          0x0E04
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMLCH          0x0E08
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMLCL          0x0E0C
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMCNT          0x0E10
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMCNTE         0x0E14
#define RCAR_GEN4_CPG_CKMDDR1CL16MCKMMDR          0x0E18
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMCSR          0x0E20
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMECR          0x0E24
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMLCH          0x0E28
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMLCL          0x0E2C
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMCNT          0x0E30
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMCNTE         0x0E34
#define RCAR_GEN4_CPG_CKMDDR1ZB3D1CKMMDR          0x0E38
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMCSR          0x0E40
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMECR          0x0E44
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMLCH          0x0E48
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMLCL          0x0E4C
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMCNT          0x0E50
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMCNTE         0x0E54
#define RCAR_GEN4_CPG_CKMDDR1ZB3D4CKMMDR          0x0E58
#define RCAR_GEN4_CPG_CKMDDR1ACKMSR               0x0E5C

/* MSTP resgister offset */
#define RCAR_GEN4_CPG_SRCR(n)             (0x2C00 + ((n) * 4))
#define RCAR_GEN4_CPG_SRSTCLR(n)          (0x2C80 + ((n) * 4))
#define RCAR_GEN4_CPG_MSTPCR(n)           (0x2D00 + ((n) * 4))
#define RCAR_GEN4_CPG_NSIRCR(n)           (0x2D80 + ((n) * 4))
#define RCAR_GEN4_CPG_RAHSR(n)            (0x2D80 + ((n) * 4))
#define RCAR_GEN4_CPG_MSTPSR(n)           (0x2E00 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACR_MSTPCR(n)    (0x3000 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACR_NSIRCR(n)    (0x3080 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACR_MSTPCR(n)    (0x3100 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACR_NSIRCR(n)    (0x3180 + ((n) * 4))
#define RCAR_GEN4_CPG_D2WACR_MSTPCR(n)    (0x3200 + ((n) * 4))
#define RCAR_GEN4_CPG_D2WACR_NSIRCR(n)    (0x3280 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACR_MSTPCR(n)    (0x3300 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACR_NSIRCR(n)    (0x3380 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACR_SRCR(n)      (0x3400 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACR_SRSTCLR(n)   (0x3480 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACR_SRCR(n)      (0x3500 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACR_SRSTCLR(n)   (0x3580 + ((n) * 4))
#define RCAR_GEN4_CPG_D2WACR_SRCR(n)      (0x3600 + ((n) * 4))
#define RCAR_GEN4_CPG_D2WACR_SRSTCLR(n)   (0x3680 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACR_SRCR(n)      (0x3700 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACR_SRSTCLR(n)   (0x3780 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACRA(n)          (0x3800 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACRA(n)          (0x3900 + ((n) * 4))
#define RCAR_GEN4_CPG_D2WACRA(n)          (0x3A00 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACRA(n)          (0x3B00 + ((n) * 4))
#define RCAR_GEN4_CPG_D0WACRB(n)          (0x3C00 + ((n) * 4))
#define RCAR_GEN4_CPG_D1WACRB(n)          (0x3D00 + ((n) * 4))
#define RCAR_GEN4_CPG_D3WACRB(n)          (0x3F00 + ((n) * 4))

/*
 * List of supported extal clocks
 */
#define RCAR_GEN4_EXT_CLK_16MHZ           (16666667)
#define RCAR_GEN4_EXT_CLK_20MHZ           (20000000)
#define RCAR_GEN4_EXT_CLK_40MHZ           (40000000)

/* Clock out */
#define RCAR_GEN4_CPG_CLK_Z0              (1800000000)
#define RCAR_GEN4_CPG_CLK_ZX              ( 900000000)
#define RCAR_GEN4_CPG_CLK_Z1              (1800000000)
#define RCAR_GEN4_CPG_CLK_ZR              (1066000000)
#define RCAR_GEN4_CPG_CLK_ZS              ( 266000000)
#define RCAR_GEN4_CPG_CLK_ZT              ( 533000000)
#define RCAR_GEN4_CPG_CLK_ZTR             ( 533000000)
#define RCAR_GEN4_CPG_CLK_S1              ( 533330000)
#define RCAR_GEN4_CPG_CLK_S3              ( 266660000)
#define RCAR_GEN4_CPG_CLK_S0D2            ( 400000000)
#define RCAR_GEN4_CPG_CLK_S0D3            ( 266666666)
#define RCAR_GEN4_CPG_CLK_S0D4            ( 200000000)
#define RCAR_GEN4_CPG_CLK_S0D2_MM         ( 400000000)
#define RCAR_GEN4_CPG_CLK_S0D3_MM         ( 266660000)
#define RCAR_GEN4_CPG_CLK_S0D4_MM         ( 200000000)
#define RCAR_GEN4_CPG_CLK_S0D2_RT         ( 400000000)
#define RCAR_GEN4_CPG_CLK_S0D3_RT         ( 266660000)
#define RCAR_GEN4_CPG_CLK_S0D4_RT         ( 200000000)
#define RCAR_GEN4_CPG_CLK_S0D6_RT         ( 133330000)
#define RCAR_GEN4_CPG_CLK_S0D24_RT        (  33330000)
#define RCAR_GEN4_CPG_CLK_S0D3_PER        ( 266660000)
#define RCAR_GEN4_CPG_CLK_S0D6_PER        ( 133330000)
#define RCAR_GEN4_CPG_CLK_S0D12_PER       (  66660000)
#define RCAR_GEN4_CPG_CLK_S0D24_PER       (  33330000)
#define RCAR_GEN4_CPG_CLK_S0D2_HSC        ( 400000000)
#define RCAR_GEN4_CPG_CLK_S0D3_HSC        ( 266660000)
#define RCAR_GEN4_CPG_CLK_S0D4_HSC        ( 200000000)
#define RCAR_GEN4_CPG_CLK_S0D6_HSC        ( 133330000)
#define RCAR_GEN4_CPG_CLK_S0D12_HSC       (  66660000)
#define RCAR_GEN4_CPG_CLK_S0D2_CC         ( 400000000)
#define RCAR_GEN4_CPG_CLK_S1D1            ( 533330000)
#define RCAR_GEN4_CPG_CLK_S1D2            ( 266660000)
#define RCAR_GEN4_CPG_CLK_S1D4            ( 133330000)
#define RCAR_GEN4_CPG_CLK_S1D8            (  66660000)
#define RCAR_GEN4_CPG_CLK_S1D12           (  33330000)
#define RCAR_GEN4_CPG_CLK_S2D1            ( 400000000)
#define RCAR_GEN4_CPG_CLK_S2D2            ( 200000000)
#define RCAR_GEN4_CPG_CLK_S2D4            ( 100000000)
#define RCAR_GEN4_CPG_CLK_S3D1            ( 266660000)
#define RCAR_GEN4_CPG_CLK_S3D2            ( 133330000)
#define RCAR_GEN4_CPG_CLK_S3D4            (  66660000)
#define RCAR_GEN4_CPG_CLK_LB              (  66660000)
#define RCAR_GEN4_CPG_CLK_CP              (  16660000)
#define RCAR_GEN4_CPG_CLK_CL              (  33330000)
#define RCAR_GEN4_CPG_CLK_CL16M           (  16666666)
#define RCAR_GEN4_CPG_CLK_ZB30            (1066000000)
#define RCAR_GEN4_CPG_CLK_ZB30D2          ( 533000000)
#define RCAR_GEN4_CPG_CLK_ZB30D4          ( 266000000)
#define RCAR_GEN4_CPG_CLK_ZB31            (1066000000)
#define RCAR_GEN4_CPG_CLK_ZB31D2          ( 533000000)
#define RCAR_GEN4_CPG_CLK_ZB31D4          ( 266000000)
#define RCAR_GEN4_CPG_CLK_SDSRC           ( 800000000)
#define RCAR_GEN4_CPG_CLK_SD0H            ( 800000000)
#define RCAR_GEN4_CPG_CLK_SD0             ( 200000000)
#define RCAR_GEN4_CPG_CLK_RPC             ( 320000000)
#define RCAR_GEN4_CPG_CLK_RPCD2           ( 160000000)
#define RCAR_GEN4_CPG_CLK_MS0             ( 133330000)
#define RCAR_GEN4_CPG_CLK_CANFD           (  80000000)
#define RCAR_GEN4_CPG_CLK_CSI0            (  25000000)
#define RCAR_GEN4_CPG_CLK_FRAY            (  80000000)
#define RCAR_GEN4_CPG_CLK_POST2           ( 133330000)
#define RCAR_GEN4_CPG_CLK_POST3           (  66660000)
#define RCAR_GEN4_CPG_CLK_DSI             ( 800000000)
#define RCAR_GEN4_CPG_CLK_VIP             ( 640000000)
#define RCAR_GEN4_CPG_CLK_ADGH            ( 800000000)
#define RCAR_GEN4_CPG_CLK_CNNDSP          ( 800000000)
#define RCAR_GEN4_CPG_CLK_ICU             ( 400000000)
#define RCAR_GEN4_CPG_CLK_ICUD2           ( 200000000)
#define RCAR_GEN4_CPG_CLK_VCBUS           ( 800000000)
#define RCAR_GEN4_CPG_CLK_CBFUSA          (  16660000)
#define RCAR_GEN4_CPG_CLK_CPEX            (  16660000)
#define RCAR_GEN4_CPG_CLK_RCLK            (     32800)
#define RCAR_GEN4_CPG_CLK_OSCCLK          (    131570)
#define RCAR_GEN4_CPG_CLK_SASYNCPERD1     ( 266660000)
#define RCAR_GEN4_CPG_CLK_SASYNCPERD2     ( 133330000)
#define RCAR_GEN4_CPG_CLK_SASYNCPERD4     (  66660000)

/*
 * APMU
 */
#define RCAR_GEN4_APMU_DM0_BASE            0xE6170000
#define RCAR_GEN4_APMU_DM1_BASE            0xE6171000
#define RCAR_GEN4_APMU_DM2_BASE            0xE6172000
#define RCAR_GEN4_APMU_DM3_BASE            0xE6173000

#define RCAR_GEN4_APMU_ADMIN_GP            0x000
#define RCAR_GEN4_APMU_R52_GP              0x300
#define RCAR_GEN4_APMU_AP_CLUSTER0_GP      0x400
#define RCAR_GEN4_APMU_AP_CLUSTER1_GP      0x440
#define RCAR_GEN4_APMU_AP_CLUSTER2_GP      0x480
#define RCAR_GEN4_APMU_AP_CLUSTER3_GP      0x4C0
#define RCAR_GEN4_APMU_AP_CORE0_GP         0x800
#define RCAR_GEN4_APMU_AP_CORE1_GP         0x840
#define RCAR_GEN4_APMU_AP_CORE2_GP         0xA00
#define RCAR_GEN4_APMU_AP_CORE3_GP         0xA40
#define RCAR_GEN4_APMU_AP_CORE4_GP         0xC00
#define RCAR_GEN4_APMU_AP_CORE5_GP         0xC40
#define RCAR_GEN4_APMU_AP_CORE6_GP         0xE00
#define RCAR_GEN4_APMU_AP_CORE7_GP         0xE40

/* Admin Group */
#define RCAR_GEN4_APMU_WPCR                0x00
#define RCAR_GEN4_APMU_WPR                 0x04
#define RCAR_GEN4_APMU_D0ACCENR            0x10
#define RCAR_GEN4_APMU_D1ACCENR            0x14
#define RCAR_GEN4_APMU_D2ACCENR            0x18
#define RCAR_GEN4_APMU_D3ACCENR            0x1C
#define RCAR_GEN4_APMU_PTCSR               0x20
#define RCAR_GEN4_APMU_PTERADR             0x24
#define RCAR_GEN4_APMU_DCLSEIJTR           0x28
#define RCAR_GEN4_APMU_SYSMONCTRL          0x30
#define RCAR_GEN4_APMU_SYSMONSTSR          0x34
#define RCAR_GEN4_APMU_A3PWRCTRL0          0x40
#define RCAR_GEN4_APMU_A3PWRCTRL1          0x44
#define RCAR_GEN4_APMU_A3FSMSTSR0          0x48
#define RCAR_GEN4_APMU_A3FSMSTSR1          0x4C
#define RCAR_GEN4_APMU_A3FSMLOCKR0         0x50
#define RCAR_GEN4_APMU_A3FSMLOCKR1         0x54
#define RCAR_GEN4_APMU_INTSTSR             0x58
#define RCAR_GEN4_APMU_ERRSTSR             0x5C
#define RCAR_GEN4_APMU_FRSTR               0x60
#define RCAR_GEN4_APMU_FRSTD               0x64
#define RCAR_GEN4_APMU_FRSTCTRL            0x68
#define RCAR_GEN4_APMU_PADDCHKSTSR         0x80
#define RCAR_GEN4_APMU_PWDATACHKSTSR       0x84
#define RCAR_GEN4_APMU_APRTMGINTMASK       0x98
#define RCAR_GEN4_APMU_RSCTRL              0x9C
#define RCAR_GEN4_APMU_AA64nAA32CR         0xA0

/* R52 Group */
#define RCAR_GEN4_APMU_CR52CR              0x00
#define RCAR_GEN4_APMU_CR52RSTCTRL         0x04
#define RCAR_GEN4_APMU_FSMLOCKRCR52        0x0C
#define RCAR_GEN4_APMU_FSMSTSRCR52         0x18
#define RCAR_GEN4_APMU_G2GPRCR52           0x1C
#define RCAR_GEN4_APMU_CR52CMPEN           0x28
#define RCAR_GEN4_APMU_GCNTERRENCR52       0x2C
#define RCAR_GEN4_APMU_CR52RVBAR           0x30
#define RCAR_GEN4_APMU_CR52BAR             0x34
#define RCAR_GEN4_APMU_CR52RVBARP          0x38
#define RCAR_GEN4_APMU_CR52BARP            0x3C

/* A76/A55 Cluster Group */
#define RCAR_GEN4_APMU_PWRCTRLCL           0x00
 #define RCAR_GEN4_APMU_PWRCTRLCL_WUP_REQ         (1 << 0)
 #define RCAR_GEN4_APMU_PWRCTRLCL_PCHPDNEN        (1 << 17)
#define RCAR_GEN4_APMU_L3CTRLCL            0x04
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_MSK		  (7 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_ALL_ON	  (4 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_34_ON      (3 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_24_ON      (2 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_14_ON      (1 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3STS_OFF		  (0 << 16)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3DENY           (1 << 9)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3ACCEPT         (1 << 8)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3REQ		      (1 << 4)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_MSK       (7 << 0)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_ALL_ON    (4 << 0)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_34_ON	  (3 << 0)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_24_ON	  (2 << 0)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_14_ON	  (1 << 0)
 #define RCAR_GEN4_APMU_L3CTRLCL_L3CTRL_OFF       (0 << 0)
#define RCAR_GEN4_APMU_FSMLOCKRCL          0x0C
#define RCAR_GEN4_APMU_PDENYSTSRCL         0x10
#define RCAR_GEN4_APMU_PDENYINTRCL         0x14
#define RCAR_GEN4_APMU_FSMSTSRCL           0x18
 #define RCAR_GEN4_APMU_FSMSTSRCL_STATE_MSK        0xFF
 #define RCAR_GEN4_APMU_FSMSTSRCL_STATE_ON         0x12
 #define RCAR_GEN4_APMU_FSMSTSRCL_STATE_OFF        0x00
 #define RCAR_GEN4_APMU_FSMSTSRCL_STATE_FUNC_RET   0x21
 #define RCAR_GEN4_APMU_FSMSTSRCL_STATE_MEM_RET    0x31
 #define RCAR_GEN4_APMU_A3PWRCTRL_A3_WUP_REQ            (1 << 0)
 #define RCAR_GEN4_APMU_A3PWRCTRL_A3_PDN_EN             (1 << 4)
 #define RCAR_GEN4_APMU_A3FSMSTSR_STATE_MSK        0xFF
 #define RCAR_GEN4_APMU_A3FSMSTSR_STATE_ON         0x04
 #define RCAR_GEN4_APMU_A3FSMSTSR_STATE_OFF        0x00

#define RCAR_GEN4_APMU_G2GPRCL             0x1C
#define RCAR_GEN4_APMU_SAFECTRLCL          0x20
#define RCAR_GEN4_APMU_DCLSENCL            0x24
#define RCAR_GEN4_APMU_DCLSCMPENCL         0x28
#define RCAR_GEN4_APMU_GCNTERRENCL         0x2C
#define RCAR_GEN4_APMU_CA55BAR             0x30
#define RCAR_GEN4_APMU_CA55BARP            0x38

/* A76/A55 Core Group */
#define RCAR_GEN4_APMU_PWRCTRLC            0x00
 #define RCAR_GEN4_APMU_PWRCTRLC_WUP_REQ           (1 << 0)
 #define RCAR_GEN4_APMU_PWRCTRLC_PCHPDNEN          (1 << 17)
#define RCAR_GEN4_APMU_FSMLOCKRC           0x0C
#define RCAR_GEN4_APMU_PDENYSTSRC          0x10
#define RCAR_GEN4_APMU_PDENYINTRC          0x14
#define RCAR_GEN4_APMU_FSMSTSRC            0x18
 #define RCAR_GEN4_APMU_FSMSTSRC_STATE_MSK         0xFF
 #define RCAR_GEN4_APMU_FSMSTSRC_STATE_ON          0x0D
 #define RCAR_GEN4_APMU_FSMSTSRC_STATE_OFF         0x00
 #define RCAR_GEN4_APMU_FSMSTSRC_STATE_OFF_EMU     0x1C
#define RCAR_GEN4_APMU_G2GPRC              0x1C
#define RCAR_GEN4_APMU_SAFECTRLC           0x20
 #define RCAR_GEN4_APMU_SAFECTRLC_DBGGEN_PPDN      (1 << 13)
#define RCAR_GEN4_APMU_RVBARLC             0x30
#define RCAR_GEN4_APMU_RVBARHC             0x34
#define RCAR_GEN4_APMU_RVBARPLC            0x38
#define RCAR_GEN4_APMU_RVBARPHC            0x3C

#define RCAR_GEN4_APMU_CLUSTER_OFFSET             0x200
#define RCAR_GEN4_APMU_CLUSTER_SIZE               0x40
#define RCAR_GEN4_APMU_CORE_IN_CLUSTER_SIZE       0x40
#define RCAR_GEN4_APMU_CORE_PER_CLUSTER           2
#define RCAR_GEN4_APMU_CORE_PER_DOMAIN            4

/*
 * Reset registers for Gen4 (V3U, S4)
 */
#define RCAR_GEN4_RESET_BASE              0xE6160000

#define RCAR_GEN4_RST_DM0_BASE            0xE6160000
#define RCAR_GEN4_RST_DM1_BASE            0xE6164000
#define RCAR_GEN4_RST_DM2_BASE            0xE6168000
#define RCAR_GEN4_RST_DM3_BASE            0xE616C000
#define RCAR_GEN4_RST_SIZE                0x4000

/* Reset register offset */
#define RCAR_GEN4_RST_MODEMR0             0x0000
  #define RCAR_GEN4_MODEMR_BOOT_MASK           0x1E
  #define RCAR_GEN4_MODEMR_BOOT_HYPERFLASH160  0x4
  #define RCAR_GEN4_MODEMR_BOOT_HYPERFLASH80   0x6
  #define RCAR_GEN4_MODEMR_BOOT_QSPI_FLASH40   0x8
  #define RCAR_GEN4_MODEMR_BOOT_QSPI_FLASH80   0xC
#define RCAR_GEN4_RST_MODEMR1             0x0004
#define RCAR_GEN4_RST_WDTRSTCR            0x0010
#define RCAR_GEN4_RST_RSTOUTCR            0x0014
#define RCAR_GEN4_RST_SRESCR0             0x0018
#define RCAR_GEN4_RST_SRESCR1             0x001C
#define RCAR_GEN4_RST_RSTFR0              0x0020
#define RCAR_GEN4_RST_RSTFR1              0x0024
#define RCAR_GEN4_RST_RSTFR2              0x0028
#define RCAR_GEN4_RST_RSTFR3              0x002C
#define RCAR_GEN4_RST_STBCHR0             0x0030
#define RCAR_GEN4_RST_STBCHR1             0x0034
#define RCAR_GEN4_RST_STBCHR2             0x0038
#define RCAR_GEN4_RST_STBCHR3             0x003C
#define RCAR_GEN4_RST_STBCHR4             0x0040
#define RCAR_GEN4_RST_STBCHR5             0x0044
#define RCAR_GEN4_RST_STBCHR6             0x0048
#define RCAR_GEN4_RST_STBCHR7             0x004C
#define RCAR_GEN4_RST_APBSFTYCHKR         0x0050
#define RCAR_GEN4_RST_ICUMXBAR            0x0054
#define RCAR_GEN4_RST_ICUMXCPCR           0x0058
#define RCAR_GEN4_RST_RSTPTCSR            0x0068
#define RCAR_GEN4_RST_RSTPTERADR          0x006C
#define RCAR_GEN4_RST_D0WACR              0x3800
#define RCAR_GEN4_RST_D1WACR              0x3A00
#define RCAR_GEN4_RST_D2WACR              0x3C00
#define RCAR_GEN4_RST_D3WACR              0x3E00

/*
 * System Controller for Gen4 (V3U, S4, V4H)
 */
#define RCAR_GEN4_SYSC_DM0_BASE          0xE6180000
#define RCAR_GEN4_SYSC_DM1_BASE          0xE6184000
#define RCAR_GEN4_SYSC_DM2_BASE          0xE6188000
#define RCAR_GEN4_SYSC_DM3_BASE          0xE618C000
#define RCAR_GEN4_SYSC_SIZE              0x4000

#define RCAR_GEN4_SYSC_SR                0x0000
#define RCAR_GEN4_SYSC_PTCSR             0x0010
#define RCAR_GEN4_SYSC_PTERADR           0x0014
#define RCAR_GEN4_SYSC_EFCR              0x0034
#define RCAR_GEN4_SYSC_PONSR0            0x0800
#define RCAR_GEN4_SYSC_PONSR1            0x0804
#define RCAR_GEN4_SYSC_POFFSR0           0x0808
#define RCAR_GEN4_SYSC_POFFSR1           0x080C
#define RCAR_GEN4_SYSC_ISCR0             0x0810
#define RCAR_GEN4_SYSC_ISCR1             0x0814
#define RCAR_GEN4_SYSC_IER0              0x0820
#define RCAR_GEN4_SYSC_IER1              0x0824
#define RCAR_GEN4_SYSC_IMR0              0x0830
#define RCAR_GEN4_SYSC_IMR1              0x0834

#define RCAR_GEN4_SYSC_PDRSR(n)          (0x1000 + ((n) * 64))
#define RCAR_GEN4_SYSC_PDRONCR(n)        (0x1004 + ((n) * 64))
#define RCAR_GEN4_SYSC_PDROFFCR(n)       (0x1008 + ((n) * 64))
#define RCAR_GEN4_SYSC_PDRESR(n)         (0x100C + ((n) * 64)) // Must be checked


#define RCAR_GEN4_SYSC_D0WACR0           0x3000
#define RCAR_GEN4_SYSC_D0WACR1           0x3004
#define RCAR_GEN4_SYSC_D0WACR2           0x3008
#define RCAR_GEN4_SYSC_D0WACR3           0x300C
#define RCAR_GEN4_SYSC_D0WACR4           0x3010
#define RCAR_GEN4_SYSC_D0WACR5           0x3014
#define RCAR_GEN4_SYSC_D1WACR0           0x3020
#define RCAR_GEN4_SYSC_D1WACR1           0x3024
#define RCAR_GEN4_SYSC_D1WACR2           0x3028
#define RCAR_GEN4_SYSC_D1WACR3           0x302C
#define RCAR_GEN4_SYSC_D1WACR4           0x3030
#define RCAR_GEN4_SYSC_D1WACR5           0x3034
#define RCAR_GEN4_SYSC_D2WACR0           0x3040
#define RCAR_GEN4_SYSC_D2WACR1           0x3044
#define RCAR_GEN4_SYSC_D2WACR2           0x3048
#define RCAR_GEN4_SYSC_D2WACR3           0x304C
#define RCAR_GEN4_SYSC_D2WACR4           0x3050
#define RCAR_GEN4_SYSC_D2WACR5           0x3054
#define RCAR_GEN4_SYSC_D3WACR0           0x3060
#define RCAR_GEN4_SYSC_D3WACR1           0x3064
#define RCAR_GEN4_SYSC_D3WACR2           0x3068
#define RCAR_GEN4_SYSC_D3WACR3           0x306C
#define RCAR_GEN4_SYSC_D3WACR4           0x3070
#define RCAR_GEN4_SYSC_D3WACR5           0x3074

/* SYSC for FuSa */
#define RCAR_GEN4_SYSC_RDNCSR            0x0020
#define RCAR_GEN4_SYSC_RDNIR             0x0024
#define RCAR_GEN4_SYSC_APBSR             0x0030
#define RCAR_GEN4_SYSC_ISOEHSR0          0x0860
#define RCAR_GEN4_SYSC_ISOEHSR1          0x0864
#define RCAR_GEN4_SYSC_ISOELSR0          0x0868
#define RCAR_GEN4_SYSC_ISOELSR1          0x086C
#define RCAR_GEN4_SYSC_ISOEHIR0          0x0870
#define RCAR_GEN4_SYSC_ISOEHIR1          0x0874
#define RCAR_GEN4_SYSC_ISOELIR0          0x0878
#define RCAR_GEN4_SYSC_ISOELIR1          0x087C
#define RCAR_GEN4_SYSC_ISOEHMR0          0x0C00
#define RCAR_GEN4_SYSC_ISOEHMR1          0x0C04
#define RCAR_GEN4_SYSC_ISOELMR0          0x0C08
#define RCAR_GEN4_SYSC_ISOELMR1          0x0C0C

/*
 * GIC-600 for Gen4 (V3U, S4, V4H)
 */
#define RCAR_GEN4_GICD_BASE               0xF1000000
#define RCAR_GEN4_GICA_BASE               0xF1010000
#define RCAR_GEN4_GICT_BASE               0xF1020000
#define RCAR_GEN4_GICP_BASE               0xF1030000
#define RCAR_GEN4_GICS_BASE               0xF1040000
#define RCAR_GEN4_GICR_LPI_BASE           0xF1060000
#define RCAR_GEN4_GICR_SGI_BASE           0xF1070000
#define RCAR_GEN4_GICDA_BASE              0xF1160000


/*
 * Interrupt ID for Gen4
 */

#define RCAR_INTCSYS_SPI(_id)             ((_id) + 32)

#define RCAR_GEN4_INTCSYS_IRQ0                        32
#define RCAR_GEN4_INTCSYS_IRQ1                        33
#define RCAR_GEN4_INTCSYS_IRQ2                        34
#define RCAR_GEN4_INTCSYS_IRQ3                        35
#define RCAR_GEN4_INTCSYS_IRQ4                        36
#define RCAR_GEN4_INTCSYS_IRQ5                        37
#define RCAR_GEN4_INTCSYS_PMU                         38
#define RCAR_GEN4_INTCSYS_DBSC                        51
#define RCAR_GEN4_INTCSYS_IPMMUMM_ERR_NSEC            52
#define RCAR_GEN4_INTCSYS_IPMMUMM_ERR_SEC             53
#define RCAR_GEN4_INTCSYS_RTDM0_SEC_ERR	            60
#define RCAR_GEN4_INTCSYS_RTDM0_NSEC_ERR	            61
#define RCAR_GEN4_INTCSYS_RTDM00	                    62
#define RCAR_GEN4_INTCSYS_RTDM01	                    63
#define RCAR_GEN4_INTCSYS_RTDM02	                    64
#define RCAR_GEN4_INTCSYS_RTDM03	                    65
#define RCAR_GEN4_INTCSYS_RTDM04	                    66
#define RCAR_GEN4_INTCSYS_RTDM05	                    67
#define RCAR_GEN4_INTCSYS_RTDM06	                    68
#define RCAR_GEN4_INTCSYS_RTDM07	                    69
#define RCAR_GEN4_INTCSYS_RTDM08	                    70
#define RCAR_GEN4_INTCSYS_RTDM09	                    71
#define RCAR_GEN4_INTCSYS_RTDM010	                    72
#define RCAR_GEN4_INTCSYS_RTDM011	                    73
#define RCAR_GEN4_INTCSYS_RTDM012	                    74
#define RCAR_GEN4_INTCSYS_RTDM013	                    75
#define RCAR_GEN4_INTCSYS_RTDM014	                    76
#define RCAR_GEN4_INTCSYS_RTDM015	                    77
#define RCAR_GEN4_INTCSYS_RTDM1_SEC_ERR	            78
#define RCAR_GEN4_INTCSYS_RTDM11_NSEC_ERR	            79
#define RCAR_GEN4_INTCSYS_RTDM10	                    80
#define RCAR_GEN4_INTCSYS_RTDM11	                    81
#define RCAR_GEN4_INTCSYS_RTDM12	                    82
#define RCAR_GEN4_INTCSYS_RTDM13	                    83
#define RCAR_GEN4_INTCSYS_RTDM14	                    84
#define RCAR_GEN4_INTCSYS_RTDM15	                    85
#define RCAR_GEN4_INTCSYS_RTDM16	                    86
#define RCAR_GEN4_INTCSYS_RTDM17	                    87
#define RCAR_GEN4_INTCSYS_RTDM18	                    88
#define RCAR_GEN4_INTCSYS_RTDM19	                    89
#define RCAR_GEN4_INTCSYS_RTDM110	                    90
#define RCAR_GEN4_INTCSYS_RTDM111	                    91
#define RCAR_GEN4_INTCSYS_RTDM112	                    92
#define RCAR_GEN4_INTCSYS_RTDM113	                    93
#define RCAR_GEN4_INTCSYS_RTDM114	                    94
#define RCAR_GEN4_INTCSYS_RTDM115	                    95
#define RCAR_GEN4_INTCSYS_RTDM2_SEC_ERR	            96
#define RCAR_GEN4_INTCSYS_RTDM2_NSEC_ERR	            97
#define RCAR_GEN4_INTCSYS_RTDM20	                    98
#define RCAR_GEN4_INTCSYS_RTDM21	                    99
#define RCAR_GEN4_INTCSYS_RTDM22	                    100
#define RCAR_GEN4_INTCSYS_RTDM23	                    101
#define RCAR_GEN4_INTCSYS_RTDM24	                    102
#define RCAR_GEN4_INTCSYS_RTDM25	                    103
#define RCAR_GEN4_INTCSYS_RTDM26	                    104
#define RCAR_GEN4_INTCSYS_RTDM27	                    105
#define RCAR_GEN4_INTCSYS_RTDM28	                    106
#define RCAR_GEN4_INTCSYS_RTDM29	                    107
#define RCAR_GEN4_INTCSYS_RTDM210	                    108
#define RCAR_GEN4_INTCSYS_RTDM211	                    109
#define RCAR_GEN4_INTCSYS_RTDM212	                    110
#define RCAR_GEN4_INTCSYS_RTDM213	                    111
#define RCAR_GEN4_INTCSYS_RTDM214	                    112
#define RCAR_GEN4_INTCSYS_RTDM215	                    113
#define RCAR_GEN4_INTCSYS_RTDM3_SEC_ERR	            114
#define RCAR_GEN4_INTCSYS_RTDM3_NSEC_ERR	            115
#define RCAR_GEN4_INTCSYS_RTDM30	                    116
#define RCAR_GEN4_INTCSYS_RTDM31	                    117
#define RCAR_GEN4_INTCSYS_RTDM32	                    118
#define RCAR_GEN4_INTCSYS_RTDM33	                    119
#define RCAR_GEN4_INTCSYS_RTDM34	                    120
#define RCAR_GEN4_INTCSYS_RTDM35	                    121
#define RCAR_GEN4_INTCSYS_RTDM36	                    122
#define RCAR_GEN4_INTCSYS_RTDM37	                    123
#define RCAR_GEN4_INTCSYS_RTDM38	                    124
#define RCAR_GEN4_INTCSYS_RTDM39	                    125
#define RCAR_GEN4_INTCSYS_RTDM310	                    126
#define RCAR_GEN4_INTCSYS_RTDM311	                    127
#define RCAR_GEN4_INTCSYS_RTDM312	                    128
#define RCAR_GEN4_INTCSYS_RTDM313	                    129
#define RCAR_GEN4_INTCSYS_RTDM314	                    130
#define RCAR_GEN4_INTCSYS_RTDM315	                    131
#define RCAR_GEN4_INTCSYS_SYDM1_SEC_ERR	            132
#define RCAR_GEN4_INTCSYS_SYDM1_NSEC_ERR	            133
#define RCAR_GEN4_INTCSYS_SYDM10	                    134
#define RCAR_GEN4_INTCSYS_SYDM11	                    135
#define RCAR_GEN4_INTCSYS_SYDM12	                    136
#define RCAR_GEN4_INTCSYS_SYDM13	                    137
#define RCAR_GEN4_INTCSYS_SYDM14	                    138
#define RCAR_GEN4_INTCSYS_SYDM15	                    139
#define RCAR_GEN4_INTCSYS_SYDM16	                    140
#define RCAR_GEN4_INTCSYS_SYDM17	                    141
#define RCAR_GEN4_INTCSYS_SYDM18	                    142
#define RCAR_GEN4_INTCSYS_SYDM19	                    143
#define RCAR_GEN4_INTCSYS_SYDM110	                    144
#define RCAR_GEN4_INTCSYS_SYDM111	                    145
#define RCAR_GEN4_INTCSYS_SYDM112	                    146
#define RCAR_GEN4_INTCSYS_SYDM113	                    147
#define RCAR_GEN4_INTCSYS_SYDM114	                    148
#define RCAR_GEN4_INTCSYS_SYDM115	                    149
#define RCAR_GEN4_INTCSYS_SYDM2_SEC_ERR               150
#define RCAR_GEN4_INTCSYS_SYDM2_NSEC_ERR	            151
#define RCAR_GEN4_INTCSYS_SYDM20	                    152
#define RCAR_GEN4_INTCSYS_SYDM21	                    153
#define RCAR_GEN4_INTCSYS_SYDM22	                    154
#define RCAR_GEN4_INTCSYS_SYDM23	                    155
#define RCAR_GEN4_INTCSYS_SYDM24	                    156
#define RCAR_GEN4_INTCSYS_SYDM25	                    157
#define RCAR_GEN4_INTCSYS_SYDM26	                    158
#define RCAR_GEN4_INTCSYS_SYDM27	                    159
#define RCAR_GEN4_INTCSYS_CR0	                        165
#define RCAR_GEN4_INTCSYS_SYSC	                    166
#define RCAR_GEN4_INTCSYS_RPC	                        167
#define RCAR_GEN4_INTCSYS_CPG	                        168
#define RCAR_GEN4_INTCSYS_RESET	                    169
#define RCAR_GEN4_INTCSYS_CCI500SS	                170
#define RCAR_GEN4_INTCSYS_CA55SS0_0	                171
#define RCAR_GEN4_INTCSYS_CA55SS0_1	                172
#define RCAR_GEN4_INTCSYS_CA55SS1_0	                173
#define RCAR_GEN4_INTCSYS_CA55SS1_1	                174
#define RCAR_GEN4_INTCSYS_STAT	                    175
#define RCAR_GEN4_INTCSYS_APMU	                    176
#define RCAR_GEN4_INTCSYS_CSD	                        177
#define RCAR_GEN4_INTCSYS_SYDM28	                    179
#define RCAR_GEN4_INTCSYS_SYDM29	                    180
#define RCAR_GEN4_INTCSYS_SYDM210	                    181
#define RCAR_GEN4_INTCSYS_SYDM211	                    182
#define RCAR_GEN4_INTCSYS_SYDM212	                    183
#define RCAR_GEN4_INTCSYS_SYDM213	                    184
#define RCAR_GEN4_INTCSYS_SYDM214	                    185
#define RCAR_GEN4_INTCSYS_SYDM215	                    186
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM0	    224
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM1     225
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM2	    226
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM3	    227
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM4	    228
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM5	    229
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM6	    230
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM7	    231
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM8	    232
#define RCAR_GEN4_INTCSYS_ICUMX_SINGLE_ICUP_2_ARM9	    233
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM0	        234
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM1	        235
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM2	        236
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM3	        237
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM4	        238
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM5	        239
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM6	        240
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM7	        241
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM8	        242
#define RCAR_GEN4_INTCSYS_ICUMX_ICUP_2_ARM9	        243
#define RCAR_GEN4_INTCSYS_RFSO0	                    250
#define RCAR_GEN4_INTCSYS_RFSO1	                    251
#define RCAR_GEN4_INTCSYS_RFSO2	                    252
#define RCAR_GEN4_INTCSYS_RFSO3	                    253
#define RCAR_GEN4_INTCSYS_RFSO4	                    254
#define RCAR_GEN4_INTCSYS_RFSO5	                    255
#define RCAR_GEN4_INTCSYS_RFSO6	                    256
#define RCAR_GEN4_INTCSYS_RFSO7	                    257
#define RCAR_GEN4_INTCSYS_RFSO8	                    258
#define RCAR_GEN4_INTCSYS_RFSO9	                    259
#define RCAR_GEN4_INTCSYS_RFSO10	                    260
#define RCAR_GEN4_INTCSYS_MSIOF0	                    262
#define RCAR_GEN4_INTCSYS_MSIOF1	                    263
#define RCAR_GEN4_INTCSYS_MSIOF2	                    264
#define RCAR_GEN4_INTCSYS_MSIOF3	                    265
#define RCAR_GEN4_INTCSYS_UFS_DEBUG	                266
#define RCAR_GEN4_INTCSYS_UFS_HC	                    267
#define RCAR_GEN4_INTCSYS_SDHI0	                    268
#define RCAR_GEN4_INTCSYS_I2C0	                    270
#define RCAR_GEN4_INTCSYS_I2C1	                    271
#define RCAR_GEN4_INTCSYS_I2C2	                    272
#define RCAR_GEN4_INTCSYS_I2C3	                    273
#define RCAR_GEN4_INTCSYS_I2C4	                    274
#define RCAR_GEN4_INTCSYS_I2C5	                    275
#define RCAR_GEN4_INTCSYS_HSCIF0	                    277
#define RCAR_GEN4_INTCSYS_HSCIF1	                    278
#define RCAR_GEN4_INTCSYS_HSCIF2	                    279
#define RCAR_GEN4_INTCSYS_HSCIF3	                    280
#define RCAR_GEN4_INTCSYS_SCIF0	                    281
#define RCAR_GEN4_INTCSYS_SCIF1	                    282
#define RCAR_GEN4_INTCSYS_SCIF3	                    284
#define RCAR_GEN4_INTCSYS_SCIF4	                    285
#define RCAR_GEN4_INTCSYS_RSW2_MFWD_ERR               288
#define RCAR_GEN4_INTCSYS_RSW2_RACE_ERR               289
#define RCAR_GEN4_INTCSYS_RSW2_COMA_ERR               290
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_ERR              291
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_ERR              292
#define RCAR_GEN4_INTCSYS_RSW2_ETHA0_ERR              293
#define RCAR_GEN4_INTCSYS_RSW2_ETHA1_ERR              294
#define RCAR_GEN4_INTCSYS_RSW2_ETHA2_ERR              295
#define RCAR_GEN4_INTCSYS_RSW2_GPTP0_STS	            297
#define RCAR_GEN4_INTCSYS_RSW2_GPTP1_STS	            298
#define RCAR_GEN4_INTCSYS_RSW2_MFWD_STS	            299
#define RCAR_GEN4_INTCSYS_RSW2_RACE_STS	            300
#define RCAR_GEN4_INTCSYS_RSW2_COMA_STS	            301
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_STS	            302
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_STS	            303
#define RCAR_GEN4_INTCSYS_RSW2_ETHA0_STS	            304
#define RCAR_GEN4_INTCSYS_RSW2_ETHA1_STS	            305
#define RCAR_GEN4_INTCSYS_RSW2_ETHA2_STS	            306
#define RCAR_GEN4_INTCSYS_RSW2_RMAC0_STS	            308
#define RCAR_GEN4_INTCSYS_RSW2_RMAC1_STS	            309
#define RCAR_GEN4_INTCSYS_RSW2_RMAC2_STS	            310
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_0           312
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_1           313
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_2           314
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_3           315
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_4           316
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_5           317
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_6           318
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTX_7           319
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_0           320
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_1           321
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_2           322
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_3           323
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_4           324
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_5           325
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_6           326
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTX_7           327
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTS_0           328
#define RCAR_GEN4_INTCSYS_RSW2_GWCA0_RXTS_1           329
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTS_0           330
#define RCAR_GEN4_INTCSYS_RSW2_GWCA1_RXTS_1           331
#define RCAR_GEN4_INTCSYS_RSW2_RAMC0_MDIO             332
#define RCAR_GEN4_INTCSYS_RSW2_RAMC1_MDIO             333
#define RCAR_GEN4_INTCSYS_RSW2_RAMC2_MDIO             334
#define RCAR_GEN4_INTCSYS_RSW2_RAMC0_PHY	            336
#define RCAR_GEN4_INTCSYS_RSW2_RAMC1_PHY	            337
#define RCAR_GEN4_INTCSYS_RSW2_RAMC2_PHY	            338
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP0_E              352
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP0_I              353
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP1_E              354
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP1_I              355
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP2_E              356
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP2_I              357
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP3_E              358
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP3_I              359
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP4_E              360
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP4_I              361
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP5_E              362
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP5_I              363
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP6_E              364
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP6_I              365
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP7_E              366
#define RCAR_GEN4_INTCSYS_MFI_AR_2_AP7_I              367
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP0_E       368
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP1_E       370
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP2_E       372
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP3_E       374
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP4_E       376
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP5_E       378
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP6_E       380
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP7_E       382
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER0_2_AP8_E       385
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP0_E       386
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP1_E       388
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP2_E       390
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP3_E       392
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP4_E       394
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP5_E       396
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP6_E       398
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP7_E       400
#define RCAR_GEN4_INTCSYS_MFI_G4MH_PER1_2_AP8_E       403
#define RCAR_GEN4_INTCSYS_CMT00	                    480
#define RCAR_GEN4_INTCSYS_CMT01	                    481
#define RCAR_GEN4_INTCSYS_CMT10	                    482
#define RCAR_GEN4_INTCSYS_CMT11	                    483
#define RCAR_GEN4_INTCSYS_CMT12	                    484
#define RCAR_GEN4_INTCSYS_CMT13	                    485
#define RCAR_GEN4_INTCSYS_CMT14	                    486
#define RCAR_GEN4_INTCSYS_CMT15	                    487
#define RCAR_GEN4_INTCSYS_CMT16	                    488
#define RCAR_GEN4_INTCSYS_CMT17	                    489
#define RCAR_GEN4_INTCSYS_CMT20	                    490
#define RCAR_GEN4_INTCSYS_CMT21	                    491
#define RCAR_GEN4_INTCSYS_CMT22	                    492
#define RCAR_GEN4_INTCSYS_CMT23	                    493
#define RCAR_GEN4_INTCSYS_CMT24	                    494
#define RCAR_GEN4_INTCSYS_CMT25	                    495
#define RCAR_GEN4_INTCSYS_CMT26	                    496
#define RCAR_GEN4_INTCSYS_CMT27	                    497
#define RCAR_GEN4_INTCSYS_CMT30	                    498
#define RCAR_GEN4_INTCSYS_CMT31	                    499
#define RCAR_GEN4_INTCSYS_CMT32	                    500
#define RCAR_GEN4_INTCSYS_CMT33	                    501
#define RCAR_GEN4_INTCSYS_CMT34	                    502
#define RCAR_GEN4_INTCSYS_CMT35	                    503
#define RCAR_GEN4_INTCSYS_CMT36	                    504
#define RCAR_GEN4_INTCSYS_CMT37	                    505
#define RCAR_GEN4_INTCSYS_TMU00	                    506
#define RCAR_GEN4_INTCSYS_TMU01	                    507
#define RCAR_GEN4_INTCSYS_TMU02	                    508
#define RCAR_GEN4_INTCSYS_TMU10	                    509
#define RCAR_GEN4_INTCSYS_TMU11	                    510
#define RCAR_GEN4_INTCSYS_TMU12	                    511
#define RCAR_GEN4_INTCSYS_TMU13	                    512
#define RCAR_GEN4_INTCSYS_TMU20	                    513
#define RCAR_GEN4_INTCSYS_TMU21	                    514
#define RCAR_GEN4_INTCSYS_TMU22	                    515
#define RCAR_GEN4_INTCSYS_TMU23	                    516
#define RCAR_GEN4_INTCSYS_TMU30	                    517
#define RCAR_GEN4_INTCSYS_TMU31	                    518
#define RCAR_GEN4_INTCSYS_TMU32	                    519
#define RCAR_GEN4_INTCSYS_TMU33	                    520
#define RCAR_GEN4_INTCSYS_TMU40	                    521
#define RCAR_GEN4_INTCSYS_TMU41	                    522
#define RCAR_GEN4_INTCSYS_TMU42	                    523
#define RCAR_GEN4_INTCSYS_TMU43	                    524
#define RCAR_GEN4_INTCSYS_WWDT0_NMI	                525
#define RCAR_GEN4_INTCSYS_WWDT0_75P	                526
#define RCAR_GEN4_INTCSYS_WWDT1_NMI	                527
#define RCAR_GEN4_INTCSYS_WWDT1_75P	                528
#define RCAR_GEN4_INTCSYS_WWDT2_NMI	                529
#define RCAR_GEN4_INTCSYS_WWDT2_75P	                530
#define RCAR_GEN4_INTCSYS_WWDT3_NMI	                531
#define RCAR_GEN4_INTCSYS_WWDT3_75P	                532
#define RCAR_GEN4_INTCSYS_WWDT4_NMI	                533
#define RCAR_GEN4_INTCSYS_WWDT4_75P	                534
#define RCAR_GEN4_INTCSYS_WWDT5_NMI	                535
#define RCAR_GEN4_INTCSYS_WWDT5_75P	                536
#define RCAR_GEN4_INTCSYS_WWDT6_NMI	                537
#define RCAR_GEN4_INTCSYS_WWDT6_75P	                538
#define RCAR_GEN4_INTCSYS_WWDT7_NMI	                539
#define RCAR_GEN4_INTCSYS_WWDT7_75P	                540
#define RCAR_GEN4_INTCSYS_WWDT8_NMI	                541
#define RCAR_GEN4_INTCSYS_WWDT8_75P	                542
#define RCAR_GEN4_INTCSYS_WWDT9_NMI	                543
#define RCAR_GEN4_INTCSYS_WWDT9_75P	                544
#define RCAR_GEN4_INTCSYS_SCMT	                    545
#define RCAR_GEN4_INTCSYS_SUCMT	                    546
#define RCAR_GEN4_INTCSYS_RWDT	                    547
#define RCAR_GEN4_INTCSYS_SWDT	                    548
#define RCAR_GEN4_INTCSYS_MCU_TOP_0	                554
#define RCAR_GEN4_INTCSYS_MCU_TOP_1	                555
#define RCAR_GEN4_INTCSYS_MCU_TOP_2	                556
#define RCAR_GEN4_INTCSYS_MCU_TOP_3	                557
#define RCAR_GEN4_INTCSYS_MCU_TOP_4	                558
#define RCAR_GEN4_INTCSYS_MCU_TOP_5	                559
#define RCAR_GEN4_INTCSYS_MCU_TOP_6	                560
#define RCAR_GEN4_INTCSYS_MCU_TOP_7	                561
#define RCAR_GEN4_INTCSYS_MCU_TOP_8	                562
#define RCAR_GEN4_INTCSYS_MCU_TOP_9	                563
#define RCAR_GEN4_INTCSYS_MCU_TOP_10	                564
#define RCAR_GEN4_INTCSYS_MCU_TOP_11	                565
#define RCAR_GEN4_INTCSYS_MCU_TOP_12	                566
#define RCAR_GEN4_INTCSYS_MCU_TOP_13	                567
#define RCAR_GEN4_INTCSYS_MCU_TOP_14	                568
#define RCAR_GEN4_INTCSYS_MCU_TOP_15	                569
#define RCAR_GEN4_INTCSYS_MCU_TOP_16	                570
#define RCAR_GEN4_INTCSYS_MCU_TOP_17	                571
#define RCAR_GEN4_INTCSYS_MCU_TOP_18	                572
#define RCAR_GEN4_INTCSYS_MCU_TOP_19	                573
#define RCAR_GEN4_INTCSYS_MCU_TOP_20	                574
#define RCAR_GEN4_INTCSYS_MCU_TOP_21	                575
#define RCAR_GEN4_INTCSYS_MCU_TOP_22	                576
#define RCAR_GEN4_INTCSYS_MCU_TOP_23	                577
#define RCAR_GEN4_INTCSYS_MCU_TOP_24	                578
#define RCAR_GEN4_INTCSYS_MCU_TOP_25	                579
#define RCAR_GEN4_INTCSYS_MCU_TOP_26	                580
#define RCAR_GEN4_INTCSYS_MCU_TOP_27	                581
#define RCAR_GEN4_INTCSYS_MCU_TOP_28	                582
#define RCAR_GEN4_INTCSYS_MCU_TOP_29	                583
#define RCAR_GEN4_INTCSYS_MCU_TOP_30	                584
#define RCAR_GEN4_INTCSYS_MCU_TOP_31	                585
#define RCAR_GEN4_INTCSYS_MCU_TOP_32	                586
#define RCAR_GEN4_INTCSYS_MCU_TOP_33	                587
#define RCAR_GEN4_INTCSYS_MCU_TOP_34	                588
#define RCAR_GEN4_INTCSYS_MCU_TOP_35	                589
#define RCAR_GEN4_INTCSYS_MCU_TOP_36	                590
#define RCAR_GEN4_INTCSYS_MCU_TOP_37	                591
#define RCAR_GEN4_INTCSYS_MCU_TOP_38	                592
#define RCAR_GEN4_INTCSYS_MCU_TOP_39	                593
#define RCAR_GEN4_INTCSYS_MCU_TOP_40	                594
#define RCAR_GEN4_INTCSYS_MCU_TOP_41	                595
#define RCAR_GEN4_INTCSYS_MCU_TOP_42	                596
#define RCAR_GEN4_INTCSYS_MCU_TOP_43	                597
#define RCAR_GEN4_INTCSYS_MCU_TOP_44	                598
#define RCAR_GEN4_INTCSYS_MCU_TOP_45	                599
#define RCAR_GEN4_INTCSYS_MCU_TOP_46	                600
#define RCAR_GEN4_INTCSYS_MCU_TOP_47	                601
#define RCAR_GEN4_INTCSYS_MCU_TOP_48	                602
#define RCAR_GEN4_INTCSYS_MCU_TOP_49	                603
#define RCAR_GEN4_INTCSYS_MCU_TOP_50	                604
#define RCAR_GEN4_INTCSYS_MCU_TOP_51	                605
#define RCAR_GEN4_INTCSYS_MCU_TOP_52	                606
#define RCAR_GEN4_INTCSYS_MCU_TOP_53	                607
#define RCAR_GEN4_INTCSYS_MCU_TOP_54	                608
#define RCAR_GEN4_INTCSYS_MCU_TOP_55	                609
#define RCAR_GEN4_INTCSYS_MCU_TOP_56	                610
#define RCAR_GEN4_INTCSYS_MCU_TOP_57	                611
#define RCAR_GEN4_INTCSYS_MCU_TOP_58	                612
#define RCAR_GEN4_INTCSYS_MCU_TOP_59	                613
#define RCAR_GEN4_INTCSYS_MCU_TOP_60                  614
#define RCAR_GEN4_INTCSYS_MCU_TOP_61                  615
#define RCAR_GEN4_INTCSYS_MCU_TOP_62                  616
#define RCAR_GEN4_INTCSYS_MCU_TOP_63                  617
#define RCAR_GEN4_INTCSYS_MCU_TOP_64                  618
#define RCAR_GEN4_INTCSYS_MCU_TOP_65                  619
#define RCAR_GEN4_INTCSYS_MCU_TOP_66                  620
#define RCAR_GEN4_INTCSYS_MCU_TOP_67                  621
#define RCAR_GEN4_INTCSYS_MCU_TOP_68                  622
#define RCAR_GEN4_INTCSYS_MCU_TOP_69                  623
#define RCAR_GEN4_INTCSYS_MCU_TOP_70                  624
#define RCAR_GEN4_INTCSYS_MCU_TOP_71                  625
#define RCAR_GEN4_INTCSYS_MCU_TOP_72                  626
#define RCAR_GEN4_INTCSYS_MCU_TOP_73                  627
#define RCAR_GEN4_INTCSYS_MCU_TOP_74                  628
#define RCAR_GEN4_INTCSYS_MCU_TOP_75                  629
#define RCAR_GEN4_INTCSYS_MCU_TOP_76                  630
#define RCAR_GEN4_INTCSYS_MCU_TOP_77                  631
#define RCAR_GEN4_INTCSYS_MCU_TOP_78                  632
#define RCAR_GEN4_INTCSYS_MCU_TOP_79                  633
#define RCAR_GEN4_INTCSYS_FBC_END_0	                644
#define RCAR_GEN4_INTCSYS_FBC_END_1	                645
#define RCAR_GEN4_INTCSYS_FBC_END_2	                646
#define RCAR_GEN4_INTCSYS_FBC_END_3	                647
#define RCAR_GEN4_INTCSYS_FBC_END_4	                648
#define RCAR_GEN4_INTCSYS_FBC_END_5	                649
#define RCAR_GEN4_INTCSYS_FBC_END_6	                650
#define RCAR_GEN4_INTCSYS_FBC_END_7	                651
#define RCAR_GEN4_INTCSYS_FBC_END_8	                652
#define RCAR_GEN4_INTCSYS_FBC_END_9	                653
#define RCAR_GEN4_INTCSYS_FBC_END_10	                654
#define RCAR_GEN4_INTCSYS_FBC_END_11	                655
#define RCAR_GEN4_INTCSYS_FBC_END_12	                656
#define RCAR_GEN4_INTCSYS_FBC_END_13	                657
#define RCAR_GEN4_INTCSYS_FBC_END_14	                658
#define RCAR_GEN4_INTCSYS_FBC_END_15	                659
#define RCAR_GEN4_INTCSYS_FBC_END_16	                660
#define RCAR_GEN4_INTCSYS_FBC_END_17	                661
#define RCAR_GEN4_INTCSYS_FBC_END_18	                662
#define RCAR_GEN4_INTCSYS_FBC_END_19	                663
#define RCAR_GEN4_INTCSYS_FBC_END_20	                664
#define RCAR_GEN4_INTCSYS_FBC_END_21	                665
#define RCAR_GEN4_INTCSYS_FBC_END_22	                666
#define RCAR_GEN4_INTCSYS_FBC_END_23	                667
#define RCAR_GEN4_INTCSYS_FBC_END_24	                668
#define RCAR_GEN4_INTCSYS_FBC_END_25	                669
#define RCAR_GEN4_INTCSYS_FBC_END_26	                670
#define RCAR_GEN4_INTCSYS_FBC_END_27                  671
#define RCAR_GEN4_INTCSYS_FBC_END_28                  672
#define RCAR_GEN4_INTCSYS_FBC_END_29                  673
#define RCAR_GEN4_INTCSYS_FBC_END_30                  674
#define RCAR_GEN4_INTCSYS_FBC_END_31                  675
#define RCAR_GEN4_INTCSYS_FBC_END_32                  676
#define RCAR_GEN4_INTCSYS_FBC_END_33                  677
#define RCAR_GEN4_INTCSYS_FBC_END_34                  678
#define RCAR_GEN4_INTCSYS_FBC_END_35                  679
#define RCAR_GEN4_INTCSYS_FBC_END_36                  680
#define RCAR_GEN4_INTCSYS_FBC_END_37                  681
#define RCAR_GEN4_INTCSYS_FBC_END_38                  682
#define RCAR_GEN4_INTCSYS_FBC_END_39                  683
#define RCAR_GEN4_INTCSYS_FBC_END_40                  684
#define RCAR_GEN4_INTCSYS_FBC_END_41                  685
#define RCAR_GEN4_INTCSYS_FBC_END_42                  686
#define RCAR_GEN4_INTCSYS_FBC_END_43                  687
#define RCAR_GEN4_INTCSYS_FBC_END_44                  688
#define RCAR_GEN4_INTCSYS_FBC_END_45                  689
#define RCAR_GEN4_INTCSYS_FBC_END_46                  690
#define RCAR_GEN4_INTCSYS_FBC_END_47                  691
#define RCAR_GEN4_INTCSYS_FBC_END_48                  692
#define RCAR_GEN4_INTCSYS_FBC_END_49                  693
#define RCAR_GEN4_INTCSYS_FBC_END_50                  694
#define RCAR_GEN4_INTCSYS_FBC_END_51                  695
#define RCAR_GEN4_INTCSYS_FBC_END_52                  696
#define RCAR_GEN4_INTCSYS_FBC_END_53                  697
#define RCAR_GEN4_INTCSYS_FBC_END_54                  698
#define RCAR_GEN4_INTCSYS_FBC_END_55                  699
#define RCAR_GEN4_INTCSYS_FBC_END_56                  700
#define RCAR_GEN4_INTCSYS_FBC_END_57                  701
#define RCAR_GEN4_INTCSYS_FBC_END_58                  702
#define RCAR_GEN4_INTCSYS_FBC_END_59                  703
#define RCAR_GEN4_INTCSYS_FBC_END_60                  704
#define RCAR_GEN4_INTCSYS_FBC_END_61                  705
#define RCAR_GEN4_INTCSYS_FBC_END_62                  706
#define RCAR_GEN4_INTCSYS_FBC_END_63                  707
#define RCAR_GEN4_INTCSYS_FBC_START_0	                708
#define RCAR_GEN4_INTCSYS_FBC_START_1	                709
#define RCAR_GEN4_INTCSYS_FBC_START_2	                710
#define RCAR_GEN4_INTCSYS_FBC_START_3	                711
#define RCAR_GEN4_INTCSYS_FBC_START_4	                712
#define RCAR_GEN4_INTCSYS_FBC_START_5	                713
#define RCAR_GEN4_INTCSYS_FBC_START_6	                714
#define RCAR_GEN4_INTCSYS_FBC_START_7	                715
#define RCAR_GEN4_INTCSYS_FBC_START_8	                716
#define RCAR_GEN4_INTCSYS_FBC_START_9	                717
#define RCAR_GEN4_INTCSYS_FBC_START_10	            718
#define RCAR_GEN4_INTCSYS_FBC_START_11	            719
#define RCAR_GEN4_INTCSYS_FBC_START_12	            720
#define RCAR_GEN4_INTCSYS_FBC_START_13	            721
#define RCAR_GEN4_INTCSYS_FBC_START_14	            722
#define RCAR_GEN4_INTCSYS_FBC_START_15	            723
#define RCAR_GEN4_INTCSYS_FBC_START_16	            724
#define RCAR_GEN4_INTCSYS_FBC_START_17	            725
#define RCAR_GEN4_INTCSYS_FBC_START_18	            726
#define RCAR_GEN4_INTCSYS_FBC_START_19	            727
#define RCAR_GEN4_INTCSYS_FBC_START_20                728
#define RCAR_GEN4_INTCSYS_FBC_START_21                729
#define RCAR_GEN4_INTCSYS_FBC_START_22                730
#define RCAR_GEN4_INTCSYS_FBC_START_23                731
#define RCAR_GEN4_INTCSYS_FBC_START_24                732
#define RCAR_GEN4_INTCSYS_FBC_START_25                733
#define RCAR_GEN4_INTCSYS_FBC_START_26                734
#define RCAR_GEN4_INTCSYS_FBC_START_27                735
#define RCAR_GEN4_INTCSYS_FBC_START_28                736
#define RCAR_GEN4_INTCSYS_FBC_START_29                737
#define RCAR_GEN4_INTCSYS_FBC_START_30                738
#define RCAR_GEN4_INTCSYS_FBC_START_31                739
#define RCAR_GEN4_INTCSYS_FBC_START_32                740
#define RCAR_GEN4_INTCSYS_FBC_START_33                741
#define RCAR_GEN4_INTCSYS_FBC_START_34                742
#define RCAR_GEN4_INTCSYS_FBC_START_35                743
#define RCAR_GEN4_INTCSYS_FBC_START_36                744
#define RCAR_GEN4_INTCSYS_FBC_START_37                745
#define RCAR_GEN4_INTCSYS_FBC_START_38                746
#define RCAR_GEN4_INTCSYS_FBC_START_39                747
#define RCAR_GEN4_INTCSYS_FBC_START_40                748
#define RCAR_GEN4_INTCSYS_FBC_START_41                749
#define RCAR_GEN4_INTCSYS_FBC_START_42                750
#define RCAR_GEN4_INTCSYS_FBC_START_43                751
#define RCAR_GEN4_INTCSYS_FBC_START_44                752
#define RCAR_GEN4_INTCSYS_FBC_START_45                753
#define RCAR_GEN4_INTCSYS_FBC_START_46                754
#define RCAR_GEN4_INTCSYS_FBC_START_47                755
#define RCAR_GEN4_INTCSYS_FBC_START_48                756
#define RCAR_GEN4_INTCSYS_FBC_START_49                757
#define RCAR_GEN4_INTCSYS_FBC_START_50                758
#define RCAR_GEN4_INTCSYS_FBC_START_51                759
#define RCAR_GEN4_INTCSYS_FBC_START_52                760
#define RCAR_GEN4_INTCSYS_FBC_START_53                761
#define RCAR_GEN4_INTCSYS_FBC_START_54                762
#define RCAR_GEN4_INTCSYS_FBC_START_55                763
#define RCAR_GEN4_INTCSYS_FBC_START_56                764
#define RCAR_GEN4_INTCSYS_FBC_START_57                765
#define RCAR_GEN4_INTCSYS_FBC_START_58                766
#define RCAR_GEN4_INTCSYS_FBC_START_59                767
#define RCAR_GEN4_INTCSYS_FBC_START_60                768
#define RCAR_GEN4_INTCSYS_FBC_START_61                769
#define RCAR_GEN4_INTCSYS_FBC_START_62                770
#define RCAR_GEN4_INTCSYS_FBC_START_63	            771
#define RCAR_GEN4_INTCSYS_FBC_START_OR	            772
#define RCAR_GEN4_INTCSYS_FBC_FINISH_OR	            773
#define RCAR_GEN4_INTCSYS_ADVFS_0	                    777
#define RCAR_GEN4_INTCSYS_ADVFS_1	                    778
#define RCAR_GEN4_INTCSYS_ECM_ERR0	                784
#define RCAR_GEN4_INTCSYS_ECM_ERR1	                785
#define RCAR_GEN4_INTCSYS_ECM_ERR2	                786
#define RCAR_GEN4_INTCSYS_ECM_ERR3	                787
#define RCAR_GEN4_INTCSYS_ECM_ERR4	                788
#define RCAR_GEN4_INTCSYS_ECM_ERR5	                789
#define RCAR_GEN4_INTCSYS_ECM_ERR6	                790
#define RCAR_GEN4_INTCSYS_ECM_ERR7	                791
#define RCAR_GEN4_INTCSYS_ECM_ERR8	                792
#define RCAR_GEN4_INTCSYS_ECM_ERR9	                793
#define RCAR_GEN4_INTCSYS_ECM_ERR10	                794
#define RCAR_GEN4_INTCSYS_ECM_ERR11	                795
#define RCAR_GEN4_INTCSYS_ECM_ERR12	                796
#define RCAR_GEN4_INTCSYS_ECM_ERR13	                797
#define RCAR_GEN4_INTCSYS_ECM_ERR14	                798
#define RCAR_GEN4_INTCSYS_ECM_ERR15	                799
#define RCAR_GEN4_INTCSYS_ECM_ERR16	                800
#define RCAR_GEN4_INTCSYS_ECM_ERR17	                801
#define RCAR_GEN4_INTCSYS_ECM_ERR18	                802
#define RCAR_GEN4_INTCSYS_ECM_ERR19	                803
#define RCAR_GEN4_INTCSYS_ECM_ERR20	                804
#define RCAR_GEN4_INTCSYS_ECM_ERR21	                805
#define RCAR_GEN4_INTCSYS_ECM_ERR22	                806
#define RCAR_GEN4_INTCSYS_ECM_ERR23	                807
#define RCAR_GEN4_INTCSYS_ECM_ERR24	                808
#define RCAR_GEN4_INTCSYS_ECM_ERR25	                809
#define RCAR_GEN4_INTCSYS_ECM_ERR26	                810
#define RCAR_GEN4_INTCSYS_ECM_ERR27	                811
#define RCAR_GEN4_INTCSYS_ECM_ERR28	                812
#define RCAR_GEN4_INTCSYS_ECM_ERR29	                813
#define RCAR_GEN4_INTCSYS_ECM_ERR30	                814
#define RCAR_GEN4_INTCSYS_ECM_ERR31	                815
#define RCAR_GEN4_INTCSYS_ECM_ERR32	                816
#define RCAR_GEN4_INTCSYS_ECM_ERR33	                817
#define RCAR_GEN4_INTCSYS_ECM_ERR34	                818
#define RCAR_GEN4_INTCSYS_ECM_ERR35	                819
#define RCAR_GEN4_INTCSYS_ECM_ERR36	                820
#define RCAR_GEN4_INTCSYS_ECM_ERR37	                821
#define RCAR_GEN4_INTCSYS_ECM_ERR38	                822
#define RCAR_GEN4_INTCSYS_ECM_ERR39	                823
#define RCAR_GEN4_INTCSYS_ECM_ERR40	                824
#define RCAR_GEN4_INTCSYS_ECM_ERR41	                825
#define RCAR_GEN4_INTCSYS_ECM_ERR42	                826
#define RCAR_GEN4_INTCSYS_ECM_ERR43	                827
#define RCAR_GEN4_INTCSYS_ECM_ERR44	                828
#define RCAR_GEN4_INTCSYS_ECM_ERR45	                829
#define RCAR_GEN4_INTCSYS_ECM_ERR46	                830
#define RCAR_GEN4_INTCSYS_ECM_ERR47	                831
#define RCAR_GEN4_INTCSYS_ECM_ERR48	                832
#define RCAR_GEN4_INTCSYS_ECM_ERR49	                833
#define RCAR_GEN4_INTCSYS_ECM_ERR50	                834
#define RCAR_GEN4_INTCSYS_ECM_ERR51	                835
#define RCAR_GEN4_INTCSYS_ECM_ERR52	                836
#define RCAR_GEN4_INTCSYS_ECM_ERR53	                837
#define RCAR_GEN4_INTCSYS_ECM_ERR54	                838
#define RCAR_GEN4_INTCSYS_ECM_ERR55	                839
#define RCAR_GEN4_INTCSYS_ECM_ERR56	                840
#define RCAR_GEN4_INTCSYS_ECM_ERR57	                841
#define RCAR_GEN4_INTCSYS_ECM_ERR58	                842
#define RCAR_GEN4_INTCSYS_ECM_ERR59	                843
#define RCAR_GEN4_INTCSYS_ECM_ERR60	                844
#define RCAR_GEN4_INTCSYS_ECM_ERR61	                845
#define RCAR_GEN4_INTCSYS_ECM_ERR62	                846
#define RCAR_GEN4_INTCSYS_ECM_ERR63	                847
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_0	            854
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_1	            855
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_2	            856
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_3	            857
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_4	            858
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_5	            859
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_6	            860
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_7	            861
#define RCAR_GEN4_INTCSYS_ECM_ECMPER0	                941
#define RCAR_GEN4_INTCSYS_ECM_ECMHSC	                942
#define RCAR_GEN4_INTCSYS_ECM_ECMMM	                943
#define RCAR_GEN4_INTCSYS_ECM_ECMRT	                944
#define RCAR_GEN4_INTCSYS_ECM_ECMTOP	                945
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_8	            948
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_9	            949
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_10	            950
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_11	            951
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_12	            952
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_13	            953
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_14	            954
#define RCAR_GEN4_INTCSYS_PFCSOC0_PORT_15	            955
#define RCAR_GEN4_INTCSYS_ECM_ECMRC	                956
#define RCAR_GEN4_INTCSYS_ECM_ECMRD	                957
#define RCAR_GEN4_INTCSYS_ECM_ECMRT0	                958
#define RCAR_GEN4_INTCSYS_ECM_ECMSTM	                959
#define RCAR_GEN4_INTCSYS_ECM_ECMSD0	                960
#define RCAR_GEN4_INTCSYS_ECM_ECMSP3	                961
#define RCAR_GEN4_INTCSYS_ECM_ECMSP4	                962
#define RCAR_GEN4_INTCSYS_ECM_ECMPS0	                963
#define RCAR_GEN4_INTCSYS_TSIPL_CH0_0	                964
#define RCAR_GEN4_INTCSYS_TSIPL_CH0_1	                965
#define RCAR_GEN4_INTCSYS_TSIPL_CH1_0	                966
#define RCAR_GEN4_INTCSYS_TSIPL_CH1_1	                967
#define RCAR_GEN4_INTCSYS_TSIPL_CH2_0	                968
#define RCAR_GEN4_INTCSYS_TSIPL_CH2_1	                969
#define RCAR_GEN4_INTCSYS_TSIPL_CH3_0	                970
#define RCAR_GEN4_INTCSYS_TSIPL_CH3_1	                971
#define RCAR_GEN4_INTCSYS_TSIPL_CH4_0	                972
#define RCAR_GEN4_INTCSYS_TSIPL_CH4_1	                973
#define RCAR_GEN4_INTCSYS_TSIPL_CH5_0	                974
#define RCAR_GEN4_INTCSYS_TSIPL_CH5_1	                975
#define RCAR_GEN4_INTCSYS_TSIPL_CH6_0	                976
#define RCAR_GEN4_INTCSYS_TSIPL_CH6_1	                977
#define RCAR_GEN4_INTCSYS_TSIPL_CH7_0	                978
#define RCAR_GEN4_INTCSYS_TSIPL_CH7_1	                979
#define RCAR_GEN4_INTCSYS_TSIP_CH0_0	                980
#define RCAR_GEN4_INTCSYS_TSIP_CH0_1	                981
#define RCAR_GEN4_INTCSYS_TSIP_CH1_0	                982
#define RCAR_GEN4_INTCSYS_TSIP_CH1_1	                983
#define RCAR_GEN4_INTCSYS_TSIP_CH2_0	                984
#define RCAR_GEN4_INTCSYS_TSIP_CH2_1	                985
#define RCAR_GEN4_INTCSYS_WCRC0	                    986
#define RCAR_GEN4_INTCSYS_WCRC1	                    987
#define RCAR_GEN4_INTCSYS_WCRC2	                    988
#define RCAR_GEN4_INTCSYS_WCRC3	                    989

/*
 * ECM base for V3U and S4
 */
#define RCAR_GEN4_ECM_BASE              0xE6250000
#define RCAR_GEN4_ECM_SIZE              0x1000


/*
 * ECM offset for S4 and V4H
 */
 #define RCAR_GEN4_ECM_ERRCTLR(n)         (0x0000 + ((n) * 4))
 #define RCAR_GEN4_ECM_ERRSTSR(n)         (0x0100 + ((n) * 4))
 #define RCAR_GEN4_ECM_ERRTGTR(n)         (0x0200 + ((n) * 4))
 #define RCAR_GEN4_ECM_ERRCNTR(n)         (0x0400 + ((n) * 4))
 #define RCAR_GEN4_ECM_ERRFATALR(n)       (0x0600 + ((n) * 4))
 #define RCAR_GEN4_ECM_PSSTATCTLRA(n)     (0x0700 + ((n) * 4))
 #define RCAR_GEN4_ECM_PSSTATCTLRB(n)     (0x0800 + ((n) * 4))
 #define RCAR_GEN4_ECM_PSSTATCTLRM         0x0900
 #define RCAR_GEN4_ECM_GEIIDR              0x0928
 #define RCAR_GEN4_ECM_SAFSTSR             0x0940
 #define RCAR_GEN4_ECM_SAFSTERRENR         0x0944
 #define RCAR_GEN4_ECM_SAFCLERRENR         0x0948
 #define RCAR_GEN4_ECM_EDCERRSIDPADDR      0x095C
 #define RCAR_GEN4_ECM_EDCERRSIDPWDATAR    0x0960
 #define RCAR_GEN4_ECM_DCLSERMON00R        0x0984
 #define RCAR_GEN4_ECM_DCLSERMON01R        0x0988
 #define RCAR_GEN4_ECM_WPCNTR              0x0A00
 #define RCAR_GEN4_ECM_WACNTR              0x0A04
/*
 * ECM offset for only V4H
 *
*/
 #define RCAR_GEN4_ECM_EXTRQHLDCNTR        0x0A10
 #define RCAR_GEN4_ECM_EXTRQMSKCNTR        0x0A14
 #define RCAR_GEN4_ECM_EXTRQSTSR           0x0A18
 #define RCAR_GEN4_ECM_ERRSTSINR           0x0A28
 #define RCAR_GEN4_ECM_ERROUTCTLR          0x0A2C
 #define RCAR_GEN4_ECM_DYNCTRLR            0x0A30
 #define RCAR_GEN4_ECM_DYNFREQSELR         0x0A34
/*--------------------------------------------------
 *
 * The following is for Gen3 and Gen4 (All R-Car SoC)
 *
 *--------------------------------------------------*/

/*
 * CMT
 */
#define RCAR_GEN4_CMT0_BASE              0xE60F0000
#define RCAR_GEN4_CMT1_BASE              0xE6130000
#define RCAR_GEN4_CMT2_BASE              0xE6140000
#define RCAR_GEN4_CMT3_BASE              0xE6148000

#define RCAR_GEN4_CMT0_REG_OFFSET        0x500
#define RCAR_GEN4_CMT1_REG_OFFSET        0x000
#define RCAR_GEN4_CMT_REG_SIZE           0x100
#define RCAR_GEN4_CMT_SIZE               (0x1000 + RCAR_GEN4_CMT_REG_SIZE)

 #define RCAR_GEN4_CMT_CMCLKE            0x1000
 #define RCAR_GEN4_CMT_CMSTR             0x0000
 #define RCAR_GEN4_CMT_CMCSR             0x0010
 #define RCAR_GEN4_CMT_CMCNT             0x0014
 #define RCAR_GEN4_CMT_CMCOR             0x0018
 #define RCAR_GEN4_CMT_CMCSRH            0x0020
 #define RCAR_GEN4_CMT_CMCNTH            0x0024
 #define RCAR_GEN4_CMT_CMCORH            0x0028
 #define RCAR_GEN4_CMT_CMCSRM            0x0040
 #define RCAR_GEN4_CMT_CMCNTM            0x0044

/*
 * PWM
 */
#define RCAR_GEN4_PWM0_BASE              0xE6E30000
#define RCAR_GEN4_PWM1_BASE              0xE6E31000
#define RCAR_GEN4_PWM2_BASE              0xE6E32000
#define RCAR_GEN4_PWM3_BASE              0xE6E33000
#define RCAR_GEN4_PWM4_BASE              0xE6E34000
#define RCAR_GEN4_PWM5_BASE              0xE6E35000
#define RCAR_GEN4_PWM6_BASE              0xE6E36000
#define RCAR_GEN4_PWM_SIZE               0x1000

#define RCAR_GEN4_PWM_PWMCR              0x0000
#define RCAR_GEN4_PWM_PWMCNT             0x0004

/* V3H only */
#define RCAR_GEN4_PWM_PWMEI              0x000C

/*
 * Timer Unit (TMU)
 */
#define RCAR_GEN4_TMU_0_2_BASE          0xE61E0000
#define RCAR_GEN4_TMU_3_5_BASE          0xE6FC0000
#define RCAR_GEN4_TMU_6_8_BASE          0xE6FD0000
#define RCAR_GEN4_TMU_9_11_BASE         0xE6FE0000
#define RCAR_GEN4_TMU_12_14_BASE        0xFFC00000
#define RCAR_GEN4_TMU_SIZE              0x30

#define RCAR_GEN4_TMU_TSTR              0x04
    #define RCAR_GEN4_TMU_START0        (1 << 0) // for channel 0, 3, 6, 9, 12
    #define RCAR_GEN4_TMU_START1        (1 << 1) // for channel 1, 4, 7, 10, 13
    #define RCAR_GEN4_TMU_START2        (1 << 2) // for channel 2, 5, 8, 11, 14

#define RCAR_GEN4_TMU_TCOR0             0x08 // for channel 0, 3, 6, 9, 12
#define RCAR_GEN4_TMU_TCOR1             0x14 // for channel 1, 4, 7, 10, 13
#define RCAR_GEN4_TMU_TCOR2             0x20 // for channel 2, 5, 8, 11, 14
#define RCAR_GEN4_TMU_TCNT0             0x0C // for channel 0, 3, 6, 9, 12
#define RCAR_GEN4_TMU_TCNT1             0x18 // for channel 1, 4, 7, 10, 13
#define RCAR_GEN4_TMU_TCNT2             0x24 // for channel 2, 5, 8, 11, 14
#define RCAR_GEN4_TMU_TCR0              0x10 // for channel 0, 3, 6, 9, 12
#define RCAR_GEN4_TMU_TCR1              0x1C // for channel 1, 4, 7, 10, 13
#define RCAR_GEN4_TMU_TCR2              0x28 // for channel 2, 5, 8, 11, 14
#define RCAR_GEN4_TMU_TCPR2             0x2C // for channel 5, 8, 11, 14
 #define RCAR_GEN4_TMU_UNIE             (1 << 5)
 #define RCAR_GEN4_TMU_UNF              (1 << 8)

/*
 * CAN
 */
#define RCAR_GEN4_CAN0_BASE              0xE6C30000
#define RCAR_GEN4_CAN1_BASE              0xE6C38000
#define RCAR_GEN4_CAN_SIZE               0x600
#define RCAR_GEN4_CAN_REG_OFFSET         0x400

/*
 * CANFD
 */
#define RCAR_GEN4_CANFD_BASE             0xE66C0000
#define RCAR_GEN4_CANFD_SIZE             0x10000

/*
 * Watchdog Timer
 */
#define RCAR_GEN4_RWDT_BASE              0xE6020000  // RCLK Watchdog Timer
#define RCAR_GEN4_SWDT_BASE              0xE6030000  // Secure Watchdog Timer
 #define RCAR_WDT_CNT               0x00        // Count Register
 #define RCAR_WDT_CSRA              0x04        // Control/Status Register A
 #define RCAR_WDT_CSRB              0x08        // Control/Status Register B
 #define RCAR_WDT_SIZE              0x0C

/*
 * ARM Generic Timer
 */
#define RCAR_GEN4_ARM_CNTR_BASE          0xE6080000  // ARM Generic Timer
 #define RCAR_GEN4_ARM_CNTR_CNTCR        0x00
  #define RCAR_GEN4_ARM_CNTR_CNTCR_EN    (1 << 0)
 #define RCAR_GEN4_ARM_CNTR_CNTFID       0x20

/*
 * Audio modules
 */


/*
 * Audio Clock Generator
 */
#define RCAR_GEN4_ADG_BASE               0xEC5A0000
#define RCAR_GEN4_ADG_SIZE               0x200

/* Offset from base */
#define RCAR_GEN4_ADG_BRRA              0x00
#define RCAR_GEN4_ADG_BRRB              0x04
#define RCAR_GEN4_ADG_BRGCKR            0x08
#define RCAR_GEN4_ADG_SSICKR            0x08
#define RCAR_GEN4_ADG_CKSEL0            0x0C
#define RCAR_GEN4_ADG_CKSEL1            0x10
#define RCAR_GEN4_ADG_CKSEL2            0x14
#define RCAR_GEN4_ADG_TIM_EN            0x30
#define RCAR_GEN4_ADG_SRCIN_TIMSEL0     0x34
#define RCAR_GEN4_ADG_SRCIN_TIMSEL1     0x38
#define RCAR_GEN4_ADG_SRCIN_TIMSEL2     0x3C
#define RCAR_GEN4_ADG_SRCIN_TIMSEL3     0x40
#define RCAR_GEN4_ADG_SRCIN_TIMSEL4     0x44
#define RCAR_GEN4_ADG_SRCOUT_TIMSEL0    0x48
#define RCAR_GEN4_ADG_SRCOUT_TIMSEL1    0x4C
#define RCAR_GEN4_ADG_SRCOUT_TIMSEL2    0x50
#define RCAR_GEN4_ADG_SRCOUT_TIMSEL3    0x54
#define RCAR_GEN4_ADG_SRCOUT_TIMSEL4    0x58
#define RCAR_GEN4_ADG_CMDOUT_TIMSEL     0x5C
#define RCAR_GEN4_ADG_DTCP_TIMSEL       0x64
#define RCAR_GEN4_ADG_ADSP_TIMSEL1      0x80
#define RCAR_GEN4_ADG_ADSP_TIMSEL2      0x84
#define RCAR_GEN4_ADG_ADSP_TIMSEL3      0x88
#define RCAR_GEN4_ADG_SSICKR1           0x100
#define RCAR_GEN4_ADG_AVBCKR            0x100
#define RCAR_GEN4_ADG_AVB_SYNC_SEL0     0x104
#define RCAR_GEN4_ADG_AVB_SYNC_SEL1     0x10C
#define RCAR_GEN4_ADG_AVB_SYNC_SEL2     0x110
#define RCAR_GEN4_ADG_AVB_SYNC_DIV0     0x114
#define RCAR_GEN4_ADG_AVB_SYNC_DIV1     0x118
#define RCAR_GEN4_ADG_AVB_CLK_DIV0      0x11C
#define RCAR_GEN4_ADG_AVB_CLK_DIV1      0x120
#define RCAR_GEN4_ADG_AVB_CLK_DIV2      0x124
#define RCAR_GEN4_ADG_AVB_CLK_DIV3      0x128
#define RCAR_GEN4_ADG_AVB_CLK_DIV4      0x12C
#define RCAR_GEN4_ADG_AVB_CLK_DIV5      0x130
#define RCAR_GEN4_ADG_AVB_CLK_DIV6      0x134
#define RCAR_GEN4_ADG_AVB_CLK_DIV7      0x138
#define RCAR_GEN4_ADG_AVB_CLK_CONFIG    0x13C
#define RCAR_GEN4_ADG_AVB_CLK_STATUS    0x140

/* SCU */
#define RCAR_GEN4_SCU_BASE               0xEC500000
#define RCAR_GEN4_SCU_SIZE               0x1000

/* SSIU */
#define RCAR_GEN4_SSIU_BASE             0xEC540000
#define RCAR_GEN4_SSIU_SIZE             0x1000
 #define RCAR_GEN4_SSIU_MODE0            0x800
 #define RCAR_GEN4_SSIU_MODE1            0x804
 #define RCAR_GEN4_SSIU_MODE2            0x808
 #define RCAR_GEN4_SSIU_MODE3            0x80C
 #define RCAR_GEN4_SSIU_CONTROL          0x810

/* V4H SSIU registers */
#define RCAR_GEN4_SSIU_SSI0_0_BUSIF_MODE    0x000   // SSI[n]_0 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_0_BUSIF_ADINR   0x004   // SSI[n]_0 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_0_BUSIF_DALIGN  0x008   // SSI[n]_0 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_MODE          0x00C   // SSI[n]_0 Mode  Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_CONTROL       0x010   // SSI[n]_0 Control Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_0_STATUS        0x014    // SSI[n]_0 Status  Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_INT_ENABLE_MAIN    0x018     // SSI[n]_0 Interrupt Enable Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_1_BUSIF_MODE    0x020   // SSI[n]_1 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_1_BUSIF_ADINR   0x024   // SSI[n]_1 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_1_BUSIF_DALIGN  0x028   // SSI[n]_1 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_2_BUSIF_MODE    0x040   // SSI[n]_2 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_2_BUSIF_ADINR   0x044   // SSI[n]_2 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_2_BUSIF_DALIGN  0x048   // SSI[n]_2 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_3_BUSIF_MODE    0x060   // SSI[n]_3 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_3_BUSIF_ADINR   0x064   // SSI[n]_3 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_3_BUSIF_DALIGN  0x068   // SSI[n]_3 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_4_BUSIF_MODE    0x500   // SSI[n]_4 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_4_BUSIF_ADINR   0x504   // SSI[n]_4 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_4_BUSIF_DALIGN  0x508   // SSI[n]_4 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_5_BUSIF_MODE    0x520   // SSI[n]_5 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_5_BUSIF_ADINR   0x524   // SSI[n]_5 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_5_BUSIF_DALIGN  0x528   // SSI[n]_5 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_6_BUSIF_MODE    0x540   // SSI[n]_6 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_6_BUSIF_ADINR   0x544   // SSI[n]_6 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_6_BUSIF_DALIGN  0x548   // SSI[n]_6 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_7_BUSIF_MODE    0x560   // SSI[n]_7 BUSIF Mode Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_7_BUSIF_ADINR   0x564   // SSI[n]_7 BUSIF Audio Information Register (n=0)
#define RCAR_GEN4_SSIU_SSI0_7_BUSIF_DALIGN  0x568   // SSI[n]_7 BUSIF Data Align Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_BUSIF_DALIGN2 0xA08   // SSI[n]_0 BUSIF Data Align2 Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_MODE2         0xA0C   // SSI[n]_0 Mode2  Register(n=0)
#define RCAR_GEN4_SSIU_SSI0_0_STATUS2       0xA14   // SSI[n]_0 Status2  Register(n=0)

#define RCAR_GEN4_SSIU_SSI_SYSTEM_STATUS0       0x840    // SSI System Status Register 0
#define RCAR_GEN4_SSIU_SSI_SYSTEM_STATUS2       0x848    // SSI System Status Register 2
#define RCAR_GEN4_SSIU_SSI_SYSTEM_INT_ENABLE0   0x850    // SSI System Interrupt Enable Register 0
#define RCAR_GEN4_SSIU_SSI_SYSTEM_INT_ENABLE2   0x858    // SSI System Interrupt Enable Register 2
#define RCAR_GEN4_SSIU_SSI_SYSTEM_INT_ENABLE4   0x890    // SSI System Interrupt Enable Register 4
#define RCAR_GEN4_SSIU_SSI_SYSTEM_INT_ENABLE6   0x898    // SSI System Interrupt Enable Register 6
#define RCAR_GEN4_SSIU_SSI_SYSTEM_STATUS4       0x880    // SSI System Status Register 4
#define RCAR_GEN4_SSIU_SSI_SYSTEM_STATUS6       0x888    // SSI System Status Register 6
#define RCAR_GEN4_SSIU_SSI_INT_ENABLE_MAIN2     0xA18    // SSI[n]_0 Interrupt Enable2 Register(n=0)

/* SSI */
#define RCAR_GEN4_SSI_BASE              0xEC541000
#define RCAR_GEN4_SSI_SIZE              0x280       // 10 Modules * 0x40 each
 #define RCAR_GEN4_SSI_CR                0x00        // Control Register
 #define RCAR_GEN4_SSI_SR                0x04        // Status Register
 #define RCAR_GEN4_SSI_TDR               0x08        // Transmit Data Register
 #define RCAR_GEN4_SSI_RDR               0x0C        // Receive Data Register
 #define RCAR_GEN4_SSI_WSR               0x20        // WS Mode Register
 #define RCAR_GEN4_SSI_FMR               0x24        // FS Mode Register
 #define RCAR_GEN4_SSI_FSR               0x28        // FS Status Register

/* AUDIO-DMAC */
 #define RCAR_GEN4_AUDIODMAC0_BASE       0xEC700000
 #define RCAR_GEN4_AUDIODMAC1_BASE       0xEC720000

/* AUDIO-DMAC-PP */
#define RCAR_GEN4_AUDIODMACPP_BASE       0xEC740000
#define RCAR_GEN4_AUDIODMACPP_EXT_BASE   0xEC760000
#define RCAR_GEN4_AUDIODMACPP_SIZE       0x1000
 #define RCAR_GEN4_PDMASAR(x)            (0x20 + 0x10 * (x))
 #define RCAR_GEN4_PDMADAR(x)            (0x24 + 0x10 * (x))
 #define RCAR_GEN4_PDMACHCR(x)           (0x2C + 0x10 * (x))

/*
 * Interrupt Controller (INTC-EX)
 */
#define RCAR_GEN4_IRQC0_BASE             0xE61C0000
#define RCAR_GEN4_NMI_BASE               0xE61C0400
 #define RCAR_GEN4_INTREQ_STS0           0x000       /* R */
 #define RCAR_GEN4_INTEN_STS0            0x004       /* R/WC1 */
 #define RCAR_GEN4_INTEN_SET0            0x008       /* W */
 #define RCAR_GEN4_INTREQ_STS1           0x010       /* R */
 #define RCAR_GEN4_INTEN_STS1            0x014       /* R/WC1 */
 #define RCAR_GEN4_INTEN_SET1            0x018       /* W */
 #define RCAR_GEN4_DETECT_STATUS         0x100       /* R/WC1 */
 #define RCAR_GEN4_CONFIG_00             0x180       /* R/W */

/*
 * SDHI
 */
#define RCAR_GEN4_SDHI0_BASE             0xEE100000  // SDHI0 Base
#define RCAR_GEN4_SDHI1_BASE             0xEE120000  // SDHI1 Base
#define RCAR_GEN4_SDHI2_BASE             0xEE140000  // SDHI2 Base
#define RCAR_GEN4_SDHI3_BASE             0xEE160000  // SDHI3 Base
#define RCAR_GEN4_SDHI_SIZE              0x2000

/*
 * MMCIF
 */
#define RCAR_GEN4_MMCIF0_BASE            0xEE140000
#define RCAR_GEN4_MMCIF1_BASE            0xEE160000
#define RCAR_GEN4_MMCIF_SIZE             0x2000

/*
 * SYS-DMAC for Gen4
 */
#define RCAR_GEN4_SYSDMAC1_BASE          0xE7300000
#define RCAR_GEN4_SYSDMAC2_BASE          0xE7310000
/*
 * SYS-DMAC for Gen4 (V3U, S4, V4H)
 */
#define RCAR_GEN4_SYSDMAC1_COM_BASE       0xE7350000
#define RCAR_GEN4_SYSDMAC2_COM_BASE       0xE7351000
#define RCAR_GEN4_SYSDMAC_COM_SIZE        0x1000

/* SYSDMAC register, offset from BASE to REGS */
#define RCAR_GEN4_SYSDMAC_DESC           0x0800
#define RCAR_GEN4_SYSDMAC_REGS           0x1000
#define RCAR_GEN4_SYSDMAC_CHREG_SIZE     0x200
#define RCAR_GEN4_SYSDMAC_SIZE           0xF200 // Size of 16 channels

/*
 * RT-DMAC for Gen4 (V3U, S4, V4H)
 */
#define RCAR_GEN4_RTDMAC0_BASE            0xFFC10000
#define RCAR_GEN4_RTDMAC1_BASE            0xFFC20000
#define RCAR_GEN4_RTDMAC2_BASE            0xFFD70000
#define RCAR_GEN4_RTDMAC3_BASE            0xFFD80000

#define RCAR_GEN4_RTDMAC0_COM_BASE        0xFFD60000
#define RCAR_GEN4_RTDMAC1_COM_BASE        0xFFD61000
#define RCAR_GEN4_RTDMAC2_COM_BASE        0xFFD62000
#define RCAR_GEN4_RTDMAC3_COM_BASE        0xFFD63000
#define RCAR_GEN4_RTDMAC_COM_SIZE         0x1000

/*
 * Offset for SYS-DMAC and RT-DMAC
 */

/* SYSDMAC registers, offset from BASE + SYSDMAC_REG */
#define RCAR_GEN4_DMAC_DMASAR        0x000
#define RCAR_GEN4_DMAC_DMADAR        0x004
#define RCAR_GEN4_DMAC_DMATCR        0x008
#define RCAR_GEN4_DMAC_DMACHCR       0x00C
#define RCAR_GEN4_DMAC_DMAFIXSAR     0x010
#define RCAR_GEN4_DMAC_DMAFIXDAR     0x014
#define RCAR_GEN4_DMAC_DMATCRB       0x018
#define RCAR_GEN4_DMAC_DMACHCRB      0x01C
#define RCAR_GEN4_DMAC_DMASART       0x020
#define RCAR_GEN4_DMAC_DMADART       0x024
#define RCAR_GEN4_DMAC_DMATSR        0x028
#define RCAR_GEN4_DMAC_DMACHCRT      0x02C
#define RCAR_GEN4_DMAC_DMATSRB       0x038
#define RCAR_GEN4_DMAC_DMARS         0x040
#define RCAR_GEN4_DMAC_DMABUFCR      0x048
#define RCAR_GEN4_DMAC_DMADPBASE     0x050
#define RCAR_GEN4_DMAC_DMADPCR       0x054
#define RCAR_GEN4_DMAC_DMAFIXDPBASE  0x060
#define RCAR_GEN4_DMAC_DMAREGIONID   0x078
#define RCAR_GEN4_DMAC_DMACHID       0x080
#define RCAR_GEN4_DMAC_DMADMSEC      0x0B0
#define RCAR_GEN4_DMAC_CHCLR         0x100

/* SYSDMAC global registers, offset from BASE */
#define RCAR_GEN4_DMAC_DMAISTA       0x0020      // DMA interrupt status register
#define RCAR_GEN4_DMAC_DMASEC        0x0030      // DMA secure control register
#define RCAR_GEN4_DMAC_DMAOR         0x0060      // DMA operation register
#define RCAR_GEN4_DMAC_DMACHCLR      0x0080      // DMA channel clear register
#define RCAR_GEN4_DMAC_DMADPSEC      0x00A0      // DPRAM secure control register
#define RCAR_GEN4_DMAC_ADRFB         0x00CC
#define RCAR_GEN4_DMAC_APBEDC        0x00D0
#define RCAR_GEN4_DMAC_APB_CH_PADDR          0x00D4
#define RCAR_GEN4_DMAC_APB_CH_PWDATA         0x00D8
#define RCAR_GEN4_DMAC_APB_PUBLIC_PADDR      0x00DC
#define RCAR_GEN4_DMAC_APB_PUBLIC_PWDATA     0x00E0
#define RCAR_GEN4_DMAC_DMCMP_STAUS   0x00F0
#define RCAR_GEN4_DMAC_RATE_RD       0x00F4
#define RCAR_GEN4_DMAC_RATE_WR       0x00F8
#define RCAR_GEN4_DMAC_DPRAM_EDC     0x00FC
#define RCAR_GEN4_DMAC_DPSAR         0x0800
#define RCAR_GEN4_DMAC_DPDAR         0x0804
#define RCAR_GEN4_DMAC_DPTCR         0x0808
#define RCAR_GEN4_DMAC_DPCHCR        0x080C

/*
 * DMA request ID
 */
#define RCAR_GEN4_DREQ_HSCIF0_RXI        0x30
#define RCAR_GEN4_DREQ_HSCIF0_TXI        0x31
#define RCAR_GEN4_DREQ_HSCIF1_RXI        0x32
#define RCAR_GEN4_DREQ_HSCIF1_TXI        0x33
#define RCAR_GEN4_DREQ_HSCIF2_RXI        0x34
#define RCAR_GEN4_DREQ_HSCIF2_TXI        0x35
#define RCAR_GEN4_DREQ_HSCIF3_RXI        0x36
#define RCAR_GEN4_DREQ_HSCIF3_TXI        0x37
#define RCAR_GEN4_DREQ_MSIOF0_RXI        0x40
#define RCAR_GEN4_DREQ_MSIOF0_TXI        0x41
#define RCAR_GEN4_DREQ_MSIOF1_RXI        0x42
#define RCAR_GEN4_DREQ_MSIOF1_TXI        0x43
#define RCAR_GEN4_DREQ_MSIOF2_RXI        0x44
#define RCAR_GEN4_DREQ_MSIOF2_TXI        0x45
#define RCAR_GEN4_DREQ_MSIOF3_RXI        0x46
#define RCAR_GEN4_DREQ_MSIOF3_TXI        0x47
#define RCAR_GEN4_DREQ_MSIOF4_RXI        0x48
#define RCAR_GEN4_DREQ_MSIOF4_TXI        0x49
#define RCAR_GEN4_DREQ_MSIOF5_RXI        0x4A
#define RCAR_GEN4_DREQ_MSIOF5_TXI        0x4B
#define RCAR_GEN4_DREQ_SCIF0_RXI         0x50
#define RCAR_GEN4_DREQ_SCIF0_TXI         0x51
#define RCAR_GEN4_DREQ_SCIF1_RXI         0x52
#define RCAR_GEN4_DREQ_SCIF1_TXI         0x53
#define RCAR_GEN4_DREQ_SCIF3_RXI         0x56
#define RCAR_GEN4_DREQ_SCIF3_TXI         0x57
#define RCAR_GEN4_DREQ_SCIF4_RXI         0x58
#define RCAR_GEN4_DREQ_SCIF4_TXI         0x59
#define RCAR_GEN4_DREQ_I2C0_RXI          0x90
#define RCAR_GEN4_DREQ_I2C0_TXI          0x91
#define RCAR_GEN4_DREQ_I2C1_RXI          0x92
#define RCAR_GEN4_DREQ_I2C1_TXI          0x93
#define RCAR_GEN4_DREQ_I2C2_RXI          0x94
#define RCAR_GEN4_DREQ_I2C2_TXI          0x95
#define RCAR_GEN4_DREQ_I2C3_RXI          0x96
#define RCAR_GEN4_DREQ_I2C3_TXI          0x97
#define RCAR_GEN4_DREQ_I2C4_RXI          0x98
#define RCAR_GEN4_DREQ_I2C4_TXI          0x99
#define RCAR_GEN4_DREQ_I2C5_RXI          0x9A
#define RCAR_GEN4_DREQ_I2C5_TXI          0x9B
#define RCAR_GEN4_DREQ_I2C6_RXI          0x9C
#define RCAR_GEN4_DREQ_I2C6_TXI          0x9D
#define RCAR_GEN4_DREQ_IIC_DVFS_TXI      0x77
#define RCAR_GEN4_DREQ_IIC_DVFS_RXI      0x78
#define RCAR_GEN4_DREQ_SDHI0_TXI         0xCD
#define RCAR_GEN4_DREQ_SDHI0_RXI         0xCE
#define RCAR_GEN4_DREQ_SDHI1_TXI         0xC9
#define RCAR_GEN4_DREQ_SDHI1_RXI         0xCA
#define RCAR_GEN4_DREQ_SDHI2_TXI         0xC1
#define RCAR_GEN4_DREQ_SDHI2_RXI         0xC2
#define RCAR_GEN4_DREQ_SDHI2_C2_TXI      0xC5
#define RCAR_GEN4_DREQ_SDHI2_C2_RXI      0xC6
#define RCAR_GEN4_DREQ_SDHI3_TXI         0xD3
#define RCAR_GEN4_DREQ_SDHI3_RXI         0xD4
#define RCAR_GEN4_DREQ_SDHI3_C2_TXI      0xDF
#define RCAR_GEN4_DREQ_SDHI3_C2_RXI      0xDE
#define RCAR_GEN4_DREQ_TPU0_TXI          0xF1
#define RCAR_GEN4_DREQ_TSIF0_RXI         0xEA
#define RCAR_GEN4_DREQ_TSIF1_RXI         0xF0
#define RCAR_GEN4_DREQ_AXISTATR_RXI      0xA6
#define RCAR_GEN4_DREQ_AXISTATR0_RXI     0xAC
#define RCAR_GEN4_DREQ_AXISTATR1_RXI     0xAA
#define RCAR_GEN4_DREQ_AXISTAT2_RXI      0xA8
#define RCAR_GEN4_DREQ_AXISTAT3C_RXI     0xA4
#define RCAR_GEN4_DREQ_MMCIF0_TXI        0xD1
#define RCAR_GEN4_DREQ_MMCIF0_RXI        0xD2
#define RCAR_GEN4_DREQ_MMCIF1_TXI        0xE1
#define RCAR_GEN4_DREQ_MMCIF1_RXI        0xE2
#define RCAR_GEN4_DREQ_AXSTM_RXI         0xAE

#define RCAR_DREQ_SCIF_TXI(n)         ((n) == 0 ? RCAR_DREQ_SCIF0_TXI: \
                                      ((n) == 1 ? RCAR_DREQ_SCIF1_TXI: \
                                      ((n) == 2 ? RCAR_DREQ_SCIF2_TXI: \
                                      ((n) == 3 ? RCAR_DREQ_SCIF3_TXI: RCAR_DREQ_SCIF4_TXI))))
#define RCAR_DREQ_SCIF_RXI(n)         ((n) == 0 ? RCAR_DREQ_SCIF0_RXI: \
                                      ((n) == 1 ? RCAR_DREQ_SCIF1_RXI: \
                                      ((n) == 2 ? RCAR_DREQ_SCIF2_RXI: \
                                      ((n) == 3 ? RCAR_DREQ_SCIF3_RXI: RCAR_DREQ_SCIF4_RXI))))
#define RCAR_DREQ_HSCIF_TXI(n)        ((n) == 0 ? RCAR_DREQ_HSCIF0_TXI: \
                                      ((n) == 1 ? RCAR_DREQ_HSCIF1_TXI: \
                                      ((n) == 2 ? RCAR_DREQ_HSCIF2_TXI: RCAR_DREQ_HSCIF3_TXI)))
#define RCAR_DREQ_HSCIF_RXI(n)        ((n) == 0 ? RCAR_DREQ_HSCIF0_RXI: \
                                      ((n) == 1 ? RCAR_DREQ_HSCIF1_RXI: \
                                      ((n) == 2 ? RCAR_DREQ_HSCIF2_RXI: RCAR_DREQ_HSCIF3_RXI)))
#define RCAR_GEN4_DREQ_I2C_TXI(n)     ((n) == 0 ? RCAR_GEN4_DREQ_I2C0_TXI: \
                                      ((n) == 1 ? RCAR_GEN4_DREQ_I2C1_TXI: \
                                      ((n) == 2 ? RCAR_GEN4_DREQ_I2C2_TXI: \
                                      ((n) == 3 ? RCAR_GEN4_DREQ_I2C3_TXI: RCAR_GEN4_DREQ_I2C4_TXI))))
#define RCAR_GEN4_DREQ_I2C_RXI(n)     ((n) == 0 ? RCAR_GEN4_DREQ_I2C0_RXI: \
                                      ((n) == 1 ? RCAR_GEN4_DREQ_I2C1_RXI: \
                                      ((n) == 2 ? RCAR_GEN4_DREQ_I2C2_RXI: \
                                      ((n) == 3 ? RCAR_GEN4_DREQ_I2C3_RXI: RCAR_GEN4_DREQ_I2C4_RXI))))
#define RCAR_GEN4_DREQ_MSIOF_TXI(n)   ((n) == 0 ? RCAR_GEN4_DREQ_MSIOF0_TXI: \
                                      ((n) == 1 ? RCAR_GEN4_DREQ_MSIOF1_TXI: \
                                      ((n) == 2 ? RCAR_GEN4_DREQ_MSIOF2_TXI: RCAR_GEN4_DREQ_MSIOF3_TXI)))
#define RCAR_GEN4_DREQ_MSIOF_RXI(n)   ((n) == 0 ? RCAR_GEN4_DREQ_MSIOF0_RXI: \
                                      ((n) == 1 ? RCAR_GEN4_DREQ_MSIOF1_RXI: \
                                      ((n) == 2 ? RCAR_GEN4_DREQ_MSIOF2_RXI: RCAR_GEN4_DREQ_MSIOF3_RXI)))

/*
 * Flexray
 */
#define RCAR_GEN4_FLEXRAY_BASE           0xE6B00000
#define RCAR_GEN4_FLEXRAY_SIZE           0x1000

/*
 * I2C
 */
#define RCAR_GEN4_I2C0_BASE              0xE6500000
#define RCAR_GEN4_I2C1_BASE              0xE6508000
#define RCAR_GEN4_I2C2_BASE              0xE6510000
#define RCAR_GEN4_I2C3_BASE              0xE66D0000
#define RCAR_GEN4_I2C4_BASE              0xE66D8000
#define RCAR_GEN4_I2C5_BASE              0xE66E0000
#define RCAR_GEN4_I2C_SIZE               0x100

/* I2C registers, offset from I2C base */
#define RCAR_GEN4_I2C_ICSCR             0x00        // Slave Control Register
 #define RCAR_GEN4_ICSCR_SDBS            (1 << 3)
 #define RCAR_GEN4_ICSCR_SIE             (1 << 2)
 #define RCAR_GEN4_ICSCR_GCAE            (1 << 1)
 #define RCAR_GEN4_ICSCR_FNA             (1 << 0)

#define RCAR_GEN4_I2C_ICMCR             0x04        // Master Control Register
 #define RCAR_GEN4_ICMCR_MDBS            (1 << 7)
 #define RCAR_GEN4_ICMCR_FSCL            (1 << 6)
 #define RCAR_GEN4_ICMCR_FSDA            (1 << 5)
 #define RCAR_GEN4_ICMCR_OBPC            (1 << 4)
 #define RCAR_GEN4_ICMCR_MIE             (1 << 3)
 #define RCAR_GEN4_ICMCR_TSBE            (1 << 2)
 #define RCAR_GEN4_ICMCR_FSB             (1 << 1)
 #define RCAR_GEN4_ICMCR_ESG             (1 << 0)

#define RCAR_GEN4_I2C_ICSSR             0x08        // Slave Status Register
 #define RCAR_GEN4_ICSSR_GCAR            (1 << 6)
 #define RCAR_GEN4_ICSSR_STM             (1 << 5)
 #define RCAR_GEN4_ICSSR_SSR             (1 << 4)
 #define RCAR_GEN4_ICSSR_SDE             (1 << 3)
 #define RCAR_GEN4_ICSSR_SDT             (1 << 2)
 #define RCAR_GEN4_ICSSR_SDR             (1 << 1)
 #define RCAR_GEN4_ICSSR_SAR             (1 << 0)

#define RCAR_GEN4_I2C_ICMSR             0x0C        // Master Status Register
 #define RCAR_GEN4_ICMSR_MNR             (1 << 6)
 #define RCAR_GEN4_ICMSR_MAL             (1 << 5)
 #define RCAR_GEN4_ICMSR_MST             (1 << 4)
 #define RCAR_GEN4_ICMSR_MDE             (1 << 3)
 #define RCAR_GEN4_ICMSR_MDT             (1 << 2)
 #define RCAR_GEN4_ICMSR_MDR             (1 << 1)
 #define RCAR_GEN4_ICMSR_MAT             (1 << 0)

#define RCAR_GEN4_I2C_ICSIER            0x10        // Slave Interrupt Enable Register
 #define RCAR_GEN4_ICSIER_SSRE           (1 << 4)
 #define RCAR_GEN4_ICSIER_SDEE           (1 << 3)
 #define RCAR_GEN4_ICSIER_SDTE           (1 << 2)
 #define RCAR_GEN4_ICSIER_SDRE           (1 << 1)
 #define RCAR_GEN4_ICSIER_SARE           (1 << 0)

#define RCAR_GEN4_I2C_ICMIER            0x14        // Master Interrupt Enable Register
 #define RCAR_GEN4_ICMIER_MNRE           (1 << 6)
 #define RCAR_GEN4_ICMIER_MALE           (1 << 5)
 #define RCAR_GEN4_ICMIER_MSTE           (1 << 4)
 #define RCAR_GEN4_ICMIER_MDEE           (1 << 3)
 #define RCAR_GEN4_ICMIER_MDTE           (1 << 2)
 #define RCAR_GEN4_ICMIER_MDRE           (1 << 1)
 #define RCAR_GEN4_ICMIER_MATE           (1 << 0)

#define RCAR_GEN4_I2C_ICCCR             0x18        // Clock Control Register
#define RCAR_GEN4_I2C_ICSAR             0x1C        // Slave Address Register
#define RCAR_GEN4_I2C_ICMAR             0x20        // Master Address Register
 #define RCAR_GEN3_ICMAR_STM1            (1 << 0)

#define RCAR_GEN4_I2C_ICRXD             0x24        // Receive Data Register
#define RCAR_GEN4_I2C_ICTXD             0x24        // Transmit Data Register
#define RCAR_GEN4_I2C_ICCCR2            0x28
#define RCAR_GEN4_I2C_ICMPR             0x2C
#define RCAR_GEN4_I2C_ICHPR             0x30
#define RCAR_GEN4_I2C_ICLPR             0x34
#define RCAR_GEN4_I2C_ICDMAER           0x3C        //DMA enable register
 #define RCAR_GEN4_ICDMAER_RSDMAE        (1 << 3)
 #define RCAR_GEN4_ICDMAER_TSDMAE        (1 << 2)
 #define RCAR_GEN4_ICDMAER_RMDMAE        (1 << 1)
 #define RCAR_GEN4_ICDMAER_TMDMAE        (1 << 0)

#define RCAR_GEN4_I2C_ICFBSCR           0x38
 #define RCAR_ICFBSCR_TCYC17        0x07

/*
 * I2C Bus Interface for DVFS IIC
 */
#define RCAR_GEN4_IIC_BASE               0xE60B0000
#define RCAR_GEN4_IIC_SIZE               0x100

/* IIC registers, offset from base */
#define RCAR_GEN4_IIC_ICDR              0x0000      // Data Register
#define RCAR_GEN4_IIC_ICCR              0x0004      // Control Register
 #define RCAR_GEN4_ICCR_ICE              (1 << 7)    // IIC Interface Enable
 #define RCAR_GEN4_ICCR_RACK             (1 << 6)    // Receive Acknowledge
 #define RCAR_GEN4_ICCR_MTM              (1 << 4)    // Master Transmit Mode
 #define RCAR_GEN4_ICCR_MRM              (0 << 4)    // Master Receive Mode
 #define RCAR_GEN4_ICCR_BBSY             (1 << 2)    // Bus Busy
 #define RCAR_GEN4_ICCR_SCP              (1 << 0)    // START/STOP Condition Prohibit

#define RCAR_GEN4_IIC_ICSR              0x0008      // Status Register
 #define RCAR_GEN4_ICSR_DTE              (1 << 0)    // Data Transmit Enable
 #define RCAR_GEN4_ICSR_WAIT             (1 << 1)    // Wait
 #define RCAR_GEN4_ICSR_TACK             (1 << 2)    // Transmit Acknowledge
 #define RCAR_GEN4_ICSR_AL               (1 << 3)    // Arbitration Lost
 #define RCAR_GEN4_ICSR_BUSY             (1 << 4)    // Transmit Busy

#define RCAR_GEN4_IIC_ICIC              0x000C      // Interrupt Control Register
 #define RCAR_GEN4_ICIC_DTE              (1 << 0)    // Data Transmit Enable
 #define RCAR_GEN4_ICIC_WAIT             (1 << 1)    // Wait
 #define RCAR_GEN4_ICIC_TACK             (1 << 2)    // Transmit Acknowledge
 #define RCAR_GEN4_ICIC_AL               (1 << 3)    // Arbitration Lost

#define RCAR_GEN4_IIC_ICCL              0x0010      // Clock Control Register Low
#define RCAR_GEN4_IIC_ICCH              0x0014      // Clock Control Register High

/*
 * MSIOF
 */
#define RCAR_GEN4_MSIOF0_BASE            0xE6E90000
#define RCAR_GEN4_MSIOF1_BASE            0xE6EA0000
#define RCAR_GEN4_MSIOF2_BASE            0xE6C00000
#define RCAR_GEN4_MSIOF3_BASE            0xE6C10000
#define RCAR_GEN4_MSIOF4_BASE            0xE6C20000
#define RCAR_GEN4_MSIOF5_BASE            0xE6C28000

#define RCAR_MSIOF_SIZE             0x100

/* MSIOF registers, offset from base */
#define RCAR_GEN4_MSIOF_TMDR1           0x00
#define RCAR_GEN4_MSIOF_TMDR2           0x04
#define RCAR_GEN4_MSIOF_TMDR3           0x08
#define RCAR_GEN4_MSIOF_RMDR1           0x10
#define RCAR_GEN4_MSIOF_RMDR2           0x14
#define RCAR_GEN4_MSIOF_RMDR3           0x18
#define RCAR_GEN4_MSIOF_TSCR            0x20
#define RCAR_GEN4_MSIOF_RSCR            0x22
#define RCAR_GEN4_MSIOF_CTR             0x28
#define RCAR_GEN4_MSIOF_FCTR            0x30
#define RCAR_GEN4_MSIOF_STR             0x40
#define RCAR_GEN4_MSIOF_IER             0x44
#define RCAR_GEN4_MSIOF_TFDR            0x50
#define RCAR_GEN4_MSIOF_RFDR            0x60

/*
 * SCIF
 */
#define RCAR_GEN4_SCIF0_BASE             0xE6E60000
#define RCAR_GEN4_SCIF1_BASE             0xE6E68000
#define RCAR_GEN4_SCIF2_BASE             0xE6E88000
#define RCAR_GEN4_SCIF3_BASE             0xE6C50000
#define RCAR_GEN4_SCIF4_BASE             0xE6C40000
#define RCAR_GEN4_SCIF5_BASE             0xE6F30000

#define RCAR_GEN4_SCIF_SIZE              0x100

#define RCAR_GEN4_SCIF_FIFO_SIZE         16

/* Offset from SCIF_BASE */
#define RCAR_GEN4_SCIF_SCSMR            0x00
 #define RCAR_GEN4_SCIF_SCSMR_CHR        (1 << 6)
 #define RCAR_GEN4_SCIF_SCSMR_PE         (1 << 5)
 #define RCAR_GEN4_SCIF_SCSMR_OE         (1 << 4)
 #define RCAR_GEN4_SCIF_SCSMR_STOP       (1 << 3)
 #define RCAR_GEN4_SCIF_SCSMR_CKS_0      (0 << 0)
 #define RCAR_GEN4_SCIF_SCSMR_CKS_4      (1 << 0)
 #define RCAR_GEN4_SCIF_SCSMR_CKS_16     (2 << 0)
 #define RCAR_GEN4_SCIF_SCSMR_CKS_64     (3 << 0)

#define RCAR_GEN4_SCIF_SCBRR            0x04
#define RCAR_GEN4_SCIF_SCSCR            0x08
  #define RCAR_GEN4_SCIF_SCSCR_TEIE      (1 << 11)
  #define RCAR_GEN4_SCIF_SCSCR_TIE       (1 << 7)
  #define RCAR_GEN4_SCIF_SCSCR_RIE       (1 << 6)
  #define RCAR_GEN4_SCIF_SCSCR_TE        (1 << 5)
  #define RCAR_GEN4_SCIF_SCSCR_RE        (1 << 4)
  #define RCAR_GEN4_SCIF_SCSCR_REIE      (1 << 3)
  #define RCAR_GEN4_SCIF_SCSCR_CKE_MASK  (3 << 0)
  #define RCAR_GEN4_SCIF_SCSCR_CKE_IN    (2 << 0)
  #define RCAR_GEN4_SCIF_SCSCR_TOIE      (1 << 2)

#define RCAR_GEN4_SCIF_SCFTDR           0x0C
#define RCAR_GEN4_SCIF_SCFSR            0x10
  #define RCAR_GEN4_SCIF_SCFSR_PERF(x)   (((x) >> 12) & 0xF)
  #define RCAR_GEN4_SCIF_SCFSR_FERF(x)   (((x) >> 8) & 0xF)
  #define RCAR_GEN4_SCIF_SCFSR_ER        (1 << 7)
  #define RCAR_GEN4_SCIF_SCFSR_TEND      (1 << 6)
  #define RCAR_GEN4_SCIF_SCFSR_TDFE      (1 << 5)
  #define RCAR_GEN4_SCIF_SCFSR_BRK       (1 << 4)
  #define RCAR_GEN4_SCIF_SCFSR_FER       (1 << 3)
  #define RCAR_GEN4_SCIF_SCFSR_PER       (1 << 2)
  #define RCAR_GEN4_SCIF_SCFSR_RDF       (1 << 1)
  #define RCAR_GEN4_SCIF_SCFSR_DR        (1 << 0)

#define RCAR_GEN4_SCIF_SCFRDR           0x14
#define RCAR_GEN4_SCIF_SCFCR            0x18
  #define RCAR_GEN4_SCIF_SCFCR_RTRG_1    (0 << 6)
  #define RCAR_GEN4_SCIF_SCFCR_RTRG_4    (1 << 6)
  #define RCAR_GEN4_SCIF_SCFCR_RTRG_8    (2 << 6)
  #define RCAR_GEN4_SCIF_SCFCR_RTRG_14   (3 << 6)
  #define RCAR_GEN4_SCIF_SCFCR_TTRG_0    (3 << 4)
  #define RCAR_GEN4_SCIF_SCFCR_TTRG_2    (2 << 4)
  #define RCAR_GEN4_SCIF_SCFCR_TTRG_4    (1 << 4)
  #define RCAR_GEN4_SCIF_SCFCR_TTRG_8    (0 << 4)
  #define RCAR_GEN4_SCIF_SCFCR_MCE       (1 << 3)
  #define RCAR_GEN4_SCIF_SCFCR_TFRST     (1 << 2)
  #define RCAR_GEN4_SCIF_SCFCR_RFRST     (1 << 1)
  #define RCAR_GEN4_SCIF_SCFCR_LOOP      (1 << 0)
#define RCAR_GEN4_SCIF_SCFDR            0x1C
  #define RCAR_GEN4_SCIF_SCFDR_TX(x)     (((x) >> 8) & 0x1F)
  #define RCAR_GEN4_SCIF_SCFDR_RX(x)     ((x) & 0x1F)

#define RCAR_GEN4_SCIF_SCSPTR           0x20
  #define RCAR_GEN4_SCIF_SCSPTR_RTSIO    (1 << 7)
  #define RCAR_GEN4_SCIF_SCSPTR_RTSDT    (1 << 6)
  #define RCAR_GEN4_SCIF_SCSPTR_CTSIO    (1 << 5)
  #define RCAR_GEN4_SCIF_SCSPTR_CTSDT    (1 << 4)
  #define RCAR_GEN4_SCIF_SCSPTR_SPB2IO   (1 << 1)
  #define RCAR_GEN4_SCIF_SCSPTR_SPB2DT   (1 << 0)

#define RCAR_GEN4_SCIF_SCLSR            0x24
  #define RCAR_GEN4_SCIF_SCLSR_ORER      (1 << 0)

#define RCAR_GEN4_SCIF_BRG_DL           0x30
#define RCAR_GEN4_SCIF_BRG_CKS          0x34

/*
 * HSCIF
 */
#define RCAR_GEN4_HSCIF0_BASE            0xE6540000
#define RCAR_GEN4_HSCIF1_BASE            0xE6550000
#define RCAR_GEN4_HSCIF2_BASE            0xE6560000
#define RCAR_GEN4_HSCIF3_BASE            0xE66A0000
#define RCAR_GEN4_HSCIF4_BASE            0xE66B0000
#define RCAR_GEN4_HSCIF_SIZE             0x100

/* Offset from HSCIFn_BASE */
#define RCAR_GEN4_HSCIF_HSSRR            0x40
  #define RCAR_GEN4_HSCIF_HSSRR_SRE      (1 << 15)
  #define RCAR_GEN4_HSCIF_HSSRR_SRCYC    (0x1F << 0)

/*
* RPC (SPI)
*/
#define RCAR_GEN4_RPC_BASE              0xEE200000
#define RCAR_GEN4_RPC_SIZE              0x200

#define RCAR_GEN4_RPC_BUFFER_BASE       0xEE208000
#define RCAR_GEN4_RPC_BUFFER_SIZE       0x100
#define RCAR_GEN4_RPC_BUFFER            0x8000

/* Hyperflash */
#define RCAR_GEN4_RPC_DEV_ADDR           0x08000000
#define RCAR_GEN4_RPC_DEV_SIZE           0x04000000

/*
 * HSUSB
 */
#define RCAR_GEN4_HSUSB_BASE             0xE6590000
#define RCAR_GEN4_HSUSB3_BASE            0xE659C000    /* R-CarH3 only */
#define RCAR_GEN4_HSUSB_SYSCFG           0x0000
#define RCAR_GEN4_HSUSB_INTENB0          0x0030
#define RCAR_GEN4_HSUSB_LPSTS            0x0102
#define RCAR_GEN4_HSUSB_UGCTRL           0x0180
#define RCAR_GEN4_HSUSB_UGCTRL2          0x0184
#define RCAR_GEN4_HSUSB_LPSTS_SUSPM      0x4000
#define RCAR_GEN4_HSUSB_USB0SEL          0x00000030
#define RCAR_GEN4_HSUSB_USB0SEL_EHCI     0x00000010
#define RCAR_GEN4_HSUSB_SUSPM           (1 << 14)
#define RCAR_GEN4_HSUSB_SUSPM_NORMAL    (1 << 14)

/* Register offset */
/* AHB bridge registers */
#define RCAR_GEN4_USB_AHB_INT_ENABLE    0x200
#define RCAR_GEN4_USB_AHB_USBCTR        0x20C
 #define RCAR_GEN4_USB_AHB_PLL_RST       (1 << 1)
 #define RCAR_GEN4_USB_AHB_USBH_INTBEN   (1 << 2)
 #define RCAR_GEN4_USB_AHB_USBH_INTAEN   (1 << 1)

/* UCOM registers */
#define RCAR_GEN4_USB_UCOM_COMMCTRL		 0x800
 #define RCAR_GEN4_USB_UCOM_OTG_PERI      (1 << 31)
 #define RCAR_GEN4_USB_UCOM_OTG_PERI_HOST (0 << 31)
 #define RCAR_GEN4_USB_UCOM_OTG_PERI_PERI (1 << 31)

 /* Core defined registers */
#define RCAR_GEN4_USB_CORE_SPD_RSM_TIMSET     0x30C
#define RCAR_GEN4_USB_CORE_OC_TIMSET          0x310

/*
 * AVB
 */
#define RCAR_GEN4_AVB0_BASE              0xE6800000
#define RCAR_GEN4_AVB1_BASE              0xE6810000
#define RCAR_GEN4_AVB2_BASE              0xE6820000
#define RCAR_GEN4_AVB3_BASE              0xE6830000
#define RCAR_GEN4_AVB4_BASE              0xE6840000
#define RCAR_GEN4_AVB5_BASE              0xE6850000
#define RCAR_GEN4_AVB_SIZE               0x0800
/*
 * USB 2.0
 */
#define RCAR_GEN4_USB0_BASE              0xEE080000     // USB2.0 ch0
#define RCAR_GEN4_USB1_BASE              0xEE0A0000     // USB2.0 ch1
#define RCAR_GEN4_USB2_BASE              0xEE0C0000     // USB2.0 ch2
#define RCAR_GEN4_USB3_BASE              0xEE0E0000     // USB2.0 ch3

#define RCAR_PCI_CONF_EHCI          0x10100        // Offset to PCI configuration Register

/* Core defined registers */
#define RCAR_GEN4_USB_CORE_SPD_RSM_TIMSET     0x30C
#define RCAR_GEN4_USB_CORE_OC_TIMSET          0x310

/*
 * USB 3.0
 */
#define RCAR_GEN4_USB30_BASE             0xEE000000  /* USB3.0 ch0 */
#define RCAR_GEN4_USB31_BASE             0xEE040000  /* USB3.0 ch1 */
#define RCAR_GEN4_USB3_SIZE              0xBFF

/* Register Offset */
#define RCAR_GEN4_USB3_CLASSCODE        0x204
#define RCAR_GEN4_USB3_RELEASE_NUMBER   0x210
#define RCAR_GEN4_USB3_INT_ENA          0x224      /* Interrupt Enable */
 #define RCAR_GEN4_USB3_INT_XHC_ENA       0x00000001
 #define RCAR_GEN4_USB3_INT_PME_ENA       0x00000002
 #define RCAR_GEN4_USB3_INT_HSE_ENA       0x00000004
 #define RCAR_GEN4_USB3_INT_LTM_ENA       0x00000008
 #define RCAR_GEN4_USB3_INT_SMI_ENA       0x00000010
 #define RCAR_GEN4_USB3_INT_ENA_VAL       (RCAR_GEN4_USB3_INT_XHC_ENA | RCAR_GEN4_USB3_INT_PME_ENA | RCAR_GEN4_USB3_INT_HSE_ENA)

#define RCAR_GEN4_USB3_DL_CTRL          0x250      /* FW Download Control & Status */
 #define RCAR_GEN4_USB3_DL_CTRL_ENABLE        0x00000001
 #define RCAR_GEN4_USB3_DL_CTRL_FW_SUCCESS    0x00000010
 #define RCAR_GEN4_USB3_DL_CTRL_FW_SET_DATA0  0x00000100

#define RCAR_GEN4_USB3_FW_DATA0         0x258      /* FW Data0 */
#define RCAR_GEN4_USB3_LCLK             0xa44      /* LCLK Select */
 #define RCAR_GEN4_USB3_PTPWER_CTRL      (1 << 24)
 #define RCAR_GEN4_USB3_RENESAS_PRIV     (0x3 << 16)
 #define RCAR_GEN4_USB3_BC_MODE          (1 << 0)
 #define RCAR_GEN4_USB3_LCLK_ENA_VAL     (RCAR_GEN4_USB3_PTPWER_CTRL | RCAR_GEN4_USB3_RENESAS_PRIV | RCAR_GEN4_USB3_BC_MODE)

#define RCAR_GEN4_USB3_CONF1            0xa48      /* USB3.0 Configuration1 */
#define RCAR_GEN4_USB3_CONF2            0xa5c      /* USB3.0 Configuration2 */
#define RCAR_GEN4_USB3_CONF3            0xaa8      /* USB3.0 Configuration3 */
 #define RCAR_GEN4_USB3_CONF1_VAL        0x00030204
 #define RCAR_GEN4_USB3_CONF2_VAL        0x00030300
 #define RCAR_GEN4_USB3_CONF3_VAL        0x13802007

#define RCAR_GEN4_USB3_RX_POL           0xab0      /* USB3.0 RX Polarity */
#define RCAR_GEN4_USB3_TX_POL           0xab8      /* USB3.0 TX Polarity */
 #define RCAR_GEN4_USB3_RX_POL_VAL      (1 << 21)
 #define RCAR_GEN4_USB3_TX_POL_VAL      (1 << 4)

/*
 * DBSC4 Controller
 */
 //#define RCAR_SDRAM_0               0x48000000
 #define RCAR_GEN4_SDRAM_0               0x40000000
 #define RCAR_GEN4_SDRAM_0_0             0x400000000 // First 2GB of this area is shadow area of RCAR_GEN3_SDRAM_0
 #define RCAR_GEN4_SDRAM_0_1             0x480000000
 #define RCAR_GEN4_SDRAM_1               0x500000000
 #define RCAR_GEN4_SDRAM_2               0x600000000
 #define RCAR_GEN4_SDRAM_3               0x700000000
 #define RCAR_MAX_RANK              4

#define RCAR_GEN4_DBSC4_BASE             0xE6790000
/* DBSC4 registers, offset from DBSC4_BASE */
#define RCAR_GEN4_DBSC4_DBSYSCONF1      0x0004U
#define RCAR_GEN4_DBSC4_DBPHYCONF0      0x0010U
#define RCAR_GEN4_DBSC4_DBKIND          0x0020U
#define RCAR_GEN4_DBSC4_DBMEMCONF_0_0   0x0030U
#define RCAR_GEN4_DBSC4_DBMEMCONF_0_1   0x0034U
#define RCAR_GEN4_DBSC4_DBMEMCONF_1_0   0x0040U
#define RCAR_GEN4_DBSC4_DBMEMCONF_1_1   0x0044U
#define RCAR_GEN4_DBSC4_DBMEMCONF_2_0   0x0050U
#define RCAR_GEN4_DBSC4_DBMEMCONF_2_1   0x0054U
#define RCAR_GEN4_DBSC4_DBMEMCONF_3_0   0x0060U
#define RCAR_GEN4_DBSC4_DBMEMCONF_3_1   0x0064U
#define RCAR_GEN4_DBSC4_DBMEMCONF_0_2   0x0038
#define RCAR_GEN4_DBSC4_DBMEMCONF_0_3   0x003C
#define RCAR_GEN4_DBSC4_DBMEMCONF_1_2   0x0048
#define RCAR_GEN4_DBSC4_DBMEMCONF_1_3   0x004C
#define RCAR_GEN4_DBSC4_DBMEMCONF_2_2   0x0058
#define RCAR_GEN4_DBSC4_DBMEMCONF_2_3   0x005C
#define RCAR_GEN4_DBSC4_DBMEMCONF_3_2   0x0068
#define RCAR_GEN4_DBSC4_DBMEMCONF_3_3   0x006C
#define RCAR_GEN4_DBSC4_DBSYSCNT0       0x0100U
#define RCAR_GEN4_DBSC4_DBSVCR1         0x0104U
#define RCAR_GEN4_DBSC4_DBSTATE0        0x0108U
#define RCAR_GEN4_DBSC4_DBSTATE1        0x010CU
#define RCAR_GEN4_DBSC4_DBACEN          0x0200U
#define RCAR_GEN4_DBSC4_DBRFEN          0x0204U
#define RCAR_GEN4_DBSC4_DBCMD           0x0208U
#define RCAR_GEN4_DBSC4_DBWAIT          0x0210U //wait DBCMD 1=busy, 0=ready
#define RCAR_GEN4_DBSC4_DBSYSCTRL0      0x0280U
#define RCAR_GEN4_DBSC4_DBTR0           0x0300U
#define RCAR_GEN4_DBSC4_DBTR1           0x0304U
#define RCAR_GEN4_DBSC4_DBTR2           0x0308U
#define RCAR_GEN4_DBSC4_DBTR3           0x030CU
#define RCAR_GEN4_DBSC4_DBTR4           0x0310U
#define RCAR_GEN4_DBSC4_DBTR5           0x0314U
#define RCAR_GEN4_DBSC4_DBTR6           0x0318U
#define RCAR_GEN4_DBSC4_DBTR7           0x031CU
#define RCAR_GEN4_DBSC4_DBTR8           0x0320U
#define RCAR_GEN4_DBSC4_DBTR9           0x0324U
#define RCAR_GEN4_DBSC4_DBTR10          0x0328U
#define RCAR_GEN4_DBSC4_DBTR11          0x032CU
#define RCAR_GEN4_DBSC4_DBTR12          0x0330U
#define RCAR_GEN4_DBSC4_DBTR13          0x0334U
#define RCAR_GEN4_DBSC4_DBTR14          0x0338U
#define RCAR_GEN4_DBSC4_DBTR15          0x033CU
#define RCAR_GEN4_DBSC4_DBTR16          0x0340U
#define RCAR_GEN4_DBSC4_DBTR17          0x0344U
#define RCAR_GEN4_DBSC4_DBTR18          0x0348U
#define RCAR_GEN4_DBSC4_DBTR19          0x034CU
#define RCAR_GEN4_DBSC4_DBTR20          0x0350U
#define RCAR_GEN4_DBSC4_DBTR21          0x0354U
#define RCAR_GEN4_DBSC4_DBTR22          0x0358U
#define RCAR_GEN4_DBSC4_DBTR23          0x035CU
#define RCAR_GEN4_DBSC4_DBTR24          0x0360U
#define RCAR_GEN4_DBSC4_DBTR25          0x0364U
#define RCAR_GEN4_DBSC4_DBBL            0x0400U
#define RCAR_GEN4_DBSC4_DBRFCNF1        0x0414U
#define RCAR_GEN4_DBSC4_DBRFCNF2        0x0418U
#define RCAR_GEN4_DBSC4_DBCALCNF        0x0424U
#define RCAR_GEN4_DBSC4_DBRNK2          0x0438U
#define RCAR_GEN4_DBSC4_DBRNK3          0x043CU
#define RCAR_GEN4_DBSC4_DBRNK4          0x0440U
#define RCAR_GEN4_DBSC4_DBRNK5          0x0444U
#define RCAR_GEN4_DBSC4_DBPDNCNF        0x0450U
#define RCAR_GEN4_DBSC4_DBODT0          0x0460U
#define RCAR_GEN4_DBSC4_DBODT1          0x0464U
#define RCAR_GEN4_DBSC4_DBODT2          0x0468U
#define RCAR_GEN4_DBSC4_DBODT3          0x046CU
#define RCAR_GEN4_DBSC4_DBADJ0          0x0500U
#define RCAR_GEN4_DBSC4_DBDBICNT        0x0518U
#define RCAR_GEN4_DBSC4_DBDFIPMSTRCNF   0x0520
#define RCAR_GEN4_DBSC4_DBDFICUPDCNF    0x052CU
#define RCAR_GEN4_DBSC4_DBDFISTAT_0     0x0600
#define RCAR_GEN4_DBSC4_DBDFICNT0       0x0604U
#define RCAR_GEN4_DBSC4_DBPDLK0         0x0620U
#define RCAR_GEN4_DBSC4_DBPDRGA0        0x0624U
#define RCAR_GEN4_DBSC4_DBPDRGD0        0x0628U
#define RCAR_GEN4_DBSC4_DBPDSTAT00      0x0630U
#define RCAR_GEN4_DBSC4_DBDFISTAT_1     0x0640
#define RCAR_GEN4_DBSC4_DBDFICNT1       0x0644U
#define RCAR_GEN4_DBSC4_DBPDLK1         0x0660U
#define RCAR_GEN4_DBSC4_DBPDRGA1        0x0664U
#define RCAR_GEN4_DBSC4_DBPDRGD1        0x0668U
#define RCAR_GEN4_DBSC4_DBDFISTAT_2     0x0680
#define RCAR_GEN4_DBSC4_DBDFICNT2       0x0684U
#define RCAR_GEN4_DBSC4_DBDFISTAT_3     0x06C0
#define RCAR_GEN4_DBSC4_DBPDLK2         0x06A0U
#define RCAR_GEN4_DBSC4_DBPDRGA2        0x06A4U
#define RCAR_GEN4_DBSC4_DBPDRGD2        0x06A8U
#define RCAR_GEN4_DBSC4_DBPDSTAT20      0x06B0U
#define RCAR_GEN4_DBSC4_DBDFICNT3       0x06C4U
#define RCAR_GEN4_DBSC4_DBPDLK3         0x06E0U
#define RCAR_GEN4_DBSC4_DBPDRGA3        0x06E4U
#define RCAR_GEN4_DBSC4_DBPDRGD3        0x06E8U
#define RCAR_GEN4_DBSC4_DBDFICNT_0      0x0604
#define RCAR_GEN4_DBSC4_DBDFICNT_1      0x0644
#define RCAR_GEN4_DBSC4_DBDFICNT_2      0x0684
#define RCAR_GEN4_DBSC4_DBDFICNT_3      0x06C4
#define RCAR_GEN4_DBSC4_DBPDCNT0_0      0x0610
#define RCAR_GEN4_DBSC4_DBPDCNT0_1      0x0650
#define RCAR_GEN4_DBSC4_DBPDCNT0_2      0x0690
#define RCAR_GEN4_DBSC4_DBPDCNT0_3      0x06D0
#define RCAR_GEN4_DBSC4_DBPDCNT3_0      0x061C
#define RCAR_GEN4_DBSC4_DBPDCNT3_1      0x065C
#define RCAR_GEN4_DBSC4_DBPDCNT3_2      0x069C
#define RCAR_GEN4_DBSC4_DBPDCNT3_3      0x06DC
#define RCAR_GEN4_DBSC4_DBPDRGA_0       0x0624
#define RCAR_GEN4_DBSC4_DBPDRGD_0       0x0628
#define RCAR_GEN4_DBSC4_DBPDRGA_1       0x0664
#define RCAR_GEN4_DBSC4_DBPDRGD_1       0x0668
#define RCAR_GEN4_DBSC4_DBPDRGA_2       0x06A4
#define RCAR_GEN4_DBSC4_DBPDRGD_2       0x06A8
#define RCAR_GEN4_DBSC4_DBPDRGA_3       0x06E4
#define RCAR_GEN4_DBSC4_DBPDRGD_3       0x06E8
#define RCAR_GEN4_DBSC4_DBBUS0CNF0      0x0800
#define RCAR_GEN4_DBSC4_DBBUS0CNF1      0x0804
#define RCAR_GEN4_DBSC4_DBCAM0CNF1      0x0904 //CAM Unit Configuration Register 1
#define RCAR_GEN4_DBSC4_DBCAM0CNF2      0x0908 //CAM Unit Configuration Register 2
#define RCAR_GEN4_DBSC4_DBCAM0CNF3      0x090C //CAM Unit Configuration Register 3
#define RCAR_GEN4_DBSC4_DBCAM0STAT0     0x0980U
#define RCAR_GEN4_DBSC4_DBCAM1STAT0     0x0990U
#define RCAR_GEN4_DBSC4_DBSCHCNT0       0x1000 //Scheduler Unit Operation Setting Register 0
#define RCAR_GEN4_DBSC4_DBSCHCNT1       0x1004 //Scheduler Unit Operation Setting Register 1
#define RCAR_GEN4_DBSC4_DBSCHSZ0        0x1010 //Size Miss Scheduling Setting Register 0
#define RCAR_GEN4_DBSC4_DBSCHRW0        0x1020 //Read/Write Scheduling Setting Register 0
#define RCAR_GEN4_DBSC4_DBSCHRW1        0x1024 //Read/Write Scheduling Setting Register 1
#define RCAR_GEN4_DBSC4_DBSCHQOS00      0x1030U
#define RCAR_GEN4_DBSC4_DBSCHQOS01      0x1034U
#define RCAR_GEN4_DBSC4_DBSCHQOS02      0x1038U
#define RCAR_GEN4_DBSC4_DBSCHQOS03      0x103CU
#define RCAR_GEN4_DBSC4_DBSCHQOS10      0x1040U
#define RCAR_GEN4_DBSC4_DBSCHQOS11      0x1044U
#define RCAR_GEN4_DBSC4_DBSCHQOS12      0x1048U
#define RCAR_GEN4_DBSC4_DBSCHQOS13      0x104CU
#define RCAR_GEN4_DBSC4_DBSCHQOS20      0x1050U
#define RCAR_GEN4_DBSC4_DBSCHQOS21      0x1054U
#define RCAR_GEN4_DBSC4_DBSCHQOS22      0x1058U
#define RCAR_GEN4_DBSC4_DBSCHQOS23      0x105CU
#define RCAR_GEN4_DBSC4_DBSCHQOS30      0x1060U
#define RCAR_GEN4_DBSC4_DBSCHQOS31      0x1064U
#define RCAR_GEN4_DBSC4_DBSCHQOS32      0x1068U
#define RCAR_GEN4_DBSC4_DBSCHQOS33      0x106CU
#define RCAR_GEN4_DBSC4_DBSCHQOS40      0x1070U
#define RCAR_GEN4_DBSC4_DBSCHQOS41      0x1074U
#define RCAR_GEN4_DBSC4_DBSCHQOS42      0x1078U
#define RCAR_GEN4_DBSC4_DBSCHQOS43      0x107CU
#define RCAR_GEN4_DBSC4_DBSCHQOS50      0x1080U
#define RCAR_GEN4_DBSC4_DBSCHQOS51      0x1084U
#define RCAR_GEN4_DBSC4_DBSCHQOS52      0x1088U
#define RCAR_GEN4_DBSC4_DBSCHQOS53      0x108CU
#define RCAR_GEN4_DBSC4_DBSCHQOS60      0x1090U
#define RCAR_GEN4_DBSC4_DBSCHQOS61      0x1094U
#define RCAR_GEN4_DBSC4_DBSCHQOS62      0x1098U
#define RCAR_GEN4_DBSC4_DBSCHQOS63      0x109CU
#define RCAR_GEN4_DBSC4_DBSCHQOS70      0x10A0U
#define RCAR_GEN4_DBSC4_DBSCHQOS71      0x10A4U
#define RCAR_GEN4_DBSC4_DBSCHQOS72      0x10A8U
#define RCAR_GEN4_DBSC4_DBSCHQOS73      0x10ACU
#define RCAR_GEN4_DBSC4_DBSCHQOS80      0x10B0U
#define RCAR_GEN4_DBSC4_DBSCHQOS81      0x10B4U
#define RCAR_GEN4_DBSC4_DBSCHQOS82      0x10B8U
#define RCAR_GEN4_DBSC4_DBSCHQOS83      0x10BCU
#define RCAR_GEN4_DBSC4_DBSCHQOS90      0x10C0U
#define RCAR_GEN4_DBSC4_DBSCHQOS91      0x10C4U
#define RCAR_GEN4_DBSC4_DBSCHQOS92      0x10C8U
#define RCAR_GEN4_DBSC4_DBSCHQOS93      0x10CCU
#define RCAR_GEN4_DBSC4_DBSCHQOS100     0x10D0U
#define RCAR_GEN4_DBSC4_DBSCHQOS101     0x10D4U
#define RCAR_GEN4_DBSC4_DBSCHQOS102     0x10D8U
#define RCAR_GEN4_DBSC4_DBSCHQOS103     0x10DCU
#define RCAR_GEN4_DBSC4_DBSCHQOS110     0x10E0U
#define RCAR_GEN4_DBSC4_DBSCHQOS111     0x10E4U
#define RCAR_GEN4_DBSC4_DBSCHQOS112     0x10E8U
#define RCAR_GEN4_DBSC4_DBSCHQOS113     0x10ECU
#define RCAR_GEN4_DBSC4_DBSCHQOS120     0x10F0U
#define RCAR_GEN4_DBSC4_DBSCHQOS121     0x10F4U
#define RCAR_GEN4_DBSC4_DBSCHQOS122     0x10F8U
#define RCAR_GEN4_DBSC4_DBSCHQOS123     0x10FCU
#define RCAR_GEN4_DBSC4_DBSCHQOS130     0x1100U
#define RCAR_GEN4_DBSC4_DBSCHQOS131     0x1104U
#define RCAR_GEN4_DBSC4_DBSCHQOS132     0x1108U
#define RCAR_GEN4_DBSC4_DBSCHQOS133     0x110CU
#define RCAR_GEN4_DBSC4_DBSCHQOS140     0x1110U
#define RCAR_GEN4_DBSC4_DBSCHQOS141     0x1114U
#define RCAR_GEN4_DBSC4_DBSCHQOS142     0x1118U
#define RCAR_GEN4_DBSC4_DBSCHQOS143     0x111CU
#define RCAR_GEN4_DBSC4_DBSCHQOS150     0x1120U
#define RCAR_GEN4_DBSC4_DBSCHQOS151     0x1124U
#define RCAR_GEN4_DBSC4_DBSCHQOS152     0x1128U
#define RCAR_GEN4_DBSC4_DBSCHQOS153     0x112CU
#define RCAR_GEN4_DBSC4_SCFCTST0        0x1700 //Schedule timing setting register 0
#define RCAR_GEN4_DBSC4_SCFCTST1        0x1708 //Schedule timing setting register 1
#define RCAR_GEN4_DBSC4_SCFCTST2        0x170C //Schedule timing setting register 2
#define RCAR_GEN4_DBSC4_DBMRRDR0        0x1800U
#define RCAR_GEN4_DBSC4_DBMRRDR1        0x1804U
#define RCAR_GEN4_DBSC4_DBMRRDR2        0x1808U
#define RCAR_GEN4_DBSC4_DBMRRDR3        0x180CU
#define RCAR_GEN4_DBSC4_DBMRRDR4        0x1810U
#define RCAR_GEN4_DBSC4_DBMRRDR5        0x1814U
#define RCAR_GEN4_DBSC4_DBMRRDR6        0x1818U
#define RCAR_GEN4_DBSC4_DBMRRDR7        0x181CU


/*
 * Thermal Sensor
 */
#define RCAR_GEN4_TSC0_BASE              0xE6190000
#define RCAR_GEN4_TSC1_BASE              0xE6198000
#define RCAR_GEN4_TSC2_BASE              0xE61A0000
#define RCAR_GEN4_TSC3_BASE              0xE61A8000
#define RCAR_GEN4_TSC4_BASE              0xE61B0000
#define RCAR_GEN4_TSC_SIZE               0x1000
#define RCAR_GEN4_THERMAL_TYPE_A         1
#define RCAR_GEN4_THERMAL_TYPE_B         2
#define RCAR_GEN4_THERMAL_TYPE_C         3

#define RCAR_GEN4_TSC_IRQSTR                  0x004
#define RCAR_GEN4_TSC_IRQMSK                  0x008
#define RCAR_GEN4_TSC_IRQCTL                  0x00C
#define RCAR_GEN4_TSC_IRQEN                   0x010
 #define RCAR_GEN4_TSC_IRQ_TEMP1_EN               (1 << 0)
 #define RCAR_GEN4_TSC_IRQ_TEMP2_EN               (1 << 1)
 #define RCAR_GEN4_TSC_IRQ_TEMP3_EN               (1 << 2)
 #define RCAR_GEN4_TSC_IRQ_TEMPD1_EN              (1 << 3)
 #define RCAR_GEN4_TSC_IRQ_TEMPD2_EN              (1 << 4)
 #define RCAR_GEN4_TSC_IRQ_TEMPD3_EN              (1 << 5)
#define RCAR_GEN4_TSC_IRQTEMP1                0x014
#define RCAR_GEN4_TSC_IRQTEMP2                0x018
#define RCAR_GEN4_TSC_IRQTEMP3                0x01C
#define RCAR_GEN4_TSC_THCTR                   0x020
 #define RCAR_GEN4_TSC_THCTR_THSST                (1 << 0)
 #define RCAR_GEN4_TSC_THCTR_THEN                 (1 << 5)
 #define RCAR_GEN4_TSC_THCTR_PONM                 (1 << 6)
 #define RCAR_GEN4_TSC_THCTR_SENSSEL              (3 << 8)
 #define RCAR_GEN4_TSC_THCTR_CIVMTST              (1 << 20)
 #define RCAR_GEN4_TSC_THCTR_CTCTL                (1 << 24)
#define RCAR_GEN4_TSC_THSTR                   0x024
#define RCAR_GEN4_TSC_TEMP                    0x028
 #define TEMP_CODE_MASK                       0xFFF
#define RCAR_GEN4_TSC_VOLT                    0x02C
 #define VOLT_CODE_MASK                       0x3FF
#define RCAR_GEN4_TSC_TEMP_X                  0x030
#define RCAR_GEN4_TSC_TEMP_YN1                0x034
#define RCAR_GEN4_TSC_TEMP_X1                 0x038
#define RCAR_GEN4_TSC_THINITSTR               0x03C
 #define RCAR_GEN4_TSC_THINITSTR_NRME_ND          (1 << 3)
 #define RCAR_GEN4_TSC_THINITSTR_THTMP3           (1 << 2)
 #define RCAR_GEN4_TSC_THINITSTR_THTMP2           (1 << 1)
 #define RCAR_GEN4_TSC_THINITSTR_THTMP1           (1 << 0)
 #define RCAR_GEN4_TSC_THINITSTR_THTMP_MASK       0x7
 #define RCAR_GEN4_TSC_THINITSTR_MASK             0xF
#define RCAR_GEN4_TSC_THCODE_INT1             0x040
#define RCAR_GEN4_TSC_THCODE_INT2             0x044
#define RCAR_GEN4_TSC_THCODE_INT3             0x048
#define RCAR_GEN4_TSC_CVCREG                  0x04C
#define RCAR_GEN4_TSC_THCODE1                 0x050
#define RCAR_GEN4_TSC_THCODE2                 0x054
#define RCAR_GEN4_TSC_THCODE3                 0x058
 #define RCAR_GEN4_TSC_THCODE_MASK            0xFFF
#define RCAR_GEN4_TSC_PTAT1                   0x05C
#define RCAR_GEN4_TSC_PTAT2                   0x060
#define RCAR_GEN4_TSC_PTAT3                   0x064
 #define RCAR_GEN4_TSC_PTAT_MASK              0xFFF
#define RCAR_GEN4_TSC_THSCP                   0x068
 #define COR_PARA_VLD                        (0x03 << 14)
#define RCAR_GEN4_TSC_IRQ_INJECTION           0x06C
#define RCAR_GEN4_TSC_ACK_TIMEOUT             0x070
#define RCAR_GEN4_TSC_THERMAL_ERROR_CTL       0x074
#define RCAR_GEN4_TSC_THSOUTCTL               0x078
#define RCAR_GEN4_TSC_SEQ_RESET               0x07C
#define RCAR_GEN4_TSC_PON_TEMP                0x080
#define RCAR_GEN4_TSC_NML_TEMP                0x084
#define RCAR_GEN4_TSC_NML_VOLT                0x088
#define RCAR_GEN4_TSC_CIVMTST_VOLT1           0x08C
#define RCAR_GEN4_TSC_CIVMTST_VOLT2           0x090
#define RCAR_GEN4_TSC_CIVMTST_VOLT3           0x094
#define RCAR_GEN4_TSC_CIVMTST_VOLT4           0x098
#define RCAR_GEN4_TSC_MANTST_VOLT             0x09C
#define RCAR_GEN4_TSC_VOLT_YN                 0x0A8
#define RCAR_GEN4_TSC_VOLT_X                  0x0B0
#define RCAR_GEN4_TSC_VOLT_X1                 0x0B8
#define RCAR_GEN4_TSC_THS_MANUAL_SET          0x0C0
#define RCAR_GEN4_TSC_VMCODE1                 0x0D0
#define RCAR_GEN4_TSC_VMCODE2                 0x0D4
#define RCAR_GEN4_TSC_VMCODE3                 0x0D8
 #define RCAR_GEN4_TSC_VMCODE_MASK            0x3FF
#define RCAR_GEN4_TSC_HVMCODE1                0x0DC
#define RCAR_GEN4_TSC_HVMCODE2                0x0E0
#define RCAR_GEN4_TSC_HVMCODE3                0x0E4
#define RCAR_GEN4_TSC_INBUF_OFF1              0x0E8
#define RCAR_GEN4_TSC_INBUF_OFF2              0x0EC
#define RCAR_GEN4_TSC_INBUF_OFF3              0x0F0
#define RCAR_GEN4_TSC_CVM_LOCK                0x0F8
#define RCAR_GEN4_TSC_CVM_LOCK_BK             0x0FC
#define RCAR_GEN4_TSC_CVM_LATCH_BK            0x100
#define RCAR_GEN4_TSC_CVM_CTRL                0x104
 #define RCAR_GEN4_TSC_CVM_CTRL_OUTPC_CVM                (1 << 8)
 #define RCAR_GEN4_TSC_CVM_CTRL_EXTERNAL_FS_CVM_OUTPUT   (0 << 4)
 #define RCAR_GEN4_TSC_CVM_CTRL_EXTERNAL_FS_THS_OUTPUT   (1 << 4)
 #define RCAR_GEN4_TSC_CVM_CTRL_CHOICE_PLACE_CVM4_EN     (1 << 3)
 #define RCAR_GEN4_TSC_CVM_CTRL_CHOICE_PLACE_CVM3_EN     (1 << 2)
 #define RCAR_GEN4_TSC_CVM_CTRL_CHOICE_PLACE_CVM2_EN     (1 << 1)
 #define RCAR_GEN4_TSC_CVM_CTRL_CHOICE_PLACE_CVM1_EN     (1 << 0)
#define RCAR_GEN4_TSC_CVM_CTRL_BK              0x108
 #define RCAR_GEN4_TSC_CVM_CTRL_BK_THEN_BK          (1 << 12)
#define RCAR_GEN4_TSC_CVM_DETECT_MON          0x10C
#define RCAR_GEN4_TSC_CVM_DETECT_MANUAL_SET   0x110
#define RCAR_GEN4_TSC_CVM_DETECT_SET_BK       0x114
#define RCAR_GEN4_TSC_CVM_TOFF_MON            0x118
#define RCAR_GEN4_TSC_CVM_TOFF_MANUAL_SET     0x11C
#define RCAR_GEN4_TSC_CVM_TOFF_BK             0x120
#define RCAR_GEN4_TSC_CVM_VMOUT_BK            0x124
#define RCAR_GEN4_TSC_STATMON1                0x128
#define RCAR_GEN4_TSC_STATMON2                0x12C
#define RCAR_GEN4_TSC_THERMAL_ERROR_MON       0x130
#define RCAR_GEN4_TSC_THCODE_MON              0x134
#define RCAR_GEN4_TSC_MANTST_SET_DT           0x138
#define RCAR_GEN4_TSC_TMR_DT                  0x13C
#define RCAR_GEN4_TSC_SEQ_ACT_MON             0x140
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_MANTSTSEQ_ACT   (1 << 4)
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_CIVMTSTSEQ_ACT  (1 << 3)
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_NMLSEQ_ACT      (1 << 2)
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_PONSEQ_ACT      (1 << 1)
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_TSTSEQ_ACT      (1 << 0)
 #define RCAR_GEN4_TSC_SEQ_ACT_MON_ACT_MASK        (0x1F << 0)
#define RCAR_GEN4_TSC_VMMSK_CTRL              0x144
#define RCAR_GEN4_TSC_VMMSK_CTRL_MON          0x148
#define RCAR_GEN4_TSC_VM_MON                  0x14C
#define RCAR_GEN4_TSC_VM06CODE1               0x150
#define RCAR_GEN4_TSC_VM06CODE2               0x154
#define RCAR_GEN4_TSC_VM06CODE3               0x158
#define RCAR_GEN4_TSC_VM11CODE1               0x15C
#define RCAR_GEN4_TSC_VM11CODE2               0x160
#define RCAR_GEN4_TSC_VM11CODE3               0x164
#define RCAR_GEN4_TSC_VMCVMCODE1              0x168
#define RCAR_GEN4_TSC_VMCVMCODE2              0x16C
#define RCAR_GEN4_TSC_VMCVMCODE3              0x170

/* Thermal type B */
#define RCAR_GEN4_TSC_B_STR                   0x00
#define RCAR_GEN4_TSC_B_ENR                   0x04
#define RCAR_GEN4_TSC_B_INT_MASK              0x0C
 #define RCAR_GEN4_TSC_B_Tj00INT                 (1 << 0)
 #define RCAR_GEN4_TSC_B_Tj01INT                 (1 << 1)
 #define RCAR_GEN4_TSC_B_Tj02INT                 (1 << 2)
#define RCAR_GEN4_TSC_B_POSNEG                0x120
#define RCAR_GEN4_TSC_B_THSCR                 0x12C
 #define RCAR_GEN4_TSC_B_THSCR_CPCTL             (1 << 12)
#define RCAR_GEN4_TSC_B_THSSR                 0x130
 #define RCAR_GEN4_TSC_THSSR_CTEMP_MASK          0x3F
#define RCAR_GEN4_TSC_B_INTCTLR               0x134
 #define INTCTLR_CTEMP0_MASK                (0x3F << 0)
 #define INTCTLR_CTEMP1_MASK                (0x3F << 8)
 #define INTCTLR_CTEMP2_MASK                (0x3F << 16)
 #define INTCTLR_CTEMP0_VAL(val)            (((val) & 0x3F) << 0)
 #define INTCTLR_CTEMP1_VAL(val)            (((val) & 0x3F) << 8)
 #define INTCTLR_CTEMP2_VAL(val)            (((val) & 0x3F) << 16)
#define RCAR_GEN4_TSC_B_CIVM_SR               0x21C
 #define RCAR_GEN4_TSC_VOLT_CODE_B_MASK       0x3F


/*
 * Video Capture
 */
#define RCAR_GEN4_VIN0_BASE              0xE6EF0000
#define RCAR_GEN4_VIN1_BASE              0xE6EF1000
#define RCAR_GEN4_VIN2_BASE              0xE6EF2000
#define RCAR_GEN4_VIN3_BASE              0xE6EF3000
#define RCAR_GEN4_VIN4_BASE              0xE6EF4000
#define RCAR_GEN4_VIN5_BASE              0xE6EF5000
#define RCAR_GEN4_VIN6_BASE              0xE6EF6000
#define RCAR_GEN4_VIN7_BASE              0xE6EF7000
#define RCAR_GEN4_VIN8_BASE              0xE6EF8000 /* V3H & V3U only */
#define RCAR_GEN4_VIN9_BASE              0xE6EF9000
#define RCAR_GEN4_VIN10_BASE             0xE6EFA000
#define RCAR_GEN4_VIN11_BASE             0xE6EFB000
#define RCAR_GEN4_VIN12_BASE             0xE6EFC000
#define RCAR_GEN4_VIN13_BASE             0xE6EFD000
#define RCAR_GEN4_VIN14_BASE             0xE6EFE000
#define RCAR_GEN4_VIN15_BASE             0xE6EFF000
#define RCAR_GEN4_VIN16_BASE             0xE6ED0000 /* V3U only */
#define RCAR_GEN4_VIN17_BASE             0xE6ED1000
#define RCAR_GEN4_VIN18_BASE             0xE6ED2000
#define RCAR_GEN4_VIN19_BASE             0xE6ED3000
#define RCAR_GEN4_VIN20_BASE             0xE6ED4000
#define RCAR_GEN4_VIN21_BASE             0xE6ED5000
#define RCAR_GEN4_VIN22_BASE             0xE6ED6000
#define RCAR_GEN4_VIN23_BASE             0xE6ED7000
#define RCAR_GEN4_VIN24_BASE             0xE6ED8000
#define RCAR_GEN4_VIN25_BASE             0xE6ED9000
#define RCAR_GEN4_VIN26_BASE             0xE6EDA000
#define RCAR_GEN4_VIN27_BASE             0xE6EDB000
#define RCAR_GEN4_VIN28_BASE             0xE6EDC000
#define RCAR_GEN4_VIN29_BASE             0xE6EDD000
#define RCAR_GEN4_VIN30_BASE             0xE6EDE000
#define RCAR_GEN4_VIN31_BASE             0xE6EDF000

#define RCAR_GEN4_VIN_SIZE               0x1000

#define RCAR_GEN4_VIN_MC                 0x00000
 #define RCAR_GEN4_VIN_MC_MASK            0xFFFFF
 #define RCAR_GEN4_VIN_MC_DPINE               (1 << 27)
 #define RCAR_GEN4_VIN_MC_SCLE                (1 << 26)
 #define RCAR_GEN4_VIN_MC_FCPE                (1 << 24)
 #define RCAR_GEN4_VIN_MC_FOC                 (1 << 21)
 #define RCAR_GEN4_VIN_MC_YCAL                (1 << 19)
 #define RCAR_GEN4_VIN_MC_INF_YUV8_BT656      (0 << 16)
 #define RCAR_GEN4_VIN_MC_INF_YUV8_BT601      (1 << 16)
 #define RCAR_GEN4_VIN_MC_INF_YUV10_BT656     (2 << 16)
 #define RCAR_GEN4_VIN_MC_INF_YUV10_BT601     (3 << 16)
 #define RCAR_GEN4_VIN_MC_INF_YUV16           (5 << 16)
 #define RCAR_GEN4_VIN_MC_INF_RGB888          (6 << 16)
 #define RCAR_GEN4_VIN_MC_INF_MASK            (7 << 16)
 #define RCAR_GEN4_VIN_MC_VUP                 (1 << 10)
 #define RCAR_GEN4_VIN_MC_IM_MSK              (3 << 3)
 #define RCAR_GEN4_VIN_MC_IM_ODD              (0 << 3)
 #define RCAR_GEN4_VIN_MC_IM_ODD_EVEN         (1 << 3)
 #define RCAR_GEN4_VIN_MC_IM_EVEN             (2 << 3)
 #define RCAR_GEN4_VIN_MC_IM_FULL             (3 << 3)
 #define RCAR_GEN4_VIN_MC_BPS                 (1 << 1)
 #define RCAR_GEN4_VIN_MC_ME                  (1 << 0)

#define RCAR_GEN4_VIN_MS                     0x0004
 #define RCAR_GEN4_VIN_MS_FBS_MASK            (3 << 3)
 #define RCAR_GEN4_VIN_MS_FBS_SHIFT           3
 #define RCAR_GEN4_VIN_MS_AV                  (1 << 1)
 #define RCAR_GEN4_VIN_MS_CA                  (1 << 0)

#define RCAR_GEN4_VIN_FC                     0x0008
 #define RCAR_GEN4_VIN_FC_C_FRAME             (1 << 1)
 #define RCAR_GEN4_VIN_FC_S_FRAME             (1 << 0)
#define RCAR_GEN4_VIN_SLPRC             0x000C
#define RCAR_GEN4_VIN_ELPRC             0x0010
#define RCAR_GEN4_VIN_SPPRC             0x0014
#define RCAR_GEN4_VIN_EPPRC             0x0018
#define RCAR_GEN4_VIN_CSI_IFMD          0x0020
 #define RCAR_GEN4_VIN_CSI_IFMD_DES2     (1 << 27)
 #define RCAR_GEN4_VIN_CSI_IFMD_DES1     (1 << 26)
 #define RCAR_GEN4_VIN_CSI_IFMD_DES0     (1 << 25)
 #define RCAR_GEN4_VIN_CSI_IFMD_CSI_CHSEL(n)  ((n) << 0)
#define RCAR_GEN4_VIN_IS                0x002C
#define RCAR_GEN4_VIN_LC                0x003C
#define RCAR_GEN4_VIN_IE                0x0040
 #define RCAR_GEN4_VIN_IE_FIE            (1 << 4)
 #define RCAR_GEN4_VIN_IE_EFE            (1 << 1)
 #define RCAR_GEN4_VIN_IE_FOE            (1 << 0)
#define RCAR_GEN4_VIN_INTS              0x0044
 #define RCAR_GEN4_VIN_INTS_FIS          (1 << 4)
 #define RCAR_GEN4_VIN_INTS_EFS          (1 << 1)
 #define RCAR_GEN4_VIN_INTS_FOS          (1 << 0)
#define RCAR_GEN4_VIN_SI                0x0048
#define RCAR_GEN4_VIN_DMR               0x0058
 #define RCAR_GEN4_VIN_DMR_EXRGB                (1 << 8)
 #define RCAR_GEN4_VIN_DMR_BPSM                 (1 << 4)
 #define RCAR_GEN4_VIN_DMR_ABIT                 (1 << 2)
 #define RCAR_GEN4_VIN_DMR_DTMD_YCSEP           (1 << 1)
 #define RCAR_GEN4_VIN_DMR_DTMD_ARGB            (1 << 0)
 #define RCAR_GEN4_VIN_DMR_A8BIT              (0xFF << 24)
 #define RCAR_GEN4_VIN_DMR_DTMD_YCSEP_YCBCR420  (3 << 0)
#define RCAR_GEN4_VIN_DMR2              0x005C
 #define RCAR_GEN4_VIN_DMR2_VPS          (1 << 30)
 #define RCAR_GEN4_VIN_DMR2_HPS          (1 << 29)
 #define RCAR_GEN4_VIN_DMR2_FTEV         (1 << 17)
 #define RCAR_GEN4_VIN_DMR2_VLV(n)       (((n) & 0xf) << 12)
#define RCAR_GEN4_VIN_UVAOF             0x0060
#define RCAR_GEN4_VIN_UDS_CTRL          0x0080
 #define RCAR_GEN4_VIN_UDS_CTRL_AMD      (1 << 30)
 #define RCAR_GEN4_VIN_UDS_CTRL_BC       (1 << 20)
 #define RCAR_GEN4_VIN_UDS_CTRL_TDIPC    (1 << 1)
#define RCAR_GEN4_VIN_UDS_SCALE         0x0084
#define RCAR_GEN4_VIN_UDS_PASS_BW       0x0090
#define RCAR_GEN4_VIN_UDS_IPC           0x0098
#define RCAR_GEN4_VIN_UDS_CLIPSIZE      0x00A4
#define RCAR_GEN4_VIN_LUTP              0x0100
#define RCAR_GEN4_VIN_LUTD              0x0104
#define RCAR_GEN4_VIN_MB(x)             (0x0030 + 0x04*(x))
#define RCAR_GEN4_VIN_CSCC(x)           (0x0064 + 0x04*(x))
#define RCAR_GEN4_VIN_YCCR(x)           (0x0228 + 0x04*(x))
#define RCAR_GEN4_VIN_CBCCR(x)          (0x0234 + 0x04*(x))
#define RCAR_GEN4_VIN_CRCCR(x)          (0x0240 + 0x04*(x))
#define RCAR_GEN4_VIN_CSCE(x)           (0x0300 + 0x04*(x))

/*
 * Camera Serial Interface 2
 */
#define RCAR_GEN4_CSI40_BASE             0xFEAA0000
#define RCAR_GEN4_CSI41_BASE             0xFEAB0000
#define RCAR_GEN4_CSI42_BASE             0xFED60000
#define RCAR_GEN4_CSI43_BASE             0xFED70000
#define RCAR_GEN4_CSI20_BASE             0xFEA80000
#define RCAR_GEN4_CSI21_BASE             0xFEA90000
#define RCAR_GEN4_CSI2_SIZE              0x10000

/*  Offset from base */
#define RCAR_GEN4_CSI2_TREF             0x00
#define RCAR_GEN4_CSI2_SRST             0x04
#define RCAR_GEN4_CSI2_PHYCNT           0x08
#define RCAR_GEN4_CSI2_CHKSUM           0x0C
#define RCAR_GEN4_CSI2_VCDT             0x10
#define RCAR_GEN4_CSI2_VCDT2            0x14
#define RCAR_GEN4_CSI2_FRDT             0x18
#define RCAR_GEN4_CSI2_FLD              0x1C
#define RCAR_GEN4_CSI2_ASTBY            0x20
#define RCAR_GEN4_CSI2_LNGDT0           0x28
#define RCAR_GEN4_CSI2_LNGDT1           0x2C
#define RCAR_GEN4_CSI2_INTEN            0x30
#define RCAR_GEN4_CSI2_INTCLOSE         0x34
#define RCAR_GEN4_CSI2_INTSTATE         0x38
#define RCAR_GEN4_CSI2_INTERRSTATE      0x3C
#define RCAR_GEN4_CSI2_SHPDAT           0x40
#define RCAR_GEN4_CSI2_SHPCNT           0x44
#define RCAR_GEN4_CSI2_LINKCNT          0x48
#define RCAR_GEN4_CSI2_LSWAP            0x4C
#define RCAR_GEN4_CSI2_PHTW             0x50
#define RCAR_GEN4_CSI2_PHTC             0x58
#define RCAR_GEN4_CSI2_PHYPLL           0x68
#define RCAR_GEN4_CSI2_PHEERM           0x74
#define RCAR_GEN4_CSI2_PHCLM            0x78
#define RCAR_GEN4_CSI2_PHDLM            0x7C

/*
 * ISP
 */
#define RCAR_GEN4_ISP0_BASE              0xFED00000
#define RCAR_GEN4_ISP0_CORE_BASE         0xFEC00000
#define RCAR_GEN4_ISP1_BASE              0xFED20000
#define RCAR_GEN4_ISP1_CORE_BASE         0xFEE00000
#define RCAR_GEN4_ISP2_BASE              0xFED30000
#define RCAR_GEN4_ISP2_CORE_BASE         0xFEF00000
#define RCAR_GEN4_ISP3_BASE              0xFED40000
#define RCAR_GEN4_ISP3_CORE_BASE         0xFE400000
#define RCAR_GEN4_ISP_SIZE               0x20000

//Basic control registers
#define RCAR_GEN4_ISPVCR				 (0x0000)
#define RCAR_GEN4_ISPFIFOCTRL		 (0x0004)
#define RCAR_GEN4_ISPINPUTSEL0		 (0x0008)
#define RCAR_GEN4_ISPSTART			 (0x0014)
#define RCAR_GEN4_ISPINT_STATUS		 (0x0040)
#define RCAR_GEN4_ISPERR0_STATUS		 (0x0044)
#define RCAR_GEN4_ISPERR1_STATUS		 (0x0048)
#define RCAR_GEN4_ISPERR2_STATUS		 (0x004C)
#define RCAR_GEN4_ISPERR3_STATUS		 (0x0080)
#define RCAR_GEN4_ISPERR4_STATUS		 (0x0084)
#define RCAR_GEN4_ISPINT_CLEAR		 (0x0050)
#define RCAR_GEN4_ISPERR0_CLEAR		 (0x0054)
#define RCAR_GEN4_ISPERR1_CLEAR		 (0x0058)
#define RCAR_GEN4_ISPERR2_CLEAR		 (0x005C)
#define RCAR_GEN4_ISPERR3_CLEAR		 (0x0090)
#define RCAR_GEN4_ISPERR4_CLEAR		 (0x0094)
#define RCAR_GEN4_ISPINT_ENABLE		 (0x0060)
#define RCAR_GEN4_ISPERR0_ENABLE		 (0x0064)
#define RCAR_GEN4_ISPERR1_ENABLE		 (0x0068)
#define RCAR_GEN4_ISPERR2_ENABLE		 (0x006C)
#define RCAR_GEN4_ISPERR3_ENABLE		 (0x00A0)
#define RCAR_GEN4_ISPERR4_ENABLE		 (0x00A4)
#define RCAR_GEN4_ISP_PADDING_CTRL		 (0x00C0)
//Pixel reconstructor register
#define RCAR_GEN4_ISPPROC_CUSTOM_FORMATn_CTRL(n)			(0x1010 + (4 * (n)))
#define RCAR_GEN4_ISP_PROCMODE_DT(n)						(0x1100 + (4 * (n)))
#define RCAR_GEN4_ISPPROC_CUSTOM_FORMAT_LUT0_Wn_B3b(n, b)	(0x8000 + (0x20 * (n)) + (4 * (b)))
//Synchronized system control register
#define RCAR_GEN4_ISPWUP_CONFIG0	            (0x2000)
#define RCAR_GEN4_ISPWUP_CONFIG1             (0x2004)
#define RCAR_GEN4_ISPWUP_EOF_CONFIG          (0x2008)
#define RCAR_GEN4_ISPWUP_STATUS              (0x200C)
#define RCAR_GEN4_ISPWUP_EOF_MATCH_ADRESSn(n)  (0x2100 + (4 * (n)))
#define RCAR_GEN4_ISPWUP_EOF_MATCH_IDn(n)      (0x2200 + (4 * (n)))
//Pixel count checker control register
#define RCAR_GEN4_PIXEL_COUNT_MAX_CHn(n)	    (0x4000 + (0x10 * (n)))
#define RCAR_GEN4_PIXEL_COUNT_MIN_CHn(n)     (0x4004 + (0x10 * (n)))
#define RCAR_GEN4_PIXEL_COUNT_MONITOR_CHn(n)	(0x4008 + (0x10 * (n)))
//Channel selector register
#define RCAR_GEN4_ISPCS_FILTER_ID_CHn(n)			(0x3000 + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_LC_MODULO_CHn(n)			(0x3004 + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_DT_CODE03_CHn(n)			(0x3008 + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_DT_CODE47_CHn(n)			(0x300C + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_H_CLIP_DT_CODEm_CHn(m, n)	(0x3020 + (4 * (m)) + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_V_CLIP_DT_CODEm_CHn(m, n)	(0x3030 + (4 * (m)) + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_OUTPUT_0TO3_MODE_CHn(n)	(0x0020 + (4 * (n)))
#define RCAR_GEN4_ISPCS_OUTPUT_4to7_MODE_CHn(n)	(0x0120 + (4 * (n)))
#define RCAR_GEN4_ISPCS_DI_FILTER_CTRL_CHn(n)		(0x3040 + (0x100 * (n)))
#define RCAR_GEN4_ISPCS_DI_FILTER_LUTp_CHn(p, n)	(0x3080 + (4 * (p)) + (0x100 * (n)))
//Write protect control register
#define RCAR_GEN4_ISPWP_CTRL			  (0x0100)
//Streaming IP register
#define RCAR_GEN4_ISPSTREAMER_MODE		  (0x7000)
#define RCAR_GEN4_ISPSTREAMER_V_BLANK	  (0x7004)
#define RCAR_GEN4_ISPSTREAMER_H_BLANK	  (0x7008)


/*
* IMR-LX4
*/
#define RCAR_GEN4_IMRLX40_BASE             0xFE860000
#define RCAR_GEN4_IMRLX41_BASE             0xFE870000
#define RCAR_GEN4_IMRLX42_BASE             0xFE880000
#define RCAR_GEN4_IMRLX43_BASE             0xFE890000
#define RCAR_GEN4_IMRLX4_SIZE              2048

#define RCAR_GEN4_IMRLX4_PULSE             58
#define RCAR_GEN4_IMRLX4_END               59

//Control registers
#define RCAR_GEN4_IMRLX4_CR                0x0008
#define RCAR_GEN4_IMRLX4_SR                0x000C
#define RCAR_GEN4_IMRLX4_SRCR              0x0010
#define RCAR_GEN4_IMRLX4_ICR               0x0014
#define RCAR_GEN4_IMRLX4_IMR               0x0018
#define RCAR_GEN4_IMRLX4_DLSP              0x001C
#define RCAR_GEN4_IMRLX4_DLPR              0x0020
#define RCAR_GEN4_IMRLX4_FUSAR             0x0024
#define RCAR_GEN4_IMRLX4_EDLR              0x0028
//Memory control registers
#define RCAR_GEN4_IMRLX4_DLSAR             0x0030
#define RCAR_GEN4_IMRLX4_DSAR              0x0034
#define RCAR_GEN4_IMRLX4_SSAR              0x0038
#define RCAR_GEN4_IMRLX4_DSTR              0x003C
#define RCAR_GEN4_IMRLX4_SSTR              0x0040
#define RCAR_GEN4_IMRLX4_DSOR              0x0050
//Rendering control registers
#define RCAR_GEN4_IMRLX4_CMRCR             0x0054
#define RCAR_GEN4_IMRLX4_CMRCSR            0x0058
#define RCAR_GEN4_IMRLX4_CMRCCR            0x005C
#define RCAR_GEN4_IMRLX4_TRIMR             0x0060
#define RCAR_GEN4_IMRLX4_TRIMSR            0x0064
#define RCAR_GEN4_IMRLX4_TRIMCR            0x0068
#define RCAR_GEN4_IMRLX4_TRICR             0x006C
#define RCAR_GEN4_IMRLX4_UVDPOR            0x0070
#define RCAR_GEN4_IMRLX4_SUSR              0x0074
#define RCAR_GEN4_IMRLX4_SVSR              0x0078
#define RCAR_GEN4_IMRLX4_XMINR             0x0080
#define RCAR_GEN4_IMRLX4_YMINR             0x0084
#define RCAR_GEN4_IMRLX4_XMAXR             0x0088
#define RCAR_GEN4_IMRLX4_YMAXR             0x008c
#define RCAR_GEN4_IMRLX4_AMXSR             0x0090
#define RCAR_GEN4_IMRLX4_AMYSR             0x0094
#define RCAR_GEN4_IMRLX4_AMXOR             0x0098
#define RCAR_GEN4_IMRLX4_AMYOR             0x009C
#define RCAR_GEN4_IMRLX4_TRICR2            0x00A0
#define RCAR_GEN4_IMRLX4_YLMINR            0x00B0
#define RCAR_GEN4_IMRLX4_UBMINR            0x00B4
#define RCAR_GEN4_IMRLX4_VRMINR            0x00B8
#define RCAR_GEN4_IMRLX4_YLMAXR            0x00BC
#define RCAR_GEN4_IMRLX4_UBMAXR            0x00C0
#define RCAR_GEN4_IMRLX4_VRMAXR            0x00C4
#define RCAR_GEN4_IMRLX4_CPDPOR            0x00D0
#define RCAR_GEN4_IMRLX4_YLCPR             0x00D4
#define RCAR_GEN4_IMRLX4_UBCPR             0x00D8
#define RCAR_GEN4_IMRLX4_VRCPR             0x00DC
#define RCAR_GEN4_IMRLX4_CMRCR2            0x00E4
#define RCAR_GEN4_IMRLX4_CMRCSR2           0x00E8
#define RCAR_GEN4_IMRLX4_CMRCCR2           0x00EC
#define RCAR_GEN4_IMRLX4_LUTDR(n)          (0x1000 | (n))
//Display list instruction
#define RCAR_GEN4_IMRLX4_INST_TRI          0x8A
#define RCAR_GEN4_IMRLX4_INST_NOP          0x80
#define RCAR_GEN4_IMRLX4_INST_TRAP         0x8F
#define RCAR_GEN4_IMRLX4_INST_WTL          0x81
#define RCAR_GEN4_IMRLX4_INST_WTL2         0x83
#define RCAR_GEN4_IMRLX4_INST_WTS          0x82
#define RCAR_GEN4_IMRLX4_INST_INT          0x88
#define RCAR_GEN4_IMRLX4_INST_SYNCM        0x86
#define RCAR_GEN4_IMRLX4_INST_GOBSUB       0x8C
#define RCAR_GEN4_IMRLX4_INST_RET          0x8D

#endif /* __AARCH64_RCAR_GEN4_H_INCLUDED */
