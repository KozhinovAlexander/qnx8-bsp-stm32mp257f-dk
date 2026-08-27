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

#ifndef IMX_CCM_H_
#define IMX_CCM_H_

/* CCM Registers, offset from base address */
#define IMX_CCM_GPR0n(n)                        (0x000 + (4 * (n)))   /* General Purpose Register */
#define IMX_CCM_PLL_CTRLn(n)                    (0x800 + (16 * (n)))  /* PLL Control Register */
#define IMX_CCM_PLL_CTRLn_SET(n)                (0x804 + (16 * (n)))  /* PLL Control Register, Set */
#define IMX_CCM_PLL_CTRLn_CLR(n)                (0x808 + (16 * (n)))  /* PLL Control Register, Clear */
#define IMX_CCM_PLL_CTRLn_TOG(n)                (0x80C + (16 * (n)))  /* PLL Control Register, Toggle */
#define IMX_CCM_CCGRn(n)                        (0x4000 + (16 * (n)))  /* Clock Gating Register */
#define IMX_CCM_CCGRn_SET(n)                    (0x4004 + (16 * (n)))  /* Clock Gating Register, Set */
#define IMX_CCM_CCGRn_CLR(n)                    (0x4008 + (16 * (n)))  /* Clock Gating Register, Clear */
#define IMX_CCM_CCGRn_TOG(n)                    (0x400C + (16 * (n)))  /* Clock Gating Register, Toggle */
#define IMX_CCM_TARGET_ROOTn(n)                 (0x8000 + (128 * (n)))  /* Target Register */
#define IMX_CCM_TARGET_ROOTn_SET(n)             (0x8004 + (128 * (n)))  /* Target Register, Set */
#define IMX_CCM_TARGET_ROOTn_CLR(n)             (0x8008 + (128 * (n)))  /* Target Register, Clear */
#define IMX_CCM_TARGET_ROOTn_TOG(n)             (0x800C + (128 * (n)))  /* Target Register, Toggle */
#define IMX_CCM_MISCn(n)                        (0x8010 + (128 * (n)))  /* Miscellaneous Register */
#define IMX_CCM_MISCn_ROOT_SET(n)               (0x8014 + (128 * (n)))  /* Miscellaneous Register, Set */
#define IMX_CCM_MISCn_ROOT_CLR(n)               (0x8018 + (128 * (n)))  /* Miscellaneous Register, Clear */
#define IMX_CCM_MISCn_ROOT_TOG(n)               (0x801C + (128 * (n)))  /* Miscellaneous Register, Toggle */
#define IMX_CCM_POSTn(n)                        (0x8020 + (128 * (n)))  /* Post Divider Register */
#define IMX_CCM_POST_ROOTn_SET(n)               (0x8024 + (128 * (n)))  /* Post Divider Register, Set */
#define IMX_CCM_POST_ROOTn_CLR(n)               (0x8028 + (128 * (n)))  /* Post Divider Register, Clear */
#define IMX_CCM_POST_ROOTn_TOG(n)               (0x802C + (128 * (n)))  /* Post Divider Register, Toggle */
#define IMX_CCM_PREn(n)                         (0x8030 + (128 * (n)))  /* Pre Divider Register */
#define IMX_CCM_PRE_ROOTn_SET(n)                (0x8034 + (128 * (n)))  /* Pre Divider Register, Set */
#define IMX_CCM_PRE_ROOTn_CLR(n)                (0x8038 + (128 * (n)))  /* Pre Divider Register, Clear */
#define IMX_CCM_PRE_ROOTn_TOG(n)                (0x803C + (128 * (n)))  /* Pre Divider Register, Toggle */
#define IMX_CCM_ACCESS_CTRLn(n)                 (0x8070 + (128 * (n)))  /* Access Control Register */
#define IMX_CCM_ACCESS_CTRL_ROOTn_SET(n)        (0x8074 + (128 * (n)))  /* Access Control Register, Set */
#define IMX_CCM_ACCESS_CTRL_ROOTn_CLR(n)        (0x8078 + (128 * (n)))  /* Access Control Register, Clear */
#define IMX_CCM_ACCESS_CTRL_ROOTn_TOG(n)        (0x807C + (128 * (n)))  /* Access Control Register, Toggle */

/*
 * CCM Control Divider Register bits *
 */
#define IMX_CCM_TARGET_ROOT_ENABLE_MASK         (0x1 << 28)           /* Enable this clock */
#define IMX_CCM_TARGET_ROOT_ENABLE_SHIFT        28
#define IMX_CCM_TARGET_ROOT_MUX_MASK            (0x07 << 24)          /* Selection of clock sources */
#define IMX_CCM_TARGET_ROOT_MUX_SHIFT           24
#define IMX_CCM_TARGET_ROOT_MUX_VALUE(x)        (((x) & 0x07) << 24)
#define IMX_CCM_TARGET_ROOT_PRE_PODF_MASK       (0x07 << 16)          /* Pre divider divide the number */
#define IMX_CCM_TARGET_ROOT_PRE_PODF_SHIFT      16
#define IMX_CCM_TARGET_ROOT_PRE_PODF(n)         (((n) - 1) << 16)
#define IMX_CCM_TARGET_ROOT_POST_PODF_MASK      (0x3F << 0)           /* Pre divider divide the number */
#define IMX_CCM_TARGET_ROOT_POST_PODF_SHIFT     0
#define IMX_CCM_TARGET_ROOT_POST_PODF(n)        (((n) - 1) << 0)
/* Clock Root Selects for clock root offsets and muxing */
#define IMX_CCM_TARGET_ARM_A53                  0U
#define IMX_CCM_TARGET_ARM_M7                   1U
#define IMX_CCM_TARGET_ML                       2U
#define IMX_CCM_TARGET_GPU3D_CORE               3U
#define IMX_CCM_TARGET_GPU3D_SHADER             4U
#define IMX_CCM_TARGET_GPU2D                    5U
#define IMX_CCM_TARGET_AUDIO_AXI                6U
#define IMX_CCM_TARGET_HSIO_AXI                 7U
#define IMX_CCM_TARGET_MEDIA_ISP                8U
#define IMX_CCM_TARGET_MAIN_AXI                 16U
#define IMX_CCM_TARGET_ENET_AXI                 17U
#define IMX_CCM_TARGET_NAND_USDHC_BUS           18U
#define IMX_CCM_TARGET_VPU_BUS                  19U
#define IMX_CCM_TARGET_MEDIA_AXI                20U
#define IMX_CCM_TARGET_MEDIA_APB                21U
#define IMX_CCM_TARGET_HDMI_APB                 22U
#define IMX_CCM_TARGET_HDMI_AXI                 23U
#define IMX_CCM_TARGET_GPU_AXI                  24U
#define IMX_CCM_TARGET_GPU_AHB                  25U
#define IMX_CCM_TARGET_NOC                      26U
#define IMX_CCM_TARGET_NOC_IO                   27U
#define IMX_CCM_TARGET_ML_AXI                   28U
#define IMX_CCM_TARGET_ML_AHB                   29U
#define IMX_CCM_TARGET_AHB                      32U
#define IMX_CCM_TARGET_IPG                      33U
#define IMX_CCM_TARGET_AUDIO_AHB                34U
#define IMX_CCM_TARGET_MEDIA_DISP2              38U
#define IMX_CCM_TARGET_DRAM                     48U
#define IMX_CCM_TARGET_CORE_SEL_CFG             49U
#define IMX_CCM_TARGET_DRAM_ALT                 64U
#define IMX_CCM_TARGET_DRAM_APB                 65U
#define IMX_CCM_TARGET_VPU_G1                   66U
#define IMX_CCM_TARGET_VPU_G2                   67U
#define IMX_CCM_TARGET_CAN1                     68U
#define IMX_CCM_TARGET_CAN2                     69U
#define IMX_CCM_TARGET_MEMREPAIR                70U
#define IMX_CCM_TARGET_PCIE_PHY                 71U
#define IMX_CCM_TARGET_PCIE_AUX                 72U
#define IMX_CCM_TARGET_I2C5                     73U
#define IMX_CCM_TARGET_I2C6                     74U
#define IMX_CCM_TARGET_SAI1                     75U
#define IMX_CCM_TARGET_SAI2                     76U
#define IMX_CCM_TARGET_SAI3                     77U
#define IMX_CCM_TARGET_SAI5                     79U
#define IMX_CCM_TARGET_SAI6                     80U
#define IMX_CCM_TARGET_ENET_QOS                 81U
#define IMX_CCM_TARGET_ENET_QOS_TIMER           82U
#define IMX_CCM_TARGET_ENET_REF                 83U
#define IMX_CCM_TARGET_ENET_TIMER               84U
#define IMX_CCM_TARGET_ENET_PHY_REF             85U
#define IMX_CCM_TARGET_NAND                     86U
#define IMX_CCM_TARGET_QSPI                     87U
#define IMX_CCM_TARGET_USDHC1                   88U
#define IMX_CCM_TARGET_USDHC2                   89U
#define IMX_CCM_TARGET_I2C1                     90U
#define IMX_CCM_TARGET_I2C2                     91U
#define IMX_CCM_TARGET_I2C3                     92U
#define IMX_CCM_TARGET_I2C4                     93U
#define IMX_CCM_TARGET_UART1                    94U
#define IMX_CCM_TARGET_UART2                    95U
#define IMX_CCM_TARGET_UART3                    96U
#define IMX_CCM_TARGET_UART4                    97U
#define IMX_CCM_TARGET_USB_CORE_REF             98U
#define IMX_CCM_TARGET_USB_PHY_REF              99U
#define IMX_CCM_TARGET_GIC                      100U
#define IMX_CCM_TARGET_ECSPI1                   101U
#define IMX_CCM_TARGET_ECSPI2                   102U
#define IMX_CCM_TARGET_PWM1                     103U
#define IMX_CCM_TARGET_PWM2                     104U
#define IMX_CCM_TARGET_PWM3                     105U
#define IMX_CCM_TARGET_PWM4                     106U
#define IMX_CCM_TARGET_GPT1                     107U
#define IMX_CCM_TARGET_GPT2                     108U
#define IMX_CCM_TARGET_GPT3                     109U
#define IMX_CCM_TARGET_GPT4                     110U
#define IMX_CCM_TARGET_GPT5                     111U
#define IMX_CCM_TARGET_GPT6                     112U
#define IMX_CCM_TARGET_TRACE                    113U
#define IMX_CCM_TARGET_WDOG                     114U
#define IMX_CCM_TARGET_WRCLK                    115U
#define IMX_CCM_TARGET_IPP_DO_CLKO1             116U
#define IMX_CCM_TARGET_IPP_DO_CLKO2             117U
#define IMX_CCM_TARGET_HDMI_FDCC_TST            118U
#define IMX_CCM_TARGET_HDMI_REF_266M            120U
#define IMX_CCM_TARGET_USDHC3                   121U
#define IMX_CCM_TARGET_MEDIA_CAM1_PIX           122U
#define IMX_CCM_TARGET_MEDIA_MIPI_PHY1          123U
#define IMX_CCM_TARGET_MEDIA_DISP1_PIX          124U
#define IMX_CCM_TARGET_MEDIA_CAM2_PIX           125U
#define IMX_CCM_TARGET_MEDIA_LDB                126U
#define IMX_CCM_TARGET_MEDIA_MIPI_TEST_BYTE     130U
#define IMX_CCM_TARGET_ECSPI3                   131U
#define IMX_CCM_TARGET_PDM                      132U
#define IMX_CCM_TARGET_VPU_VC8000E              133U
#define IMX_CCM_TARGET_SAI7                     134U

/* CCGR device clock gate mapping */
#define IMX_CCM_CCGR_DVFS                       0U
#define IMX_CCM_CCGR_ANAMIX                     1U
#define IMX_CCM_CCGR_CPU                        2U
#define IMX_CCM_CCGR_CSU                        3U
#define IMX_CCM_CCGR_DEBUG                      4U
#define IMX_CCM_CCGR_DRAM1                      5U
#define IMX_CCM_CCGR_RESERVED5                  6U
#define IMX_CCM_CCGR_ECSPI1                     7U
#define IMX_CCM_CCGR_ECSPI2                     8U
#define IMX_CCM_CCGR_ECSPI3                     9U
#define IMX_CCM_CCGR_ENET1                      10U
#define IMX_CCM_CCGR_GPIO1                      11U
#define IMX_CCM_CCGR_GPIO2                      12U
#define IMX_CCM_CCGR_GPIO3                      13U
#define IMX_CCM_CCGR_GPIO4                      14U
#define IMX_CCM_CCGR_GPIO5                      15U
#define IMX_CCM_CCGR_GPT1                       16U
#define IMX_CCM_CCGR_GPT2                       17U
#define IMX_CCM_CCGR_GPT3                       18U
#define IMX_CCM_CCGR_GPT4                       19U
#define IMX_CCM_CCGR_GPT5                       20U
#define IMX_CCM_CCGR_GPT6                       21U
#define IMX_CCM_CCGR_HS                         22U
#define IMX_CCM_CCGR_I2C1                       23U
#define IMX_CCM_CCGR_I2C2                       24U
#define IMX_CCM_CCGR_I2C3                       25U
#define IMX_CCM_CCGR_I2C4                       26U
#define IMX_CCM_CCGR_IOMUX                      27U
#define IMX_CCM_CCGR_IPMUX1                     28U
#define IMX_CCM_CCGR_IPMUX2                     29U
#define IMX_CCM_CCGR_IPMUX3                     30U
#define IMX_CCM_CCGR_RESERVED31                 31U
#define IMX_CCM_CCGR_RESERVED32                 32U
#define IMX_CCM_CCGR_MU                         33U
#define IMX_CCM_CCGR_OCOTP                      34U
#define IMX_CCM_CCGR_OCRAM                      35U
#define IMX_CCM_CCGR_OCRAM_S                    36U
#define IMX_CCM_CCGR_PCIE                       37U
#define IMX_CCM_CCGR_PERFMON1                   38U
#define IMX_CCM_CCGR_PERFMON2                   39U
#define IMX_CCM_CCGR_PWM1                       40U
#define IMX_CCM_CCGR_PWM2                       41U
#define IMX_CCM_CCGR_PWM3                       42U
#define IMX_CCM_CCGR_PWM4                       43U
#define IMX_CCM_CCGR_QOS                        44U
#define IMX_CCM_CCGR_RESERVED45                  45U
#define IMX_CCM_CCGR_QOS_ENET                   46U
#define IMX_CCM_CCGR_FLEXSPI                    47U
#define IMX_CCM_CCGR_RAWNAND                    48U
#define IMX_CCM_CCGR_RDC                        49U
#define IMX_CCM_CCGR_ROM                        50U
#define IMX_CCM_CCGR_I2C5                       51U
#define IMX_CCM_CCGR_I2C6                       52U
#define IMX_CCM_CCGR_CAN1                       53U
#define IMX_CCM_CCGR_CAN2                       54U
#define IMX_CCM_CCGR_RESERVED55                 55U
#define IMX_CCM_CCGR_RESERVED56                 56U
#define IMX_CCM_CCGR_SCTR                       57U
#define IMX_CCM_CCGR_SDMA1                      58U
#define IMX_CCM_CCGR_ENET_QOS                   59U
#define IMX_CCM_CCGR_SEC_DEBUG                  60U
#define IMX_CCM_CCGR_SEMA1                      61U
#define IMX_CCM_CCGR_SEMA2                      62U
#define IMX_CCM_CCGR_IRQ_STEER                  63U
#define IMX_CCM_CCGR_SIM_ENET                   64U
#define IMX_CCM_CCGR_SIM_M                      65U
#define IMX_CCM_CCGR_SIM_MAIN                   66U
#define IMX_CCM_CCGR_SIM_S                      67U
#define IMX_CCM_CCGR_SIM_WAKEUP                 68U
#define IMX_CCM_CCGR_GPU2D                      69U
#define IMX_CCM_CCGR_GPU3D                      70U
#define IMX_CCM_CCGR_SNVS                       71U
#define IMX_CCM_CCGR_TRACE                      72U
#define IMX_CCM_CCGR_UART1                      73U
#define IMX_CCM_CCGR_UART2                      74U
#define IMX_CCM_CCGR_UART3                      75U
#define IMX_CCM_CCGR_UART4                      76U
#define IMX_CCM_CCGR_USB                        77U
#define IMX_CCM_CCGR_RESERVED78                 78U
#define IMX_CCM_CCGR_USB_PHY                    79U
#define IMX_CCM_CCGR_RESERVED80                 80U
#define IMX_CCM_CCGR_USDHC1                     81U
#define IMX_CCM_CCGR_USDHC2                     82U
#define IMX_CCM_CCGR_WDOG1                      83U
#define IMX_CCM_CCGR_WDOG2                      84U
#define IMX_CCM_CCGR_WDOG3                      85U
#define IMX_CCM_CCGR_VPUG1                      86U
#define IMX_CCM_CCGR_GPU                        87U
#define IMX_CCM_CCGR_OC_WRAPPER                 88U
#define IMX_CCM_CCGR_VPU_VC8KE                  89U
#define IMX_CCM_CCGR_VPU_G2                     90U
#define IMX_CCM_CCGR_NPU                        91U
#define IMX_CCM_CCGR_HSIO                       92U
#define IMX_CCM_CCGR_MEDIA                      93U
#define IMX_CCM_CCGR_USDHC3                     94U
#define IMX_CCM_CCGR_HDMI                       95U
#define IMX_CCM_CCGR_XTAL                       96U
#define IMX_CCM_CCGR_PLL                        97U
#define IMX_CCM_CCGR_TSENSOR                    98U
#define IMX_CCM_CCGR_VPU                        99U
#define IMX_CCM_CCGR_MRPR                       100U
#define IMX_CCM_CCGR_AUDIO                      101U
#define IMX_CCM_CCGR_RESERVED102                102U

/* Register GPR0 - General Purpose Register */
#define IMX_CCM_GPR0                             0
/* Field GP0 -  */
#define IMX_CCM_GPR0_GP0_SHIFT                   0
#define IMX_CCM_GPR0_GP0_MASK                    0xFFFFFFFF
#define IMX_CCM_GPR0_GP0(v)                      (((v) << 0) & 0xFFFFFFFF)

/* Register GPR0_SET - General Purpose Register */
#define IMX_CCM_GPR0_SET                         0x4
/* Field GP0 -  */
#define IMX_CCM_GPR0_SET_GP0_SHIFT               0
#define IMX_CCM_GPR0_SET_GP0_MASK                0xFFFFFFFF
#define IMX_CCM_GPR0_SET_GP0(v)                  (((v) << 0) & 0xFFFFFFFF)

/* Register GPR0_CLR - General Purpose Register */
#define IMX_CCM_GPR0_CLR                         0x8
/* Field GP0 -  */
#define IMX_CCM_GPR0_CLR_GP0_SHIFT               0
#define IMX_CCM_GPR0_CLR_GP0_MASK                0xFFFFFFFF
#define IMX_CCM_GPR0_CLR_GP0(v)                  (((v) << 0) & 0xFFFFFFFF)

/* Register GPR0_TOG - General Purpose Register */
#define IMX_CCM_GPR0_TOG                         0xC
/* Field GP0 -  */
#define IMX_CCM_GPR0_TOG_GP0_SHIFT               0
#define IMX_CCM_GPR0_TOG_GP0_MASK                0xFFFFFFFF
#define IMX_CCM_GPR0_TOG_GP0(v)                  (((v) << 0) & 0xFFFFFFFF)

/* Register PLL_CTRL - CCM PLL Control Register */
#define IMX_CCM_PLL_CTRL                         0x800
/* Field SETTING0 -  */
#define IMX_CCM_PLL_CTRL_SETTING0_SHIFT          0
#define IMX_CCM_PLL_CTRL_SETTING0_MASK           0x3
#define IMX_CCM_PLL_CTRL_SETTING0(v)             (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_PLL_CTRL_SETTING1_SHIFT          4
#define IMX_CCM_PLL_CTRL_SETTING1_MASK           0x30
#define IMX_CCM_PLL_CTRL_SETTING1(v)             (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_PLL_CTRL_SETTING2_SHIFT          8
#define IMX_CCM_PLL_CTRL_SETTING2_MASK           0x300
#define IMX_CCM_PLL_CTRL_SETTING2(v)             (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_PLL_CTRL_SETTING3_SHIFT          12
#define IMX_CCM_PLL_CTRL_SETTING3_MASK           0x3000
#define IMX_CCM_PLL_CTRL_SETTING3(v)             (((v) << 0xC) & 0x3000)

/* Register PLL_CTRL_SET - CCM PLL Control Register */
#define IMX_CCM_PLL_CTRL_SET                     0x804
/* Field SETTING0 -  */
#define IMX_CCM_PLL_CTRL_SET_SETTING0_SHIFT      0
#define IMX_CCM_PLL_CTRL_SET_SETTING0_MASK       0x3
#define IMX_CCM_PLL_CTRL_SET_SETTING0(v)         (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_PLL_CTRL_SET_SETTING1_SHIFT      4
#define IMX_CCM_PLL_CTRL_SET_SETTING1_MASK       0x30
#define IMX_CCM_PLL_CTRL_SET_SETTING1(v)         (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_PLL_CTRL_SET_SETTING2_SHIFT      8
#define IMX_CCM_PLL_CTRL_SET_SETTING2_MASK       0x300
#define IMX_CCM_PLL_CTRL_SET_SETTING2(v)         (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_PLL_CTRL_SET_SETTING3_SHIFT      12
#define IMX_CCM_PLL_CTRL_SET_SETTING3_MASK       0x3000
#define IMX_CCM_PLL_CTRL_SET_SETTING3(v)         (((v) << 0xC) & 0x3000)

/* Register PLL_CTRL_CLR - CCM PLL Control Register */
#define IMX_CCM_PLL_CTRL_CLR                     0x808
/* Field SETTING0 -  */
#define IMX_CCM_PLL_CTRL_CLR_SETTING0_SHIFT      0
#define IMX_CCM_PLL_CTRL_CLR_SETTING0_MASK       0x3
#define IMX_CCM_PLL_CTRL_CLR_SETTING0(v)         (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_PLL_CTRL_CLR_SETTING1_SHIFT      4
#define IMX_CCM_PLL_CTRL_CLR_SETTING1_MASK       0x30
#define IMX_CCM_PLL_CTRL_CLR_SETTING1(v)         (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_PLL_CTRL_CLR_SETTING2_SHIFT      8
#define IMX_CCM_PLL_CTRL_CLR_SETTING2_MASK       0x300
#define IMX_CCM_PLL_CTRL_CLR_SETTING2(v)         (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_PLL_CTRL_CLR_SETTING3_SHIFT      12
#define IMX_CCM_PLL_CTRL_CLR_SETTING3_MASK       0x3000
#define IMX_CCM_PLL_CTRL_CLR_SETTING3(v)         (((v) << 0xC) & 0x3000)

/* Register PLL_CTRL_TOG - CCM PLL Control Register */
#define IMX_CCM_PLL_CTRL_TOG                     0x80C
/* Field SETTING0 -  */
#define IMX_CCM_PLL_CTRL_TOG_SETTING0_SHIFT      0
#define IMX_CCM_PLL_CTRL_TOG_SETTING0_MASK       0x3
#define IMX_CCM_PLL_CTRL_TOG_SETTING0(v)         (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_PLL_CTRL_TOG_SETTING1_SHIFT      4
#define IMX_CCM_PLL_CTRL_TOG_SETTING1_MASK       0x30
#define IMX_CCM_PLL_CTRL_TOG_SETTING1(v)         (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_PLL_CTRL_TOG_SETTING2_SHIFT      8
#define IMX_CCM_PLL_CTRL_TOG_SETTING2_MASK       0x300
#define IMX_CCM_PLL_CTRL_TOG_SETTING2(v)         (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_PLL_CTRL_TOG_SETTING3_SHIFT      12
#define IMX_CCM_PLL_CTRL_TOG_SETTING3_MASK       0x3000
#define IMX_CCM_PLL_CTRL_TOG_SETTING3(v)         (((v) << 0xC) & 0x3000)

/* Register CCGR - CCM Clock Gating Register */
#define IMX_CCM_CCGR                             0x4000
/* Field SETTING0 -  */
#define IMX_CCM_CCGR_SETTING0_SHIFT              0
#define IMX_CCM_CCGR_SETTING0_MASK               0x3
#define IMX_CCM_CCGR_SETTING0(v)                 (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_CCGR_SETTING1_SHIFT              4
#define IMX_CCM_CCGR_SETTING1_MASK               0x30
#define IMX_CCM_CCGR_SETTING1(v)                 (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_CCGR_SETTING2_SHIFT              8
#define IMX_CCM_CCGR_SETTING2_MASK               0x300
#define IMX_CCM_CCGR_SETTING2(v)                 (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_CCGR_SETTING3_SHIFT              12
#define IMX_CCM_CCGR_SETTING3_MASK               0x3000
#define IMX_CCM_CCGR_SETTING3(v)                 (((v) << 0xC) & 0x3000)

/* Register CCGR_SET - CCM Clock Gating Register */
#define IMX_CCM_CCGR_SET                         0x4004
/* Field SETTING0 -  */
#define IMX_CCM_CCGR_SET_SETTING0_SHIFT          0
#define IMX_CCM_CCGR_SET_SETTING0_MASK           0x3
#define IMX_CCM_CCGR_SET_SETTING0(v)             (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_CCGR_SET_SETTING1_SHIFT          4
#define IMX_CCM_CCGR_SET_SETTING1_MASK           0x30
#define IMX_CCM_CCGR_SET_SETTING1(v)             (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_CCGR_SET_SETTING2_SHIFT          8
#define IMX_CCM_CCGR_SET_SETTING2_MASK           0x300
#define IMX_CCM_CCGR_SET_SETTING2(v)             (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_CCGR_SET_SETTING3_SHIFT          12
#define IMX_CCM_CCGR_SET_SETTING3_MASK           0x3000
#define IMX_CCM_CCGR_SET_SETTING3(v)             (((v) << 0xC) & 0x3000)

/* Register CCGR_CLR - CCM Clock Gating Register */
#define IMX_CCM_CCGR_CLR                         0x4008
/* Field SETTING0 -  */
#define IMX_CCM_CCGR_CLR_SETTING0_SHIFT          0
#define IMX_CCM_CCGR_CLR_SETTING0_MASK           0x3
#define IMX_CCM_CCGR_CLR_SETTING0(v)             (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_CCGR_CLR_SETTING1_SHIFT          4
#define IMX_CCM_CCGR_CLR_SETTING1_MASK           0x30
#define IMX_CCM_CCGR_CLR_SETTING1(v)             (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_CCGR_CLR_SETTING2_SHIFT          8
#define IMX_CCM_CCGR_CLR_SETTING2_MASK           0x300
#define IMX_CCM_CCGR_CLR_SETTING2(v)             (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_CCGR_CLR_SETTING3_SHIFT          12
#define IMX_CCM_CCGR_CLR_SETTING3_MASK           0x3000
#define IMX_CCM_CCGR_CLR_SETTING3(v)             (((v) << 0xC) & 0x3000)

/* Register CCGR_TOG - CCM Clock Gating Register */
#define IMX_CCM_CCGR_TOG                         0x400C
/* Field SETTING0 -  */
#define IMX_CCM_CCGR_TOG_SETTING0_SHIFT          0
#define IMX_CCM_CCGR_TOG_SETTING0_MASK           0x3
#define IMX_CCM_CCGR_TOG_SETTING0(v)             (((v) << 0) & 0x3)
/* Field SETTING1 -  */
#define IMX_CCM_CCGR_TOG_SETTING1_SHIFT          4
#define IMX_CCM_CCGR_TOG_SETTING1_MASK           0x30
#define IMX_CCM_CCGR_TOG_SETTING1(v)             (((v) << 0x4) & 0x30)
/* Field SETTING2 -  */
#define IMX_CCM_CCGR_TOG_SETTING2_SHIFT          8
#define IMX_CCM_CCGR_TOG_SETTING2_MASK           0x300
#define IMX_CCM_CCGR_TOG_SETTING2(v)             (((v) << 0x8) & 0x300)
/* Field SETTING3 -  */
#define IMX_CCM_CCGR_TOG_SETTING3_SHIFT          12
#define IMX_CCM_CCGR_TOG_SETTING3_MASK           0x3000
#define IMX_CCM_CCGR_TOG_SETTING3(v)             (((v) << 0xC) & 0x3000)

/* Register TARGET_ROOT - Target Register */
#define IMX_CCM_TARGET_ROOT                      0x8000
/* Field POST_PODF - Post divider divide number */
//#define IMX_CCM_TARGET_ROOT_POST_PODF_SHIFT      0
//#define IMX_CCM_TARGET_ROOT_POST_PODF_MASK       0x3F
//#define IMX_CCM_TARGET_ROOT_POST_PODF(v)         (((v) << 0) & 0x3F)
/* Field PRE_PODF - Pre divider divide the number */
//#define IMX_CCM_TARGET_ROOT_PRE_PODF_SHIFT       16
//#define IMX_CCM_TARGET_ROOT_PRE_PODF_MASK        0x70000
//#define IMX_CCM_TARGET_ROOT_PRE_PODF(v)          (((v) << 0x10) & 0x70000)
/* Field MUX - Selection of clock sources */
//#define IMX_CCM_TARGET_ROOT_MUX_SHIFT            24
//#define IMX_CCM_TARGET_ROOT_MUX_MASK             0x7000000
//#define IMX_CCM_TARGET_ROOT_MUX(v)               (((v) << 0x18) & 0x7000000)
/* Field ENABLE - Enable this clock */
//#define IMX_CCM_TARGET_ROOT_ENABLE_SHIFT         28
//#define IMX_CCM_TARGET_ROOT_ENABLE_MASK          0x10000000
//#define IMX_CCM_TARGET_ROOT_ENABLE(v)            (((v) << 0x1C) & 0x10000000)

/* Register TARGET_ROOT_SET - Target Register */
#define IMX_CCM_TARGET_ROOT_SET                  0x8004
/* Field POST_PODF - Post divider divide the number */
#define IMX_CCM_TARGET_ROOT_SET_POST_PODF_SHIFT  0
#define IMX_CCM_TARGET_ROOT_SET_POST_PODF_MASK   0x3F
#define IMX_CCM_TARGET_ROOT_SET_POST_PODF(v)     (((v) << 0) & 0x3F)
/* Field PRE_PODF - Pre divider divide the number */
#define IMX_CCM_TARGET_ROOT_SET_PRE_PODF_SHIFT   16
#define IMX_CCM_TARGET_ROOT_SET_PRE_PODF_MASK    0x70000
#define IMX_CCM_TARGET_ROOT_SET_PRE_PODF(v)      (((v) << 0x10) & 0x70000)
/* Field MUX - Selection of clock sources */
#define IMX_CCM_TARGET_ROOT_SET_MUX_SHIFT        24
#define IMX_CCM_TARGET_ROOT_SET_MUX_MASK         0x7000000
#define IMX_CCM_TARGET_ROOT_SET_MUX(v)           (((v) << 0x18) & 0x7000000)
/* Field ENABLE - Enable this clock */
#define IMX_CCM_TARGET_ROOT_SET_ENABLE_SHIFT     28
#define IMX_CCM_TARGET_ROOT_SET_ENABLE_MASK      0x10000000
#define IMX_CCM_TARGET_ROOT_SET_ENABLE(v)        (((v) << 0x1C) & 0x10000000)

/* Register TARGET_ROOT_CLR - Target Register */
#define IMX_CCM_TARGET_ROOT_CLR                  0x8008
/* Field POST_PODF - Post divider divide the number */
#define IMX_CCM_TARGET_ROOT_CLR_POST_PODF_SHIFT  0
#define IMX_CCM_TARGET_ROOT_CLR_POST_PODF_MASK   0x3F
#define IMX_CCM_TARGET_ROOT_CLR_POST_PODF(v)     (((v) << 0) & 0x3F)
/* Field PRE_PODF - Pre divider divide the number */
#define IMX_CCM_TARGET_ROOT_CLR_PRE_PODF_SHIFT   16
#define IMX_CCM_TARGET_ROOT_CLR_PRE_PODF_MASK    0x70000
#define IMX_CCM_TARGET_ROOT_CLR_PRE_PODF(v)      (((v) << 0x10) & 0x70000)
/* Field MUX -  */
#define IMX_CCM_TARGET_ROOT_CLR_MUX_SHIFT        24
#define IMX_CCM_TARGET_ROOT_CLR_MUX_MASK         0x7000000
#define IMX_CCM_TARGET_ROOT_CLR_MUX(v)           (((v) << 0x18) & 0x7000000)
/* Field ENABLE - Enable this clock */
#define IMX_CCM_TARGET_ROOT_CLR_ENABLE_SHIFT     28
#define IMX_CCM_TARGET_ROOT_CLR_ENABLE_MASK      0x10000000
#define IMX_CCM_TARGET_ROOT_CLR_ENABLE(v)        (((v) << 0x1C) & 0x10000000)

/* Register TARGET_ROOT_TOG - Target Register */
#define IMX_CCM_TARGET_ROOT_TOG                  0x800C
/* Field POST_PODF - Post divider divide the number */
#define IMX_CCM_TARGET_ROOT_TOG_POST_PODF_SHIFT  0
#define IMX_CCM_TARGET_ROOT_TOG_POST_PODF_MASK   0x3F
#define IMX_CCM_TARGET_ROOT_TOG_POST_PODF(v)     (((v) << 0) & 0x3F)
/* Field PRE_PODF - Pre divider divide the number */
#define IMX_CCM_TARGET_ROOT_TOG_PRE_PODF_SHIFT   16
#define IMX_CCM_TARGET_ROOT_TOG_PRE_PODF_MASK    0x70000
#define IMX_CCM_TARGET_ROOT_TOG_PRE_PODF(v)      (((v) << 0x10) & 0x70000)
/* Field MUX -  */
#define IMX_CCM_TARGET_ROOT_TOG_MUX_SHIFT        24
#define IMX_CCM_TARGET_ROOT_TOG_MUX_MASK         0x7000000
#define IMX_CCM_TARGET_ROOT_TOG_MUX(v)           (((v) << 0x18) & 0x7000000)
/* Field ENABLE - Enable this clock */
#define IMX_CCM_TARGET_ROOT_TOG_ENABLE_SHIFT     28
#define IMX_CCM_TARGET_ROOT_TOG_ENABLE_MASK      0x10000000
#define IMX_CCM_TARGET_ROOT_TOG_ENABLE(v)        (((v) << 0x1C) & 0x10000000)

/* Register MISC - Miscellaneous Register */
#define IMX_CCM_MISC                             0x8010
/* Field AUTHEN_FAIL -  */
#define IMX_CCM_MISC_AUTHEN_FAIL_SHIFT           0
#define IMX_CCM_MISC_AUTHEN_FAIL_MASK            0x1
#define IMX_CCM_MISC_AUTHEN_FAIL(v)              (((v) << 0) & 0x1)
/* Field TIMEOUT -  */
#define IMX_CCM_MISC_TIMEOUT_SHIFT               4
#define IMX_CCM_MISC_TIMEOUT_MASK                0x10
#define IMX_CCM_MISC_TIMEOUT(v)                  (((v) << 0x4) & 0x10)
/* Field VIOLATE -  */
#define IMX_CCM_MISC_VIOLATE_SHIFT               8
#define IMX_CCM_MISC_VIOLATE_MASK                0x100
#define IMX_CCM_MISC_VIOLATE(v)                  (((v) << 0x8) & 0x100)

/* Register MISC_ROOT_SET - Miscellaneous Register */
#define IMX_CCM_MISC_ROOT_SET                    0x8014
/* Field AUTHEN_FAIL -  */
#define IMX_CCM_MISC_ROOT_SET_AUTHEN_FAIL_SHIFT  0
#define IMX_CCM_MISC_ROOT_SET_AUTHEN_FAIL_MASK   0x1
#define IMX_CCM_MISC_ROOT_SET_AUTHEN_FAIL(v)     (((v) << 0) & 0x1)
/* Field TIMEOUT -  */
#define IMX_CCM_MISC_ROOT_SET_TIMEOUT_SHIFT      4
#define IMX_CCM_MISC_ROOT_SET_TIMEOUT_MASK       0x10
#define IMX_CCM_MISC_ROOT_SET_TIMEOUT(v)         (((v) << 0x4) & 0x10)
/* Field VIOLATE -  */
#define IMX_CCM_MISC_ROOT_SET_VIOLATE_SHIFT      8
#define IMX_CCM_MISC_ROOT_SET_VIOLATE_MASK       0x100
#define IMX_CCM_MISC_ROOT_SET_VIOLATE(v)         (((v) << 0x8) & 0x100)

/* Register MISC_ROOT_CLR - Miscellaneous Register */
#define IMX_CCM_MISC_ROOT_CLR                    0x8018
/* Field AUTHEN_FAIL -  */
#define IMX_CCM_MISC_ROOT_CLR_AUTHEN_FAIL_SHIFT  0
#define IMX_CCM_MISC_ROOT_CLR_AUTHEN_FAIL_MASK   0x1
#define IMX_CCM_MISC_ROOT_CLR_AUTHEN_FAIL(v)     (((v) << 0) & 0x1)
/* Field TIMEOUT -  */
#define IMX_CCM_MISC_ROOT_CLR_TIMEOUT_SHIFT      4
#define IMX_CCM_MISC_ROOT_CLR_TIMEOUT_MASK       0x10
#define IMX_CCM_MISC_ROOT_CLR_TIMEOUT(v)         (((v) << 0x4) & 0x10)
/* Field VIOLATE -  */
#define IMX_CCM_MISC_ROOT_CLR_VIOLATE_SHIFT      8
#define IMX_CCM_MISC_ROOT_CLR_VIOLATE_MASK       0x100
#define IMX_CCM_MISC_ROOT_CLR_VIOLATE(v)         (((v) << 0x8) & 0x100)

/* Register MISC_ROOT_TOG - Miscellaneous Register */
#define IMX_CCM_MISC_ROOT_TOG                    0x801C
/* Field AUTHEN_FAIL -  */
#define IMX_CCM_MISC_ROOT_TOG_AUTHEN_FAIL_SHIFT  0
#define IMX_CCM_MISC_ROOT_TOG_AUTHEN_FAIL_MASK   0x1
#define IMX_CCM_MISC_ROOT_TOG_AUTHEN_FAIL(v)     (((v) << 0) & 0x1)
/* Field TIMEOUT -  */
#define IMX_CCM_MISC_ROOT_TOG_TIMEOUT_SHIFT      4
#define IMX_CCM_MISC_ROOT_TOG_TIMEOUT_MASK       0x10
#define IMX_CCM_MISC_ROOT_TOG_TIMEOUT(v)         (((v) << 0x4) & 0x10)
/* Field VIOLATE -  */
#define IMX_CCM_MISC_ROOT_TOG_VIOLATE_SHIFT      8
#define IMX_CCM_MISC_ROOT_TOG_VIOLATE_MASK       0x100
#define IMX_CCM_MISC_ROOT_TOG_VIOLATE(v)         (((v) << 0x8) & 0x100)

/* Register POST - Post Divider Register */
#define IMX_CCM_POST                             0x8020
/* Field POST_PODF -  */
#define IMX_CCM_POST_POST_PODF_SHIFT             0
#define IMX_CCM_POST_POST_PODF_MASK              0x3F
#define IMX_CCM_POST_POST_PODF(v)                (((v) << 0) & 0x3F)
/* Field BUSY1 -  */
#define IMX_CCM_POST_BUSY1_SHIFT                 7
#define IMX_CCM_POST_BUSY1_MASK                  0x80
#define IMX_CCM_POST_BUSY1(v)                    (((v) << 0x7) & 0x80)
/* Field SELECT -  */
#define IMX_CCM_POST_SELECT_SHIFT                28
#define IMX_CCM_POST_SELECT_MASK                 0x10000000
#define IMX_CCM_POST_SELECT(v)                   (((v) << 0x1C) & 0x10000000)
/* Field BUSY2 -  */
#define IMX_CCM_POST_BUSY2_SHIFT                 31
#define IMX_CCM_POST_BUSY2_MASK                  0x80000000
#define IMX_CCM_POST_BUSY2(v)                    (((v) << 0x1F) & 0x80000000)

/* Register POST_ROOT_SET - Post Divider Register */
#define IMX_CCM_POST_ROOT_SET                    0x8024
/* Field POST_PODF -  */
#define IMX_CCM_POST_ROOT_SET_POST_PODF_SHIFT    0
#define IMX_CCM_POST_ROOT_SET_POST_PODF_MASK     0x3F
#define IMX_CCM_POST_ROOT_SET_POST_PODF(v)       (((v) << 0) & 0x3F)
/* Field BUSY1 -  */
#define IMX_CCM_POST_ROOT_SET_BUSY1_SHIFT        7
#define IMX_CCM_POST_ROOT_SET_BUSY1_MASK         0x80
#define IMX_CCM_POST_ROOT_SET_BUSY1(v)           (((v) << 0x7) & 0x80)
/* Field SELECT -  */
#define IMX_CCM_POST_ROOT_SET_SELECT_SHIFT       28
#define IMX_CCM_POST_ROOT_SET_SELECT_MASK        0x10000000
#define IMX_CCM_POST_ROOT_SET_SELECT(v)          (((v) << 0x1C) & 0x10000000)
/* Field BUSY2 -  */
#define IMX_CCM_POST_ROOT_SET_BUSY2_SHIFT        31
#define IMX_CCM_POST_ROOT_SET_BUSY2_MASK         0x80000000
#define IMX_CCM_POST_ROOT_SET_BUSY2(v)           (((v) << 0x1F) & 0x80000000)

/* Register POST_ROOT_CLR - Post Divider Register */
#define IMX_CCM_POST_ROOT_CLR                    0x8028
/* Field POST_PODF -  */
#define IMX_CCM_POST_ROOT_CLR_POST_PODF_SHIFT    0
#define IMX_CCM_POST_ROOT_CLR_POST_PODF_MASK     0x3F
#define IMX_CCM_POST_ROOT_CLR_POST_PODF(v)       (((v) << 0) & 0x3F)
/* Field BUSY1 -  */
#define IMX_CCM_POST_ROOT_CLR_BUSY1_SHIFT        7
#define IMX_CCM_POST_ROOT_CLR_BUSY1_MASK         0x80
#define IMX_CCM_POST_ROOT_CLR_BUSY1(v)           (((v) << 0x7) & 0x80)
/* Field SELECT -  */
#define IMX_CCM_POST_ROOT_CLR_SELECT_SHIFT       28
#define IMX_CCM_POST_ROOT_CLR_SELECT_MASK        0x10000000
#define IMX_CCM_POST_ROOT_CLR_SELECT(v)          (((v) << 0x1C) & 0x10000000)
/* Field BUSY2 -  */
#define IMX_CCM_POST_ROOT_CLR_BUSY2_SHIFT        31
#define IMX_CCM_POST_ROOT_CLR_BUSY2_MASK         0x80000000
#define IMX_CCM_POST_ROOT_CLR_BUSY2(v)           (((v) << 0x1F) & 0x80000000)

/* Register POST_ROOT_TOG - Post Divider Register */
#define IMX_CCM_POST_ROOT_TOG                    0x802C
/* Field POST_PODF -  */
#define IMX_CCM_POST_ROOT_TOG_POST_PODF_SHIFT    0
#define IMX_CCM_POST_ROOT_TOG_POST_PODF_MASK     0x3F
#define IMX_CCM_POST_ROOT_TOG_POST_PODF(v)       (((v) << 0) & 0x3F)
/* Field BUSY1 -  */
#define IMX_CCM_POST_ROOT_TOG_BUSY1_SHIFT        7
#define IMX_CCM_POST_ROOT_TOG_BUSY1_MASK         0x80
#define IMX_CCM_POST_ROOT_TOG_BUSY1(v)           (((v) << 0x7) & 0x80)
/* Field SELECT -  */
#define IMX_CCM_POST_ROOT_TOG_SELECT_SHIFT       28
#define IMX_CCM_POST_ROOT_TOG_SELECT_MASK        0x10000000
#define IMX_CCM_POST_ROOT_TOG_SELECT(v)          (((v) << 0x1C) & 0x10000000)
/* Field BUSY2 -  */
#define IMX_CCM_POST_ROOT_TOG_BUSY2_SHIFT        31
#define IMX_CCM_POST_ROOT_TOG_BUSY2_MASK         0x80000000
#define IMX_CCM_POST_ROOT_TOG_BUSY2(v)           (((v) << 0x1F) & 0x80000000)

/* Register PRE - Pre Divider Register */
#define IMX_CCM_PRE                              0x8030
/* Field PRE_PODF_B -  */
#define IMX_CCM_PRE_PRE_PODF_B_SHIFT             0
#define IMX_CCM_PRE_PRE_PODF_B_MASK              0x7
#define IMX_CCM_PRE_PRE_PODF_B(v)                (((v) << 0) & 0x7)
/* Field BUSY0 -  */
#define IMX_CCM_PRE_BUSY0_SHIFT                  3
#define IMX_CCM_PRE_BUSY0_MASK                   0x8
#define IMX_CCM_PRE_BUSY0(v)                     (((v) << 0x3) & 0x8)
/* Field MUX_B -  */
#define IMX_CCM_PRE_MUX_B_SHIFT                  8
#define IMX_CCM_PRE_MUX_B_MASK                   0x700
#define IMX_CCM_PRE_MUX_B(v)                     (((v) << 0x8) & 0x700)
/* Field EN_B -  */
#define IMX_CCM_PRE_EN_B_SHIFT                   12
#define IMX_CCM_PRE_EN_B_MASK                    0x1000
#define IMX_CCM_PRE_EN_B(v)                      (((v) << 0xC) & 0x1000)
/* Field BUSY1 -  */
#define IMX_CCM_PRE_BUSY1_SHIFT                  15
#define IMX_CCM_PRE_BUSY1_MASK                   0x8000
#define IMX_CCM_PRE_BUSY1(v)                     (((v) << 0xF) & 0x8000)
/* Field PRE_PODF_A -  */
#define IMX_CCM_PRE_PRE_PODF_A_SHIFT             16
#define IMX_CCM_PRE_PRE_PODF_A_MASK              0x70000
#define IMX_CCM_PRE_PRE_PODF_A(v)                (((v) << 0x10) & 0x70000)
/* Field BUSY3 -  */
#define IMX_CCM_PRE_BUSY3_SHIFT                  19
#define IMX_CCM_PRE_BUSY3_MASK                   0x80000
#define IMX_CCM_PRE_BUSY3(v)                     (((v) << 0x13) & 0x80000)
/* Field MUX_A -  */
#define IMX_CCM_PRE_MUX_A_SHIFT                  24
#define IMX_CCM_PRE_MUX_A_MASK                   0x7000000
#define IMX_CCM_PRE_MUX_A(v)                     (((v) << 0x18) & 0x7000000)
/* Field EN_A -  */
#define IMX_CCM_PRE_EN_A_SHIFT                   28
#define IMX_CCM_PRE_EN_A_MASK                    0x10000000
#define IMX_CCM_PRE_EN_A(v)                      (((v) << 0x1C) & 0x10000000)
/* Field BUSY4 -  */
#define IMX_CCM_PRE_BUSY4_SHIFT                  31
#define IMX_CCM_PRE_BUSY4_MASK                   0x80000000
#define IMX_CCM_PRE_BUSY4(v)                     (((v) << 0x1F) & 0x80000000)

/* Register PRE_ROOT_SET - Pre Divider Register */
#define IMX_CCM_PRE_ROOT_SET                     0x8034
/* Field PRE_PODF_B -  */
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_B_SHIFT    0
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_B_MASK     0x7
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_B(v)       (((v) << 0) & 0x7)
/* Field BUSY0 -  */
#define IMX_CCM_PRE_ROOT_SET_BUSY0_SHIFT         3
#define IMX_CCM_PRE_ROOT_SET_BUSY0_MASK          0x8
#define IMX_CCM_PRE_ROOT_SET_BUSY0(v)            (((v) << 0x3) & 0x8)
/* Field MUX_B -  */
#define IMX_CCM_PRE_ROOT_SET_MUX_B_SHIFT         8
#define IMX_CCM_PRE_ROOT_SET_MUX_B_MASK          0x700
#define IMX_CCM_PRE_ROOT_SET_MUX_B(v)            (((v) << 0x8) & 0x700)
/* Field EN_B -  */
#define IMX_CCM_PRE_ROOT_SET_EN_B_SHIFT          12
#define IMX_CCM_PRE_ROOT_SET_EN_B_MASK           0x1000
#define IMX_CCM_PRE_ROOT_SET_EN_B(v)             (((v) << 0xC) & 0x1000)
/* Field BUSY1 -  */
#define IMX_CCM_PRE_ROOT_SET_BUSY1_SHIFT         15
#define IMX_CCM_PRE_ROOT_SET_BUSY1_MASK          0x8000
#define IMX_CCM_PRE_ROOT_SET_BUSY1(v)            (((v) << 0xF) & 0x8000)
/* Field PRE_PODF_A -  */
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_A_SHIFT    16
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_A_MASK     0x70000
#define IMX_CCM_PRE_ROOT_SET_PRE_PODF_A(v)       (((v) << 0x10) & 0x70000)
/* Field BUSY3 -  */
#define IMX_CCM_PRE_ROOT_SET_BUSY3_SHIFT         19
#define IMX_CCM_PRE_ROOT_SET_BUSY3_MASK          0x80000
#define IMX_CCM_PRE_ROOT_SET_BUSY3(v)            (((v) << 0x13) & 0x80000)
/* Field MUX_A -  */
#define IMX_CCM_PRE_ROOT_SET_MUX_A_SHIFT         24
#define IMX_CCM_PRE_ROOT_SET_MUX_A_MASK          0x7000000
#define IMX_CCM_PRE_ROOT_SET_MUX_A(v)            (((v) << 0x18) & 0x7000000)
/* Field EN_A -  */
#define IMX_CCM_PRE_ROOT_SET_EN_A_SHIFT          28
#define IMX_CCM_PRE_ROOT_SET_EN_A_MASK           0x10000000
#define IMX_CCM_PRE_ROOT_SET_EN_A(v)             (((v) << 0x1C) & 0x10000000)
/* Field BUSY4 -  */
#define IMX_CCM_PRE_ROOT_SET_BUSY4_SHIFT         31
#define IMX_CCM_PRE_ROOT_SET_BUSY4_MASK          0x80000000
#define IMX_CCM_PRE_ROOT_SET_BUSY4(v)            (((v) << 0x1F) & 0x80000000)

/* Register PRE_ROOT_CLR - Pre Divider Register */
#define IMX_CCM_PRE_ROOT_CLR                     0x8038
/* Field PRE_PODF_B -  */
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_B_SHIFT    0
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_B_MASK     0x7
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_B(v)       (((v) << 0) & 0x7)
/* Field BUSY0 -  */
#define IMX_CCM_PRE_ROOT_CLR_BUSY0_SHIFT         3
#define IMX_CCM_PRE_ROOT_CLR_BUSY0_MASK          0x8
#define IMX_CCM_PRE_ROOT_CLR_BUSY0(v)            (((v) << 0x3) & 0x8)
/* Field MUX_B -  */
#define IMX_CCM_PRE_ROOT_CLR_MUX_B_SHIFT         8
#define IMX_CCM_PRE_ROOT_CLR_MUX_B_MASK          0x700
#define IMX_CCM_PRE_ROOT_CLR_MUX_B(v)            (((v) << 0x8) & 0x700)
/* Field EN_B -  */
#define IMX_CCM_PRE_ROOT_CLR_EN_B_SHIFT          12
#define IMX_CCM_PRE_ROOT_CLR_EN_B_MASK           0x1000
#define IMX_CCM_PRE_ROOT_CLR_EN_B(v)             (((v) << 0xC) & 0x1000)
/* Field BUSY1 -  */
#define IMX_CCM_PRE_ROOT_CLR_BUSY1_SHIFT         15
#define IMX_CCM_PRE_ROOT_CLR_BUSY1_MASK          0x8000
#define IMX_CCM_PRE_ROOT_CLR_BUSY1(v)            (((v) << 0xF) & 0x8000)
/* Field PRE_PODF_A -  */
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_A_SHIFT    16
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_A_MASK     0x70000
#define IMX_CCM_PRE_ROOT_CLR_PRE_PODF_A(v)       (((v) << 0x10) & 0x70000)
/* Field BUSY3 -  */
#define IMX_CCM_PRE_ROOT_CLR_BUSY3_SHIFT         19
#define IMX_CCM_PRE_ROOT_CLR_BUSY3_MASK          0x80000
#define IMX_CCM_PRE_ROOT_CLR_BUSY3(v)            (((v) << 0x13) & 0x80000)
/* Field MUX_A -  */
#define IMX_CCM_PRE_ROOT_CLR_MUX_A_SHIFT         24
#define IMX_CCM_PRE_ROOT_CLR_MUX_A_MASK          0x7000000
#define IMX_CCM_PRE_ROOT_CLR_MUX_A(v)            (((v) << 24) & 0x7000000)
/* Field EN_A -  */
#define IMX_CCM_PRE_ROOT_CLR_EN_A_SHIFT          28
#define IMX_CCM_PRE_ROOT_CLR_EN_A_MASK           0x10000000
#define IMX_CCM_PRE_ROOT_CLR_EN_A(v)             (((v) << 0x1C) & 0x10000000)
/* Field BUSY4 -  */
#define IMX_CCM_PRE_ROOT_CLR_BUSY4_SHIFT         31
#define IMX_CCM_PRE_ROOT_CLR_BUSY4_MASK          0x80000000
#define IMX_CCM_PRE_ROOT_CLR_BUSY4(v)            (((v) << 0x1F) & 0x80000000)

/* Register PRE_ROOT_TOG - Pre Divider Register */
#define IMX_CCM_PRE_ROOT_TOG                     0x803C
/* Field PRE_PODF_B -  */
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_B_SHIFT    0
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_B_MASK     0x7
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_B(v)       (((v) << 0) & 0x7)
/* Field BUSY0 -  */
#define IMX_CCM_PRE_ROOT_TOG_BUSY0_SHIFT         3
#define IMX_CCM_PRE_ROOT_TOG_BUSY0_MASK          0x8
#define IMX_CCM_PRE_ROOT_TOG_BUSY0(v)            (((v) << 0x3) & 0x8)
/* Field MUX_B -  */
#define IMX_CCM_PRE_ROOT_TOG_MUX_B_SHIFT         8
#define IMX_CCM_PRE_ROOT_TOG_MUX_B_MASK          0x700
#define IMX_CCM_PRE_ROOT_TOG_MUX_B(v)            (((v) << 0x8) & 0x700)
/* Field EN_B -  */
#define IMX_CCM_PRE_ROOT_TOG_EN_B_SHIFT          12
#define IMX_CCM_PRE_ROOT_TOG_EN_B_MASK           0x1000
#define IMX_CCM_PRE_ROOT_TOG_EN_B(v)             (((v) << 0xC) & 0x1000)
/* Field BUSY1 -  */
#define IMX_CCM_PRE_ROOT_TOG_BUSY1_SHIFT         15
#define IMX_CCM_PRE_ROOT_TOG_BUSY1_MASK          0x8000
#define IMX_CCM_PRE_ROOT_TOG_BUSY1(v)            (((v) << 0xF) & 0x8000)
/* Field PRE_PODF_A -  */
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_A_SHIFT    16
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_A_MASK     0x70000
#define IMX_CCM_PRE_ROOT_TOG_PRE_PODF_A(v)       (((v) << 0x10) & 0x70000)
/* Field BUSY3 -  */
#define IMX_CCM_PRE_ROOT_TOG_BUSY3_SHIFT         19
#define IMX_CCM_PRE_ROOT_TOG_BUSY3_MASK          0x80000
#define IMX_CCM_PRE_ROOT_TOG_BUSY3(v)            (((v) << 0x13) & 0x80000)
/* Field MUX_A -  */
#define IMX_CCM_PRE_ROOT_TOG_MUX_A_SHIFT         24
#define IMX_CCM_PRE_ROOT_TOG_MUX_A_MASK          0x7000000
#define IMX_CCM_PRE_ROOT_TOG_MUX_A(v)            (((v) << 0x18) & 0x7000000)
/* Field EN_A -  */
#define IMX_CCM_PRE_ROOT_TOG_EN_A_SHIFT          28
#define IMX_CCM_PRE_ROOT_TOG_EN_A_MASK           0x10000000
#define IMX_CCM_PRE_ROOT_TOG_EN_A(v)             (((v) << 0x1C) & 0x10000000)
/* Field BUSY4 -  */
#define IMX_CCM_PRE_ROOT_TOG_BUSY4_SHIFT         31
#define IMX_CCM_PRE_ROOT_TOG_BUSY4_MASK          0x80000000
#define IMX_CCM_PRE_ROOT_TOG_BUSY4(v)            (((v) << 0x1F) & 0x80000000)

/* Register ACCESS_CTRL - Access Control Register */
#define IMX_CCM_ACCESS_CTRL                      0x8070
/* Field DOMAIN0_INFO -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_INFO_SHIFT   0
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_INFO_MASK    0xF
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_INFO(v)      (((v) << 0) & 0xF)
/* Field DOMAIN1_INFO -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_INFO_SHIFT   4
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_INFO_MASK    0xF0
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_INFO(v)      (((v) << 0x4) & 0xF0)
/* Field DOMAIN2_INFO -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_INFO_SHIFT   8
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_INFO_MASK    0xF00
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_INFO(v)      (((v) << 0x8) & 0xF00)
/* Field DOMAIN3_INFO -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_INFO_SHIFT   12
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_INFO_MASK    0xF000
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_INFO(v)      (((v) << 0xC) & 0xF000)
/* Field OWNER_ID -  */
#define IMX_CCM_ACCESS_CTRL_OWNER_ID_SHIFT       16
#define IMX_CCM_ACCESS_CTRL_OWNER_ID_MASK        0x30000
#define IMX_CCM_ACCESS_CTRL_OWNER_ID(v)          (((v) << 0x10) & 0x30000)
/* Field MUTEX -  */
#define IMX_CCM_ACCESS_CTRL_MUTEX_SHIFT          20
#define IMX_CCM_ACCESS_CTRL_MUTEX_MASK           0x100000
#define IMX_CCM_ACCESS_CTRL_MUTEX(v)             (((v) << 0x14) & 0x100000)
/* Field DOMAIN0_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_WHITELIST_SHIFT 24
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_WHITELIST_MASK 0x1000000
#define IMX_CCM_ACCESS_CTRL_DOMAIN0_WHITELIST(v) (((v) << 0x18) & 0x1000000)
/* Field DOMAIN1_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_WHITELIST_SHIFT 25
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_WHITELIST_MASK 0x2000000
#define IMX_CCM_ACCESS_CTRL_DOMAIN1_WHITELIST(v) (((v) << 0x19) & 0x2000000)
/* Field DOMAIN2_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_WHITELIST_SHIFT 26
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_WHITELIST_MASK 0x4000000
#define IMX_CCM_ACCESS_CTRL_DOMAIN2_WHITELIST(v) (((v) << 0x1A) & 0x4000000)
/* Field DOMAIN3_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_WHITELIST_SHIFT 27
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_WHITELIST_MASK 0x8000000
#define IMX_CCM_ACCESS_CTRL_DOMAIN3_WHITELIST(v) (((v) << 0x1B) & 0x8000000)
/* Field SEMA_EN -  */
#define IMX_CCM_ACCESS_CTRL_SEMA_EN_SHIFT        28
#define IMX_CCM_ACCESS_CTRL_SEMA_EN_MASK         0x10000000
#define IMX_CCM_ACCESS_CTRL_SEMA_EN(v)           (((v) << 0x1C) & 0x10000000)
/* Field LOCK -  */
#define IMX_CCM_ACCESS_CTRL_LOCK_SHIFT           31
#define IMX_CCM_ACCESS_CTRL_LOCK_MASK            0x80000000
#define IMX_CCM_ACCESS_CTRL_LOCK(v)              (((v) << 0x1F) & 0x80000000)

/* Register ACCESS_CTRL_ROOT_SET - Access Control Register */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET             0x8074
/* Field DOMAIN0_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_INFO_SHIFT 0
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_INFO_MASK 0xF
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_INFO(v) (((v) << 0) & 0xF)
/* Field DOMAIN1_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_INFO_SHIFT 4
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_INFO_MASK 0xF0
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_INFO(v) (((v) << 0x4) & 0xF0)
/* Field DOMAIN2_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_INFO_SHIFT 8
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_INFO_MASK 0xF00
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_INFO(v) (((v) << 0x8) & 0xF00)
/* Field DOMAIN3_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_INFO_SHIFT 12
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_INFO_MASK 0xF000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_INFO(v) (((v) << 0xC) & 0xF000)
/* Field OWNER_ID -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_OWNER_ID_SHIFT 16
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_OWNER_ID_MASK 0x30000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_OWNER_ID(v) (((v) << 0x10) & 0x30000)
/* Field MUTEX -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_MUTEX_SHIFT 20
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_MUTEX_MASK  0x100000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_MUTEX(v)    (((v) << 0x14) & 0x100000)
/* Field DOMAIN0_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_WHITELIST_SHIFT 24
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_WHITELIST_MASK 0x1000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN0_WHITELIST(v) (((v) << 0x18) & 0x1000000)
/* Field DOMAIN1_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_WHITELIST_SHIFT 25
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_WHITELIST_MASK 0x2000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN1_WHITELIST(v) (((v) << 0x19) & 0x2000000)
/* Field DOMAIN2_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_WHITELIST_SHIFT 26
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_WHITELIST_MASK 0x4000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN2_WHITELIST(v) (((v) << 0x1A) & 0x4000000)
/* Field DOMAIN3_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_WHITELIST_SHIFT 27
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_WHITELIST_MASK 0x8000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_DOMAIN3_WHITELIST(v) (((v) << 0x1B) & 0x8000000)
/* Field SEMA_EN -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_SEMA_EN_SHIFT 28
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_SEMA_EN_MASK 0x10000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_SEMA_EN(v)  (((v) << 0x1C) & 0x10000000)
/* Field LOCK -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_LOCK_SHIFT  31
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_LOCK_MASK   0x80000000
#define IMX_CCM_ACCESS_CTRL_ROOT_SET_LOCK(v)     (((v) << 0x1F) & 0x80000000)

/* Register ACCESS_CTRL_ROOT_CLR - Access Control Register */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR             0x8078
/* Field DOMAIN0_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_INFO_SHIFT 0
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_INFO_MASK 0xF
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_INFO(v) (((v) << 0) & 0xF)
/* Field DOMAIN1_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_INFO_SHIFT 4
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_INFO_MASK 0xF0
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_INFO(v) (((v) << 0x4) & 0xF0)
/* Field DOMAIN2_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_INFO_SHIFT 8
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_INFO_MASK 0xF00
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_INFO(v) (((v) << 0x8) & 0xF00)
/* Field DOMAIN3_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_INFO_SHIFT 12
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_INFO_MASK 0xF000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_INFO(v) (((v) << 0xC) & 0xF000)
/* Field OWNER_ID -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_OWNER_ID_SHIFT 16
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_OWNER_ID_MASK 0x30000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_OWNER_ID(v) (((v) << 0x10) & 0x30000)
/* Field MUTEX -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_MUTEX_SHIFT 20
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_MUTEX_MASK  0x100000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_MUTEX(v)    (((v) << 0x14) & 0x100000)
/* Field DOMAIN0_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_WHITELIST_SHIFT 24
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_WHITELIST_MASK 0x1000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN0_WHITELIST(v) (((v) << 0x18) & 0x1000000)
/* Field DOMAIN1_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_WHITELIST_SHIFT 25
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_WHITELIST_MASK 0x2000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN1_WHITELIST(v) (((v) << 0x19) & 0x2000000)
/* Field DOMAIN2_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_WHITELIST_SHIFT 26
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_WHITELIST_MASK 0x4000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN2_WHITELIST(v) (((v) << 0x1A) & 0x4000000)
/* Field DOMAIN3_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_WHITELIST_SHIFT 27
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_WHITELIST_MASK 0x8000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_DOMAIN3_WHITELIST(v) (((v) << 0x1B) & 0x8000000)
/* Field SEMA_EN -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_SEMA_EN_SHIFT 28
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_SEMA_EN_MASK 0x10000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_SEMA_EN(v)  (((v) << 0x1C) & 0x10000000)
/* Field LOCK -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_LOCK_SHIFT  31
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_LOCK_MASK   0x80000000
#define IMX_CCM_ACCESS_CTRL_ROOT_CLR_LOCK(v)     (((v) << 0x1F) & 0x80000000)

/* Register ACCESS_CTRL_ROOT_TOG - Access Control Register */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG             0x807C
/* Field DOMAIN0_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_INFO_SHIFT 0
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_INFO_MASK 0xF
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_INFO(v) (((v) << 0) & 0xF)
/* Field DOMAIN1_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_INFO_SHIFT 4
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_INFO_MASK 0xF0
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_INFO(v) (((v) << 0x4) & 0xF0)
/* Field DOMAIN2_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_INFO_SHIFT 8
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_INFO_MASK 0xF00
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_INFO(v) (((v) << 0x8) & 0xF00)
/* Field DOMAIN3_INFO -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_INFO_SHIFT 12
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_INFO_MASK 0xF000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_INFO(v) (((v) << 0xC) & 0xF000)
/* Field OWNER_ID -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_OWNER_ID_SHIFT 16
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_OWNER_ID_MASK 0x30000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_OWNER_ID(v) (((v) << 0x10) & 0x30000)
/* Field MUTEX -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_MUTEX_SHIFT 20
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_MUTEX_MASK  0x100000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_MUTEX(v)    (((v) << 0x14) & 0x100000)
/* Field DOMAIN0_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_WHITELIST_SHIFT 24
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_WHITELIST_MASK 0x1000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN0_WHITELIST(v) (((v) << 0x18) & 0x1000000)
/* Field DOMAIN1_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_WHITELIST_SHIFT 25
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_WHITELIST_MASK 0x2000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN1_WHITELIST(v) (((v) << 0x19) & 0x2000000)
/* Field DOMAIN2_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_WHITELIST_SHIFT 26
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_WHITELIST_MASK 0x4000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN2_WHITELIST(v) (((v) << 0x1A) & 0x4000000)
/* Field DOMAIN3_WHITELIST -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_WHITELIST_SHIFT 27
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_WHITELIST_MASK 0x8000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_DOMAIN3_WHITELIST(v) (((v) << 0x1B) & 0x8000000)
/* Field SEMA_EN -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_SEMA_EN_SHIFT 28
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_SEMA_EN_MASK 0x10000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_SEMA_EN(v)  (((v) << 0x1C) & 0x10000000)
/* Field LOCK -  */
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_LOCK_SHIFT  31
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_LOCK_MASK   0x80000000
#define IMX_CCM_ACCESS_CTRL_ROOT_TOG_LOCK(v)     (((v) << 0x1F) & 0x80000000)

#endif /* IMX_CCM_H */
