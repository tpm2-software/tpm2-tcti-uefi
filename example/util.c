/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include "tcg2-protocol.h"
#include "tcg2-util.h"

#define TRUE_STR L"true"
#define FALSE_STR L"false"

WCHAR*
bitmap_val_str (UINT32 member, UINT32 selector)
{
    if (member & selector) {
        return TRUE_STR;
    } else {
        return FALSE_STR;
    }
}

void EFIAPI
tcg2_algorithm_bitmap_prettyprint (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap)
{
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA1: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA1));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA256: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA256));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA384: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA384));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA512: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA512));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SM3_256: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SM3_256));
}
