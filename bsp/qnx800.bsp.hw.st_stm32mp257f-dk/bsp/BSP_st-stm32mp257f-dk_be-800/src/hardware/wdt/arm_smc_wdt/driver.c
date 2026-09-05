/*
 * Copyright (c) 2020, 2021, 2023, BlackBerry Limited.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drvr/hwinfo.h>
#include <sys/procmgr.h>
#include <sys/neutrino.h>


static inline uint64_t read_current_el(void) {
  uint64_t currentEL;
  asm volatile("mrs %0, CurrentEL" : "=r" (currentEL));
  currentEL = (currentEL >> 2) & 0x3;
  return currentEL;
}

/*
 * ARM SMC Calling Convention (SMCCC) - AArch64
 * Input:  x0-x7  = function ID + up to 7 args
 * Output: x0-x3  (or x0-x17 for SMCCC 1.2+ "extended" results, rarely needed)
 *
 * Clobbers x8-x17 per AAPCS64 (caller-saved) even though SMCCC 1.0/1.1
 * technically only guarantees x0-x3 as output — the monitor/secure side
 * may still trash x4-x17, so we must tell the compiler.
 */
struct smc_args {
    uint64_t a0, a1, a2, a3, a4, a5, a6, a7;
};

struct smc_res {
    uint64_t a0, a1, a2, a3;
};

static inline void smc_call(const struct smc_args *args, struct smc_res *res)
{
    register uint64_t x0 asm("x0") = args->a0;
    register uint64_t x1 asm("x1") = args->a1;
    register uint64_t x2 asm("x2") = args->a2;
    register uint64_t x3 asm("x3") = args->a3;
    register uint64_t x4 asm("x4") = args->a4;
    register uint64_t x5 asm("x5") = args->a5;
    register uint64_t x6 asm("x6") = args->a6;
    register uint64_t x7 asm("x7") = args->a7;

    asm volatile(
        "smc #0\n"
        : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
        : "r"(x4), "r"(x5), "r"(x6), "r"(x7)
        : "x8", "x9", "x10", "x11", "x12", "x13", "x14",
          "x15", "x16", "x17", "memory", "cc"
    );

    res->a0 = x0;
    res->a1 = x1;
    res->a2 = x2;
    res->a3 = x3;
}

/**
* arm_wdt: watchdog {
*   compatible = "arm,smc-wdt";
*   arm,smc-id = <0xbc000000>;
*   status = "disabled";
* };
*/
#define PSCI_ARM_WDT_SMC_ID   0xbc000000

enum smcwd_call {
  SMCWD_INIT          = 0,
  SMCWD_SET_TIMEOUT   = 1,
  SMCWD_ENABLE        = 2,
  SMCWD_PET           = 3,
  SMCWD_GET_TIMELEFT  = 4,
};

/**
 *  @brief             Main function.
 *  @param argc        Arguments counter
 *  @param argv        Arguments string array.
 *
 *  @return            EXIT_SUCCESS on success; EXIT_FAILURE otherwise
 */
int main(int argc, char *argv[])
{
  /* Must be root (uid=0) at this point — startup-script processes typically are */
  if (procmgr_ability(0,
          PROCMGR_ADN_ROOT
          | PROCMGR_AOP_ALLOW
          | PROCMGR_AOP_LOCK
          | PROCMGR_AID_IO,
          PROCMGR_AID_EOL) != 0) {
      perror("procmgr_ability");
      exit(EXIT_FAILURE);
  }

  if (ThreadCtl(_NTO_TCTL_IO_LEVEL, _NTO_IO_LEVEL_2 ) == -1) {
      perror("ThreadCtl _NTO_TCTL_IO_LEVEL");
      exit(EXIT_FAILURE);
  }

  uint64_t el = read_current_el();
  printf("Current Exception Level: EL%lu\n", el);

  struct smc_args args = {0};
  struct smc_res  res  = {0};

  args.a0 = 0x82000000;   /* SMC function ID, e.g. an OP-TEE/PSCI/SiP call ID */
  args.a1 = 0;            /* param1 */
  args.a2 = 0;            /* param2 */
  args.a3 = 0;            /* param3 */

  smc_call(&args, &res);

  /* res.a0 typically holds the return/status code */
  printf("SMC returned: 0x%llx 0x%llx 0x%llx 0x%llx\n",
          (unsigned long long)res.a0, (unsigned long long)res.a1,
           (unsigned long long)res.a2, (unsigned long long)res.a3);

  while (1) {
      args.a0 = PSCI_ARM_WDT_SMC_ID;    /* SMC function ID, e.g. an OP-TEE/PSCI/SiP call ID */
      args.a1 = SMCWD_PET;              /* param1 */
      args.a2 = 0;                      /* param2 */
      args.a3 = 0;                      /* param3 */
      smc_call(&args, &res);
      sleep(1);
  }

  return EXIT_SUCCESS;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/wdt/arm_smc_wdt/driver.c $ $Rev: 985670 $")
#endif
