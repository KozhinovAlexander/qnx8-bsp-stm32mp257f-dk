 /*
 * Copyright (c) 2016, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017, 2020, NXP
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
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef IMX_EDMA_REQUESTS_H_
#define IMX_EDMA_REQUESTS_H_

/**
 * @file        src/lib/dma/public/arm/imx/imx_edma_requests.h
 * @addtogroup  edma
 * @{
 */

#include <soc/nxp/imx8/common/imx_edma.h>


#define IMX_DMA0_CH_NUM                          32
#define IMX_DMA1_CH_NUM                          32
#define IMX_DMA2_CH_NUM                          32
#define IMX_DMA3_CH_NUM                          32

/** eDMA1 channel count on i.MX8QXP. */
#define IMX8XP_DMA1_CH_NUM                       16
/** eDMA3 channel count on i.MX8QXP. */
#define IMX8XP_DMA3_CH_NUM                       16
/** eDMA2 channel count on i.MX8DXL. */
#define IMX8DXL_DMA2_CH_NUM                      16

#define IMX_DMA0_ERRATA                          0
#define IMX_DMA1_ERRATA                          0
#define IMX_DMA2_ERRATA                          0
#define IMX_DMA3_ERRATA                          0

/* i.MX8 Common */
/* IMX_DMA0 */
#define IMX_DMA_REQ_LPSPI0_RX                    (imx_edma_define_request(0, 0, 448))
#define IMX_DMA_REQ_LPSPI0_TX                    (imx_edma_define_request(0, 1, 449))
#define IMX_DMA_REQ_LPSPI1_RX                    (imx_edma_define_request(0, 2, 450))
#define IMX_DMA_REQ_LPSPI1_TX                    (imx_edma_define_request(0, 3, 451))
#define IMX_DMA_REQ_LPSPI2_RX                    (imx_edma_define_request(0, 4, 452))
#define IMX_DMA_REQ_LPSPI2_TX                    (imx_edma_define_request(0, 5, 453))
#define IMX_DMA_REQ_LPSPI3_RX                    (imx_edma_define_request(0, 6, 454))
#define IMX_DMA_REQ_LPSPI3_TX                    (imx_edma_define_request(0, 7, 455))
#define IMX_DMA_REQ_EDMA0_EXT_INT_0              (imx_edma_define_request(0, 30, 274))
#define IMX_DMA_REQ_EDMA0_EXT_INT_1              (imx_edma_define_request(0, 31, 275))
/* IMX_DMA1 */
#define IMX_DMA_REQ_LPI2C0_RX                    (imx_edma_define_request(1, 0, 456))
#define IMX_DMA_REQ_LPI2C0_TX                    (imx_edma_define_request(1, 1, 457))
#define IMX_DMA_REQ_LPI2C1_RX                    (imx_edma_define_request(1, 2, 458))
#define IMX_DMA_REQ_LPI2C1_TX                    (imx_edma_define_request(1, 3, 459))
#define IMX_DMA_REQ_LPI2C2_RX                    (imx_edma_define_request(1, 4, 460))
#define IMX_DMA_REQ_LPI2C2_TX                    (imx_edma_define_request(1, 5, 461))
#define IMX_DMA_REQ_LPI2C3_RX                    (imx_edma_define_request(1, 6, 462))
#define IMX_DMA_REQ_LPI2C3_TX                    (imx_edma_define_request(1, 7, 463))
/* IMX_DMA2 */
#define IMX_DMA_REQ_ASRC0_PAIR_A_INPUT           (imx_edma_define_request(2, 0, 406))
#define IMX_DMA_REQ_ASRC0_PAIR_B_INPUT           (imx_edma_define_request(2, 1, 407))
#define IMX_DMA_REQ_ASRC0_PAIR_C_INPUT           (imx_edma_define_request(2, 2, 408))
#define IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT          (imx_edma_define_request(2, 3, 409))
#define IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT          (imx_edma_define_request(2, 4, 410))
#define IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT          (imx_edma_define_request(2, 5, 411))
#define IMX_DMA_REQ_ESAI0_RX                     (imx_edma_define_request(2, 6, 442))
#define IMX_DMA_REQ_ESAI0_TX                     (imx_edma_define_request(2, 7, 442))
#define IMX_DMA_REQ_SPDIF0_RX                    (imx_edma_define_request(2, 8, 489))
#define IMX_DMA_REQ_SPDIF0_TX                    (imx_edma_define_request(2, 9, 491))
#define IMX_DMA_REQ_SAI0_RX                      (imx_edma_define_request(2, 12, 347))
#define IMX_DMA_REQ_SAI0_TX                      (imx_edma_define_request(2, 13, 347))
#define IMX_DMA_REQ_SAI1_RX                      (imx_edma_define_request(2, 14, 349))
#define IMX_DMA_REQ_SAI1_TX                      (imx_edma_define_request(2, 15, 349))
#define IMX_DMA_REQ_SAI2_RX                      (imx_edma_define_request(2, 16, 351))
#define IMX_DMA_REQ_SAI3_RX                      (imx_edma_define_request(2, 17, 356))
#define IMX_DMA_REQ_GPT0_EC                      (imx_edma_define_request(2, 21, 423))
#define IMX_DMA_REQ_GPT1_EC                      (imx_edma_define_request(2, 22, 424))
#define IMX_DMA_REQ_GPT2_EC                      (imx_edma_define_request(2, 23, 425))
#define IMX_DMA_REQ_GPT3_EC                      (imx_edma_define_request(2, 24, 426))
#define IMX_DMA_REQ_GPT4_EC                      (imx_edma_define_request(2, 25, 427))
#define IMX_DMA_REQ_GPT5_EC                      (imx_edma_define_request(2, 26, 428))
/* IMX_DMA3 */
#define IMX_DMA_REQ_ASRC1_PAIR_A_INPUT           (imx_edma_define_request(3, 0, 414))
#define IMX_DMA_REQ_ASRC1_PAIR_B_INPUT           (imx_edma_define_request(3, 1, 415))
#define IMX_DMA_REQ_ASRC1_PAIR_C_INPUT           (imx_edma_define_request(3, 2, 416))
#define IMX_DMA_REQ_ASRC1_PAIR_A_OUTPUT          (imx_edma_define_request(3, 3, 417))
#define IMX_DMA_REQ_ASRC1_PAIR_B_OUTPUT          (imx_edma_define_request(3, 4, 418))
#define IMX_DMA_REQ_ASRC1_PAIR_C_OUTPUT          (imx_edma_define_request(3, 5, 419))
#define IMX_DMA_REQ_SW                           0x80000000
#define IMX_DMA_REQ_NONE                         0xFFFFFFFF

/* i.MX8M Specific */
#define IMX_DMA_REQ_RESERVED1_MX8M               (imx_edma_define_request(0, 8, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED2_MX8M               (imx_edma_define_request(0, 9, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED3_MX8M               (imx_edma_define_request(0, 10, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED4_MX8M               (imx_edma_define_request(0, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_LPUART0_RX_MX8M              (imx_edma_define_request(0, 12, 466))
#define IMX_DMA_REQ_LPUART0_TX_MX8M              (imx_edma_define_request(0, 13, 467))
#define IMX_DMA_REQ_LPUART1_RX_MX8M              (imx_edma_define_request(0, 14, 468))
#define IMX_DMA_REQ_LPUART1_TX_MX8M              (imx_edma_define_request(0, 15, 469))
#define IMX_DMA_REQ_LPUART2_RX_MX8M              (imx_edma_define_request(0, 16, 470))
#define IMX_DMA_REQ_LPUART2_TX_MX8M              (imx_edma_define_request(0, 17, 471))
#define IMX_DMA_REQ_LPUART3_RX_MX8M              (imx_edma_define_request(0, 18, 472))
#define IMX_DMA_REQ_LPUART3_TX_MX8M              (imx_edma_define_request(0, 19, 473))
#define IMX_DMA_REQ_LPUART4_RX_MX8M              (imx_edma_define_request(0, 20, 474))
#define IMX_DMA_REQ_LPUART4_TX_MX8M              (imx_edma_define_request(0, 21, 475))
#define IMX_DMA_REQ_RESERVED5_MX8M               (imx_edma_define_request(0, 22, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED6_MX8M               (imx_edma_define_request(0, 23, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED7_MX8M               (imx_edma_define_request(0, 24, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED8_MX8M               (imx_edma_define_request(0, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_EMVSIM0_RX_MX8M              (imx_edma_define_request(0, 26, 476))
#define IMX_DMA_REQ_EMVSIM0_TX_MX8M              (imx_edma_define_request(0, 27, 477))
#define IMX_DMA_REQ_EMVSIM1_RX_MX8M              (imx_edma_define_request(0, 28, 478))
#define IMX_DMA_REQ_EMVSIM1_TX_MX8M              (imx_edma_define_request(0, 29, 479))
#define IMX_DMA_REQ_LPI2C4_RX_MX8M               (imx_edma_define_request(1, 8, 464))
#define IMX_DMA_REQ_LPI2C4_TX_MX8M               (imx_edma_define_request(1, 9, 465))
#define IMX_DMA_REQ_RESERVED9_MX8M               (imx_edma_define_request(1, 10, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED10_MX8M              (imx_edma_define_request(1, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FLEXSPI0_RX_MX8M             (imx_edma_define_request(1, 12, 276))
#define IMX_DMA_REQ_FLEXSPI1_RX_MX8M             (imx_edma_define_request(1, 13, 277))
#define IMX_DMA_REQ_FLEXSPI0_TX_MX8M             (imx_edma_define_request(1, 14, 278))
#define IMX_DMA_REQ_FLEXSPI1_TX_MX8M             (imx_edma_define_request(1, 15, 279))
#define IMX_DMA_REQ_ADC0_MX8M                    (imx_edma_define_request(1, 16, 396))
#define IMX_DMA_REQ_RESERVED11_MX8M              (imx_edma_define_request(1, 17, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_ADC1_MX8M                    (imx_edma_define_request(1, 18, 397))
#define IMX_DMA_REQ_RESERVED12_MX8M              (imx_edma_define_request(1, 19, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FTM0_MX8M                    (imx_edma_define_request(1, 20, 394))
#define IMX_DMA_REQ_RESERVED13_MX8M              (imx_edma_define_request(1, 21, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FTM1_MX8M                    (imx_edma_define_request(1, 22, 395))
#define IMX_DMA_REQ_RESERVED14_MX8M              (imx_edma_define_request(1, 23, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED15_MX8M              (imx_edma_define_request(1, 24, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED16_MX8M              (imx_edma_define_request(1, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FLEXCAN0_MX8M                (imx_edma_define_request(1, 26, 391))
#define IMX_DMA_REQ_RESERVED17_MX8M              (imx_edma_define_request(1, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FLEXCAN1_MX8M                (imx_edma_define_request(1, 28, 392))
#define IMX_DMA_REQ_RESERVED18_MX8M              (imx_edma_define_request(1, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FLEXCAN2_MX8M                (imx_edma_define_request(1, 30, 393))
#define IMX_DMA_REQ_RESERVED19_MX8M              (imx_edma_define_request(1, 31, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_SPDIF1_RX_MX8M               (imx_edma_define_request(2, 10, 493))
#define IMX_DMA_REQ_SPDIF1_TX_MX8M               (imx_edma_define_request(2, 11, 495))
#define IMX_DMA_REQ_HDMI_RX_SAI0_RX_MX8M         (imx_edma_define_request(2, 18, 358))
#define IMX_DMA_REQ_HDMI_TX_SAI0_TX_MX8M         (imx_edma_define_request(2, 19, 360))
#define IMX_DMA_REQ_RESERVED20_MX8M              (imx_edma_define_request(2, 20, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED21_MX8M              (imx_edma_define_request(2, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED22_MX8M              (imx_edma_define_request(2, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED23_MX8M              (imx_edma_define_request(2, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED24_MX8M              (imx_edma_define_request(2, 30, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED25_MX8M              (imx_edma_define_request(2, 31, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_ESAI1_RX_MX8M                (imx_edma_define_request(3, 6, 444))
#define IMX_DMA_REQ_ESAI1_TX_MX8M                (imx_edma_define_request(3, 7, 444))
#define IMX_DMA_REQ_SAI6_RX_MX8M                 (imx_edma_define_request(3, 8, 362))
#define IMX_DMA_REQ_SAI6_TX_MX8M                 (imx_edma_define_request(3, 9, 362))
#define IMX_DMA_REQ_SAI7_TX_MX8M                 (imx_edma_define_request(3, 10, 364))
#define IMX_DMA_REQ_RESERVED26_MX8M              (imx_edma_define_request(3, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED27_MX8M              (imx_edma_define_request(3, 12, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED28_MX8M              (imx_edma_define_request(3, 13, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED29_MX8M              (imx_edma_define_request(3, 14, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED30_MX8M              (imx_edma_define_request(3, 15, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED31_MX8M              (imx_edma_define_request(3, 16, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED32_MX8M              (imx_edma_define_request(3, 17, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED33_MX8M              (imx_edma_define_request(3, 18, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED34_MX8M              (imx_edma_define_request(3, 19, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED35_MX8M              (imx_edma_define_request(3, 20, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED36_MX8M              (imx_edma_define_request(3, 21, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED37_MX8M              (imx_edma_define_request(3, 22, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED38_MX8M              (imx_edma_define_request(3, 23, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED39_MX8M              (imx_edma_define_request(3, 24, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED40_MX8M              (imx_edma_define_request(3, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED41_MX8M              (imx_edma_define_request(3, 26, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED42_MX8M              (imx_edma_define_request(3, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED43_MX8M              (imx_edma_define_request(3, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED44_MX8M              (imx_edma_define_request(3, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED45_MX8M              (imx_edma_define_request(3, 30, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED46_MX8M              (imx_edma_define_request(3, 31, IMX_DMA_COMMON_IRQ))

/* i.MX8XP Specific */
#define IMX_DMA_REQ_LPUART0_RX_MX8XP             (imx_edma_define_request(0, 8, 466))
#define IMX_DMA_REQ_LPUART0_TX_MX8XP             (imx_edma_define_request(0, 9, 467))
#define IMX_DMA_REQ_LPUART1_RX_MX8XP             (imx_edma_define_request(0, 10, 468))
#define IMX_DMA_REQ_LPUART1_TX_MX8XP             (imx_edma_define_request(0, 11, 469))
#define IMX_DMA_REQ_LPUART2_RX_MX8XP             (imx_edma_define_request(0, 12, 470))
#define IMX_DMA_REQ_LPUART2_TX_MX8XP             (imx_edma_define_request(0, 13, 471))
#define IMX_DMA_REQ_LPUART3_RX_MX8XP             (imx_edma_define_request(0, 14, 472))
#define IMX_DMA_REQ_LPUART3_TX_MX8XP             (imx_edma_define_request(0, 15, 473))
#define IMX_DMA_REQ_EDMA0_EXT_INT2_MX8XP         (imx_edma_define_request(0, 16, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_EDMA0_EXT_INT3_MX8XP         (imx_edma_define_request(0, 17, 277))
#define IMX_DMA_REQ_EDMA0_EXT_INT4_MX8XP         (imx_edma_define_request(0, 18, 278))
#define IMX_DMA_REQ_EDMA0_EXT_INT5_MX8XP         (imx_edma_define_request(0, 19, 279))
#define IMX_DMA_REQ_LCDIF0_MX8XP                 (imx_edma_define_request(0, 20, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED1_MX8XP              (imx_edma_define_request(0, 21, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED2_MX8XP              (imx_edma_define_request(0, 22, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED3_MX8XP              (imx_edma_define_request(0, 23, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED4_MX8XP              (imx_edma_define_request(0, 24, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED5_MX8XP              (imx_edma_define_request(0, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED6_MX8XP              (imx_edma_define_request(0, 26, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED7_MX8XP              (imx_edma_define_request(0, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED8_MX8XP              (imx_edma_define_request(0, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED9_MX8XP              (imx_edma_define_request(0, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_ADC0_MX8XP                   (imx_edma_define_request(1, 8, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED10_MX8XP             (imx_edma_define_request(1, 9, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FTM0_MX8XP                   (imx_edma_define_request(1, 10, 394))
#define IMX_DMA_REQ_FTM1_MX8XP                   (imx_edma_define_request(1, 11, 395))
#define IMX_DMA_REQ_FLEXCAN0_MX8XP               (imx_edma_define_request(1, 12, 391))
#define IMX_DMA_REQ_FLEXCAN1_MX8XP               (imx_edma_define_request(1, 13, 392))
#define IMX_DMA_REQ_FLEXCAN2_MX8XP               (imx_edma_define_request(1, 14, 393))
#define IMX_DMA_REQ_RESERVED11_MX8XP             (imx_edma_define_request(1, 15, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED12_MX8XP             (imx_edma_define_request(2, 10, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED13_MX8XP             (imx_edma_define_request(2, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED14_MX8XP             (imx_edma_define_request(2, 18, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED15_MX8XP             (imx_edma_define_request(2, 19, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED16_MX8XP             (imx_edma_define_request(2, 20, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED17_MX8XP             (imx_edma_define_request(2, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED18_MX8XP             (imx_edma_define_request(2, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED19_MX8XP             (imx_edma_define_request(2, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED20_MX8XP             (imx_edma_define_request(2, 30, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED21_MX8XP             (imx_edma_define_request(2, 31, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED22_MX8XP             (imx_edma_define_request(3, 6, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED23_MX8XP             (imx_edma_define_request(3, 7, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_SAI4_RX_MX8XP                (imx_edma_define_request(3, 8, 362))
#define IMX_DMA_REQ_SAI4_TX_MX8XP                (imx_edma_define_request(3, 9, 362))
#define IMX_DMA_REQ_SAI5_TX_MX8XP                (imx_edma_define_request(3, 10, 364))
#define IMX_DMA_REQ_RESERVED24_MX8XP             (imx_edma_define_request(3, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED25_MX8XP             (imx_edma_define_request(3, 12, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED26_MX8XP             (imx_edma_define_request(3, 13, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED27_MX8XP             (imx_edma_define_request(3, 14, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED28_MX8XP             (imx_edma_define_request(3, 15, IMX_DMA_COMMON_IRQ))

static const unsigned imx8m_edma0_requests[IMX_DMA0_CH_NUM] = {
    IMX_DMA_REQ_LPSPI0_RX      ,
    IMX_DMA_REQ_LPSPI0_TX      ,
    IMX_DMA_REQ_LPSPI1_RX      ,
    IMX_DMA_REQ_LPSPI1_TX      ,
    IMX_DMA_REQ_LPSPI2_RX      ,
    IMX_DMA_REQ_LPSPI2_TX      ,
    IMX_DMA_REQ_LPSPI3_RX      ,
    IMX_DMA_REQ_LPSPI3_TX      ,
    IMX_DMA_REQ_RESERVED1_MX8M ,
    IMX_DMA_REQ_RESERVED2_MX8M ,
    IMX_DMA_REQ_RESERVED3_MX8M ,
    IMX_DMA_REQ_RESERVED4_MX8M ,
    IMX_DMA_REQ_LPUART0_RX_MX8M,
    IMX_DMA_REQ_LPUART0_TX_MX8M,
    IMX_DMA_REQ_LPUART1_RX_MX8M,
    IMX_DMA_REQ_LPUART1_TX_MX8M,
    IMX_DMA_REQ_LPUART2_RX_MX8M,
    IMX_DMA_REQ_LPUART2_TX_MX8M,
    IMX_DMA_REQ_LPUART3_RX_MX8M,
    IMX_DMA_REQ_LPUART3_TX_MX8M,
    IMX_DMA_REQ_LPUART4_RX_MX8M,
    IMX_DMA_REQ_LPUART4_TX_MX8M,
    IMX_DMA_REQ_RESERVED5_MX8M ,
    IMX_DMA_REQ_RESERVED6_MX8M ,
    IMX_DMA_REQ_RESERVED7_MX8M ,
    IMX_DMA_REQ_RESERVED8_MX8M ,
    IMX_DMA_REQ_EMVSIM0_RX_MX8M,
    IMX_DMA_REQ_EMVSIM0_TX_MX8M,
    IMX_DMA_REQ_EMVSIM1_RX_MX8M,
    IMX_DMA_REQ_EMVSIM1_TX_MX8M,
    IMX_DMA_REQ_EDMA0_EXT_INT_0,
    IMX_DMA_REQ_EDMA0_EXT_INT_1,
};

static const unsigned imx8m_edma1_requests[IMX_DMA1_CH_NUM] = {
    IMX_DMA_REQ_LPI2C0_RX       ,
    IMX_DMA_REQ_LPI2C0_TX       ,
    IMX_DMA_REQ_LPI2C1_RX       ,
    IMX_DMA_REQ_LPI2C1_TX       ,
    IMX_DMA_REQ_LPI2C2_RX       ,
    IMX_DMA_REQ_LPI2C2_TX       ,
    IMX_DMA_REQ_LPI2C3_RX       ,
    IMX_DMA_REQ_LPI2C3_TX       ,
    IMX_DMA_REQ_LPI2C4_RX_MX8M  ,
    IMX_DMA_REQ_LPI2C4_TX_MX8M  ,
    IMX_DMA_REQ_RESERVED9_MX8M  ,
    IMX_DMA_REQ_RESERVED10_MX8M ,
    IMX_DMA_REQ_FLEXSPI0_RX_MX8M,
    IMX_DMA_REQ_FLEXSPI1_RX_MX8M,
    IMX_DMA_REQ_FLEXSPI0_TX_MX8M,
    IMX_DMA_REQ_FLEXSPI1_TX_MX8M,
    IMX_DMA_REQ_ADC0_MX8M       ,
    IMX_DMA_REQ_RESERVED11_MX8M ,
    IMX_DMA_REQ_ADC1_MX8M       ,
    IMX_DMA_REQ_RESERVED12_MX8M ,
    IMX_DMA_REQ_FTM0_MX8M       ,
    IMX_DMA_REQ_RESERVED13_MX8M ,
    IMX_DMA_REQ_FTM1_MX8M       ,
    IMX_DMA_REQ_RESERVED14_MX8M ,
    IMX_DMA_REQ_RESERVED15_MX8M ,
    IMX_DMA_REQ_RESERVED16_MX8M ,
    IMX_DMA_REQ_FLEXCAN0_MX8M   ,
    IMX_DMA_REQ_RESERVED17_MX8M ,
    IMX_DMA_REQ_FLEXCAN1_MX8M   ,
    IMX_DMA_REQ_RESERVED18_MX8M ,
    IMX_DMA_REQ_FLEXCAN2_MX8M   ,
    IMX_DMA_REQ_RESERVED19_MX8M ,
};

static const unsigned imx8m_edma2_requests[IMX_DMA2_CH_NUM] = {
    IMX_DMA_REQ_ASRC0_PAIR_A_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_B_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_C_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT ,
    IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT ,
    IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT ,
    IMX_DMA_REQ_ESAI0_RX            ,
    IMX_DMA_REQ_ESAI0_TX            ,
    IMX_DMA_REQ_SPDIF0_RX           ,
    IMX_DMA_REQ_SPDIF0_TX           ,
    IMX_DMA_REQ_SPDIF1_RX_MX8M      ,
    IMX_DMA_REQ_SPDIF1_TX_MX8M      ,
    IMX_DMA_REQ_SAI0_RX             ,
    IMX_DMA_REQ_SAI0_TX             ,
    IMX_DMA_REQ_SAI1_RX             ,
    IMX_DMA_REQ_SAI1_TX             ,
    IMX_DMA_REQ_SAI2_RX             ,
    IMX_DMA_REQ_SAI3_RX             ,
    IMX_DMA_REQ_HDMI_RX_SAI0_RX_MX8M,
    IMX_DMA_REQ_HDMI_TX_SAI0_TX_MX8M,
    IMX_DMA_REQ_RESERVED20_MX8M     ,
    IMX_DMA_REQ_GPT0_EC             ,
    IMX_DMA_REQ_GPT1_EC             ,
    IMX_DMA_REQ_GPT2_EC             ,
    IMX_DMA_REQ_GPT3_EC             ,
    IMX_DMA_REQ_GPT4_EC             ,
    IMX_DMA_REQ_GPT5_EC             ,
    IMX_DMA_REQ_RESERVED21_MX8M     ,
    IMX_DMA_REQ_RESERVED22_MX8M     ,
    IMX_DMA_REQ_RESERVED23_MX8M     ,
    IMX_DMA_REQ_RESERVED24_MX8M     ,
    IMX_DMA_REQ_RESERVED25_MX8M     ,
};

static const unsigned imx8m_edma3_requests[IMX_DMA3_CH_NUM] = {
    IMX_DMA_REQ_ASRC1_PAIR_A_INPUT ,
    IMX_DMA_REQ_ASRC1_PAIR_B_INPUT ,
    IMX_DMA_REQ_ASRC1_PAIR_C_INPUT ,
    IMX_DMA_REQ_ASRC1_PAIR_A_OUTPUT,
    IMX_DMA_REQ_ASRC1_PAIR_B_OUTPUT,
    IMX_DMA_REQ_ASRC1_PAIR_C_OUTPUT,
    IMX_DMA_REQ_ESAI1_RX_MX8M      ,
    IMX_DMA_REQ_ESAI1_TX_MX8M      ,
    IMX_DMA_REQ_SAI6_RX_MX8M       ,
    IMX_DMA_REQ_SAI6_TX_MX8M       ,
    IMX_DMA_REQ_SAI7_TX_MX8M       ,
    IMX_DMA_REQ_RESERVED26_MX8M    ,
    IMX_DMA_REQ_RESERVED27_MX8M    ,
    IMX_DMA_REQ_RESERVED28_MX8M    ,
    IMX_DMA_REQ_RESERVED29_MX8M    ,
    IMX_DMA_REQ_RESERVED30_MX8M    ,
    IMX_DMA_REQ_RESERVED31_MX8M    ,
    IMX_DMA_REQ_RESERVED32_MX8M    ,
    IMX_DMA_REQ_RESERVED33_MX8M    ,
    IMX_DMA_REQ_RESERVED34_MX8M    ,
    IMX_DMA_REQ_RESERVED35_MX8M    ,
    IMX_DMA_REQ_RESERVED36_MX8M    ,
    IMX_DMA_REQ_RESERVED37_MX8M    ,
    IMX_DMA_REQ_RESERVED38_MX8M    ,
    IMX_DMA_REQ_RESERVED39_MX8M    ,
    IMX_DMA_REQ_RESERVED40_MX8M    ,
    IMX_DMA_REQ_RESERVED41_MX8M    ,
    IMX_DMA_REQ_RESERVED42_MX8M    ,
    IMX_DMA_REQ_RESERVED43_MX8M    ,
    IMX_DMA_REQ_RESERVED44_MX8M    ,
    IMX_DMA_REQ_RESERVED45_MX8M    ,
    IMX_DMA_REQ_RESERVED46_MX8M    ,
};

static const unsigned imx8xp_edma0_requests[IMX_DMA0_CH_NUM] = {
    IMX_DMA_REQ_LPSPI0_RX           ,
    IMX_DMA_REQ_LPSPI0_TX           ,
    IMX_DMA_REQ_LPSPI1_RX           ,
    IMX_DMA_REQ_LPSPI1_TX           ,
    IMX_DMA_REQ_LPSPI2_RX           ,
    IMX_DMA_REQ_LPSPI2_TX           ,
    IMX_DMA_REQ_LPSPI3_RX           ,
    IMX_DMA_REQ_LPSPI3_TX           ,
    IMX_DMA_REQ_LPUART0_RX_MX8XP    ,
    IMX_DMA_REQ_LPUART0_TX_MX8XP    ,
    IMX_DMA_REQ_LPUART1_RX_MX8XP    ,
    IMX_DMA_REQ_LPUART1_TX_MX8XP    ,
    IMX_DMA_REQ_LPUART2_RX_MX8XP    ,
    IMX_DMA_REQ_LPUART2_TX_MX8XP    ,
    IMX_DMA_REQ_LPUART3_RX_MX8XP    ,
    IMX_DMA_REQ_LPUART3_TX_MX8XP    ,
    IMX_DMA_REQ_EDMA0_EXT_INT2_MX8XP,
    IMX_DMA_REQ_EDMA0_EXT_INT3_MX8XP,
    IMX_DMA_REQ_EDMA0_EXT_INT4_MX8XP,
    IMX_DMA_REQ_EDMA0_EXT_INT5_MX8XP,
    IMX_DMA_REQ_LCDIF0_MX8XP        ,
    IMX_DMA_REQ_RESERVED1_MX8XP     ,
    IMX_DMA_REQ_RESERVED2_MX8XP     ,
    IMX_DMA_REQ_RESERVED3_MX8XP     ,
    IMX_DMA_REQ_RESERVED4_MX8XP     ,
    IMX_DMA_REQ_RESERVED5_MX8XP     ,
    IMX_DMA_REQ_RESERVED6_MX8XP     ,
    IMX_DMA_REQ_RESERVED7_MX8XP     ,
    IMX_DMA_REQ_RESERVED8_MX8XP     ,
    IMX_DMA_REQ_RESERVED9_MX8XP     ,
    IMX_DMA_REQ_EDMA0_EXT_INT_0     ,
    IMX_DMA_REQ_EDMA0_EXT_INT_1     ,
};
static const unsigned imx8xp_edma1_requests[IMX8XP_DMA1_CH_NUM] = {
    IMX_DMA_REQ_LPI2C0_RX       ,
    IMX_DMA_REQ_LPI2C0_TX       ,
    IMX_DMA_REQ_LPI2C1_RX       ,
    IMX_DMA_REQ_LPI2C1_TX       ,
    IMX_DMA_REQ_LPI2C2_RX       ,
    IMX_DMA_REQ_LPI2C2_TX       ,
    IMX_DMA_REQ_LPI2C3_RX       ,
    IMX_DMA_REQ_LPI2C3_TX       ,
    IMX_DMA_REQ_ADC0_MX8XP      ,
    IMX_DMA_REQ_RESERVED10_MX8XP,
    IMX_DMA_REQ_FTM0_MX8XP      ,
    IMX_DMA_REQ_FTM1_MX8XP      ,
    IMX_DMA_REQ_FLEXCAN0_MX8XP  ,
    IMX_DMA_REQ_FLEXCAN1_MX8XP  ,
    IMX_DMA_REQ_FLEXCAN2_MX8XP  ,
    IMX_DMA_REQ_RESERVED11_MX8XP,
};
static const unsigned imx8xp_edma2_requests[IMX_DMA2_CH_NUM] = {
    IMX_DMA_REQ_ASRC0_PAIR_A_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_B_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_C_INPUT  ,
    IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT ,
    IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT ,
    IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT ,
    IMX_DMA_REQ_ESAI0_RX            ,
    IMX_DMA_REQ_ESAI0_TX            ,
    IMX_DMA_REQ_SPDIF0_RX           ,
    IMX_DMA_REQ_SPDIF0_TX           ,
    IMX_DMA_REQ_RESERVED12_MX8XP    ,
    IMX_DMA_REQ_RESERVED13_MX8XP    ,
    IMX_DMA_REQ_SAI0_RX             ,
    IMX_DMA_REQ_SAI0_TX             ,
    IMX_DMA_REQ_SAI1_RX             ,
    IMX_DMA_REQ_SAI1_TX             ,
    IMX_DMA_REQ_SAI2_RX             ,
    IMX_DMA_REQ_SAI3_RX             ,
    IMX_DMA_REQ_RESERVED14_MX8XP    ,
    IMX_DMA_REQ_RESERVED15_MX8XP    ,
    IMX_DMA_REQ_RESERVED16_MX8XP    ,
    IMX_DMA_REQ_GPT0_EC             ,
    IMX_DMA_REQ_GPT1_EC             ,
    IMX_DMA_REQ_GPT2_EC             ,
    IMX_DMA_REQ_GPT3_EC             ,
    IMX_DMA_REQ_GPT4_EC             ,
    IMX_DMA_REQ_GPT5_EC             ,
    IMX_DMA_REQ_RESERVED17_MX8XP    ,
    IMX_DMA_REQ_RESERVED18_MX8XP    ,
    IMX_DMA_REQ_RESERVED19_MX8XP    ,
    IMX_DMA_REQ_RESERVED20_MX8XP    ,
    IMX_DMA_REQ_RESERVED21_MX8XP    ,
};
static const unsigned imx8xp_edma3_requests[IMX8XP_DMA3_CH_NUM] = {
    IMX_DMA_REQ_ASRC1_PAIR_A_INPUT  ,
    IMX_DMA_REQ_ASRC1_PAIR_B_INPUT  ,
    IMX_DMA_REQ_ASRC1_PAIR_C_INPUT  ,
    IMX_DMA_REQ_ASRC1_PAIR_A_OUTPUT ,
    IMX_DMA_REQ_ASRC1_PAIR_B_OUTPUT ,
    IMX_DMA_REQ_ASRC1_PAIR_C_OUTPUT ,
    IMX_DMA_REQ_RESERVED22_MX8XP    ,
    IMX_DMA_REQ_RESERVED23_MX8XP    ,
    IMX_DMA_REQ_SAI4_RX_MX8XP       ,
    IMX_DMA_REQ_SAI4_TX_MX8XP       ,
    IMX_DMA_REQ_SAI5_TX_MX8XP       ,
    IMX_DMA_REQ_RESERVED24_MX8XP    ,
    IMX_DMA_REQ_RESERVED25_MX8XP    ,
    IMX_DMA_REQ_RESERVED26_MX8XP    ,
    IMX_DMA_REQ_RESERVED27_MX8XP    ,
    IMX_DMA_REQ_RESERVED28_MX8XP    ,
};

/* i.MX8DXL Specific */
/* IMX DMA0 */
#define IMX_DMA_REQ_ASRC0_PAIR_A_INPUT_MX8DXL     (imx_edma_define_request(0,  0, 294))
#define IMX_DMA_REQ_ASRC0_PAIR_B_INPUT_MX8DXL     (imx_edma_define_request(0,  1, 295))
#define IMX_DMA_REQ_ASRC0_PAIR_C_INPUT_MX8DXL     (imx_edma_define_request(0,  2, 296))
#define IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT_MX8DXL    (imx_edma_define_request(0,  3, 297))
#define IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT_MX8DXL    (imx_edma_define_request(0,  4, 298))
#define IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT_MX8DXL    (imx_edma_define_request(0,  5, 299))
#define IMX_DMA_REQ_RESERVED0_MX8DXL              (imx_edma_define_request(0,  6, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED1_MX8DXL              (imx_edma_define_request(0,  7, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_SPDIF0_RX_MX8DXL              (imx_edma_define_request(0,  8, 310))
#define IMX_DMA_REQ_SPDIF0_TX_MX8DXL              (imx_edma_define_request(0,  9, 311))
#define IMX_DMA_REQ_RESERVED2_MX8DXL              (imx_edma_define_request(0, 10, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED3_MX8DXL              (imx_edma_define_request(0, 11, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_SAI0_RX_MX8DXL                (imx_edma_define_request(0, 12, 221))
#define IMX_DMA_REQ_SAI0_TX_MX8DXL                (imx_edma_define_request(0, 13, 221))
#define IMX_DMA_REQ_SAI1_RX_MX8DXL                (imx_edma_define_request(0, 14, 223))
#define IMX_DMA_REQ_SAI1_TX_MX8DXL                (imx_edma_define_request(0, 15, 223))
#define IMX_DMA_REQ_SAI2_RX_MX8DXL                (imx_edma_define_request(0, 16, 225))
#define IMX_DMA_REQ_SAI3_RX_MX8DXL                (imx_edma_define_request(0, 17, 231))
#define IMX_DMA_REQ_RESERVED4_MX8DXL              (imx_edma_define_request(0, 18, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED5_MX8DXL              (imx_edma_define_request(0, 19, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED6_MX8DXL              (imx_edma_define_request(0, 20, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_GPT0_EC_MX8DXL                (imx_edma_define_request(0, 21, 300))
#define IMX_DMA_REQ_GPT1_EC_MX8DXL                (imx_edma_define_request(0, 22, 301))
#define IMX_DMA_REQ_GPT2_EC_MX8DXL                (imx_edma_define_request(0, 23, 302))
#define IMX_DMA_REQ_GPT3_EC_MX8DXL                (imx_edma_define_request(0, 24, 303))
#define IMX_DMA_REQ_RESERVED7_MX8DXL              (imx_edma_define_request(0, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED8_MX8DXL              (imx_edma_define_request(0, 26, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED9_MX8DXL              (imx_edma_define_request(0, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED10_MX8DXL             (imx_edma_define_request(0, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED11_MX8DXL             (imx_edma_define_request(0, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED12_MX8DXL             (imx_edma_define_request(0, 30, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED13_MX8DXL             (imx_edma_define_request(0, 31, IMX_DMA_COMMON_IRQ))

/* IMX DMA1 */
#define IMX_DMA_REQ_LPSPI0_RX_MX8DXL             (imx_edma_define_request(1,  0, 150))
#define IMX_DMA_REQ_LPSPI0_TX_MX8DXL             (imx_edma_define_request(1,  1, 150))
#define IMX_DMA_REQ_LPSPI1_RX_MX8DXL             (imx_edma_define_request(1,  2, 151))
#define IMX_DMA_REQ_LPSPI1_TX_MX8DXL             (imx_edma_define_request(1,  3, 151))
#define IMX_DMA_REQ_LPSPI2_RX_MX8DXL             (imx_edma_define_request(1,  4, 152))
#define IMX_DMA_REQ_LPSPI2_TX_MX8DXL             (imx_edma_define_request(1,  5, 152))
#define IMX_DMA_REQ_LPSPI3_RX_MX8DXL             (imx_edma_define_request(1,  6, 153))
#define IMX_DMA_REQ_LPSPI3_TX_MX8DXL             (imx_edma_define_request(1,  7, 153))
#define IMX_DMA_REQ_LPUART0_RX_MX8DXL            (imx_edma_define_request(1,  8, 158))
#define IMX_DMA_REQ_LPUART0_TX_MX8DXL            (imx_edma_define_request(1,  9, 158))
#define IMX_DMA_REQ_LPUART1_RX_MX8DXL            (imx_edma_define_request(1, 10, 159))
#define IMX_DMA_REQ_LPUART1_TX_MX8DXL            (imx_edma_define_request(1, 11, 159))
#define IMX_DMA_REQ_LPUART2_RX_MX8DXL            (imx_edma_define_request(1, 12, 160))
#define IMX_DMA_REQ_LPUART2_TX_MX8DXL            (imx_edma_define_request(1, 13, 160))
#define IMX_DMA_REQ_LPUART3_RX_MX8DXL            (imx_edma_define_request(1, 14, 161))
#define IMX_DMA_REQ_LPUART3_TX_MX8DXL            (imx_edma_define_request(1, 15, 161))
#define IMX_DMA_REQ_FLEXSPI0_RX_MX8DXL           (imx_edma_define_request(1, 16, 182))
#define IMX_DMA_REQ_FLEXSPI1_RX_MX8DXL           (imx_edma_define_request(1, 17, 183))
#define IMX_DMA_REQ_FLEXSPI0_TX_MX8DXL           (imx_edma_define_request(1, 18, 184))
#define IMX_DMA_REQ_FLEXSPI1_TX_MX8DXL           (imx_edma_define_request(1, 19, 185))
#define IMX_DMA_REQ_LCDIF0_MX8DXL                (imx_edma_define_request(1, 20, IMX_DMA_COMMON_IRQ)) // TODO IRQ ??
#define IMX_DMA_REQ_RESERVED14_MX8DXL            (imx_edma_define_request(1, 21, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED15_MX8DXL            (imx_edma_define_request(1, 22, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED16_MX8DXL            (imx_edma_define_request(1, 23, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED17_MX8DXL            (imx_edma_define_request(1, 24, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED18_MX8DXL            (imx_edma_define_request(1, 25, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED19_MX8DXL            (imx_edma_define_request(1, 26, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED20_MX8DXL            (imx_edma_define_request(1, 27, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED21_MX8DXL            (imx_edma_define_request(1, 28, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_RESERVED22_MX8DXL            (imx_edma_define_request(1, 29, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_EXTERNAL0_MX8DXL             (imx_edma_define_request(1, 30, 180))
#define IMX_DMA_REQ_EXTERNAL1_MX8DXL             (imx_edma_define_request(1, 31, 181))

/* IMX DMA2 */
#define IMX_DMA_REQ_LPI2C0_RX_MX8DXL             (imx_edma_define_request(2,  0, 154))
#define IMX_DMA_REQ_LPI2C0_TX_MX8DXL             (imx_edma_define_request(2,  1, 154))
#define IMX_DMA_REQ_LPI2C1_RX_MX8DXL             (imx_edma_define_request(2,  2, 155))
#define IMX_DMA_REQ_LPI2C1_TX_MX8DXL             (imx_edma_define_request(2,  3, 155))
#define IMX_DMA_REQ_LPI2C2_RX_MX8DXL             (imx_edma_define_request(2,  4, 156))
#define IMX_DMA_REQ_LPI2C2_TX_MX8DXL             (imx_edma_define_request(2,  5, 156))
#define IMX_DMA_REQ_LPI2C3_RX_MX8DXL             (imx_edma_define_request(2,  6, 157))
#define IMX_DMA_REQ_LPI2C3_TX_MX8DXL             (imx_edma_define_request(2,  7, 157))
#define IMX_DMA_REQ_ADC0_MX8DXL                  (imx_edma_define_request(2,  8, 178))
#define IMX_DMA_REQ_RESERVED23_MX8DXL            (imx_edma_define_request(2,  9, IMX_DMA_COMMON_IRQ))
#define IMX_DMA_REQ_FTM0_MX8DXL                  (imx_edma_define_request(2, 10, 176))
#define IMX_DMA_REQ_FTM1_MX8DXL                  (imx_edma_define_request(2, 11, 177))
#define IMX_DMA_REQ_FLEXCAN0_MX8DXL              (imx_edma_define_request(2, 12, 173))
#define IMX_DMA_REQ_FLEXCAN1_MX8DXL              (imx_edma_define_request(2, 13, 174))
#define IMX_DMA_REQ_FLEXCAN2_MX8DXL              (imx_edma_define_request(2, 14, 175))
#define IMX_DMA_REQ_RESERVED24_MX8DXL            (imx_edma_define_request(2, 15, IMX_DMA_COMMON_IRQ))

/* IMX8MP DMA1 */
#define IMX_DMA_REQ_RESERVED0_MX8MP              (imx_edma_define_request(0, 0, (132 + 32)))
#define IMX_DMA_REQ_RESERVED1_MX8MP              (imx_edma_define_request(0, 1, (132 + 32)))
#define IMX_DMA_REQ_RESERVED2_MX8MP              (imx_edma_define_request(0, 2, (132 + 32)))
#define IMX_DMA_REQ_RESERVED3_MX8MP              (imx_edma_define_request(0, 3, (132 + 32)))
#define IMX_DMA_REQ_RESERVED4_MX8MP              (imx_edma_define_request(0, 4, (132 + 32)))
#define IMX_DMA_REQ_RESERVED5_MX8MP              (imx_edma_define_request(0, 5, (132 + 32)))
#define IMX_DMA_REQ_RESERVED6_MX8MP              (imx_edma_define_request(0, 6, (132 + 32)))
#define IMX_DMA_REQ_RESERVED7_MX8MP              (imx_edma_define_request(0, 7, (132 + 32)))
#define IMX_DMA_REQ_RESERVED8_MX8MP              (imx_edma_define_request(0, 8, (132 + 32)))
#define IMX_DMA_REQ_RESERVED9_MX8MP              (imx_edma_define_request(0, 9, (132 + 32)))
#define IMX_DMA_REQ_RESERVED10_MX8MP             (imx_edma_define_request(0, 10, (132 + 32)))
#define IMX_DMA_REQ_RESERVED11_MX8MP             (imx_edma_define_request(0, 11, (132 + 32)))
#define IMX_DMA_REQ_RESERVED12_MX8MP             (imx_edma_define_request(0, 12, (132 + 32)))
#define IMX_DMA_REQ_RESERVED13_MX8MP             (imx_edma_define_request(0, 13, (132 + 32)))
#define IMX_DMA_REQ_RESERVED14_MX8MP             (imx_edma_define_request(0, 14, (132 + 32)))
#define IMX_DMA_REQ_RESERVED15_MX8MP             (imx_edma_define_request(0, 15, (132 + 32)))

#define IMX_DMA_REQ_RESERVED16_MX8MP             (imx_edma_define_request(0, 16, (133 + 32)))
#define IMX_DMA_REQ_RESERVED17_MX8MP             (imx_edma_define_request(0, 17, (133 + 32)))
#define IMX_DMA_REQ_RESERVED18_MX8MP             (imx_edma_define_request(0, 18, (133 + 32)))
#define IMX_DMA_REQ_RESERVED19_MX8MP             (imx_edma_define_request(0, 19, (133 + 32)))
#define IMX_DMA_REQ_RESERVED20_MX8MP             (imx_edma_define_request(0, 20, (133 + 32)))
#define IMX_DMA_REQ_RESERVED21_MX8MP             (imx_edma_define_request(0, 21, (133 + 32)))
#define IMX_DMA_REQ_RESERVED22_MX8MP             (imx_edma_define_request(0, 22, (133 + 32)))
#define IMX_DMA_REQ_RESERVED23_MX8MP             (imx_edma_define_request(0, 23, (133 + 32)))
#define IMX_DMA_REQ_RESERVED24_MX8MP             (imx_edma_define_request(0, 24, (133 + 32)))
#define IMX_DMA_REQ_RESERVED25_MX8MP             (imx_edma_define_request(0, 25, (133 + 32)))
#define IMX_DMA_REQ_RESERVED26_MX8MP             (imx_edma_define_request(0, 26, (133 + 32)))
#define IMX_DMA_REQ_RESERVED27_MX8MP             (imx_edma_define_request(0, 27, (133 + 32)))
#define IMX_DMA_REQ_RESERVED28_MX8MP             (imx_edma_define_request(0, 28, (133 + 32)))
#define IMX_DMA_REQ_RESERVED29_MX8MP             (imx_edma_define_request(0, 29, (133 + 32)))
#define IMX_DMA_REQ_RESERVED30_MX8MP             (imx_edma_define_request(0, 30, (133 + 32)))
#define IMX_DMA_REQ_RESERVED31_MX8MP             (imx_edma_define_request(0, 31, (133 + 32)))

static const unsigned imx8dxl_edma0_requests[IMX_DMA0_CH_NUM] = {
    IMX_DMA_REQ_ASRC0_PAIR_A_INPUT_MX8DXL  ,
    IMX_DMA_REQ_ASRC0_PAIR_B_INPUT_MX8DXL  ,
    IMX_DMA_REQ_ASRC0_PAIR_C_INPUT_MX8DXL  ,
    IMX_DMA_REQ_ASRC0_PAIR_A_OUTPUT_MX8DXL ,
    IMX_DMA_REQ_ASRC0_PAIR_B_OUTPUT_MX8DXL ,
    IMX_DMA_REQ_ASRC0_PAIR_C_OUTPUT_MX8DXL ,
    IMX_DMA_REQ_RESERVED0_MX8DXL           ,
    IMX_DMA_REQ_RESERVED1_MX8DXL           ,
    IMX_DMA_REQ_SPDIF0_RX_MX8DXL           ,
    IMX_DMA_REQ_SPDIF0_TX_MX8DXL           ,
    IMX_DMA_REQ_RESERVED2_MX8DXL           ,
    IMX_DMA_REQ_RESERVED3_MX8DXL           ,
    IMX_DMA_REQ_SAI0_RX_MX8DXL             ,
    IMX_DMA_REQ_SAI0_TX_MX8DXL             ,
    IMX_DMA_REQ_SAI1_RX_MX8DXL             ,
    IMX_DMA_REQ_SAI1_TX_MX8DXL             ,
    IMX_DMA_REQ_SAI2_RX_MX8DXL             ,
    IMX_DMA_REQ_SAI3_RX_MX8DXL             ,
    IMX_DMA_REQ_RESERVED4_MX8DXL           ,
    IMX_DMA_REQ_RESERVED5_MX8DXL           ,
    IMX_DMA_REQ_RESERVED6_MX8DXL           ,
    IMX_DMA_REQ_GPT0_EC_MX8DXL             ,
    IMX_DMA_REQ_GPT1_EC_MX8DXL             ,
    IMX_DMA_REQ_GPT2_EC_MX8DXL             ,
    IMX_DMA_REQ_GPT3_EC_MX8DXL             ,
    IMX_DMA_REQ_RESERVED7_MX8DXL           ,
    IMX_DMA_REQ_RESERVED8_MX8DXL           ,
    IMX_DMA_REQ_RESERVED9_MX8DXL           ,
    IMX_DMA_REQ_RESERVED10_MX8DXL          ,
    IMX_DMA_REQ_RESERVED11_MX8DXL          ,
    IMX_DMA_REQ_RESERVED12_MX8DXL          ,
    IMX_DMA_REQ_RESERVED13_MX8DXL
};

static const unsigned imx8dxl_edma1_requests[IMX_DMA1_CH_NUM] = {
    IMX_DMA_REQ_LPSPI0_RX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI0_TX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI1_RX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI1_TX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI2_RX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI2_TX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI3_RX_MX8DXL   ,
    IMX_DMA_REQ_LPSPI3_TX_MX8DXL   ,
    IMX_DMA_REQ_LPUART0_RX_MX8DXL  ,
    IMX_DMA_REQ_LPUART0_TX_MX8DXL  ,
    IMX_DMA_REQ_LPUART1_RX_MX8DXL  ,
    IMX_DMA_REQ_LPUART1_TX_MX8DXL  ,
    IMX_DMA_REQ_LPUART2_RX_MX8DXL  ,
    IMX_DMA_REQ_LPUART2_TX_MX8DXL  ,
    IMX_DMA_REQ_LPUART3_RX_MX8DXL  ,
    IMX_DMA_REQ_LPUART3_TX_MX8DXL  ,
    IMX_DMA_REQ_FLEXSPI0_RX_MX8DXL ,
    IMX_DMA_REQ_FLEXSPI1_RX_MX8DXL ,
    IMX_DMA_REQ_FLEXSPI0_TX_MX8DXL ,
    IMX_DMA_REQ_FLEXSPI1_TX_MX8DXL ,
    IMX_DMA_REQ_LCDIF0_MX8DXL      ,
    IMX_DMA_REQ_RESERVED14_MX8DXL  ,
    IMX_DMA_REQ_RESERVED15_MX8DXL  ,
    IMX_DMA_REQ_RESERVED16_MX8DXL  ,
    IMX_DMA_REQ_RESERVED17_MX8DXL  ,
    IMX_DMA_REQ_RESERVED18_MX8DXL  ,
    IMX_DMA_REQ_RESERVED19_MX8DXL  ,
    IMX_DMA_REQ_RESERVED20_MX8DXL  ,
    IMX_DMA_REQ_RESERVED21_MX8DXL  ,
    IMX_DMA_REQ_RESERVED22_MX8DXL  ,
    IMX_DMA_REQ_EXTERNAL0_MX8DXL   ,
    IMX_DMA_REQ_EXTERNAL1_MX8DXL
};

static const unsigned imx8dxl_edma2_requests[IMX8DXL_DMA2_CH_NUM] = {
    IMX_DMA_REQ_LPI2C0_RX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C0_TX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C1_RX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C1_TX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C2_RX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C2_TX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C3_RX_MX8DXL  ,
    IMX_DMA_REQ_LPI2C3_TX_MX8DXL  ,
    IMX_DMA_REQ_ADC0_MX8DXL       ,
    IMX_DMA_REQ_RESERVED23_MX8DXL ,
    IMX_DMA_REQ_FTM0_MX8DXL       ,
    IMX_DMA_REQ_FTM1_MX8DXL       ,
    IMX_DMA_REQ_FLEXCAN0_MX8DXL   ,
    IMX_DMA_REQ_FLEXCAN1_MX8DXL   ,
    IMX_DMA_REQ_FLEXCAN2_MX8DXL   ,
    IMX_DMA_REQ_RESERVED24_MX8DXL
};

static const unsigned imx8mp_edma1_requests[IMX_DMA1_CH_NUM] = {
    IMX_DMA_REQ_RESERVED0_MX8MP   ,
    IMX_DMA_REQ_RESERVED1_MX8MP   ,
    IMX_DMA_REQ_RESERVED2_MX8MP   ,
    IMX_DMA_REQ_RESERVED3_MX8MP   ,
    IMX_DMA_REQ_RESERVED4_MX8MP   ,
    IMX_DMA_REQ_RESERVED5_MX8MP   ,
    IMX_DMA_REQ_RESERVED6_MX8MP   ,
    IMX_DMA_REQ_RESERVED7_MX8MP   ,
    IMX_DMA_REQ_RESERVED8_MX8MP   ,
    IMX_DMA_REQ_RESERVED9_MX8MP   ,
    IMX_DMA_REQ_RESERVED10_MX8MP   ,
    IMX_DMA_REQ_RESERVED11_MX8MP   ,
    IMX_DMA_REQ_RESERVED12_MX8MP   ,
    IMX_DMA_REQ_RESERVED13_MX8MP   ,
    IMX_DMA_REQ_RESERVED14_MX8MP   ,
    IMX_DMA_REQ_RESERVED15_MX8MP   ,
    IMX_DMA_REQ_RESERVED16_MX8MP   ,
    IMX_DMA_REQ_RESERVED17_MX8MP   ,
    IMX_DMA_REQ_RESERVED18_MX8MP   ,
    IMX_DMA_REQ_RESERVED19_MX8MP   ,
    IMX_DMA_REQ_RESERVED20_MX8MP   ,
    IMX_DMA_REQ_RESERVED21_MX8MP   ,
    IMX_DMA_REQ_RESERVED22_MX8MP   ,
    IMX_DMA_REQ_RESERVED23_MX8MP   ,
    IMX_DMA_REQ_RESERVED24_MX8MP   ,
    IMX_DMA_REQ_RESERVED25_MX8MP   ,
    IMX_DMA_REQ_RESERVED26_MX8MP   ,
    IMX_DMA_REQ_RESERVED27_MX8MP   ,
    IMX_DMA_REQ_RESERVED28_MX8MP   ,
    IMX_DMA_REQ_RESERVED29_MX8MP   ,
    IMX_DMA_REQ_RESERVED30_MX8MP   ,
    IMX_DMA_REQ_RESERVED31_MX8MP   ,
};

typedef unsigned imx_edma_request_source_t;

/** @} */ /* End of edma */

#endif /* IMX_EDMA_REQUESTS_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/public/soc/nxp/imx8/common/imx_edma_requests.h $ $Rev: 965835 $")
#endif
