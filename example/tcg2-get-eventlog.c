/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include <inttypes.h>
#include <stdlib.h>

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
    Print (L"  digest: \n");
    DumpHex (4, 0, sizeof (event->digest), &event->digest);
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
EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    EFI_TCG2_PROTOCOL *tcg2_protocol;
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS first, last;
    EFI_TCG2_EVENT_LOG_FORMAT format = EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2;
    BOOLEAN trunc;

    InitializeLib (ImageHandle, SystemTable);
    status = tcg2_get_protocol (&tcg2_protocol);
    if (EFI_ERROR (status))
        return status;

    status = tcg2_get_eventlog (tcg2_protocol, format, &first, &last, &trunc);
    if (EFI_ERROR (status))
        return status;

    if (trunc)
        Print (L"WARNING: Eventlog has been truncated\n");

    prettyprint_tpm12_eventlog (first, last);

    return status;
}
