/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <tss2/tss2_sys.h>

#include "tcg2-util.h"
#include "tss2-util.h"
#include "util.h"

typedef struct callback_data {
    size_t digest_i;
    TPMI_ALG_HASH last_alg;
    TPML_DIGEST tpml_digests;
    TPML_PCR_SELECTION pcr_select;
} callback_data_t;

static bool
selected_digest_callback (TPMI_ALG_HASH alg,
                          uint8_t pcr,
                          void *data)
{
    callback_data_t *cbdata = (callback_data_t*)data;
    TPMS_PCR_SELECTION *selection;
    TPM2B_DIGEST *digest;
    size_t i;

    if (cbdata == NULL) {
        Print (L"ERROR: cbdata cannot be NULL.\n");
        return false;
    }
    digest = &cbdata->tpml_digests.digests [cbdata->digest_i++];
    if (get_alg_size (alg) != digest->size) {
        Print (L"ERROR: algorithm and digest size disagre?\n");
    }
    if (cbdata->last_alg != alg) {
        cbdata->last_alg = alg;
        Print (L"PCR bank: %s\n", get_alg_name (alg));
    }
    Print (L"  PCR: %u\n", pcr);
    DumpHex (4, 0, digest->size, digest->buffer);

    for (i = 0; i < cbdata->pcr_select.count; ++i) {
        selection = &cbdata->pcr_select.pcrSelections [i];
        if (selection->hash == alg) {
            deselect_pcr (selection->pcrSelect, pcr);
        }
    }

    if (!(cbdata->digest_i < cbdata->tpml_digests.count))
        return false;
    else
        return true;
}

int
prettyprint_pcrs_all (TSS2_SYS_CONTEXT *sys_ctx,
                      EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks)
{
    TSS2_RC rc;
    UINT32 update_count = 0;
    TPML_PCR_SELECTION pcr_select_out = { 0, };
    callback_data_t cbdata = { 0, };

    select_all_active_pcrs (active_banks, &cbdata.pcr_select);
    do {
        cbdata.digest_i = 0;
        memset (&cbdata.tpml_digests, 0, sizeof (cbdata.tpml_digests));
        rc = Tss2_Sys_PCR_Read (sys_ctx,
                                NULL,
                                &cbdata.pcr_select,
                                &update_count,
                                &pcr_select_out,
                                &cbdata.tpml_digests,
                                NULL);
        if (rc != TSS2_RC_SUCCESS) {
            Print (L"ERROR: Tss2_Sys_PCR_Read failed with RC: 0x%" PRIx32 "\n",
                   rc);
            return 1;
        }
        foreach_selection (&pcr_select_out,
                           selected_digest_callback,
                           &cbdata);
    } while (cbdata.tpml_digests.count > 0);

    return 0;
}

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_STATUS status;
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = 0;
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    TSS2_RC rc;
    TSS2_SYS_CONTEXT *sys_ctx;

#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status)) {
        return status;
    }

    status = tcg2_get_active_pcr_banks (tcg2_protocol, &active_banks);
    if (EFI_ERROR (status)) {
        return status;
    }

    rc = sys_uefi_init (&sys_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        return EFI_ABORTED;
    }

    return prettyprint_pcrs_all (sys_ctx, active_banks);
}
