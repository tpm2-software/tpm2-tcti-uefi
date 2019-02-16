/* SPDX-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <setjmp.h>
#include <cmocka.h>

#include "uefi-types.h"
#include "tcg2-protocol.h"
#include "tcti-uefi-wraps.h"

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
/* Returns void so we treat it as a noop */
VOID
__wrap_FreePool (IN VOID *p)
{
    printf ("%s\n", __func__);
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
/*
 * first mock value goes into 'Interface' OUT param
 * second mock value is return status from function
 */
EFI_STATUS
__wrap_tcg2_get_protocol (EFI_TCG2_PROTOCOL **tcg2_protocol)
{
    *tcg2_protocol = mock_ptr_type (VOID*);
    return mock_type (EFI_STATUS);
}

UINT64
__wrap_efi_call2 (void *func,
                  UINT64 arg1,
                  UINT64 arg2)
{
    EFI_TCG2_BOOT_SERVICE_CAPABILITY *tcg2_bs_caps = (EFI_TCG2_BOOT_SERVICE_CAPABILITY*)arg2;
    tcg2_bs_caps->MaxCommandSize = mock_type (UINT16);
    tcg2_bs_caps->MaxResponseSize = mock_type (UINT16);
    return mock_type (UINT64);
}

UINT64
__wrap_efi_call5 (void *func,
                  UINT64 arg1,
                  UINT64 arg2,
                  UINT64 arg3,
                  UINT64 arg4,
                  UINT64 arg5)
{
    return mock_type (UINT64);
}

EFI_STATUS
__wrap_LibLocateProtocol (IN  EFI_GUID *ProtocolGuid,
                          OUT VOID **Interface)
{
    *Interface = mock_type (void*);
    return mock_type (EFI_STATUS);
}
