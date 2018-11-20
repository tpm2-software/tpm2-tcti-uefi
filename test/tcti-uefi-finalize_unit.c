/* SPDX-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tcti.h>
#include <tss2-tcti-uefi.h>

#include "tcti-uefi.h"

static void
tcti_uefi_finalize_context_null (void **state)
{
    tcti_uefi_finalize (NULL);
    assert_true (1 == 1);
}

static void
tcti_uefi_finalize_context_cast_fail (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = 0x0,
        .state = UEFI_STATE_TRANSMIT,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;

    tcti_uefi_finalize (tcti_ctx);
    assert_int_equal (uefi_ctx.state, UEFI_STATE_TRANSMIT);
}

static void
tcti_uefi_finalize_success (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = TCTI_UEFI_VERSION,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;

    tcti_uefi_finalize (tcti_ctx);
    assert_int_equal (uefi_ctx.state, UEFI_STATE_FINAL);
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (tcti_uefi_finalize_context_null),
        cmocka_unit_test (tcti_uefi_finalize_context_cast_fail),
        cmocka_unit_test (tcti_uefi_finalize_success),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
