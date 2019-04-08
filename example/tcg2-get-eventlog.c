/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include <inttypes.h>
#include <stdlib.h>

#include "tcg2-protocol.h"
#include "tcg2-util.h"

static CHAR16*
eventtype_to_string (TCG_EVENTTYPE event_type)
{
    switch (event_type) {
    case EV_PREBOOT_CERT:
        return L"EV_PREBOOT_CERT";
    case EV_POST_CODE:
        return L"EV_POST_CODE";
    case EV_UNUSED:
        return L"EV_UNUSED";
    case EV_NO_ACTION:
        return L"EV_NO_ACTION";
    case EV_SEPARATOR:
        return L"EV_SEPARATOR";
    case EV_ACTION:
        return L"EV_ACTION";
    case EV_EVENT_TAG:
        return L"EV_EVENT_TAG";
    case EV_S_CRTM_CONTENTS:
        return L"EV_S_CRTM_CONTENTS";
    case EV_S_CRTM_VERSION:
        return L"EV_S_CRTM_VERSION";
    case EV_CPU_MICROCODE:
        return L"EV_CPU_MICROCODE";
    case EV_PLATFORM_CONFIG_FLAGS:
        return L"EV_PLATFORM_CONFIG_FLAGS";
    case EV_TABLE_OF_DEVICES:
        return L"EV_TABLE_OF_DEVICES";
    case EV_COMPACT_HASH:
        return L"EV_COMPACT_HASH";
    case EV_IPL:
        return L"EV_IPL";
    case EV_IPL_PARTITION_DATA:
        return L"EV_IPL_PARTITION_DATA";
    case EV_NONHOST_CODE:
        return L"EV_NONHOST_CODE";
    case EV_NONHOST_CONFIG:
        return L"EV_NONHOST_CONFIG";
    case EV_NONHOST_INFO:
        return L"EV_NONHOST_INFO";
    case EV_OMIT_BOOT_DEVICE_EVENTS:
        return L"EV_OMIT_BOOT_DEVICE_EVENTS";
    case EV_EFI_VARIABLE_DRIVER_CONFIG:
        return L"EV_EFI_VARIABLE_DRIVER_CONFIG";
    case EV_EFI_VARIABLE_BOOT:
        return L"EV_EFI_VARIABLE_BOOT";
    case EV_EFI_BOOT_SERVICES_APPLICATION:
        return L"EV_EFI_BOOT_SERVICES_APPLICATION";
    case EV_EFI_BOOT_SERVICES_DRIVER:
        return L"EV_EFI_BOOT_SERVICES_DRIVER";
    case EV_EFI_RUNTIME_SERVICES_DRIVER:
        return L"EV_EFI_RUNTIME_SERVICES_DRIVER";
    case EV_EFI_GPT_EVENT:
        return L"EV_EFI_GPT_EVENT";
    case EV_EFI_ACTION:
        return L"EV_EFI_ACTION";
    case EV_EFI_PLATFORM_FIRMWARE_BLOB:
        return L"EV_EFI_PLATFORM_FIRMWARE_BLOB";
    case EV_EFI_HANDOFF_TABLES:
        return L"EV_EFI_HANDOFF_TABLES";
    case EV_EFI_VARIABLE_AUTHORITY:
        return L"EV_EFI_VARIABLE_AUTHORITY";
    default:
        return L"Unknown event type";
    }
}
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
