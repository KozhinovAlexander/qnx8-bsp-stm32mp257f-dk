/*
 * $QNXLicenseC:
 * Copyright 2009, 2023, 2025, BlackBerry Limited.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resmgr.h>
#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <errno.h>
#include <sys/procmgr.h>
#include <stdbool.h>
#include <startup.h>

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

typedef struct {
    struct sched_param threadparam;
    uint64_t smc_id;
    char *name;
    long kick_time;
    int opt;
    int priority;
    int sched_type;
    int verbose;
} wdt_init_t;

static int wd_parse_options(const int argc, char * const argv[], wdt_init_t *wdi)
{
    int ret = EXIT_SUCCESS;
    uint64_t val = 0ULL;
    int64_t val1 = 0;
    /* Process dash options.*/
    while ((wdi->opt = getopt(argc, argv, "i:p:t:v")) != -1) {
        switch (wdi->opt) {
        case 'i':    // SMC ID
            errno = EOK;
            val = strtoull(optarg, NULL, 0);
            if (errno == EOK) {
                wdi->smc_id = val;
            }
            break;
        case 'p':    // priority
            errno = EOK;
            val1 = strtol(optarg, NULL, 0);
            if (errno == EOK) {
                wdi->priority = (int)val1;
            }
            break;
        case 't':
            errno = EOK;
            val = strtoul(optarg, NULL, 0);
            if (errno == EOK) {
                wdi->kick_time = (long)val;
            }
            break;
        case 'v':
            wdi->verbose++;
            break;
        default:
            break;
        }
    }
    return ret;
}

static int wd_evaluate_params(wdt_init_t const * const wdi)
{
    /*check if the params are valid*/
    if (wdi->smc_id == 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid  smc-id");
        return EXIT_FAILURE;
    }
    if (wdi->kick_time == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick error : Invalid default time for watchdog timer kick.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int wd_configure_thread(wdt_init_t *wdi)
{
    /* Must be root (uid=0) at this point — startup-script processes typically are */
    if (procmgr_ability(0,
            PROCMGR_ADN_ROOT
            | PROCMGR_AOP_ALLOW
            | PROCMGR_AOP_LOCK  /* lock abilities s.t. can not be changed by inheriting threads */
            | PROCMGR_AID_IO,
            PROCMGR_AID_EOL) != 0) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: failure to acquire procmgr ability");
        return EXIT_FAILURE;
    }

    /*
     * ALERT: On AARCH64, when your thread acquires _NTO_IO_LEVEL_2 privileges,
     * it runs at the same exception level as the kernel (typically Exception Level 1, EL1).
     * As a result, the thread may execute any instruction that is permitted at that level
     * (e.g., SMC, HVC, MRS/MSR access to system registers, etc.).
     * This is a major security vulnerability, because the thread has all CPU privileges associated with this level,
     * including the ability to access and modify kernel data.
     * Your thread should acquire these privileges only as long as necessary, and relinquish them as soon as possible.
     * Therefore, you must be extra careful to not have programming errors in code running with this level of execution privilege,
     * as the damage to your system could be substantial.
     */
    // Enable IO capability.
    if (ThreadCtl( _NTO_TCTL_IO_LEVEL, _NTO_IO_LEVEL_2 ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"wdtkick: failure to acquire IO level 2 capability");
        return EXIT_FAILURE;
    }
    // run in the background
    if (procmgr_daemon( EXIT_SUCCESS, PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s:  procmgr_daemon", wdi->name);
        return EXIT_FAILURE;
    }

    // configure information
    if (wdi->verbose) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, "wdtkick: smc-id = 0x%zx, kick = %ld ms, priority = %d",
              wdi->smc_id, wdi->kick_time, wdi->priority);
    }

    // Set priority
    if (pthread_getschedparam(pthread_self(),&(wdi->sched_type), &(wdi->threadparam)) != EOK) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_WARNING,"wdtkick: get priority request failed");
        return EXIT_FAILURE;
    }
    if (wdi->priority != wdi->threadparam.sched_priority) {
        wdi->threadparam.sched_priority = wdi->priority;
        if (pthread_setschedparam(pthread_self(),wdi->sched_type, &(wdi->threadparam)) != EOK) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_WARNING,"wdtkick: can't change priority");
        }
    }

    return EXIT_SUCCESS;
}

int main(const int argc, char *argv[])
{
    int ret;
    wdt_init_t wdi;        /* wdtkick init structure */

    wdi.priority = 10;    /* default priority:default 10 */
    wdi.kick_time = -1;    /* default time for watchdog timer kick */
    wdi.smc_id = PSCI_ARM_WDT_SMC_ID;
    wdi.verbose = 0;
    wdi.name = argv[0];

    ret = wd_parse_options(argc, argv, &wdi);
    if (ret == EXIT_FAILURE) {
        return ret;
    }

    ret = wd_evaluate_params(&wdi);
    if (ret == EXIT_FAILURE) {
        return ret;
    }

    ret = wd_configure_thread(&wdi);
    if (ret == EXIT_FAILURE) {
        return ret;
    }

    struct smc_args args = {0};
    struct smc_res  res  = {0};

    slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s: start main kick loop", wdi.name);

    size_t run_cnt = 0;
    const size_t heartbeat_msg_interval_ms = 15000; /* 15 seconds */

    while (1) {
        args.a0 = wdi.smc_id;  /* SMC function ID, e.g. an OP-TEE/PSCI/SiP call ID */
        args.a1 = SMCWD_PET;   /* param1 */
        args.a2 = 0;           /* param2 */
        args.a3 = 0;           /* param3 */
        smc_call(&args, &res);
        delay((unsigned int)wdi.kick_time);
        run_cnt++;
        if(wdi.verbose && (run_cnt * wdi.kick_time) % heartbeat_msg_interval_ms == 0) {
            slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s: watchdog timer kicked %zu times", wdi.name, run_cnt);
        }
    }

    // Disable IO capability.
    if (ThreadCtl( _NTO_TCTL_IO_LEVEL, _NTO_IO_LEVEL_NONE ) == -1) {
        slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO,"%s: failure to disable IO capability", wdi.name);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
