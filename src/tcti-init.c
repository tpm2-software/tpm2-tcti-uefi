/* SPDK-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include "tss2-tcti-uefi.h"
#include "tcti-uefi.h"

EFI_STATUS EFIAPI
efi_main (EFI_HANDLE ImageHandle,
          EFI_SYSTEM_TABLE *SystemTable)
{
    TSS2_RC rc;
    TSS2_TCTI_CONTEXT *ctx;

    InitializeLib (ImageHandle, SystemTable);

    rc = tcti_uefi_init (&ctx);
    if (rc == TSS2_RC_SUCCESS) {
        Print (L"UEFI TCTI context initialized successfully.\n");
        FreePool (ctx);
        return EFI_SUCCESS;
    }
    switch (rc) {
    case TSS2_TCTI_RC_GENERAL_FAILURE:
        return EFI_OUT_OF_RESOURCES;
    default:
        return EFI_DEVICE_ERROR;
    }
}
