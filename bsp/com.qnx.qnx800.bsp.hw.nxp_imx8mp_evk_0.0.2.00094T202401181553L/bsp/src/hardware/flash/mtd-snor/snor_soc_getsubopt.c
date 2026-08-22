/*
 * Copyright (c) 2022, BlackBerry Limited.
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
** File: snor_soc_getsubopt.c
**
** Description:
**
** This file contains the SoC specific command line option parser
**
** Ident: $Id: snor_soc_getsubopt.c $
*/

/*
** Includes
*/

#include "f3s_snor.h"

/**
 *  @brief             Parse SoC suboptions from a string.
 *  @param optionp     Option pointer
 *  @param tokens      Tokens
 *  @param valuep      Value pointer
 *
 *  @return            Index of suboption, -1 if isn't in the tokens vector.
 */
int snor_soc_getsubopt(char **optionp, char* const *tokens, char **valuep)
{
    char        *p, *opt;
    int         len, index;
    const char  *token;

    *valuep = NULL;

    opt = *optionp;
    len = 0;
    for (p = opt; *p && (*p != ':'); p++) {
        if (*p == '=') {
            for (*valuep = ++p; *p && (*p != ':'); p++) {
                /* Nothing to do */
            }
            break;
        }
        len++;
    }

    if (*p) {
        *p++ = '\0';
    }
    *optionp = p;

//    for (index = 0; (token = *tokens++); index++) {
    index = 0;
    while (*tokens != NULL) {
        token = *tokens++;
        if ((*token == *opt) && !strncmp(token, opt, (size_t)len) && (token[len] == '\0')) {
            return (index);
        }
        index++;
    }

    *valuep = opt;

    return (-1);
}


/*
** End
*/

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif
