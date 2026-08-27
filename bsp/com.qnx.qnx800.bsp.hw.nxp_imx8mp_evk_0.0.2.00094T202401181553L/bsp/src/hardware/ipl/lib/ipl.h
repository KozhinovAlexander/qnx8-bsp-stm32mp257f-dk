/*
 * Copyright (c) 2007, 2008, 2019, 2022-2023, BlackBerry Limited.
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

#ifndef __IPL_H_INCLUDED
#define __IPL_H_INCLUDED

#include <sys/platform.h>
#include <sys/startup.h>
#include <stdint.h>
#include <sys/types.h>
#include <cpu_ipl.h>

/*Global definitions for some space that we share in the IPL code*/

extern char                     scratch [512];
extern struct startup_header    startup_hdr;

/* image_scan.c */
extern paddr_t  image_scan(paddr_t start, const paddr_t end, const int docksum);

/* image_setup.c */
extern int      image_setup(const paddr_t addr);

/* image_start.c */
extern int      image_start(const paddr_t addr);
extern int      image_start_with_fdt (unsigned long addr, unsigned long fdt_addr);

/* checksum() -- platform dependant */
extern int      checksum(paddr_t addr, size_t len);

/* jump() -- platform dependant */
extern void     jump(paddr_t addr);

/* copy.c */
extern void     copy(const paddr_t dst, const paddr_t src, const size_t size);

/* string.c */
extern unsigned long strhextoul(const char *cp);

/*
 * image_add_info.c
 * add a startup info structure to the startup header.
 * Returns: 1 upon success, otherwise 0
 */
extern int      image_add_info(void *const imaddr, const struct startup_info_hdr *const info);

/*
 * for generic serial image download.
 * NOTE: you must call init_serxxx() before calling any of those
 * functions.
 */
typedef struct _ser_dev_t {
    char    (*get_byte)(void);
    void    (*put_byte)(char);
    char    (*poll)(void);
} ser_dev;

extern int      image_download_ser(const paddr_t dst_address);

/*
 * ser_dev.c
 */
extern void     init_serdev(ser_dev *const dev);
extern char     ser_getchar(void);
extern char     ser_poll(void);
extern void     ser_putchar(const char chr);

/* ser8250.c */
extern void     init_ser8250(const paddr_t address, const uint8_t size, const uint8_t shift, const uint32_t baud, uint32_t clk, uint32_t divisor);

/* kprintf.c */
extern void     kprintf(const char *const fmt, ...);

/* udelay.c */
extern void     init_udelay(void (*udly)(const unsigned));
extern void     (*udelay)(unsigned);

/* memcmp.c */
extern int memcmp(const void *p1, const void *p2, size_t nbytes);

/* memcpy.c */
extern void     *memcpy(void *dst, const void *src, size_t nbytes);

/* memmove.c */
extern void     *memmove(void *const dest, const void *const src, size_t len);

/* memset.c */
extern void     *memset(void *const str, const int value, size_t num);

/* _main.c */
extern void     _main(void);

/* __main.c */
extern void     __main(void);

/* main.c */
extern int      main(void);

/*
 * Block or block-like device
 */
typedef struct _blkdev_t {
    int     (*blkrw)(void *, void *buf, uint32_t blkno, uint32_t blkcnt, int rd);
    void*   (*alloc_dmabuf)(int nbytes);    // DMA buffer won't be freed!
    int     verbose;
} blkdev_t;

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/ipl.h $ $Rev: 980326 $")
#endif
