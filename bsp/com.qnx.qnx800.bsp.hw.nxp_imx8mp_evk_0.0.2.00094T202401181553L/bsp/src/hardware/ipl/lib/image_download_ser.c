/*
 * Copyright (c) 2007, 2008, 2022-2023, BlackBerry Limited.
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


#include "ipl.h"
#include <inttypes.h>

typedef struct  data_record {
    uint8_t     cmd;
    uint8_t     seq;
    uint8_t     cksum;
    uint8_t     nbytes;
    uint32_t    daddr;
} data_record_t;

#define DATA_RECORD_HEADER      8

#define START_CMD               0x80
#define DATA_CMD                0x81
#define GO_CMD                  0x82
#define ECHO_CMD                0x83
#define ABORT_CMD               0x88

#define ABORT_CKSUM             1
#define ABORT_SEQ               2
#define ABORT_PROTOCOL          3

static void
download_abort(const char abort)
{
    ser_putchar((char)ABORT_CMD);
    ser_putchar(abort);
}

int
image_download_ser(const paddr_t dst_address)
{
    char            seq = 0;
    data_record_t   record = {0};
    char            *src;
    char            *dst;

    /*
     * set destination address within memory
     */
    dst = (char *)dst_address;

    /*
     * Wait for initial start record
     */
    while (ser_getchar() != START_CMD) {
    }

    while (1) {
        /*
         * start processing the data/go records
         */
        record.cmd = (uint8_t)ser_getchar();

        if (record.cmd != DATA_CMD) {
            /*
             * check for a GO cmd to return control to the IPL
             */
            if (record.cmd == GO_CMD) {
                /*
                 * eat up the rest of the record
                 */
                for (int idx = 0; idx < (DATA_RECORD_HEADER - 1); idx++) {
                    ser_getchar();
                }
                return (0);
            }

            if (record.cmd == ECHO_CMD) {
                ser_putchar((char)ECHO_CMD);
                continue;
            }

            download_abort(ABORT_PROTOCOL);

            return (1);
        }

        /*
         *  read data_record header
         *  (DATA_RECORD_HEADER -1 since cmd already consumed by get_byte)
         */
        src = (char *)&record.seq;

        for (int idx = 0; idx < (DATA_RECORD_HEADER - 1); idx++) {
            *src = ser_getchar();
            src++;
        }

        if (seq != (char)record.seq) {
            download_abort(ABORT_SEQ);
            return (1);
        }

        seq = (seq + 1) & 0x7f;

        /*
         * Get rest of data
         */
        for (uint8_t idx = 0; idx <= record.nbytes; idx++) {
            *dst = ser_getchar();
            dst++;
        }
    }
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/ipl/lib/image_download_ser.c $ $Rev: 975396 $")
#endif
