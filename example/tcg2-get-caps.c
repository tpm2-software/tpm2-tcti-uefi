/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

#include "tcg2-protocol.h"
#include "tcg2-util.h"
#include "util.h"

void
tcg2_caps_prettyprint (EFI_TCG2_BOOT_SERVICE_CAPABILITY *caps)
{
    Print (L"TPM2 Capabilities:\n");
    Print (L"  Size: 0x%02x\n", caps->Size);
    Print (L"  StructureVersion:\n");
    Print (L"    Major: 0x%02x\n", caps->StructureVersion.Major);
    Print (L"    Minor: 0x%02x\n", caps->StructureVersion.Minor);
    Print (L"  ProtocolVersion:\n");
    Print (L"    Major: 0x%02x\n", caps->ProtocolVersion.Major);
    Print (L"    Minor: 0x%02x\n", caps->ProtocolVersion.Minor);
    Print (L"  HashAlgorithmBitmap: 0x%08x\n",
           caps->HashAlgorithmBitmap);
    tcg2_algorithm_bitmap_prettyprint (caps->HashAlgorithmBitmap);
    Print (L"  SupportedEventLogs: 0x%08x\n", caps->SupportedEventLogs);
    Print (L"    EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2: %s\n",
        bitmap_val_str (caps->SupportedEventLogs, EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2));
    Print (L"    EFI_TCG2_EVENT_LOG_FORMAT_TCG_2: %s\n",
        bitmap_val_str (caps->SupportedEventLogs, EFI_TCG2_EVENT_LOG_FORMAT_TCG_2));
    Print (L"  TPMPresentFlag: 0x%02x : %s\n",
        caps->TPMPresentFlag, bitmap_val_str (caps->TPMPresentFlag, 1));
    Print (L"  MaxCommandSize: 0x%04x\n", caps->MaxCommandSize);
    Print (L"  MaxResponseSize: 0x%04x\n", caps->MaxResponseSize);
    Print (L"  ManufacturerID: 0x%08x\n", caps->ManufacturerID);
    Print (L"  NumberOfPcrBanks: 0x%08x\n", caps->NumberOfPcrBanks);
    Print (L"  ActivePcrBanks: 0x%08x\n", caps->ActivePcrBanks);
    tcg2_algorithm_bitmap_prettyprint (caps->ActivePcrBanks);
}

#if !defined(uefi_call_wrapper)
#define uefi_call_wrapper(func, va_num, ...) func(__VA_ARGS__)
#endif

EFI_STATUS
get_capability_tcg2 (
    EFI_TCG2_PROTOCOL *tcg2_protocol,
    EFI_TCG2_BOOT_SERVICE_CAPABILITY *tcg2_bs_caps
    )
{
    EFI_STATUS status;

    status = uefi_call_wrapper (tcg2_protocol->GetCapability,
                                2,
                                tcg2_protocol,
                                tcg2_bs_caps);
    if (EFI_ERROR (status)) {
        Print (L"GetCapability failed: 0x%x\n", status);
    }

    return status;
}

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    EFI_STATUS status;
    EFI_TCG2_BOOT_SERVICE_CAPABILITY tcg2_bs_caps = {
        .Size = sizeof (EFI_TCG2_BOOT_SERVICE_CAPABILITY),
    };

#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status)) {
        return status;
    }

    status = tcg2_get_capability (tcg2_protocol, &tcg2_bs_caps);
    if (EFI_ERROR (status)) {
        return status;
    }
    tcg2_caps_prettyprint (&tcg2_bs_caps);

    return status;
}
