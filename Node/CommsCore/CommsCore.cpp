/**
  ******************************************************************************
  * @file    CommsCore.cpp
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

#include <Atams/Node/Developer/UniversalBlockManager.hpp>
#include "CommsCore.hpp"

#include "string.h"

#include "CommsPlatform.hpp"
#include "../Developer/FramingConstants.hpp"
#include "../Developer/NodeUtilities.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../../Shared/Utilities/AtamsUtilities.hpp"
#include "../../Shared/Utilities/CRC32.hpp"
#include "../Developer/NodeTypedefs.hpp"
#include "../Developer/CircularBuffer.hpp"
#include "../Developer/WatchdogHandler.hpp"
#include "../Developer/NVMUnitHandler.hpp"

#include "main.h"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static bool     postStore {false};
static uint32_t receiveCallbackCount {0U};
static uint32_t receiveByteCount[100];
static uint32_t processPacketCountPreDecode {0U};
static uint32_t processPacketCountPostDecode{0U};
static uint32_t processPreDecodeByteCount[100];
static uint32_t processPostDecodeByteCount[100];

static uint8_t packets[10][1000];

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

enum NodeCommsState_t: uint8_t
{
  NODE_STATE_UNINITIALISED = 0U,
  NODE_STATE_INITIALISED   = 1U,
  NODE_STATE_ACTIVE        = 2U,
  NODE_STATE_PROCESS_PENDING     = 3U
};

enum NVMTransfer_t : uint8_t
{
  TRANSFER_LOAD = 0U,
  TRANSFER_SAVE = 1U
};

struct ChannelResponse_t
{
  uint8_t              buffer[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint16_t             index            {0U};
  bool                 aborted          {false};
  Atams::MessageType_t abortMessageType {Atams::MESSAGE_ABORT_RESPONSE};
  uint8_t              activeSyncCount  {0U};
  uint8_t              encodedBuffer[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t             encodedLength    {0U};

};

struct ChannelSyncPacket_t
{
  uint8_t  buffer[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint16_t length {0U};
};

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static const MemoryMap_t   *s_memoryMapPtr;
static CRC32                s_nodeCRC(Atams::CRC32_POLYNOMIAL);
static CircularBuffer       s_circularBuffers[Platform::NUMBER_OF_COMMS_PERIPHERALS];

static WatchdogHandler      s_watchdogHandler;
static UniversalBlockManager s_uniBlockManager(s_watchdogHandler);
static NVMUnitHandler       s_nvmUnitHandler(s_nodeCRC);

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t         s_validVarCount       {0U};
static NodeCommsState_t s_nodeCommsState      {NODE_STATE_UNINITIALISED};
static bool             s_appCoreInitRequired {true};

/* Core Init Synchronisation */
//ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static CoreInitStatus_t s_coreInitComplete[Atams::NUMBER_OF_CORES] {CORE_INIT_IN_PROGRESS, CORE_INIT_IN_PROGRESS};

//ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static Atams::VarStorage_t s_varStorage[Platform::NODE_NUMBER_OF_VARS];

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void resetVars(void)
{
  Platform::acquireVarStorageLock();

  for (VarStorage_t &varStorage : s_varStorage)
  {
    memset(varStorage, 0U, sizeof(varStorage));
  }

  Platform::releaseVarStorageLock();
}

static void invalidateMemoryMap(void)
{
  s_validVarCount = 0U;
  s_memoryMapPtr  = nullptr;
  resetVars();
}

static Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                const uint16_t  varID,
                                uint8_t * const bytesPtr,
                                const uint8_t   length)
{
  if (varID >= s_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length)                 return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (bytesPtr                   == nullptr)                return (Atams::ERROR_NULLPTR);        /* Early Return */
  if (accessRequest              >  varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Atams::VarStorage_t &varStorage = s_varStorage[varID];
  Atams::Error_t      accessError = Atams::ERROR_NONE;

  Platform::acquireVarStorageLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(bytesPtr, varStorage, TYPE_LENGTHS[varInfo.type]);
      break;

    case ACCESS_WRITE:
      memcpy(varStorage, bytesPtr, TYPE_LENGTHS[varInfo.type]);
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  Platform::releaseVarStorageLock();

  return (accessError);
}

static DataStatusReturn_t<uint8_t> getVarLength(const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (varID >= s_validVarCount)
  {
    lengthReturn.status = Atams::ERROR_VAR_ID;
    return (lengthReturn); /* Early Return */
  }

  const VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  lengthReturn.data   = TYPE_LENGTHS[varInfo.type];
  lengthReturn.status = ERROR_NONE;

  return (lengthReturn);
}

static void receiveCallback(const Platform::CommsPeripheralID_t commsChannel,
                                  uint8_t                      *rxBufferPtr,
                            const uint16_t                      rxBufferLength)
{
  if (postStore)
  {
    receiveCallbackCount++;

    if (receiveCallbackCount < 100U)
    {
      receiveByteCount[receiveCallbackCount - 1U] = rxBufferLength;
    }

    if (receiveCallbackCount < 10)
    {
      if (rxBufferLength <= 1000) memcpy(packets[receiveCallbackCount - 1U], rxBufferPtr, rxBufferLength);
    }
  }

  if ((commsChannel   < Platform::NUMBER_OF_COMMS_PERIPHERALS) &&
      (rxBufferLength > 0U                                   ) )
  {
    s_circularBuffers[commsChannel].pushHead(rxBufferPtr, rxBufferLength);
    Platform::signalCommsBufferSemaphore();
  }
}

static void resetResponse(ChannelResponse_t &response, const uint8_t syncCount)
{
  response.activeSyncCount               = syncCount;
  response.aborted                       = false;
  response.buffer[HEADER_INDEX_MSG_TYPE] = MESSAGE_UNKNOWN;
  response.index                         = HEADER_INDEX_FIRST_DATAGRAM;
}

static void abortResponse(ChannelResponse_t &response, const Atams::Error_t error, const uint16_t varID)
{
  response.buffer[HEADER_INDEX_NODE_ID]  = s_uniBlockManager.getLocalNodeID();
  response.buffer[HEADER_INDEX_MSG_TYPE] = response.abortMessageType;
  response.buffer[ABORT_INDEX_ERROR]     = error;
  response.buffer[ABORT_INDEX_VAR_ID_HI] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_HI) & ABORT_MASK_VAR_ID_HI);
  response.buffer[ABORT_INDEX_VAR_ID_LO] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_LO) & ABORT_MASK_VAR_ID_LO);
  response.index                         = ABORT_SIZE_PACKET;
  response.aborted                       = true;
}

static void sendResponsePacket(Platform::CommsPeripheralID_t commsChannel,
                               ChannelResponse_t            &response,
                               Atams::MessageType_t          messageType)
{
  if (response.aborted == false)
  {
    response.buffer[HEADER_INDEX_MSG_TYPE] = messageType;
  }

  response.buffer[HEADER_INDEX_NODE_ID] = s_uniBlockManager.getLocalNodeID();
  response.buffer[HEADER_INDEX_SYNC]    = response.activeSyncCount;

  if (encodeBusPacket(response.buffer,
                      response.index,
                      response.encodedBuffer,
                      sizeof(response.encodedBuffer),
                      response.encodedLength        ) == Atams::ERROR_NONE)
  {
    Platform::transmitBuffer(commsChannel, response.encodedBuffer, response.encodedLength);
  }
}

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
    abortResponse(response, transferStatus, datagramHeader.varID);
  }
  else
  {
    if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS) s_uniBlockManager.processRead(datagramHeader.varID);
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    requestPacketDatagramIndex += DATAGRAM_SIZE_HEADER;
  }
}

static void processDatagramWrite(ChannelResponse_t &response,
                                 DatagramHeader_t   datagramHeader,
                                 uint16_t          &requestPacketDatagramIndex,
                                 uint8_t * const    datagramPayload,
                                 const uint8_t      payloadLength)
{
  if ((datagramHeader.varID >= BlockUniversal::NUMBER_OF_VARS          ) ||
      (s_uniBlockManager.checkWriteAccess(datagramHeader.varID) == true) )
  {
    Atams::Error_t transferStatus = externalTransfer(ACCESS_WRITE,
                                                     datagramHeader.varID,
                                                     datagramPayload,
                                                     payloadLength);

    if (transferStatus == Atams::ERROR_NONE)
    {
      datagramHeader.command      = RESPONSE_ACK_WRITE;
      datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
      response.index             += DATAGRAM_SIZE_HEADER;
      requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
      if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
      {
        s_uniBlockManager.setUpdateRequired();
      }
    }
    else
    {
      abortResponse(response, transferStatus, datagramHeader.varID);
    }
  }
  else
  {
    datagramHeader.command      = Atams::RESPONSE_NACK;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index             += Atams::DATAGRAM_SIZE_HEADER;
    requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
  }
}

static void validateRequestPacket(ChannelResponse_t &response,
                                  uint8_t * const    requestPacket,
                                  const uint16_t     requestPacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex     = HEADER_INDEX_FIRST_DATAGRAM;
  uint16_t         requiredResponseLength = HEADER_SIZE_HEADER;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacketLength) &&
         (response.aborted                          == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    DataStatusReturn_t<uint8_t> varLength = getVarLength(datagramHeader.varID);

    if (varLength.status != Atams::ERROR_NONE)
    {
      abortResponse(response, varLength.status, datagramHeader.varID);
    }
    else
    {
      switch (static_cast<Access_t>(datagramHeader.command))
      {
        case Atams::ACCESS_READ:
          datagramStartIndex     += DATAGRAM_SIZE_HEADER;
          requiredResponseLength += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength.data);
          break;

        case Atams::ACCESS_WRITE:
          datagramStartIndex     += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength.data);
          requiredResponseLength += DATAGRAM_SIZE_HEADER;
          break;

        default:
          abortResponse(response, Atams::ERROR_ACCESS_INVALID, datagramHeader.varID);
          break;
      }
    }
  }

  if (response.aborted == false)
  {
    if (datagramStartIndex     != requestPacketLength    ) abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH,  Atams::VAR_ID_NULL);
    if (requiredResponseLength  > sizeof(response.buffer)) abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH, Atams::VAR_ID_NULL);
  }
}

static void processRequestPacket(ChannelResponse_t &response,
                                 uint8_t * const    requestPacket,
                                 const uint16_t     requestPacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex = HEADER_INDEX_FIRST_DATAGRAM;

  resetResponse(response, requestPacket[HEADER_INDEX_SYNC]);

  validateRequestPacket(response, requestPacket, requestPacketLength);

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacketLength) &&
         (response.aborted                          == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateRequestPacket */
    uint8_t varLength = TYPE_LENGTHS[s_memoryMapPtr->varInfoList[datagramHeader.varID].type];

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ:
        processDatagramRead(response, datagramHeader, datagramStartIndex, varLength);
        break;

      case ACCESS_WRITE:
        processDatagramWrite(response,
                             datagramHeader,
                             datagramStartIndex,
                             &requestPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                             varLength);
        break;

      default:
        abortResponse(response, Atams::ERROR_ACCESS_INVALID, datagramHeader.varID);
        return;
    }
  }

  if (response.aborted == false)
  {
    s_watchdogHandler.toggleWatchdog();
    s_nodeCommsState = NODE_STATE_ACTIVE;
  }
}

static inline void copyRequestToSyncPacket(ChannelSyncPacket_t  &syncPacket,
                                           const uint8_t * const requestBuffer,
                                           const uint16_t        requestLength)
{
  memcpy(syncPacket.buffer, requestBuffer, requestLength);
  syncPacket.length = requestLength;
}

static inline void setAbortMessageType(ChannelResponse_t &response, const MessageType_t messageType)
{
  switch (messageType)
  {
    case MESSAGE_REQUEST_SYNCED:
    case MESSAGE_RESPONSE_SYNCED:
    case MESSAGE_ABORT_RESPONSE_SYNCED:
    case MESSAGE_SYNC_JOG:
      response.abortMessageType = MESSAGE_ABORT_RESPONSE_SYNCED;
      break;
    default:
      response.abortMessageType = MESSAGE_ABORT_RESPONSE;
      break;
  }
}

static void processEncodedMeshPacket(const Platform::CommsPeripheralID_t commsChannel,
                                     const uint8_t * const               packetBuffer,
                                     const uint16_t                      packetLength)
{
  static uint8_t             decodedPacket[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  static uint16_t            decodedLength = 0U;
  static ChannelSyncPacket_t commsChannelSyncPackets[Platform::NUMBER_OF_COMMS_PERIPHERALS];
  static ChannelResponse_t   commsChannelResponses[Platform::NUMBER_OF_COMMS_PERIPHERALS];

  if (decodeBusPacket(packetBuffer,
                      packetLength,
                      &decodedPacket[0U],
                      sizeof(decodedPacket),
                      decodedLength         ) == Atams::ERROR_NONE)
  {
    MessageType_t        messageType     = static_cast<MessageType_t>(decodedPacket[HEADER_INDEX_MSG_TYPE]);
    uint8_t              packetNodeID    = decodedPacket[HEADER_INDEX_NODE_ID];
    ChannelSyncPacket_t &syncPacket      = commsChannelSyncPackets[commsChannel];
    ChannelResponse_t   &response        = commsChannelResponses[commsChannel];
    uint8_t              localNodeID     = s_uniBlockManager.getLocalNodeID();
    uint8_t              firstSyncNodeID = s_uniBlockManager.getFirstSyncNodeID();
    uint8_t              finalSyncNodeID = s_uniBlockManager.getFinalSyncNodeID();
    uint8_t              prevSyncNodeID  = s_uniBlockManager.getPrevSyncNodeID();
    uint8_t              packetSyncCount = decodedPacket[HEADER_INDEX_SYNC];

    if (postStore)
    {
      processPacketCountPostDecode++;
      if (processPacketCountPostDecode < 100U)
      {
        processPostDecodeByteCount[processPacketCountPostDecode - 1U] = decodedLength;
      }
    }

    setAbortMessageType(response, messageType);

    switch (messageType)
    {
      case MESSAGE_REQUEST:
        if (packetNodeID == localNodeID)
        {
          processRequestPacket(response, decodedPacket, decodedLength);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == localNodeID)
        {
          if (localNodeID == finalSyncNodeID)
          {
            processRequestPacket(response, decodedPacket, decodedLength);
            if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
          }
          else
          {
            copyRequestToSyncPacket(syncPacket, decodedPacket, decodedLength);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length);
          if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
      case MESSAGE_ABORT_RESPONSE_SYNCED:
        if ((packetNodeID == prevSyncNodeID) &&
            (packetNodeID != localNodeID   ) )
        {
          if (packetSyncCount != response.activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_SYNC_JOG:
        if (packetNodeID == localNodeID)
        {
          if (packetSyncCount != response.activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
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
    Atams::write(BlockUniversal::VAR_CRC_ERROR_COUNT, errorCount);
  }
}

static void processRawMeshData(void)
{
  static uint8_t  meshPacketRXBuffer[Platform::MAX_BUS_PACKET_SIZE];
  static uint16_t meshPacketRXLength {0U};

  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_PERIPHERALS; commsChannel++)
  {
    CircularBuffer::Error_t bufferStatus = s_circularBuffers[commsChannel].getPacket(meshPacketRXBuffer,
                                                                                     sizeof(meshPacketRXBuffer),
                                                                                     meshPacketRXLength);

    /* Early return if no packets ready */
    if (bufferStatus == CircularBuffer::ERROR_NONE)
    {
      if (postStore)
      {
        processPacketCountPreDecode++;
        if (processPacketCountPreDecode < 100U)
        {
          processPreDecodeByteCount[processPacketCountPreDecode - 1U] = meshPacketRXLength;
        }
      }

      /* Process the packet that has been copied into the request packet buffer */
      processEncodedMeshPacket(static_cast<Platform::CommsPeripheralID_t>(commsChannel),
                               meshPacketRXBuffer,
                               meshPacketRXLength);
    }
    else if (bufferStatus == CircularBuffer::ERROR_NO_EOL_BUFFER_FULL)
    {
      s_circularBuffers[commsChannel].reset();
    }
  }
}

static Atams::Error_t getMemoryMapIsValid(void)
{
  if ((s_memoryMapPtr != nullptr) &&
      (s_validVarCount > 0U     ) )
  {
    return (Atams::ERROR_NONE);
  }
  else
  {
    return (Atams::ERROR_MEMORY_MAP);
  }
}

static void waitForControlCoreInit(void)
{
  uint32_t                  previousCoreCheckTime = 0U;
  volatile CoreInitStatus_t coreInitStatus        = CORE_INIT_IN_PROGRESS;

  Platform::acquireVarStorageLock();
  s_coreInitComplete[CORE_APP] = CORE_INIT_IN_PROGRESS;
  Platform::releaseVarStorageLock();

  while (coreInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime = Platform::getMillis();

    if (currentTime - previousCoreCheckTime >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireVarStorageLock();

      coreInitStatus = s_coreInitComplete[CORE_APP];

      Platform::releaseVarStorageLock();

      previousCoreCheckTime = currentTime;
    }
  }
}

static void signalCommsCoreInitComplete(void)
{
  Platform::acquireVarStorageLock();
  s_coreInitComplete[CORE_COMMS] = CORE_INIT_COMPLETE;
  Platform::releaseVarStorageLock();
}

static Atams::Error_t validateNVMChecksum(const NVMHeader_t &nvmHeader, const NVMFooter_t &nvmFooter)
{
  Atams::Error_t error           = Atams::ERROR_NONE;
  uint32_t       endOfVarStorage = nvmHeader.length - sizeof(nvmFooter);

  s_nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex = 0U; nvmIndex < endOfVarStorage; nvmIndex++)
  {
    uint8_t nvmByte;

    if (s_nvmUnitHandler.readFromNVM(nvmIndex, &nvmByte, 1U) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    s_nodeCRC.updateRollingCRC(nvmByte);
  }

  const uint32_t nvmChecksum = s_nodeCRC.getRollingCRC();

  if (nvmFooter.checksum != nvmChecksum)
  {
    error = Atams::ERROR_NVM_CHECKSUM;
  }

  return (error);
}

static uint32_t getNVMVarSpaceRequirement(void)
{
  uint32_t requiredVarSpace = 0U;

  for (uint16_t varID = 0U; varID < s_memoryMapPtr->noOfVars; varID++)
  {
    const VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

    if (varInfo.NVMStorage) requiredVarSpace += Atams::TYPE_LENGTHS[varInfo.type];
  }

  return (requiredVarSpace);
}

static Atams::Error_t constructAndStoreFooter(const uint32_t nvmSpaceUsed)
{
  NVMFooter_t nvmFooter;

  nvmFooter.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmFooter.checksum   = s_nodeCRC.getRollingCRC();

  return (s_nvmUnitHandler.writeToNVM(nvmSpaceUsed, reinterpret_cast<uint8_t*>(&nvmFooter), sizeof(NVMFooter_t)));
}

static Atams::Error_t nvmTransferVars(const uint32_t      maxIndex,
                                      uint32_t           &nvmIndex,
                                      const NVMTransfer_t transferType,
                                      const bool          universalBlockOnly)
{
  uint16_t numberOfVarsToTransfer = universalBlockOnly == true                            ?
                                    static_cast<uint16_t>(BlockUniversal::NUMBER_OF_VARS) :
                                    s_memoryMapPtr->noOfVars;

  for (uint16_t varID = 0U; varID < numberOfVarsToTransfer; varID++)
  {
    const VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

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
          if (s_nvmUnitHandler.readFromNVM(nvmIndex, s_varStorage[varID], varLength) != Atams::ERROR_NONE)
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        case TRANSFER_SAVE:
          if (s_nvmUnitHandler.writeToNVM(nvmIndex, s_varStorage[varID], varLength) != Atams::ERROR_NONE)
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
  }

  return (Atams::ERROR_NONE);
}

static Atams::Error_t loadNVMAllBlocks(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t);

  nvmStatus = nvmTransferVars(nvmHeader.length, nvmIndex, TRANSFER_LOAD, false);

  return (nvmStatus);
}

static Atams::Error_t loadNVMUniversalBlock(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t);

  nvmStatus = nvmTransferVars(nvmHeader.length, nvmIndex, TRANSFER_LOAD, true);

  return (nvmStatus);
}

static Atams::Error_t saveVarsToNVM(const uint32_t availableNVMSpace)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t);

  nvmStatus = nvmTransferVars(availableNVMSpace, nvmIndex, TRANSFER_SAVE, false);

  return (nvmStatus);
}

static void initCommsBuffers(void)
{
  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_PERIPHERALS; commsChannel++)
  {
    s_circularBuffers[commsChannel].setEOLChar(Atams::EOL_BYTE);
    s_circularBuffers[commsChannel].setLockArgument(static_cast<Platform::CommsPeripheralID_t>(commsChannel));
  }
}

static Atams::Error_t initUniversalDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockUniversal::VAR_NODE_ID,          BlockUniversal::DEFAULT_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_FIRST_NODE_ID,    BlockUniversal::DEFAULT_FIRST_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_LAST_NODE_ID,     BlockUniversal::DEFAULT_LAST_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_PREVIOUS_NODE_ID, BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_BITRATE,          BlockUniversal::DEFAULT_BITRATE);
  if (!error) error = Atams::write(BlockUniversal::VAR_WATCHDOG_PERIOD,  BlockUniversal::DEFAULT_WATCHDOG_PERIOD);

  return (error);
}

static Atams::Error_t initAllDefaults(void)
{
  Atams::Error_t error = getMemoryMapIsValid();
  if (!error)    error = initUniversalDefaults();
  if (!error)    error = s_memoryMapPtr->initUserDefaults();

  return (error);
}

static Atams::Error_t storeNVMHeader(const NVMHeader_t &nvmHeader)
{
  return (s_nvmUnitHandler.writeToNVM(0U, reinterpret_cast<const uint8_t*>(&nvmHeader), sizeof(nvmHeader)));
}

static Atams::Error_t extractNVMHeader(NVMHeader_t &nvmHeader)
{
  return (s_nvmUnitHandler.readFromNVM(0U, reinterpret_cast<uint8_t*>(&nvmHeader), sizeof(nvmHeader)));
}

static Atams::Error_t extractNVMFooter(const NVMHeader_t &nvmHeader, NVMFooter_t &nvmFooter)
{
  uint32_t nvmFooterIndex = nvmHeader.length - sizeof(nvmFooter);

  return (s_nvmUnitHandler.readFromNVM(nvmFooterIndex, reinterpret_cast<uint8_t*>(&nvmFooter), sizeof(nvmFooter)));
}

static Atams::Error_t validateNVMHeaderFooter(NVMHeader_t &nvmHeader, NVMFooter_t &nvmFooter)
{
  if (Platform::NVM_STORAGE_SIZE < sizeof(NVMHeader_t))            return (Atams::ERROR_NVM_PLATFORM_SIZE);   /* Early Return */

  if (extractNVMHeader(nvmHeader) != Atams::ERROR_NONE)            return (Atams::ERROR_PLATFORM);            /* Early Return */

  if (nvmHeader.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)  return (Atams::ERROR_NVM_HEADER_VALIDITY); /* Early Return */

  if (Platform::NVM_STORAGE_SIZE < nvmHeader.length)               return (Atams::ERROR_NVM_HEADER_LENGTH);   /* Early Return */

  if (extractNVMFooter(nvmHeader, nvmFooter) != Atams::ERROR_NONE) return (Atams::ERROR_PLATFORM);            /* Early Return */

  if (nvmFooter.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)  return (Atams::ERROR_NVM_HEADER_VALIDITY); /* Early Return */

  return (validateNVMChecksum(nvmHeader, nvmFooter));
}

static Atams::Error_t validateNVMGenInfo(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t   error        = Atams::ERROR_NONE;
  Atams::GenInfo_t nvmGenInfo   = nvmHeader.genInfo;
  Atams::GenInfo_t mapGenInfo   = s_memoryMapPtr->genInfo;

  if ((nvmGenInfo.atamsVersionMajor != mapGenInfo.atamsVersionMajor) &&
      (nvmGenInfo.atamsVersionMinor != mapGenInfo.atamsVersionMinor) )
  {
    error = Atams::ERROR_GEN_INFO_MISMATCH;
  }
  else if (nvmGenInfo != mapGenInfo)
  {
    error = Atams::ERROR_NVM_USER_BLOCKS_INVALID;
  }

  return (error);
}

static Atams::Error_t initNVM(void)
{
  NVMHeader_t    nvmHeader;
  NVMFooter_t    nvmFooter;

  Atams::Error_t error = getMemoryMapIsValid();

  if (!error) error = validateNVMHeaderFooter(nvmHeader, nvmFooter);

  if (!error) error = validateNVMGenInfo(nvmHeader);

  if (!error) error = loadNVMAllBlocks(nvmHeader);

  else if (error == Atams::ERROR_NVM_USER_BLOCKS_INVALID)
  {
    error = loadNVMUniversalBlock(nvmHeader);

    if (!error) error = Atams::ERROR_NVM_USER_BLOCKS_INVALID;
  }

  s_uniBlockManager.notifyStorageProcessComplete(error);

  return (error);
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap)
{
  s_appCoreInitRequired = false;

  return (initCommsCore(memoryMap));
}

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap)
{
  for (uint32_t i = 0U; i < 100; i++)
  {
    receiveByteCount[i] = 0U;
    processPreDecodeByteCount[i] = 0U;
    processPostDecodeByteCount[i] = 0U;
  }

  if (s_appCoreInitRequired) waitForControlCoreInit();

  Atams::Error_t error = Atams::validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (!error)
  {
    s_memoryMapPtr  = &memoryMap;
    s_validVarCount =  memoryMap.noOfVars;
  }

  if (!error) resetVars();

  if (!error) error = initAllDefaults();

  if (!error) error = initNVM();

  /* Invalid user blocks will not be loaded in initNVM and user has been notified through BlockUniversal::VAR_STORAGE_STATUS */
  if (error == Atams::ERROR_NVM_USER_BLOCKS_INVALID) error = Atams::ERROR_NONE;

  if (error != Atams::ERROR_NONE)
  {
    resetVars();
    error = initAllDefaults();
  }

  if (!error) error = memoryMap.initGenInfo();

  if (!error)
  {
    s_uniBlockManager.initConfiguration();

    Platform::beginReceive(receiveCallback);

    initCommsBuffers();

    signalCommsCoreInitComplete();

    s_nodeCommsState = Atams::NODE_STATE_INITIALISED;
  }
  else
  {
    invalidateMemoryMap();
  }

  return (error);
}

template <typename T>
Atams::Error_t write(const uint16_t varID, const T writeValue)
{
  if (varID >= s_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &varStorage = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  writeToVarStorage(writeValue, varStorage);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t write<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template Atams::Error_t write<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template Atams::Error_t write<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template Atams::Error_t write<int16_t >(const uint16_t varID, const int16_t  writeValue);
template Atams::Error_t write<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template Atams::Error_t write<int32_t >(const uint16_t varID, const int32_t  writeValue);
template Atams::Error_t write<float   >(const uint16_t varID, const float    writeValue);

template <typename T>
Atams::Error_t read(const uint16_t varID, T &outputRef)
{
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &var = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  readFromVarStorage(outputRef, var);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t read<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template Atams::Error_t read<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template Atams::Error_t read<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template Atams::Error_t read<int16_t >(const uint16_t varID, int16_t  &outputRef);
template Atams::Error_t read<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template Atams::Error_t read<int32_t >(const uint16_t varID, int32_t  &outputRef);
template Atams::Error_t read<float   >(const uint16_t varID, float    &outputRef);

void updateCommsPolling(void)
{
  uint32_t currentTime = Platform::getMillis();

  switch (s_nodeCommsState)
  {
    case Atams::NODE_STATE_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_STATE_INITIALISED:
      Platform::update();
      processRawMeshData();
      break;
    case Atams::NODE_STATE_ACTIVE:
      Platform::update();
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      break;
    case Atams::NODE_STATE_PROCESS_PENDING:
      if (Platform::transmitReady())
      {
        s_uniBlockManager.triggerPendingProcess();
        s_nodeCommsState = Atams::NODE_STATE_ACTIVE;
      }
      break;
    default:
      /* Do Nothing */
      break;
  }

  if (s_uniBlockManager.update() < UniversalBlockManager::NUMBER_OF_PROCESSES)
  {
    s_nodeCommsState = Atams::NODE_STATE_PROCESS_PENDING;
  }
}

void updateCommsBlocking(void)
{
  uint32_t currentTime = Platform::getMillis();

  switch (s_nodeCommsState)
  {
    case Atams::NODE_STATE_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_STATE_INITIALISED:
      Platform::waitOnCommsBufferSemaphore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      break;
    case Atams::NODE_STATE_ACTIVE:
      Platform::waitOnCommsBufferSemaphore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      break;
    default:
      /* Do Nothing */
      break;
  }

  s_uniBlockManager.update();
}


Atams::Error_t restoreAll(void)
{
  Atams::Error_t error = initAllDefaults();

  if (!error) error = storeAll();

  return (error);
}

Atams::Error_t restoreUser(void)
{
  Atams::Error_t error = getMemoryMapIsValid();

  if (!error) error = s_memoryMapPtr->initUserDefaults();

  if (!error) error = storeAll();

  return (error);
}

Atams::Error_t storeAll(void)
{
  NVMHeader_t nvmHeader;

  if (getMemoryMapIsValid() != Atams::ERROR_NONE) return (Atams::ERROR_MEMORY_MAP); /* Early Return */

  Platform::stopReceive();
  __disable_irq();

  const uint32_t requiredNVMVarSpace = getNVMVarSpaceRequirement();
  const uint32_t requiredNVMSpace    = sizeof(NVMHeader_t) + requiredNVMVarSpace + sizeof(NVMFooter_t);
  const uint32_t nvmFooterIndex      = sizeof(NVMHeader_t) + requiredNVMVarSpace;

  if (requiredNVMSpace > Platform::NVM_STORAGE_SIZE) return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return */

  nvmHeader.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmHeader.length     = requiredNVMSpace;
  nvmHeader.genInfo    = s_memoryMapPtr->genInfo;

  /* Erase NVM to invalidate */
  Atams::Error_t error = s_nvmUnitHandler.eraseNVM();

  if (!error) error = storeNVMHeader(nvmHeader);

  if (!error) error = saveVarsToNVM(nvmFooterIndex);

  if (!error) error = constructAndStoreFooter(nvmFooterIndex);

  if (!error) error = s_nvmUnitHandler.flushPendingUnit();

  for (CircularBuffer &circularBuffer : s_circularBuffers)
  {
    circularBuffer.reset();
  }

  Platform::beginReceive(receiveCallback);
  __enable_irq();

  postStore = true;

  return (error);
}


} /* End Namespace - Atams */

/**
  * @}End of File
  */
