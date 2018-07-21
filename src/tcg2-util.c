/* SPDK-License-Identifier: BSD-2 */
#include <inttypes.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include "tcg2-protocol.h"

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
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA1: %s\n",
        bitmap_val_str (caps->HashAlgorithmBitmap, EFI_TCG2_BOOT_HASH_ALG_SHA1));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA256: %s\n",
        bitmap_val_str (caps->HashAlgorithmBitmap, EFI_TCG2_BOOT_HASH_ALG_SHA256));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA384: %s\n",
        bitmap_val_str (caps->HashAlgorithmBitmap, EFI_TCG2_BOOT_HASH_ALG_SHA384));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA512: %s\n",
        bitmap_val_str (caps->HashAlgorithmBitmap, EFI_TCG2_BOOT_HASH_ALG_SHA512));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SM3_256: %s\n",
        bitmap_val_str (caps->HashAlgorithmBitmap, EFI_TCG2_BOOT_HASH_ALG_SM3_256));
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
