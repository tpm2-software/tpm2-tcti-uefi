/* SPDX-License-Identifier: BSD-2 */
#ifndef UEFI_WRAPS_H
#define UEFI_WRAPS_H

#include <stdlib.h>

int
isspace (int c);
void*
memcpy (void *dest,
        const void *src,
        size_t count);
void*
memmove (void *dest,
         const void *src,
         size_t count);
void*
memset (void *s,
        int c,
        size_t n);

#endif
