/* SPDX-License-Identifier: BSD-2 */
#ifndef TSS2_UTIL_H
#define TSS2_UTIL_H

#include <tss2/tss2_tcti.h>
#include <tss2/tss2_sys.h>

TSS2_RC
sys_uefi_init_from_tcti_ctx (TSS2_TCTI_CONTEXT *tcti_ctx,
                             TSS2_SYS_CONTEXT **sys_ctx);
TSS2_RC
sys_uefi_init (TSS2_SYS_CONTEXT **sys_ctx);
void
sys_uefi_teardown (TSS2_SYS_CONTEXT *sys_ctx);
void
sys_uefi_teardown_full (TSS2_SYS_CONTEXT *sys_ctx);
TSS2_RC
tcti_uefi_init (TSS2_TCTI_CONTEXT **ctx);
void
tcti_uefi_teardown (TSS2_TCTI_CONTEXT *ctx);

#endif /* TSS2_UTIL_H */
