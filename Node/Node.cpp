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
#include <type_traits>
#include "Node.hpp"
#include "../Shared/Maps/BlockUniversal.hpp"
#include "../Shared/Utilities/AtamsUtilities.hpp"
#include "Utilities/CircularBuffer.hpp"
#include "../Shared/Utilities/CRC32.hpp"
#include "Developer/WatchdogHandler.hpp"
#include "Developer/ConfigurationHandler.hpp"
#include "Developer/NVMUnitHandler.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint32_t CORE_STATUS_CHECK_PERIOD = 10U;

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

enum NodeCommsState_t: uint8_t
{
  NODE_COMMS_UNINITIALISED = 0U,
  NODE_COMMS_INITIALISED   = 1U,
  NODE_COMMS_ACTIVE        = 2U
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
  uint8_t  buffer[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t index     = 0U;
  bool     aborted   = false;
};

struct ChannelSyncPacket_t
{
  uint8_t  buffer[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t length    = 0U;
};

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static const MemoryMap_t   *s_memoryMap;
static CRC32                s_nodeCRC(Atams::CRC32_POLYNOMIAL);
static CircularBuffer       s_circularBuffers[Platform::NUMBER_OF_COMMS_CHANNELS];

static WatchdogHandler      s_watchdogHandler;
static ConfigurationHandler s_ConfigurationHandler(s_watchdogHandler);
static NVMUnitHandler       s_nvmUnitHandler(s_nodeCRC);

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t         s_validVarCount       = 0U;
static NodeCommsState_t s_nodeCommsState      = NODE_COMMS_UNINITIALISED;
static bool             s_bufferResetRequired = false;
static uint8_t          s_activeSyncCount     = 0U;

/* Core Init Synchronisation */
ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static CoreInitStatus_t s_coreInitComplete[Atams::NUMBER_OF_CORES] = {CORE_INIT_IN_PROGRESS,
                                                                      CORE_INIT_IN_PROGRESS};

ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
Var_t s_varStorage[Platform::NODE_NUMBER_OF_VARS];

/*************************************************************************************/
/* PRIVATE CONSTEXPR FUNCTION DEFINITIONS                                            */
/*************************************************************************************/

template <typename T>
constexpr Atams::VarType_t getAtamsType(void)
{
    if      constexpr (std::is_same<T, uint8_t>::value)  return (Atams::TYPE_UINT8);
    else if constexpr (std::is_same<T, int8_t>::value)   return (Atams::TYPE_INT8);
    else if constexpr (std::is_same<T, uint16_t>::value) return (Atams::TYPE_UINT16);
    else if constexpr (std::is_same<T, int16_t>::value)  return (Atams::TYPE_INT16);
    else if constexpr (std::is_same<T, uint32_t>::value) return (Atams::TYPE_UINT32);
    else if constexpr (std::is_same<T, int32_t>::value)  return (Atams::TYPE_INT32);
    else if constexpr (std::is_same<T, float>::value)    return (Atams::TYPE_FLOAT);
    return (Atams::TYPE_NULL);
}

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

static inline void resetResponse(ChannelResponse_t &response, uint8_t syncCount)
{
  s_activeSyncCount = syncCount;
  response.aborted                     = false;
  response.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_UNKNOWN;
  response.buffer[MESH_INDEX_SYNC]     = s_activeSyncCount;
  response.buffer[MESH_INDEX_NODE_ID]  = s_ConfigurationHandler.getLocalNodeID();
  response.index                       = MESH_INDEX_FIRST_DATAGRAM;
}

static inline void abortResponse(ChannelResponse_t &response, const Atams::Error_t error, const uint16_t varID)
{
  response.buffer[MESH_INDEX_NODE_ID]    = s_ConfigurationHandler.getLocalNodeID();
  response.buffer[MESH_INDEX_MSG_TYPE]   = MESSAGE_ABORTED_RESPONSE;
  response.buffer[ABORT_INDEX_ERROR]     = error;
  response.buffer[ABORT_INDEX_VAR_ID_HI] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_HI) & ABORT_MASK_VAR_ID_HI);
  response.buffer[ABORT_INDEX_VAR_ID_LO] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_LO) & ABORT_MASK_VAR_ID_LO);
  response.index                         = ABORT_SIZE_PACKET;
  response.aborted                       = true;
}

static void sendResponsePacket(Platform::CommsChannel_t commsChannel,
                               ChannelResponse_t       &response,
                               Atams::MessageType_t     messageType)
{
  static uint8_t  encodedResponseBuffer[Platform::MAX_BUS_PACKET_SIZE] = {0U};
  static uint16_t encodedLength = 0U;

  if (response.aborted)
  {
    if ((response.buffer[MESH_INDEX_MSG_TYPE] != MESSAGE_ABORTED_RESPONSE) ||
        (response.index                       != ABORT_SIZE_PACKET       ) )
    {
      abortResponse(response, Atams::ERROR_ABORT_FAILURE, Atams::VAR_ID_NULL);
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
    case BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE:
      write(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(Atams::ATAMS_FALSE));
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
    abortResponse(response, transferStatus, datagramHeader.varID);
  }
  else
  {
    if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS) processUniversalRead(datagramHeader.varID);
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    requestPacketDatagramIndex += DATAGRAM_SIZE_HEADER;
  }
}

static bool accessAllowedWithoutConfig(const uint16_t varID)
{
  return ((varID == BlockUniversal::VAR_CONFIGURATION_PASSKEY) ||
          (varID == BlockUniversal::VAR_WATCHDOG_RESET       ) );
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

  if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
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
      datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
      response.index             += DATAGRAM_SIZE_HEADER;
      requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
      if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
      {
        s_ConfigurationHandler.setUpdateRequired();
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
                                  const uint16_t     requestPacketLength,
                                  const bool         universalBroadcast)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex     = MESH_INDEX_FIRST_DATAGRAM;
  uint16_t         requiredResponseLength = MESH_SIZE_HEADER;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacketLength) &&
         (response.aborted                          == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    if ((universalBroadcast                                    ) &&
        (datagramHeader.varID >= BlockUniversal::NUMBER_OF_VARS) )
    {
      abortResponse(response, Atams::ERROR_VAR_ID, datagramHeader.varID);
    }
    else
    {
      DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.varID);

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
  }

  if (response.aborted == false)
  {
    if (datagramStartIndex     != requestPacketLength    ) abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH,  Atams::VAR_ID_NULL);
    if (requiredResponseLength  > sizeof(response.buffer)) abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH, Atams::VAR_ID_NULL);
  }
}

static void processRequestPacket(ChannelResponse_t &response,
                                 uint8_t * const    requestPacket,
                                 const uint16_t     requestPacketLength,
                                 const bool         universalBroadcast)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  resetResponse(response, requestPacket[MESH_INDEX_SYNC]);

  validateRequestPacket(response, requestPacket, requestPacketLength, universalBroadcast);

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacketLength) &&
         (response.aborted                          == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateRequestPacket */
    uint8_t varLength = TYPE_LENGTHS[s_memoryMap->varInfoList[datagramHeader.varID].type];

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
    s_nodeCommsState = NODE_COMMS_ACTIVE;
  }
}

static inline void copyRequestToSyncPacket(ChannelSyncPacket_t  &syncPacket,
                                           const uint8_t * const requestBuffer,
                                           const uint16_t        requestLength)
{
  memcpy(syncPacket.buffer, requestBuffer, requestLength);
  syncPacket.length = requestLength;
}

static void processEncodedMeshPacket(const Platform::CommsChannel_t commsChannel,
                                     const uint8_t          * const packetBuffer,
                                     const uint16_t                 packetLength)
{
  static uint8_t             decodedPacket[Platform::MAX_BUS_PACKET_SIZE];
  static uint16_t            decodedLength = 0U;
  static ChannelSyncPacket_t commsChannelSyncPackets[Platform::NUMBER_OF_COMMS_CHANNELS];
  static ChannelResponse_t   commsChannelResponses[Platform::NUMBER_OF_COMMS_CHANNELS];

  if (decodeMeshPacket(packetBuffer,
                       packetLength,
                       &decodedPacket[0U],
                       sizeof(decodedPacket),
                       decodedLength         ) == Atams::ERROR_NONE)
  {
    MessageType_t        messageType     = static_cast<MessageType_t>(decodedPacket[MESH_INDEX_MSG_TYPE]);
    uint8_t              packetNodeID    = decodedPacket[MESH_INDEX_NODE_ID];
    ChannelSyncPacket_t &syncPacket      = commsChannelSyncPackets[commsChannel];
    ChannelResponse_t   &response        = commsChannelResponses[commsChannel];
    uint8_t              localNodeID     = s_ConfigurationHandler.getLocalNodeID();
    uint8_t              firstSyncNodeID = s_ConfigurationHandler.getFirstSyncNodeID();
    uint8_t              finalSyncNodeID = s_ConfigurationHandler.getFinalSyncNodeID();
    uint8_t              prevSyncNodeID  = s_ConfigurationHandler.getPrevSyncNodeID();
    uint8_t              packetSyncCount = decodedPacket[MESH_INDEX_SYNC];

    switch (messageType)
    {
      case MESSAGE_BROADCAST_UNIVERSAL:
        processRequestPacket(response, decodedPacket, decodedLength, true);
        sendResponsePacket(commsChannel, response, Atams::MESSAGE_BROADCAST_RESPONSE);
        break;
      case MESSAGE_REQUEST:
        if (packetNodeID == localNodeID)
        {
          processRequestPacket(response, decodedPacket, decodedLength, false);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == localNodeID)
        {
          if (localNodeID == finalSyncNodeID)
          {
            processRequestPacket(response, decodedPacket, decodedLength, false);
            if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
          }
          else
          {
            copyRequestToSyncPacket(syncPacket, decodedPacket, decodedLength);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length, false);
          if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
        if ((packetNodeID == prevSyncNodeID) &&
            (packetNodeID != localNodeID   ) )
        {
          if (packetSyncCount != s_activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_SYNC_JOG:
        if (packetNodeID == localNodeID)
        {
          if (packetSyncCount != s_activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
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
  static uint8_t  meshPacketRXBuffer[Platform::MAX_BUS_PACKET_SIZE] = {0U};
  static uint16_t meshPacketRXLength                                = 0U;

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
  s_memoryMap     = nullptr;
  resetVars();
}

static Atams::Error_t getMemoryMapIsValid(void)
{
  if ((s_memoryMap    != nullptr) &&
      (s_validVarCount > 0U     ) )
  {
    return (Atams::ERROR_NONE);
  }
  else
  {
    return (Atams::ERROR_MEMORY_MAP);
  }
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

static Atams::Error_t validateNVMChecksum(const NVMHeader_t &nvmHeader, const NVMFooter_t &nvmFooter)
{
  Atams::Error_t statusReturn    = Atams::ERROR_NONE;
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
    statusReturn = Atams::ERROR_NVM_CHECKSUM;
  }

  return (statusReturn);
}

static uint32_t getNVMVarSpaceRequirement(void)
{
  uint32_t requiredVarSpace = 0U;

  for (uint16_t varID = 0U; varID < s_memoryMap->noOfVars; varID++)
  {
    const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

    if (varInfo.NVMStorage) requiredVarSpace += Atams::TYPE_LENGTHS[varInfo.type];
  }

  return (requiredVarSpace);
}

static Atams::Error_t constructAndStoreFooter(const uint32_t nvmSpaceUsed)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMFooter_t    nvmFooter;

  nvmFooter.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmFooter.checksum   = s_nodeCRC.getRollingCRC();

  statusReturn = s_nvmUnitHandler.writeToNVM(nvmSpaceUsed, reinterpret_cast<uint8_t*>(&nvmFooter), sizeof(NVMFooter_t));

  return (statusReturn);
}

static Atams::Error_t nvmTransferVars(const uint32_t      maxIndex,
                                      uint32_t           &nvmIndex,
                                      const NVMTransfer_t transferType)
{
  for (uint16_t varID = 0U; varID < s_memoryMap->noOfVars; varID++)
  {
    const VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

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
          if (s_nvmUnitHandler.readFromNVM(nvmIndex, s_varStorage[varID].storage, varLength) != Atams::ERROR_NONE)
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        case TRANSFER_SAVE:
          if (s_nvmUnitHandler.writeToNVM(nvmIndex, s_varStorage[varID].storage, varLength) != Atams::ERROR_NONE)
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

static Atams::Error_t loadNVMAllVars(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t);;

  nvmStatus = nvmTransferVars(nvmHeader.length, nvmIndex, TRANSFER_LOAD);

  return (nvmStatus);
}

static Atams::Error_t saveVarsToNVM(const uint32_t availableNVMSpace)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t);

  nvmStatus = nvmTransferVars(availableNVMSpace, nvmIndex, TRANSFER_SAVE);

  return (nvmStatus);
}

static void initCommsBuffers(void)
{
  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
  {
    s_circularBuffers[commsChannel].setEOLChar(Atams::EOL_BYTE);
    s_circularBuffers[commsChannel].setLockArgument(static_cast<Platform::CommsChannel_t>(commsChannel));
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
  if (!error)    error = s_memoryMap->initUserDefaults();

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
  GenInfo_t nvmGenInfo = nvmHeader.genInfo;
  GenInfo_t mapGenInfo = s_memoryMap->genInfo;

  return ((nvmGenInfo == mapGenInfo) ? Atams::ERROR_NONE : Atams::ERROR_GEN_INFO_MISMATCH);
}

static Atams::Error_t initNVM(void)
{
  Atams::Error_t statusReturn = getMemoryMapIsValid();
  NVMHeader_t    nvmHeader;
  NVMFooter_t    nvmFooter;

  statusReturn = validateNVMHeaderFooter(nvmHeader, nvmFooter);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = validateNVMGenInfo(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = loadNVMAllVars(nvmHeader);

  if (statusReturn != Atams::ERROR_NONE)
  {
    resetVars();
  }

  s_ConfigurationHandler.notifyStorageProcessComplete(statusReturn);

  /* Messages may have been received while storage was in progress -
   * request buffer reset to clear old data */
  s_bufferResetRequired = true;

  return (statusReturn);
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap)
{
  s_coreInitComplete[CORE_CONTROL] = CORE_INIT_COMPLETE;

  return (initCommsCore(memoryMap));
}

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap)
{
  waitForCoreInit(CORE_CONTROL);

  Atams::Error_t initStatus = Atams::validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (initStatus == Atams::ERROR_NONE)
  {
    s_memoryMap     = &memoryMap;
    s_validVarCount =  memoryMap.noOfVars;
  }

  if (initStatus == Atams::ERROR_NONE) initStatus = initAllDefaults();

  if ((initStatus == Atams::ERROR_NONE) &&
      (initNVM()  != Atams::ERROR_NONE) )
  {
    initStatus = initAllDefaults();
  }

  if (initStatus == Atams::ERROR_NONE) initStatus = memoryMap.initGenInfo();

  if (initStatus == Atams::ERROR_NONE)
  {
    s_ConfigurationHandler.initConfiguration();

    Platform::setReceiveCallback(receiveCallback);

    initCommsBuffers();

    signalCoreInitComplete(CORE_COMMS);

    s_nodeCommsState = Atams::NODE_COMMS_INITIALISED;
  }
  else
  {
    invalidateMemoryMap();
  }

  return (initStatus);
}

Atams::Error_t initControlCore(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (initStatus == Atams::ERROR_NONE)
  {
    s_memoryMap     = &memoryMap;
    s_validVarCount =  memoryMap.noOfVars;

    signalCoreInitComplete(CORE_CONTROL);

    waitForCoreInit(CORE_COMMS);
  }
  else
  {
    invalidateMemoryMap();
  }

  return (initStatus);
}

Atams::Error_t restoreAll(void)
{
  Atams::Error_t statusReturn = getMemoryMapIsValid();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = initUniversalDefaults();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = s_memoryMap->initUserDefaults();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = storeAll();

  return (statusReturn);
}

Atams::Error_t restoreUser(void)
{
  Atams::Error_t statusReturn = getMemoryMapIsValid();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = s_memoryMap->initUserDefaults();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = storeAll();

  return (statusReturn);
}

Atams::Error_t storeAll(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMHeader_t    nvmHeader;

  if (getMemoryMapIsValid() != Atams::ERROR_NONE) return (Atams::ERROR_MEMORY_MAP); /* Early Return */

  const uint32_t requiredNVMVarSpace = getNVMVarSpaceRequirement();
  const uint32_t requiredNVMSpace    = sizeof(NVMHeader_t) + requiredNVMVarSpace + sizeof(NVMFooter_t);
  const uint32_t nvmFooterIndex      = sizeof(NVMHeader_t) + requiredNVMVarSpace;

  if (requiredNVMSpace > Platform::NVM_STORAGE_SIZE) return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return */

  nvmHeader.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmHeader.length     = requiredNVMSpace;
  nvmHeader.genInfo    = s_memoryMap->genInfo;

  /* Erase NVM to invalidate */
  statusReturn = s_nvmUnitHandler.eraseNVM();

  if (statusReturn == Atams::ERROR_NONE) statusReturn = storeNVMHeader(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = saveVarsToNVM(nvmFooterIndex);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = constructAndStoreFooter(nvmFooterIndex);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = s_nvmUnitHandler.flushPendingUnit();

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
    case Atams::NODE_COMMS_ACTIVE:
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
    case Atams::NODE_COMMS_ACTIVE:
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
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::Var_t &var = s_varStorage[varID];

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
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::Var_t &var = s_varStorage[varID];

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
  if (varID >= s_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMap->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length)              return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (bytesPtr                   == nullptr)             return (Atams::ERROR_NULLPTR);        /* Early Return */
  if (accessRequest              >  varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Atams::Var_t   &var        = s_varStorage[varID];
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
