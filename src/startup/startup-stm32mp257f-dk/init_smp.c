/*
 * init_smp.c — SMP descriptor for STM32MP257F-DK
 *
 * The board has 2 Cortex-A35 cores.  Secondary bring-up is handled
 * by board_smp.c via PSCI CPU_ON (SMC call to TF-A BL31).
 */

#include <startup.h>
#include <aarch64/smp.h>

/* MPIDR values for each core (cluster 0, cores 0 and 1) */
static const uint64_t cpu_mpidr[] = {
    0x0000000000000000ULL,   /* CPU0: Aff0=0, Aff1=0 */
    0x0000000000000001ULL,   /* CPU1: Aff0=1, Aff1=0 */
};

extern void board_smp_start(unsigned cpu);
extern void board_smp_spin(void);
extern void board_smp_init(unsigned num_cpus);

static const struct smp_entry smp_ops = {
    .start = board_smp_start,
    .spin  = board_smp_spin,
};

void
init_smp(void)
{
    add_smp_entry(&smp_ops, cpu_mpidr,
                  sizeof(cpu_mpidr) / sizeof(cpu_mpidr[0]));
}
