/* SPDK-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tcti.h>
#include <tss2-tcti-uefi.h>

#include "tcti-uefi.h"

static void
tcti_uefi_cast_context_null_fail (void **state)
{
    assert_null(tcti_uefi_context_cast (NULL));
}

static void
tcti_uefi_cast_magic_fail (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = 0x0,
    };

    assert_null (tcti_uefi_context_cast ((TSS2_TCTI_CONTEXT*)&uefi_ctx));
}

static void
tcti_uefi_cast_version_fail (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = 0x1,
    };

    assert_null (tcti_uefi_context_cast ((TSS2_TCTI_CONTEXT*)&uefi_ctx));
}
static void
tcti_uefi_cast_success (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = TCTI_UEFI_VERSION,
    };

    assert_non_null (tcti_uefi_context_cast ((TSS2_TCTI_CONTEXT*)&uefi_ctx));
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (tcti_uefi_cast_context_null_fail),
        cmocka_unit_test (tcti_uefi_cast_magic_fail),
        cmocka_unit_test (tcti_uefi_cast_version_fail),
        cmocka_unit_test (tcti_uefi_cast_success),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
