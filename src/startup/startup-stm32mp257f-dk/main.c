/*
 * QNX 8 Startup — STM32MP257F-DK
 * SoC  : STM32MP257FAK3
 * CPU  : dual Cortex-A35 (AArch64), GIC-600, ARM Generic Timer
 *
 * This startup code runs at EL1.  TF-A (BL31) has already:
 *   - initialised LPDDR4
 *   - set up the GIC in secure mode
 *   - written CNTFRQ_EL0 (25 MHz)
 * U-Boot has already:
 *   - enabled PLLs and peripheral clocks
 *   - configured pinmux for UART4
 */

#include <startup.h>
#include <hw/inout.h>
#include <aarch64/gic.h>

/* ------------------------------------------------------------------ */
/* Peripheral base addresses                                           */
/* ------------------------------------------------------------------ */
#define STM32MP2_UART4_BASE     0x40010000UL
#define STM32MP2_RCC_BASE       0x44200000UL
#define STM32MP2_GICD_BASE      0x58000000UL   /* GIC-600 distributor  */
#define STM32MP2_GICR_BASE      0x580A0000UL   /* GIC-600 redistributor*/
#define STM32MP2_GICR_STRIDE    0x20000UL      /* per-CPU stride       */
#define STM32MP2_NUM_CPUS       2

/* ARM Generic Timer counter frequency (written by TF-A) */
#define STM32MP2_TIMER_FREQ     25000000UL

/* UART4 source clock after PLL setup by U-Boot */
#define STM32MP2_UART4_CLK      64000000UL

/* ------------------------------------------------------------------ */
/* Callout forward declarations (callout_debug_stm32_uart.S)           */
/* ------------------------------------------------------------------ */
extern struct callout_rtn callout_debug_putc_stm32_uart;
extern struct callout_rtn callout_debug_getc_stm32_uart;
extern struct callout_rtn callout_debug_size_stm32_uart;

/* ------------------------------------------------------------------ */
/* Debug device descriptor                                             */
/* ------------------------------------------------------------------ */
static const struct debug_device debug_devs[] = {
    {
        .name   = "stm32-uart",
        .base   = { STM32MP2_UART4_BASE },
        .clock  = STM32MP2_UART4_CLK,
        .baud   = 115200,
        .putc   = &callout_debug_putc_stm32_uart,
        .getc   = &callout_debug_getc_stm32_uart,
        .size   = &callout_debug_size_stm32_uart,
    },
};

/* ================================================================== */
void
init_hardware(void)
{
    /*
     * Clock tree is already configured by TF-A + U-Boot.
     * Nothing to do here except confirm the debug UART is enabled
     * (U-Boot leaves UART4 running at 115200 8N1).
     */
}

/* ================================================================== */
void
main(int argc, char **argv, char **envv)
{
    /*
     * Register the STM32 USART debug callout first so that kprintf
     * works as early as possible.
     */
    add_callout_array(debug_devs, sizeof(debug_devs));

    /* Hardware init (clocks already set by TF-A/U-Boot) */
    init_hardware();

    /*
     * RAM: 4 GB LPDDR4 starting at 0x80000000.
     * The first 1 MB is reserved for TF-A/U-Boot artefacts.
     */
    init_raminfo();

    /*
     * GIC-600 (GICv3)
     *   GICD @ 0x58000000
     *   GICR @ 0x580A0000, stride 0x20000 per CPU
     */
    init_intrinfo();

    /*
     * SMP: 2x Cortex-A35.
     * Secondary CPU bring-up uses PSCI CPU_ON (SMC to TF-A BL31).
     */
    init_smp();

    /*
     * ARM Generic Timer @ 25 MHz.
     * CNTFRQ_EL0 is already set by TF-A; we just record the value.
     */
    init_timing();

    /* Hand off to the QNX kernel */
    main_parsed(argc, argv, envv);
}
