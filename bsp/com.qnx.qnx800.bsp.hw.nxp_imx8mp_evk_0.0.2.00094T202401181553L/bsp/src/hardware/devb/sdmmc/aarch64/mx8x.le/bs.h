/*
 * Copyright (c) 2013, 2023, BlackBerry Limited.
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2017-2019, 2022, NXP.
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
 *
 */

#ifndef BS_H_
#define BS_H_

/**
 * Board specific interface
 *
 * @file       bs.h
 * @addtogroup sdmmc_bs
 * @{
 */

#include <sys/utsname.h>

// add new chipset externs here
#define SDIO_HC_IMX8

#ifndef IMX_GPIO_SIZE
    #define IMX_GPIO_SIZE           0x10000
#endif

#define SDIO_SOC_SUPPORT
#define ADMA_SUPPORTED          1
#define SDHC_IPP_RESET_CTL      1
#define SDHC_CTRL_RESET_CTL     1
#define SDHC_REG_RESET_CTL      1
#define SDHC_BUS_SYNC           1

typedef enum _imx_usdhc_tuning_t {
    IMX_USDHC_TUNING_MANUAL     = 0,
    IMX_USDHC_TUNING_STANDARD   = 1,
} imx_usdhc_tuning_t;

/** Structure describing board specific configuration */
typedef struct _imx_ext {
    uint64_t            cd_pbase;       /**< CD I/O base physical address */
    uintptr_t           cd_base;        /**< CD I/O base mapped address */
    uint64_t            wp_pbase;       /**< WP I/O base physical address */
    uintptr_t           wp_base;        /**< WP I/O base mapped address */
    uint64_t            rs_pbase;       /**< Reset GPIO pin physical address */
    uintptr_t           rs_base;        /**< Reset GPIO pin mapped address */
    int                 nocd;           /**< If non-0, indicates CD is not supported */
    int                 cd_irq;         /**< CD GPIO IRQ */
    int                 cd_iid;         /**< CD GPIO interrupt id */
    int                 cd_pin;         /**< CD I/O bit number */
    int                 wp_pin;         /**< WP I/O bit number */
    int                 vdd1_8;         /**< 1.8V support */
    int                 rs_pin;         /**< Reset GPIO pin number */
} imx_ext_t;

/** @} */ /* End of sdmmc_bs */

#endif /* BS_H_ */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devb/sdmmc/aarch64/mx8x.le/bs.h $ $Rev: 976550 $")
#endif
