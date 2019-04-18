/* SPDX-License-Identifier: BSD-2 */
#ifndef TCG2_PROTOCOL_H
#define TCG2_PROTOCOL_H 1

#include "uefi-types.h"

/* IN / OUT denote parameter directionality in TCG EFI spec. They
 * don't do anything, just for readiility I guess.
 */
#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef UINT32 TCG_PCRINDEX;
typedef UINT32 TCG_EVENTTYPE;
typedef UINT8 TCG_DIGEST [20];

typedef UINT32 EFI_TCG2_EVENT_LOG_BITMAP;
typedef UINT32 EFI_TCG2_EVENT_LOG_FORMAT;
typedef UINT32 EFI_TCG2_EVENT_ALGORITHM_BITMAP;

#define EFI_TCG2_PROTOCOL_GUID \
  { 0x607f766c, 0x7455, 0x42be, \
    { 0x93, 0x0b, 0xe4, 0xd7, 0x6d, 0xb2, 0x72, 0x0f } \
  }

/* values for EFI_TCG2_EVENT_LOG_FORMAT */
#define EFI_TCG2_EVENT_LOG_FORMAT_TCG_1_2 0x00000001
#define EFI_TCG2_EVENT_LOG_FORMAT_TCG_2   0x00000002

/* values for EFI_TCG2_EVENT_ALGORITHM_BITMAP */
#define EFI_TCG2_BOOT_HASH_ALG_SHA1    0x00000001
#define EFI_TCG2_BOOT_HASH_ALG_SHA256  0x00000002
#define EFI_TCG2_BOOT_HASH_ALG_SHA384  0x00000004
#define EFI_TCG2_BOOT_HASH_ALG_SHA512  0x00000008
#define EFI_TCG2_BOOT_HASH_ALG_SM3_256 0x00000010

/*
 * Log event types. These are spread out over 2 specs:
 * "TCG EFI Protocol Specification For TPM Family 1.1 or 1.2" and
 * "TCG PC Client Specific Implementation Specification for Conventional BIOS"
 */
#define EV_PREBOOT_CERT            0x0
#define EV_POST_CODE               0x1
#define EV_UNUSED                  0x2
#define EV_NO_ACTION               0x3
#define EV_SEPARATOR               0x4
#define EV_ACTION                  0x5
#define EV_EVENT_TAG               0x6
#define EV_S_CRTM_CONTENTS         0x7
#define EV_S_CRTM_VERSION          0x8
#define EV_CPU_MICROCODE           0x9
#define EV_PLATFORM_CONFIG_FLAGS   0xa
#define EV_TABLE_OF_DEVICES        0xb
#define EV_COMPACT_HASH            0xc
#define EV_IPL                     0xd
#define EV_IPL_PARTITION_DATA      0xe
#define EV_NONHOST_CODE            0xf
#define EV_NONHOST_CONFIG          0x10
#define EV_NONHOST_INFO            0x11
#define EV_OMIT_BOOT_DEVICE_EVENTS 0x12

/*
 * TCG EFI Platform Specification For TPM Family 1.1 or 1.2
 */
#define EV_EFI_EVENT_BASE                0x80000000
#define EV_EFI_VARIABLE_DRIVER_CONFIG    EV_EFI_EVENT_BASE + 0x1
#define EV_EFI_VARIABLE_BOOT             EV_EFI_EVENT_BASE + 0x2
#define EV_EFI_BOOT_SERVICES_APPLICATION EV_EFI_EVENT_BASE + 0x3
#define EV_EFI_BOOT_SERVICES_DRIVER      EV_EFI_EVENT_BASE + 0x4
#define EV_EFI_RUNTIME_SERVICES_DRIVER   EV_EFI_EVENT_BASE + 0x5
#define EV_EFI_GPT_EVENT                 EV_EFI_EVENT_BASE + 0x6
#define EV_EFI_ACTION                    EV_EFI_EVENT_BASE + 0x7
#define EV_EFI_PLATFORM_FIRMWARE_BLOB    EV_EFI_EVENT_BASE + 0x8
#define EV_EFI_HANDOFF_TABLES            EV_EFI_EVENT_BASE + 0x9
#define EV_EFI_VARIABLE_AUTHORITY        EV_EFI_EVENT_BASE + 0xe0

#ifndef PACKED
#define PACKED __attribute__((__packed__))
#endif

typedef struct tdEFI_TCG2_PROTOCOL EFI_TCG2_PROTOCOL;

/* structures */
typedef struct {
  TCG_PCRINDEX  PCRIndex;
  TCG_EVENTTYPE EventType;
  TCG_DIGEST    Digest;
  UINT32        EventSize;
  UINT8         Event[1];
} PACKED TCG_PCR_EVENT;

typedef struct {
  UINT16 AlgorithmId;
  UINT8 Digest[];
} PACKED TCG_DIGEST2;

typedef struct {
  UINT32 EventSize;
  UINT8 Event [];
} PACKED TCG_EVENT2;

typedef struct {
  TCG_PCRINDEX PCRIndex;
  TCG_EVENTTYPE EventType;
  UINT32 DigestCount;
  TCG_DIGEST Digests[];
 /* TCG_EVENT2 comes next */
} PACKED TCG_EVENT_HEADER2;

typedef struct tdEFI_TCG2_EVENT_HEADER {
  UINT32        HeaderSize;
  UINT16        HeaderVersion;
  TCG_PCRINDEX  PCRIndex;
  TCG_EVENTTYPE EventType;
} PACKED EFI_TCG2_EVENT_HEADER;

typedef struct tdEFI_TCG2_EVENT {
  UINT32                Size;
  EFI_TCG2_EVENT_HEADER Header;
  UINT8                 Event[];
} PACKED EFI_TCG2_EVENT;

typedef struct tdEFI_TCG2_VERSION {
  UINT8 Major;
  UINT8 Minor;
} PACKED EFI_TCG2_VERSION;

/* This structure not packed per the TrEE protocol spec.
 * Microsoft and TCG disagree on this currently but TCG will likely adopt the
 * same convention as Win10 is deployed and TCG protocol spec is still
 * under public review.
 */
typedef struct tdEFI_TCG2_BOOT_SERVICE_CAPABILITY {
  UINT8                           Size;
  EFI_TCG2_VERSION                StructureVersion;
  EFI_TCG2_VERSION                ProtocolVersion;
  EFI_TCG2_EVENT_ALGORITHM_BITMAP HashAlgorithmBitmap;
  EFI_TCG2_EVENT_LOG_BITMAP       SupportedEventLogs;
  BOOLEAN                         TPMPresentFlag;
  UINT16                          MaxCommandSize;
  UINT16                          MaxResponseSize;
  UINT32                          ManufacturerID;
  UINT32                          NumberOfPcrBanks;
  EFI_TCG2_EVENT_ALGORITHM_BITMAP ActivePcrBanks;
} EFI_TCG2_BOOT_SERVICE_CAPABILITY;

/* function types */
typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_GET_CAPABILITY) (
  IN EFI_TCG2_PROTOCOL                    *This,
  IN OUT EFI_TCG2_BOOT_SERVICE_CAPABILITY *ProtocolCapability
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_GET_EVENT_LOG) (
  IN  EFI_TCG2_PROTOCOL         *This,
  IN  EFI_TCG2_EVENT_LOG_FORMAT EventLogFormat,
  OUT EFI_PHYSICAL_ADDRESS      *EventLogLocation,
  OUT EFI_PHYSICAL_ADDRESS      *EventLogLastEntry,
  OUT BOOLEAN                   *EventLogTruncated
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_HASH_LOG_EXTEND_EVENT) (
  IN EFI_TCG2_PROTOCOL    *This,
  IN UINT64               Flags,
  IN EFI_PHYSICAL_ADDRESS DataToHash,
  IN UINT64               DataToHashLen,
  IN EFI_TCG2_EVENT       *EfiTcgEvent
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_SUBMIT_COMMAND) (
  IN EFI_TCG2_PROTOCOL *This,
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN UINT32            OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_GET_ACTIVE_PCR_BANKS) (
  IN  EFI_TCG2_PROTOCOL *This,
  OUT UINT32            *ActivePcrBanks
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_SET_ACTIVE_PCR_BANKS) (
  IN EFI_TCG2_PROTOCOL *This,
  IN UINT32            ActivePcrBanks
);

typedef
EFI_STATUS
(EFIAPI *EFI_TCG2_GET_RESULT_OF_SET_ACTIVE_PCR_BANKS) (
  IN  EFI_TCG2_PROTOCOL *This,
  OUT UINT32            *OperationPresent,
  OUT UINT32            *Response
);

/* Protocol definition */
typedef struct tdEFI_TCG2_PROTOCOL {
  EFI_TCG2_GET_CAPABILITY                      GetCapability;
  EFI_TCG2_GET_EVENT_LOG                       GetEventLog;
  EFI_TCG2_HASH_LOG_EXTEND_EVENT               HashLogExtendEvent;
  EFI_TCG2_SUBMIT_COMMAND                      SubmitCommand;
  EFI_TCG2_GET_ACTIVE_PCR_BANKS                GetActivePcrBanks;
  EFI_TCG2_SET_ACTIVE_PCR_BANKS                SetActivePcrBanks;
  EFI_TCG2_GET_RESULT_OF_SET_ACTIVE_PCR_BANKS  GetResultOfSetActivePcrBanks;
} PACKED EFI_TCG2_PROTOCOL;

#endif /* TSS2_TCG2_H */
