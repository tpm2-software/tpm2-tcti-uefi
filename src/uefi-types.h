/* SPDX-License-Identifier: BSD-2 */
#ifndef UEFI_TYPES_H
#define UEFI_TYPES_H

#ifndef EDK2_BUILD
#ifdef ARCH_x86_64
#include <efi/x86_64/efibind.h>
#endif
#ifdef ARCH_ia32
#include <efi/ia32/efibind.h>
#endif
#ifdef ARCH_aarch64
#include <efi/aarch64/efibind.h>
#endif
#if !defined(ARCH_x86_64) && !defined(ARCH_ia32) && !defined(ARCH_aarch64)
#error "Unsupported ARCH."
#endif
#include <efi/efidef.h>
#endif

#endif /* UEFI_TYPES_H */
