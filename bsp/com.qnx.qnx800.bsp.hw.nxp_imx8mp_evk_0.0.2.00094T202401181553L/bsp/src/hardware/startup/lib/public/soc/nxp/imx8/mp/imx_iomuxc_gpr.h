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

#ifndef IMX_IOMUXC_GPR_H_
#define IMX_IOMUXC_GPR_H_

/* IOMUXC_GPR registers, offset from base address */
#define IMX_IOMUXC_GPR0                             0x00     /* General Purpose Register 0 */
#define IMX_IOMUXC_GPR1                             0x04     /* General Purpose Register 1 */
#define IMX_IOMUXC_GPR2                             0x08     /* General Purpose Register 2 */
#define IMX_IOMUXC_GPR3                             0x0C     /* General Purpose Register 3 */
#define IMX_IOMUXC_GPR4                             0x10     /* General Purpose Register 4 */
#define IMX_IOMUXC_GPR5                             0x14     /* General Purpose Register 5 */
#define IMX_IOMUXC_GPR6                             0x18     /* General Purpose Register 6 */
#define IMX_IOMUXC_GPR7                             0x1C     /* General Purpose Register 7 */
#define IMX_IOMUXC_GPR8                             0x20     /* General Purpose Register 8 */
#define IMX_IOMUXC_GPR9                             0x24     /* General Purpose Register 9 */
#define IMX_IOMUXC_GPR10                            0x28     /* General Purpose Register 10 */
#define IMX_IOMUXC_GPR11                            0x2C     /* General Purpose Register 11 */
#define IMX_IOMUXC_GPR12                            0x30     /* General Purpose Register 12 */
#define IMX_IOMUXC_GPR13                            0x34     /* General Purpose Register 13 */
#define IMX_IOMUXC_GPR14                            0x38     /* General Purpose Register 14 */
#define IMX_IOMUXC_GPR15                            0x3C     /* General Purpose Register 15 */
#define IMX_IOMUXC_GPR16                            0x40     /* General Purpose Register 16 */
#define IMX_IOMUXC_GPR17                            0x44     /* General Purpose Register 17 */
#define IMX_IOMUXC_GPR18                            0x48     /* General Purpose Register 18 */
#define IMX_IOMUXC_GPR19                            0x4C     /* General Purpose Register 19 */
#define IMX_IOMUXC_GPR20                            0x50     /* General Purpose Register 20 */
#define IMX_IOMUXC_GPR21                            0x54     /* General Purpose Register 21 */
#define IMX_IOMUXC_GPR22                            0x58     /* General Purpose Register 22 */
#define IMX_IOMUXC_GPR23                            0x5C     /* General Purpose Register 23 */
#define IMX_IOMUXC_GPR24                            0x60     /* General Purpose Register 24 */
#define IMX_IOMUXC_GPR(n)                           (4 * n)  /* General Purpose Register 0 - 24 */

/*
 *  General Purpose Register 1 bits
 */
#define IMX_IOMUXC_GPR_IOMUXC_DBG_ACK_A53_MASK                  (0x01UL << 28) /* Mask debug ack from each CA53 core */
#define IMX_IOMUXC_GPR_IOMUXC_DBG_ACK_A53_SHIFT                 28
#define IMX_IOMUXC_GPR_IOMUXC_ACK_M7_MASK                       (0x01UL << 27) /* Mask debug ack from each CM7 */
#define IMX_IOMUXC_GPR_IOMUXC_ACK_M7_SHIFT                      27
#define IMX_IOMUXC_GPR_IOMUXC_TZASC1_SECURE_BOOT_LOCK_MASK      (0x01UL << 23) /* secure_boot_lock for TZASC */
#define IMX_IOMUXC_GPR_IOMUXC_TZASC1_SECURE_BOOT_LOCK_SHIFT     23
#define IMX_IOMUXC_GPR_IOMUXC_GPR_ENET1_RGMII_EN_MASK           (0x01UL << 22) /* ENET1 TX clock direction select for RGMII or MII */
#define IMX_IOMUXC_GPR_IOMUXC_GPR_ENET1_RGMII_EN_SHIFT          22
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_RGMII_EN_MASK            (0x01UL << 21) /* ENET QOS TX clock direction select for RGMII or MII */
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_RGMII_EN_SHIFT           21
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_TX_CLK_SEL_MASK      (0x01UL << 20) /* SOI bit for the pad */
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_TX_CLK_SEL_SHIFT     20
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_GEN_EN_MASK          (0x01UL << 19) /* Enable clk generate module for ENET QoS */
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_CLK_GEN_EN_SHIFT         19
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_INTF_SEL_MASK            (0x07UL << 16) /* Select ENET QOS working mode */
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_INTF_SEL_SHIFT           16
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_INTF_SEL(n)              ((n) << 16)
#define IMX_IOMUXC_GPR_IOMUXC_ANAMIX_IPT_MODE_MASK              (0x01UL << 15) /* Mask ANAMIX ipt_mode */
#define IMX_IOMUXC_GPR_IOMUXC_ANAMIX_IPT_MODE_SHIFT             15
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_DIS_CRC_CHK_MASK         (0x01UL << 14) /* Disable CRC check feature */
#define IMX_IOMUXC_GPR_IOMUXC_ENET_QOS_DIS_CRC_CHK_SHIFT        14
#define IMX_IOMUXC_GPR_IOMUXC_ENET1_TX_CLK_SEL_MASK             (0x01UL << 13) /* SOI bit for the pad */
#define IMX_IOMUXC_GPR_IOMUXC_ENET1_TX_CLK_SEL_SHIFT            13
#define IMX_IOMUXC_GPR_IOMUXC_IRQ_MASK                          (0x01UL << 12) /* Generate IRQ on IRQ0 */
#define IMX_IOMUXC_GPR_IOMUXC_IRQ_SHIFT                         12
#define IMX_IOMUXC_GPR_IOMUXC_GPT1_CAPIN2_SEL_MASK              (0x01UL << 3)  /* Selector for GPT1 Capture in Channel 2 */
#define IMX_IOMUXC_GPR_IOMUXC_GPT1_CAPIN2_SEL_SHIFT             3
#define IMX_IOMUXC_GPR_IOMUXC_GPT1_CAPIN1_SEL_MASK              (0x01UL << 2)  /* Selector for GPT1 Capture in Channel 1 */
#define IMX_IOMUXC_GPR_IOMUXC_GPT1_CAPIN1_SEL_SHIFT             2
#define IMX_IOMUXC_GPR_IOMUXC_GPR_ENET_QOS_EVENT0IN_SEL_MASK    (0x01UL << 1)  /* Selector for ENET QoS EVENT0 IN */
#define IMX_IOMUXC_GPR_IOMUXC_GPR_ENET_QOS_EVENT0IN_SEL_SHIFT   1
#define IMX_IOMUXC_GPR_IOMUXC_ENET1_EVENT0IN_SEL_MASK           (0x01UL << 0)  /* Selector for ENET1 EVENT0 IN */
#define IMX_IOMUXC_GPR_IOMUXC_ENET1_EVENT0IN_SEL_SHIFT          0

/*
 *  General Purpose Register 14 bits
 */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_PLL_REF_CLK_SEL_MASK    (0x03UL << 24) /* Selects reference clock */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_PLL_REF_CLK_SEL_SHIFT   24
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_AUX_EN_MASK             (0x01UL << 19) /* External Reference Clock I/O (for PLL) Enable Signal */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_AUX_EN_SHIFT            19
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_CMN_RSTN_MASK           (0x01UL << 18) /* Resets the PCIe PHY Common Block Reset */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_CMN_RSTN_SHIFT          18
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_POWER_OFF_MASK          (0x01UL << 17) /* PMA power down signal */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_POWER_OFF_SHIFT         17
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_SSC_EN_MASK             (0x01UL << 16) /* SSC enable signal */
#define IMX_IOMUXC_GPR_PCIE1_PHY_FUNC_I_SSC_EN_SHIFT            16
#define IMX_IOMUXC_GPR_PCIE1_CLKREQ_B_OVERRIDE_MASK             (0x01UL << 11) /* Control PCIE_CLKREQ_B to the pad together with CLKREQ_B from controller */
#define IMX_IOMUXC_GPR_PCIE1_CLKREQ_B_OVERRIDE_SHIFT            11
#define IMX_IOMUXC_GPR_PCIE1_CLKREQ_B_OVERRIDE_EN_MASK          (0x01UL << 10) /* Control PCIE_CLKREQ_B to the pad together with CLKREQ_B from controller */
#define IMX_IOMUXC_GPR_PCIE1_CLKREQ_B_OVERRIDE_EN_SHIFT         10
#define IMX_IOMUXC_GPR_PCIE1_PHY_I_AUX_EN_OVERRIDE_EN_MASK      (0x01UL << 9)  /* Enable/disable external ref. clock I/O */
#define IMX_IOMUXC_GPR_PCIE1_PHY_I_AUX_EN_OVERRIDE_EN_SHIFT     9
#define IMX_IOMUXC_GPR_PCIE1_APP_CLK_PM_EN_MASK                 (0x01UL << 8)  /* To PCIe CTRL. */
#define IMX_IOMUXC_GPR_PCIE1_APP_CLK_PM_EN_SHIFT                8

#endif
