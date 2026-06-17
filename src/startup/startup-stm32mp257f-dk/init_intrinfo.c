/*
 * init_intrinfo.c — GIC-600 (GICv3) initialisation for STM32MP257F-DK
 *
 * GICD base  : 0x58000000
 * GICR base  : 0x580A0000  (stride 0x20000 per CPU, 2 CPUs)
 *
 * Reference: RM0457 §12 "General interrupt controller (GIC-600)"
 */

#include <startup.h>
#include <aarch64/gic.h>

#define STM32MP2_GICD_BASE      0x58000000UL
#define STM32MP2_GICR_BASE      0x580A0000UL
#define STM32MP2_GICR_STRIDE    0x20000UL
#define STM32MP2_NUM_CPUS       2

void
init_intrinfo(void)
{
    /*
     * gic_v3_init() maps the GIC registers, sets up the distributor,
     * and enumerates redistributors for each CPU.
     */
    gic_v3_init(STM32MP2_GICD_BASE,
                STM32MP2_GICR_BASE,
                STM32MP2_NUM_CPUS,
                STM32MP2_GICR_STRIDE);
}
