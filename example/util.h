/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>
#include <stdlib.h>

#include "tcg2-protocol.h"

WCHAR*
bitmap_val_str (UINT32 member, UINT32 selector);
void EFIAPI
tcg2_algorithm_bitmap_prettyprint (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap);
CHAR16* EFIAPI
eventtype_to_string (TCG_EVENTTYPE event_type);
size_t EFIAPI
get_alg_size (UINT16 alg_id);
wchar_t* EFIAPI
get_alg_name (UINT16 alg_id);
EFI_STATUS EFIAPI
get_eventlog_format_high (EFI_TCG2_PROTOCOL *tcg2_protocol,
                          EFI_TCG2_EVENT_LOG_FORMAT *format);
TCG_DIGEST2* EFIAPI
get_next_digest (TCG_DIGEST2* digest);
