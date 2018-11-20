/* SPDX-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <efi/efierr.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tcti.h>
#include <tss2-tcti-uefi.h>

#include "tcg2-protocol.h"
#include "tcti-uefi.h"
#include "tcti-uefi-wraps.h"

#define GETCAP_PTR 0x1234abcd
#define PROTOCOL_PTR 0xabcd1234
#define MAXBUF_SIZE 0x3f0

/*
 * when given an NULL context and a pointer to a size_t, set the size_t
 * parameter to the size of the TSS2_TCTI_SKELETON_CONTEXT structure.
 */
static void
tcti_uefi_init_size_test (void **state)
{
    size_t tcti_size;

    will_return (__wrap_tcg2_get_protocol, PROTOCOL_PTR);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_get_max_buf, MAXBUF_SIZE);
    Tss2_Tcti_Uefi_Init (NULL, &tcti_size, NULL);
    assert_int_equal (tcti_size, sizeof (TSS2_TCTI_UEFI_CONTEXT) + MAXBUF_SIZE);
}
/*
 * when given a NULL context and a pointer to size_t, the init function
 * returns TSS2_RC_SUCCESS
 */
static void
tcti_uefi_init_success_return_value_test (void **state)
{
    size_t tcti_size;
    TSS2_RC ret;

    will_return (__wrap_tcg2_get_protocol, PROTOCOL_PTR);
    will_return (__wrap_tcg2_get_protocol, EFI_SUCCESS);
    will_return (__wrap_tcg2_get_max_buf, MAXBUF_SIZE);
    ret = Tss2_Tcti_Uefi_Init (NULL, &tcti_size, NULL);
    assert_int_equal (ret, TSS2_RC_SUCCESS);
}
/*
 * when given NULL pointers for both parameters the init function returns
 * an error indicating that the values are bad (TSS2_TCTI_RC_BAD_VALUE)
 */
static void
tcti_uefi_init_allnull_is_bad_value (void **state)
{
    TSS2_RC ret;

    ret = Tss2_Tcti_Uefi_Init (NULL, NULL, NULL);
    assert_int_equal (ret, TSS2_TCTI_RC_BAD_VALUE);
}

int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (tcti_uefi_init_size_test),
        cmocka_unit_test (tcti_uefi_init_success_return_value_test),
        cmocka_unit_test (tcti_uefi_init_allnull_is_bad_value),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
