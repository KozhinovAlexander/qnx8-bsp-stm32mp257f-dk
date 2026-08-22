/*
 * $QNXLicenseC:
 * Copyright 2016, 2023, BlackBerry Limited. All rights reserved.
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

#ifndef IMX_LPWDOG_H_
#define IMX_LPWDOG_H_

/*
 * Low Power Watchdog Timer Registers, offset from base address
 */
#define IMX_LPWDOG_CS                  0x00
#define IMX_LPWDOG_CNT                 0x04
#define IMX_LPWDOG_TOVAL               0x08
#define IMX_LPWDOG_WIN                 0x0C

/*
 * Watchdog Control and Status Register (CS) bits *
 */
#define IMX_LPWDOG_CS_STOP_MASK                        (0x01U)
#define IMX_LPWDOG_CS_STOP_SHIFT                       (0U)
#define IMX_LPWDOG_CS_WAIT_MASK                        (0x02U)
#define IMX_LPWDOG_CS_WAIT_SHIFT                       (1U)
#define IMX_LPWDOG_CS_DBG_MASK                         (0x04U)
#define IMX_LPWDOG_CS_DBG_SHIFT                        (2U)
#define IMX_LPWDOG_CS_TST_MASK                         (0x18U)
#define IMX_LPWDOG_CS_TST_SHIFT                        (3U)
#define IMX_LPWDOG_CS_TST(x)                           (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_CS_TST_SHIFT)) & IMX_LPWDOG_CS_TST_MASK)
#define IMX_LPWDOG_CS_UPDATE_MASK                      (0x20U)
#define IMX_LPWDOG_CS_UPDATE_SHIFT                     (5U)
#define IMX_LPWDOG_CS_INT_MASK                         (0x40U)
#define IMX_LPWDOG_CS_INT_SHIFT                        (6U)
#define IMX_LPWDOG_CS_EN_MASK                          (0x80U)
#define IMX_LPWDOG_CS_EN_SHIFT                         (7U)
#define IMX_LPWDOG_CS_CLK_MASK                         (0x300U)
#define IMX_LPWDOG_CS_CLK_SHIFT                        (8U)
#define IMX_LPWDOG_CS_CLK(x)                           (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_CS_CLK_SHIFT)) & IMX_LPWDOG_CS_CLK_MASK)
#define IMX_LPWDOG_CS_RCS_MASK                         (0x400U)
#define IMX_LPWDOG_CS_RCS_SHIFT                        (10U)
#define IMX_LPWDOG_CS_ULK_MASK                         (0x800U)
#define IMX_LPWDOG_CS_ULK_SHIFT                        (11U)
#define IMX_LPWDOG_CS_PRES_MASK                        (0x1000U)
#define IMX_LPWDOG_CS_PRES_SHIFT                       (12U)
#define IMX_LPWDOG_CS_CMD32EN_MASK                     (0x2000U)
#define IMX_LPWDOG_CS_CMD32EN_SHIFT                    (13U)
#define IMX_LPWDOG_CS_FLG_MASK                         (0x4000U)
#define IMX_LPWDOG_CS_FLG_SHIFT                        (14U)
#define IMX_LPWDOG_CS_WIN_MASK                         (0x8000U)
#define IMX_LPWDOG_CS_WIN_SHIFT                        (15U)

/*
 * Watchdog Counter Register (CNT) bits *
 */
#define IMX_LPWDOG_CNT_CNTLOW_MASK                     (0xFFU)
#define IMX_LPWDOG_CNT_CNTLOW_SHIFT                    (0U)
#define IMX_LPWDOG_CNT_CNTHIGH_MASK                    (0xFF00U)
#define IMX_LPWDOG_CNT_CNTHIGH_SHIFT                   (8U)

/*
 * Watchdog Timeout Value Register (TOVAL) bits *
 */
#define IMX_LPWDOG_TOVAL_TOVALLOW_MASK                 (0xFFU)
#define IMX_LPWDOG_TOVAL_TOVALLOW_SHIFT                (0U)
#define IMX_LPWDOG_TOVAL_TOVALLOW(x)                   (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_TOVAL_TOVALLOW_SHIFT)) & IMX_LPWDOG_TOVAL_TOVALLOW_MASK)
#define IMX_LPWDOG_TOVAL_TOVALHIGH_MASK                (0xFF00U)
#define IMX_LPWDOG_TOVAL_TOVALHIGH_SHIFT               (8U)
#define IMX_LPWDOG_TOVAL_TOVALHIGH(x)                  (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_TOVAL_TOVALHIGH_SHIFT)) & IMX_LPWDOG_TOVAL_TOVALHIGH_MASK)

/*
 * Watchdog Window Register (WIN) bits *
 */
#define IMX_LPWDOG_WIN_WINLOW_MASK                     (0xFFU)
#define IMX_LPWDOG_WIN_WINLOW_SHIFT                    (0U)
#define IMX_LPWDOG_WIN_WINLOW(x)                       (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_WIN_WINLOW_SHIFT)) & IMX_LPWDOG_WIN_WINLOW_MASK)
#define IMX_LPWDOG_WIN_WINHIGH_MASK                    (0xFF00U)
#define IMX_LPWDOG_WIN_WINHIGH_SHIFT                   (8U)
#define IMX_LPWDOG_WIN_WINHIGH(x)                      (((uint32_t)(((uint32_t)(x)) << IMX_LPWDOG_WIN_WINHIGH_SHIFT)) & IMX_LPWDOG_WIN_WINHIGH_MASK)

#endif /* IMX_LPWDOG_H_ */

