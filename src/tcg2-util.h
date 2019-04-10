/* SPDX-License-Identifier: BSD-2 */
#ifndef TCG2_UTIL_H
#define TCG2_UTIL_H

#include <efi/efi.h>

#include "tcg2-protocol.h"

EFI_STATUS EFIAPI
tcg2_get_eventlog (EFI_TCG2_PROTOCOL *tpm2_prot,
                   EFI_TCG2_EVENT_LOG_FORMAT format,
                   EFI_PHYSICAL_ADDRESS *first,
                   EFI_PHYSICAL_ADDRESS *last,
                   BOOLEAN *truncated);
EFI_STATUS EFIAPI
tcg2_get_active_pcr_banks (EFI_TCG2_PROTOCOL *tcg2_protocol,
                           UINT32 *pcr_banks);
EFI_STATUS EFIAPI
tcg2_get_capability (EFI_TCG2_PROTOCOL *tcg2_protocol,
                     EFI_TCG2_BOOT_SERVICE_CAPABILITY *tcg2_bs_caps);
void EFIAPI
tcg2_caps_prettyprint (EFI_TCG2_BOOT_SERVICE_CAPABILITY *caps);
UINT16 EFIAPI
tcg2_get_max_buf (EFI_TCG2_PROTOCOL *tcg2_protocol);
EFI_STATUS EFIAPI
tcg2_submit_command (EFI_TCG2_PROTOCOL *tcg2_protocol,
                     UINT32 input_size,
                     UINT8 *input_buf,
                     UINT32 output_size,
                     UINT8 *output_buf);
EFI_STATUS EFIAPI
tcg2_get_protocol (EFI_TCG2_PROTOCOL **tcg2_protocol);

#endif /* TCG2_UTIL_H */
