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
tcti_uefi_transmit_null_context_fail (void **state)
{
    TSS2_RC rc;
    uint8_t buf[1];

    rc = tcti_uefi_transmit (NULL, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_REFERENCE);
}

static void
tcti_uefi_transmit_null_command_fail (void **state)
{
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)0x1;
    TSS2_RC rc;

    rc = tcti_uefi_transmit (tcti_ctx, 0, NULL);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_REFERENCE);
}

static void
tcti_uefi_transmit_context_fail (void **state)
{
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = 0x0,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;
    TSS2_RC rc;
    uint8_t buf[1];

    rc = tcti_uefi_transmit (tcti_ctx, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_CONTEXT);
}

static void
tcti_uefi_transmit_state_fail (void **state)
{
     TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = TCTI_UEFI_VERSION,
        .state = UEFI_STATE_RECEIVE,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;
    TSS2_RC rc;
    uint8_t buf[1];

    rc = tcti_uefi_transmit (tcti_ctx, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_SEQUENCE);
}

static void
tcti_uefi_transmit_size_fail (void **state)
{
     TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = TCTI_UEFI_VERSION,
        .size = 1,
        .state = UEFI_STATE_TRANSMIT,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;
    TSS2_RC rc;
    uint8_t buf[1];

    rc = tcti_uefi_transmit (tcti_ctx, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_uefi_transmit_success (void **state)
{
     TSS2_TCTI_UEFI_CONTEXT uefi_ctx = {
        .common.v1.magic = TCTI_UEFI_MAGIC,
        .common.v1.version = TCTI_UEFI_VERSION,
        .size = 20,
        .state = UEFI_STATE_TRANSMIT,
    };
    TSS2_TCTI_CONTEXT *tcti_ctx = (TSS2_TCTI_CONTEXT*)&uefi_ctx;
    TSS2_RC rc;
    uint8_t buf[15];

    rc = tcti_uefi_transmit (tcti_ctx, sizeof (buf), buf);
    assert_int_equal (rc, TSS2_RC_SUCCESS);
}

int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (tcti_uefi_transmit_null_context_fail),
        cmocka_unit_test (tcti_uefi_transmit_null_command_fail),
        cmocka_unit_test (tcti_uefi_transmit_context_fail),
        cmocka_unit_test (tcti_uefi_transmit_state_fail),
        cmocka_unit_test (tcti_uefi_transmit_size_fail),
        cmocka_unit_test (tcti_uefi_transmit_success),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
