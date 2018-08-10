/* SPDK-License-Identifier: BSD-2 */
#include <inttypes.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include <tss2/tss2_tpm2_types.h>
#include <tss2/tss2_tcti.h>

#include "tcg2-util.h"
#include "tss2-tcti-uefi.h"
#include "tcti-uefi.h"

#define TPM2_HEADER_SIZE 10

/*
 * This function wraps the "up-cast" of the opaque TCTI context type to the
 * type for the device TCTI context. The only safe-guard we have to ensure
 * this operation is possible is the magic number for the device TCTI context.
 * If passed a NULL context, or the magic number check fails, this function
 * will return NULL.
 */
TSS2_TCTI_UEFI_CONTEXT*
tcti_uefi_context_cast (TSS2_TCTI_CONTEXT *tcti_ctx)
{
    if (tcti_ctx != NULL &&
        TSS2_TCTI_MAGIC (tcti_ctx) == TCTI_UEFI_MAGIC &&
        TSS2_TCTI_VERSION (tcti_ctx) == TCTI_UEFI_VERSION) {
        return (TSS2_TCTI_UEFI_CONTEXT*)tcti_ctx;
    }
    return NULL;
}
/*
 * This function down-casts the device TCTI context to the common context
 * defined in the tcti-common module.
 */
TSS2_TCTI_CONTEXT_COMMON_V2*
tcti_uefi_down_cast (TSS2_TCTI_UEFI_CONTEXT *tcti_uefi)
{
    if (tcti_uefi == NULL) {
        return NULL;
    }

    return &tcti_uefi->common;
}
/*
 * Transmit function is a bit weird. This is the result of an impedance
 * mismatch between the TCG2 UEFI protocol / API and the TCTI API.
 * The prior only synchronous, the later separates the transmit and receive
 * functions. This means we must keep a copy of the buffer in ternally so
 * that we can send it in the 'receive' function.
 */
TSS2_RC EFIAPI
tcti_uefi_transmit (TSS2_TCTI_CONTEXT *context,
                    size_t size,
                    const uint8_t *command)
{
    TSS2_TCTI_UEFI_CONTEXT *tcti_uefi;

    if (context == NULL || command == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }
    tcti_uefi = tcti_uefi_context_cast (context);
    if (tcti_uefi == NULL) {
        Print (L"upcast to TSS2_TCTI_UEFI_CONTEXT failed\n");
        return TSS2_TCTI_RC_BAD_CONTEXT;
    }
    if (tcti_uefi->state != UEFI_STATE_TRANSMIT) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }
    if (size > tcti_uefi->size || size <= TPM2_HEADER_SIZE) {
        return TSS2_TCTI_RC_BAD_VALUE;
    }
    CopyMem (tcti_uefi->cmd_buf,
             command,
             size);
    tcti_uefi->state = UEFI_STATE_RECEIVE;

    return TSS2_RC_SUCCESS;
}
/*
 * Due to the synchronous nature of the TCG2 UEFI protocol this
 * function can only doing blocking I/O. We transmit the stored
 * command buffer from the 'transmit' function.
 */
TSS2_RC EFIAPI
tcti_uefi_receive (TSS2_TCTI_CONTEXT *context,
                   size_t *size,
                   uint8_t *response,
                   int32_t timeout)
{
    TSS2_TCTI_UEFI_CONTEXT *tcti_uefi;
    EFI_TCG2_PROTOCOL *tcg2_proto;
    EFI_STATUS status;

    if (context == NULL || size == NULL) {
        return TSS2_TCTI_RC_BAD_REFERENCE;
    }
    /* The TCG2 UEFI protocol is only capable of blocking I/O */
    if (timeout != TSS2_TCTI_TIMEOUT_BLOCK) {
        return TSS2_TCTI_RC_NOT_IMPLEMENTED;
    }
    tcti_uefi = tcti_uefi_context_cast (context);
    if (tcti_uefi == NULL) {
        return TSS2_TCTI_RC_BAD_CONTEXT;
    }
    if (tcti_uefi->state != UEFI_STATE_RECEIVE) {
        return TSS2_TCTI_RC_BAD_SEQUENCE;
    }
    if (response == NULL) {
        *size = tcti_uefi->size;
        return TSS2_RC_SUCCESS;
    }

    status = tcg2_get_protocol (&tcg2_proto);
    if (EFI_ERROR (status)) {
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    }

    status = tcg2_submit_command (tcg2_proto,
                                  tcti_uefi->size,
                                  tcti_uefi->cmd_buf,
                                  *size,
                                  response);
    if (status == EFI_SUCCESS) {
        tcti_uefi->state = UEFI_STATE_TRANSMIT;
        return TSS2_RC_SUCCESS;
    }
    switch (status) {
    case EFI_INVALID_PARAMETER:
        Print (L"EFI_INVALID_PARAMTER\n");
        return TSS2_TCTI_RC_BAD_VALUE;
    case EFI_BUFFER_TOO_SMALL:
        Print (L"EFI_BUFFER_TOO_SMALL\n");
        return TSS2_TCTI_RC_INSUFFICIENT_BUFFER;
    case EFI_DEVICE_ERROR:
        Print (L"EFI_DEVICE_ERROR\n");
        return TSS2_TCTI_RC_IO_ERROR;
    case EFI_ACCESS_DENIED:
        Print (L"EFI_ACCESS_DENIED\n");
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    default:
        Print (L"Unexpected EFI error code\n");
        return TSS2_TCTI_RC_GENERAL_FAILURE;
    }
}
/*
 * Finalize function only validates the context structure before setting the
 * state to 'final'.
 */
void EFIAPI
tcti_uefi_finalize (TSS2_TCTI_CONTEXT *context)
{
    TSS2_TCTI_UEFI_CONTEXT *tcti_uefi;
    if (context == NULL) {
        Print (L"Invalid parameter");
        return;
    }
    tcti_uefi = tcti_uefi_context_cast (context);
    if (tcti_uefi != NULL) {
        tcti_uefi->state = UEFI_STATE_FINAL;
    }
}
/*
 * This utility function queries the TCG2 UEFI protocol for the largest
 * command and response buffer supported, then adds that value to the size
 * of the TSS2_TCTI_UEFI_CONTEXT structure.
 */
size_t
sizeof_tcti_uefi_context (void)
{
    EFI_STATUS status;
    EFI_TCG2_PROTOCOL *tcg2_proto;
    size_t size = 0;

    status = tcg2_get_protocol (&tcg2_proto);
    if (EFI_ERROR (status)) {
        return 0;
    }
    size += tcg2_get_max_buf (tcg2_proto);
    if (size == 0) {
        return 0;
    }
    size += sizeof (TSS2_TCTI_UEFI_CONTEXT);
    return size;
}
/*
 * Initialization function compliant with TCTI spec.
 */
TSS2_RC
Tss2_Tcti_Uefi_Init (TSS2_TCTI_CONTEXT *context,
                     size_t *size,
                     const char *conf)
{
    TSS2_TCTI_UEFI_CONTEXT *tcti_uefi;

    if (size == NULL) {
        return TSS2_TCTI_RC_BAD_VALUE;
    } else if (context == NULL) {
        *size = sizeof_tcti_uefi_context ();
        if (*size == 0) {
            return TSS2_TCTI_RC_GENERAL_FAILURE;
        }
        return TSS2_RC_SUCCESS;
    }

    TSS2_TCTI_MAGIC (context) = TCTI_UEFI_MAGIC;
    TSS2_TCTI_VERSION (context) = TCTI_UEFI_VERSION;

    tcti_uefi = tcti_uefi_context_cast (context);

    TSS2_TCTI_TRANSMIT (context) = tcti_uefi_transmit;
    TSS2_TCTI_RECEIVE (context) = tcti_uefi_receive;
    TSS2_TCTI_FINALIZE (context) = tcti_uefi_finalize;
    TSS2_TCTI_CANCEL (context) = NULL;
    TSS2_TCTI_GET_POLL_HANDLES (context) = NULL;
    TSS2_TCTI_SET_LOCALITY (context) = NULL;
    TSS2_TCTI_MAKE_STICKY (context) = NULL;
    tcti_uefi->state = UEFI_STATE_TRANSMIT;

    /*
     * The size of the buffer we have to work with is the size of the
     * provided context less the size of the context structure itself. The
     * size of this buffer may be less than the max buffer allowed by the
     * UEFI TCG2 protocol.
     */
    tcti_uefi->size = *size - sizeof (TSS2_TCTI_UEFI_CONTEXT);

    return TSS2_RC_SUCCESS;
}
/*
 * Utility function that does the memory allocation for the context structure.
 */
TSS2_RC EFIAPI
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
