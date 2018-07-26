/* SPDX-License-Identifier: BSD-2 */
#ifndef TCTI_UEFI_H
#define TCTI_UEFI_H

#include <tss2/tss2_tcti.h>

#define TCTI_UEFI_MAGIC 0x3a2e16965be011f2
#define TCTI_UEFI_VERSION 2

typedef enum {
    UEFI_STATE_FINAL,
    UEFI_STATE_RECEIVE,
    UEFI_STATE_TRANSMIT,
} tcti_uefi_state_t;

typedef struct {
    TSS2_TCTI_CONTEXT_COMMON_V2 common;
    tcti_uefi_state_t state;
    UINTN size;
    BYTE cmd_buf[];
} TSS2_TCTI_UEFI_CONTEXT;

TSS2_RC EFIAPI
tcti_uefi_init (TSS2_TCTI_CONTEXT **ctx);
TSS2_RC EFIAPI
tcti_uefi_receive (TSS2_TCTI_CONTEXT *context,
                   size_t *size,
                   uint8_t *response,
                   int32_t timeout);
TSS2_RC EFIAPI
tcti_uefi_transmit (TSS2_TCTI_CONTEXT *context,
                    size_t size,
                    const uint8_t *command);
void EFIAPI
tcti_uefi_finalize (TSS2_TCTI_CONTEXT *context);

#endif /* TCTI_UEFI_H */
