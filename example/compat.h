#ifndef UEFI_WRAPS_H
#define UEFI_WRAPS_H

#include <stdlib.h>

int
isspace (int c);
void*
memcpy (void *dest,
        const void *src,
        size_t count);

#endif
