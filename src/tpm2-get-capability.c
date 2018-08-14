/* SPDK-License-Identifier: BSD-2 */
#include <endian.h>
#include <inttypes.h>

#include <efi/efi.h>
#include <efi/efilib.h>

#include <tss2/tss2_mu.h>
#include <tss2/tss2_sys.h>

#include "compat.h"
#include "tss2-util.h"

#define TPM2_HEADER_SIZE 10

#define prop_str(val) val ? L"true" : L"false"

/*
 * There are a number of fixed TPM properties (tagged properties) that are
 * characters (8bit chars) packed into 32bit integers, trim leading and trailing spaces
 */
static char *
get_uint32_as_chars (UINT32    value)
{
    static char buf[5];

    value = be32toh (value);
    UINT8 *bytes = (UINT8 *)&value;

    /*
     * move the start of the string to the beginning
     * first non space character
     * Record the number of skips in i.
     */
    unsigned i;
    for(i=0; i < sizeof(value); i++) {
        UINT8 b = *bytes;
        if (!isspace(b)) {
            break;
        }
        bytes++;
    }

    /* record the number of trailing spaces in j */
    unsigned j;
    for(j=sizeof(value) - i; j > i; j--) {
        UINT8 b = bytes[j - 1];
        /* NULL bytes count as space */
        if (b && !isspace(b)) {
            break;
        }
    }

    memcpy(buf, bytes, j);
    buf[j] = '\0';
    return buf;
}
/*
 * Print string representations of the TPMA_MODES.
 */
static void
print_tpma_modes (TPMA_MODES    modes)
{
    Print (L"TPM2_PT_MODES:\n  raw: 0x%X\n", modes);
    if (modes & TPMA_MODES_FIPS_140_2)
        Print (L"  value: TPMA_MODES_FIPS_140_2\n");
    if (modes & TPMA_MODES_RESERVED1_MASK)
        Print (L"  value: TPMA_MODES_RESERVED1 (these bits shouldn't be set)\n");
}
/*
 * Iterate over all fixed properties, call the unique print function for each.
 */
static void
dump_tpm_properties_fixed (TPMS_TAGGED_PROPERTY properties[],
                           size_t               count)
{
    size_t i;
    char *buf;

    for (i = 0; i < count; ++i) {
        TPM2_PT property = properties[i].property;
        UINT32 value    = properties[i].value;
        switch (property) {
        case TPM2_PT_FAMILY_INDICATOR:
            buf = get_uint32_as_chars (value);
            Print (L"TPM2_PT_FAMILY_INDICATOR:\n"
                    "  raw: 0x08%x\n"
                    "  value: \"%s\"\n",
                    value,
                    buf);
            break;
        case TPM2_PT_LEVEL:
            Print (L"TPM2_PT_LEVEL:\n"
                    "  value: %d\n", value);
            break;
        case TPM2_PT_REVISION:
            Print (L"TPM2_PT_REVISION:\n"
                    "  value: %.2f\n", (float)value / 100);
            break;
        case TPM2_PT_DAY_OF_YEAR:
            Print (L"TPM2_PT_DAY_OF_YEAR:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_YEAR:
            Print (L"TPM2_PT_YEAR:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MANUFACTURER:
            Print (L"TPM2_PT_MANUFACTURER:\n"
                    " value        0x%X\n", value);
            break;
        case TPM2_PT_VENDOR_STRING_1:
            buf = get_uint32_as_chars (value);
            Print (L"TPM2_PT_VENDOR_STRING_1:\n"
                    "  raw: 0x%X\n"
                    "  value: \"%s\"\n",
                    value,
                    buf);
            break;
        case TPM2_PT_VENDOR_STRING_2:
            buf = get_uint32_as_chars (value);
            Print (L"TPM2_PT_VENDOR_STRING_2:\n"
                    "  raw: 0x%X\n"
                    "  value: \"%s\"\n",
                    value,
                    buf);
            break;
        case TPM2_PT_VENDOR_STRING_3:
            buf = get_uint32_as_chars (value);
            Print (L"TPM2_PT_VENDOR_STRING_3:\n"
                    "  raw: 0x%X\n"
                    "  value: \"%s\"\n",
                    value,
                    buf);
            break;
        case TPM2_PT_VENDOR_STRING_4:
            buf = get_uint32_as_chars (value);
            Print (L"TPM2_PT_VENDOR_STRING_4:\n"
                    "  raw: 0x%X\n"
                    "  value: \"%s\"\n",
                    value,
                    buf);
            break;
        case TPM2_PT_VENDOR_TPM_TYPE:
            Print (L"TPM2_PT_VENDOR_TPM_TYPE:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_FIRMWARE_VERSION_1:
            Print (L"TPM2_PT_FIRMWARE_VERSION_1:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_FIRMWARE_VERSION_2:
            Print (L"TPM2_PT_FIRMWARE_VERSION_2:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_INPUT_BUFFER:
            Print (L"TPM2_PT_INPUT_BUFFER:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_TPM2_HR_TRANSIENT_MIN:
            Print (L"TPM2_PT_TPM2_HR_TRANSIENT_MIN:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_TPM2_HR_PERSISTENT_MIN:
            Print (L"TPM2_PT_TPM2_HR_PERSISTENT_MIN:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_HR_LOADED_MIN:
            Print (L"TPM2_PT_HR_LOADED_MIN:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_ACTIVE_SESSIONS_MAX:
            Print (L"TPM2_PT_ACTIVE_SESSIONS_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PCR_COUNT:
            Print (L"TPM2_PT_PCR_COUNT:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PCR_SELECT_MIN:
            Print (L"TPM2_PT_PCR_SELECT_MIN:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_CONTEXT_GAP_MAX:
            Print (L"TPM2_PT_CONTEXT_GAP_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_NV_COUNTERS_MAX:
            Print (L"TPM2_PT_NV_COUNTERS_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_NV_INDEX_MAX:
            Print (L"TPM2_PT_NV_INDEX_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MEMORY:
            Print (L"TPM2_PT_MEMORY:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_CLOCK_UPDATE:
            Print (L"TPM2_PT_CLOCK_UPDATE:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_CONTEXT_HASH: /* this may be a TPM2_ALG_ID type */
            Print (L"TPM2_PT_CONTEXT_HASH:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_CONTEXT_SYM: /* this is a TPM2_ALG_ID type */
            Print (L"TPM2_PT_CONTEXT_SYM:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_CONTEXT_SYM_SIZE:
            Print (L"TPM2_PT_CONTEXT_SYM_SIZE:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_ORDERLY_COUNT:
            Print (L"TPM2_PT_ORDERLY_COUNT:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MAX_COMMAND_SIZE:
            Print (L"TPM2_PT_MAX_COMMAND_SIZE:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MAX_RESPONSE_SIZE:
            Print (L"TPM2_PT_MAX_RESPONSE_SIZE:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MAX_DIGEST:
            Print (L"TPM2_PT_MAX_DIGEST:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MAX_OBJECT_CONTEXT:
            Print (L"TPM2_PT_MAX_OBJECT_CONTEXT:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MAX_SESSION_CONTEXT:
            Print (L"TPM2_PT_MAX_SESSION_CONTEXT:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PS_FAMILY_INDICATOR:
            Print (L"TPM2_PT_PS_FAMILY_INDICATOR:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PS_LEVEL:
            Print (L"TPM2_PT_PS_LEVEL:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PS_REVISION:
            Print (L"TPM2_PT_PS_REVISION:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PS_DAY_OF_YEAR:
            Print (L"TPM2_PT_PS_DAY_OF_YEAR:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_PS_YEAR:
            Print (L"TPM2_PT_PS_YEAR:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_SPLIT_MAX:
            Print (L"TPM2_PT_SPLIT_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_TOTAL_COMMANDS:
            Print (L"TPM2_PT_TOTAL_COMMANDS:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_LIBRARY_COMMANDS:
            Print (L"TPM2_PT_LIBRARY_COMMANDS:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_VENDOR_COMMANDS:
            Print (L"TPM2_PT_VENDOR_COMMANDS:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_NV_BUFFER_MAX:
            Print (L"TPM2_PT_NV_BUFFER_MAX:\n"
                    "  value: 0x%X\n", value);
            break;
        case TPM2_PT_MODES:
            print_tpma_modes ((TPMA_MODES)value);
            break;
        default:
            Print (L"Unknown property: 0x%x\n", property);
            break;
        }
    }
}

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
    TSS2_SYS_CONTEXT *sys_ctx;
    TSS2_RC rc;
    TPMI_YES_NO more_data = TPM2_NO;
    /* TPMS_CAPABILITY_DATA capability_data = { .capability = 0 }; */
    TPMS_CAPABILITY_DATA capability_data = { 0 };

    InitializeLib (ImageHandle, SystemTable);
    rc = sys_uefi_init (&sys_ctx);
    if (rc != TSS2_RC_SUCCESS) {
        return EFI_ABORTED;
    }

    rc = Tss2_Sys_GetCapability (sys_ctx,
                                 NULL,
                                 TPM2_CAP_TPM_PROPERTIES,
                                 TPM2_PT_FIXED,
                                 TPM2_PT_MODES - TPM2_PT_FIXED,
                                 &more_data,
                                 &capability_data,
                                 NULL);
    if (rc != TSS2_RC_SUCCESS) {
        Print (L"Failed to Tss2_Sys_GetCapability: 0x%x\n", rc);
    }
    Print (L"Tss2_Sys_GetCapability returned TSS2_RC_SUCCESS\n");
    Print (L"Tss2_Sys_Getcapability has more data: %s\n",
           prop_str (more_data));

    Print (L"dump_tpm_properties_fixed\n");
    dump_tpm_properties_fixed (capability_data.data.tpmProperties.tpmProperty,
                               capability_data.data.tpmProperties.count);

    sys_uefi_teardown_full (sys_ctx);
    return EFI_SUCCESS;
}
