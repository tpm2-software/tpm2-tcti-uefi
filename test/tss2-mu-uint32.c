/* SPDX-License-Identifier: BSD-2 */
#include <inttypes.h>
#include <stdint.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include <tss2/tss2_mu.h>

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    size_t offset = 0;
    UINT32 tmp = 0xdeadbeef, tmp_marsh = 0;
    TSS2_RC rc;

    InitializeLib (ImageHandle, SystemTable);
    Print (L"unmarshalled: 0x%" PRIx32 "\n", tmp);
    rc = Tss2_MU_UINT32_Marshal (tmp, (uint8_t*)&tmp_marsh, sizeof (tmp_marsh), &offset);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"failed to marshal UINT32: RC: 0x%" PRIx32 "\n", rc);
        return EFI_LOAD_ERROR;
    }
    Print (L"marshalled: 0x%" PRIx32 "\n", tmp_marsh);
    return EFI_SUCCESS;
}
