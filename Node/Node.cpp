/**
  ******************************************************************************
  * @file    Node.cpp
  *
  * @author  D. Baines
  *
  * @brief
  *
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "string.h"
#include <typeinfo>
#include "Node.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Utilities/CircularBuffer.hpp"
#include "../Utilities/CRC32.hpp"
#include "Developer/WatchdogHandler.hpp"
#include "Developer/ConfigurationHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint8_t  ABORT_RESPONSE_SIZE      = Atams::MESH_SIZE_HEADER + sizeof(Atams::Error_t);
static constexpr uint32_t CORE_STATUS_CHECK_PERIOD = 10U;

static const char * PLATFORM_TYPE_NAMES[NUMBER_OF_TYPES] =
{
  /* [TYPE_NULL  ] = */ "NULL",
  /* [TYPE_UINT8 ] = */ typeid(uint8_t ).name(),
  /* [TYPE_INT8  ] = */ typeid(int8_t  ).name(),
  /* [TYPE_UINT16] = */ typeid(uint16_t).name(),
  /* [TYPE_INT16 ] = */ typeid(int16_t ).name(),
  /* [TYPE_UINT32] = */ typeid(uint32_t).name(),
  /* [TYPE_INT32 ] = */ typeid(int32_t ).name(),
  /* [TYPE_FLOAT ] = */ typeid(float   ).name(),
};

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

enum NodeCommsState_t: uint8_t
{
  NODE_COMMS_UNINITIALISED = 0U,
  NODE_COMMS_INITIALISED   = 1U,
  NODE_COMMS_COMMS_ACTIVE  = 2U
};

enum CoreInitStatus_t: uint8_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
};

enum NVMTransfer_t : uint8_t
{
  TRANSFER_LOAD = 0U,
  TRANSFER_SAVE = 1U
};

struct Var_t
{
  uint8_t storage[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
};

struct ChannelResponse_t
{
  uint8_t  buffer[MAX_MESH_PACKET_SIZE];
  uint16_t index   = 0U;
  bool     aborted = false;
};

struct ChannelSyncPacket_t
{
  uint8_t  buffer[MAX_MESH_PACKET_SIZE];
  uint16_t length    = 0U;
  uint8_t  syncCount = 0U;
};

/*************************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static const MemoryMap_t   *s_memoryMap;
static CRC32                s_nodeCRC(Atams::CRC32_POLYNOMIAL);
static CircularBuffer       s_circularBuffers[Platform::NUMBER_OF_COMMS_CHANNELS];

static WatchdogHandler      s_watchdogHandler;
static ConfigurationHandler s_ConfigurationHandler(s_watchdogHandler);

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t         s_validVarCount            = 0U;
static NodeCommsState_t s_nodeCommsState           = NODE_COMMS_UNINITIALISED;
static bool             s_bufferResetRequired      = false;

/* Core Init Synchronisation */
ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static CoreInitStatus_t s_coreInitComplete[Atams::NUMBER_OF_CORES] = {CORE_INIT_IN_PROGRESS,
                                                                      CORE_INIT_IN_PROGRESS};

Var_t s_varStorage[Platform::NODE_NUMBER_OF_VARS] = {0U};

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void receiveCallback(const Platform::CommsChannel_t commsChannel,
                                  uint8_t                 *rxBufferPtr,
                            const uint16_t                 rxBufferLength)
{
  if (commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS)
  {
    s_circularBuffers[commsChannel].pushHead(rxBufferPtr, rxBufferLength);
    Platform::signalCommsBufferSemaphore();
  }
}

static inline void resetResponse(ChannelResponse_t &response)
{
  response.aborted                     = false;
  response.buffer[MESH_INDEX_NODE_ID]  = s_ConfigurationHandler.getLocalNodeID();
  response.buffer[MESH_INDEX_MSG_TYPE] = Atams::MESSAGE_UNKNOWN;
  response.index                       = MESH_INDEX_FIRST_DATAGRAM;
}

static inline void abortResponse(ChannelResponse_t &response, Atams::Error_t error)
{
  response.buffer[MESH_INDEX_NODE_ID]  = s_ConfigurationHandler.getLocalNodeID();;
  response.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_ABORTED_RESPONSE;
  response.buffer[MESH_SIZE_HEADER]    = error;
  response.index                       = MESH_SIZE_HEADER + sizeof(error);
  response.aborted                     = true;
}

static void sendResponsePacket(Platform::CommsChannel_t commsChannel,
                               ChannelResponse_t       &response,
                               Atams::MessageType_t     messageType)
{
  static uint8_t  encodedResponseBuffer[MAX_MESH_PACKET_SIZE] = {0U};
  static uint16_t encodedLength                               = 0U;

  if (response.aborted)
  {
    if ((response.buffer[MESH_INDEX_MSG_TYPE] != MESSAGE_ABORTED_RESPONSE) ||
        (response.index                       != ABORT_RESPONSE_SIZE     ) )
    {
      abortResponse(response, Atams::ERROR_ABORT_FAILURE);
    }
  }
  else
  {
    response.buffer[MESH_INDEX_MSG_TYPE] = messageType;
  }

  if (encodeMeshPacket(response.buffer,
                       response.index,
                       encodedResponseBuffer,
                       sizeof(encodedResponseBuffer),
                       encodedLength                 ) == Atams::ERROR_NONE)
  {
    Platform::transmitBuffer(commsChannel, encodedResponseBuffer, encodedLength);
  }
}

static void processUniversalRead(const uint16_t varID)
{
  switch (varID)
  {
    case Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE:
      write(Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(Atams::ATAMS_FALSE));
      break;
    default:
      /* Do Nothing */
      break;
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex. */
static void processDatagramRead(ChannelResponse_t &response,
                                DatagramHeader_t   datagramHeader,
                                uint16_t          &requestPacketDatagramIndex,
                                const uint8_t      payloadLength)
{
  Atams::Error_t transferStatus = externalTransfer(ACCESS_READ,
                                                   datagramHeader.varID,
                                                   &response.buffer[response.index + DATAGRAM_SIZE_HEADER],
                                                   payloadLength);

  if (transferStatus != Atams::ERROR_NONE)
  {
    abortResponse(response, transferStatus);
  }
  else
  {
    if (datagramHeader.blockID == BLOCK_ID_UNIVERSAL) processUniversalRead(datagramHeader.varID);
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    requestPacketDatagramIndex += DATAGRAM_SIZE_HEADER;
  }
}

static bool accessAllowedWithoutConfig(const uint16_t varID)
{
  return ((varID == Atams::UNIVERSAL_VAR_ID_CONFIGURATION_PASSKEY) ||
          (varID == Atams::UNIVERSAL_VAR_ID_WATCHDOG_RESET       ) );
}

static bool checkUniversalAccess(const uint16_t varID)
{
  bool accessPermitted = false;

  if ((accessAllowedWithoutConfig(varID)              ) ||
      (s_ConfigurationHandler.getConfigurationActive()) )
  {
    accessPermitted = true;
  }

  return (accessPermitted);
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex. */
static void processDatagramWrite(ChannelResponse_t &response,
                                 DatagramHeader_t   datagramHeader,
                                 uint16_t          &requestPacketDatagramIndex,
                                 uint8_t * const    datagramPayload,
                                 const uint8_t      payloadLength)
{
  bool transferAllowed = true;

  if (datagramHeader.blockID == Atams::BLOCK_ID_UNIVERSAL)
  {
    transferAllowed = checkUniversalAccess(datagramHeader.varID);
  }

  if (transferAllowed)
  {
    Atams::Error_t transferStatus = externalTransfer(ACCESS_WRITE,
                                                     datagramHeader.varID,
                                                     datagramPayload,
                                                     payloadLength);

    if (transferStatus == Atams::ERROR_NONE)
    {
      datagramHeader.command      = RESPONSE_ACK_WRITE;
      response.index             += DATAGRAM_SIZE_HEADER;
      requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
      datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
      if (datagramHeader.blockID == Atams::BLOCK_ID_UNIVERSAL) s_ConfigurationHandler.setUpdateRequired();
    }
    else
    {
      abortResponse(response, transferStatus);
    }
  }
  else
  {
    datagramHeader.command      = Atams::RESPONSE_NACK;
    response.index             += Atams::DATAGRAM_SIZE_HEADER;
    requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
  }
}

static void processRequestPacket(ChannelResponse_t &response,
                                 uint8_t * const    meshPacket,
                                 const uint16_t     meshPacketLength,
                                 const bool         universalBroadcast)
{
  uint16_t datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  response.index = MESH_SIZE_HEADER;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= meshPacketLength) &&
         (response.aborted                          == false           ) )
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&meshPacket[datagramStartIndex], datagramHeader);

    if ((universalBroadcast                          ) &&
        (datagramHeader.blockID != BLOCK_ID_UNIVERSAL) )
    {
      abortResponse(response, Atams::ERROR_BLOCK_ID);
      break;
    }

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.varID);

    if (varLength.status != Atams::ERROR_NONE)
    {
      abortResponse(response, varLength.status);
      break;
    }

    const uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + varLength.data;
    const uint16_t remainingOutputLength = sizeof(response.buffer) - response.index;
    const uint16_t remainingInputLength  = meshPacketLength - datagramStartIndex;

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ:
        if (datagramLength > remainingOutputLength) abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH);
        else processDatagramRead(response,
                                 datagramHeader,
                                 datagramStartIndex,
                                 varLength.data);
        break;

      case ACCESS_WRITE:
      {
        if      (datagramLength       > remainingInputLength)  abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH);
        else if (DATAGRAM_SIZE_HEADER > remainingOutputLength) abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH);
        else processDatagramWrite(response,
                                  datagramHeader,
                                  datagramStartIndex,
                                  &meshPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                                  varLength.data);
        break;
      }

      default:
        abortResponse(response, Atams::ERROR_ACCESS_INVALID);
        return;
    }
  }

  if (response.aborted == false)
  {
    if (datagramStartIndex != meshPacketLength)
    {
      abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH);
    }
    else
    {
      s_watchdogHandler.toggleWatchdog();
      s_nodeCommsState = NODE_COMMS_COMMS_ACTIVE;
    }
  }
}

static void processEncodedMeshPacket(const Platform::CommsChannel_t commsChannel,
                                     const uint8_t                * const packetBufferPtr,
                                     const uint16_t                 packetLength)
{
  static uint8_t             decodedPacket[MAX_MESH_PACKET_SIZE];
  static uint16_t            decodedLength = 0U;
  static ChannelSyncPacket_t commsChannelSyncPackets[Platform::NUMBER_OF_COMMS_CHANNELS];
  static ChannelResponse_t   commsChannelResponses[Platform::NUMBER_OF_COMMS_CHANNELS];

  if (decodeMeshPacket(packetBufferPtr,
                       packetLength,
                       &decodedPacket[0U],
                       sizeof(decodedPacket),
                       decodedLength         ) == Atams::ERROR_NONE)
  {
    MessageType_t        messageType     = static_cast<MessageType_t>(decodedPacket[MESH_INDEX_MSG_TYPE]);
    uint8_t              packetNodeID    = decodedPacket[MESH_INDEX_NODE_ID];
    uint8_t              packetSyncCount = decodedPacket[MESH_INDEX_SYNC];
    ChannelSyncPacket_t &syncPacket      = commsChannelSyncPackets[commsChannel];
    ChannelResponse_t   &response        = commsChannelResponses[commsChannel];
    uint8_t              localNodeID     = s_ConfigurationHandler.getLocalNodeID();
    uint8_t              firstSyncNodeID = s_ConfigurationHandler.getFirstSyncNodeID();
    uint8_t              finalSyncNodeID = s_ConfigurationHandler.getFinalSyncNodeID();
    uint8_t              prevSyncNodeID  = s_ConfigurationHandler.getPrevSyncNodeID();

    switch (messageType)
    {
      case MESSAGE_BROADCAST_UNIVERSAL:
        resetResponse(response);
        processRequestPacket(response, decodedPacket, decodedLength, true);
        sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE);
        break;
      case MESSAGE_REQUEST:
        if (packetNodeID == localNodeID)
        {
          resetResponse(response);
          processRequestPacket(response, decodedPacket, decodedLength, false);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == localNodeID)
        {
          resetResponse(response);
          syncPacket.syncCount = packetSyncCount;
          if (localNodeID == finalSyncNodeID)
          {
            processRequestPacket(response, decodedPacket, decodedLength, false);
          }
          else
          {
            memcpy(syncPacket.buffer, decodedPacket, decodedLength);
            syncPacket.length = decodedLength;
          }
          if (localNodeID == firstSyncNodeID)
          {
            sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length, false);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
        if (packetNodeID == prevSyncNodeID)
        {
          if (packetSyncCount != syncPacket.syncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT);
          else                                         sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_SYNC_JOG:
        if (packetNodeID == localNodeID)
        {
          if (packetSyncCount != syncPacket.syncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT);
          else                                         sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      default:
        /* Do Nothing */
        break;
    }
  }
  else
  {
    static uint32_t errorCount = 0U;
    errorCount++;
    Atams::write(Atams::UNIVERSAL_VAR_ID_CRC_ERROR_COUNT, errorCount);
  }
}

static void processRawMeshData(void)
{
  static uint8_t  meshPacketRXBuffer[MAX_MESH_PACKET_SIZE] = {0U};
  static uint16_t meshPacketRXLength                       = 0U;

  if (s_bufferResetRequired == true)
  {
    s_bufferResetRequired = false;
    for (CircularBuffer &circularBuffer : s_circularBuffers) circularBuffer.reset();
  }

  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
  {
    CircularBuffer::Error_t bufferStatus = s_circularBuffers[commsChannel].getPacket(meshPacketRXBuffer,
                                                                                   sizeof(meshPacketRXBuffer),
                                                                                   meshPacketRXLength);

    /* Early return if no packets ready */
    if (bufferStatus != CircularBuffer::ERROR_NONE)
    {
      if ((bufferStatus == CircularBuffer::ERROR_NO_EOL_BUFFER_FULL  ) ||
          (bufferStatus == CircularBuffer::ERROR_OUTPUT_BUFFER_LENGTH) )
      {
        /* Reset the buffer if no valid packet has been found or if
         * the valid packet is too long for the request packet buffer */
        s_circularBuffers[commsChannel].reset();
      }
    }
    else
    {
      /* Process the packet that has been copied into the request packet buffer */
      processEncodedMeshPacket(static_cast<Platform::CommsChannel_t>(commsChannel),
                               meshPacketRXBuffer,
                               meshPacketRXLength);
    }
  }
}

static bool validateUniversalVars(const MemoryMap_t &memoryMap)
{

}

static void resetVars(void)
{
  Platform::acquireVarStorageLock();

  for (Var_t &var : s_varStorage)
  {
    memset(var.storage, 0U, sizeof(var.storage));
  }

  Platform::releaseVarStorageLock();
}

static void invalidateMemoryMap(void)
{
  s_validVarCount = 0U;
  resetVars();
}

static bool getMemoryMapIsValid(void)
{
  return (s_memoryMap != nullptr);
}

static Atams::Error_t validateMemoryMap(const MemoryMap_t &memoryMap)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((memoryMap.noOfVars != sizeof(s_varStorage)     ) ||
      (memoryMap.noOfVars  > Atams::MAX_NUMBER_OF_VARS) )
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  if (validateUniversalVars(memoryMap) == false)
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  s_nodeCRC.beginRollingCRC();

  uint16_t varIndex = 0U;

  for (const VarInfo_t &varInfo : memoryMap.varInfoList)
  {
    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.accessLevel));
    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.NVMStorage));

    varIndex++;
  }

  if (memoryMap.genInfo.genChecksum != s_nodeCRC.getRollingCRC())
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }

  if (statusReturn == Atams::ERROR_NONE)
  {
    s_memoryMap = &memoryMap;
  }

  return (statusReturn);
}

static void waitForCoreInit(CoreID_t coreID)
{
  uint32_t                  previousCoreCheckTime = 0U;
  volatile CoreInitStatus_t coreInitStatus        = CORE_INIT_IN_PROGRESS;

  while (coreInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime = Platform::getMillis();

    if (currentTime - previousCoreCheckTime >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireVarStorageLock();

      coreInitStatus = s_coreInitComplete[coreID];

      Platform::releaseVarStorageLock();

      previousCoreCheckTime = currentTime;
    }
  }
}

static void signalCoreInitComplete(CoreID_t coreID)
{
  Platform::acquireVarStorageLock();
  s_coreInitComplete[coreID] = CORE_INIT_COMPLETE;
  Platform::releaseVarStorageLock();
}

static Atams::Error_t validateNVMChecksum(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  s_nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex = sizeof(NVMHeader_t); nvmIndex < nvmHeader.length; nvmIndex++)
  {
    uint8_t nvmByte;

    if (Platform::readFromNVM(nvmIndex, sizeof(nvmByte), &nvmByte) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    s_nodeCRC.updateRollingCRC(nvmByte);
  }

  if (nvmHeader.checksum != s_nodeCRC.getRollingCRC())
  {
    statusReturn = Atams::ERROR_NVM_CHECKSUM;
  }

  return (statusReturn);
}

static Atams::Error_t validateNVMGenInfo(const NVMHeader_t &nvmHeader)
{
  GenInfo_t      nvmGenInfo;
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (nvmHeader.length < (sizeof(NVMHeader_t) + sizeof(GenInfo_t)))
  {
    statusReturn = Atams::ERROR_NVM_HEADER_LENGTH;
  }
  else if (!Platform::readFromNVM(sizeof(NVMHeader_t), sizeof(nvmGenInfo), reinterpret_cast<uint8_t*>(&nvmGenInfo)))
  {
    statusReturn = Atams::ERROR_PLATFORM;
  }
  else if (nvmGenInfo != s_memoryMap->genInfo)
  {
    statusReturn = Atams::ERROR_NVM_GEN_INFO;
  }

  return (statusReturn);
}

static uint32_t getNVMSpaceRequirement(void)
{
  uint32_t requiredNVMSpace = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (const VarInfo_t &varInfo : s_memoryMap->varInfoList)
  {
    if (varInfo.NVMStorage) requiredNVMSpace += Atams::TYPE_LENGTHS[varInfo.type];
  }

  return (requiredNVMSpace);
}

static Atams::Error_t constructAndWriteNVMHeader(const uint32_t nvmSpaceUsed)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMHeader_t    nvmHeader;

  s_nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex = sizeof(NVMHeader_t); nvmIndex < nvmSpaceUsed; nvmIndex++)
  {
    uint8_t nvmByte;

    if (Platform::readFromNVM(nvmIndex, sizeof(nvmByte), &nvmByte) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    s_nodeCRC.updateRollingCRC(nvmByte);
  }

  nvmHeader.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmHeader.length     = nvmSpaceUsed;
  nvmHeader.checksum   = s_nodeCRC.getRollingCRC();

  if (Platform::writeToNVM(0U, sizeof(NVMHeader_t), reinterpret_cast<uint8_t*>(&nvmHeader)))
  {
    statusReturn = Atams::ERROR_PLATFORM;
  }

  return (statusReturn);
}

static Atams::Error_t nvmTransferVars(const uint32_t      maxIndex,
                                      uint32_t           &nvmIndex,
                                      const NVMTransfer_t transferType)
{
  uint16_t varID = 0U;

  for (const VarInfo_t &varInfo : s_memoryMap->varInfoList)
  {
    if (varInfo.NVMStorage)
    {
      uint8_t varLength = Atams::TYPE_LENGTHS[varInfo.type];

      if ((nvmIndex + varLength) > maxIndex)
      {
        return (Atams::ERROR_NVM_HEADER_LENGTH); /* Early Return */
      }

      switch (transferType)
      {
        case TRANSFER_LOAD:
          if (!Platform::readFromNVM(nvmIndex, varLength, s_varStorage[varID].storage))
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        case TRANSFER_SAVE:
          if (!Platform::writeToNVM(nvmIndex, varLength, s_varStorage[varID].storage))
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        default:
          /* Do Nothing */
          break;
      }

      nvmIndex += varLength;
    }

    varID++;
  }

  return (Atams::ERROR_NONE);
}

static Atams::Error_t loadNVMAllVars(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  nvmStatus = nvmTransferVars(nvmHeader.length, nvmIndex, TRANSFER_LOAD);

  return (nvmStatus);
}

static Atams::Error_t saveNVMAllVars(const uint32_t availableNVMSpace)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  nvmStatus = nvmTransferVars(availableNVMSpace, nvmIndex, TRANSFER_LOAD);

  return (nvmStatus);
}

static void initCommsBuffers(void)
{
  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
  {
    s_circularBuffers[commsChannel].setEOLChar(EOL_BYTE);
    s_circularBuffers[commsChannel].setLockArgument(static_cast<Platform::CommsChannel_t>(commsChannel));
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = initControlCore(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = initCommsCore(memoryMap);

  return (initStatus);
}

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap)
{
  waitForCoreInit(CORE_CONTROL);

  Atams::Error_t initStatus = validateMemoryMap(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = memoryMap.initAllDefaults();

  if (initStatus == Atams::ERROR_NONE) initStatus = memoryMap.initGenInfo();

  if (initStatus == Atams::ERROR_NONE)
  {
    Platform::setReceiveCallback(receiveCallback);

    initCommsBuffers();

    signalCoreInitComplete(CORE_COMMS);

    if (initStatus == Atams::ERROR_NONE) s_nodeCommsState = Atams::NODE_COMMS_INITIALISED;
  }
  else
  {
    invalidateMemoryMap();
  }

  return (initStatus);
}

Atams::Error_t initControlCore(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap);

  if (initStatus == Atams::ERROR_NONE)
  {
    signalCoreInitComplete(CORE_CONTROL);

    waitForCoreInit(CORE_COMMS);
  }
  else
  {
    invalidateMemoryMap();
  }

  return (initStatus);
}

Atams::Error_t initNVM(void)
{
  NVMHeader_t    nvmHeader;
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (getMemoryMapIsValid() == false)
  {
    return (Atams::ERROR_MEMORY_MAP);                /* Early Return */
  }
  if (sizeof(NVMHeader_t) > Platform::NVM_STORAGE_SIZE)
  {
    statusReturn = Atams::ERROR_NVM_PLATFORM_SIZE;   /* Early Return */
  }
  else if (!Platform::readFromNVM(0U, sizeof(nvmHeader), reinterpret_cast<uint8_t*>(&nvmHeader)))
  {
    statusReturn = Atams::ERROR_PLATFORM;            /* Early Return */
  }
  else if (nvmHeader.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)
  {
    statusReturn = Atams::ERROR_NVM_HEADER_VALIDITY; /* Early Return */
  }
  else if (nvmHeader.length > Platform::NVM_STORAGE_SIZE)
  {
    statusReturn = Atams::ERROR_NVM_HEADER_LENGTH;   /* Early Return */
  }

  if (statusReturn == Atams::ERROR_NONE) statusReturn = validateNVMChecksum(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = validateNVMGenInfo(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = loadNVMAllVars(nvmHeader);

  static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_STATUS,           statusReturn));
  static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE, ATAMS_TRUE));

  if (statusReturn == Atams::ERROR_NONE)
  {
    resetVars();
    s_memoryMap->initAllDefaults();
  }

  /* Messages may have been received while storage was in progress -
   * request buffer reset to clear old data */
  s_bufferResetRequired = true;

  return (statusReturn);
}

Atams::Error_t restoreAll(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_MEMORY_MAP;

  if (getMemoryMapIsValid()) statusReturn = s_memoryMap->initAllDefaults();

  if (statusReturn == Atams::ERROR_NONE)
  {
    statusReturn = storeAll();
  }
  else
  {
    static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_STATUS,           statusReturn));
    static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE, ATAMS_TRUE));
  }

  return (statusReturn);
}

Atams::Error_t restoreUserVars(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_MEMORY_MAP;

  if (getMemoryMapIsValid()) statusReturn = s_memoryMap->initUserDefaults();

  if (statusReturn == Atams::ERROR_NONE)
  {
    statusReturn = storeAll();
  }
  else
  {
    static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_STATUS,           statusReturn));
    static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE, ATAMS_TRUE));
  }

  return (statusReturn);
}

Atams::Error_t storeAll(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMHeader_t    invalidNVMHeader;

  if (getMemoryMapIsValid() == false)
  {
    return (Atams::ERROR_MEMORY_MAP);        /* Early Return */
  }

  uint32_t requiredNVMSpace = getNVMSpaceRequirement();

  if (requiredNVMSpace > Platform::NVM_STORAGE_SIZE)
  {
    return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return */
  }

  /* Write invalid header to invalidate NVM */
  if (!Platform::writeToNVM(0U, sizeof(Atams::NVMHeader_t), reinterpret_cast<uint8_t*>(&invalidNVMHeader)))
  {
    return (Atams::ERROR_PLATFORM);          /* Early Return */
  }

  /* Write genInfo to NVM after header */
  if (!Platform::writeToNVM(sizeof(Atams::NVMHeader_t), sizeof(Atams::GenInfo_t), reinterpret_cast<const uint8_t*>(&s_memoryMap->genInfo)))
  {
    return (Atams::ERROR_PLATFORM);          /* Early Return */
  }

  statusReturn = saveNVMAllVars(requiredNVMSpace);

  /* Write valid header to validate NVM */
  if (statusReturn == Atams::ERROR_NONE) statusReturn = constructAndWriteNVMHeader(requiredNVMSpace);

  static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_STATUS,           static_cast<uint8_t>(statusReturn)));
  static_cast<void>(Atams::write(Atams::UNIVERSAL_VAR_ID_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(ATAMS_TRUE)));

  /* Messages may have been received while storage was in progress -
   * request buffer reset to clear old data */
  s_bufferResetRequired = true;

  return (statusReturn);
}

void updateCommsPolling(void)
{
  uint32_t currentTime = Platform::getMillis();

  switch (s_nodeCommsState)
  {
    case Atams::NODE_COMMS_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_COMMS_INITIALISED:
      Platform::update();
      processRawMeshData();
      break;
    case Atams::NODE_COMMS_COMMS_ACTIVE:
      Platform::update();
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      break;
    default:
      /* Do Nothing */
      break;
  }

  s_ConfigurationHandler.update();
}

void updateCommsBlocking(void)
{
  uint32_t currentTime = Platform::getMillis();

  switch (s_nodeCommsState)
  {
    case Atams::NODE_COMMS_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_COMMS_INITIALISED:
      Platform::waitOnCommsBufferSemaphore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      break;
    case Atams::NODE_COMMS_COMMS_ACTIVE:
      Platform::waitOnCommsBufferSemaphore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      break;
    default:
      /* Do Nothing */
      break;
  }

  s_ConfigurationHandler.update();
}

template <typename T>
Atams::Error_t write(const uint16_t varID,
                     const T        writeData)
{
  if (getMemoryMapIsValid() == false) return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  if (varID >= s_memoryMap->noOfVars) return (Atams:: ERROR_VAR_ID);    /* Early Return */

  const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (PLATFORM_TYPE_NAMES[varInfo.type] != typeid(T).name()) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Var_t &var = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  memcpy(var.storage, &writeData, sizeof(writeData));

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t write<uint8_t >(const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t write<int8_t  >(const uint16_t varID, const int8_t   writeData);
template Atams::Error_t write<uint16_t>(const uint16_t varID, const uint16_t writeData);
template Atams::Error_t write<int16_t >(const uint16_t varID, const int16_t  writeData);
template Atams::Error_t write<uint32_t>(const uint16_t varID, const uint32_t writeData);
template Atams::Error_t write<int32_t >(const uint16_t varID, const int32_t  writeData);
template Atams::Error_t write<float   >(const uint16_t varID, const float    writeData);

template <typename T>
Atams::Error_t read(const uint16_t  varID,
                          T        &readData)
{
  if (getMemoryMapIsValid() == false) return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  if (varID >= s_memoryMap->noOfVars) return (Atams:: ERROR_VAR_ID);    /* Early Return */

  const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (PLATFORM_TYPE_NAMES[varInfo.type] != typeid(T).name()) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Var_t &var = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  memcpy(&readData, var.storage, sizeof(readData));

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t read<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t read<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t read<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t read<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t read<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t read<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t read<float   >(const uint16_t varID, float    &readData);

Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                const uint16_t  varID,
                                uint8_t * const bytesPtr,
                                const uint8_t   length)
{
  if (getMemoryMapIsValid() == false) return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  if (varID >= s_memoryMap->noOfVars) return (Atams:: ERROR_VAR_ID);    /* Early Return */

  const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length)              return (Atams::ERROR_VAR_LENGTH);     /* Early Return */
  if (bytesPtr                   == nullptr)             return (Atams::ERROR_NULL_PTR);       /* Early Return */
  if (accessRequest              >  varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Var_t         &var         = s_varStorage[varID];
  Atams::Error_t accessError = Atams::ERROR_NONE;

  Platform::acquireVarStorageLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(bytesPtr, var.storage, TYPE_LENGTHS[varInfo.type]);
      if (systemIsBigEndian()) swapEndiannessRaw(bytesPtr, TYPE_LENGTHS[varInfo.type]);
      break;

    case ACCESS_WRITE:
      if (systemIsBigEndian()) swapEndiannessRaw(bytesPtr, TYPE_LENGTHS[varInfo.type]);
      memcpy(var.storage, bytesPtr, TYPE_LENGTHS[varInfo.type]);
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  Platform::releaseVarStorageLock();

  return (accessError);
}

DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (getMemoryMapIsValid() == false)
  {
    lengthReturn.status = Atams::ERROR_MEMORY_MAP;
    return (lengthReturn); /* Early Return */
  }

  if (varID >= s_validVarCount)
  {
    lengthReturn.status = Atams::ERROR_VAR_ID;
    return (lengthReturn); /* Early Return */
  }

  const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  lengthReturn.data   = TYPE_LENGTHS[varInfo.type];
  lengthReturn.status = ERROR_NONE;

  return (lengthReturn);
}


} /* End Namespace - Atams */

/**
  * @}End of File
  */
