/*
 * $QNXLicenseC:
 * Copyright 2018-2019 NXP
 * Copyright 2018, QNX Software Systems.
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

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <hw/inout.h>
#include <time.h>
#include <assert.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <string.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <unistd.h>

#include <soc/nxp/imx8/common/imx8_mu_drv.h>

#define _mu_slogf(msg,...)                                            \
do{                                                                   \
  slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_INFO, (msg), ##__VA_ARGS__); \
}while(0)

#define IMX_MU_SIZE            0x10000

/**
 * Get MU status register flags
 *
 * @param handle  MU device handle.
 *
 * @return Status register flags.
 */
uint32_t imx_mu_read_status_reg( void * const handle)
{
    assert(handle);
    return in32(((imx_mu_t *)handle)->vbase + IMX_MU_xSR);
}

/**
 * Write Mu status register
 *
 * @param handle MU device handle.
 * @param value  Value to write.
 */
void imx_mu_write_status_reg(void * const handle, uint32_t const value)
{
    assert(handle);
    out32(((imx_mu_t *)handle)->vbase + IMX_MU_xSR, value);
}

/**
 * Write MU TX register
 *
 * @param handle  MU device handle.
 * @param index   MU TX register index.
 * @param value   Value to write.
 */
void imx_mu_write_tx_reg(void * const handle, const uint32_t index, const uint32_t value)
{
    assert(handle);
    assert(index < 4);
    /* Write data to TX register */
    out32(((imx_mu_t *)handle)->vbase + IMX_MU_xTR0 + 0x4 * index, value);
}

/**
 * Read MU RX register
 *
 * @param handle  MU device handle.
 * @param index   MU RX register index.
 *
 * @return Value read.
 */
uint32_t imx_mu_read_rx_reg(void * const handle, const uint32_t index)
{
    assert(handle);
    assert(index < 4);
    /* Write data to TX register */
    return in32(((imx_mu_t *)handle)->vbase + IMX_MU_xRR0 + 0x4 * index);
}

/**
 * Read ACR or BCR register
 *
 * @param handle  MU device handle.
 *
 * @return xCR register value.
 */
uint32_t imx_mu_read_ctrl_reg(void * const handle)
{
    assert(handle);
    return in32(((imx_mu_t *)handle)->vbase + IMX_MU_xCR);
}

/**
 * Write ACR or BCR register
 *
 * @param handle  MU device handle.
 * @param value   Value to write.
 *
 */
void imx_mu_write_ctrl_reg(void * const handle, const uint32_t value)
{
    assert(handle);
    out32(((imx_mu_t *)handle)->vbase + IMX_MU_xCR, value);
}

/**
 * Resets both MUA and MUB
 *
 * Note only MUA side can call this function. User should call imx_mu_waitfor_reset_complete() as next.
 * @param handle MU device handle.
 */
void imx_mu_reset(void * const handle)
{
    assert(handle);
    imx_mu_write_ctrl_reg(handle, imx_mu_read_ctrl_reg(handle) | IMX_MU_ACR_MUR_MASK);
}

/**
 * Waits on reset completion.
 *
 * @param handle  MU device handle.
 * @param timeout Timeout in ms.
 *
 * @return Execution status. 0 when success, -1 on timeout.
 */
int imx_mu_waitfor_reset_complete(void *const handle, uint32_t timeout)
{
    assert(handle);
    while ((imx_mu_read_status_reg(handle) & imx_mu_reset_state) && timeout) {
        delay(1);
        timeout--;
    }
    return timeout ? 0 : -1;
}

/**
 * Set general purpose interrupt request.
 *
 * Request might not be set in case previous request is still pending on other MU side. In that case -1 is returned and
 * user should call again.
 *
 * @param handle  MU device handle.
 * @param index   MU index of general purpose request (0-3).
 *
 * @return        Execution status. 0 on success or -1 when previous request is still pending.
 */
int imx_mu_set_gpint_request(void * const handle, const uint32_t index)
{
    uint32_t cr;

    assert(handle);
    assert(index < 4);

    cr = imx_mu_read_ctrl_reg(handle);
    if (cr & (1 << (IMX_MU_xCR_GIR_SHIFT + index))) {
        /* If GIRn bit is set we cannot write but we must wait. Client should call again. */
        return -1;
    }
    /* We can set */
    imx_mu_write_ctrl_reg(handle, cr | (1 << (IMX_MU_xCR_GIR_SHIFT + index)));
    return 0;
}

/**
 * Functions blocks calling thread until General purpose interrupt request is set or timeout occurs.
 *
 * @param handle  MU device handle.
 * @param index   GP bit index. 0-3.
 * @param timeout Timeout in us.
 * @param flags   Driver flags. When IMX_MU_FLAG_BLOCK flag is set the driver will not use interrupt mode.
 * @return        Execution status. 0 when success or -1.
 */
int imx_mu_waitfor_gpint_request(void * const handle, uint32_t index, const uint32_t timeout, const uint32_t flags)
{
    imx_mu_t *data = handle;
    uint64_t ntime = (uint64_t)1000 * timeout;
    pthread_mutex_t *lock;
    int stat = EOK;

    assert(handle);
    assert(index < 4);
    /* Lock mutex for access to GP bitfield */
    lock = &data->gp_lock[index];
    stat = pthread_mutex_lock(lock);
    if (stat != EOK) {
        /* Exit in case someone else already use this TXn */
        if (flags & IMX_MU_FLAG_LOG) {
            _mu_slogf("MU base 0x%x, TX%u mutex lock failed: %s", data->pbase, index, strerror(stat));
        }
        return stat;
    }
    /* Check if we have interrupt mode enabled and user does not require blocking mode */
    if (data->intr && !(flags & IMX_MU_FLAG_BLOCK)) {
        /* Check if required GIP flag is set, if not then loop */
        while (!(imx_mu_read_status_reg(handle) & (1 << (index + IMX_MU_xSR_GIP_SHIFT)))) {
            /* Enable GIP interrupt */
            imx_mu_write_ctrl_reg(handle, imx_mu_read_ctrl_reg(handle) | (1 << (index + IMX_MU_xCR_GIE_SHIFT)));
            /* Set interrupt timeout - if required by user */
            if (timeout) {
                TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_INTR, NULL, &ntime, NULL);
            }
            /* Wait on interrupt or timeout */
            stat = InterruptWait_r(0, NULL);
            /* Disable GIP interrupt */
            imx_mu_write_ctrl_reg(handle, imx_mu_read_ctrl_reg(handle) & ~(1 << (index + IMX_MU_xCR_GIE_SHIFT)));
            /* Unmask interrupt */
            InterruptUnmask(data->intr, data->iid);
            if (stat != EOK) {
                if (flags & IMX_MU_FLAG_LOG) {
                    _mu_slogf("MU base 0x%x, GIP%u InterruptWait: %s", data->pbase, index, strerror(stat));
                }
                break;
            }
        }
    } else {
        /* Blocking mode. Interrupt disabled.  */
        ntime = (timeout > 0) ? timeout : 1; /* Initialize ntime if timeout is required. Otherwise will wait infinitely. */
        /* Wait for empty TXn register */
        while (ntime && !(imx_mu_read_status_reg(handle) & (1 << (index + IMX_MU_xSR_GIP_SHIFT)))) {
            /* Wait 1 us */
            nanospin_ns(1000);
            if (timeout) {
                /* Decrement ntime if timeout value was set by user */
                ntime--;
            }
        }
        if (!ntime) {
            if (flags & IMX_MU_FLAG_LOG) {
                _mu_slogf("MU base 0x%x, GIP%u timed out", data->pbase, index);
            }
        }
    }
    /* In case of timeout we have to unlock mutex and exit */
    if (((timeout > 0) && !ntime) || (stat != EOK)) {
        pthread_mutex_unlock(lock);
        return -1;
    }
    /* Clear the bit */
    imx_mu_write_status_reg(handle, imx_mu_read_status_reg(handle) | (1 << (index + IMX_MU_xSR_GIP_SHIFT)));
    return pthread_mutex_unlock(lock);
}
/**
 * Writes data to MU TX register.
 *
 * @param handle  MU device handle.
 * @param index   MU TX register index.
 * @param val     Value to send.
 * @param timeout Timeout in us or zero to disable timeout.
 * @param flags   Driver flags. When IMX_MU_FLAG_BLOCK flag is set the driver will not use interrupt mode.
 *
 * @return Execution status.
 */
int imx_mu_send(void * const handle, uint32_t index, const uint32_t val, const uint32_t timeout, const uint32_t flags)
{
    imx_mu_t *data = handle;
    uint64_t ntime = (uint64_t)1000 * timeout;
    pthread_mutex_t *lock;
    int stat = EOK;
    uint32_t reg;

    assert(data);
    assert(index < 4);
    /* Lock mutex for access to TXn register */
    lock = &data->tx_lock[index];
    stat = pthread_mutex_lock(lock);
    if (stat != EOK) {
        /* Exit in case someone else already use this TXn */
        if (flags & IMX_MU_FLAG_LOG) {
            _mu_slogf("MU base 0x%x, TX%u mutex lock failed: %s", data->pbase, index, strerror(stat));
        }
        return stat;
    }
    /* Extract MU transmit register offset */
    reg = IMX_MU_xTR0 + 0x4 * index;
    /* Reverse index because of different indexing of xSR and xCR bitfields */
    index = 3 - index;
    /* Check if we have interrupt mode enabled and user does not require blocking mode */
    if (data->intr && !(flags & IMX_MU_FLAG_BLOCK)) {
        /* Check if there is already some data in register, waiting to send. If yes then call InterruptWait() and wait for empty TX reg. */
        while (!(in32(data->vbase + IMX_MU_xSR) & ((1 << index) << IMX_MU_xSR_TE_SHIFT))) {
            /* Enable TIE - transmit interrupt */
            out32(data->vbase + IMX_MU_xCR, in32(data->vbase + IMX_MU_xCR) | ((1 << index) << IMX_MU_xCR_TIE_SHIFT));
            /* Set interrupt timeout - if required by user */
            if (timeout) {
                TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_INTR, NULL, &ntime, NULL);
            }
            /* Wait on interrupt or timeout */
            stat = InterruptWait_r(0, NULL);
            /* Disable interrupt */
            out32(data->vbase + IMX_MU_xCR, in32(data->vbase + IMX_MU_xCR) & ~((1 << index) << IMX_MU_xCR_TIE_SHIFT));
            /* Unmask interrupt */
            InterruptUnmask(data->intr, data->iid);
            if (stat != EOK) {
                if (flags & IMX_MU_FLAG_LOG) {
                    _mu_slogf("MU base 0x%x, TX%u InterruptWait: %s", data->pbase, index, strerror(stat));
                }
                break;
            }
        }
    } else {
        /* Blocking mode. Interrupt disabled.  */
        ntime = (timeout > 0) ? timeout : 1; /* Initialize ntime if timeout is required. Otherwise will wait infinitely. */
        /* Wait for empty TXn register */
        while (ntime && !(in32(data->vbase + IMX_MU_xSR) & ((1 << index) << IMX_MU_xSR_TE_SHIFT))) {
            /* Wait 1 us */
            nanospin_ns(1000);
            if (timeout) {
                /* Decrement ntime if timeout value was set by user */
                ntime--;
            }
        }
        if (!ntime) {
            if (flags & IMX_MU_FLAG_LOG) {
                _mu_slogf("MU base 0x%x, TX%u timed out", data->pbase, index);
            }
        }
    }
    /* In case of timeout we have to unlock mutex and exit */
    if (((timeout > 0) && !ntime) || (stat != EOK)) {
        pthread_mutex_unlock(lock);
        return -1;
    }
    /* Write data to TX register, unlock mutex and return */
    out32(data->vbase + reg, val);
    return pthread_mutex_unlock(lock);
}

/**
 * Reads data from MU RX register.
 *
 * @param handle  MU device handle.
 * @param index   MU RX register index.
 * @param val     Pointer to variable to store read data.
 * @param timeout Timeout in us or zero to disable timeout.
 * @param flags   Driver flags. When IMX_MU_FLAG_BLOCK flag is set the driver will not use interrupt mode.
 *
 * @return Execution status.
 */
int imx_mu_read(void *const handle, uint32_t index, uint32_t *val, const uint32_t timeout, const uint32_t flags)
{
    imx_mu_t *data = handle;
    uint64_t ntime = (uint64_t)1000 * timeout;
    pthread_mutex_t *lock;
    int stat = EOK;
    uint32_t reg;

    assert(val);
    assert(data);
    assert(index < 4);
    /* Lock mutex for access to RXn register */
    lock = &data->rx_lock[index];
    stat = pthread_mutex_lock(lock);
    if (stat != EOK) {
        /* Exit in case someone else already use this RXn */
        if (flags & IMX_MU_FLAG_LOG) {
            _mu_slogf("MU base 0x%x, RX%u mutex lock failed: %s", data->pbase, index, strerror(stat));
        }
        return stat;
    }
    /* Extract MU receive register offset */
    reg = IMX_MU_xRR0 + 0x4 * index;
    /* Reverse index because of different indexing of xSR and xCR bitfields */
    index = 3 - index;
    /* Check if we have interrupt mode enabled and user does not require blocking mode */
    if (data->intr && !(flags & IMX_MU_FLAG_BLOCK)) {
        /* Check if RX is empty. If yes then call InterruptWait() and wait for data. */
        while (!(in32(data->vbase + IMX_MU_xSR) & ((1 << index) << IMX_MU_xSR_RF_SHIFT))) {
            /* Enable RIE - receive interrupt */
            out32(data->vbase + IMX_MU_xCR, in32(data->vbase + IMX_MU_xCR) | ((1 << index) << IMX_MU_xCR_RIE_SHIFT));
            /* Set interrupt timeout - if required by user */
            if (timeout) {
                TimerTimeout(CLOCK_MONOTONIC, _NTO_TIMEOUT_INTR, NULL, &ntime, NULL);
            }
            /* Wait on interrupt or timeout */
            stat = InterruptWait_r(0, NULL);
            /* Disable interrupt */
            out32(data->vbase + IMX_MU_xCR, in32(data->vbase + IMX_MU_xCR) & ~((1 << index) << IMX_MU_xCR_RIE_SHIFT));
            /* Unmask interrupt */
            InterruptUnmask(data->intr, data->iid);
            if (stat != EOK) {
                if (flags & IMX_MU_FLAG_LOG) {
                    _mu_slogf("MU base 0x%x, RX%u InterruptWait: %s", data->pbase, index, strerror(stat));
                }
                break;
            }
        }
    } else {
        /* Blocking mode. Interrupt disabled.  */
        ntime = (timeout > 0) ? timeout : 1; /* Initialize ntime if timeout is required. Otherwise will wait infinitely. */
        /* Wait for full RXn register */
        while (ntime && !(in32(data->vbase + IMX_MU_xSR) & ((1 << index) << IMX_MU_xSR_RF_SHIFT))) {
            /* Wait 1 us */
            nanospin_ns(1000);
            if (timeout) {
                /* Decrement ntime if timeout value was set by user */
                ntime--;
            }
        }
        if (!ntime) {
            if (flags & IMX_MU_FLAG_LOG) {
                _mu_slogf("MU base 0x%x, RX%u timed out", data->pbase, index);
            }
        }
    }
    /* In case of timeout we have to unlock mutex and exit */
    if (((timeout > 0) && !ntime) || (stat != EOK)) {
        pthread_mutex_unlock(lock);
        return -1;
    }
    /* Read data from RX register, unlock mutex and return */
    *val = in32(data->vbase + reg);
    return pthread_mutex_unlock(lock);
}

/**
 * Set flag in xCR register. Other MU side can read this flag in xSR register.
 * Set may fail in case the previous write is still updating on other MU side.
 *
 * @param handle  MU device handle.
 * @param index   MU flag index. (0-2)
 * @param set     Set or clear the flag. 0 means clear, other value means set.
 *
 * @return        Execution status. 0 on success or -1 when update is not possible.
 */
int imx_mu_set_flag(void * const handle, const uint32_t index, const uint32_t set)
{
    assert(handle);
    assert(index < 3);
    if (imx_mu_read_status_reg(handle) & imx_mu_flags_updating) {
        /* We cannot write a new flag */
        return -1;
    }
    imx_mu_write_ctrl_reg(handle, set ? (imx_mu_read_ctrl_reg(handle) | (1 << index)) :
                          (imx_mu_read_ctrl_reg(handle) & ~(1 << index)));
    return 0;
}

/**
 * Get other side flags.
 *
 * Client calls this function to obtain flags set by other MU side.
 *
 * @param handle  MU device handle.
 * @param index   MU flag index. (0-2)
 *
 * @return        Read flag value.
 */
uint32_t imx_mu_get_flag(void * const handle, const uint32_t index)
{
    assert(handle);
    assert(index < 3);

    return imx_mu_read_status_reg(handle) & (1 << index);
}

/**
 * Initializes MU unit.
 *
 * @param base MU base address.
 * @param intr MU interrupt number. Driver will not use interrupt mode when 0 is passed.
 *
 * @return Handle to allocated driver internal data.
 */
void *imx_mu_init(const uint32_t base, const int32_t intr)
{
    int stat;
    imx_mu_t *data;
    unsigned count, j;

    assert(base);
    do {
        /* Allocate handle */
        data = calloc(sizeof(imx_mu_t), 1);
        if (data == NULL) {
            fprintf(stderr, "%s(0x%x, %u): Unable to allocate MU data\n", __FUNCTION__, base, intr);
            goto fail0;
        }
        data->pbase = base;
        data->intr = intr;
        /* Mmap MU registers */
        data->vbase = mmap_device_io(IMX_MU_SIZE, data->pbase);

        if (!data->vbase) {
            fprintf(stderr, "%s(0x%x, %u): Unable to mmap MU registers.\n", __FUNCTION__, base, intr);
            goto fail1;
        }
        /* Clear status flags */
        out32(data->vbase + IMX_MU_xSR, in32(data->vbase + IMX_MU_xSR));
        /* Attach interrupt if IRQ number is passed by user */
        if (data->intr) {
            SIGEV_INTR_INIT(&data->intrevent);
            data->iid = InterruptAttachEvent(data->intr, &data->intrevent, _NTO_INTR_FLAGS_TRK_MSK);
            if (data->iid == -1) {
                fprintf(stderr, "%s(0x%x, %u): Interrupt attach failed. errno: %s\n", __FUNCTION__, base, intr, strerror(errno));
                goto fail2;
            }
        }
        /* Initialize RX mutexes for all RX registers (xRR0 - xRR3) */
        for (count= 0; count< (sizeof(data->rx_lock) / sizeof(pthread_mutex_t)); count++) {
            stat = pthread_mutex_init(&data->rx_lock[count], NULL);
            if (stat != EOK) {
                fprintf(stderr, "%s(0x%x, %u): RX%u mutex init failed: %s\n", __FUNCTION__, count, base, intr, strerror(errno));
                goto fail3;
            }
        }
        /* Initialize TX mutexes for all TX registers (xTR0 - xTR3) */
        for (count= 0; count< (sizeof(data->tx_lock) / sizeof(pthread_mutex_t)); count++) {
            stat = pthread_mutex_init(&data->tx_lock[count], NULL);
            if (stat != EOK) {
                fprintf(stderr, "%s(0x%x, %u): TX%u mutex init failed: %s\n", __FUNCTION__, count, base, intr, strerror(errno));
                goto fail4;
            }
        }
        /* Initialize  mutexes for general purpose interrupts */
        for (count= 0; count< (sizeof(data->gp_lock) / sizeof(pthread_mutex_t)); count++) {
            stat = pthread_mutex_init(&data->gp_lock[count], NULL);
            if (stat != EOK) {
                fprintf(stderr, "%s(0x%x, %u): GP%u mutex init failed: %s\n", __FUNCTION__, count, base, intr, strerror(errno));
                goto fail5;
            }
        }
        /* Return pointer to MU data */
        return (void *)data;
    } while (0);
fail5:
    /* Destroy GP mutexes */
    for (j = 0; j < count; j++) {
        pthread_mutex_destroy(&data->gp_lock[j]);
    }
    count= (sizeof(data->tx_lock) / sizeof(pthread_mutex_t));
fail4:
    /* Destroy TX mutexes */
    for (j = 0; j < count; j++) {
        pthread_mutex_destroy(&data->tx_lock[j]);
    }
    /* Initialize countto destroy RX mutexes */
    count= (sizeof(data->rx_lock) / sizeof(pthread_mutex_t));
fail3:
    /* Destroy RX mutexes */
    for (j = 0; j < count; j++) {
        pthread_mutex_destroy(&data->rx_lock[j]);
    }
fail2:
    /* Unmap device registers */
    munmap_device_io(data->vbase, IMX_MU_SIZE);
fail1:
    /* Free handle */
    free(data);
fail0:
    /* Return NULL to signalize error */
    return NULL;
}

/**
 * De-initializes MU driver.
 *
 * @param handle  MU device handle.
 *
 * @return Execution status.
 */
int imx_mu_deinit(void *const handle)
{
    unsigned i;
    int status = 0;
    imx_mu_t *data = handle;
    assert(data);
    /* Destroy RX mutexes */
    for (i = 0; i < (sizeof(data->rx_lock) / sizeof(pthread_mutex_t)); i++) {
        pthread_mutex_destroy(&data->rx_lock[i]);
    }
    /* Destroy TX mutexes */
    for (i = 0; i < (sizeof(data->tx_lock) / sizeof(pthread_mutex_t)); i++) {
        pthread_mutex_destroy(&data->tx_lock[i]);
    }
    /* Destroy GP mutexes */
    for (i = 0; i < (sizeof(data->gp_lock) / sizeof(pthread_mutex_t)); i++) {
        pthread_mutex_destroy(&data->gp_lock[i]);
    }
    /* Detach interrupt */
    if (data->intr) {
        status = InterruptDetach(data->iid);
    }
    /* Unmap device registers */
    status |= munmap_device_io(data->vbase, IMX_MU_SIZE);
    /* Free handle */
    free(handle);
    return status;
}

void ctor(void) __attribute__((__constructor__));
/**
 * MU library constructor. It's run when a shared library is loaded.
 */
void ctor(void)
{
    /* Actually not used by MU lib. But kept here. */
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/hw_vendor/nxp/imx8/mu/imx8_mu_drv.c $ $Rev: 980950 $")
#endif
