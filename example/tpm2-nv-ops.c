/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

#include <inttypes.h>
#include <string.h>

#include <tss2/tss2_mu.h>
#include <tss2/tss2_sys.h>

#include "tss2-util.h"

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    TSS2_SYS_CONTEXT *sys_ctx;
    TSS2_RC rc;

#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif

    rc = sys_uefi_init (&sys_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        return EFI_ABORTED;
    }

    const TPMI_RH_PROVISION create_handle = TPM2_RH_OWNER;

    const TSS2L_SYS_AUTH_COMMAND create_auths = {
        .count = 1,
        .auths = { {
            .sessionHandle = TPM2_RH_PW,
            .hmac = {
                .size = 0,
                .buffer = { 0x12, 0x34, }
            }
        } }
    };

    const TPMI_RH_NV_INDEX nvIndex = 0x01000000;

    const TPM2B_NV_PUBLIC index_public = {
        .nvPublic = {
            .nvIndex = 0x01000000,
            .nameAlg = TPM2_ALG_SHA256,
            .attributes = (
                TPMA_NV_OWNERWRITE |
                TPMA_NV_AUTHWRITE |
                TPMA_NV_AUTHREAD |
                TPMA_NV_OWNERREAD
                //| TPMA_NV_PLATFORMCREATE
                ),
            .authPolicy = {
                 .size = 0,
                 .buffer = {},
             },
            .dataSize = 32,
        }
    };

    TPM2B_AUTH index_auth = { .size = 16, .buffer = { 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, } };

    const TSS2L_SYS_AUTH_COMMAND index_auths = {
        .count = 1,
        .auths = { {
            .sessionHandle = TPM2_RH_PW,
            .hmac = {
                .size = 16,
                .buffer = { 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, 0xAF, 0xFE, }
            }
        } }
    };

    TPM2B_DIGEST random_data = { .size = 32 };
    TPM2B_MAX_NV_BUFFER data;

    memset(&data.buffer[0], 0, 5);
    memcpy(&data.buffer[5], &data.buffer[0], 5);
    memmove(&data.buffer[5], &data.buffer[0], 5);

    rc = Tss2_Sys_GetRandom(sys_ctx, NULL, 32, &random_data, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        Print(L"Failed to Tss2_Sys_NV_DefineSpace: 0x%x\n", rc);
        goto exit_cleanup;
    }
    Print(L"Tss2_Sys_GetRandom returned TSS2_RC_SUCCESS\n");

    data.size = random_data.size;
    for (size_t i = 0; i < data.size; i++) {
        data.buffer[i] = random_data.buffer[i];
    }

    rc = Tss2_Sys_NV_DefineSpace (sys_ctx, create_handle, &create_auths,
                                  &index_auth, &index_public, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        Print(L"Failed to Tss2_Sys_NV_DefineSpace: 0x%x\n", rc);
        goto exit_cleanup;
    }
    Print(L"Tss2_Sys_NV_DefineSpace returned TSS2_RC_SUCCESS\n");

    rc = Tss2_Sys_NV_Write (sys_ctx, nvIndex, nvIndex, &index_auths,
                            &data, 0, NULL);
    if (rc == TPM2_RC_RETRY) {
        Print(L"Tss2_Sys_NV_Write TPM asked for TPM2_RC_RETRY\n");
        rc = Tss2_Sys_NV_Write (sys_ctx, nvIndex, nvIndex, &index_auths,
                                &data, 0, NULL);
    }
    if (rc != TSS2_RC_SUCCESS) {
        Print(L"Failed to Tss2_Sys_NV_Write: 0x%x\n", rc);
        goto exit_cleanup;
    }
    Print(L"Tss2_Sys_NV_Write returned TSS2_RC_SUCCESS\n");

    rc = Tss2_Sys_NV_UndefineSpace(sys_ctx, create_handle, nvIndex, &create_auths, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        Print(L"Failed to Tss2_Sys_NV_UndefineSpace: 0x%x\n", rc);
        goto exit_cleanup;
    }
    Print(L"Tss2_Sys_NV_UndefineSpace returned TSS2_RC_SUCCESS\n");

    rc = Tss2_Sys_GetRandom(sys_ctx, NULL, 32, &random_data, NULL);
    if (rc != TSS2_RC_SUCCESS) {
        Print(L"Failed to Tss2_Sys_GetRandom: 0x%x\n", rc);
        goto exit_cleanup;
    }
    Print(L"Tss2_Sys_GetRandom returned TSS2_RC_SUCCESS\n");

exit_cleanup:
    sys_uefi_teardown_full (sys_ctx);
    return (rc == TSS2_RC_SUCCESS)? EFI_SUCCESS : EFI_ABORTED;
}
