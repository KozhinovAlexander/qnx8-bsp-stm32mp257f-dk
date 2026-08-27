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

#ifndef IMX_IOMUX_H_
#define IMX_IOMUX_H_

/* IOMUXC Registers, offset from base address */
#define IMX_IOMUXC_SW_MUX_CTL_PADx(n)                      (((n) * 4) + 0x14)
#define IMX_IOMUXC_SW_PAD_CTL_PADx(n)                      (((n) * 4) + 0x250)
#define IMX_IOMUXC_INPUTx(n)                               (((n) * 4) + 0x4C0)
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
/* Pull Enable/Disable */
#define IMX_PAD_CTL_PE_PULL_DISABLED                       (0x0 << 8)
#define IMX_PAD_CTL_PE_PULL_ENABLED                        (0x1 << 8)

/* Schmitt/CMOS */
#define IMX_PAD_CTL_HYS_CMOS                               (0x0 << 7)
#define IMX_PAD_CTL_HYS_SCHMITT                            (0x1 << 7)

/* Pull Up/Pull Down */
#define IMX_PAD_CTL_PUE_PULL_DOWN                          (0x0 << 6)
#define IMX_PAD_CTL_PUE_PULL_UP                            (0x1 << 6)

/* Open Drain/Push Pull */
#define IMX_PAD_CTL_ODE_PUSH_PULL                          (0x0 << 5)
#define IMX_PAD_CTL_ODE_OPEN_DRAIN                         (0x1 << 5)

/* Slew Rate */
#define IMX_PAD_CTL_SRE_SLOW                               (0x0 << 4)
#define IMX_PAD_CTL_SRE_FAST                               (0x1 << 4)

/* Drive strength */
#define IMX_PAD_CTL_DSE_1X                                 (0x0 << 1)
#define IMX_PAD_CTL_DSE_2X                                 (0x1 << 1)
#define IMX_PAD_CTL_DSE_4X                                 (0x2 << 1)
#define IMX_PAD_CTL_DSE_6X                                 (0x3 << 1)

/* Input Select (DAISY) */
#define IMX_PAD_CTL_DAISY_DISABLE                          (0x0 << 0)
#define IMX_PAD_CTL_DAISY_ENABLE                           (0x1 << 0)
#define IMX_PAD_CTL_DAISY_MASK                             (0x1 << 0)

/*
 * Offsets of IOMUXC registers from IMX_IOMUX_SWMUX
 * where IMX_IOMUX_SWMUX = IMX_IOMUXC_BASE + 0x0014
 */
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO00             0
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO01             1
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO02             2
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO03             3
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO04             4
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO05             5
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO06             6
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO07             7
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO08             8
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO09             9
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO10             10
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO11             11
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO12             12
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO13             13
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO14             14
#define IMX_IOMUXC_SW_MUX_CTL_PAD_GPIO1_IO15             15
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC               16
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO              17
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD3               18
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD2               19
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD1               20
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TD0               21
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_CTL            22
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_TXC               23
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_CTL            24
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RXC               25
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD0               26
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD1               27
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD2               28
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ENET_RD3               29
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_CLK                30
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD                31
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0              32
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1              33
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2              34
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3              35
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA4              36
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA5              37
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA6              38
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA7              39
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_RESET_B            40
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD1_STROBE             41
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CD_B               42
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK                43
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD                44
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0              45
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1              46
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2              47
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3              48
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_RESET_B            49
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SD2_WP                 50
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_ALE               51
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE0_B             52
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B             53
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE2_B             54
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CE3_B             55
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_CLE               56
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00            57
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01            58
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02            59
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03            60
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04            61
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05            62
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06            63
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07            64
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_DQS               65
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_RE_B              66
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_READY_B           67
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WE_B              68
#define IMX_IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B              69
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXFS              70
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXC               71
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD0              72
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD1              73
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD2              74
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_RXD3              75
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI5_MCLK              76
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXFS              77
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXC               78
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD0              79
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD1              80
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD2              81
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD3              82
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD4              83
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD5              84
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD6              85
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_RXD7              86
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXFS              87
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXC               88
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD0              89
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD1              90
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD2              91
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD3              92
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD4              93
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD5              94
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD6              95
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_TXD7              96
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI1_MCLK              97
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXFS              98
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXC               99
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_RXD0              100
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXFS              101
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXC               102
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_TXD0              103
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI2_MCLK              104
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXFS              105
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXC               106
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_RXD               107
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXFS              108
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXC               109
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_TXD               110
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SAI3_MCLK              111
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_TX               112
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_RX               113
#define IMX_IOMUXC_SW_MUX_CTL_PAD_SPDIF_EXT_CLK          114
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_SCLK            115
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_MOSI            116
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_MISO            117
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI1_SS0             118
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_SCLK            119
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_MOSI            120
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_MISO            121
#define IMX_IOMUXC_SW_MUX_CTL_PAD_ECSPI2_SS0             122
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SCL               123
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C1_SDA               124
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C2_SCL               125
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C2_SDA               126
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C3_SCL               127
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C3_SDA               128
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C4_SCL               129
#define IMX_IOMUXC_SW_MUX_CTL_PAD_I2C4_SDA               130
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART1_RXD              131
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART1_TXD              132
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_RXD              133
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART2_TXD              134
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART3_RXD              135
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART3_TXD              136
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART4_RXD              137
#define IMX_IOMUXC_SW_MUX_CTL_PAD_UART4_TXD              138
#define IMX_IOMUXC_SW_MUX_CTL_PAD_HDMI_DDC_SCL           139
#define IMX_IOMUXC_SW_MUX_CTL_PAD_HDMI_DDC_SDA           140
#define IMX_IOMUXC_SW_MUX_CTL_PAD_HDMI_CEC               141
#define IMX_IOMUXC_SW_MUX_CTL_PAD_HDMI_HPD               142

/*
 * Offsets of IOMUXC registers from IMX_IOMUX_INPUT
 * where IMX_IOMUX_INPUT = IMX_IOMUXC_BASE + 0x04BC
 */
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE0            0
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE1            1
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE2            2
#define IMX_IOMUXC_SW_PAD_CTL_PAD_BOOT_MODE3            3
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_MOD              4
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDI              5
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TMS              6
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TCK              7
#define IMX_IOMUXC_SW_PAD_CTL_PAD_JTAG_TDO              8
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO00            9
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO01            10
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO02            11
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO03            12
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO04            13
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO05            14
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO06            15
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO07            16
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO08            17
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO09            18
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO10            19
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO11            20
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO12            21
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO13            22
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO14            23
#define IMX_IOMUXC_SW_PAD_CTL_PAD_GPIO1_IO15            24
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_MDC              25
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_MDIO             26
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD3              27
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD2              28
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD1              29
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TD0              30
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TX_CTL           31
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_TXC              32
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RX_CTL           33
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RXC              34
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD0              35
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD1              36
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD2              37
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ENET_RD3              38
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_CLK               39
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_CMD               40
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA0             41
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA1             42
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA2             43
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA3             44
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA4             45
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA5             46
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA6             47
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_DATA7             48
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_RESET_B           49
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD1_STROBE            50
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CD_B              51
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CLK               52
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_CMD               53
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA0             54
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA1             55
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA2             56
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_DATA3             57
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_RESET_B           58
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SD2_WP                59
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_ALE              60
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE0_B            61
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE1_B            62
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE2_B            63
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CE3_B            64
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_CLE              65
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA00           66
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA01           67
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA02           68
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA03           69
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04           70
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA05           71
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA06           72
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DATA07           73
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_DQS              74
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_RE_B             75
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_READY_B          76
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WE_B             77
#define IMX_IOMUXC_SW_PAD_CTL_PAD_NAND_WP_B             78
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXFS             79
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXC              80
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD0             81
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD1             82
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD2             83
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_RXD3             84
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI5_MCLK             85
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXFS             86
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXC              87
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD0             88
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD1             89
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD2             90
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD3             91
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD4             92
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD5             93
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD6             94
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_RXD7             95
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXFS             96
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXC              97
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD0             98
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD1             99
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD2             100
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD3             101
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD4             102
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD5             103
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD6             104
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_TXD7             105
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI1_MCLK             106
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXFS             107
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXC              108
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_RXD0             109
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXFS             110
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXC              111
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_TXD0             112
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI2_MCLK             113
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXFS             114
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXC              115
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_RXD              116
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXFS             117
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXC              118
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_TXD              119
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SAI3_MCLK             120
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_TX              121
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_RX              122
#define IMX_IOMUXC_SW_PAD_CTL_PAD_SPDIF_EXT_CLK         123
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_SCLK           124
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_MOSI           125
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_MISO           126
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI1_SS0            127
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_SCLK           128
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_MOSI           129
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_MISO           130
#define IMX_IOMUXC_SW_PAD_CTL_PAD_ECSPI2_SS0            131
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SCL              132
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C1_SDA              133
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C2_SCL              134
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C2_SDA              135
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C3_SCL              136
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C3_SDA              137
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C4_SCL              138
#define IMX_IOMUXC_SW_PAD_CTL_PAD_I2C4_SDA              139
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART1_RXD             140
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART1_TXD             141
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART2_RXD             142
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART2_TXD             143
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART3_RXD             144
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART3_TXD             145
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART4_RXD             146
#define IMX_IOMUXC_SW_PAD_CTL_PAD_UART4_TXD             147
#define IMX_IOMUXC_SW_PAD_CTL_PAD_HDMI_DDC_SCL          148
#define IMX_IOMUXC_SW_PAD_CTL_PAD_HDMI_DDC_SDA          149
#define IMX_IOMUXC_SW_PAD_CTL_PAD_HDMI_CEC              150
#define IMX_IOMUXC_SW_PAD_CTL_PAD_HDMI_HPD              151
#define IMX_IOMUXC_SW_PAD_CTL_PAD_CLKIN1                152
#define IMX_IOMUXC_SW_PAD_CTL_PAD_CLKIN2                153
#define IMX_IOMUXC_SW_PAD_CTL_PAD_CLKOUT1               154
#define IMX_IOMUXC_SW_PAD_CTL_PAD_CLKOUT2               155


#define IMX_IOMUXC_AUDIOMIX_PDM_MIC_PDM_BITSTREAM_SELECT_INPUT_0 0
#define IMX_IOMUXC_AUDIOMIX_PDM_MIC_PDM_BITSTREAM_SELECT_INPUT_1 1
#define IMX_IOMUXC_AUDIOMIX_PDM_MIC_PDM_BITSTREAM_SELECT_INPUT_2 2
#define IMX_IOMUXC_AUDIOMIX_PDM_MIC_PDM_BITSTREAM_SELECT_INPUT_3 3
#define IMX_IOMUXC_AUDIOMIX_SAI1_RXSYNC_SELECT_INPUT    4
#define IMX_IOMUXC_AUDIOMIX_SAI1_TXBCLK_SELECT_INPUT    5
#define IMX_IOMUXC_AUDIOMIX_SAI1_TXSYNC_SELECT_INPUT    6
#define IMX_IOMUXC_AUDIOMIX_SAI2_RXDATA_SELECT_INPUT_1  7
#define IMX_IOMUXC_AUDIOMIX_SAI3_MCLK_SELECT_INPUT      8
#define IMX_IOMUXC_AUDIOMIX_SAI3_RXDATA_SELECT_INPUT_0  9
#define IMX_IOMUXC_AUDIOMIX_SAI3_TXBCLK_SELECT_INPUT    10
#define IMX_IOMUXC_AUDIOMIX_SAI3_TXSYNC_SELECT_INPUT    11
#define IMX_IOMUXC_AUDIOMIX_SAI5_MCLK_SELECT_INPUT      12
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXBCLK_SELECT_INPUT    13
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXDATA_SELECT_INPUT_0  14
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXDATA_SELECT_INPUT_1  15
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXDATA_SELECT_INPUT_2  16
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXDATA_SELECT_INPUT_3  17
#define IMX_IOMUXC_AUDIOMIX_SAI5_RXSYNC_SELECT_INPUT    18
#define IMX_IOMUXC_AUDIOMIX_SAI5_TXBCLK_SELECT_INPUT    19
#define IMX_IOMUXC_AUDIOMIX_SAI5_TXSYNC_SELECT_INPUT    20
#define IMX_IOMUXC_AUDIOMIX_SAI6_MCLK_SELECT_INPUT      21
#define IMX_IOMUXC_AUDIOMIX_SAI6_RXBCLK_SELECT_INPUT    22
#define IMX_IOMUXC_AUDIOMIX_SAI6_RXDATA_SELECT_INPUT_0  23
#define IMX_IOMUXC_AUDIOMIX_SAI6_RXSYNC_SELECT_INPUT    24
#define IMX_IOMUXC_AUDIOMIX_SAI6_TXBCLK_SELECT_INPUT    25
#define IMX_IOMUXC_AUDIOMIX_SAI6_TXSYNC_SELECT_INPUT    26
#define IMX_IOMUXC_AUDIOMIX_SAI7_MCLK_SELECT_INPUT      27
#define IMX_IOMUXC_AUDIOMIX_SAI7_RXBCLK_SELECT_INPUT    28
#define IMX_IOMUXC_AUDIOMIX_SAI7_RXDATA_SELECT_INPUT_0  29
#define IMX_IOMUXC_AUDIOMIX_SAI7_RXSYNC_SELECT_INPUT    30
#define IMX_IOMUXC_AUDIOMIX_SAI7_TXBCLK_SELECT_INPUT    31
#define IMX_IOMUXC_AUDIOMIX_SAI7_TXSYNC_SELECT_INPUT    32
#define IMX_IOMUXC_AUDIOMIX_EARC_PHY_SPDIF_IN_SELECT_INPUT 33
#define IMX_IOMUXC_AUDIOMIX_SPDIF_EXTCLK_SELECT_INPUT   34
#define IMX_IOMUXC_CAN1_CANRX_SELECT_INPUT              35
#define IMX_IOMUXC_CAN2_CANRX_SELECT_INPUT              36
#define IMX_IOMUXC_CCM_GPC_PMIC_VFUNCTIONAL_READY_SELECT_INPUT 37
#define IMX_IOMUXC_ECSPI1_CSPI_CLK_IN_SELECT_INPUT      38
#define IMX_IOMUXC_ECSPI1_MISO_SELECT_INPUT             39
#define IMX_IOMUXC_ECSPI1_MOSI_SELECT_INPUT             40
#define IMX_IOMUXC_ECSPI1_SS_B_SELECT_INPUT_0           41
#define IMX_IOMUXC_ECSPI2_CSPI_CLK_IN_SELECT_INPUT      42
#define IMX_IOMUXC_ECSPI2_MISO_SELECT_INPUT             43
#define IMX_IOMUXC_ECSPI2_MOSI_SELECT_INPUT             44
#define IMX_IOMUXC_ECSPI2_SS_B_SELECT_INPUT_0           45
#define IMX_IOMUXC_ENET1_IPG_CLK_RMII_SELECT_INPUT      46
#define IMX_IOMUXC_ENET1_MDIO_SELECT_INPUT              47
#define IMX_IOMUXC_ENET1_RXDATA_0_SELECT_INPUT          48
#define IMX_IOMUXC_ENET1_RXDATA_1_SELECT_INPUT          49
#define IMX_IOMUXC_ENET1_RXEN_SELECT_INPUT              50
#define IMX_IOMUXC_ENET1_RXERR_SELECT_INPUT             51
#define IMX_IOMUXC_ENET_QOS_GMII_MDI_I_SELECT_INPUT     52
#define IMX_IOMUXC_GPT1_CAPIN1_SELECT_INPUT             53
#define IMX_IOMUXC_GPT1_CAPIN2_SELECT_INPUT             54
#define IMX_IOMUXC_GPT1_CLKIN_SELECT_INPUT              55
#define IMX_IOMUXC_PCIE_CLKREQ_B_SELECT_INPUT           56
#define IMX_IOMUXC_I2C1_SCL_IN_SELECT_INPUT             57
#define IMX_IOMUXC_I2C1_SDA_IN_SELECT_INPUT             58
#define IMX_IOMUXC_I2C2_SCL_IN_SELECT_INPUT             59
#define IMX_IOMUXC_I2C2_SDA_IN_SELECT_INPUT             60
#define IMX_IOMUXC_I2C3_SCL_IN_SELECT_INPUT             61
#define IMX_IOMUXC_I2C3_SDA_IN_SELECT_INPUT             62
#define IMX_IOMUXC_I2C4_SCL_IN_SELECT_INPUT             63
#define IMX_IOMUXC_I2C4_SDA_IN_SELECT_INPUT             64
#define IMX_IOMUXC_I2C5_SCL_IN_SELECT_INPUT             65
#define IMX_IOMUXC_I2C5_SDA_IN_SELECT_INPUT             66
#define IMX_IOMUXC_I2C6_SCL_IN_SELECT_INPUT             67
#define IMX_IOMUXC_I2C6_SDA_IN_SELECT_INPUT             68
#define IMX_IOMUXC_ISP_FL_TRIG_0_SELECT_INPUT           69
#define IMX_IOMUXC_ISP_FL_TRIG_1_SELECT_INPUT           70
#define IMX_IOMUXC_ISP_SHUTTER_TRIG_0_SELECT_INPUT      71
#define IMX_IOMUXC_ISP_SHUTTER_TRIG_1_SELECT_INPUT      72
#define IMX_IOMUXC_UART1_UART_RTS_B_SELECT_INPUT        73
#define IMX_IOMUXC_UART1_UART_RXD_MUX_SELECT_INPUT      74
#define IMX_IOMUXC_UART2_UART_RTS_B_SELECT_INPUT        75
#define IMX_IOMUXC_UART2_UART_RXD_MUX_SELECT_INPUT      76
#define IMX_IOMUXC_UART3_UART_RTS_B_SELECT_INPUT        77
#define IMX_IOMUXC_UART3_UART_RXD_MUX_SELECT_INPUT      78
#define IMX_IOMUXC_UART4_UART_RTS_B_SELECT_INPUT        79
#define IMX_IOMUXC_UART4_UART_RXD_MUX_SELECT_INPUT      80
#define IMX_IOMUXC_USDHC3_CARD_CLK_IN_SELECT_INPUT      81
#define IMX_IOMUXC_USDHC3_CARD_DET_SELECT_INPUT         82
#define IMX_IOMUXC_USDHC3_CMD_IN_SELECT_INPUT           83
#define IMX_IOMUXC_USDHC3_DAT0_IN_SELECT_INPUT          84
#define IMX_IOMUXC_USDHC3_DAT1_IN_SELECT_INPUT          85
#define IMX_IOMUXC_USDHC3_DAT2_IN_SELECT_INPUT          86
#define IMX_IOMUXC_USDHC3_DAT3_IN_SELECT_INPUT          87
#define IMX_IOMUXC_USDHC3_DAT4_IN_SELECT_INPUT          88
#define IMX_IOMUXC_USDHC3_DAT5_IN_SELECT_INPUT          89
#define IMX_IOMUXC_USDHC3_DAT6_IN_SELECT_INPUT          90
#define IMX_IOMUXC_USDHC3_DAT7_IN_SELECT_INPUT          91
#define IMX_IOMUXC_USDHC3_STROBE_SELECT_INPUT           92
#define IMX_IOMUXC_USDHC3_WP_ON_SELECT_INPUT            93

#endif  /* IMX_IOMUX_H_ */
