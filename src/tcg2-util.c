/* SPDX-License-Identifier: BSD-2 */
#include <inttypes.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include "tcg2-protocol.h"

EFI_STATUS EFIAPI
tcg2_get_eventlog (EFI_TCG2_PROTOCOL *tpm2_prot,
                   EFI_TCG2_EVENT_LOG_FORMAT format,
                   EFI_PHYSICAL_ADDRESS *first,
                   EFI_PHYSICAL_ADDRESS *last,
                   BOOLEAN *truncated)
{
    EFI_STATUS status;

    status = uefi_call_wrapper (tpm2_prot->GetEventLog,
                                5,
                                tpm2_prot,
                                format,
                                first,
                                last,
                                truncated);
    if (EFI_ERROR (status)) {
        Print (L"Tcg2 SubmitCommand failed: 0x%x\n", status);
    }

    return status;
}

EFI_STATUS EFIAPI
tcg2_get_capability (
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

UINT16 EFIAPI
tcg2_get_max_buf (EFI_TCG2_PROTOCOL *tcg2_protocol)
{
    EFI_TCG2_BOOT_SERVICE_CAPABILITY tcg2_bs_caps = {
        .Size = sizeof (EFI_TCG2_BOOT_SERVICE_CAPABILITY),
    };
    EFI_STATUS status;

    status = tcg2_get_capability (tcg2_protocol, &tcg2_bs_caps);
    if (EFI_ERROR (status)) {
        Print (L"Failed to GetCapability: 0x%x\n", status);
        return 0;
    }

    if (tcg2_bs_caps.MaxCommandSize > tcg2_bs_caps.MaxResponseSize) {
        return tcg2_bs_caps.MaxCommandSize;
    } else {
        return tcg2_bs_caps.MaxResponseSize;
    }
}

EFI_STATUS EFIAPI
tcg2_submit_command (EFI_TCG2_PROTOCOL *tcg2_protocol,
                     UINT32 input_size,
                     UINT8 *input_buf,
                     UINT32 output_size,
                     UINT8 *output_buf)
{
    EFI_STATUS status;

    status = uefi_call_wrapper (tcg2_protocol->SubmitCommand,
                                5,
                                tcg2_protocol,
                                input_size,
                                input_buf,
                                output_size,
                                output_buf);
    if (EFI_ERROR (status)) {
        Print (L"Tcg2 SubmitCommand failed: 0x%x\n", status);
    }

    return status;
}

EFI_STATUS EFIAPI
tcg2_get_protocol (EFI_TCG2_PROTOCOL **tcg2_protocol)
{
    EFI_TCG2_PROTOCOL *tcg2_proto_tmp;
    EFI_GUID tcg2_guid = EFI_TCG2_PROTOCOL_GUID;
    EFI_STATUS status;

    status = LibLocateProtocol (&tcg2_guid, (VOID**)&tcg2_proto_tmp);
    if (!EFI_ERROR (status)) {
        Print (L"LibLocateProtocol returned handle to EFI_TCG2_PROTOCOL: 0x%"
               PRIxPTR "\n", (uintptr_t)tcg2_proto_tmp);
    } else {
        Print (L"LibLocateProtocol status: 0x%x\n", status);
    }
    *tcg2_protocol = tcg2_proto_tmp;

    return status;
}
