/*
 * init_timing.c — ARM Generic Timer for STM32MP257F-DK
 *
 * The counter frequency is 25 MHz.  TF-A writes CNTFRQ_EL0 before
 * handing off to U-Boot, so we only need to read it back here.
 *
 * QNX uses the EL1 physical timer (CNTP_TVAL_EL0 / CNTP_CTL_EL0).
 */

#include <startup.h>
#include <aarch64/cpu.h>

#define STM32MP2_TIMER_FREQ  25000000UL   /* 25 MHz */

void
init_timing(void)
{
    /*
     * Read the frequency programmed by TF-A.  If it differs from our
     * expected value, override with the known-good constant.
     */
    unsigned long freq;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    if (freq == 0) {
        freq = STM32MP2_TIMER_FREQ;
        __asm__ volatile("msr cntfrq_el0, %0" :: "r"(freq));
    }

    /* Register the ARM Generic Timer with the QNX startup library */
    arm_generic_timer_init(/* ppi_irq= */ 30, freq);
}
