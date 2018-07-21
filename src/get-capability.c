/* SPDK-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include "tcg2-protocol.h"
#include "tcg2-util.h"

EFI_STATUS EFIAPI
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

    InitializeLib (ImageHandle, SystemTable);
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
