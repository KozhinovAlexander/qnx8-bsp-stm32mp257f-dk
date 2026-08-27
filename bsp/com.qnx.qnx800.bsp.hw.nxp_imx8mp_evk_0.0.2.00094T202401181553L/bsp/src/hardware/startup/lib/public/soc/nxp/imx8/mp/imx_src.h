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

#ifndef IMX_SRC_H_
#define IMX_SRC_H_

/* SRC registers, offset from base address */
#define IMX_SRC_SCR                        0x00     /* SRC Reset Control Register */
#define IMX_SRC_A53RCR0                    0x04     /* A53 Reset Control Register */
#define IMX_SRC_A53RCR1                    0x08     /* A53 Reset Control Register */
#define IMX_SRC_M7RCR                      0x0C     /* M7 Reset Control Register */
#define IMX_SRC_SUPERMIX_RCR               0x18     /* SUPERMIX Reset Control Register */
#define IMX_SRC_AUDIOMIX_RCR               0x1C     /* AUDIOMIX Reset Control Register */
#define IMX_SRC_USBOPHY1_RCR               0x20     /* USB OTG PHY1 Reset Control Register */
#define IMX_SRC_USBOPHY2_RCR               0x24     /* USB OTG PHY2 Reset Control Register */
#define IMX_SRC_SRC_MLMIX_RCR              0x28     /* MLMIX Reset Control Register */
#define IMX_SRC_PCIEPHY_RCR                0x2C     /* PCIE PHY Reset Control Register */
#define IMX_SRC_HDMI_RCR                   0x30     /* HDMI Reset Control Register */
#define IMX_SRC_SRC_MEDIA_RCR              0x34     /* MEDIAMIX Reset Control Register */
#define IMX_SRC_GPU2D_RCR                  0x38     /* GPU2D Reset Control Register */
#define IMX_SRC_GPU3D_RCR                  0x3C     /* GPU3D Reset Control Register */
#define IMX_SRC_GPU_RCR                    0x40     /* GPU Reset Control Register */
#define IMX_SRC_VPU_RCR                    0x44     /* VPU Reset Control Register */
#define IMX_SRC_VPU_G1_RCR                 0x48     /* VPU G1 Reset Control Register */
#define IMX_SRC_VPU_G2_RCR                 0x4C     /* VPU G2 Reset Control Register */
#define IMX_SRC_VPUVC8KE_RCR               0x50     /* VPU VC8000E Reset Control Register */
#define IMX_SRC_NOC_RCR                    0x54     /* NOC Wrapper Reset Control Register */
#define IMX_SRC_SBMR1                      0x58     /* SRC Boot Mode Register 1 */
#define IMX_SRC_SRSR                       0x5C     /* SRC Reset Status Register */
#define IMX_SRC_SISR                       0x68     /* SRC Interrupt Status Register */
#define IMX_SRC_SIMR                       0x6C     /* SRC Interrupt Mask Register */
#define IMX_SRC_SBMR2                      0x70     /* SRC Boot Mode Register 2 */

#define IMX_SRC_GPR1                       0x74     /* SRC General Purpose Register 1 */
#define IMX_SRC_GPR2                       0x78     /* SRC General Purpose Register 2 */
#define IMX_SRC_GPR3                       0x7C     /* SRC General Purpose Register 3 */
#define IMX_SRC_GPR4                       0x80     /* SRC General Purpose Register 4 */
#define IMX_SRC_GPR5                       0x84     /* SRC General Purpose Register 5 */
#define IMX_SRC_GPR6                       0x88     /* SRC General Purpose Register 6 */
#define IMX_SRC_GPR7                       0x8C     /* SRC General Purpose Register 7 */
#define IMX_SRC_GPR8                       0x90     /* SRC General Purpose Register 8 */
#define IMX_SRC_GPR9                       0x94     /* SRC General Purpose Register 9 */
#define IMX_SRC_GPR10                      0x98     /* SRC General Purpose Register 10 */
#define IMX_SRC_DDRC_RCR                   0x1000   /* SRC DDR Controller Reset Control Register */
#define IMX_SRC_SRC_HDMIPHY_RCR            0x1008   /* HDMIPHY Reset Control Register */
#define IMX_SRC_SRC_MIPIPHY1_RCR           0x100C   /* MIPI PHY1 Reset Control Register */
#define IMX_SRC_SRC_MIPIPHY2_RCR           0x1010   /* MIPI PHY2 Reset Control Register */
#define IMX_SRC_SRC_HSIO_RCR               0x1014   /* HSIO Reset Control Register */
#define IMX_SRC_SRC_MEDIAISPDWP_RCR        0x1018   /* MEDIAMIX ISP and Dewarp Reset Control Register */

/*
 *  PCIE PHY Reset Control Register bits
 */
#define IMX_PCIEPHY_RCR_DOM_EN_MASK                      (0x01 << 31)  /* Domain Control enable for this register */
#define IMX_PCIEPHY_RCR_DOM_EN_SHIFT                     31
#define IMX_PCIEPHY_RCR_LOCK_MASK                        (0x01 << 30)  /* Domain control bits lock */
#define IMX_PCIEPHY_RCR_LOCK_SHIFT                       30
#define IMX_PCIEPHY_RCR_DOMAIN3_MASK                     (0x01 << 27)  /* Domain3 assignment control */
#define IMX_PCIEPHY_RCR_DOMAIN3_SHIFT                    27
#define IMX_PCIEPHY_RCR_DOMAIN2_MASK                     (0x01 << 26)  /* Domain2 assignment control */
#define IMX_PCIEPHY_RCR_DOMAIN2_SHIFT                    26
#define IMX_PCIEPHY_RCR_DOMAIN1_MASK                     (0x01 << 25)  /* Domain1 assignment control */
#define IMX_PCIEPHY_RCR_DOMAIN1_SHIFT                    25
#define IMX_PCIEPHY_RCR_DOMAIN0_MASK                     (0x01 << 24)  /* Domain0 assignment control */
#define IMX_PCIEPHY_RCR_DOMAIN0_SHIFT                    24
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APP_XFER_PENDING_MASK  (0x01 << 16)  /* PCIE_CTRL_APP_XFER_PENDING */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APP_XFER_PENDING_SHIFT 16
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APP_UNLOCK_MSG_MASK    (0x01 << 15)  /* PCIE_CTRL_APP_UNLOCK_MSG */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APP_UNLOCK_MSG_SHIFT   15
#define IMX_PCIEPHY_RCR_PCIE_CTRL_SYS_INT_MASK           (0x01 << 14)  /* PCIE_CTRL_SYS_INT */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_SYS_INT_SHIFT          14
#define IMX_PCIEPHY_RCR_PCIE_CTRL_CFG_L1_AUX_MASK        (0x01 << 12)  /* PCIE_CTRL_CFG_L1_AUX */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_CFG_L1_AUX_SHIFT       12
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_TURNOFF_MASK      (0x01 << 11)  /* Pcie_ctrl_apps_pm_xmt_turnoff */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_TURNOFF_SHIFT     11
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_PME_MASK          (0x01 << 10)  /* Pcie_ctrl_apps_pm_xmt_pme */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_PME_SHIFT         10
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_EXIT_MASK         (0x01 << 9)   /* Pcie_ctrl_app_req_exit_l1 */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_EXIT_SHIFT        9
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_ENTER_MASK        (0x01 << 8)   /* Pcie_ctrl_app_req_entr_l1 */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_ENTER_SHIFT       8
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_READY_MASK        (0x01 << 7)   /* Pcie_ctrl_app_ready_entr_l23 */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_READY_SHIFT       7
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_EN_MASK           (0x01 << 6)   /* Pcie_ctrl_app_ltssm_enable */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_EN_SHIFT          6
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_RST_MASK          (0x01 << 5)   /* Pcie_ctrl_app_init_rst */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_RST_SHIFT         5
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_CLK_REQ_MASK      (0x01 << 4)   /* Pcie_ctrl_app_clk_req_n */
#define IMX_PCIEPHY_RCR_PCIE_CTRL_APPS_CLK_REQ_SHIFT     4
#define IMX_PCIEPHY_RCR_PCIEPHY_PERST_MASK               (0x01 << 3)   /* Pciephy_perst */
#define IMX_PCIEPHY_RCR_PCIEPHY_PERST_SHIFT              3
#define IMX_PCIEPHY_RCR_PCIEPHY_BTNRST_MASK              (0x01 << 2)   /* PCIE PHY button */
#define IMX_PCIEPHY_RCR_PCIEPHY_BTNRST_SHIFT             2
#define IMX_PCIEPHY_RCR_PCIE_PHY_POWER_ON_RESET_MASK     (0x01 << 0)   /* PCIE_PHY_POWER_ON_RESET */
#define IMX_PCIEPHY_RCR_PCIE_PHY_POWER_ON_RESET_SHIFT    0

#endif
