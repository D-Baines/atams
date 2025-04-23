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

inline constexpr uint8_t  NODE_ID_MAX                    = 254U;
inline constexpr uint8_t  NODE_ID_NULL                   = 255U;
inline constexpr uint8_t  MAX_NUMBER_OF_NODE_IDS         = 255U;
inline constexpr uint8_t  BLOCK_ID_UNIVERSAL             = 0U;
inline constexpr uint8_t  FIRST_USER_BLOCK_ID            = 1U;
inline constexpr uint8_t  USER_DATA_BLOCK_ID_START       = 1U;
inline constexpr uint16_t MAX_MESH_PACKET_DATAGRAM_COUNT = 256U;
inline constexpr uint16_t MAX_MESH_PACKET_SIZE           = 256U;
inline constexpr uint16_t MAX_NODE_PACKET_SIZE           = 256U;
inline constexpr uint8_t  MAX_NUMBER_OF_MESH             = 10U;
inline constexpr uint8_t  MAX_NUMBER_OF_DATA_BLOCKS      = 14U;
inline constexpr uint16_t MAX_NUMBER_OF_VARS     = 512U;
inline constexpr uint8_t  MAX_TYPE_SIZE                  = 4U;
inline constexpr uint8_t  EOL_BYTE                       = 0U;
inline constexpr int32_t  MAX_INT32                      = 2147483647L;
inline constexpr int32_t  MIN_INT32                      = -2147483648L;
inline constexpr uint32_t MAX_UINT32                     = 4294967295U;
inline constexpr uint32_t MIN_UINT32                     = 0U;
inline constexpr uint8_t  BITS_IN_A_BYTE                 = 8U;
inline constexpr uint32_t CRC32_POLYNOMIAL               = 0x04C11DB7U;
inline constexpr uint32_t NVM_HEADER_IDENTIFIER_INVALID  = 0x00000000U;
inline constexpr uint32_t NVM_HEADER_IDENTIFIER_VALID    = 0xD0D0CACAU;
inline constexpr uint32_t CONFIGURATION_PASSKEY_ACCESS   = 0x0000000FU;
inline constexpr uint32_t CONFIGURATION_PASSKEY_APPLY    = 0x0000000FU;
inline constexpr uint32_t CONFIGURATION_PASSKEY_CANCEL   = 0x00000000U;
inline constexpr uint32_t STORE_ALL_PASSCODE             = 0x73617665U;
inline constexpr uint32_t RESTORE_USER_BLOCKS_PASSCODE   = 0x52455553U;
inline constexpr uint32_t RESTORE_ALL_PASSCODE           = 0x5245414CU;
inline constexpr uint32_t RESET_NODE_PASSCODE            = 0x0000B33FU;
inline constexpr uint32_t WATCHDOG_RESET_PASSCODE        = 0x72737477U;

/*************************************************************************************/
/* STATIC ASSERTIONS                                                                 */
/*************************************************************************************/

static_assert(sizeof(float) == Atams::MAX_TYPE_SIZE, "Platform float size incompatible with Atams");
static_assert(std::numeric_limits<float>::is_iec559, "Platform float representation incompatible with Atams" );

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

enum ProcessState_t: uint8_t
{
  PROCESS_STATE_READY       = 0U,
  PROCESS_STATE_IN_PROGRESS = 1U,
  PROCESS_STATE_ERROR       = 2U,
  PROCESS_STATE_COMPLETE    = 3U
};

enum SystemType_t: uint8_t
{
  SYSTEM_UNKNOWN = 0U,
  SYSTEM_HUB     = 1U,
  SYSTEM_NODE    = 2U,
  SYSTEM_NULL    = 255U,
};

enum MessageType_t: uint8_t
{
  MESSAGE_UNKNOWN             = 0U,
  MESSAGE_BROADCAST_UNIVERSAL = 1U,
  MESSAGE_REQUEST             = 2U,
  MESSAGE_RESPONSE            = 3U,
  MESSAGE_REQUEST_SYNCED      = 4U,
  MESSAGE_RESPONSE_SYNCED     = 5U,
  MESSAGE_SYNC_JOG            = 6U,
  MESSAGE_ABORTED_RESPONSE    = 7U,
};

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
  DATAGRAM_HEADER_SHIFT_BLOCK_ID  = 1U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_HI = 8U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_LO = 0U
};

enum DatagramHeaderMask_t: uint8_t
{
  DATAGRAM_HEADER_MASK_COMMAND   = 0xE0U,
  DATAGRAM_HEADER_MASK_BLOCK_ID  = 0x1EU,
  DATAGRAM_HEADER_MASK_VAR_ID_HI = 0x01U,
  DATAGRAM_HEADER_MASK_VAR_ID_LO = 0xFFU,
};

enum Error_t: uint8_t
{
  ERROR_NONE                         = 0U,
  ERROR_BLOCK_ID                     = 1U,
  ERROR_VAR_ID                       = 2U,
  ERROR_VAR_TYPE                     = 3U,
  ERROR_VAR_LENGTH                   = 4U,
  ERROR_REQUEST_BUFFER_LENGTH        = 5U,
  ERROR_RESPONSE_BUFFER_LENGTH       = 6U,
  ERROR_NULL_PTR                     = 7U,
  ERROR_ACCESS_INVALID               = 8U,
  ERROR_REQUEST_PATTERN_INVALID      = 9U,
  ERROR_MEMORY_MAP                   = 10U,
  ERROR_WRITE_LIST                   = 11U,
  ERROR_ABORT_FAILURE                = 12U,
  ERROR_ENCODE                       = 13U,
  ERROR_DECODE                       = 14U,
  ERROR_MESSAGE_TYPE                 = 15U,
  ERROR_SYNC_COUNT                   = 16U,
  ERROR_SYNC_NODE                    = 17U,
  ERROR_ACCESS_RESPONSE_INVALID      = 18U,
  ERROR_WRITE_DATA_UPDATE            = 19U,
  ERROR_PATTERN_AUTO_UPDATE          = 20U,
  ERROR_PACKET_PROCESSING            = 21U,
  ERROR_NODE_ID_LIST                 = 22U,
  ERROR_DATAGRAM_SEARCH              = 23U,
  ERROR_ERROR_MANAGEMENT             = 24U,
  ERROR_PLATFORM                     = 25U,
  ERROR_LIMITS                       = 26U,
  ERROR_WRITE_LOCK                   = 27U,
  ERROR_BUS_FULL                     = 28U,
  ERROR_UPDATE_CYCLE_IN_PROGRESS     = 29U,
  ERROR_OLD_DATA                     = 30U,
  ERROR_RESPONSE_TIMEOUT             = 31U,
  ERROR_NUMBER_OF_DATA_BLOCKS        = 32U,
  ERROR_NUMBER_OF_DATA_MEMBERS       = 33U,
  ERROR_NO_RESPONSE                  = 34U,
  ERROR_INIT_REQUIRED                = 35U,
  ERROR_NVM_CHECKSUM                 = 36U,
  ERROR_NVM_GEN_INFO                 = 37U,
  ERROR_NVM_HEADER_LENGTH            = 38U,
  ERROR_NVM_HEADER_VALIDITY          = 39U,
  ERROR_NVM_PLATFORM_SIZE            = 40U,
  ERROR_INVALID_NACK                 = 41U,
  ERROR_CONFIGURATION_STATE_INACTIVE = 42U,
  ERROR_BUS_PROCESSING               = 43U,

  NUMBER_OF_ATAMS_ERRORS
};

enum Access_t: uint8_t
{
  ACCESS_NONE  = 0U,
  ACCESS_READ  = 1U,
  ACCESS_WRITE = 2U,
};

enum AtamsBool_t: uint8_t
{
  ATAMS_FALSE = 0U,
  ATAMS_TRUE  = 1U
};

enum ConfigurationStatus_t
{
  CONFIGURATION_STATUS_INACTIVE = 0U,
  CONFIGURATION_STATUS_ACTIVE   = 1U,
  CONFIGURATION_STATUS_DENIED   = 2U,
};

enum AccessResponse_t: uint8_t
{
  RESPONSE_NACK      = 0U,
  RESPONSE_ACK_READ  = 1U,
  RESPONSE_ACK_WRITE = 2U,
  RESPONSE_FATAL     = 3U
};

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
  NUMBER_OF_TYPES
};

enum RequestPattern_t: uint8_t
{
  REQUEST_INACTIVE           = 0U,
  REQUEST_STREAM             = 1U,
  REQUEST_UNTIL_ACK          = 2U,
  NUMBER_OF_REQUEST_PATTERNS = 3U
};

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
    if ((type        == other.type       ) &&
        (accessLevel == other.accessLevel) &&
        (NVMStorage  == other.NVMStorage ) )
    {
      return (true);
    }

    return (false);
  }

  bool operator!=(const VarInfo_t &other)
  {
    if ((type        != other.type       ) ||
        (accessLevel != other.accessLevel) ||
        (NVMStorage  != other.NVMStorage ) )
    {
      return (true);
    }

    return (false);
  }

  static_assert(sizeof(type)        == 1U, "VarInfo_t member type size invalid");
  static_assert(sizeof(accessLevel) == 1U, "VarInfo_t member type size invalid");
  static_assert(sizeof(NVMStorage)  == 1U, "VarInfo_t member type size invalid");
};

struct DatagramHeader_t
{
  uint8_t  command;
  uint8_t  blockID;
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
        (genChecksum       == other.genChecksum      ) )
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
  }
};

struct NVMHeader_t
{
  uint32_t identifier = NVM_HEADER_IDENTIFIER_INVALID;
  uint32_t length     = 0U;
  uint32_t checksum   = 0U;
};

template <typename T>
struct DataStatusReturn_t
{
  T              data;
  Atams::Error_t status;
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

/*************************************************************************************/
/* POST-TYPEDEF CONSTANTS                                                            */
/*************************************************************************************/

constexpr uint8_t TYPE_LENGTHS[Atams::NUMBER_OF_TYPES] =
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


