/*
 * $QNXLicenseC:
 * Copyright 2008,2009, 2022 BlackBerry Limited.
 * Copyright 2016, Freescale Semiconductor, Inc.
 * Copyright 2017-2019, 2022 NXP
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


/*
 * The purpose of this module is to control the access to shared variables
 * to shared mutexes that are used to implement
 * synchronization control for multiple instances of the dma library.
 * The 'mx35_dma_cfg' utility must be called before the dmalib is initialized
 * in order to generate the shared resources.
 */

#include "sdma.h"

#ifndef SDMA_MUTEX_PATH
    #define SDMA_MUTEX_PATH "/SDMA_MUTEX"
#endif

// local variables
static int fd;
static sdma_shmem_t * shmem_ptr;
#ifdef IMX8
static sdma_dma_mem_t *dma_mem_ptr = NULL;
static int fd_dma_mem = NOFD;
#endif
////////////////////////////////////////////////////////////////////////////////
//                              PUBLIC FUNCTIONS                              //
////////////////////////////////////////////////////////////////////////////////

// This function opens and maps shared memory that was created by the
// 'mx35_dma_cfg' utility

int sdmasync_init(void) {

    fd = shm_open(SDMA_MUTEX_PATH, O_RDWR, 0666);
    if (fd == -1) {
        perror("Failed to open shared memory object");
        goto fail1;
    }

    //map it to our control structure
    shmem_ptr = mmap(    0,
                         sizeof(sdma_shmem_t),
                         PROT_READ|PROT_WRITE,
                         MAP_SHARED,
                         fd,
                         0          );
    if (shmem_ptr == MAP_FAILED) {
        perror("Failed to map shared memory object");
        goto fail2;
    }
#ifdef IMX8
    /* This region was already allocated by library constructor so
     * we can open it. */
    fd_dma_mem = posix_typed_mem_open("/memory/dma", O_RDWR, POSIX_TYPED_MEM_MAP_ALLOCATABLE);
    if (fd_dma_mem == NOFD) {
        perror("sdmasync_init() posix_typed_mem_open failed.\n");
        goto fail3;
    }
    /* Map dma memory into our address space */
    dma_mem_ptr = mmap(NULL, sizeof(sdma_dma_mem_t),
                       PROT_READ | PROT_WRITE,
                       MAP_SHARED | MAP_NOINIT, fd_dma_mem, (off_t) shmem_ptr->paddr);
    close(fd_dma_mem);
    if (dma_mem_ptr == MAP_FAILED) {
        perror("sdmasync_init() error on mmap dma region\n");
        goto fail3;
    }
#endif
    return 0;
#ifdef IMX8
fail3:
    munmap(shmem_ptr,sizeof(sdma_shmem_t));
#endif
fail2:
    close(fd);
fail1:
    return -1;
}


void sdmasync_fini(void) {
    munmap(shmem_ptr,sizeof(sdma_shmem_t));
#ifdef IMX8
    if (dma_mem_ptr) {
        munmap(dma_mem_ptr, sizeof(sdma_dma_mem_t));
        dma_mem_ptr = NULL;
    }
    if (fd_dma_mem != NOFD) {
        close(fd_dma_mem);
        fd_dma_mem = NOFD;
    }
#endif
    close(fd);
}

// Shared-memory variable access functions

pthread_mutex_t * sdmasync_cmdmutex_get(void) {
    return &(shmem_ptr->command_mutex);
}

pthread_mutex_t * sdmasync_libinit_mutex_get(void) {
    return &(shmem_ptr->libinit_mutex);
}

pthread_mutex_t * sdmasync_regmutex_get(void) {
    return &(shmem_ptr->register_mutex);
}


int sdmasync_is_first_process(void) {
    if (shmem_ptr->process_cnt == 1)
        return 1;
    else
        return 0;
}

int sdmasync_is_last_process(void) {
    if (shmem_ptr->process_cnt == 0)
        return 1;
    else
        return 0;
}

void sdmasync_process_cnt_incr(void) {
    shmem_ptr->process_cnt++;
}

void sdmasync_process_cnt_decr(void) {
    shmem_ptr->process_cnt--;
}

uint32_t sdmasync_ccb_paddr_get(void) {
#ifndef IMX8
    return shmem_ptr->ccb_paddr;
#else
    return (uint32_t)(uintptr_t) shmem_ptr->paddr->ccb;
#endif
}

sdma_ccb_t * sdmasync_ccb_ptr_get(void) {
#ifndef IMX8
	return shmem_ptr->ccb_arr;
#else
	return dma_mem_ptr->ccb;
#endif
}

sdma_bd_t * sdmasync_cmdbd_ptr_get(void) {
#ifndef IMX8
	return &shmem_ptr->cmd_chn_bd;
#else
	return &dma_mem_ptr->cmd_chn_bd;
#endif
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/sdma/sync.c $ $Rev: 972795 $")
#endif
