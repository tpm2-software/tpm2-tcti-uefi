/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include "tcg2-protocol.h"

WCHAR*
bitmap_val_str (UINT32 member, UINT32 selector);
void EFIAPI
tcg2_algorithm_bitmap_prettyprint (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap);
CHAR16* EFIAPI
eventtype_to_string (TCG_EVENTTYPE event_type);
