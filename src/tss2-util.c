#include <efi/efi.h>
#include <efi/efilib.h>

#include <tss2/tss2_tcti.h>

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
