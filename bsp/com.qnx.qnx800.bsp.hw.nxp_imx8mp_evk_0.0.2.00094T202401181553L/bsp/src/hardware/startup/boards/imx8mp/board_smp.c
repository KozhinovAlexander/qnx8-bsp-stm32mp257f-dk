/*
 * Copyright (c) 2014,2022-2023, BlackBerry Limited.
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
#include "board.h"
#include "imx_startup.h"
#include <soc/nxp/imx8/common/imx_smc_call.h>


/**
 * i.MX startup source file.
 *
 * @file       board_smp.c
 * @addtogroup startup
 * @{
 */

typedef void send_ipi_t(struct syspage_entry *, unsigned, unsigned, unsigned *);
static const unsigned board_smp_max_cpu = IMX_MCU_CORES_NUMBER;
extern void board_mmu_disable(void);

static const uint64_t imx8x_core_affinity[6] = {
    0x0000,
    0x0001,
    0x0002,
    0x0003
};

uintptr_t   secondary_start;      /* Start address for cores waiting in cstart.S */
long        secondary_cpu;        /* CPU being woken up */

/**
 * Return CPU core number.
 *
 * @return CPU core number.
 */
unsigned board_smp_num_cpu(void)
{
    kprintf("board_smp_num_cpu: %d cores\n", board_smp_max_cpu);
    return board_smp_max_cpu;
}

/**
 * Perform any board specific SMP initialisation.
 *
 * @param smp      Pointer to smp_entry structure.
 * @param num_cpus CPU cores number.
 */
void board_smp_init(struct smp_entry *smp, const unsigned num_cpus)
{
    smp->send_ipi = (void *)&sendipi_gic_v3_sr;
}

/**
 * Initialize and start secondary CPU core.
 *
 * @param cpu   CPU core index.
 * @param start CPU reset address.
 *
 * @return  CPU core start status.
 * @retval  0   CPU core start failed.
 * @retval  1   Success, OK.
 */
int board_smp_start(const unsigned cpu, void (*start)(void))
{
    imx_smc_status_t status;
    /* Disable MMU */
    board_mmu_disable();
    /*
     * Secondary cores will be spinning in _start.S .
     */
    status = imx_sec_firmware_psci(IMX_PSCI_CPU_ON_AARCH64, imx8x_core_affinity[cpu], (uintptr_t)start, 0x00, 0x00);
    if (status != IMX_PSCI_SUCCESS) {
        crash("Cortex-A core %d start failed! Status: 0x%x. \n", cpu, status);
    }
    return 1;
}

/**
 * Perform any board/cpu-specific actions required to adjust the cpu number.
 *
 * @param cpu CPU core number.
 *
 * @return    Adjusted CPU core number.
 */
unsigned board_smp_adjust_num(const unsigned cpu)
{
    return cpu;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/startup/boards/imx8mp/board_smp.c $ $Rev: 984580 $")
#endif
