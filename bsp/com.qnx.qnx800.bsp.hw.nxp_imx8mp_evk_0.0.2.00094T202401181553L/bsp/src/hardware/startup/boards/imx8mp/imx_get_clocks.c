/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2019-2020 NXP
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
 */

#include <stdbool.h>
#include <startup.h>
#include "imx_startup.h"
#include <soc/nxp/imx8/mp/imx_ccm.h>
#include <soc/nxp/imx8/mp/imx_ccm_analog.h>

/**
 * i.MX startup source file.
 *
 * @file       imx_get_clocks.c
 * @addtogroup startup
 * @{
 */

/* Clock source definitions */
#define IMX_OSC_24M             24000000UL
#define IMX_EXT_0_FREQ          0UL
#define IMX_EXT_1_FREQ          0UL
#define IMX_EXT_2_FREQ          0UL
#define IMX_EXT_3_FREQ          0UL
#define IMX_EXT_4_FREQ          0UL

#define ONE_MHZ                 1000000
#define ONE_KHZ                 1000

/* Round last digit during division */
#define SAFE_DIVIDE(A,B)         (((A) + ((B) - 1)) / (B))

/** Target clock root coding macros */
#define IMX_CCM_SOURCE_ROOT_MASK        0xFFFF0000UL
#define IMX_CCM_SOURCE_ROOT_SHIFT       16U
#define IMX_CCM_SET_SOURCE_ROOT(x)      ((x) << IMX_CCM_SOURCE_ROOT_SHIFT)
#define IMX_CCM_GET_SOURCE_ROOT(x)      (((x) & IMX_CCM_SOURCE_ROOT_MASK) >> IMX_CCM_SOURCE_ROOT_SHIFT)

#define IMX_CCM_SOURCE_DIVIDE_MASK      0xFFUL
#define IMX_CCM_SOURCE_DIVIDE_SHIFT     0U
#define IMX_CCM_SET_SOURCE_DIVIDE(x)    ((x) << IMX_CCM_SOURCE_DIVIDE_SHIFT)
#define IMX_CCM_GET_SOURCE_DIVIDE(x)    (((x) & IMX_CCM_SOURCE_DIVIDE_MASK) >> IMX_CCM_SOURCE_DIVIDE_SHIFT)

/** Module clock sources */
typedef enum {
    IMX_24M_REF,
    IMX_EXT_0,
    IMX_EXT_1,
    IMX_EXT_2,
    IMX_EXT_3,
    IMX_EXT_4,
    IMX_ARM_PLL,
    IMX_GPU_PLL,
    IMX_VPU_PLL,
    IMX_SYSTEM_PLL_1,
    IMX_SYSTEM_PLL_2,
    IMX_SYSTEM_PLL_3,
    IMX_AUDIO_PLL_1,
    IMX_AUDIO_PLL_2,
    IMX_DRAM_PLL,
    IMX_VIDEO_PLL
} imx_clk_source_t;

/** Clock sources from PLLs */
#define IMX_24M_REF_CLK             (IMX_CCM_SET_SOURCE_ROOT(IMX_24M_REF))
#define IMX_EXT_0_CLK               (IMX_CCM_SET_SOURCE_ROOT(IMX_EXT_0))
#define IMX_EXT_1_CLK               (IMX_CCM_SET_SOURCE_ROOT(IMX_EXT_1))
#define IMX_EXT_2_CLK               (IMX_CCM_SET_SOURCE_ROOT(IMX_EXT_2))
#define IMX_EXT_3_CLK               (IMX_CCM_SET_SOURCE_ROOT(IMX_EXT_3))
#define IMX_EXT_4_CLK               (IMX_CCM_SET_SOURCE_ROOT(IMX_EXT_4))
#define IMX_ARM_PLL_CLK             (IMX_CCM_SET_SOURCE_ROOT(IMX_ARM_PLL))
#define IMX_VPU_PLL_CLK             (IMX_CCM_SET_SOURCE_ROOT(IMX_VPU_PLL))
#define IMX_GPU_PLL_CLK             (IMX_CCM_SET_SOURCE_ROOT(IMX_GPU_PLL))
#define IMX_SYSTEM_PLL1_CLK         (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(1))
#define IMX_SYSTEM_PLL1_DIV2        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(2))
#define IMX_SYSTEM_PLL1_DIV3        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(3))
#define IMX_SYSTEM_PLL1_DIV4        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(4))
#define IMX_SYSTEM_PLL1_DIV5        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(5))
#define IMX_SYSTEM_PLL1_DIV6        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(6))
#define IMX_SYSTEM_PLL1_DIV8        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(8))
#define IMX_SYSTEM_PLL1_DIV10       (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(10))
#define IMX_SYSTEM_PLL1_DIV20       (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_1) | IMX_CCM_SET_SOURCE_DIVIDE(20))
#define IMX_SYSTEM_PLL2_CLK         (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(1))
#define IMX_SYSTEM_PLL2_DIV2        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(2))
#define IMX_SYSTEM_PLL2_DIV3        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(3))
#define IMX_SYSTEM_PLL2_DIV4        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(4))
#define IMX_SYSTEM_PLL2_DIV5        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(5))
#define IMX_SYSTEM_PLL2_DIV6        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(6))
#define IMX_SYSTEM_PLL2_DIV8        (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(8))
#define IMX_SYSTEM_PLL2_DIV10       (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(10))
#define IMX_SYSTEM_PLL2_DIV20       (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_2) | IMX_CCM_SET_SOURCE_DIVIDE(20))
#define IMX_SYSTEM_PLL3_CLK         (IMX_CCM_SET_SOURCE_ROOT(IMX_SYSTEM_PLL_3) | IMX_CCM_SET_SOURCE_DIVIDE(1))
#define IMX_AUDIO_PLL1_CLK          (IMX_CCM_SET_SOURCE_ROOT(IMX_AUDIO_PLL_1))
#define IMX_AUDIO_PLL2_CLK          (IMX_CCM_SET_SOURCE_ROOT(IMX_AUDIO_PLL_2))
#define IMX_VIDEO_PLL_CLK           (IMX_CCM_SET_SOURCE_ROOT(IMX_VIDEO_PLL))

typedef struct {
    uint8_t     target_idx;
    uint32_t    target_clock_sources[8];
} target_root_t;

/** Target root slices and clock source inputs list. */
static const target_root_t device_clock_root_list[] = {
        /* TARGET_ARM_A53, index 0 */
        {.target_idx = IMX_CCM_TARGET_ARM_A53,
        {IMX_24M_REF_CLK, IMX_ARM_PLL_CLK, IMX_SYSTEM_PLL2_DIV2, IMX_SYSTEM_PLL2_CLK, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_AUDIO_PLL1_CLK, IMX_SYSTEM_PLL3_CLK}},
        /* TARGET_UART1, index 1 */
        {.target_idx = IMX_CCM_TARGET_UART1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL3_CLK, IMX_EXT_2_CLK, IMX_EXT_4_CLK, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_UART2, index 2 */
        {.target_idx = IMX_CCM_TARGET_UART2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL3_CLK, IMX_EXT_2_CLK, IMX_EXT_4_CLK, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_UART3, index 3 */
        {.target_idx = IMX_CCM_TARGET_UART3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL3_CLK, IMX_EXT_2_CLK, IMX_EXT_4_CLK, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_UART4, index 4 */
        {.target_idx = IMX_CCM_TARGET_UART4,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL3_CLK, IMX_EXT_2_CLK, IMX_EXT_4_CLK, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_USDHC1, index 5 */
        {.target_idx = IMX_CCM_TARGET_USDHC1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL2_DIV2, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL1_DIV3, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV8}},
        /* TARGET_USDHC1, index 6 */
        {.target_idx = IMX_CCM_TARGET_USDHC2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL2_DIV2, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL1_DIV3, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV8}},
        /* TARGET_USDHC1, index 7 */
        {.target_idx = IMX_CCM_TARGET_USDHC3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL2_DIV2, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL1_DIV3, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV8}},
        /* TARGET_I2C1, index 8 */
        {.target_idx = IMX_CCM_TARGET_I2C1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL2_DIV20, IMX_SYSTEM_PLL3_CLK, IMX_AUDIO_PLL1_CLK, IMX_VIDEO_PLL_CLK, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV6}},
        /* TARGET_I2C2, index 9 */
        {.target_idx = IMX_CCM_TARGET_I2C2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL2_DIV20, IMX_SYSTEM_PLL3_CLK, IMX_AUDIO_PLL1_CLK, IMX_VIDEO_PLL_CLK, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV6}},
        /* TARGET_I2C3, index 10 */
        {.target_idx = IMX_CCM_TARGET_I2C3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL2_DIV20, IMX_SYSTEM_PLL3_CLK, IMX_AUDIO_PLL1_CLK, IMX_VIDEO_PLL_CLK, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV6}},
        /* TARGET_I2C4, index 11 */
        {.target_idx = IMX_CCM_TARGET_I2C3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL2_DIV20, IMX_SYSTEM_PLL3_CLK, IMX_AUDIO_PLL1_CLK, IMX_VIDEO_PLL_CLK, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV6}},
        /* TARGET_ECSPI1, index 12 */
        {.target_idx = IMX_CCM_TARGET_ECSPI1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL1_DIV20, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_ECSPI2, index 13 */
        {.target_idx = IMX_CCM_TARGET_ECSPI2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL1_DIV20, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_ECSPI3, index 14 */
        {.target_idx = IMX_CCM_TARGET_ECSPI3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL1_DIV20, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_AUDIO_PLL2_CLK}},
        /* TARGET_QSPI, index 15 */
        {.target_idx = IMX_CCM_TARGET_QSPI,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL2_DIV3, IMX_SYSTEM_PLL2_DIV2, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL1_DIV3, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL1_DIV8}},
        /* TARGET_NAND, index 16 */
        {.target_idx = IMX_CCM_TARGET_NAND,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV2, IMX_AUDIO_PLL1_CLK, IMX_SYSTEM_PLL1_DIV2, IMX_AUDIO_PLL2_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_VIDEO_PLL_CLK}},
        /* TARGET_GPT1, index 17 */
        {.target_idx = IMX_CCM_TARGET_GPT1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_1_CLK}},
        /* TARGET_GPT2, index 18 */
        {.target_idx = IMX_CCM_TARGET_GPT2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_2_CLK}},
        /* TARGET_GPT3, index 19 */
        {.target_idx = IMX_CCM_TARGET_GPT3,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_3_CLK}},
        /* TARGET_GPT4, index 20 */
        {.target_idx = IMX_CCM_TARGET_GPT4,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_1_CLK}},
        /* TARGET_GPT5, index 21 */
        {.target_idx = IMX_CCM_TARGET_GPT5,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_2_CLK}},
        /* TARGET_GPT6, index 22 */
        {.target_idx = IMX_CCM_TARGET_GPT6,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV10, IMX_SYSTEM_PLL1_DIV2, IMX_SYSTEM_PLL1_DIV20, IMX_VIDEO_PLL_CLK, IMX_SYSTEM_PLL1_DIV10, IMX_AUDIO_PLL1_CLK, IMX_EXT_3_CLK}},
        /* TARGET_CAN1, index 23 */
        {.target_idx = IMX_CCM_TARGET_CAN1,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL1_DIV20, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_AUDIO_PLL2_CLK,}},
        /* TARGET_CAN2, index 24 */
        {.target_idx = IMX_CCM_TARGET_CAN2,
        {IMX_24M_REF_CLK, IMX_SYSTEM_PLL2_DIV5, IMX_SYSTEM_PLL1_DIV20, IMX_SYSTEM_PLL1_DIV5, IMX_SYSTEM_PLL1_CLK, IMX_SYSTEM_PLL3_CLK, IMX_SYSTEM_PLL2_DIV4, IMX_AUDIO_PLL2_CLK,}},
};

typedef enum {
    IMX_ARM_A53_MODULE  = 0,
    IMX_UART1_MODULE    = 1,
    IMX_UART2_MODULE    = 2,
    IMX_UART3_MODULE    = 3,
    IMX_UART4_MODULE    = 4,
    IMX_USDHC1_MODULE   = 5,
    IMX_USDHC2_MODULE   = 6,
    IMX_USDHC3_MODULE   = 7,
    IMX_I2C1_MODULE     = 8,
    IMX_I2C2_MODULE     = 9,
    IMX_I2C3_MODULE     = 10,
    IMX_I2C4_MODULE     = 11,
    IMX_ECSPI1_MODULE   = 12,
    IMX_ECSPI2_MODULE   = 13,
    IMX_ECSPI3_MODULE   = 14,
    IMX_QSPI_MODULE     = 15,
    IMX_NAND_MODULE     = 16,
    IMX_GPT1_MODULE     = 17,
    IMX_GPT2_MODULE     = 18,
    IMX_GPT3_MODULE     = 19,
    IMX_GPT4_MODULE     = 20,
    IMX_GPT5_MODULE     = 21,
    IMX_GPT6_MODULE     = 22,
    IMX_CAN1_MODULE     = 23,
    IMX_CAN2_MODULE     = 24,
} imx_module_t;

/**
 * Return ARM PLL clock frequency.
 *
 * @return  ARM_PLL frequency (in herz).
 */
static uint32_t imx_get_arm_pll_freq(void)
{
    uint32_t val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL);
    uint32_t main_div, pre_div, post_div, freq;

    if ((val_cfg & IMX_CCM_ANALOG_ARM_PLL_GEN_CTRL_PLL_BYPASS_MASK) != 0U) {
        freq = IMX_OSC_24M;
    } else {
        val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0);
        main_div = ((val_cfg & IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_MAIN_DIV_MASK) >>
                   IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_MAIN_DIV_SHIFT);
        pre_div = ((val_cfg & IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_PRE_DIV_MASK) >>
                  IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_PRE_DIV_SHIFT);
        post_div = ((val_cfg & IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_POST_DIV_MASK) >>
                   IMX_CCM_ANALOG_ARM_PLL_FDIV_CTL0_PLL_POST_DIV_SHIFT);
        freq = (uint32_t)(((uint64_t)IMX_OSC_24M * main_div) / (pre_div * (0x01UL << post_div)));
    }
    return freq;
}

/**
 * Return GPU PLL clock frequency.
 *
 * @return  GPU_PLL frequency (in herz).
 */
static uint32_t imx_get_gpu_pll_freq(void)
{
    uint32_t val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL);
    uint32_t main_div, pre_div, post_div, freq;

    if ((val_cfg & IMX_CCM_ANALOG_GPU_PLL_GEN_CTRL_PLL_BYPASS_MASK) != 0U) {
        freq = IMX_OSC_24M;
    } else {
        val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0);
        main_div = ((val_cfg & IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_MAIN_DIV_MASK) >>
                   IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_MAIN_DIV_SHIFT);
        pre_div = ((val_cfg & IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_PRE_DIV_MASK) >>
                  IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_PRE_DIV_SHIFT);
        post_div = ((val_cfg & IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_POST_DIV_MASK) >>
                   IMX_CCM_ANALOG_GPU_PLL_FDIV_CTL0_PLL_POST_DIV_SHIFT);
        freq = (uint32_t)(((uint64_t)IMX_OSC_24M * main_div) / (pre_div * (0x01UL << post_div)));
    }
    return freq;
}

/**
 * Return VPU PLL clock frequency.
 *
 * @return  VPU_PLL frequency (in herz).
 */
static uint32_t imx_get_vpu_pll_freq(void)
{
    uint32_t val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL);
    uint32_t main_div, pre_div, post_div, freq;

    if ((val_cfg & IMX_CCM_ANALOG_VPU_PLL_GEN_CTRL_PLL_BYPASS_MASK) != 0U) {
        freq = IMX_OSC_24M;
    } else {
        val_cfg = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0);
        main_div = ((val_cfg & IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_MAIN_DIV_MASK) >>
                   IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_MAIN_DIV_SHIFT);
        pre_div = ((val_cfg & IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_PRE_DIV_MASK) >>
                  IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_PRE_DIV_SHIFT);
        post_div = ((val_cfg & IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_POST_DIV_MASK) >>
                   IMX_CCM_ANALOG_VPU_PLL_FDIV_CTL0_PLL_POST_DIV_SHIFT);
        freq = (uint32_t)(((uint64_t)IMX_OSC_24M * main_div) / (pre_div * (0x01UL << post_div)));
    }
    return freq;
}

/**
 * Return System PLL clock frequency.
 *
 * @param pll_num System PLL index.
 * @param div     System PLL divide factor.
 *
 * @return        System PLL frequency (in herz).
 */
static uint32_t imx_get_sys_pll_freq(const int pll_num, int div)
{
    uint32_t gen_ctrl_val;
    uint32_t fdiv_val, main_div, pre_div, post_div, freq = 0U, is_enabled;

    if ((pll_num == 1) || (pll_num == 2)) {
        gen_ctrl_val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL + ((pll_num - 1) * 0x70U));
        fdiv_val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0 + ((pll_num - 1) * 0x70U));
        switch (div) {
        case 0:
            is_enabled = TRUE;
            div = 1;
            break;
        case 1:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_CLKE_MASK) ? TRUE : FALSE);
            break;
        case 2:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV2_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 3:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV3_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 4:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV4_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 5:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV5_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 6:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV6_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 8:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV8_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 10:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV10_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        case 20:
            is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_DIV20_CLKE_OVERRIDE_MASK) ? TRUE : FALSE);
            break;
        default:
            is_enabled = FALSE;
            kprintf("Unsupported divider value %d of system %d PLL clock!\n", div, pll_num);
            break;
        }
    } else {
        if ((pll_num == 3) && ((div == 0) || (div == 1))) {
            gen_ctrl_val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL);
            fdiv_val = in32(IMX_CCM_ANALOG_BASE + IMX_CCM_ANALOG_SYS_PLL3_FDIV_CTL0);
            if (div == 0) {
                is_enabled = TRUE;
                div = 1;
            } else {
                is_enabled = ((gen_ctrl_val &  IMX_CCM_ANALOG_SYS_PLL3_GEN_CTRL_PLL_CLKE_MASK) ? TRUE : FALSE);
            }
        } else {
            is_enabled = FALSE;
            kprintf("Unsupported divider value %d of system %d PLL clock!\n", div, pll_num);
        }
    }
    if (is_enabled) {
        if ((gen_ctrl_val & IMX_CCM_ANALOG_SYS_PLL1_GEN_CTRL_PLL_BYPASS_MASK) != 0U) {
            freq = IMX_OSC_24M;
        } else {
            main_div = ((fdiv_val & IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_MAIN_DIV_MASK) >>
                       IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_MAIN_DIV_SHIFT);
            pre_div = ((fdiv_val & IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_PRE_DIV_MASK) >>
                      IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_PRE_DIV_SHIFT);
            post_div = ((fdiv_val & IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_POST_DIV_MASK) >>
                       IMX_CCM_ANALOG_SYS_PLL1_FDIV_CTL0_PLL_POST_DIV_SHIFT);
            freq = (uint32_t)(((uint64_t)IMX_OSC_24M * main_div) / (pre_div * (0x01UL << post_div) * div));
        }
    }
    return freq;
}

/**
 * Return PLL clock frequency in herz.
 *
 * @param type    PLL type (audio, video, dram).
 * @param pll_num Audio PLL index.
 *
 * @return        PLL frequency (in herz).
 */
static uint32_t imx_get_pll_freq(const imx_clk_source_t type, const int pll_num)
{
    uint32_t gen_ctrl_offset, fdiv_ctl0_offset, fdiv_ctl1_offset;
    uint32_t gen_ctrl_val, fdiv0_val, fdiv1_val, main_div, pre_div, post_div, freq = 0U, k;
    bool param_error = FALSE;

    switch (type) {
    case IMX_AUDIO_PLL_1:
    case IMX_AUDIO_PLL_2:
        if ((pll_num == 1) || (pll_num == 2)) {
            gen_ctrl_offset = IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL + ((pll_num - 1) * 0x14);
            fdiv_ctl0_offset = IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0 + ((pll_num - 1) * 0x14);
            fdiv_ctl1_offset = IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL1 + ((pll_num - 1) * 0x14);
        } else {
            param_error = TRUE;
        }
        break;
    case IMX_DRAM_PLL:
        if (pll_num == 0) {
            gen_ctrl_offset = IMX_CCM_ANALOG_DRAM_PLL_GEN_CTRL;
            fdiv_ctl0_offset = IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL0;
            fdiv_ctl1_offset = IMX_CCM_ANALOG_DRAM_PLL_FDIV_CTL1;
        } else {
            param_error = TRUE;kprintf("Unsupported DRAM %d PLL clock!\n", pll_num);
        }
        break;
    case IMX_VIDEO_PLL:
        if (pll_num == 0) {
            gen_ctrl_offset = IMX_CCM_ANALOG_VIDEO_PLL1_GEN_CTRL;
            fdiv_ctl0_offset = IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL0;
            fdiv_ctl1_offset = IMX_CCM_ANALOG_VIDEO_PLL1_FDIV_CTL1;
        } else {
            param_error = TRUE;
        }
        break;
    default:
        param_error = TRUE;
        break;
    }

    if (param_error == FALSE) {
        gen_ctrl_val = in32(IMX_CCM_ANALOG_BASE + gen_ctrl_offset);
        if ((gen_ctrl_val & IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_REF_CLK_SEL_MASK) != 0U) {
            freq = 0UL;
        } else {
            if ((gen_ctrl_val & IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_RST_MASK) == 0U) {
                freq = 0UL;
            } else {
                if ((gen_ctrl_val & IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_BYPASS_MASK) != 0U) {
                    freq = IMX_OSC_24M;
                } else {
                    if ((gen_ctrl_val & IMX_CCM_ANALOG_AUDIO_PLL1_GEN_CTRL_PLL_CLKE_MASK) == 0U) {
                        freq = 0UL;
                    } else {
                        fdiv0_val = in32(IMX_CCM_ANALOG_BASE + fdiv_ctl0_offset);
                        fdiv1_val = in32(IMX_CCM_ANALOG_BASE + fdiv_ctl1_offset);

                        main_div = ((fdiv0_val & IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_MAIN_DIV_MASK) >>
                                   IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_MAIN_DIV_SHIFT);
                        pre_div = ((fdiv0_val & IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_PRE_DIV_MASK) >>
                                  IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_PRE_DIV_SHIFT);
                        post_div = ((fdiv0_val & IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_POST_DIV_MASK) >>
                                   IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL0_PLL_POST_DIV_SHIFT);
                        k = (fdiv1_val & IMX_CCM_ANALOG_AUDIO_PLL1_FDIV_CTL1_PLL_DSM_MASK);
                        freq = (uint32_t)(((uint64_t)IMX_OSC_24M * ((main_div * 65536UL) + k)) / (65536UL * pre_div * (0x01UL << post_div)));
                    }
                }
            }
        }
    } else {
        kprintf("Parameters error! Unsupported %d, %d PLL clock!\n", type, pll_num);
    }

    return freq;
}

/**
 * Return specific module (target root) clock frequency.
 *
 * @param module Target root index.
 *
 * @return       Module clock frequency (in herz).
 */
static uint32_t imx_get_module_clk(const imx_module_t module_idx)
{
    uint32_t    clk_root_reg;
    int         module;
    uint32_t    module_clk_info, pre_podf, post_podf, inp_freq = 0U, freq = 0U;

    if (module_idx >= sizeof(device_clock_root_list)/sizeof(target_root_t)) {
        kprintf("Unsupported module index in device_clock_root_list structure!\n");
        return 0;
    }

    module = device_clock_root_list[module_idx].target_idx;
    clk_root_reg = in32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(module));
    module_clk_info = device_clock_root_list[module_idx].target_clock_sources[
                                  ((clk_root_reg & IMX_CCM_TARGET_ROOT_MUX_MASK) >> IMX_CCM_TARGET_ROOT_MUX_SHIFT)];
    /* Is enabled this clock? */
    if ((clk_root_reg & IMX_CCM_TARGET_ROOT_ENABLE_MASK) != 0) {
        /* Get Pre/Post divider values */
        pre_podf =  ((clk_root_reg & IMX_CCM_TARGET_ROOT_PRE_PODF_MASK) >> IMX_CCM_TARGET_ROOT_PRE_PODF_SHIFT);
        post_podf = ((clk_root_reg & IMX_CCM_TARGET_ROOT_POST_PODF_MASK) >> IMX_CCM_TARGET_ROOT_POST_PODF_SHIFT);
        /* Decode & get clock module source */
        switch (IMX_CCM_GET_SOURCE_ROOT(module_clk_info)) {
        case IMX_24M_REF:
            inp_freq = IMX_OSC_24M;
            break;
        case IMX_EXT_0:
            inp_freq = IMX_EXT_0_FREQ;
            break;
        case IMX_EXT_1:
            inp_freq = IMX_EXT_1_FREQ;
            break;
        case IMX_EXT_2:
            inp_freq = IMX_EXT_2_FREQ;
            break;
        case IMX_EXT_3:
            inp_freq = IMX_EXT_3_FREQ;
            break;
        case IMX_EXT_4:
            inp_freq = IMX_EXT_4_FREQ;
            break;
        case IMX_ARM_PLL:
            inp_freq = imx_get_arm_pll_freq();
            break;
        case IMX_GPU_PLL:
            inp_freq = imx_get_gpu_pll_freq();
            break;
        case IMX_VPU_PLL:
            inp_freq = imx_get_vpu_pll_freq();
            break;
        case IMX_SYSTEM_PLL_1:
            inp_freq = imx_get_sys_pll_freq(1, IMX_CCM_GET_SOURCE_DIVIDE(module_clk_info));
            break;
        case IMX_SYSTEM_PLL_2:
            inp_freq = imx_get_sys_pll_freq(2, IMX_CCM_GET_SOURCE_DIVIDE(module_clk_info));
            break;
        case IMX_SYSTEM_PLL_3:
            inp_freq = imx_get_sys_pll_freq(3, IMX_CCM_GET_SOURCE_DIVIDE(module_clk_info));
            break;
        case IMX_AUDIO_PLL_1:
            inp_freq = imx_get_pll_freq(IMX_AUDIO_PLL_1, IMX_CCM_GET_SOURCE_DIVIDE(module_clk_info));
            break;
        case IMX_AUDIO_PLL_2:
            inp_freq = imx_get_pll_freq(IMX_AUDIO_PLL_2, IMX_CCM_GET_SOURCE_DIVIDE(module_clk_info));
            break;
        case IMX_DRAM_PLL:
            inp_freq = imx_get_pll_freq(IMX_DRAM_PLL, 0);
            break;
        case IMX_VIDEO_PLL:
            inp_freq = imx_get_pll_freq(IMX_VIDEO_PLL, 0);
            break;
        default:
            kprintf("Unsupported clock source!\n");
            break;
        }
        freq = (inp_freq / (pre_podf + 1UL)) / (post_podf + 1UL);
    }
    return freq;
}

/**
 * Return Cortex-A53 core clock frequency.
 *
 * @return Cortex-A53 core clock frequency (in herz).
 */
uint32_t imx_get_cpu_clk(void)
{
    const uint32_t reg = in32(IMX_CCM_BASE + IMX_CCM_TARGET_ROOTn(IMX_CCM_TARGET_CORE_SEL_CFG));
    /* Check whether the ROOT select is ARM PLL or IMX_CCM_TARGET_ARM_A53 */
    if ((reg & IMX_CCM_TARGET_ROOT_MUX_MASK) == 0U) {
        return imx_get_module_clk(IMX_ARM_A53_MODULE);
    } else {
        return imx_get_arm_pll_freq();
    }
}

/**
 * Print chip information and input clock module speed.
 */
void imx_dump_clocks(imx_startup_data_t *startup_data)
{
    if (debug_flag) {
        kprintf("\n");
        print_chip_info();
        kprintf("\n");
        /* Print PLL frequency */
        kprintf("MCU PLL clock configuration:\n");
        kprintf("   ARM_PLL      : %dMHz\n", SAFE_DIVIDE(imx_get_arm_pll_freq(), ONE_MHZ));
        kprintf("   DRAM_PLL     : %dMHz\n", SAFE_DIVIDE(imx_get_pll_freq(IMX_DRAM_PLL, 0), ONE_MHZ));
        kprintf("   SYSTEM_PLL_1 : %dMHz\n", SAFE_DIVIDE(imx_get_sys_pll_freq(1, 0), ONE_MHZ));
        kprintf("   SYSTEM_PLL_2 : %dMHz\n", SAFE_DIVIDE(imx_get_sys_pll_freq(2, 0), ONE_MHZ));
        kprintf("   SYSTEM_PLL_3 : %dMHz\n", SAFE_DIVIDE(imx_get_sys_pll_freq(3, 0), ONE_MHZ));
        kprintf("   AUDIO_PLL_1  : %dMHz\n", SAFE_DIVIDE(imx_get_pll_freq(IMX_AUDIO_PLL_1, 1), ONE_MHZ));
        kprintf("   AUDIO_PLL_2  : %dMHz\n", SAFE_DIVIDE(imx_get_pll_freq(IMX_AUDIO_PLL_2, 2), ONE_MHZ));
        kprintf("   VIDEO_PLL    : %dMHz\n", SAFE_DIVIDE(imx_get_pll_freq(IMX_VIDEO_PLL, 0), ONE_MHZ));
        kprintf("   GPU_PLL      : %dMHz\n", SAFE_DIVIDE(imx_get_gpu_pll_freq(), ONE_MHZ));
        kprintf("   VPU_PLL      : %dMHz\n", SAFE_DIVIDE(imx_get_vpu_pll_freq(), ONE_MHZ));
        kprintf("\n");
        /* Print module frequency */
        kprintf("IP blocks clock configuration:\n");
        kprintf("   Cortex-A53 : %dMHz\n", SAFE_DIVIDE(imx_get_cpu_clk(), ONE_MHZ));
        kprintf("   UART1      : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_UART1_MODULE), ONE_KHZ));
        kprintf("   UART2      : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_UART2_MODULE), ONE_KHZ));
        kprintf("   UART3      : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_UART3_MODULE), ONE_KHZ));
        kprintf("   UART4      : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_UART4_MODULE), ONE_KHZ));
        kprintf("   USDHC1     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_USDHC1_MODULE), ONE_KHZ));
        kprintf("   USDHC2     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_USDHC2_MODULE), ONE_KHZ));
        kprintf("   USDHC3     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_USDHC3_MODULE), ONE_KHZ));
        kprintf("   I2C1       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_I2C1_MODULE), ONE_KHZ));
        kprintf("   I2C2       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_I2C2_MODULE), ONE_KHZ));
        kprintf("   I2C3       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_I2C3_MODULE), ONE_KHZ));
        kprintf("   I2C4       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_I2C4_MODULE), ONE_KHZ));
        kprintf("   ECSPI1     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_ECSPI1_MODULE), ONE_KHZ));
        kprintf("   ECSPI2     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_ECSPI2_MODULE), ONE_KHZ));
        kprintf("   ECSPI3     : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_ECSPI3_MODULE), ONE_KHZ));
        kprintf("   QSPI       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_QSPI_MODULE), ONE_KHZ));
        kprintf("   NAND       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_NAND_MODULE), ONE_KHZ));
        kprintf("   GPT1       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT1_MODULE), ONE_KHZ));
        kprintf("   GPT2       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT2_MODULE), ONE_KHZ));
        kprintf("   GPT3       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT3_MODULE), ONE_KHZ));
        kprintf("   GPT4       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT4_MODULE), ONE_KHZ));
        kprintf("   GPT5       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT5_MODULE), ONE_KHZ));
        kprintf("   GPT6       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_GPT6_MODULE), ONE_KHZ));
        kprintf("   CAN1       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_CAN1_MODULE), ONE_KHZ));
        kprintf("   CAN2       : %dkHz\n", SAFE_DIVIDE(imx_get_module_clk(IMX_CAN2_MODULE), ONE_KHZ));
        kprintf("\n");
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/imx_get_clocks.c $ $Rev: 985114 $")
#endif
