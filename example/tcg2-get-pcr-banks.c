/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

#include <inttypes.h>
#include <string.h>

#include "tcg2-util.h"
#include "util.h"

#define TRUE_STR L"true"
#define FALSE_STR L"false"

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    EFI_STATUS status;
    EFI_TCG2_BOOT_SERVICE_CAPABILITY bs_caps = {
        .Size = sizeof (EFI_TCG2_BOOT_SERVICE_CAPABILITY),
    };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = 0;
    UINT8 proto_major, proto_minor;

#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status)) {
        return status;
    }

    status = tcg2_get_capability (tcg2_protocol, &bs_caps);
    if (EFI_ERROR (status)) {
        return status;
    }
    proto_major = bs_caps.ProtocolVersion.Major;
    proto_minor = bs_caps.ProtocolVersion.Minor;
    if (proto_major < 1 || (proto_minor == 1 && proto_minor < 1)) {
        Print (L"ERROR: GetPcrBanks not supported in protocol version %"
               PRIu8 ".%" PRIu8 "\n", proto_major, proto_minor);
        return 1;
    }

    status = tcg2_get_active_pcr_banks (tcg2_protocol, &active_banks);
    if (EFI_ERROR (status)) {
        return status;
    }
    if (active_banks != bs_caps.ActivePcrBanks) {
        Print (L"WARNING: GetActivePcrBanks disagrees with "
                "EFI_TCG2_BOOT_SERVICE_CAPABILITY.ActivePcrBanks.\n");
    }
    Print (L"GetActivePcrBanks: 0x%08" PRIx32 "\n", active_banks);
    tcg2_algorithm_bitmap_prettyprint (active_banks);
    return status;
}
