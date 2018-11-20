/* SPDX-License-Identifier: BSD-2 */
#ifndef UEFI_TYPES_H
#define UEFI_TYPES_H

#if ARCH == x86_64
#include <efi/x86_64/efibind.h>
#elif ARCH == ia32
#include <efi/ia32/efibind.h>
#else
#error "Unsupported ARCH."
#endif

#include <efi/efidef.h>

#endif /* UEFI_TYPES_H */
