#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if ARCH == x86_64
#include <efi/x86_64/efibind.h>
#elif ARCH == ia32
#include <efi/ia32/efibind.h>
#else
#error "Unsupported ARCH."
#endif

#include <efi/efidef.h>
#include <efi/efierr.h>

#include <setjmp.h>
#include <cmocka.h>

#include <tss2/tss2_tcti.h>
#include <tss2-tcti-uefi.h>

#include "tcg2-protocol.h"
#include "tcti-uefi.h"

#define GETCAP_PTR 0x1234abcd
#define PROTOCOL_PTR 0xabcd1234
#define MAXBUF_SIZE 0x3f0

/* first mock value is the return value */
VOID*
__wrap_AllocatePool (IN UINTN Size)
{
    printf ("%s\n", __func__);
    return mock_ptr_type (VOID*);
}
/* CopyMem returns void so we can treat it like it always succeeds. */
VOID
__wrap_CopyMem (IN VOID *Dest,
                IN CONST VOID *Src,
                IN UINTN len)
{
    printf ("%s\n", __func__);
}
/*
 * Populating the capability structure may be necessary for more complex tests.
 * For now just one mock that is the return value.
 */
EFI_STATUS
__wrap_tcg2_get_capability (
    EFI_TCG2_PROTOCOL *tcg2_protocol,
    EFI_TCG2_BOOT_SERVICE_CAPABILITY *tcg2_bs_caps
    )
{
    printf ("%s\n", __func__);
    return mock_type (UINT64);
}
/* First mock is the size as a UINT16 */
UINT16
__wrap_tcg2_get_max_buf (EFI_TCG2_PROTOCOL *tcg2_protocol)
{
    printf ("%s\n", __func__);
    return mock_type (UINT16);
}
/* first mock is return value */
EFI_STATUS
__wrap_tcg2_submit_command (EFI_TCG2_PROTOCOL *tcg2_protocol,
                            UINT32 input_size,
                            UINT8 *input_buf,
                            UINT32 output_size,
                            UINT8 *output_buf)
{
    printf ("%s\n", __func__);
    return mock_type (EFI_STATUS);
}
/* Returns void so we treat it as a noop */
VOID
__wrap_FreePool (IN VOID *p)
{
    printf ("%s\n", __func__);
}
/*
 * first mock value goes into 'Interface' OUT param
 * second mock value is return status from function
 */
EFI_STATUS
__wrap_LibLocateProtocol (IN  EFI_GUID *ProtocolGuid,
                          OUT VOID **Interface)
{
    printf ("%s\n", __func__);
    *Interface = mock_ptr_type (VOID*);
    return mock_type (EFI_STATUS);
}
/* map UEFI 'Print' function to printf. No mocking required. */
UINTN
__wrap_Print (IN CHAR16   *fmt,
              ...)
{
    va_list args;

    printf ("%s\n", __func__);
    va_start (args, fmt);
    vprintf ((char*)fmt, args);
    va_end (args);
    return 0;
}
/*
 * when given an NULL context and a pointer to a size_t, set the size_t
 * parameter to the size of the TSS2_TCTI_SKELETON_CONTEXT structure.
 */
static void
tcti_uefi_init_size_test (void **state)
{
    size_t tcti_size;

    will_return (__wrap_LibLocateProtocol, PROTOCOL_PTR);
    will_return (__wrap_LibLocateProtocol, EFI_SUCCESS);
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

    will_return (__wrap_LibLocateProtocol, PROTOCOL_PTR);
    will_return (__wrap_LibLocateProtocol, EFI_SUCCESS);
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
