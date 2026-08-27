/*
 * Copyright (c) 2014, 2022, BlackBerry Limited.
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

#ifndef __LIBCAN_H_INCLUDED
#define __LIBCAN_H_INCLUDED

#include <stdint.h>
#include <inttypes.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <sys/slog2.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/can_dcmd.h>
#include <secpol/secpol.h>

/* CANDEV CFLAGS bit[31] = 0: Standard CAN | 1: CAN FD  */
#define CANDEV_CFLAG_CANFD	    (1 << 31)
#define CANDEV_CFLAG_CAN	    (0 << 31)

/* CAN Message Linked List element */
typedef struct _canmsg {
    CAN_MSG         cmsg;   /* CAN message */
    struct _canmsg *next;   /* Pointer to next message */
    struct _canmsg *prev;   /* Pointer to previous message */
} canmsg_t;

/* Message List control */
typedef struct _canmsg_list {
    size_t      len;        /* Length of each msg in bytes */
    unsigned    nmsg;       /* Total number of messages in list */
    unsigned    msgcnt;     /* Number of messages in use */
    canmsg_t    *msgs;      /* Array of messages defining the message list */
    canmsg_t    *head;      /* Head of list to add new messages */
    canmsg_t    *tail;      /* Tail of list to consume messages */
} canmsg_list_t;

/* CAN mailbox type */
typedef enum
{
    CANDEV_TYPE_RX,
    CANDEV_TYPE_TX
} CANDEV_TYPE;


/* Blocked Client Wait Structure */
typedef struct client_wait_entry
{
    struct client_wait_entry    *next;  /* Pointer to next blocked client */
    rcvid_t                      rcvid; /* Blocked client's rcvid */
} CLIENTWAIT;

/* Blocked Client Wait Queue */
typedef struct client_waitq_entry
{
    CLIENTWAIT     *elems;
    CLIENTWAIT     *head;       /* Head of client wait queue */
    CLIENTWAIT     *tail;       /* Tail of client wait queue */
    size_t          cnt;        /* Number of clients waiting */
} CLIENTWAITQ;

/* CAN Init Device Structure */
typedef struct can_dev_init_entry
{
    CANDEV_TYPE     devtype;    /* CAN device type */
    int             can_unit;   /* CAN unit number */
    int             dev_unit;   /* Device unit number */
    uint32_t        msgq_size;  /* Number of messages */
    uint32_t        waitq_size; /* Length of CAN message data */
    CANDEV_MODE     mode;       /* CAN driver mode - I/O or raw frames */
} CANDEV_INIT;

/* Generic CAN Device Structure */
typedef struct can_dev_entry
{
    iofunc_attr_t   attr;
    iofunc_mount_t  mount;
    CANDEV_TYPE     devtype;            /* CAN device type */
    int             can_unit;           /* CAN unit number */
    int             dev_unit;           /* Device unit number */
    CANDEV_MODE     mode;               /* CAN driver mode - I/O or raw frames */
    canmsg_list_t   *free_queue;        /* Free element queue */
    canmsg_list_t   *msg_queue;         /* Active tx or rx queue */
    int             rsmid;              /* Resource manager link ID */
    uint32_t        cflags;             /* CAN device flags
                                           - bit[31]= [0: Standard CAN | 1: CAN FD] */
    struct sigevent event;              /* Device event */
    CLIENTWAITQ     *wait_free_queue;   /* Client wait queue free elems */
    CLIENTWAITQ     *wait_client_queue; /* Blocked Clients queue */
} CANDEV;

/* CAN Device Driver Implemented Functions */
typedef struct _can_drvr_funcs_t
{
    void    (*transmit)     (CANDEV *cdev);
    int     (*devctl)       (CANDEV *cdev, io_devctl_t *msg);
    void    (*transmit_raw) (CANDEV *cdev, CAN_MSG msg);
    void    (*receive_raw)  (CANDEV *cdev, CAN_MSG msg, uint32_t is_blocking);
    CANDEV* (*event_handler)(void* const hdl, const int code);
} can_drvr_funcs_t;

/* Library functions available to driver */
int  can_resmgr_init(can_drvr_funcs_t* const drvr_funcs, const int verbosity);
void can_resmgr_fini(void);
void can_resmgr_start(void);
int  can_resmgr_create_device(CANDEV* cdev);
void can_resmgr_destroy_device(CANDEV* const cdev);
int  can_resmgr_init_device(CANDEV* cdev, CANDEV_INIT* const devinit);
int  can_resmgr_attach_intr(void* const hdl, const int irq, const short pulse_code);
void can_resmgr_detach_intr(const int iid, const int pulse_code);

void can_slogf(const int level, const char *fmt, ...);

/* Queueing function for two queue mode (free queue + data queue) */
canmsg_t *canmsg_dequeue_element(canmsg_list_t* const ml);
void      canmsg_queue_element(canmsg_list_t* const ml, canmsg_t* const queue_elem);

#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/lib/can/public/hw/libcan.h $ $Rev: 976683 $")
#endif
