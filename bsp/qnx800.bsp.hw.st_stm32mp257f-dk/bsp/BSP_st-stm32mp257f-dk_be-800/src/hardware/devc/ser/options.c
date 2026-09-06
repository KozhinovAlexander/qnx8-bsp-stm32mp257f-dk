/*
 * Copyright (c) 2007, 2008, 2023, BlackBerry Limited.
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

#include "proto.h"

unsigned
ser_options(const int argc, char *argv[])
{
    int         opt;
    int         numports = 0;
    unsigned    rx_fifo = 24;    // default
    unsigned    tx_fifo = 32;    // set max, to use TRDY status as a FIFO full indicator

    static TTYINIT devinit = {
        .port = 0,
        .port_shift = 0,
        .intr = -1,
        .baud = 115200,
        .isize = 2048,
        .osize = 2048,
        .csize = 256,
        .c_cflag = 0,
        .c_iflag = 0,
        .c_lflag = 0,
        .c_oflag = 0,
        .fifo = 0,
        .clk = 96000000,
        .div = 16,
        .name = "/dev/ser",
        .reserved1 = NULL,
        .reserved2 = 0,
        .verbose = 0,
        .highwater = 0,
        .logging_path = "",
        .lflags = 0,
        .unit = 1
    };

    /*
     * Initialize the devinit to raw mode
     */
    ttc(TTC_INIT_RAW, &devinit, 0);

    while (optind < argc) {
        /*
         * Process dash options.
         * Options already used by io-char (do not use these!): b,c,e,E,f,F,s,S,C,I,O,o,u,v
         */
        while ((opt = getopt(argc, argv, IO_CHAR_SERIAL_OPTIONS "t:T:")) != -1) {
            switch (ttc(TTC_SET_OPTION, &devinit, opt)) {
                case 't':
                    rx_fifo = (unsigned)strtoul(optarg, NULL, 0);
                    if (rx_fifo > MX1_UART_FIFO_SIZE) {
                        fprintf(stderr, "FIFO trigger must be <= 32.\n");
                        fprintf(stderr, "Will disable FIFO.\n");
                        rx_fifo = 0;
                    }
                break;

                case 'T':
                    tx_fifo = (unsigned)strtoul(optarg, NULL, 0);
                    if ((tx_fifo > MX1_UART_FIFO_SIZE) || (tx_fifo < MX1_UART_MIN_TX_FIFO)) {
                        fprintf(stderr, "Tx fifo size must be >= 2 and <= 32.\n");
                        fprintf(stderr, "Using tx fifo size of 32\n");
                        tx_fifo = MX1_UART_MIN_TX_FIFO;
                    }
                    break;

                default:
                    break;
            }
        }

        devinit.fifo = (int)(rx_fifo | (tx_fifo << 10));

        /*
         * Process ports and interrupts.
         */
        while ((optind < argc) && (*argv[optind] != '-')) {
            optarg = argv[optind];
            devinit.port = strtoul(optarg, &optarg, 16);
            if (*optarg == ',') {
                devinit.intr = (unsigned)strtoul(optarg + 1, &optarg, 0);
            }

            if ((devinit.port != 0) && (devinit.intr != (unsigned)-1)) {
                create_device(&devinit);
                devinit.unit++;
                ++numports;
            }
            ++optind;
        }
    }

    return numports;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devc/sermx1/options.c $ $Rev: 987527 $")
#endif
