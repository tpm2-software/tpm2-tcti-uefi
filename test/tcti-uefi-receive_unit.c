#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include <efi/efierr.h>

#include <tss2/tss2_tcti.h>
#include <tss2-tcti-uefi.h>

#include "tcg2-protocol.h"
#include "tcti-uefi.h"

/*
 * An instance of this structure is passed to each test through the 'state'
 * parameter.
 */
typedef struct {
    TSS2_TCTI_UEFI_CONTEXT uefi_ctx;
    TSS2_TCTI_CONTEXT *tcti_ctx;
    TSS2_RC rc;
    uint8_t buf[1];
    size_t size;
} data_t;

/* We maintain a single global instance of the test data structure. */
static data_t data_global;

/* Setup function to initialize global data_t instance. */
static int
tcti_uefi_receive_setup (void **state)
{
    /* set global data to 0 */
    memset (&data_global, 0, sizeof (data_t));
    /* initialize "size defaults" */
    data_global.tcti_ctx = (TSS2_TCTI_CONTEXT*)&data_global.uefi_ctx;
    data_global.uefi_ctx.common.v1.magic = TCTI_UEFI_MAGIC;
    data_global.uefi_ctx.common.v1.version = TCTI_UEFI_VERSION;
    data_global.uefi_ctx.size = 10;
    data_global.uefi_ctx.state = UEFI_STATE_RECEIVE;

    *state = &data_global;
    return 0;
}

static void
tcti_uefi_receive_null_context_fail (void **state)
{
    data_t *data = *state;

    data->rc = tcti_uefi_receive (NULL,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_BAD_REFERENCE);
}

static void
tcti_uefi_receive_null_size_fail (void **state)
{
    data_t *data = *state;

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  NULL,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_BAD_REFERENCE);
}

static void
tcti_uefi_receive_timeout_fail (void **state)
{
    data_t *data = *state;

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  0);
    assert_int_equal (data->rc, TSS2_TCTI_RC_NOT_IMPLEMENTED);
}

static void
tcti_uefi_receive_context_fail (void **state)
{
    data_t *data = *state;

    data->uefi_ctx.common.v1.magic = 0x0,
    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_BAD_CONTEXT);
}

static void
tcti_uefi_receive_state_fail (void **state)
{
    data_t *data = *state;

    data->uefi_ctx.state = UEFI_STATE_TRANSMIT,
    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_BAD_SEQUENCE);
}

static void
tcti_uefi_receive_size_query_success (void **state)
{
    data_t *data = *state;

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  NULL,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_RC_SUCCESS);
    assert_int_equal (data->size, data->uefi_ctx.size);
}

#define TEST_TCG2_PROTOCOL_REF (EFI_TCG2_PROTOCOL*)0xdeadbeef
static void
tcti_uefi_receive_protocol_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_INVALID_PARAMETER);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}

static void
tcti_uefi_receive_efi_invalid_parameter_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_INVALID_PARAMETER);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_BAD_VALUE);
}

static void
tcti_uefi_receive_efi_buffer_too_small_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_BUFFER_TOO_SMALL);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_INSUFFICIENT_BUFFER);
}

static void
tcti_uefi_receive_efi_device_error_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_DEVICE_ERROR);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_IO_ERROR);
}

static void
tcti_uefi_receive_efi_access_denied_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_ACCESS_DENIED);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}

static void
tcti_uefi_receive_efi_unknown_fail (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_MEDIA_CHANGED);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_TCTI_RC_GENERAL_FAILURE);
}

static void
tcti_uefi_receive_success (void **state)
{
    data_t *data = *state;

    will_return (__wrap_tcg2_get_protocol, TEST_TCG2_PROTOCOL_REF);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_submit_command, EFI_SUCCESS);

    data->rc = tcti_uefi_receive (data->tcti_ctx,
                                  &data->size,
                                  data->buf,
                                  TSS2_TCTI_TIMEOUT_BLOCK);
    assert_int_equal (data->rc, TSS2_RC_SUCCESS);
}

int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_setup (tcti_uefi_receive_null_context_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_null_size_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_timeout_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_context_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_state_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_size_query_success,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_protocol_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_efi_invalid_parameter_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_efi_buffer_too_small_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_efi_device_error_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_efi_access_denied_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_efi_unknown_fail,
                                tcti_uefi_receive_setup),
        cmocka_unit_test_setup (tcti_uefi_receive_success,
                                tcti_uefi_receive_setup),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
