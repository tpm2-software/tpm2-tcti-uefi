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

#ifndef PACKED
#define PACKED __attribute__((__packed__))
#endif

typedef struct tdEFI_TCG2_PROTOCOL EFI_TCG2_PROTOCOL;

/* structures */
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
