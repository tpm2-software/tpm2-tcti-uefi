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
CHAR16* EFIAPI
eventtype_to_string (TCG_EVENTTYPE event_type)
{
    switch (event_type) {
    case EV_PREBOOT_CERT:
        return L"EV_PREBOOT_CERT";
    case EV_POST_CODE:
        return L"EV_POST_CODE";
    case EV_UNUSED:
        return L"EV_UNUSED";
    case EV_NO_ACTION:
        return L"EV_NO_ACTION";
    case EV_SEPARATOR:
        return L"EV_SEPARATOR";
    case EV_ACTION:
        return L"EV_ACTION";
    case EV_EVENT_TAG:
        return L"EV_EVENT_TAG";
    case EV_S_CRTM_CONTENTS:
        return L"EV_S_CRTM_CONTENTS";
    case EV_S_CRTM_VERSION:
        return L"EV_S_CRTM_VERSION";
    case EV_CPU_MICROCODE:
        return L"EV_CPU_MICROCODE";
    case EV_PLATFORM_CONFIG_FLAGS:
        return L"EV_PLATFORM_CONFIG_FLAGS";
    case EV_TABLE_OF_DEVICES:
        return L"EV_TABLE_OF_DEVICES";
    case EV_COMPACT_HASH:
        return L"EV_COMPACT_HASH";
    case EV_IPL:
        return L"EV_IPL";
    case EV_IPL_PARTITION_DATA:
        return L"EV_IPL_PARTITION_DATA";
    case EV_NONHOST_CODE:
        return L"EV_NONHOST_CODE";
    case EV_NONHOST_CONFIG:
        return L"EV_NONHOST_CONFIG";
    case EV_NONHOST_INFO:
        return L"EV_NONHOST_INFO";
    case EV_OMIT_BOOT_DEVICE_EVENTS:
        return L"EV_OMIT_BOOT_DEVICE_EVENTS";
    case EV_EFI_VARIABLE_DRIVER_CONFIG:
        return L"EV_EFI_VARIABLE_DRIVER_CONFIG";
    case EV_EFI_VARIABLE_BOOT:
        return L"EV_EFI_VARIABLE_BOOT";
    case EV_EFI_BOOT_SERVICES_APPLICATION:
        return L"EV_EFI_BOOT_SERVICES_APPLICATION";
    case EV_EFI_BOOT_SERVICES_DRIVER:
        return L"EV_EFI_BOOT_SERVICES_DRIVER";
    case EV_EFI_RUNTIME_SERVICES_DRIVER:
        return L"EV_EFI_RUNTIME_SERVICES_DRIVER";
    case EV_EFI_GPT_EVENT:
        return L"EV_EFI_GPT_EVENT";
    case EV_EFI_ACTION:
        return L"EV_EFI_ACTION";
    case EV_EFI_PLATFORM_FIRMWARE_BLOB:
        return L"EV_EFI_PLATFORM_FIRMWARE_BLOB";
    case EV_EFI_HANDOFF_TABLES:
        return L"EV_EFI_HANDOFF_TABLES";
    case EV_EFI_VARIABLE_AUTHORITY:
        return L"EV_EFI_VARIABLE_AUTHORITY";
    default:
        return L"Unknown event type";
    }
}
