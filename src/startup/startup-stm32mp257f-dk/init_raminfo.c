/*
 * init_raminfo.c — RAM region descriptors for STM32MP257F-DK
 *
 * The board carries 4 GB LPDDR4.
 * Physical layout:
 *   0x80000000 – 0xFFFFFFFF   first 2 GB  (32-bit addressable)
 *   0x100000000 – 0x17FFFFFFF  upper 2 GB  (requires 40-bit PA / LPAE)
 *
 * The first 1 MB (0x80000000 – 0x800FFFFF) is reserved for
 * TF-A / U-Boot artefacts and the FIT image header; QNX starts at
 * 0x80100000.
 */

#include <startup.h>

void
init_raminfo(void)
{
    /* Lower 2 GB minus the reserved 1 MB at the start */
    as_add_containing(0x80100000UL, 0xFFFFFFFFUL,
                      AS_ATTR_RAM, "ram", "memory");

    /* Upper 2 GB — only available if TF-A enabled 40-bit PA */
    as_add(0x100000000ULL, 0x17FFFFFFFULL,
           AS_ATTR_RAM, "ram.hi", "memory");
}
