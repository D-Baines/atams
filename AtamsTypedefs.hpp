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
#include "Utilities/List.hpp"

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
inline constexpr uint8_t  USER_DATA_BLOCK_ID_START       = 1U;
inline constexpr uint16_t MAX_MESH_PACKET_DATAGRAM_COUNT = 256U;
inline constexpr uint16_t MAX_MESH_PACKET_SIZE           = 256U;
inline constexpr uint16_t MAX_NODE_PACKET_SIZE           = 256U;
inline constexpr uint8_t  MAX_NUMBER_OF_MESH             = 10U;
inline constexpr uint8_t  MAX_NUMBER_OF_DATA_BLOCKS      = 14U;
inline constexpr uint8_t  BLOCK_ID_ERROR_INDICATOR       = 14U;
inline constexpr uint16_t MAX_NUMBER_OF_DATA_MEMBERS     = 512U;
inline constexpr uint8_t  MAX_TYPE_SIZE                  = 4U;
inline constexpr uint8_t  EOL_BYTE                       = 0U;
inline constexpr int32_t  MAX_INT32                      = 2147483647U;
inline constexpr int32_t  MIN_INT32                      = -2147483648U;
inline constexpr uint32_t MAX_UINT32                     = 4294967295U;
inline constexpr uint32_t MIN_UINT32                     = 0U;
inline constexpr uint8_t  BITS_IN_A_BYTE                 = 8U;
inline constexpr uint32_t CRC32_POLYNOMIAL               = 0x04C11DB7;

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

typedef enum: uint8_t
{
  SYSTEM_UNKNOWN     = 0U,
  SYSTEM_HUB         = 1U,
  SYSTEM_NODE        = 2U,
  SYSTEM_NULL        = 255U,
} SystemType_t;

typedef enum: uint8_t
{
  MESSAGE_UNKNOWN             = 0U,
  MESSAGE_BROADCAST_UNIVERSAL = 1U,
  MESSAGE_REQUEST             = 2U,
  MESSAGE_REQUEST_SYNCED      = 3U,
  MESSAGE_RESPONSE_SYNCED     = 4U,
  MESSAGE_SYNC_JOG            = 5U,
  MESSAGE_ABORTED_RESPONSE    = 6U,
} MessageType_t;

typedef enum: uint8_t
{
  MESH_SIZE_MSG_TYPE = sizeof(MessageType_t),
  MESH_SIZE_SYNC     = sizeof(uint8_t),
  MESH_SIZE_CRC      = sizeof(uint32_t),
  MESH_SIZE_NODE_ID  = sizeof(uint8_t),
  MESH_SIZE_HEADER   = MESH_SIZE_MSG_TYPE + MESH_SIZE_SYNC + MESH_SIZE_CRC + MESH_SIZE_NODE_ID,
} MeshSize_t;

typedef enum: uint8_t
{
  MESH_INDEX_MSG_TYPE       = 0U,
  MESH_INDEX_SYNC           = MESH_INDEX_MSG_TYPE + MESH_SIZE_MSG_TYPE,
  MESH_INDEX_CRC            = MESH_INDEX_SYNC     + MESH_SIZE_SYNC,
  MESH_INDEX_NODE_ID        = MESH_INDEX_CRC      + MESH_SIZE_CRC,
  MESH_INDEX_FIRST_DATAGRAM = MESH_SIZE_HEADER,
} MeshIndex_t;

typedef enum: uint8_t
{
  DATAGRAM_SIZE_HEADER = 2U,
} DatagramSize_t;

typedef enum: uint8_t
{
  DATAGRAM_INDEX_HEADER  = 0U,
  DATAGRAM_INDEX_PAYLOAD = DATAGRAM_SIZE_HEADER,

} DatagramIndex_t;

typedef enum: uint8_t
{
  DATAGRAM_HEADER_SHIFT_COMMAND   = 5U,
  DATAGRAM_HEADER_SHIFT_BLOCK_ID  = 1U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_HI = 8U,
  DATAGRAM_HEADER_SHIFT_VAR_ID_LO = 0U
} DatagramHeaderShift_t;

typedef enum: uint8_t
{
  DATAGRAM_HEADER_MASK_COMMAND   = 0xE0U,
  DATAGRAM_HEADER_MASK_BLOCK_ID  = 0x1EU,
  DATAGRAM_HEADER_MASK_VAR_ID_HI = 0x01U,
  DATAGRAM_HEADER_MASK_VAR_ID_LO = 0xFFU,
} DatagramHeaderMask_t;

typedef enum: uint8_t
{
  ERROR_NONE                     = 0U,
  ERROR_BLOCK_ID                 = 1U,
  ERROR_VAR_ID                   = 2U,
  ERROR_VAR_TYPE                 = 3U,
  ERROR_VAR_LENGTH               = 4U,
  ERROR_REQUEST_BUFFER_LENGTH    = 5U,
  ERROR_NULL_PTR                 = 6U,
  ERROR_ACCESS_INVALID           = 7U,
  ERROR_REQUEST_PATTERN_INVALID  = 8U,
  ERROR_MEMORY                   = 9U,
  ERROR_WRITE_LIST               = 10U,
  ERROR_NODE_FATAL               = 11U,
  ERROR_MEMBER_PATH_INVALID      = 12U,
  ERROR_RESPONSE_BUFFER_LENGTH   = 13U,
  ERROR_ABORT_FAILURE            = 14U,
  ERROR_ENCODE                   = 15U,
  ERROR_DECODE                   = 16U,
  ERROR_MESSAGE_TYPE             = 17U,
  ERROR_SYNC_COUNT               = 18U,
  ERROR_SYNC_NODE                = 19U,
  ERROR_COMMAND_RESPONSE_INVALID = 20U,
  ERROR_WRITE_DATA_UPDATE        = 21U,
  ERROR_PATTERN_AUTO_UPDATE      = 22U,
  ERROR_PACKET_PROCESSING        = 23U,
  ERROR_NODE_ID_LIST             = 24U,
  ERROR_DATAGRAM_SEARCH          = 25U,
  ERROR_ERROR_MANAGEMENT         = 26U,
  ERROR_PLATFORM                 = 27U,
  ERROR_LIMITS                   = 28U,
  ERROR_WRITE_LOCK               = 29U,
  ERROR_BUS_FULL                 = 30U,
  ERROR_UPDATE_CYCLE_IN_PROGRESS = 31U,

  NUMBER_OF_ATAMS_ERRORS
} Error_t;

typedef enum: uint8_t
{
  ACCESS_NONE  = 0U,
  ACCESS_READ  = 1U,
  ACCESS_WRITE = 2U,
} Access_t;

typedef enum: uint8_t
{
  RESPONSE_NACK      = 0U,
  RESPONSE_ACK_READ  = 1U,
  RESPONSE_ACK_WRITE = 2U,
  RESPONSE_FATAL     = 3U
} AccessResponse_t;

typedef enum: uint8_t
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
} DataType_t;

typedef enum: uint8_t
{
  REQUEST_INACTIVE           = 0U,
  REQUEST_ACTIVE             = 1U,
  REQUEST_UNTIL_ACK          = 2U,
  NUMBER_OF_REQUEST_PATTERNS = 3U
} RequestPattern_t;

struct DatagramHeader_t
{
  uint8_t  command;
  uint8_t  blockID;
  uint16_t varID;
};

template <typename T>
struct DataStatusReturn_t
{
  T       data;
  Error_t status;
};

/*-- COMMS --------------------------------------------------------------------------*/

typedef enum: uint8_t
{
  COMMS_TRANSMIT = 0U,
  COMMS_RECEIVE  = 1U,
} CommsDirection_t;

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

constexpr uint8_t TYPE_LENGTHS[NUMBER_OF_TYPES] =
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


