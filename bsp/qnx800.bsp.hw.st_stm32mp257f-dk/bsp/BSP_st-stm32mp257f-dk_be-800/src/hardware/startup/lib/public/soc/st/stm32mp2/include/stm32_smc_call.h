/*
 * Copyright (c) 2026, Alexander Kozhinov <ak.alexander.kozhinov@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

#ifndef STM32_SMC_CALL_H_
#define STM32_SMC_CALL_H_

#include "aarch64/psci.h"


// static inline uint64_t stm32_sec_firmware_psci(
//     uint64_t regX0, uint64_t regX1, uint64_t regX2, uint64_t regX3, uint64_t regX4)
// {
//     __asm__ __volatile__(
//         "ldr x0, %0\n"
//         "ldr x1, %1\n"
//         "ldr x2, %2\n"
//         "ldr x3, %3\n"
//         "ldr x4, %4\n"
//         "smc    #0\n"
//         "str x0, %0\n"
//         : "+m"(regX0), "+m"(regX1),
//         "+m"(regX2), "+m"(regX3), "+m"(regX4)
//
//         : "m"(regX0)
//         : "x0", "x1", "x2", "x3", "x4"
//     );
//     return regX0;
// }

static inline uint64_t stm32_sec_firmware_psci(
    uint64_t regX0, uint64_t regX1, uint64_t regX2, uint64_t regX3, uint64_t regX4)
{
    register uint64_t x0 __asm__("x0") = regX0;
    register uint64_t x1 __asm__("x1") = regX1;
    register uint64_t x2 __asm__("x2") = regX2;
    register uint64_t x3 __asm__("x3") = regX3;
    register uint64_t x4 __asm__("x4") = regX4;

    __asm__ __volatile__(
        "smc #0\n"
        : "+r"(x0)
        : "r"(x1), "r"(x2), "r"(x3), "r"(x4)
        : "memory"
    );

    return (uint64_t)x0;
}

#define STM32_PSCI_CPU_ON_AARCH64	0xC4000003

/* PSCI error codes */
#define STM32_PSCI_SUCCESS                    (0)
#define STM32_PSCI_NOT_SUPPORTED              (-1)
#define STM32_PSCI_INVALID_PARAMS             (-2)
#define STM32_PSCI_DENIED                     (-3)
#define STM32_PSCI_ALREADY_ON                 (-4)
#define STM32_PSCI_ON_PENDING                 (-5)
#define STM32_PSCI_INTERN_FAIL                (-6)
#define STM32_PSCI_NOT_PRESENT                (-7)
#define STM32_PSCI_DISABLED                   (-8)
#define STM32_PSCI_INVALID_ADDRESS            (-9)

#endif    /* STM32_SMC_CALL_H_ */
