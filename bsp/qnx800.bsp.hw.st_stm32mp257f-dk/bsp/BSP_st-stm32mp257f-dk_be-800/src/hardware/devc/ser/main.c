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

/* Called from io-char's cleanup thread */
int dev_cleanup(TTYDEV *const dev)
{
	DEV_MX1 *const mx1dev = (DEV_MX1 *)dev;

	slogf(_SLOG_SETCODE(_SLOGC_CHAR, 0), _SLOG_ERROR, "%s: Cleaning up device %s", __FUNCTION__, dev->name);

	if (mx1dev->intr != -1) {
		ser_detach_intr(mx1dev);
	}

	return EOK;
}

int
main(const int argc, char *argv[])
{
	ttyctrl.max_devs = 6;
	ttyctrl.flags |= CREATE_CLEANUP_THREAD;
	ttc(TTC_INIT_PROC, &ttyctrl, 24);

	if (ser_options(argc, argv) == 0) {
		fprintf(stderr, "%s: No serial ports found\n", argv[0]);
		exit(0);
	}

	ttc(TTC_INIT_START, &ttyctrl, 0);

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/hardware/branches/release/hardware/devc/sermx1/main.c $ $Rev: 982527 $")
#endif
