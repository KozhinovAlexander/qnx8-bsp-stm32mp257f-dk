/*
 * board_smp.c — Secondary CPU bring-up via PSCI for STM32MP257F-DK
 *
 * TF-A BL31 implements PSCI.  We use SMC (SMCCC 32-bit calling
 * convention) to invoke CPU_ON and send the secondary core to the
 * QNX spin loop, then to the kernel entry point.
 *
 * PSCI function IDs (SMC32, PSCI 1.0)
 */

#include <startup.h>
#include <aarch64/smp.h>
#include <aarch64/cpu.h>

#define PSCI_CPU_ON_AARCH64     0xC4000003UL
#define PSCI_SUCCESS            0

/* Entry point for secondary CPUs — provided by the QNX startup library */
extern void _smp_start(void);

/* ------------------------------------------------------------------ */
static inline long
psci_call(unsigned long func_id, unsigned long arg0,
          unsigned long arg1,   unsigned long arg2)
{
    register unsigned long r0 __asm__("x0") = func_id;
    register unsigned long r1 __asm__("x1") = arg0;
    register unsigned long r2 __asm__("x2") = arg1;
    register unsigned long r3 __asm__("x3") = arg2;

    __asm__ volatile(
        "smc #0"
        : "+r"(r0), "+r"(r1), "+r"(r2), "+r"(r3)
        :
        : "x4","x5","x6","x7","x8","x9",
          "x10","x11","x12","x13","x14","x15",
          "x16","x17","memory"
    );
    return (long)r0;
}

/* ------------------------------------------------------------------ */
void
board_smp_init(unsigned num_cpus)
{
    /* Nothing board-specific needed; GIC is already initialised */
    (void)num_cpus;
}

void
board_smp_start(unsigned cpu)
{
    /*
     * PSCI CPU_ON
     *   arg0 : target_cpu  — MPIDR of the secondary (cpu index == MPIDR here)
     *   arg1 : entry_point — physical address of _smp_start
     *   arg2 : context_id  — opaque value passed to secondary (unused)
     */
    long rc = psci_call(PSCI_CPU_ON_AARCH64,
                        (unsigned long)cpu,          /* MPIDR  */
                        (unsigned long)&_smp_start,  /* entry  */
                        0UL);                         /* ctx id */
    if (rc != PSCI_SUCCESS) {
        kprintf("board_smp_start: PSCI CPU_ON failed for cpu%u (rc=%ld)\n",
                cpu, rc);
    }
}

void
board_smp_spin(void)
{
    /* Secondary CPUs spin here until kicked by the primary */
    for (;;) {
        __asm__ volatile("wfe");
    }
}
