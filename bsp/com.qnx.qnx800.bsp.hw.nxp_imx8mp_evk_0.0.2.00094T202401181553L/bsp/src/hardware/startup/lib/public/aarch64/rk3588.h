/*
 * Copyright (c) 2022, BlackBerry Limited.
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


/*
 * Rockchip RK3588
 */

#ifndef __AARCH64_RK3588_H_INCLUDED
#define __AARCH64_RK3588_H_INCLUDED

#define RK3588_A76_NUM_CORES                      4
#define RK3588_A55_NUM_CORES                      4
#define RK3588_NUM_CORES                          (RK3588_A76_NUM_CORES + RK3588_A55_NUM_CORES)

/* Set the bits and the write mask */
#define SET_BITS_WM(_shift, _mask, _val)         ( ((_val) << (_shift)) | ((_mask) << ((_shift) + 16)) )

/* GPIO */
#define RK3588_GPIO0_BASE                         0xFD8A0000
#define RK3588_GPIO1_BASE                         0xFEC20000
#define RK3588_GPIO2_BASE                         0xFEC30000
#define RK3588_GPIO3_BASE                         0xFEC40000
#define RK3588_GPIO4_BASE                         0xFEC50000
#define RK3588_GPIO_PORTS_NUM                     5
#define GPIO_SWPORTA_DR_L                         0x0000
#define GPIO_SWPORTA_DR_H                         0x0004
#define GPIO_SWPORTA_DDR_L                        0x0008
#define GPIO_SWPORTA_DDR_H                        0x000C
#define GPIO_INTEN_L                              0x0010
#define GPIO_INTEN_H                              0x0014
#define GPIO_INTMASK_L                            0x0018
#define GPIO_INTMASK_H                            0x001C
#define GPIO_INTTYPE_LEVEL_L                      0x0020
#define GPIO_INTTYPE_LEVEL_H                      0x0024
#define GPIO_INT_POLARITY_L                       0x0028
#define GPIO_INT_POLARITY_H                       0x002C
#define GPIO_INT_BOTHEDGE_L                       0x0030
#define GPIO_INT_BOTHEDGE_H                       0x0034
#define GPIO_DEBOUNCE_L                           0x0038
#define GPIO_DEBOUNCE_H                           0x003C
#define GPIO_DBCLK_DIV_EN_L                       0x0040
#define GPIO_DBCLK_DIV_EN_H                       0x0044
#define GPIO_DBCLK_DIV_CON                        0x0048
#define GPIO_INT_STATUS                           0x0050
#define GPIO_INT_RAWSTATUS                        0x0058
#define GPIO_PORTA_EOI_L                          0x0060
#define GPIO_PORTA_EOI_H                          0x0064
#define GPIO_EXT_PORT                             0x0070
#define GPIO_VER_ID                               0x0078
#define GPIO_GPIO_REG_GROUP_L                     0x0100
#define GPIO_GPIO_REG_GROUP_H                     0x0104
#define GPIO_GPIO_VIRTUAL_EN                      0x0108

#define BUS_IOC                                   0xFD5F8000

#define GPIO_SEL_MASK                             0x0F

#define GPIO2A_IOMUX_SEL_H                        0x44
#define GPIO2A6_SEL_SHIFT                         0x08
#define GPIO2A7_SEL_SHIFT                         0x0C

#define GPIO2B_IOMUX_SEL_L                        0x48
#define GPIO2B0_SEL_SHIFT                         0x00
#define GPIO2B1_SEL_SHIFT                         0x04
#define GPIO2B2_SEL_SHIFT                         0x08
#define GPIO2B3_SEL_SHIFT                         0x0C

#define GPIO2B_IOMUX_SEL_H                        0x4c
#define GPIO2B4_SEL_SHIFT                         0x00
#define GPIO2B5_SEL_SHIFT                         0x04
#define GPIO2B6_SEL_SHIFT                         0x08
#define GPIO2B7_SEL_SHIFT                         0x0C

#define GPIO2C_IOMUX_SEL_L                        0x50
#define GPIO2C0_SEL_SHIFT                         0x00
#define GPIO2C1_SEL_SHIFT                         0x04
#define GPIO2C2_SEL_SHIFT                         0x08
#define GPIO2C3_SEL_SHIFT                         0x0C

#define GPIO4C_IOMUX_SEL_L                        0x90
#define GPIO4C0_SEL_SHIFT                         0x00
#define GPIO4C1_SEL_SHIFT                         0x04
#define GPIO4C2_SEL_SHIFT                         0x08
#define GPIO4C3_SEL_SHIFT                         0x0C

#define GPIO4C_IOMUX_SEL_H                        0x94
#define GPIO4C4_SEL_SHIFT                         0x00
#define GPIO4C5_SEL_SHIFT                         0x04
#define GPIO4C6_SEL_SHIFT                         0x08
#define GPIO4C7_SEL_SHIFT                         0x0C

/* CRU */
#define RK3588_CRU_BASE                           0xFD7C0000
#define RK3588_PHP_CRU_BASE		                  0xFD7C8000
#define RK3588_BIGCORE0_CRU_BASE	              0xFD810000
#define RK3588_BIGCORE1_CRU_BASE	              0xFD812000
#define RK3588_DSU_CRU_BASE		                  0xFD818000

#define RK3588_CRU_V0PLL_CON0                            0x0160
#define RK3588_CRU_V0PLL_CON1                            0x0164
#define RK3588_CRU_V0PLL_CON2                            0x0168
#define RK3588_CRU_V0PLL_CON3                            0x016C
#define RK3588_CRU_V0PLL_CON4                            0x0170
#define RK3588_CRU_V0PLL_CON5                            0x0174
#define RK3588_CRU_V0PLL_CON6                            0x0178
#define MODE_V0PLL_SHIFT                                 0x04

#define RK3588_CRU_AUPLL_CON0                            0x0180
#define RK3588_CRU_AUPLL_CON1                            0x0184
#define RK3588_CRU_AUPLL_CON2                            0x0188
#define RK3588_CRU_AUPLL_CON3                            0x018C
#define RK3588_CRU_AUPLL_CON4                            0x0190
#define RK3588_CRU_AUPLL_CON5                            0x0194
#define RK3588_CRU_AUPLL_CON6                            0x0198
#define MODE_AUPLL_SHIFT                                 0x06

#define RK3588_CRU_CPLL_CON0                             0x01A0
#define RK3588_CRU_CPLL_CON1                             0x01A4
#define RK3588_CRU_CPLL_CON2                             0x01A8
#define RK3588_CRU_CPLL_CON3                             0x01AC
#define RK3588_CRU_CPLL_CON4                             0x01B0
#define RK3588_CRU_CPLL_CON5                             0x01B4
#define RK3588_CRU_CPLL_CON6                             0x01B8
#define MODE_CPLL_SHIFT                                   0x08

#define RK3588_CRU_GPLL_CON0                             0x01C0
#define RK3588_CRU_GPLL_CON1                             0x01C4
#define RK3588_CRU_GPLL_CON2                             0x01C8
#define RK3588_CRU_GPLL_CON3                             0x01CC
#define RK3588_CRU_GPLL_CON4                             0x01D0
#define RK3588_CRU_GPLL_CON5                             0x01D4
#define RK3588_CRU_GPLL_CON6                             0x01D8
#define MODE_GPLL_SHIFT                                  0x02

#define RK3588_CRU_NPLL_CON0                             0x01E0
#define RK3588_CRU_NPLL_CON1                             0x01E4
#define RK3588_CRU_NPLL_CON4                             0x01F0
#define RK3588_CRU_NPLL_CON5                             0x01F4
#define RK3588_CRU_NPLL_CON6                             0x01F8
#define MODE_NPLL_SHIFT                                  0x00

#define BIGCORE0CRU_B0PLL_CON0                    0x0000
#define BIGCORE0CRU_B0PLL_CON1                    0x0004
#define BIGCORE0CRU_B0PLL_CON4                    0x0010
#define BIGCORE0CRU_B0PLL_CON5                    0x0014
#define BIGCORE0CRU_B0PLL_CON6                    0x0018
#define MODE_B0PLL_SHIFT                          0x00

#define BIGCORE1CRU_B1PLL_CON0                    0x0020
#define BIGCORE1CRU_B1PLL_CON1                    0x0024
#define BIGCORE1CRU_B1PLL_CON4                    0x0030
#define BIGCORE1CRU_B1PLL_CON5                    0x0034
#define BIGCORE1CRU_B1PLL_CON6                    0x0038
#define MODE_B1PLL_SHIFT                          0x00

#define PHPTOPCRU_PPLL_CON0                       0x0200
#define PHPTOPCRU_PPLL_CON1                       0x0204
#define PHPTOPCRU_PPLL_CON2                       0x0208
#define PHPTOPCRU_PPLL_CON3                       0x020C
#define PHPTOPCRU_PPLL_CON4                       0x0210
#define PHPTOPCRU_PPLL_CON5                       0x0214
#define PHPTOPCRU_PPLL_CON6                       0x0218
#define MODE_PPLL_SHIFT                           0x00

#define DSUCRU_LPLL_CON0                          0x0040
#define DSUCRU_LPLL_CON1                          0x0044
#define DSUCRU_LPLL_CON4                          0x0050
#define DSUCRU_LPLL_CON5                          0x0054
#define DSUCRU_LPLL_CON6                          0x0058
#define MODE_LPLL_SHIFT                           0x00

#define PLL_CON0_M_SHIFT                          0U
#define PLL_CON0_M_MASK                           0x3FFU

#define PLL_CON0_BYPASS_SHIFT                     15U
#define PLL_CON0_BYPASS_MASK                      0x01U

#define PLL_CON1_RESETB                           (1 << 13)
#define PLL_CON1_RESETB_SHIFT                     13U
#define PLL_CON1_RESETB_MASK                      1U
#define PLL_CON1_P_SHIFT                          0U
#define PLL_CON1_P_MASK                           0x3FU
#define PLL_CON1_S_SHIFT                          6U
#define PLL_CON1_S_MASK                           0x7U

#define PLL_CON2_K_SHIFT                          0U
#define PLL_CON2_K_MASK                           0xFFFFU

#define PLL_CON6_PLL_LOCKED                       (1<<15)

#define RK3588_MODE_CON0		                  0x280
#define RK3588_CLKSEL_CON(x)		              ((x) * 0x4 + 0x300)
#define RK3588_CLKGATE_CON(x)		              ((x) * 0x4 + 0x800)
#define RK3588_SOFTRST_CON(x)		              ((x) * 0x4 + 0xa00)

#define MODE_CON0_MASK                            0x3U
#define MODE_CON0_SLOW                            0x0U
#define MODE_CON0_NORMAL                          0x1U
#define MODE_CON0_DEEP_SLOW                       0x2U

// TODO: change this to proper ID
#define RK3588_INTR_PCIE_LEGACY                 24

#define PHP_GRF                                   0xFD5B0000

#define PHP_GRF_GMAC_CON0                         0x08
#define GMAC0_PHY_INTF_SEL_MASK                   0x7
#define GMAC0_PHY_INTF_SEL_SHIFT                  3

#define PHP_GRF_CLK_CON1                          0x70
#define IO_CLKSEL_GMAC0_MASK                      1
#define IO_CLKSEL_GMAC0_SHIFT                     4
#define RMII_MODE_GMAC0_MASK                      1
#define RMII_MODE_GMAC0_SHIFT                     0

#define SYS_GRF                                   0xFD58C000

#define SYS_GRF_SOC_CON7                          0x31c
#define GMAC0_TXCLK_DLY_ENA_MASK                  0x1
#define GMAC0_TXCLK_DLY_ENA_SHIFT                 0x2

#define SYS_GRF_SOC_CON8                          0x320
#define GMAC0_CLK_TX_DL_CFG_MASK                  0xFF
#define GMAC0_CLK_TX_DL_CFG_SHIFT                 0x0

#endif /* __AARCH64_RK3588_H_INCLUDED */
