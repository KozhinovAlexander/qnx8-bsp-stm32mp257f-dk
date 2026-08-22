/*
 * Copyright (c) 2016,2022-2023, BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2018-2019 NXP
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

#include <startup.h>
#include <hw/hwinfo_private.h>
#include <soc/nxp/imx8/common/hwinfo_imx8x.h>
#include <drvr/hwinfo.h>                /* For hwi support routines in libdrvr */
#include <soc/nxp/imx8/mp/mx8mp.h>
#include <soc/nxp/imx8/common/imx_edma_requests.h>
#include <soc/nxp/imx8/common/imx_edma.h>
#include <soc/nxp/imx8/common/imx_flexcan.h>
#include "board.h"
#include "imx_startup.h"
#include "mp_evk_cpu.h"

#if (IMX_FLEXCAN_INIT_ENABLED == 1)
/* Instead of hwibus_add_can() function use the following function to avoid empty tag creation
 * that happens inside hwibus_add_can(), as facing difficulty to fill up that empty tag
 */
static unsigned imx_hwibus_add_can(unsigned parent_hwi_off, hwiattr_can_t *attr);
#endif


/**
 * Add common device to the hwinfo table.
 *
 * @param device_name Name of a device used for find the device in hwinfo table
 * @param base        Hardware base address
 * @param size        Size of device address space
 * @param irq         Pointer to a variable or array with interrupt vector numbers
 * @param irq_count   Number of interrupt vectors in array
 * @param dma         Pointer to a variable or array with DMA requests
 * @param dma_count   Number of DMA requests in array
 *
 * @return            Always returns 0. Reserved for future use.
 */
static int imx_add_common_device(const char *const device_name, const paddr_t base, const uint32_t size, const uint32_t *const irq, const uint32_t irq_count,
                          const uint32_t *const dma, const uint32_t dma_count, const uint32_t *const errata, const uint32_t errata_count)
{
    unsigned hwi_off;
    const unsigned hwi_bus_internal = 0;
    unsigned i;
    hwiattr_common_t attr = HWIATTR_COMMON_INITIALIZER;
    HWIATTR_SET_NUM_IRQ(&attr, irq_count);
    HWIATTR_SET_NUM_DMA(&attr, dma_count);
    HWIATTR_SET_NUM_ERRATA(&attr, errata_count);
    HWIATTR_SET_LOCATION(&attr, base, size, 0, hwi_find_as(base, 1));
    /* Create a device with device_name */
    hwi_off = hwidev_add(device_name, 0, hwi_bus_internal);
    ASSERT(hwi_off != HWI_NULL_OFF);
    /* Assign attribute structure */
    hwitag_add_common(hwi_off, &attr);
    /* Assign IRQ numbers */
    for (i = 0; i < irq_count; i++) {
        if (hwitag_set_ivec(hwi_off, i, irq[i]) == 0) {
            crash("%s hwitag_set_ivec failed for IRQ index %u\n", device_name, i);
        }
    }
    /* Assign DMA requests */
    for (i = 0; i < dma_count; i++) {
        if (hwitag_set_dma(hwi_off, i, dma[i]) == 0) {
            crash("%s hwitag_set_dma failed for DMA index %u\n", device_name, i);
        }
    }
    /* Assign ERRATA */
    for (i = 0; i < errata_count; i++) {
        if (hwitag_set_errata(hwi_off, i, errata[i]) == 0) {
            crash("%s hwitag_set_errata failed for index %u\n", device_name, i);
        }
    }
    return 0;
}

/**
 * Add imx8 devices information (e.g.: base address, interrupt vector,...)
 * to the hardware info section of the syspage.
 *
 * @param startup_data Pointer to the startup data.
 */

void imx_init_hwinfo(imx_startup_data_t *const startup_data)
{
    const unsigned hwi_bus_internal = 0;

#if IMX_FLEXCAN_INIT_ENABLED
    {
        /* Add CAN buses */
        unsigned hwi_off;
        hwiattr_can_t attr = HWIATTR_CAN_T_INITIALIZER;
        HWIATTR_CAN_SET_NUM_IRQ(&attr, 2);
        HWIATTR_CAN_SET_NUM_MEMADDR(&attr, 1);

        /* Create CAN0 */
        HWIATTR_CAN_SET_LOCATION(&attr, IMX_FLEXCAN0_REG_BASE, IMX_FLEXCAN_REG_SIZE, 0, hwi_find_as(IMX_FLEXCAN0_REG_BASE, 1));
        hwi_off = imx_hwibus_add_can(hwi_bus_internal, &attr);
        hwitag_add_location(hwi_off, IMX_FLEXCAN0_MEM_BASE, IMX_FLEXCAN_MEM_SIZE, 0, 0);
        ASSERT(hwi_find_unit(hwi_off) == 0);
        hwitag_set_ivec(hwi_off, 0, IMX_FLEXCAN0_IRQ);
        hwitag_set_ivec(hwi_off, 1, IMX_FLEXCAN0_IRQ + 1);

#if IMX_FLEXCAN2_ENABLED
        /* Create CAN1 */
        HWIATTR_CAN_SET_LOCATION(&attr, IMX_FLEXCAN1_REG_BASE, IMX_FLEXCAN_REG_SIZE, 0, hwi_find_as(IMX_FLEXCAN1_REG_BASE, 1));
        hwi_off = imx_hwibus_add_can(hwi_bus_internal, &attr);
        hwitag_add_location(hwi_off, IMX_FLEXCAN1_MEM_BASE, IMX_FLEXCAN_MEM_SIZE, 0, 0);
        ASSERT(hwi_find_unit(hwi_off) == 1);
        hwitag_set_ivec(hwi_off, 0, IMX_FLEXCAN1_IRQ);
        hwitag_set_ivec(hwi_off, 1, IMX_FLEXCAN1_IRQ + 1);
#endif
    }
#endif

    /* Add  UART2 */
    {
        unsigned hwi_off;
        hwiattr_uart_t attr = HWIATTR_UART_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = startup_data->imx_uart_clock[1], .div = 1};
        HWIATTR_UART_SET_NUM_IRQ(&attr, 1);
        HWIATTR_UART_SET_NUM_CLK(&attr, 1);
        HWIATTR_UART_SET_NUM_DMA(&attr, 2);
        /* Create UART2 */
        HWIATTR_UART_SET_LOCATION(&attr, IMX_UART2_BASE, IMX_UART_SIZE, 0, hwi_find_as(IMX_UART2_BASE, 1));
        hwi_off = hwidev_add_uart(IMX_HWI_UART, &attr, hwi_bus_internal);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_ivec(hwi_off, 0, IMX_UART2_IRQ);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
//        hwitag_set_dma(hwi_off, 0, IMX_DMA_REQ_LPUART0_RX);
//        hwitag_set_dma(hwi_off, 1, IMX_DMA_REQ_LPUART0_TX);
    }

    /* Add FEC (ENET1 peripheral) */
    {
        unsigned hwi_off;
        hwiattr_enet_t attr = HWIATTR_ENET_T_INITIALIZER;
        HWIATTR_ENET_SET_NUM_IRQ(&attr, 1);

        /* Create FEC1 */
        HWIATTR_ENET_SET_LOCATION(&attr, IMX_ENET1_BASE, IMX_ENET1_MEM_SIZE, 0, hwi_find_as(IMX_ENET1_BASE, 1));
        hwi_off = hwidev_add_enet(IMX_HWI_LEGACY_FEC, &attr, hwi_bus_internal);
        ASSERT(hwi_find_unit(hwi_off) == 0);
        hwitag_set_avail_ivec(hwi_off, 0, IMX_ENET1_IRQ); /* Add ENET1 IRQ number */
        {
            hwi_tag *tag_gpt, *tag_gpt_irq;
            unsigned hwi_off_gpt;

            hwi_off_gpt = hwidev_add("fec0_gpt", 0, 0);
            ASSERT(hwi_off_gpt != HWI_NULL_OFF);
            if (hwi_off_gpt != HWI_NULL_OFF) {
                tag_gpt = hwi_alloc_tag(HWI_TAG_INFO(location));
                tag_gpt->location.base = IMX_GPT1_BASE;
                tag_gpt->location.len = IMX_GPT_SIZE;
                tag_gpt->location.regshift = 0;
                tag_gpt->location.addrspace = hwi_find_as(IMX_GPT1_BASE, 1);
                tag_gpt_irq = hwi_alloc_tag(HWI_TAG_INFO(irq));
                tag_gpt_irq->irq.vector = IMX_GPT1_IRQ;
            }
        }
    }

    /* Add ENET QoS */
    {
        unsigned hwi_off;
        hwiattr_enet_t attr = HWIATTR_ENET_T_INITIALIZER;
        HWIATTR_ENET_SET_NUM_IRQ(&attr, 1);

        /* Create DWC0 */
        HWIATTR_ENET_SET_LOCATION(&attr, IMX_ENET_QOS_BASE, IMX_ENET_QOS_MEM_SIZE, 0, hwi_find_as(IMX_ENET_QOS_BASE, 1));
        hwi_off = hwidev_add_enet("dwc", &attr, hwi_bus_internal);
        ASSERT(hwi_find_unit(hwi_off) == 0);

        /* Add IRQ number */
        hwitag_set_avail_ivec(hwi_off, 0, IMX_ENET_QOS_IRQ);
    }

    /* Add  USDHC1 */
    {
        unsigned hwi_off;
        hwiattr_sdio_t attr = HWIATTR_SDIO_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = startup_data->imx_usdhc_clk[0], .div = 1};
        HWIATTR_SDIO_SET_NUM_IRQ(&attr, 1);
        HWIATTR_SDIO_SET_NUM_CLK(&attr, 1);
        HWIATTR_SDIO_SET_DLL(&attr, "imx");
        /* Create USDHC1 */
        HWIATTR_SDIO_SET_LOCATION(&attr, IMX_USDHC1_BASE, IMX_USDHC_SIZE, 0, hwi_find_as(IMX_USDHC1_BASE, 1));
        hwi_off = hwibus_add_sdio(hwi_bus_internal, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_ivec(hwi_off, 0, IMX_USDHC1_IRQ);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
        hwi_add_synonym(hwi_off, "usdhc1");
    }
    /* Add  USDHC2 */
    {
        unsigned hwi_off;
        hwiattr_sdio_t attr = HWIATTR_SDIO_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = startup_data->imx_usdhc_clk[1], .div = 1};
        HWIATTR_SDIO_SET_NUM_IRQ(&attr, 1);
        HWIATTR_SDIO_SET_NUM_CLK(&attr, 1);
        HWIATTR_SDIO_SET_DLL(&attr, "imx");
        /* Create USDHC2 */
        HWIATTR_SDIO_SET_LOCATION(&attr, IMX_USDHC2_BASE, IMX_USDHC_SIZE, 0, hwi_find_as(IMX_USDHC2_BASE, 1));
        hwi_off = hwibus_add_sdio(hwi_bus_internal, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_ivec(hwi_off, 0, IMX_USDHC2_IRQ);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
        hwi_add_synonym(hwi_off, "usdhc2");
    }
    /* Add  USDHC3 */
    {
        unsigned hwi_off;
        hwiattr_sdio_t attr = HWIATTR_SDIO_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = startup_data->imx_usdhc_clk[2], .div = 1};
        HWIATTR_SDIO_SET_NUM_IRQ(&attr, 1);
        HWIATTR_SDIO_SET_NUM_CLK(&attr, 1);
        HWIATTR_SDIO_SET_DLL(&attr, "imx");
        /* Create USDHC2 */
        HWIATTR_SDIO_SET_LOCATION(&attr, IMX_USDHC3_BASE, IMX_USDHC_SIZE, 0, hwi_find_as(IMX_USDHC3_BASE, 1));
        hwi_off = hwibus_add_sdio(hwi_bus_internal, &attr);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_ivec(hwi_off, 0, IMX_USDHC3_IRQ);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
        hwi_add_synonym(hwi_off, "usdhc3");
    }
    /* Add the WATCHDOG device */
    {
        unsigned hwi_off;
        hwiattr_timer_t attr = HWIATTR_TIMER_T_INITIALIZER;
        const struct hwi_inputclk clksrc_kick = {.clk = 10, .div = 1};
        HWIATTR_TIMER_SET_NUM_CLK(&attr, 1);
        HWIATTR_TIMER_SET_LOCATION(&attr, IMX_WDOG1_BASE, IMX_WDOG_SIZE, 0, hwi_find_as(IMX_WDOG1_BASE, 1));
        hwi_off = hwidev_add_timer(IMX_HWI_WDOG, &attr,  HWI_NULL_OFF);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_inputclk(hwi_off, 0, (struct hwi_inputclk *)&clksrc_kick);

        hwi_off = hwidev_add("wdt,options", 0, HWI_NULL_OFF);
        hwitag_add_regname(hwi_off, "enable_width", 16);
        hwitag_add_regname(hwi_off, "write_width", 16);
        /* Uncomment for enable WDOG device by "wdtkick-imx8m" driver */
        /* hwitag_add_regname(hwi_off, "enable_mask", 0x4); */
        hwitag_add_regname(hwi_off, "enable_condition", 0x4);
        hwitag_add_regname(hwi_off, "enable_offset", 0x0);

        hwi_off = hwidev_add("wdt,regwrite", 0, HWI_NULL_OFF);
        hwitag_add_regname(hwi_off, "offset", 0x2);
        hwitag_add_regname(hwi_off, "value", 0x5555);
        hwitag_add_regname(hwi_off, "offset", 0x2);
        hwitag_add_regname(hwi_off, "value", 0xAAAA);
    }
    /* Add RTC devices */
    {
        hwi_add_rtc("mx8msrtc", IMX_SNVS_BASE, 0, IMX_SNVS_SIZE, 1, -1);
    }
    /* Add ATF status */
    {
        unsigned hwi_off;

        hwi_off = hwidev_add("smc_call", 0, HWI_NULL_OFF);
        ASSERT(hwi_off != HWI_NULL_OFF);
        const char *const optstr = "smc_call=yes";

        if (hwi_off != HWI_NULL_OFF) {
            hwitag_add_optstr(hwi_off, optstr);
        }
    }
    /* Add imx8 board and type and silicon version */
    {
        hwi_tag *tag;
        unsigned off;
        unsigned hwi_off;

        hwi_off = hwidev_add("board", 0, 0);
        ASSERT(hwi_off != HWI_NULL_OFF);
        if (hwi_off != HWI_NULL_OFF) {
            off = add_string(IMX_BOARD_INFO);
            tag = hwi_alloc_tag(HWI_TAG_INFO(hwversion));
            tag->hwversion.hclass = (_Uint8t)(startup_data->chip_type); /* hclass = imx chip type */
            tag->hwversion.version = (_Uint8t)(startup_data->chip_rev); /* version = imx chip version */
            tag->hwversion.name = (_Uint16t)off;
        }
    }
#if (IMX_AUDIO_INIT_ENABLED == 1)
    /* Add SAI1 */
    {
        const uint32_t irq = IMX_SAI1_IRQ;
        const uint32_t dma[] = { (IMX_SAI1_RX_EVENT << 16) | 4,
                           (IMX_SAI1_TX_EVENT << 16) | 3,
                          };
        imx_add_common_device(IMX_HWI_SAI, IMX_SAI1_BASE, IMX_SAI_SIZE, &irq, 1, dma,
                              sizeof(dma) / sizeof(uint32_t), NULL, 0);
    }
    /* Add SAI2 */
    {
        const uint32_t irq = IMX_SAI2_IRQ;
        const uint32_t dma[] = { (IMX_SAI2_RX_EVENT << 16) | 4,
                           (IMX_SAI2_TX_EVENT << 16) | 3,
                          };
        imx_add_common_device(IMX_HWI_SAI, IMX_SAI2_BASE, IMX_SAI_SIZE, &irq, 1, dma,
                              sizeof(dma) / sizeof(uint32_t), NULL, 0);
    }
    /* Add SAI3 */
    {
        const uint32_t irq = IMX_SAI3_IRQ;
        const uint32_t dma[] = { (IMX_SAI3_RX_EVENT << 16) | 4,
                           (IMX_SAI3_TX_EVENT << 16) | 3,
                          };
        imx_add_common_device(IMX_HWI_SAI, IMX_SAI3_BASE, IMX_SAI_SIZE, &irq, 1, dma,
                              sizeof(dma) / sizeof(uint32_t), NULL, 0);
    }
    /* Add SAI4 not supported by SoC */
    {
        const uint32_t irq = 0;
        imx_add_common_device(IMX_HWI_SAI, 0, 0, &irq, 1, NULL, 0, NULL, 0);
    }
    /* Add SAI5 */
    {
        const uint32_t irq = IMX_SAI5_IRQ;
        const uint32_t dma[] = { (IMX_SAI5_RX_EVENT << 16) | 4,
                           (IMX_SAI5_TX_EVENT << 16) | 3,
                          };
        imx_add_common_device(IMX_HWI_SAI, IMX_SAI5_BASE, IMX_SAI_SIZE, &irq, 1, dma,
                              sizeof(dma) / sizeof(uint32_t), NULL, 0);
    }
    /* Add SAI6 */
    {
        const uint32_t irq = IMX_SAI6_IRQ;
        const uint32_t dma[] = { (IMX_SAI6_RX_EVENT << 16) | 4,
                           (IMX_SAI6_TX_EVENT << 16) | 3,
                          };
        imx_add_common_device(IMX_HWI_SAI, IMX_SAI6_BASE, IMX_SAI_SIZE, &irq, 1, dma,
                              sizeof(dma) / sizeof(uint32_t), NULL, 0);
    }
    /* Add eDMA peripheral */
    {
        const uint32_t errata =  IMX_DMA1_ERRATA;
        const uint32_t irq[] = { IMX_EDMA1_CHN_LOW_IRQ,
                           IMX_EDMA1_ERROR_IRQ,
                           IMX_EDMA1_CHN_HI_IRQ,
                         };
        imx_add_common_device(IMX_HWI_DMA, IMX_EDMA1_BASE, IMX_DMA_DEVICE_SIZE, irq, 3, (imx_edma_request_source_t *)imx8mp_edma1_requests,
                IMX_DMA1_CH_NUM, &errata, 1);
    }
#endif
}

#if (IMX_FLEXCAN_INIT_ENABLED == 1)
static unsigned imx_hwibus_add_can(unsigned parent_hwi_off, hwiattr_can_t *attr)
{
    const unsigned hwi_off = hwibus_add(HWI_ITEM_BUS_CAN, parent_hwi_off);
    if ((hwi_off != HWI_NULL_OFF) && (attr != NULL))
    {
        unsigned i;
        hwitag_add_common(hwi_off, &attr->common);
        for (i=0; i<attr->num_clks; i++)
        {
            hwitag_add_inputclk(hwi_off, 0, 1);
        }
    }
    return hwi_off;
}
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/init_hwinfo.c $ $Rev: 985114 $")
#endif
