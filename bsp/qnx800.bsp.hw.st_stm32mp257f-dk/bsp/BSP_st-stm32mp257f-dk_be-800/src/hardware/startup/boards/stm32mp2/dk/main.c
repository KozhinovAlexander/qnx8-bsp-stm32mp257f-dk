/*
 * Copyright 2026 Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
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

/*
 * STMicroelectronics STM32MP257F-DK Board with Cortex-A35 cores
 */

#include <stdbool.h>
#include <time.h>
#include <startup.h>
#include "board.h"
#include "stm32mp2_startup.h"

/**
 * STM32MP257F startup source file.
 *
 * @file       boards/stm32mp2/dk/main.c
 * @addtogroup startup
 * @{
 */

#define STM32_WDT_ENABLE  (1 << 1)  /* watchdog enable */

/* Callout prototype */
extern struct callout_rtn reboot_stm32mp2;

const struct callout_slot callouts[] = {
    {
        CALLOUT_SLOT(reboot, _psci_smc)
    },
};

/* The USB-Serial is on UART2 */
struct debug_device debug_devices[] = {
    {
        .name = "stm32mp2_usart2",
        /* Debug port on STM32_USART2_BASE_ADDR: 0x400e0000 */
        .defaults[DEBUG_DEV_CONSOLE] = "0x400e0000^0.115200.64000000.16",
        .defaults[DEBUG_DEV_KDEBUG] = NULL,
        .init = stm32mp2_init_uart,
        .put = stm32mp2_uart_put_char,
        .callouts[DEBUG_DISPLAY_CHAR] = &stm32mp2_uart_display_char,
        .callouts[DEBUG_POLL_KEY] = &stm32mp2_uart_poll_key,
        .callouts[DEBUG_BREAK_DETECT] = &stm32mp2_uart_break_detect,
    },
};

/**
 *  Startup program executing out of RAM.
 *
 * 1. It gathers information about the system and places it in a structure
 *    called the system page. The kernel references this structure to
 *    determine everything it needs to know about the system. This structure
 *    is also available to user programs (read only if protection is on)
 *    via _syspage->.
 *
 * 2. It (optionally) turns on the MMU and starts the next program
 *    in the image file system.
 *
 * @param argc Count of the arguments supplied to the startup.
 * @param argv Array of pointers to the strings which are those arguments.
 * @param envv Environment variable.
 *
 * @return     Always 0.
 */
int main(const int argc, char **const argv, const char **const envv)
{
    int status, opt, options = 0;
    hyp_flags_t hyp_flags;  /* hypervisor flags */

    /* Setup hypervisor flags */
    hyp_flags = hypervisor_get_required_flags();
    hyp_flags |= HYP_FLAG_ENABLED;
    hypervisor_set_options(hyp_flags);

    /* Initialize debug interface. */
    select_debug(debug_devices, sizeof(debug_devices));

    add_callout_array(callouts, sizeof(callouts));

    kprintf("\n");
    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "W")) != -1) {
        switch (opt) {
            case 'W':
                options |= STM32_WDT_ENABLE;
                break;
            default:
                handle_common_option(opt);
                break;
        }
    }

    /* TF-A of stm32mp2 already providing watchdog and
     * it's interface to OS over SMCCC as arm,smc-wdt dts entity
     */
    status = stm32mp2_wdt_reset();
    kprintf("Reset watchdog. Status: %d\n", status);
    if ((options & STM32_WDT_ENABLE) != STM32_WDT_ENABLE) {
        status = stm32mp2_wdt_stop();
        kprintf("Disable watchdog. Status: %d\n", status);
    }

    /* Collect information on all free RAM in the system */
    stm32mp2_init_raminfo();

    /* Remove RAM used by modules in the image */
    alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

    /* Enable Hypervisor if requested (and possible) */
    hypervisor_init(0);

    init_smp();

    if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
        init_mmu();
         /* Enable the cache and MMU */
        stm32mp2_board_mmu_enable(STM32MP2_DRAM0_OS_BASE,
                                  STM32MP2_DRAM0_OS_SIZE);
        // board_alignment_check_disable();
    }

    // stm32mp2_init_pcie_ext_msi_controller();

     /* Initialize the Interrupts related Information */
    init_intrinfo();

    /* Initialize the timer related information */
    init_qtime();

    /* Initialize cache controller */
    init_cacheattr();

    /* Initialize the CPU related information */
    init_cpuinfo();

    /* Initialize the Hwinfo section of the Syspage */
    init_hwinfo();
    stm32mp2_init_hwinfo();

    // init_gpio_aon_bcm();

    /* TODO: Implement following functions (see: startup/lib/public/startup.h)*/
    // void init_raminfo(void);
    // void init_intrinfo(void);
    // void init_qtime(void);
    // void init_cacheattr(void);
    // void init_cpuinfo(void);
    // void init_hwinfo(void);
    // void init_asinfo(unsigned mem);
    // void init_nanospin(void);
    // void init_mitigation_mem(void);


    /*
     * Load bootstrap executables in the image file system and Initialize
     * various syspage pointers. This must be the _last_ initialization done
     * before transferring control to the next program.
     */
    init_system_private();

    /*
     * This is handy for debugging a new version of the startup program.
     * Commenting this line out will save a great deal of code.
     */
    print_syspage();
    return 0;
}

/** @} */ /* End of startup */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/stm32mp2/dk/main.c $ $Rev: 985114 $")
#endif
