/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#endif

#include <tss2/tss2_tcti.h>
#include <tss2/tss2_sys.h>

#include "tss2-tcti-uefi.h"

/*
 * Utility function that does the memory allocation for the context structure.
 */
TSS2_RC
tcti_uefi_init (TSS2_TCTI_CONTEXT **ctx)
{
    TSS2_RC rc;
    size_t size;

    rc = Tss2_Tcti_Uefi_Init (NULL, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }

    *ctx = AllocatePool (size);
    if (*ctx == NULL) {
        Print (L"Failed to allocate 0x%x bytes\n", size);
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    }

    rc = Tss2_Tcti_Uefi_Init (*ctx, &size, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        FreePool (*ctx);
        return rc;
    }

    return rc;
}
/*
 * Teardown / Finalize TCTI context and free memory.
 */
void
tcti_uefi_teardown (TSS2_TCTI_CONTEXT *tcti_ctx)
{
    if (tcti_ctx) {
        Tss2_Tcti_Finalize (tcti_ctx);
        FreePool (tcti_ctx);
    }
}
/*
 * Initialize a SAPI context using the TCTI context provided by the caller.
 * This function allocates memory for the SAPI context and returns it to the
 * caller. This memory must be freed by the caller.
 */
TSS2_RC
sys_uefi_init_from_tcti_ctx (TSS2_TCTI_CONTEXT *tcti_ctx,
                             TSS2_SYS_CONTEXT **sys_ctx)
{
    TSS2_SYS_CONTEXT *sys_ctx_tmp;
    TSS2_RC rc;
    size_t size;
    TSS2_ABI_VERSION abi_version = {
        .tssCreator = 1,
        .tssFamily = 2,
        .tssLevel = 1,
        .tssVersion = 108,
    };

    size = Tss2_Sys_GetContextSize(0);
    sys_ctx_tmp = AllocatePool (size);
    if (sys_ctx_tmp == NULL) {
        Print (L"Failed to allocate 0x%zx bytes for the SAPI context\n", size);
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    }
    rc = Tss2_Sys_Initialize(sys_ctx_tmp, size, tcti_ctx, &abi_version);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"Failed to initialize SAPI context: 0x%x\n", rc);
        FreePool (sys_ctx_tmp);
        return rc;
    }

    *sys_ctx = sys_ctx_tmp;
    return TSS2_RC_SUCCESS;
}
TSS2_RC
sys_uefi_init (TSS2_SYS_CONTEXT **sys_ctx)
{
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;

    rc = tcti_uefi_init (&tcti_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        return rc;
    }

    return sys_uefi_init_from_tcti_ctx (tcti_ctx, sys_ctx);
}
/*
 * Teardown and free the resources associated with a SAPI context structure.
 */
void
sys_uefi_teardown (TSS2_SYS_CONTEXT *sys_ctx)
{
    if (sys_ctx) {
        Tss2_Sys_Finalize (sys_ctx);
        FreePool (sys_ctx);
    }
}
/*
 * Teardown and free the resources associated with a SAPI context structure.
 * This includes tearing down the TCTI as well.
 */
void
sys_uefi_teardown_full (TSS2_SYS_CONTEXT *sys_ctx)
{
    TSS2_TCTI_CONTEXT *tcti_ctx = NULL;
    TSS2_RC rc;

    if (sys_ctx == NULL) {
        return;
    }

    rc = Tss2_Sys_GetTctiContext (sys_ctx, &tcti_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"Tss2_Sys_GetTctiContext failed\n");
        return;
    }

    sys_uefi_teardown (sys_ctx);
    tcti_uefi_teardown (tcti_ctx);
}
