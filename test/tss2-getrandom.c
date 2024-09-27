/* SPDX-License-Identifier: BSD-2 */
#include <efi/efierr.h>
#include <inttypes.h>
#include <stdint.h>

#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

#include <tss2-tcti-uefi.h>
#include <tss2/tss2_sys.h>

#define CHECK_RC(rc) { if (rc != TSS2_RC_SUCCESS) { \
        Print(L"Error in Line %i: 0x08" PRIx32 "\n", __LINE__, rc); \
        goto out; } }

#define CHECK_NULL(x) { if (x == NULL) { \
        Print(L"Error in Line %i: OOM\n", __LINE__); \
        rc = 0xffffff; \
        goto out; } }

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    TSS2_RC rc;
    TSS2_TCTI_CONTEXT *tcti = NULL;
    TSS2_SYS_CONTEXT *sys = NULL;
    size_t size;

    TPM2B_DIGEST randomBytes = { 0 };

#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif

    rc = Tss2_Tcti_Uefi_Init (tcti, &size, NULL);
    CHECK_RC(rc);

    tcti = AllocatePool (size);
    CHECK_NULL(tcti);

    rc = Tss2_Tcti_Uefi_Init (tcti, &size, NULL);
    CHECK_RC(rc);

    size = Tss2_Sys_GetContextSize(0);
    sys = AllocatePool(size);
    CHECK_NULL(sys);

    rc = Tss2_Sys_Initialize(sys, size, tcti, NULL);
    CHECK_RC(rc);

    rc = Tss2_Sys_GetRandom(sys, NULL, 32, &randomBytes, NULL);
    CHECK_RC(rc);

    /* Calling again to test state checking in command sequences */
    rc = Tss2_Sys_GetRandom(sys, NULL, 32, &randomBytes, NULL);
    CHECK_RC(rc);

out:
    Tss2_Sys_Finalize(sys);
    Tss2_Tcti_Finalize(tcti);

    Print (L"Done\n");
    return (rc == TSS2_RC_SUCCESS)? EFI_SUCCESS : EFI_LOAD_ERROR;
}
