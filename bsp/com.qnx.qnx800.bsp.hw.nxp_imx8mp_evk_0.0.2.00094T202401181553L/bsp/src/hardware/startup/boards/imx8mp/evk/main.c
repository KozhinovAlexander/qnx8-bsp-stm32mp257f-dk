/*
 * Copyright (c) 2016, 2022-2023, BlackBerry Limited.
 * Copyright 2018-2019, 2022 NXP
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
 * NXP i.MX8MP EVK Board with Cortex-A53 cores
 */

#include <stdbool.h>
#include <time.h>
#include <startup.h>
#include "board.h"
#include "imx_startup.h"
#include "mp_evk_cpu.h"
#include "imx_i2c_drv.h"

/**
 * i.MX startup source file.
 *
 * @file       boards/imx8mp/evk/main.c
 * @addtogroup startup
 * @{
 */

/* Global startup data */
imx_startup_data_t startup_data;
/* Callout prototype */
extern struct callout_rtn reboot_imx;

const struct callout_slot callouts[] = {
    {
        CALLOUT_SLOT(reboot, _imx)
    },
};

/* The USB-Serial is on UART2 */
struct debug_device debug_devices[] = {
    {
        .name = "mx1",
        /* Debug port on IMX_UART2_BASE: 0x30890000 */
        .defaults[DEBUG_DEV_CONSOLE] = "0x30890000^0.115200.24000000.16",
        .defaults[DEBUG_DEV_KDEBUG] = NULL,
        .init = imx_init_uart,
        .put = imx_uart_put_char,
        .callouts[DEBUG_DISPLAY_CHAR] = &imx_uart_display_char,
        .callouts[DEBUG_POLL_KEY] = &imx_uart_poll_key,
        .callouts[DEBUG_BREAK_DETECT] = &imx_uart_break_detect,
    },
};

/**
 * Init IO Expander
 */
static void init_pca6416 (void) {
    imx_i2c_dev_t dev;
    uint8_t data;

    /* I2C3 base */
    dev.base = IMX_I2C3_BASE;
    dev.div = 0x10;
    /* PCA6416 slave address */
    dev.slave = 0x20;

    /* Initialize I2C channel */
    init_i2c_bus(&dev);

    /* Select BB_CAN_I2C5_SEL and BB_PDM_CAN2_SEL pins as outputs */
    data = 0x00;
    i2c_write(&dev, EXP_CONFIGURATION_PORT_0_REGISTER, &data);
#if IMX_FLEXCAN2_ENABLED
    /* Put BB_PDM_CAN2_SEL pin to high level */
    data = BB_PDM_CAN2_SEL;
    i2c_write(&dev, EXP_OUTPUT_PORT_0_REGISTER, &data);
#else
    /* Put BB_PDM_CAN2_SEL pin to low level */
    data = 0x00;
    i2c_write(&dev, EXP_OUTPUT_PORT_0_REGISTER, &data);
#endif
}

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
    int opt, options = 0;

    /* Initialize global startup structure */
    memset(&startup_data, 0x00, sizeof(startup_data));

    /* Initialize debugging console */
    startup_data.imx_uart_clock[1] = 24000000UL;
    select_debug(debug_devices, sizeof(debug_devices));

    add_callout_array(callouts, sizeof(callouts));

    /* Common options that should be avoided are:
       "AD:F:f:I:i:K:M:N:o:P:R:S:Tvr:j:Z" */
    while ((opt = getopt(argc, argv, COMMON_OPTIONS_STRING "W")) != -1) {
        switch (opt) {
            case 'W':
                options |= IMX_WDOG_ENABLE;
                break;
            default:
                handle_common_option(opt);
                break;
        }
    }

#if (IMX_CHECK_RESET == 1)
    /* Initialize WDOGs clock */
    imx_init_wdog_clock();
    /* Check and print source of the last reset */
    imx_check_reset_source();
#endif
    /* Enable WDT */
    if (options & IMX_WDOG_ENABLE) {
#if (IMX_CHECK_RESET == 0)
        imx_init_wdog_clock();
#endif
        imx_wdg_reload();
        imx_wdg_enable();
    }
    /* Get chip revision */
    startup_data.chip_rev = imx_get_chip_rev();
    /* Get chip type */
    startup_data.chip_type = (imx_get_chip_type() & IMX_MCU_TYPE_MASK);
    /* Collect information on all free RAM in the system */
    imx_init_raminfo();
    /* Remove RAM used by modules in the image */
    alloc_ram(shdr->ram_paddr, shdr->ram_size, 1);

    /* Enable Hypervisor if requested (and possible) */
    hypervisor_init(0);

    /* Initialize SMP */
    init_smp();
    /* Initialize MMU and caches */
    if (shdr->flags1 & STARTUP_HDR_FLAGS1_VIRTUAL) {
        init_mmu();
        imx_board_mmu_enable(IMX_SDRAM0_BASE, IMX_SDRAM0_SIZE);
        board_alignment_check_disable();
    }

    /* Initialize the Interrupts related Information */
    init_intrinfo();
    /* Initialize peripheral power and clock */
    if (imx_init_clocks(&startup_data) != 0) {
        crash("Initialization of peripheral clocks failed.");
    }
    /* Initialize the timer related information */
    init_qtime();
    imx_timer_init();
    /* Initialize cache controller */
    init_cacheattr();
    /* Initialize the CPU related information */
    init_cpuinfo();
    /* Initialize the Hwinfo section of the Syspage */
    imx_init_hwinfo(&startup_data);
    add_typed_string(_CS_MACHINE, "NXP i.MX8MP EVK Board");
    /* Initialize peripheral pads */
    if (imx_init_pads(&startup_data) != 0) {
        crash("Initialization of peripheral pads failed.");
    }

#if IMX_USB_INIT_ENABLED
    /* Init USB3 OTG1,OTG2 */
    imx_usb3_otg_host_init();
#endif

#if IMX_ENET_GET_MAC_ENABLED
    /* ENET MAC address initialization */
    imx_init_enet_mac_addr();
#endif
    /* Report peripheral clock frequencies */
    imx_dump_clocks(&startup_data);
    /*
     * Load bootstrap executables in the image file system and Initialize
     * various syspage pointers. This must be the _last_ initialization done
     * before transferring control to the next program.
     */
    init_system_private();
    /* Disable the cache and MMU */
    board_mmu_disable();
    /* Configure CAN */
    init_pca6416();
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
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/evk/main.c $ $Rev: 985114 $")
#endif
