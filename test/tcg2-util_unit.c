/* SPDX-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <efi/efierr.h>

#include <setjmp.h>
#include <cmocka.h>

#include "tcg2-util.h"

static void
get_capability_fail (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    EFI_TCG2_BOOT_SERVICE_CAPABILITY tcg2_bs_caps = { 0 };
    EFI_STATUS status;

    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, EFI_LOAD_ERROR);
    status = tcg2_get_capability (&protocol, &tcg2_bs_caps);
    assert_int_equal (status, EFI_LOAD_ERROR);
}

static void
get_capability_success (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    EFI_TCG2_BOOT_SERVICE_CAPABILITY tcg2_bs_caps = { 0 };
    EFI_STATUS status;

    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, EFI_SUCCESS);
    status = tcg2_get_capability (&protocol, &tcg2_bs_caps);
    assert_int_equal (status, EFI_SUCCESS);
}

static void
get_max_buf_fail (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    UINT16 max_buf;

    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, EFI_LOAD_ERROR);
    max_buf = tcg2_get_max_buf (&protocol);
    assert_int_equal (max_buf, 0);
}

static void
get_max_buf_cmd_gt_success (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    UINT16 max_buf;

    will_return (__wrap_efi_call2, 1);
    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, EFI_SUCCESS);
    max_buf = tcg2_get_max_buf (&protocol);
    assert_int_equal (max_buf, 1);
}

static void
get_max_buf_resp_gt_success (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    UINT16 max_buf;

    will_return (__wrap_efi_call2, 0);
    will_return (__wrap_efi_call2, 1);
    will_return (__wrap_efi_call2, EFI_SUCCESS);
    max_buf = tcg2_get_max_buf (&protocol);
    assert_int_equal (max_buf, 1);
}

static void
submit_command_fail (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    EFI_STATUS status;

    will_return (__wrap_efi_call5, EFI_LOAD_ERROR);
    status = tcg2_submit_command (&protocol, 0, NULL, 0, NULL);
    assert_int_equal (status, EFI_LOAD_ERROR);
}

static void
submit_command_success (void **state)
{
    EFI_TCG2_PROTOCOL protocol = { 0 };
    EFI_STATUS status;

    will_return (__wrap_efi_call5, EFI_SUCCESS);
    status = tcg2_submit_command (&protocol, 0, NULL, 0, NULL);
    assert_int_equal (status, EFI_SUCCESS);
}

static void
get_protocol_fail (void **state)
{
    EFI_TCG2_PROTOCOL *protocol = NULL;
    EFI_STATUS status;

    will_return (__wrap_LibLocateProtocol, NULL);
    will_return (__wrap_LibLocateProtocol, EFI_LOAD_ERROR);
    status = tcg2_get_protocol (&protocol);
    assert_int_equal (status, EFI_LOAD_ERROR);
}

static void
get_protocol_success (void **state)
{
    EFI_TCG2_PROTOCOL *protocol = NULL;
    EFI_STATUS status;

    will_return (__wrap_LibLocateProtocol, 1);
    will_return (__wrap_LibLocateProtocol, EFI_SUCCESS);
    status = tcg2_get_protocol (&protocol);
    assert_int_equal (status, EFI_SUCCESS);
    assert_int_equal (protocol, 1);
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (get_capability_fail),
        cmocka_unit_test (get_capability_success),
        cmocka_unit_test (get_max_buf_fail),
        cmocka_unit_test (get_max_buf_cmd_gt_success),
        cmocka_unit_test (get_max_buf_resp_gt_success),
        cmocka_unit_test (submit_command_fail),
        cmocka_unit_test (submit_command_success),
        cmocka_unit_test (get_protocol_fail),
        cmocka_unit_test (get_protocol_success),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
