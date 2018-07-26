/* SPDX-License-Identifier: BSD-2 */
#ifndef TSS2_TCTI_UEFI_H
#define TSS2_TCTI_UEFI_H

#include <tss2/tss2_tcti.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TCTI specific / unique Init function. */
TSS2_RC
Tss2_Tcti_Uefi_Init (TSS2_TCTI_CONTEXT *context,
                     size_t *size,
                     const char *conf);

#ifdef __cplusplus
}
#endif

#endif /* TSS2_TCTI_UEFI_H */
