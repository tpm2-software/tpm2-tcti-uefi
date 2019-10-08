/* SPDX-License-Identifier: BSD-2 */
#include <efi/efi.h>
#include <efi/efilib.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <tss2/tss2_tpm2_types.h>

#include "tcg2-protocol.h"
#include "tcg2-util.h"
#include "util.h"

#define TRUE_STR L"true"
#define FALSE_STR L"false"

wchar_t*
bitmap_val_str (UINT32 member, UINT32 selector)
{
    if (member & selector) {
        return TRUE_STR;
    } else {
        return FALSE_STR;
    }
}

void
tcg2_algorithm_bitmap_prettyprint (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap)
{
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA1: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA1));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA256: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA256));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA384: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA384));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SHA512: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SHA512));
    Print (L"    EFI_TCG2_BOOT_HASH_ALG_SM3_256: %s\n",
        bitmap_val_str (bitmap, EFI_TCG2_BOOT_HASH_ALG_SM3_256));
}
wchar_t*
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
size_t
get_alg_size (UINT16 alg_id)
{
    switch (alg_id) {
    case TPM2_ALG_SHA1:
        return TPM2_SHA1_DIGEST_SIZE;
    case TPM2_ALG_SHA256:
        return TPM2_SHA256_DIGEST_SIZE;
    case TPM2_ALG_SHA384:
        return TPM2_SHA384_DIGEST_SIZE;
    case TPM2_ALG_SHA512:
        return TPM2_SHA512_DIGEST_SIZE;
    case TPM2_ALG_SM3_256:
        return TPM2_SM3_256_DIGEST_SIZE;
    default:
        return 0;
    }
}
wchar_t*
get_alg_name (UINT16 alg_id)
{
    switch (alg_id) {
    case TPM2_ALG_SHA1:
        return L"EFI_TCG2_BOOT_HASH_ALG_SHA1";
    case TPM2_ALG_SHA256:
        return L"EFI_TCG2_BOOT_HASH_ALG_SHA256";
    case TPM2_ALG_SHA384:
        return L"EFI_TCG2_BOOT_HASH_ALG_SHA384";
    case TPM2_ALG_SHA512:
        return L"EFI_TCG2_BOOT_HASH_ALG_SHA512";
    case TPM2_ALG_SM3_256:
        return L"EFI_TCG2_BOOT_HASH_ALG_SM3_256";
    default:
        return L"UNKNOWN_ALGORITHM";
    }
}
/*
 * "high" in this context means the supported event log format with the
 * highest version number
 * In the exceptional (your firmware is broken) case that the UEFI TCG2
 * protocol driver supports only a format that we don't understand this
 * function will still return EFI_SUCCESS, but the 'format' parameter
 * will be set to 0.
 */
EFI_STATUS
get_eventlog_format_high (EFI_TCG2_PROTOCOL *tcg2_protocol,
                          EFI_TCG2_EVENT_LOG_FORMAT *format)
{
    EFI_STATUS status;
    EFI_TCG2_BOOT_SERVICE_CAPABILITY caps = {
        .Size = sizeof (EFI_TCG2_BOOT_SERVICE_CAPABILITY),
    };

    status = tcg2_get_capability (tcg2_protocol, &caps);
    if (EFI_ERROR (status))
        return status;

    if (caps.SupportedEventLogs && EFI_TCG2_EVENT_LOG_FORMAT_TCG_2)
        *format = EFI_TCG2_EVENT_LOG_FORMAT_TCG_2;
    else if (caps.SupportedEventLogs && EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2)
        *format = EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2;
    else {
        *format = 0;
    }
    return EFI_SUCCESS;
}
TCG_DIGEST2*
get_next_digest (TCG_DIGEST2* digest)
{
    return (TCG_DIGEST2*)(digest->Digest + get_alg_size (digest->AlgorithmId));
}
bool
is_pcr_selected (BYTE pcr_selection[],
                 uint8_t pcr)
{
    return pcr_selection [pcr/8] & (1 << (pcr % 8));
}
void
select_pcr (BYTE pcr_selection [],
            uint8_t pcr)
{
    pcr_selection [pcr/8] |= 1 << (pcr % 8);
}
void
deselect_pcr (BYTE pcr_selection [],
              uint8_t pcr)
{
    pcr_selection [pcr/8] &= ~(1 << (pcr % 8));
}
void
foreach_selection (TPML_PCR_SELECTION *pcr_selection,
                   PCR_SELECTION_CALLBACK callback,
                   void *data)
{
    BYTE *selection;
    TPMI_ALG_HASH hash;
    uint8_t pcr;
    size_t i;

    /* iterate over TPMS_PCR_SELECTION array from TPML_PCR_SELECTION param */
    for (i = 0; i < pcr_selection->count; ++i) {
        /* iterate over PCRs */
        for (pcr = 0; pcr < TPM2_MAX_PCRS; ++pcr) {
            selection = pcr_selection->pcrSelections [i].pcrSelect;
            /* if bit corresponding to pcr in selection is set */
            if (is_pcr_selected (selection, pcr)) {
                hash = pcr_selection->pcrSelections [i].hash;
                if (callback (hash, pcr, data) == false)
                    return;
            }
        }
    }
}
int
count_algs_in_bitmap (EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap)
{
    size_t i, bitcount = 0;

    for (i = 0; i < sizeof (bitmap) * 8; ++i) {
        bitcount += (bitmap >> i) & 1;
    }
    return bitcount;
}
void
select_all_active_pcrs (EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks,
                        TPML_PCR_SELECTION *selections)
{
    size_t i;

    selections->count = count_algs_in_bitmap (active_banks);
    for (i = 0; i < selections->count; ++i) {
        selections->pcrSelections [i].sizeofSelect = 3;
        memset (selections->pcrSelections [i].pcrSelect, 0xff, 3);
        if (active_banks & EFI_TCG2_BOOT_HASH_ALG_SHA1) {
            selections->pcrSelections [i].hash = TPM2_ALG_SHA1;
            active_banks &= ~(EFI_TCG2_BOOT_HASH_ALG_SHA1);
            continue;
        }
        if (active_banks & EFI_TCG2_BOOT_HASH_ALG_SHA256) {
            selections->pcrSelections [i].hash = TPM2_ALG_SHA256;
            active_banks &= ~(EFI_TCG2_BOOT_HASH_ALG_SHA256);
            continue;
        }
        if (active_banks & EFI_TCG2_BOOT_HASH_ALG_SHA384) {
            selections->pcrSelections [i].hash = TPM2_ALG_SHA384;
            active_banks &= ~(EFI_TCG2_BOOT_HASH_ALG_SHA384);
            continue;
        }
        if (active_banks & EFI_TCG2_BOOT_HASH_ALG_SHA512) {
            selections->pcrSelections [i].hash = TPM2_ALG_SHA512;
            active_banks &= ~(EFI_TCG2_BOOT_HASH_ALG_SHA512);
            continue;
        }
        if (active_banks & EFI_TCG2_BOOT_HASH_ALG_SM3_256) {
            selections->pcrSelections [i].hash = TPM2_ALG_SM3_256;
            active_banks &= ~(EFI_TCG2_BOOT_HASH_ALG_SM3_256);
            continue;
        }
    }
}
void
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
size_t
sizeof_digest2 (TCG_DIGEST2 *digest)
{
    return sizeof (*digest) + get_alg_size (digest->AlgorithmId);
}
bool
prettyprint_tpm2_digest_callback (TCG_DIGEST2 *digest,
                                  void *data)
{
    size_t *size = (size_t*)data;

    prettyprint_tpm2_digest (digest);
    if (size) {
        *size += sizeof_digest2 (digest);
    }
    return true;
}
void
prettyprint_tpm2_event_header (TCG_EVENT_HEADER2 *event_hdr)
{
    Print (L"  PCRIndex: %d\n", event_hdr->PCRIndex);
    Print (L"  EventType: %s (0x%" PRIx32 ")\n",
           eventtype_to_string (event_hdr->EventType),
           event_hdr->EventType);
    Print (L"  DigestCount: %d\n", event_hdr->DigestCount);
}
void
prettyprint_tpm2_eventbuf (TCG_EVENT2 *event)
{
    Print (L"  Event: %" PRIu32 " bytes\n", event->EventSize);
    DumpHex (2, 0, event->EventSize, event->Event);
}

void
prettyprint_tpm2_event (TCG_EVENT_HEADER2 *event_hdr)
{
    TCG_EVENT2 *event;
    size_t digests_size = 0;
    bool ret;

    if (event_hdr == NULL) {
        return;
    }
    prettyprint_tpm2_event_header (event_hdr);
    ret = foreach_digest2 (event_hdr,
                           prettyprint_tpm2_digest_callback,
                           &digests_size);
    if (!ret)
        return;
    event = (TCG_EVENT2*)((uintptr_t)event_hdr->Digests + digests_size);
    prettyprint_tpm2_eventbuf (event);
}
bool
prettyprint_tpm2_event_callback (TCG_EVENT_HEADER2 *event_hdr,
                                 void *data)
{
    prettyprint_tpm2_event (event_hdr);
    return true;
}
bool
foreach_digest2 (TCG_EVENT_HEADER2 *event_hdr,
                 DIGEST2_CALLBACK callback,
                 void *data)
{
    bool ret = true;

    TCG_DIGEST2 *digest = (TCG_DIGEST2*)event_hdr->Digests;
    for (size_t i = 0; i < event_hdr->DigestCount && digest != NULL; ++i) {
        ret = callback (digest, data);
        if (!ret)
            break;
        digest = get_next_digest (digest);
    }
    return ret;
}
typedef bool (*EVENT2_CALLBACK) (TCG_EVENT_HEADER2 *event_hdr,
                                 void *data);
static bool
digest2_accumulator_callback (TCG_DIGEST2 *digest,
                              void *data)
{
    size_t *size = (size_t*)data;
    if (size == NULL)
        return false;

    *size += sizeof_digest2 (digest);
    return true;
}
TCG_EVENT_HEADER2*
get_next_event (TCG_EVENT_HEADER2 *event_hdr)
{
    size_t digests_size = 0;
    bool ret;
    TCG_EVENT2 *event;

    ret = foreach_digest2 (event_hdr,
                           digest2_accumulator_callback,
                           &digests_size);
    if (!ret)
        return NULL;
    event = (TCG_EVENT2*)((uintptr_t)event_hdr->Digests + digests_size);
    return (TCG_EVENT_HEADER2*)((uintptr_t)event->Event + event->EventSize);
}
bool
foreach_event2 (TCG_EVENT_HEADER2 *event_first,
                TCG_EVENT_HEADER2 *event_last,
                EVENT2_CALLBACK callback,
                void *data)
{
    TCG_EVENT_HEADER2 *event;

    if (event_first == NULL || event_last == NULL || callback == NULL)
        return false;

    for (event = event_first;
         event <= event_last && event != NULL;
         event = get_next_event (event))
    {
        if (!callback (event, data))
            return false;
    }
    return true;
}
