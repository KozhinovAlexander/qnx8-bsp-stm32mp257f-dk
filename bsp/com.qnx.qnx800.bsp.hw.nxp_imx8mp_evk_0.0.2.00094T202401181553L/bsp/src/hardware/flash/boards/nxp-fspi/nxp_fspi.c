/*
 * Copyright (c) 2023, BlackBerry Limited.
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
 */

#include "soc/nxp_fspi/nxp_fspi.h"
#include "f3s_snor.h"

/*
 * This is the main function for f3s flash file system.
 */

int main(const int argc, char **const argv)
{
    int    error;
    static f3s_service_t service[] =
    {
        {
            .struct_size = sizeof(f3s_service_t),
            .open = f3s_nxp_fspi_open,        // NXP SoCs with LUT
            .page  = f3s_snor_page,           // generic SNOR page callout
            .status = f3s_snor_status,        // generic SNOR status callout
            .close = f3s_snor_close           // generic SNOR close callout
        },
        {
            0   // mandatory last entry
        }
    };

#if MTD_VER == 2
    static f3s_flash_v2_t flash[] =
    {
        {
            .struct_size = sizeof(f3s_flash_v2_t),
            .ident = f3s_mt35x_ident,        // Ident
            .reset = f3s_snor_reset,         // Common Reset

            // v1 callouts, not supported
            .read  = NULL,
            .write = NULL,
            .erase = NULL,
            .suspend = NULL,
            .resume = NULL,
            .sync = NULL,

            // v2 callouts
            .v2read = f3s_snor_read,          // v2 Read
            .v2write = f3s_snor_program,      // v2 Write
            .v2erase = f3s_snor_erase,        // v2 Erase
            .v2suspend = f3s_snor_suspend,    // v2 Suspend
            .v2resume = f3s_snor_resume,      // v2 Resume
            .v2sync = f3s_snor_sync,          // v2 Sync
            .v2islock = NULL,                 // v2 islock
            .v2lock = NULL,                   // v2 lock
            .v2unlock = NULL,                 // v2 unlock
            .v2unlockall = NULL,              // v2 unlockall
            .v2ssrop = NULL                   // v2 ssrop
        },

        {
            .struct_size = sizeof(f3s_flash_v2_t),
            .ident = f3s_mt25q_ident,        // Ident
            .reset = f3s_snor_reset,         // Common Reset

            // v1 callouts, not supported
            .read  = NULL,
            .write = NULL,
            .erase = NULL,
            .suspend = NULL,
            .resume = NULL,
            .sync = NULL,

            // v2 callouts
            .v2read = f3s_snor_read,          // v2 Read
            .v2write = f3s_snor_program,      // v2 Write
            .v2erase = f3s_snor_erase,        // v2 Erase
            .v2suspend = f3s_snor_suspend,    // v2 Suspend
            .v2resume = f3s_snor_resume,      // v2 Resume
            .v2sync = f3s_snor_sync,          // v2 Sync
            .v2islock = NULL,                 // v2 islock
            .v2lock = NULL,                   // v2 lock
            .v2unlock = NULL,                 // v2 unlock
            .v2unlockall = NULL,              // v2 unlockall
            .v2ssrop = NULL                   // v2 ssrop
        },

        {
           0    // mandatory last entry
        }
    };
#else
#error "MTD version must be 2"
#endif

    /* init f3s */
    f3s_init(argc, argv, (f3s_flash_t *)flash);

    /* start f3s */
    error = f3s_start(service, (f3s_flash_t *)flash);

    return error;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
