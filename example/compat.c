/* SPDX-License-Identifier: BSD-2 */
#include <stdlib.h>

#include <efi/efi.h>
#include <efi/efilib.h>

/*
 */
int
isspace (int c)
{
    switch (c) {
    case ' ':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
        return 1;
    }
    return 0;
}
void*
memmove (void *dest,
         const void *src,
         size_t count)
{
    CopyMem (dest, src, count);
    return dest;
}
