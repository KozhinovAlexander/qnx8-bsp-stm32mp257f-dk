/*
 * $QNXLicenseC:
 * Copyright 2022 BlackBerry Limited.
 * Copyright 2019 NXP
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

#ifndef IMX_DDRC_H_
#define IMX_DDRC_H_

/* DDRC registers, offset from base address */
#define IMX_DDRC_MSTR                       0x0000
#define IMX_DDRC_STAT                       0x0004
#define IMX_DDRC_MSTR1                      0x0008
#define IMX_DDRC_MRCTRL0                    0x0010
#define IMX_DDRC_MRCTRL1                    0x0014
#define IMX_DDRC_MRSTAT                     0x0018
#define IMX_DDRC_MRCTRL2                    0x001C
#define IMX_DDRC_DERATEEN                   0x0020
#define IMX_DDRC_DERATEINT                  0x0024
#define IMX_DDRC_MSTR2                      0x0028
#define IMX_DDRC_PWRCTL                     0x0030
#define IMX_DDRC_PWRTMG                     0x0034
#define IMX_DDRC_HWLPCTL                    0x0038
#define IMX_DDRC_HWFFCCTL                   0x003C
#define IMX_DDRC_HWFFCSTAT                  0x0040
#define IMX_DDRC_RFSHCTL0                   0x0050
#define IMX_DDRC_RFSHCTL1                   0x0054
#define IMX_DDRC_RFSHCTL2                   0x0058
#define IMX_DDRC_RFSHCTL3                   0x0060
#define IMX_DDRC_RFSHTMG                    0x0064
#define IMX_DDRC_ECCCFG0                    0x0070
#define IMX_DDRC_ECCCFG1                    0x0074
#define IMX_DDRC_ECCSTAT                    0x0078
#define IMX_DDRC_ECCCLR                     0x007C
#define IMX_DDRC_ECCERRCNT                  0x0080
#define IMX_DDRC_ECCCADDR0                  0x0084
#define IMX_DDRC_ECCCADDR1                  0x0088
#define IMX_DDRC_ECCCSYN0                   0x008C
#define IMX_DDRC_ECCCSYN1                   0x0090
#define IMX_DDRC_ECCCSYN2                   0x0094
#define IMX_DDRC_ECCBITMASK0                0x0098
#define IMX_DDRC_ECCBITMASK1                0x009C
#define IMX_DDRC_ECCBITMASK2                0x00A0
#define IMX_DDRC_ECCUADDR0                  0x00A4
#define IMX_DDRC_ECCUADDR1                  0x00A8
#define IMX_DDRC_ECCUSYN0                   0x00AC
#define IMX_DDRC_ECCUSYN1                   0x00B0
#define IMX_DDRC_ECCUSYN2                   0x00B4
#define IMX_DDRC_ECCPOISONADDR0             0x00B8
#define IMX_DDRC_ECCPOISONADDR1             0x00BC
#define IMX_DDRC_CRCPARCTL0                 0x00C0
#define IMX_DDRC_CRCPARCTL1                 0x00C4
#define IMX_DDRC_CRCPARCTL2                 0x00C8
#define IMX_DDRC_CRCPARSTAT                 0x00CC
#define IMX_DDRC_INIT0                      0x00D0
#define IMX_DDRC_INIT1                      0x00D4
#define IMX_DDRC_INIT2                      0x00D8
#define IMX_DDRC_INIT3                      0x00DC
#define IMX_DDRC_INIT4                      0x00E0
#define IMX_DDRC_INIT5                      0x00E4
#define IMX_DDRC_INIT6                      0x00E8
#define IMX_DDRC_INIT7                      0x00EC
#define IMX_DDRC_DIMMCTL                    0x00F0
#define IMX_DDRC_RANKCTL                    0x00F4
#define IMX_DDRC_DRAMTMG0                   0x0100
#define IMX_DDRC_DRAMTMG1                   0x0104
#define IMX_DDRC_DRAMTMG2                   0x0108
#define IMX_DDRC_DRAMTMG3                   0x010C
#define IMX_DDRC_DRAMTMG4                   0x0110
#define IMX_DDRC_DRAMTMG5                   0x0114
#define IMX_DDRC_DRAMTMG6                   0x0118
#define IMX_DDRC_DRAMTMG7                   0x011C
#define IMX_DDRC_DRAMTMG8                   0x0120
#define IMX_DDRC_DRAMTMG9                   0x0124
#define IMX_DDRC_DRAMTMG10                  0x0128
#define IMX_DDRC_DRAMTMG11                  0x012C
#define IMX_DDRC_DRAMTMG12                  0x0130
#define IMX_DDRC_DRAMTMG13                  0x0134
#define IMX_DDRC_DRAMTMG14                  0x0138
#define IMX_DDRC_DRAMTMG15                  0x013C
#define IMX_DDRC_DRAMTMG16                  0x0140
#define IMX_DDRC_DRAMTMG17                  0x0144
#define IMX_DDRC_ZQCTL0                     0x0180
#define IMX_DDRC_ZQCTL1                     0x0184
#define IMX_DDRC_ZQCTL2                     0x0188
#define IMX_DDRC_ZQSTAT                     0x018C
#define IMX_DDRC_DFITMG0                    0x0190
#define IMX_DDRC_DFITMG1                    0x0194
#define IMX_DDRC_DFILPCFG0                  0x0198
#define IMX_DDRC_DFILPCFG1                  0x019C
#define IMX_DDRC_DFIUPD0                    0x01A0
#define IMX_DDRC_DFIUPD1                    0x01A4
#define IMX_DDRC_DFIUPD2                    0x01A8
#define IMX_DDRC_DFIMISC                    0x01B0
#define IMX_DDRC_DFITMG2                    0x01B4
#define IMX_DDRC_DFITMG3                    0x01B8
#define IMX_DDRC_DFISTAT                    0x01BC
#define IMX_DDRC_DBICTL                     0x01C0
#define IMX_DDRC_DFIPHYMSTR                 0x01C4
#define IMX_DDRC_TRAINCTL0                  0x01D0
#define IMX_DDRC_TRAINCTL1                  0x01D4
#define IMX_DDRC_TRAINCTL2                  0x01D8
#define IMX_DDRC_TRAINSTAT                  0x01DC
#define IMX_DDRC_ADDRMAP0                   0x0200
#define IMX_DDRC_ADDRMAP1                   0x0204
#define IMX_DDRC_ADDRMAP2                   0x0208
#define IMX_DDRC_ADDRMAP3                   0x020C
#define IMX_DDRC_ADDRMAP4                   0x0210
#define IMX_DDRC_ADDRMAP5                   0x0214
#define IMX_DDRC_ADDRMAP6                   0x0218
#define IMX_DDRC_ADDRMAP7                   0x021C
#define IMX_DDRC_ADDRMAP8                   0x0220
#define IMX_DDRC_ADDRMAP9                   0x0224
#define IMX_DDRC_ADDRMAP10                  0x0228
#define IMX_DDRC_ADDRMAP11                  0x022C
#define IMX_DDRC_ODTCFG                     0x0240
#define IMX_DDRC_ODTMAP                     0x0244
#define IMX_DDRC_SCHED                      0x0250
#define IMX_DDRC_SCHED1                     0x0254
#define IMX_DDRC_PERFHPR1                   0x025C
#define IMX_DDRC_PERFLPR1                   0x0264
#define IMX_DDRC_PERFWR1                    0x026C
#define IMX_DDRC_PERFVPR1                   0x0274
#define IMX_DDRC_PERFVPW1                   0x0278
#define IMX_DDRC_DQMAP0                     0x0280
#define IMX_DDRC_DQMAP1                     0x0284
#define IMX_DDRC_DQMAP2                     0x0288
#define IMX_DDRC_DQMAP3                     0x028C
#define IMX_DDRC_DQMAP4                     0x0290
#define IMX_DDRC_DQMAP5                     0x0294
#define IMX_DDRC_DBG0_0                     0x0300
#define IMX_DDRC_DBG1                       0x0304
#define IMX_DDRC_DBGCAM                     0x0308
#define IMX_DDRC_DBGCMD                     0x030C
#define IMX_DDRC_DBGSTAT                    0x0310
#define IMX_DDRC_SWCTL                      0x0320
#define IMX_DDRC_SWSTAT                     0x0324
#define IMX_DDRC_OCPARCFG0                  0x0330
#define IMX_DDRC_OCPARCFG1                  0x0334
#define IMX_DDRC_OCPARCFG2                  0x0338
#define IMX_DDRC_OCPARCFG3                  0x033C
#define IMX_DDRC_OCPARSTAT0                 0x0340
#define IMX_DDRC_OCPARSTAT1                 0x0344
#define IMX_DDRC_OCPARWLOG0_0               0x0348
#define IMX_DDRC_OCPARWLOG1_0               0x034C
#define IMX_DDRC_OCPARWLOG2_0               0x0350
#define IMX_DDRC_OCPARAWLOG0_0              0x0354
#define IMX_DDRC_OCPARAWLOG1_0              0x0358
#define IMX_DDRC_OCPARRLOG0_0               0x035C
#define IMX_DDRC_OCPARRLOG1_0               0x0360
#define IMX_DDRC_OCPARARLOG0_0              0x0364
#define IMX_DDRC_OCPARARLOG1_0              0x0368
#define IMX_DDRC_POISONCFG_0                0x036C
#define IMX_DDRC_POISONSTAT_0               0x0370
#define IMX_DDRC_ADVECCINDEX_0              0x0003
#define IMX_DDRC_ADVECCSTAT_0               0x0003
#define IMX_DDRC_ECCPOISONPAT0              0x0003
#define IMX_DDRC_ECCPOISONPAT1              0x0003
#define IMX_DDRC_ECCPOISONPAT2              0x0003
#define IMX_DDRC_HIFCTL                     0x0003
#define IMX_DDRC_PSTAT                      0x03FC
#define IMX_DDRC_PCCFG                      0x0400
#define IMX_DDRC_PCFGR_0                    0x0404
#define IMX_DDRC_PCFGR_1                    0x04B4
#define IMX_DDRC_PCFGR_2                    0x0564
#define IMX_DDRC_PCFGR_3                    0x0614
#define IMX_DDRC_PCFGW_0                    0x0408
#define IMX_DDRC_PCFGW_1                    0x0408
#define IMX_DDRC_PCFGW_2                    0x0568
#define IMX_DDRC_PCFGW_3                    0x0618
#define IMX_DDRC_PCFGC_0                    0x040C
#define IMX_DDRC_PCFGIDMASKCH               0x0410
#define IMX_DDRC_PCFGIDVALUECH              0x0414
#define IMX_DDRC_PCTRL_0                    0x0490
#define IMX_DDRC_PCTRL_1                    0x0540
#define IMX_DDRC_PCTRL_2                    0x05F0
#define IMX_DDRC_PCTRL_3                    0x06A0
#define IMX_DDRC_PCFGQOS0_0                 0x0494
#define IMX_DDRC_PCFGQOS1_0                 0x0498
#define IMX_DDRC_PCFGWQOS0_0                0x049C
#define IMX_DDRC_PCFGWQOS1_0                0x04A0
#define IMX_DDRC_SARBASE0                   0x0F04
#define IMX_DDRC_SARSIZE0                   0x0F08
#define IMX_DDRC_SBRCTL                     0x0F24
#define IMX_DDRC_SBRSTAT                    0x0F28
#define IMX_DDRC_SBRWDATA0                  0x0F2C
#define IMX_DDRC_SBRWDATA1                  0x0F30
#define IMX_DDRC_PDCH                       0x0F34
#define IMX_DDRC_RFSHCTL0_SHADOW            0x2050
#define IMX_DDRC_RFSHTMG_SHADOW             0x2064
#define IMX_DDRC_INIT3_SHADOW               0x20DC
#define IMX_DDRC_INIT4_SHADOW               0x20E0
#define IMX_DDRC_INIT6_SHADOW               0x20E8
#define IMX_DDRC_INIT7_SHADOW               0x20EC
#define IMX_DDRC_DRAMTMG0_SHADOW            0x2100
#define IMX_DDRC_DRAMTMG1_SHADOW            0x2104
#define IMX_DDRC_DRAMTMG2_SHADOW            0x2108
#define IMX_DDRC_DRAMTMG3_SHADOW            0x210C
#define IMX_DDRC_DRAMTMG4_SHADOW            0x2110
#define IMX_DDRC_DRAMTMG5_SHADOW            0x2114
#define IMX_DDRC_DRAMTMG6_SHADOW            0x2118
#define IMX_DDRC_DRAMTMG7_SHADOW            0x211C
#define IMX_DDRC_DRAMTMG8_SHADOW            0x2120
#define IMX_DDRC_DRAMTMG9_SHADOW            0x2124
#define IMX_DDRC_DRAMTMG10_SHADOW           0x2128
#define IMX_DDRC_DRAMTMG11_SHADOW           0x212C
#define IMX_DDRC_DRAMTMG12_SHADOW           0x2130
#define IMX_DDRC_DRAMTMG13_SHADOW           0x2134
#define IMX_DDRC_DRAMTMG14_SHADOW           0x2138
#define IMX_DDRC_DRAMTMG15_SHADOW           0x213C
#define IMX_DDRC_ZQCTL0_SHADOW              0x2180
#define IMX_DDRC_DFITMG0_SHADOW             0x2190
#define IMX_DDRC_DFITMG1_SHADOW             0x2194
#define IMX_DDRC_DFITMG2_SHADOW             0x21B4
#define IMX_DDRC_DFITMG3_SHADOW             0x21B8
#define IMX_DDRC_ODTCFG_SHADOW              0x2240

/* DDRPHY registers, offset from base address */
#define IMX_DDRPHY_PORT_CONTROL_0           0x80240
#define IMX_DDRPHY_CAL_BUSY_0               0x8025C
#define IMX_DDRPHY_CAL_MISC2_0              0x80260
#define IMX_DDRPHY_CAL_MISC_0               0x80268
#define IMX_DDRPHY_CAL_CMPR5_0              0x80270
#define IMX_DDRPHY_CAL_NINT_0               0x80274
#define IMX_DDRPHY_CAL_PEXT_0               0x80278
#define IMX_DDRPHY_CAL_CMP_INVERT_0         0x802A0
#define IMX_DDRPHY_CAL_CMPANA_CTRL_0        0x802B8

#endif
