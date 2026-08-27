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

#ifndef IMX_HSIO_H_
#define IMX_HSIO_H_

/** Clock select reset and debug info select */
#define IMX_HSIO_GPR_REG0                                       0x00
#define IMX_HSIO_GPR_REG0_PCIE_CLOCK_MODULE_EN_MASK             (0x01UL << 0)
#define IMX_HSIO_GPR_REG0_PCIE_CLOCK_MODULE_EN_SHIFT            0
#define IMX_HSIO_GPR_REG0_USB_CLOCK_MODULE_EN_MASK              (0x01UL << 1)
#define IMX_HSIO_GPR_REG0_USB_CLOCK_MODULE_EN_SHIFT             1
#define IMX_HSIO_GPR_REG0_PHY_APB_RST_MASK                      (0x01UL << 4)
#define IMX_HSIO_GPR_REG0_PHY_APB_RST_SHIFT                     4
#define IMX_HSIO_GPR_REG0_PHY_INIT_RST_MASK                     (0x01UL << 5)
#define IMX_HSIO_GPR_REG0_PHY_INIT_RST_SHIFT                    5

/** PCIE controller status */
#define IMX_HSIO_GPR_REG1                                       0x04

#define IMX_HSIO_GPR_REG1_PLL_LOCK_MASK                         (0x01UL << 13)
#define IMX_HSIO_GPR_REG1_PLL_LOCK_SHIFT                        13
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_DSTATE_MASK              (0x07UL << 10)
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_DSTATE_SHIFT             10
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L0S_MASK       (0x01UL << 9)
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L0S_SHIFT      9
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L1_MASK        (0x01UL << 8)
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L1_SHIFT       8
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L1SUB_MASK     (0x01UL << 7)
#define IMX_HSIO_GPR_REG1_PCIE_CTRL_PM_LINKST_IN_L1SUB_SHIFT    7
#define IMX_HSIO_GPR_REG1_SMLH_LTSSM_STATE_MASK                 (0x3FUL << 1)
#define IMX_HSIO_GPR_REG1_SMLH_LTSSM_STATE_SHIFT                1
#define IMX_HSIO_GPR_REG1_PM_EN_CORE_CLK_MASK                   (0x01UL << 0)
#define IMX_HSIO_GPR_REG1_PM_EN_CORE_CLK_SHIFT                  0

/** PLL configuration 0 */
#define IMX_HSIO_GPR_REG2                                       0x08

#define IMX_HSIO_GPR_REG2_S_PLL_MASK                            (0x07UL << 16)
#define IMX_HSIO_GPR_REG2_S_PLL_SHIFT                           16
#define IMX_HSIO_GPR_REG2_M_PLL_MASK                            (0x3FFUL << 6)
#define IMX_HSIO_GPR_REG2_M_PLL_SHIFT                           6
#define IMX_HSIO_GPR_REG2_P_PLL_MASK                            (0x3FUL << 0)
#define IMX_HSIO_GPR_REG2_P_PLL_SHIFT                           0


/** PLL configuration 1 */
#define IMX_HSIO_GPR_REG3                                       0x0C

#define IMX_HSIO_GPR_REG3_PLL_RESETB_MASK                       (0x01UL << 31)
#define IMX_HSIO_GPR_REG3_PLL_RESETB_SHIFT                      31
#define IMX_HSIO_GPR_REG3_PLL_EXT_BYPASS_MASK                   (0x01UL << 18)
#define IMX_HSIO_GPR_REG3_PLL_EXT_BYPASS_SHIFT                  18
#define IMX_HSIO_GPR_REG3_PLL_CKE_MASK                          (0x01UL << 17)
#define IMX_HSIO_GPR_REG3_PLL_CKE_SHIFT                         17
#define IMX_HSIO_GPR_REG3_RSEL_PLL_MASK                         (0x0FUL << 13)
#define IMX_HSIO_GPR_REG3_RSEL_PLL_SHIFT                        13
#define IMX_HSIO_GPR_REG3_LRD_EN_PLL_MASK                       (0x01UL << 12)
#define IMX_HSIO_GPR_REG3_LRD_EN_PLL_SHIFT                      12
#define IMX_HSIO_GPR_REG3_PBIAS_CTRL_PLL_MASK                   (0x01UL << 11)
#define IMX_HSIO_GPR_REG3_PBIAS_CTRL_PLL_SHIFT                  11
#define IMX_HSIO_GPR_REG3_PBIAS_CTRL_EN_PLL_MASK                (0x01UL << 10)
#define IMX_HSIO_GPR_REG3_PBIAS_CTRL_EN_PLL_SHIFT               10
#define IMX_HSIO_GPR_REG3_VCO_BOOST_PLL_MASK                    (0x01UL << 9)
#define IMX_HSIO_GPR_REG3_VCO_BOOST_PLL_SHIFT                   9
#define IMX_HSIO_GPR_REG3_FOUT_MASK_PLL_MASK                    (0x01UL << 8)
#define IMX_HSIO_GPR_REG3_FOUT_MASK_PLL_SHIFT                   8
#define IMX_HSIO_GPR_REG3_AFCINIT_SEL_PLL_MASK                  (0x01UL << 7)
#define IMX_HSIO_GPR_REG3_AFCINIT_SEL_PLL_SHIFT                 7
#define IMX_HSIO_GPR_REG3_FSEL_PLL_MASK                         (0x01UL << 6)
#define IMX_HSIO_GPR_REG3_FSEL_PLL_SHIFT                        6
#define IMX_HSIO_GPR_REG3_FEED_EN_PLL_MASK                      (0x01UL << 5)
#define IMX_HSIO_GPR_REG3_FEED_EN_PLL_SHIFT                     5
#define IMX_HSIO_GPR_REG3_EXTAFC_PLL_MASK                       (0x1FUL << 0)
#define IMX_HSIO_GPR_REG3_EXTAFC_PLL_SHIFT                      0


/** PCIE PME message and error detect register */
#define IMX_HSIO_GPR_REG4                                       0x10

/** PCIE PME message and error detect interrupt enable register */
#define IMX_HSIO_GPR_REG5                                       0x14

/** PCIE PME message and error detect interrupt detect disable register */
#define IMX_HSIO_GPR_REG6                                       0x18
/** USB1 beat limit and enable */
#define IMX_HSIO_GPR_REG7                                       0x1C
/** USB2 beat limit and enable */
#define IMX_HSIO_GPR_REG8                                       0x20
/** PCIE beat limit and enable */
#define IMX_HSIO_GPR_REG9                                       0x24
/** Register for USB1 wakeup */
#define IMX_HSIO_USB1_WAKEUP_CTRL                               0x100
/** Status of USB1 wakeup */
#define IMX_HSIO_USB1_WAKEUP_STATUS                             0x104
/** Register for USB2 wakeup */
#define IMX_HSIO_USB2_WAKEUP_CTRL                               0x108
/** Status of USB2 wakeup */
#define IMX_HSIO_USB2_WAKEUP_STATUS                             0x104

#endif /* IMX_HSIO_H_ */
