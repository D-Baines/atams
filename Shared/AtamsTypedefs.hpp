/**
  ******************************************************************************
  * @file    AtamsTypedefs.hpp
  *
  * @author  D. Baines
  *
  * @brief
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include <limits>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PRE-TYPEDEF CONSTANTS                                                             */
/*************************************************************************************/

inline constexpr uint8_t ATAMS_VERSION_MAJOR = 0U;
inline constexpr uint8_t ATAMS_VERSION_MINOR = 1U;

inline constexpr uint8_t  NODE_ID_MAX                   = 254U;
inline constexpr uint8_t  MAX_NUMBER_OF_NODES_PER_BUS   = 255U;
inline constexpr uint16_t MAX_NUMBER_OF_VARS            = 8191U;
inline constexpr uint16_t VAR_ID_NULL                   = 8191U;
inline constexpr uint8_t  MAX_TYPE_SIZE                 = 4U;
inline constexpr uint8_t  EOL_BYTE                      = 0U;
inline constexpr int32_t  MAX_INT32                     = 2147483647L;
inline constexpr int32_t  MIN_INT32                     = -2147483648L;
inline constexpr uint32_t MAX_UINT32                    = 4294967295U;
inline constexpr uint32_t MIN_UINT32                    = 0U;
inline constexpr uint8_t  BITS_IN_A_BYTE                = 8U;
inline constexpr uint32_t CRC32_POLYNOMIAL              = 0x04C11DB7U;
inline constexpr uint32_t NVM_HEADER_IDENTIFIER_INVALID = 0x00000000U;
inline constexpr uint32_t NVM_HEADER_IDENTIFIER_VALID   = 0xD0D0CACAU;
inline constexpr uint32_t CONFIGURATION_PASSKEY_ACCESS  = 0x454E5452U;
inline constexpr uint32_t CONFIGURATION_PASSKEY_APPLY   = 0x41504C59U;
inline constexpr uint32_t CONFIGURATION_PASSKEY_CANCEL  = 0x00000000U;
inline constexpr uint32_t STORE_ALL_PASSCODE            = 0x53415645U;
inline constexpr uint32_t RESTORE_USER_BLOCKS_PASSCODE  = 0x55534552U;
inline constexpr uint32_t RESTORE_ALL_PASSCODE          = 0x52535452U;
inline constexpr uint32_t RESET_NODE_PASSCODE           = 0x4E525354U;
inline constexpr uint32_t WATCHDOG_RESET_PASSCODE       = 0x57444F47U;

/*************************************************************************************/
/* STATIC ASSERTIONS                                                                 */
/*************************************************************************************/

static_assert(sizeof(float) == Atams::MAX_TYPE_SIZE, "Platform float size incompatible with Atams");
static_assert(std::numeric_limits<float>::is_iec559, "Platform float representation incompatible with Atams" );

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

enum class ProcessState: uint8_t
{
  IN_PROGRESS = 0U,
  ERROR       = 1U,
  COMPLETE    = 2U
};

enum MessageType_t: uint8_t
{
  MESSAGE_UNKNOWN             = 0U,
  MESSAGE_BROADCAST_UNIVERSAL = 1U,
  MESSAGE_BROADCAST_RESPONSE  = 2U,
  MESSAGE_REQUEST             = 3U,
  MESSAGE_RESPONSE            = 4U,
  MESSAGE_REQUEST_SYNCED      = 5U,
  MESSAGE_RESPONSE_SYNCED     = 6U,
  MESSAGE_SYNC_JOG            = 7U,
  MESSAGE_ABORTED_RESPONSE    = 8U,
};
static_assert(sizeof(Atams::MessageType_t) == 1U, "Atams::MessageType_t size invalid");

enum MeshSize_t: uint8_t
{
  MESH_SIZE_MSG_TYPE = sizeof(MessageType_t),
  MESH_SIZE_SYNC     = sizeof(uint8_t),
  MESH_SIZE_CRC      = sizeof(uint32_t),
  MESH_SIZE_NODE_ID  = sizeof(uint8_t),
  MESH_SIZE_HEADER   = MESH_SIZE_MSG_TYPE + MESH_SIZE_SYNC + MESH_SIZE_CRC + MESH_SIZE_NODE_ID,
};

enum MeshIndex_t: uint8_t
{
  MESH_INDEX_MSG_TYPE       = 0U,
  MESH_INDEX_SYNC           = MESH_INDEX_MSG_TYPE + MESH_SIZE_MSG_TYPE,
  MESH_INDEX_CRC            = MESH_INDEX_SYNC     + MESH_SIZE_SYNC,
  MESH_INDEX_NODE_ID        = MESH_INDEX_CRC      + MESH_SIZE_CRC,
  MESH_INDEX_FIRST_DATAGRAM = MESH_SIZE_HEADER,
};

enum DatagramSize_t: uint8_t
{
  DATAGRAM_SIZE_HEADER = 2U,
};

enum DatagramIndex_t: uint8_t
{
  DATAGRAM_INDEX_HEADER  = 0U,
  DATAGRAM_INDEX_PAYLOAD = DATAGRAM_SIZE_HEADER,
};

enum DatagramHeaderShift_t: uint8_t
{
  DATAGRAM_HEADER_SHIFT_COMMAND   = 5U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_HI = 8U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_LO = 0U
};

enum DatagramHeaderMask_t: uint8_t
{
  DATAGRAM_HEADER_MASK_COMMAND   = 0xE0U,
  DATAGRAM_HEADER_MASK_VAR_ID_HI = 0x1FU,
  DATAGRAM_HEADER_MASK_VAR_ID_LO = 0xFFU,
};

enum AbortSize_t: uint8_t
{
  ABORT_SIZE_ERROR     = sizeof(uint8_t),
  ABORT_SIZE_VAR_ID_HI = sizeof(uint8_t),
  ABORT_SIZE_VAR_ID_LO = sizeof(uint8_t),
  ABORT_SIZE_PACKET    = MESH_SIZE_HEADER + ABORT_SIZE_ERROR + ABORT_SIZE_VAR_ID_HI + ABORT_SIZE_VAR_ID_LO
};

enum AbortIndex_t: uint8_t
{
  ABORT_INDEX_ERROR     = MESH_SIZE_HEADER,
  ABORT_INDEX_VAR_ID_HI = ABORT_INDEX_ERROR     + ABORT_SIZE_ERROR,
  ABORT_INDEX_VAR_ID_LO = ABORT_INDEX_VAR_ID_HI + ABORT_SIZE_VAR_ID_HI
};

enum AbortShift_t: uint8_t
{
  ABORT_SHIFT_VAR_ID_HI = 8U,
  ABORT_SHIFT_VAR_ID_LO = 0U
};

enum AbortMask_t: uint16_t
{
  ABORT_MASK_VAR_ID_HI = 0xFFU,
  ABORT_MASK_VAR_ID_LO = 0xFFU,
};

enum Error_t: uint8_t
{
  ERROR_NONE                         = 0U,
  ERROR_VAR_ID                       = 1U,
  ERROR_VAR_TYPE                     = 2U,
  ERROR_ACCESS_INVALID               = 3U,
  ERROR_REQUEST_BUFFER_LENGTH        = 4U,
  ERROR_RESPONSE_BUFFER_LENGTH       = 5U,
  ERROR_NULLPTR                      = 6U,
  ERROR_REQUEST_PATTERN_INVALID      = 7U,
  ERROR_MEMORY_MAP                   = 8U,
  ERROR_WRITE_LIST_FULL              = 9U,
  ERROR_ABORT_FAILURE                = 10U,
  ERROR_ENCODE                       = 11U,
  ERROR_DECODE_FRAMING               = 12U,
  ERROR_DECODE_CHECKSUM              = 13U,
  ERROR_MESSAGE_TYPE                 = 14U,
  ERROR_SYNC_COUNT                   = 15U,
  ERROR_SYNC_NODE                    = 16U,
  ERROR_ACCESS_RESPONSE_INVALID      = 17U,
  ERROR_PATTERN_AUTO_UPDATE          = 18U,
  ERROR_PACKET_PROCESSING            = 19U,
  ERROR_DATAGRAM_SEARCH              = 20U,
  ERROR_ERROR_MANAGEMENT             = 21U,
  ERROR_PLATFORM                     = 22U,
  ERROR_WRITE_LOCK                   = 23U,
  ERROR_BUS_FULL                     = 24U,
  ERROR_BUS_EMPTY                    = 25U,
  ERROR_UPDATE_CYCLE_IN_PROGRESS     = 26U,
  ERROR_NEW_DATA_NOT_READY           = 27U,
  ERROR_ACK_NOT_RECEIVED             = 28U,
  ERROR_RESPONSE_TIMEOUT             = 29U,
  ERROR_NO_RESPONSE                  = 30U,
  ERROR_INIT_ORDER                   = 31U,
  ERROR_NVM_CHECKSUM                 = 32U,
  ERROR_GEN_INFO_MISMATCH            = 33U,
  ERROR_NVM_HEADER_LENGTH            = 34U,
  ERROR_NVM_HEADER_VALIDITY          = 35U,
  ERROR_NVM_PLATFORM_SIZE            = 36U,
  ERROR_NVM_WRITE_ORDER              = 37U,
  ERROR_INVALID_NACK                 = 38U,
  ERROR_CONFIGURATION_STATE_DENIED   = 39U,
  ERROR_CONFIGURATION_STATE_INACTIVE = 40U,
  ERROR_REQUEST_PACKET_FATAL         = 41U,
  ERROR_INVALID_CASE                 = 42U,
  ERROR_SET_CONFIG_VAR_FAILED        = 43U,
  ERROR_STORAGE_PROCESS_FAILED       = 44U,
  ERROR_PROCESS_TIMEOUT              = 45U,
  ERROR_INITIALISATION_REQUIRED      = 46U,
  ERROR_ID_ASSIGNMENT_FAILED         = 47U,
  ERROR_CONFIGURATION_EXIT           = 48U,
  ERROR_NODE_ALREADY_ON_BUS          = 49U,

  NUMBER_OF_ATAMS_ERRORS
};
static_assert(sizeof(Atams::Error_t) == 1U, "Atams::Error_t size invalid");

enum Access_t: uint8_t
{
  ACCESS_NONE  = 0U,
  ACCESS_READ  = 1U,
  ACCESS_WRITE = 2U,
};
static_assert(sizeof(Atams::Error_t) == 1U, "Atams::Access_t size invalid");

enum Bool_t: uint8_t
{
  ATAMS_FALSE = 0U,
  ATAMS_TRUE  = 1U
};
static_assert(sizeof(Atams::Bool_t) == 1U, "Atams::Bool_t size invalid");

enum ConfigurationStatus_t: uint8_t
{
  CONFIGURATION_STATUS_INACTIVE = 0U,
  CONFIGURATION_STATUS_ACTIVE   = 1U,
  CONFIGURATION_STATUS_DENIED   = 2U,
  CONFIGURATION_STATUS_APPLIED  = 3U,
};
static_assert(sizeof(Atams::ConfigurationStatus_t) == 1U, "Atams::ConfigurationStatus_t size invalid");

enum AccessResponse_t: uint8_t
{
  RESPONSE_NACK      = 0U,
  RESPONSE_ACK_READ  = 1U,
  RESPONSE_ACK_WRITE = 2U,
  RESPONSE_FATAL     = 3U
};
static_assert(sizeof(Atams::AccessResponse_t) == 1U, "Atams::AccessResponse_t size invalid");

enum VarType_t: uint8_t
{
  TYPE_NULL   = 0U,
  TYPE_UINT8  = 1U,
  TYPE_INT8   = 2U,
  TYPE_UINT16 = 3U,
  TYPE_INT16  = 4U,
  TYPE_UINT32 = 5U,
  TYPE_INT32  = 6U,
  TYPE_FLOAT  = 7U,
  NUMBER_OF_VAR_TYPES
};
static_assert(sizeof(Atams::VarType_t) == 1U, "Atams::VarType_t size invalid");

enum RequestPattern_t: uint8_t
{
  REQUEST_INACTIVE           = 0U,
  REQUEST_STREAM             = 1U,
  REQUEST_UNTIL_ACK          = 2U,
  NUMBER_OF_REQUEST_PATTERNS = 3U
};
static_assert(sizeof(Atams::RequestPattern_t) == 1U, "Atams::RequestPattern_t size invalid");

enum BitrateOption_t: uint8_t
{
  BITRATE_OPTION_0 = 0U,
  BITRATE_OPTION_1 = 1U,
  BITRATE_OPTION_2 = 2U,
  BITRATE_OPTION_3 = 3U,
  BITRATE_OPTION_4 = 4U,
  BITRATE_OPTION_5 = 5U,
  BITRATE_OPTION_6 = 6U,
  BITRATE_OPTION_7 = 7U,
  BITRATE_OPTION_8 = 8U,
  BITRATE_OPTION_9 = 9U,
};
static_assert(sizeof(Atams::BitrateOption_t) == 1U, "Atams::BitrateOption_t size invalid");

enum CoreID_t: uint8_t
{
  CORE_CONTROL = 0U,
  CORE_COMMS   = 1U,
  NUMBER_OF_CORES
};

struct VarInfo_t
{
  VarType_t type        = Atams::TYPE_NULL;
  Access_t  accessLevel = Atams::ACCESS_NONE;
  uint8_t   NVMStorage  = Atams::ATAMS_FALSE;

  bool operator==(const VarInfo_t &other)
  {
    return ((type        == other.type       ) &&
            (accessLevel == other.accessLevel) &&
            (NVMStorage  == other.NVMStorage ) );
  }

  bool operator!=(const VarInfo_t &other)
  {
    return ((type        != other.type       ) ||
            (accessLevel != other.accessLevel) ||
            (NVMStorage  != other.NVMStorage ) );
  }

  static_assert(sizeof(type)        == 1U, "VarInfo_t member type size invalid");
  static_assert(sizeof(accessLevel) == 1U, "VarInfo_t member type size invalid");
  static_assert(sizeof(NVMStorage)  == 1U, "VarInfo_t member type size invalid");
};

struct DatagramHeader_t
{
  uint8_t  command;
  uint16_t varID;
};

struct GenInfo_t
{
  uint8_t  atamsVersionMajor  = 0U;
  uint8_t  atamsVersionMinor  = 0U;
  uint8_t  genDay             = 0U;
  uint8_t  genMonth           = 0U;
  uint16_t genYear            = 0U;
  uint8_t  genHour            = 0U;
  uint8_t  genMinute          = 0U;
  uint8_t  genSecond          = 0U;
  uint32_t genChecksum        = 0U;
  uint16_t noOfVars           = 0U;

  bool operator==(const GenInfo_t &other)
  {
    if ((atamsVersionMajor == other.atamsVersionMajor) &&
        (atamsVersionMinor == other.atamsVersionMinor) &&
        (genDay            == other.genDay           ) &&
        (genMonth          == other.genMonth         ) &&
        (genYear           == other.genYear          ) &&
        (genHour           == other.genHour          ) &&
        (genMinute         == other.genMinute        ) &&
        (genSecond         == other.genSecond        ) &&
        (genChecksum       == other.genChecksum      ) &&
        (noOfVars          == other.noOfVars         ) )
    {
      return (true);
    }
    return (false);
  }

  bool operator!=(const GenInfo_t &other)
  {
    if (*this == other)
    {
      return (false);
    }

    return (true);
  }

  void invalidate(void)
  {
    atamsVersionMajor = 0U;
    atamsVersionMinor = 0U;
    genDay            = 0U;
    genMonth          = 0U;
    genYear           = 0U;
    genHour           = 0U;
    genMinute         = 0U;
    genSecond         = 0U;
    genChecksum       = 0U;
    noOfVars          = 0U;
  }
};

struct NVMHeader_t
{
  uint32_t  identifier = NVM_HEADER_IDENTIFIER_INVALID;
  uint32_t  length     = 0U;
  GenInfo_t genInfo;
};

struct NVMFooter_t
{
  uint32_t identifier = NVM_HEADER_IDENTIFIER_INVALID;
  uint32_t checksum   = 0U;
};

template <typename T>
struct DataStatusReturn_t
{
  T              data;
  Atams::Error_t status;
};

struct NodeUserConfig_t
{
  uint8_t                nodeID;
  Atams::BitrateOption_t bitrateOption;
  uint32_t               watchdogPeriod;

  bool operator==(const NodeUserConfig_t &other)
  {
    return ((nodeID         == other.nodeID        ) &&
            (bitrateOption  == other.bitrateOption ) &&
            (watchdogPeriod == other.watchdogPeriod) );
  }
};

struct BusIDs_t
{
  uint8_t firstNodeID;
  uint8_t lastNodeID;
  uint8_t previousNodeID;

  bool operator==(const BusIDs_t &other)
  {
    return ((firstNodeID    == other.firstNodeID   ) &&
            (lastNodeID     == other.lastNodeID    ) &&
            (previousNodeID == other.previousNodeID) );
  }
};

struct TXMessage_t
{
  uint8_t      *bufferPtr    = nullptr;
  uint16_t      bufferLength = 0U;
  MessageType_t messageType  = MESSAGE_UNKNOWN;

  TXMessage_t(void)
  {
    bufferPtr    = nullptr;
    bufferLength = 0U;
    messageType  = MESSAGE_UNKNOWN;
  };

  TXMessage_t(uint8_t      *newBufferPtr,
              uint16_t      newBufferLength,
              MessageType_t newMessageType)
  {
    bufferPtr    = newBufferPtr;
    bufferLength = newBufferLength;
    messageType  = newMessageType;
  };

  TXMessage_t(const TXMessage_t &other)
  {
    bufferPtr    = other.bufferPtr;
    bufferLength = other.bufferLength;
    messageType  = other.messageType;
  };

  TXMessage_t & operator=(const TXMessage_t &other)
  {
    if (this == &other) return (*this);

    bufferPtr    = other.bufferPtr;
    bufferLength = other.bufferLength;
    messageType  = other.messageType;

    return (*this);
  }
};

typedef void (*CommsTransmitCallback_t)(TXMessage_t message);

struct SharedMemoryMap_t
{
  const uint16_t   noOfVars = 0U;
  const GenInfo_t  genInfo;
  const VarInfo_t *varInfoList;

  SharedMemoryMap_t(const uint16_t   initNumberOfVars,
                    const GenInfo_t  initGenInfo,
                    const VarInfo_t (*initVarInfoList)) :
  noOfVars(initNumberOfVars),
  genInfo(initGenInfo),
  varInfoList(initVarInfoList){};

  /* Default Constructor */
  SharedMemoryMap_t(void) = delete;

  /* Default Destructor */
  ~SharedMemoryMap_t(void){};

  /* Copy Constructor */
  SharedMemoryMap_t(const SharedMemoryMap_t &other) :
  noOfVars(other.noOfVars),
  genInfo(other.genInfo),
  varInfoList(other.varInfoList){};

  /* Copy Assignment Operator */
  SharedMemoryMap_t & operator=(const SharedMemoryMap_t &other) = delete;

  /* Move Constructor */
  SharedMemoryMap_t(SharedMemoryMap_t &&other) = delete;

  /* Move Assignment Operator */
  SharedMemoryMap_t & operator=(SharedMemoryMap_t &&other) = delete;
};

/*************************************************************************************/
/* POST-TYPEDEF CONSTANTS                                                            */
/*************************************************************************************/

static constexpr uint8_t TYPE_LENGTHS[Atams::NUMBER_OF_VAR_TYPES] =
{
  /* [TYPE_NULL  ] = */ 0U,
  /* [TYPE_UINT8 ] = */ 1U,
  /* [TYPE_INT8  ] = */ 1U,
  /* [TYPE_UINT16] = */ 2U,
  /* [TYPE_INT16 ] = */ 2U,
  /* [TYPE_UINT32] = */ 4U,
  /* [TYPE_INT32 ] = */ 4U,
  /* [TYPE_FLOAT ] = */ 4U,
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


