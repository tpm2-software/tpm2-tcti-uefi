/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include <inttypes.h>
#include <stdlib.h>

#include <tss2/tss2_tpm2_types.h>

#include "tcg2-protocol.h"
#include "tcg2-util.h"
#include "util.h"

static void
prettyprint_tpm12_event (TCG_PCR_EVENT *event)
{
    Print (L"TCG_PCR_EVENT: 0x%" PRIxPTR "\n", (uintptr_t)event);
    Print (L"  PCRIndex: %d\n", event->PCRIndex);
    Print (L"  EventType: %s (0x%08" PRIx32 ")\n",
           eventtype_to_string (event->EventType),
           event->EventType);
    Print (L"  Digest: \n");
    DumpHex (4, 0, sizeof (event->Digest), &event->Digest);
    Print (L"  EventSize: %d\n", event->EventSize);
    Print (L"  Event: \n");
    DumpHex (4, 0, event->EventSize, event->Event);
}
static TCG_PCR_EVENT*
tpm12_event_next (TCG_PCR_EVENT *current,
                  TCG_PCR_EVENT *last)
{
    TCG_PCR_EVENT *next;

    if (current == last)
        return NULL;

    next = (TCG_PCR_EVENT*)((char*)(current + 1) + current->EventSize - 1);
    if (next > last) {
        Print (L"Error: current element is beyond the end of the log\n");
        return NULL;
    }

    return next;
}
static void
prettyprint_tpm12_eventlog (EFI_PHYSICAL_ADDRESS first,
                            EFI_PHYSICAL_ADDRESS last)
{
    TCG_PCR_EVENT *event_last, *event;

    event = (TCG_PCR_EVENT*) first;
    event_last  = (TCG_PCR_EVENT*) last;

    if (event == NULL) {
        Print (L"TPM2 EventLog is empty.");
        return;
    }

    Print (L"TPM2 EventLog, EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 format\n");
    do {
        prettyprint_tpm12_event (event);
    } while ((event = tpm12_event_next (event, event_last)) != NULL);
}
static void
prettyprint_tpm2_eventbuf (TCG_EVENT2 *event)
{
    Print (L"  Event: %" PRIu32 " bytes\n", event->EventSize);
    DumpHex (2, 0, event->EventSize, event->Event);
}
static void
prettyprint_tpm2_digest (TCG_DIGEST2 *digest)
{
    size_t digest_size;

    Print (L"    AlgorithmId: %s (0x%" PRIx16 ")\n",
           get_alg_name (digest->AlgorithmId), digest->AlgorithmId);
    /* map alg id to alg size to get buffer size */
    digest_size = get_alg_size (digest->AlgorithmId);
    Print (L"    Digest: %d bytes\n", digest_size);
    DumpHex (4, 0, digest_size, digest->Digest);
}
static void
prettyprint_tpm2_event_header (TCG_EVENT_HEADER2 *event_hdr)
{
    Print (L"  PCRIndex: %d\n", event_hdr->PCRIndex);
    Print (L"  EventType: %s (0x%" PRIx32 ")\n",
           eventtype_to_string (event_hdr->EventType),
           event_hdr->EventType);
    Print (L"  DigestCount: %d\n", event_hdr->DigestCount);
}
static TCG_EVENT_HEADER2*
prettyprint_tpm2_event (TCG_EVENT_HEADER2 *event_hdr)
{
    TCG_DIGEST2 *digest;
    TCG_EVENT2 *event;
    size_t i;

    prettyprint_tpm2_event_header (event_hdr);
    digest = (TCG_DIGEST2*)event_hdr->Digests;
    for (i = 0; i < event_hdr->DigestCount; ++i) {
        Print (L"  TCG_DIGEST2: %u\n", i);
        prettyprint_tpm2_digest (digest);
        digest = get_next_digest (digest);
    }
    event = (TCG_EVENT2*)digest;
    prettyprint_tpm2_eventbuf (event);

    return (TCG_EVENT_HEADER2*)(event->Event + event->EventSize);
}
static void
prettyprint_tpm2_eventlog (EFI_PHYSICAL_ADDRESS first,
                           EFI_PHYSICAL_ADDRESS last)
{
    TCG_EVENT_HEADER2 *event = (TCG_EVENT_HEADER2*)first;
    TCG_EVENT_HEADER2 *event_last = (TCG_EVENT_HEADER2*)last;
    size_t i;

    if (event == NULL) {
        Print (L"TPM2 EventLog is empty.\n");
        return;
    }

    Print (L"TPM2 EventLog, EFI_TCG2_EVENT_LOG_FORMAT_TCG_2 format\n");
    for (i = 0; event <= event_last; ++i) {
        Print (L"EVENT: %" PRIu32 "\n", i);
        event = prettyprint_tpm2_event (event);
    }
    Print (L"TPM2 EventLog end\n");
}
static void
prettyprint_eventlog (EFI_PHYSICAL_ADDRESS first,
                      EFI_PHYSICAL_ADDRESS last,
                      EFI_TCG2_EVENT_LOG_FORMAT format)
{
    switch (format) {
    case EFI_TCG2_EVENT_LOG_FORMAT_TCG_2:
        prettyprint_tpm2_eventlog (first, last);
        break;
    case EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2:
        prettyprint_tpm12_eventlog (first, last);
        break;
    default:
        Print (L"ERROR: Unknown format: 0x%" PRIx32 "\n", format);
        break;
    }
}

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS first, last;
    EFI_TCG2_EVENT_LOG_FORMAT format = 0;
    BOOLEAN trunc;

    InitializeLib (ImageHandle, SystemTable);
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status))
        return status;

    status = get_eventlog_format_high (tcg2_protocol, &format);
    if (EFI_ERROR (status))
        return status;
    else if (format == 0) {
        Print (L"ERROR: TCG2 UEFI protocol doesn't support a log format "
               "known to this tool.\n");
        return 1;
    }

    status = tcg2_get_eventlog (tcg2_protocol, format, &first, &last, &trunc);
    if (EFI_ERROR (status))
        return status;

    if (trunc)
        Print (L"WARNING: Eventlog has been truncated\n");

    prettyprint_eventlog (first, last, format);

    return status;
}
