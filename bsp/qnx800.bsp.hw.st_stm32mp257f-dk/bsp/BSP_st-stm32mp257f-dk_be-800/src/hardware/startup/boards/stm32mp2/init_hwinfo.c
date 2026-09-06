/*
 * Copyright 2026, Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
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

/*
 * init_hwinfo.c
 * Tell syspage about our HW configuration
 */

#include <startup.h>
#include <hw/hwinfo_private.h>
#include <drvr/hwinfo.h>  /* For hwi support routines in libdrvr */
#include <libfdt.h>
#include "board.h"
#include "stm32mp2_startup.h"


/**
 * STM32MP2 startup source file.
 *
 * @file       init_hwinfo.c
 * @addtogroup startup
 * @{
 */

static char UID[STM32MP2_UID_SIZE_BYTES];

void stm32mp2_get_UID(char* uid)
{
  if (uid == NULL) return;
  kprintf("0x%lx\n", (uint64_t)0x123456789ABCDEF);
}

const char* stm32mp2_get_PKG(void)
{
  const volatile uint64_t *pkg_addr = (uint64_t*)(STM32MP2_BSEC_BASE + STM32MP2_BSEC_FVR122);
  kprintf("---> pkg: 0x%x\n", (uint32_t)(*pkg_addr));
  return "TODO: implement reading the package type from the board";
}

const char* stm32mp2_get_PRN(void)
{
  volatile uint32_t* prn_addr = (volatile uint32_t*)(0x44000024UL);
  uint32_t prn = *prn_addr;

  kprintf("---> prn: 0x%x\n", prn);
  return "TODO: implement reading the device part number from the board";
}

[[maybe_unused]]
static void get_eth_mac_address(uint8_t* const mac) {
    /* BC-05-DD-E4-D3-DC */
    mac[0] = 0xbc;
    mac[1] = 0x05;
    mac[2] = 0xdd;
    mac[3] = 0xe4;
    mac[4] = 0xd3;
    mac[5] = 0xdc;
}

 /**
  * Initialize the board type information in the syspage.
  * This function uses "Device electronic signature" (RM0457 Rev 5 5837/5881)
  */
void init_hwinfo(void)
{
    const unsigned hwi_bus_internal = 0;

    /* Add  UART2 */
    {
        unsigned hwi_off;
        hwiattr_uart_t attr = HWIATTR_UART_T_INITIALIZER;
        struct hwi_inputclk clksrc = {.clk = STM32MP2_UART_CLOCK_FREQ, .div = 1};
        HWIATTR_UART_SET_NUM_IRQ(&attr, 1);
        HWIATTR_UART_SET_NUM_CLK(&attr, 1);
        HWIATTR_UART_SET_NUM_DMA(&attr, 2);
        /* Create UART2 */
        HWIATTR_UART_SET_LOCATION(&attr, STM32_USART2_BASE_ADDR, STM32_UART_SIZE, 0, hwi_find_as(STM32_USART2_BASE_ADDR, 1));
        hwi_off = hwidev_add_uart("uart", &attr, hwi_bus_internal);
        ASSERT(hwi_off != HWI_NULL_OFF);
        hwitag_set_ivec(hwi_off, 0, STM32MP2_UART2_IRQ);
        hwitag_set_inputclk(hwi_off, 0, &clksrc);
        // hwitag_set_dma(hwi_off, 0, IMX_DMA_REQ_LPUART0_RX);
        // hwitag_set_dma(hwi_off, 1, IMX_DMA_REQ_LPUART0_TX);
    }

    /* Add ENET */
    {
        unsigned i, hwi_off;
        uint8_t mac[6] = { 0 };
        hwiattr_enet_t attr = HWIATTR_ENET_T_INITIALIZER;
        hwiattr_common_t common_attr = HWIATTR_COMMON_INITIALIZER;
        HWIATTR_ENET_SET_NUM_IRQ(&attr, 1);

        uint32_t irqs[] = {
            STM32MP2_ETH1_MAC_IRQ
        };

        HWIATTR_SET_NUM_IRQ(&common_attr, NUM_ELTS(irqs));

        /* Create ETH1 */
        HWIATTR_ENET_SET_LOCATION(&attr, STM32MP2_ETH1_MAC_BASE, STM32MP2_ETH1_MAC_SIZE, 0, hwi_find_as(STM32MP2_ETH1_MAC_BASE, 1));
        hwi_off = hwidev_add_enet(STM32MP2_ETH_HWI_SGEM, &attr, hwi_bus_internal);
        hwitag_add_common(hwi_off, &attr);
        ASSERT(hwi_find_unit(hwi_off) == 0);

        /* Add IRQ number */
        for(i = 0; i < NUM_ELTS(irqs); i++) {
            hwitag_set_ivec(hwi_off, i, irqs[i]);
        }

        get_eth_mac_address(mac);
        hwitag_add_nicaddr(hwi_off, mac, sizeof(mac));
    }

  /* Board type and silicon version */
  {
    // board_rev = get_board_revision();
    add_typed_string(_CS_MACHINE, "STM32MP257F-DK Board (MB1605 Var1.0 Rev.C-01)");  /* Name of the hardware type on which the system is running */
    add_typed_string(_CS_HW_PROVIDER, "STMicroelectronics");
    add_typed_string(_CS_ARCHITECTURE, "unknown architecture");  /* Name of the instructions set architechure */
    add_typed_string(_CS_HW_SERIAL, UID);  /* A serial number assiciated with the hardware */

    // add_typed_string(_CS_HOSTNAME, "unknown hostname");  /* Name of this node within the communications network */
    // add_typed_string(_CS_RELEASE, "unknown release");  /* Current release level of this implementation */
    // add_typed_string(_CS_VERSION, "unknown version");  /* Current version of this release */
    add_typed_string(_CS_HW_PROVIDER, "STMicroelectronics");  /* The name of the hardware manufacturers */
    // add_typed_string(_CS_SYSNAME, "unknown sysname");  /* Name of this implementation of the operating system */
  }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/init_hwinfo.c $ $Rev: 985114 $")
#endif
