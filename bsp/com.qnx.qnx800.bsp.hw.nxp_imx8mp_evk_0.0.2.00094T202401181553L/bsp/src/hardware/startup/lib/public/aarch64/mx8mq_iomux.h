/*
 * $QNXLicenseC:
 * Copyright 2022 BlackBerry Limited.
 * Copyright 2018 NXP
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

#ifndef IMX_IOMUX_H_
#define IMX_IOMUX_H_

/* IOMUXC Registers, offset from base address */
#define IMX_IOMUXC_SW_MUX_CTL_PADx(n)                      ((n * 4) + 0x14)
#define IMX_IOMUXC_SW_PAD_CTL_PADx(n)                      ((n * 4) + 0x254)
#define IMX_IOMUXC_INPUTx(n)                               ((n * 4) + 0x4BC)
/*
 * Bit definitions for SW_MUX_CTL registers
 */
#define IMX_MUX_CTL_SION                                   (0x01 << 4)
#define IMX_MUX_CTL_MUX_MODE_ALT0                           0
#define IMX_MUX_CTL_MUX_MODE_ALT1                           1
#define IMX_MUX_CTL_MUX_MODE_ALT2                           2
#define IMX_MUX_CTL_MUX_MODE_ALT3                           3
#define IMX_MUX_CTL_MUX_MODE_ALT4                           4
#define IMX_MUX_CTL_MUX_MODE_ALT5                           5
#define IMX_MUX_CTL_MUX_MODE_ALT6                           6
#define IMX_MUX_CTL_MUX_MODE_ALT7                           7

/*
 * Bit definitions for SW_PAD_CTL registers
 */
/* Lvttl Enable */
#define IMX_PAD_CTL_LVTTL_DISABLE                          (0x0 << 8)
#define IMX_PAD_CTL_LVTTL_ENABLE                           (0x1 << 8)
#define IMX_PAD_CTL_LVTTL_MASK                             (0x1 << 8)

/* Schmitt trigger Enable */
#define IMX_PAD_CTL_HYS_DISABLE                            (0x0 << 7)
#define IMX_PAD_CTL_HYS_ENABLE                             (0x1 << 7)
#define IMX_PAD_CTL_HYS_MASK                               (0x1 << 7)

/* Pull Up Enable */
#define IMX_PAD_CTL_PUE_DISABLE                            (0x0 << 6)
#define IMX_PAD_CTL_PUE_ENABLE                             (0x1 << 6)
#define IMX_PAD_CTL_PUE_MASK                               (0x1 << 6)

/* Open Drain Enable */
#define IMX_PAD_CTL_ODE_DISABLE                            (0x0 << 5)
#define IMX_PAD_CTL_ODE_ENABLE                             (0x1 << 5)
#define IMX_PAD_CTL_ODE_MASK                               (0x1 << 5)

/* Slew Rate */
#define IMX_PAD_CTL_SRE_SLOW                               (0x00 << 3)
#define IMX_PAD_CTL_SRE_MEDIUM                             (0x01 << 3)
#define IMX_PAD_CTL_SRE_FAST                               (0x01 << 3)
#define IMX_PAD_CTL_SRE_MAX                                (0x03 << 3)
#define IMX_PAD_CTL_SRE_MASK                               (0x03 << 3)

/* Drive strength */
#define IMX_PAD_CTL_DSE_HIZ                                (0x0 << 0)
#define IMX_PAD_CTL_DSE_255_OHM                            (0x1 << 0)
#define IMX_PAD_CTL_DSE_105_OHM                            (0x2 << 0)
#define IMX_PAD_CTL_DSE_75_OHM                             (0x3 << 0)
#define IMX_PAD_CTL_DSE_85_OHM                             (0x4 << 0)
#define IMX_PAD_CTL_DSE_65_OHM                             (0x5 << 0)
#define IMX_PAD_CTL_DSE_45_OHM                             (0x6 << 0)
#define IMX_PAD_CTL_DSE_40_OHM                             (0x7 << 0)

/* Input Select (DAISY) */
#define IMX_PAD_CTL_DAISY_DISABLE                          (0x0 << 0)
#define IMX_PAD_CTL_DAISY_ENABLE                           (0x1 << 0)
#define IMX_PAD_CTL_DAISY_MASK                             (0x1 << 0)

/*
 * Offsets of IOMUXC registers from IMX_IOMUX_SWMUX
 * where IMX_IOMUX_SWMUX = IMX_IOMUXC_BASE + 0x0014
 */
#define IMX_IOMUXC_SW_MUX_CTL_PAD_PMIC_STBY_REQ            0
#define IMX_IOMUXC_SW_MUX_CTL_PAD_PMIC_ON_REQ              1
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ONOFF                    2
#define IMX_IOMUXC_SW_MUX_CTL_PAD_POR_B                    3
#define IMX_IOMUXC_SW_MUX_CTL_PAD_RTC_RESET_B              4
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00               5
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01               6
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02               7
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03               8
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04               9
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO05               10
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO06               11
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO07               12
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO08               13
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09               14
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO10               15
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO11               16
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO12               17
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO13               18
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO14               19
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO15               20
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC                 21
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO                22
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD3                 23
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD2                 24
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD1                 25
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD0                 26
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_CTL              27
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TXC                 28
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_CTL              29
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RXC                 30
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD0                 31
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD1                 32
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD2                 33
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD3                 34
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_CLK                  35
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD                  36
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0                37
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1                38
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2                39
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3                40
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA4                41
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA5                42
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA6                43
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA7                44
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_RESET_B              45
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_STROBE               46
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CD_B                 47
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK                  48
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD                  49
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0                50
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1                51
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2                52
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3                53
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_RESET_B              54
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_WP                   55
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_ALE                 56
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE0_B               57
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B               58
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE2_B               59
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE3_B               60
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CLE                 61
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00              62
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01              63
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02              64
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03              65
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04              66
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05              67
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06              68
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07              69
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DQS                 70
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_RE_B                71
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_READY_B             72
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WE_B                73
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B                74
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXFS                75
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXC                 76
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD0                77
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD1                78
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD2                79
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD3                80
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_MCLK                81
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXFS                82
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXC                 83
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD0                84
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD1                85
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD2                86
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD3                87
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD4                88
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD5                89
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD6                90
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD7                91
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXFS                92
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXC                 93
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD0                94
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD1                95
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD2                96
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD3                97
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD4                98
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD5                99
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD6                100
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD7                101
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_MCLK                102
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXFS                103
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXC                 104
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXD0                105
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXFS                106
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXC                 107
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXD0                108
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_MCLK                109
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXFS                110
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXC                 111
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXD                 112
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXFS                113
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXC                 114
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXD                 115
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_MCLK                116
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_TX                 117
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_RX                 118
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_EXT_CLK            119
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_SCLK              120
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_MOSI              121
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_MISO              122
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_SS0               123
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_SCLK              124
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_MOSI              125
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_MISO              126
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_SS0               127
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SCL                 128
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SDA                 129
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL                 130
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA                 131
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C3_SCL                 132
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C3_SDA                 133
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C4_SCL                 134
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C4_SDA                 135
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD                136
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD                137
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_RXD                138
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_TXD                139
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART3_RXD                140
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART3_TXD                141
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART4_RXD                142
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART4_TXD                143

/*
 * Offsets of IOMUXC registers from IMX_IOMUX_SW_PAD
 * where IMX_IOMUX_SW_PAD = IMX_IOMUXC_BASE + 0x0254
 */
#define IMX_IOMUXC_SW_PAD_CTL_PAD_TEST_MODE                0
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE0               1
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE1               2
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_MOD                 3
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TRST_B              4
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDI                 5
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TMS                 6
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TCK                 7
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDO                 8
#define IMX_IOMUXC_SW_PAD_CTL_PAD_RTC                      9
#define IMX_IOMUXC_SW_PAD_CTL_PAD_PMIC_STBY_REQ            10
#define IMX_IOMUXC_SW_PAD_CTL_PAD_PMIC_ON_REQ              11
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ONOFF                    12
#define IMX_IOMUXC_SW_PAD_CTL_PAD_POR_B                    13
#define IMX_IOMUXC_SW_PAD_CTL_PAD_RTC_RESET_B              14
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO00               15
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO01               16
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO02               17
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03               18
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04               19
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO05               20
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO06               21
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO07               22
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO08               23
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO09               24
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO10               25
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO11               26
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO12               27
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO13               28
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO14               29
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO15               30
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_MDC                 31
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_MDIO                32
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD3                 33
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD2                 34
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD1                 35
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD0                 36
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TX_CTL              37
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TXC                 38
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RX_CTL              39
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RXC                 40
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD0                 41
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD1                 42
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD2                 43
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD3                 44
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_CLK                  45
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_CMD                  46
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA0                47
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1                48
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2                49
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3                50
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA4                51
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA5                52
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA6                53
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA7                54
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_RESET_B              55
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_STROBE               56
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CD_B                 57
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CLK                  58
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CMD                  59
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA0                60
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA1                61
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA2                62
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA3                63
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_RESET_B              64
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_WP                   65
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_ALE                 66
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE0_B               67
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE1_B               68
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE2_B               69
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE3_B               70
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CLE                 71
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA00              72
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA01              73
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA02              74
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA03              75
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04              76
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA05              77
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA06              78
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA07              79
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DQS                 80
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_RE_B                81
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_READY_B             82
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WE_B                83
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WP_B                84
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXFS                85
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXC                 86
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD0                87
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD1                88
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD2                89
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD3                90
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_MCLK                91
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXFS                92
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXC                 93
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD0                94
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD1                95
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD2                96
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD3                97
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD4                98
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD5                99
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD6                100
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD7                101
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXFS                102
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXC                 103
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD0                104
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD1                105
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD2                106
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD3                107
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD4                108
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD5                109
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD6                110
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD7                111
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_MCLK                112
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXFS                113
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXC                 114
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXD0                115
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXFS                116
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXC                 117
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXD0                118
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_MCLK                119
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXFS                120
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXC                 121
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXD                 122
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXFS                123
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXC                 124
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXD                 125
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_MCLK                126
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_TX                 127
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_RX                 128
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_EXT_CLK            129
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_SCLK              130
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_MOSI              131
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_MISO              132
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_SS                133
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_SCLK              134
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_MOSI              135
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_MISO              136
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_SS0               137
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SCL                 138
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SDA                 139
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL                 140
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA                 141
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C3_SCL                 142
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C3_SDA                 143
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C4_SCL                 144
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C4_SDA                 145
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD                146
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD                147
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART2_RXD                148
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART2_TXD                149
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART3_RXD                150
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART3_TXD                151
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART4_RXD                152
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART4_TXD                153

/*
 * Offsets of IOMUXC registers from IMX_IOMUX_INPUT
 * where IMX_IOMUX_INPUT = IMX_IOMUXC_BASE + 0x04BC
 */
#define IMX_IOMUXC_CCM_PMIC_READY_SELECT_INPUT             0
#define IMX_IOMUXC_ENET1_MDIO_SELECT_INPUT                 1
#define IMX_IOMUXC_SAI1_RX_SYNC_SELECT_INPUT               2
#define IMX_IOMUXC_SAI1_TX_BCLK_SELECT_INPUT               3
#define IMX_IOMUXC_SAI1_TX_SYNC_SELECT_INPUT               4
#define IMX_IOMUXC_SAI5_RX_BCLK_SELECT_INPUT               5
#define IMX_IOMUXC_SAI5_RXD0_SELECT_INPUT                  6
#define IMX_IOMUXC_SAI5_RXD1_SELECT_INPUT                  7
#define IMX_IOMUXC_SAI5_RXD2_SELECT_INPUT                  8
#define IMX_IOMUXC_SAI5_RXD3_SELECT_INPUT                  9
#define IMX_IOMUXC_SAI5_RX_SYNC_SELECT_INPUT               10
#define IMX_IOMUXC_SAI5_TX_BCLK_SELECT_INPUT               11
#define IMX_IOMUXC_SAI5_TX_SYNC_SELECT_INPUT               12
#define IMX_IOMUXC_UART1_RTS_B_SELECT_INPUT                13
#define IMX_IOMUXC_UART1_RXD_SELECT_INPUT                  14
#define IMX_IOMUXC_UART2_RTS_B_SELECT_INPUT                15
#define IMX_IOMUXC_UART2_RXD_SELECT_INPUT                  16
#define IMX_IOMUXC_UART3_RTS_B_SELECT_INPUT                17
#define IMX_IOMUXC_UART3_RXD_SELECT_INPUT                  18
#define IMX_IOMUXC_UART4_RTS_B_SELECT_INPUT                19
#define IMX_IOMUXC_UART4_RXD_SELECT_INPUT                  20
#define IMX_IOMUXC_SAI6_RX_BCLK_SELECT_INPUT               21
#define IMX_IOMUXC_SAI6_RXD0_SELECT_INPUT                  22
#define IMX_IOMUXC_SAI6_RX_SYNC_SELECT_INPUT               23
#define IMX_IOMUXC_SAI6_TX_BCLK_SELECT_INPUT               24
#define IMX_IOMUXC_SAI6_TX_SYNC_SELECT_INPUT               25
#define IMX_IOMUXC_PCIE1_CLKREQ_B_SELECT_INPUT             26
#define IMX_IOMUXC_PCIE2_CLKREQ_B_SELECT_INPUT             27
#define IMX_IOMUXC_SAI5_MCLK_SELECT_INPUT                  28
#define IMX_IOMUXC_SAI6_MCLK_SELECT_INPUT                  29

#endif  /* IMX_IOMUX_H_ */
