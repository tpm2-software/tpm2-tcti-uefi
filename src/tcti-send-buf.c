/* SPDK-License-Identifier: BSD-2 */
#include <endian.h>
#include <inttypes.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

#include "tcg2-util.h"
#include "tcti-uefi.h"

#define TPM2_HEADER_SIZE 10

void
tpm2_header_dump (BYTE buf[],
                  size_t buf_size)
{
    TPMI_ST_COMMAND_TAG tag;
    UINT32 out_size = 0, out_code = 0;

    if (buf_size < TPM2_HEADER_SIZE) {
        Print (L"buffer smaller than TPM2 response header, not good\n");
        return;
    }
    tag = be16toh (*(TPMI_ST_COMMAND_TAG*)buf);
    out_size = be32toh (*(UINT32*)(&buf[sizeof (TPMI_ST_COMMAND_TAG)]));
    out_code = be32toh (*(UINT32*)(&buf[sizeof (TPMI_ST_COMMAND_TAG) + sizeof (UINT32)]));
    Print (L"  TPMI_ST_COMMAND_TAG: 0x%x\n  UINT32 size: 0x%x\n  UINT32 "
           L"code: 0x%x\n", tag, out_size, out_code);
}

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    EFI_STATUS status;
    /*
     * this command buffer sends the TPM2_GetCapability command with the
     * TPM2_CAP set to TPM2_CAP_TPM_PROPERTIES and the property set to
     * TPM2_PT_FIXED.
     */
    BYTE cmd_buf [] = {
        0x80, 0x01, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00,
        0x01, 0x7a, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x7f
    };
    BYTE *resp_buf;
    size_t resp_size;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;

    InitializeLib (ImageHandle, SystemTable);
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status)) {
        return status;
    }

    resp_size = tcg2_get_max_buf (tcg2_protocol);
    if (resp_size == 0) {
        Print (L"Failed to get max response buffer size\n");
        return EFI_ABORTED;
    }

    resp_buf = AllocatePool (resp_size);
    if (resp_buf == NULL) {
        Print (L"Failed to AllocatePool\n");
    }

    rc = tcti_uefi_init (&tcti_ctx);
    if (rc != TSS2_RC_SUCCESS || tcti_ctx == NULL) {
        Print (L"failed to Init TCTI\n");
        return EFI_ABORTED;
    }

    Print (L"Tss2_Tcti_Transmit: buffer at 0x%" PRIxPTR " of size 0x%x\n",
           cmd_buf, sizeof (cmd_buf));
    rc = Tss2_Tcti_Transmit (tcti_ctx, sizeof (cmd_buf), cmd_buf);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"Tss2_Tcti_Transmit failed: 0x%x\n", rc);
        return EFI_DEVICE_ERROR;
    }

    rc = Tss2_Tcti_Receive (tcti_ctx, &resp_size, resp_buf, TSS2_TCTI_TIMEOUT_BLOCK);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"Tss2_Tcti_Receive failed: 0x%x\n", rc);
        return EFI_DEVICE_ERROR;
    }

    Print (L"tpm2_header_dump\n");
    tpm2_header_dump (resp_buf, resp_size);

    Print (L"Tss2_Tcti_Finalize\n");
    Tss2_Tcti_Finalize (tcti_ctx);
    FreePool (tcti_ctx);

    return status;
}
