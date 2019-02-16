/* SPDX-License-Identifier: BSD-2 */
#ifndef TCTI_UEFI_WRAPS_H
#define TCTI_UEFI_WRAPS_H

#include "uefi-types.h"
#include "tcg2-protocol.h"

/* wrap functions for gnu-efi functions */
VOID*
__wrap_AllocatePool (IN UINTN Size);
VOID
__wrap_CopyMem (IN VOID *Dest,
                IN CONST VOID *Src,
                IN UINTN len);
VOID
__wrap_FreePool (IN VOID *p);
UINTN
__wrap_Print (IN CHAR16 *fmt,
              ...);
UINT64
__wrap_efi_call2 (void *func,
                  UINT64 arg1,
                  UINT64 arg2);
UINT64
__wrap_efi_call5 (void *func,
                  UINT64 arg1,
                  UINT64 arg2,
                  UINT64 arg3,
                  UINT64 arg4,
                  UINT64 arg5);
EFI_STATUS
__wrap_LibLocateProtocol (IN  EFI_GUID *ProtocolGuid,
                          OUT VOID **Interface);

/* wrap functions for tcg2-util module */
EFI_STATUS
__wrap_tcg2_get_capability (EFI_TCG2_PROTOCOL *tcg2_protocol,
                            EFI_TCG2_BOOT_SERVICE_CAPABILITY *tcg2_bs_caps);
EFI_STATUS
__wrap_tcg2_get_protocol (EFI_TCG2_PROTOCOL **tcg2_protocol);
UINT16
__wrap_tcg2_get_max_buf (EFI_TCG2_PROTOCOL *tcg2_protocol);
EFI_STATUS
__wrap_tcg2_submit_command (EFI_TCG2_PROTOCOL *tcg2_protocol,
                            UINT32 input_size,
                            UINT8 *input_buf,
                            UINT32 output_size,
                            UINT8 *output_buf);

#endif /* TCTI_UEFI_WRAPS_H */
