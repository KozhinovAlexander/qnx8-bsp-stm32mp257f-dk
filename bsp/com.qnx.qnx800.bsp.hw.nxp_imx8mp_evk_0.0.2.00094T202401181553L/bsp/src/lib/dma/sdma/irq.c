/*
 * $QNXLicenseC:
 * Copyright 2008,2009, 2022 BlackBerry Limited.
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


#include "sdma.h"

////////////////
// local vars //
////////////////
static uint32_t channel_mask;   //channels that belong to THIS process
static const struct sigevent * event_array[SDMA_N_CH];
static sdmairq_callback_t callback_array[SDMA_N_CH];
static int id = -1;
static int sdma_chid = -1;
static int sdma_coid = -1;
static pthread_t tid = -1;

/////////////////
// global vars //
/////////////////
extern uintptr_t sdma_base;


////////////////////////////////////////////////////////////////////////////////
//                                  ISR                                       //
////////////////////////////////////////////////////////////////////////////////

static int event_handler(void) {
    uint32_t irq_status;
    uint32_t i;
    const struct sigevent *event;

    irq_status = in32(sdma_base + SDMA_INTR) & channel_mask;

    // find first active interrupt that belongs to this process
    for(i=0; i < SDMA_N_CH; i++) {
       if (irq_status & (1 << i)) {
            // clear irq status bit i
            out32(sdma_base + SDMA_INTR,(1 << i));

            //call the callback if present
            if (callback_array[i]) {
                callback_array[i](i);
            }

            event = event_array[i];
            MsgSendPulse( event->sigev_coid, event->sigev_priority, event->sigev_code, 0 );

            return EOK;
        }
    }

    return -1;
}

static void *sdma_event_handler (void)
{
    struct _pulse pulse;
    int rcvid;

    pthread_setname_np(tid, "sdma_event_handler");

    while (1) {
        rcvid = MsgReceivePulse(sdma_chid, &pulse, sizeof(pulse), NULL);
        if (rcvid == -1) {
            continue;
        }

        switch (pulse.code) {
            case SDMA_CMD_COMPLETE_PULSE_CODE:
            case SDMA_EVENT_PULSE_CODE:
                event_handler();
                InterruptUnmask(sdma_irq, id);
                break;
            case SDMA_PULSE_CODE_EXIT:
                pthread_exit(NULL);
                break;
            default:
                break;
        }
    }

    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//                             PUBLIC FUNCTIONS                               //
////////////////////////////////////////////////////////////////////////////////

int sdmairq_init(uint32_t irq, uint32_t init_flags) {
    int i;
    uint32_t flags = _NTO_INTR_FLAGS_TRK_MSK;
    pthread_attr_t      attr;
    struct sched_param  param = { 0 };
    int status;
    struct sigevent sdma_event = { 0 };


    flags |= (init_flags & DMA_ATTACH_PROCESS) ? _NTO_INTR_FLAGS_PROCESS : 0;

    ThreadCtl( _NTO_TCTL_IO, 0 );
    status = pthread_attr_init(&attr);
    if ( status != EOK ) {
        goto fail;
    }
    pthread_getschedparam( pthread_self(), NULL, &param );
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    /* Start the event handler thread */
    status = pthread_create(&tid, &attr, (void *)sdma_event_handler, NULL);
    if (status != EOK) {
        perror("Failed to create event handler thread");
        pthread_attr_destroy(&attr);
        goto fail;
    }
    pthread_attr_destroy(&attr);

    /* Create channel for event handler thread */
    sdma_chid = ChannelCreate(_NTO_CHF_DISCONNECT | _NTO_CHF_UNBLOCK);
    if (sdma_chid == -1) {
        perror("ChannelCreate failed");
        goto fail;
    }

    sdma_coid = ConnectAttach(0, 0, sdma_chid, _NTO_SIDE_CHANNEL, 0);
    if (sdma_coid == -1) {
        perror("ConnectAttach failed");
        goto fail;
    }

    /* Initialize event for event handler */
    SIGEV_PULSE_INIT(&sdma_event, sdma_coid, SIGEV_PULSE_PRIO_INHERIT, SDMA_EVENT_PULSE_CODE, NULL);
    id = InterruptAttachEvent(sdma_irq, &sdma_event, init_flags);
    if (id == -1) {
        perror("InterruptAttachEvent failed");
        goto fail;
    }

    channel_mask=0;
    for(i=0;i<SDMA_N_CH;i++) {
        event_array[i] = NULL;
        callback_array[i] = NULL;
    }
    return 0;

fail:
    sdmairq_fini();
    return -1;
}


void sdmairq_fini(void) {

    if (id != -1) {
        InterruptDetach(id);
    }

    if (tid != -1) {
        int32_t err = EOK;
        struct timespec timeout;

        MsgSendPulse( sdma_coid, -1, SDMA_PULSE_CODE_EXIT, 0 );

        /* Wait for event handler thread to exit normally */
        clock_gettime(CLOCK_MONOTONIC, &timeout);
        timeout.tv_sec += 2;
        err = pthread_timedjoin_monotonic(tid, NULL, &timeout);
        if (err == ETIMEDOUT) {
            perror("event handler thread join timed out");
        }
    }

    if (sdma_coid != -1) {
        ConnectDetach (sdma_coid);
    }

    if (sdma_chid != -1) {
        ChannelDestroy(sdma_chid);
    }
}

void sdmairq_event_add(uint32_t channel, const struct sigevent *event) {
    atomic_set(&channel_mask,1 << channel);
    event_array[channel] = event;
}

void sdmairq_event_remove(uint32_t channel) {
    atomic_clr(&channel_mask,1 << channel);
    event_array[channel] = NULL;
}

void sdmairq_callback_add(uint32_t channel,sdmairq_callback_t func_ptr) {
    callback_array[channel] = func_ptr;
}

void sdmairq_callback_remove(uint32_t channel) {
    callback_array[channel] = NULL;
}


#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/dma/sdma/irq.c $ $Rev: 972859 $")
#endif
