/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>

#include <tss2/tss2_tpm2_types.h>

#include "tcg2-protocol.h"

typedef bool (*PCR_SELECTION_CALLBACK) (TPMI_ALG_HASH alg,
                                        uint8_t pcr,
                                        void *data);

wchar_t*
bitmap_val_str (UINT32 member, UINT32 selector);
void EFIAPI
tcg2_algorithm_bitmap_prettyprint (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap);
wchar_t* EFIAPI
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
bool EFIAPI
is_pcr_selected (BYTE pcr_selection[],
                 uint8_t pcr);
void EFIAPI
select_pcr (BYTE pcr_selection [],
            uint8_t pcr);
void EFIAPI
deselect_pcr (BYTE pcr_selection [],
              uint8_t pcr);
void
foreach_selection (TPML_PCR_SELECTION *pcr_selection,
                   PCR_SELECTION_CALLBACK callback,
                   void *data);
int EFIAPI
count_algs_in_bitmap (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap);
void EFIAPI
select_all_active_pcrs (EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks,
                        TPML_PCR_SELECTION *selections);
void
prettyprint_tpm2_digest (TCG_DIGEST2 *digest);
bool
prettyprint_tpm2_digest_callback (TCG_DIGEST2 *digest,
                                  void *data);
void
prettyprint_tpm2_event_header (TCG_EVENT_HEADER2 *event_hdr);
void
prettyprint_tpm2_eventbuf (TCG_EVENT2 *event);
void
prettyprint_tpm2_event (TCG_EVENT_HEADER2 *event_hdr);
bool
prettyprint_tpm2_event_callback (TCG_EVENT_HEADER2 *event_hdr,
                                 void *data);

typedef bool (*DIGEST2_CALLBACK) (TCG_DIGEST2 *digest,
                                  void *data);
bool
foreach_digest2 (TCG_EVENT_HEADER2 *event_hdr,
                 DIGEST2_CALLBACK callback,
                 void *data);

typedef bool (*EVENT2_CALLBACK) (TCG_EVENT_HEADER2 *event_hdr,
                                 void *data);
bool
foreach_event2 (TCG_EVENT_HEADER2 *event_hdr,
                TCG_EVENT_HEADER2 *last_hdr,
                EVENT2_CALLBACK callback,
                void *data);
