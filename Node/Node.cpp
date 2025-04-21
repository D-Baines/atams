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
#include "Node.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Maps/BlockUniversal.hpp"
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

/* Storage Passcode Function Declarations */
static void saveToNVMNoReturn(void);
static void restoreUserBlocksAndSave(void);
static void restoreAndSave(void);

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static MemoryMap_t          s_memoryMap;
static BlockOwnerInteractor s_dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS];
static CRC32                s_nodeCRC(Atams::CRC32_POLYNOMIAL);
static CircularBuffer       s_circularBuffers[Platform::NUMBER_OF_COMMS_CHANNELS];
static DataBlock           &s_universalBlock = s_dataBlocks[BLOCK_ID_UNIVERSAL];
static WatchdogHandler      s_watchdogHandler(s_universalBlock);
static ConfigurationHandler s_ConfigurationHandler(s_universalBlock,
                                                   s_watchdogHandler,
                                                   saveToNVMNoReturn,
                                                   restoreUserBlocksAndSave,
                                                   restoreAndSave,
                                                   Platform::resetNode);

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static NodeCommsState_t s_nodeCommsState      = NODE_COMMS_UNINITIALISED;
static bool             s_bufferResetRequired = false;

/* Core Init Synchronisation */
ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static CoreInitStatus_t s_coreInitComplete[Atams::NUMBER_OF_CORES] = {CORE_INIT_IN_PROGRESS,
                                                                      CORE_INIT_IN_PROGRESS};

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
    case BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE:
      s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(Atams::ATAMS_FALSE));
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
                                                   datagramHeader.blockID,
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
  return ((varID == BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY) ||
          (varID == BlockUniversal::VAR_ID_WATCHDOG_RESET       ) );
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
                                                     datagramHeader.blockID,
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

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.blockID,
                                                            datagramHeader.varID);

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
    s_universalBlock.write(BlockUniversal::VAR_ID_CRC_ERROR_COUNT, errorCount);
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

static bool validateUniversalBlock(const MemoryMap_t &memoryMap)
{
  bool                          blockValid         = false;
  const DataBlock::Descriptor_t *universalBlockPtr = memoryMap.blockDescriptors[BLOCK_ID_UNIVERSAL];

  if ((universalBlockPtr != nullptr                         ) &&
      (universalBlockPtr == &BlockUniversal::blockDescriptor) )
  {
    blockValid = true;
  }

  return (blockValid);
}

static Atams::Error_t validateMemoryMap(const MemoryMap_t &memoryMap)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((memoryMap.noOfDataBlocks > sizeof(s_dataBlocks)             ) ||
      (memoryMap.noOfDataBlocks > Atams::MAX_NUMBER_OF_DATA_BLOCKS) )
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  if (validateUniversalBlock(memoryMap) == false)
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  s_nodeCRC.beginRollingCRC();

  uint8_t blockIndex = 0U;

  for (const DataBlock::Descriptor_t * const blockDescriptor : memoryMap.blockDescriptors)
  {
    if (blockDescriptor == nullptr)
    {
      if (blockIndex != memoryMap.noOfDataBlocks) statusReturn = Atams::ERROR_MEMORY_MAP;
      break; /* Early Break */
    }

    uint16_t varIndex = 0U;

    for (const DataBlock::VarInfo_t &varInfo : blockDescriptor->varInfo)
    {
      if ((varInfo.type        == Atams::TYPE_NULL  ) ||
          (varInfo.accessLevel == Atams::ACCESS_NONE) )
      {
        if (varIndex != blockDescriptor->noOfDataMembers) statusReturn = Atams::ERROR_MEMORY_MAP;
        break; /* Early Break */
      }

      s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
      s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.accessLevel));
      s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.NVMStorage));

      varIndex++;
    }

    blockIndex++;
  }

  if (memoryMap.genInfo.genChecksum != s_nodeCRC.getRollingCRC())
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }

  return (statusReturn);
}

static Atams::Error_t initBlockDescriptors(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = Atams::ERROR_NONE;

  if (memoryMap.noOfDataBlocks > sizeof(s_dataBlocks))
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  uint8_t blockIndex = 0U;

  for (const DataBlock::Descriptor_t * const blockDescriptor : memoryMap.blockDescriptors)
  {
    BlockOwnerInteractor &block = s_dataBlocks[blockIndex];

    if (blockDescriptor == nullptr)
    {
      break;
    }
    else
    {
      initStatus = block.initDescriptor(blockDescriptor);
      if (initStatus != Atams::ERROR_NONE) break;
    }
    blockIndex++;
  }

  if (initStatus != Atams::ERROR_NONE)
  {
    for (BlockOwnerInteractor &block : s_dataBlocks) block.deinitDescriptor();
    BlockOwnerInteractor::resetStorageBlockIndex();
  }

  return (initStatus);
}

static Atams::Error_t sharedInit(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = initBlockDescriptors(memoryMap);

  if (initStatus == Atams::ERROR_NONE) s_memoryMap = memoryMap;

  return (initStatus);
}

static void waitForCoreInit(CoreID_t coreID)
{
  uint32_t                  previousCoreCheckTime = 0U;
  volatile CoreInitStatus_t coreInitStatus         = CORE_INIT_IN_PROGRESS;

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
  else if (nvmGenInfo != s_memoryMap.genInfo)
  {
    statusReturn = Atams::ERROR_NVM_GEN_INFO;
  }

  return (statusReturn);
}

static uint32_t getNVMSpaceRequirement(void)
{
  uint32_t requiredNVMSpace = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (BlockOwnerInteractor &block : s_dataBlocks)
  {
    requiredNVMSpace += block.getNVMSpaceRequirement();
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

static Atams::Error_t loadNVMAllBlocks(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (BlockOwnerInteractor &block : s_dataBlocks)
  {
    if (nvmStatus == Atams::ERROR_NONE) nvmStatus = block.loadFromNVM(nvmHeader.length, nvmIndex);
  }

  return (nvmStatus);
}

static Atams::Error_t saveNVMAllBlocks(const uint32_t availableNVMSpace)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (BlockOwnerInteractor &block : s_dataBlocks)
  {
    if (nvmStatus == Atams::ERROR_NONE) nvmStatus = block.saveToNVM(availableNVMSpace, nvmIndex);
  }

  return (nvmStatus);
}

static void invalidateMemoryMap(void)
{
  s_memoryMap.noOfDataBlocks    = 0U;
  s_memoryMap.initUniversalData = nullptr;
  s_memoryMap.genInfo.invalidate();

   for (const DataBlock::Descriptor_t *&blockDescriptor : s_memoryMap.blockDescriptors)
   {
     blockDescriptor = nullptr;
   }

   for (BlockOwnerInteractor &block : s_dataBlocks)
   {
     block.deinitDescriptor();
   }

   BlockOwnerInteractor::resetStorageBlockIndex();
}

static Atams::Error_t initUniversalData(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = Atams::ERROR_NONE;

  if (memoryMap.initUniversalData != nullptr) initStatus = memoryMap.initUniversalData();
  else                                        initStatus = Atams::ERROR_NULL_PTR;

  return (initStatus);
}

static void initCommsBuffers(void)
{
  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
  {
    s_circularBuffers[commsChannel].setEOLChar(EOL_BYTE);
    s_circularBuffers[commsChannel].setLockArgument(static_cast<Platform::CommsChannel_t>(commsChannel));
  }
}

static void restoreAndSave(void)
{
  Atams::Error_t restoreStatus = restoreDefaults();

  if (restoreStatus == Atams::ERROR_NONE)
  {
    static_cast<void>(saveToNVM());
  }
  else
  {
    static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_STATUS,           static_cast<uint8_t>(restoreStatus)));
    static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(ATAMS_TRUE)));
  }
}

static void restoreUserBlocksAndSave(void)
{
  Atams::Error_t restoreStatus = restoreDefaultsUserBlocks();

  if (restoreStatus == Atams::ERROR_NONE)
  {
    static_cast<void>(saveToNVM());
  }
  else
  {
    static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_STATUS,           static_cast<uint8_t>(restoreStatus)));
    static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(ATAMS_TRUE)));
  }
}

static void saveToNVMNoReturn(void)
{
  static_cast<void>(saveToNVM());
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

  Atams::Error_t initStatus = sharedInit(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = initUniversalData(memoryMap);

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
  Atams::Error_t initStatus = sharedInit(memoryMap);

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

Atams::Error_t restoreDefaults(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (DataBlock &dataBlock : s_dataBlocks)
  {
    if (statusReturn == Atams::ERROR_NONE) statusReturn = dataBlock.restoreDefaults();
  }

  return (statusReturn);
}

Atams::Error_t restoreDefaultsUserBlocks(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (uint8_t blockIndex = Atams::FIRST_USER_BLOCK_ID; blockIndex < s_memoryMap.noOfDataBlocks; blockIndex++)
  {
    if (statusReturn == Atams::ERROR_NONE) statusReturn = s_dataBlocks[blockIndex].restoreDefaults();
  }

  return (statusReturn);
}

Atams::Error_t loadFromNVM(void)
{
  NVMHeader_t    nvmHeader;
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (sizeof(NVMHeader_t) > Platform::NVM_STORAGE_SIZE)
  {
    statusReturn = Atams::ERROR_NVM_PLATFORM_SIZE;
  }
  else if (!Platform::readFromNVM(0U, sizeof(nvmHeader), reinterpret_cast<uint8_t*>(&nvmHeader)))
  {
    statusReturn = Atams::ERROR_PLATFORM;
  }
  else if (nvmHeader.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)
  {
    statusReturn = Atams::ERROR_NVM_HEADER_VALIDITY;
  }
  else if (nvmHeader.length > Platform::NVM_STORAGE_SIZE)
  {
    statusReturn = Atams::ERROR_NVM_HEADER_LENGTH;
  }

  if (statusReturn == Atams::ERROR_NONE) statusReturn = validateNVMChecksum(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = validateNVMGenInfo(nvmHeader);

  if (statusReturn == Atams::ERROR_NONE) statusReturn = loadNVMAllBlocks(nvmHeader);

  static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_STATUS,           statusReturn));
  static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, ATAMS_TRUE));

  /* Messages may have been received while storage was in progress -
   * request buffer reset to clear old data */
  s_bufferResetRequired = true;

  return (statusReturn);
}

Atams::Error_t saveToNVM(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMHeader_t    invalidNVMHeader;

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
  if (!Platform::writeToNVM(sizeof(Atams::NVMHeader_t), sizeof(Atams::GenInfo_t), reinterpret_cast<uint8_t*>(&s_memoryMap.genInfo)))
  {
    return (Atams::ERROR_PLATFORM);          /* Early Return */
  }

  statusReturn = saveNVMAllBlocks(requiredNVMSpace);

  /* Write valid header to validate NVM */
  if (statusReturn == Atams::ERROR_NONE) statusReturn = constructAndWriteNVMHeader(requiredNVMSpace);

  static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_STATUS,
                                           static_cast<uint8_t>(statusReturn)));
  static_cast<void>(s_universalBlock.write(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE,
                                           static_cast<uint8_t>(ATAMS_TRUE)));

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
Atams::Error_t write(const uint8_t  blockID,
                     const uint16_t varID,
                     const T        writeData)
{
  if (blockID >= s_memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (s_dataBlocks[blockID].write(varID, writeData));
}

template Atams::Error_t write<uint8_t >(const uint8_t blockID, const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t write<int8_t  >(const uint8_t blockID, const uint16_t varID, const int8_t   writeData);
template Atams::Error_t write<uint16_t>(const uint8_t blockID, const uint16_t varID, const uint16_t writeData);
template Atams::Error_t write<int16_t >(const uint8_t blockID, const uint16_t varID, const int16_t  writeData);
template Atams::Error_t write<uint32_t>(const uint8_t blockID, const uint16_t varID, const uint32_t writeData);
template Atams::Error_t write<int32_t >(const uint8_t blockID, const uint16_t varID, const int32_t  writeData);
template Atams::Error_t write<float   >(const uint8_t blockID, const uint16_t varID, const float    writeData);

template <typename T>
Atams::Error_t read(const uint8_t   blockID,
                    const uint16_t  varID,
                          T        &readData)
{
  if (blockID >= s_memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (s_dataBlocks[blockID].read(varID, readData));
}

template Atams::Error_t read<uint8_t >(const uint8_t blockID, const uint16_t varID, uint8_t  &readData);
template Atams::Error_t read<int8_t  >(const uint8_t blockID, const uint16_t varID, int8_t   &readData);
template Atams::Error_t read<uint16_t>(const uint8_t blockID, const uint16_t varID, uint16_t &readData);
template Atams::Error_t read<int16_t >(const uint8_t blockID, const uint16_t varID, int16_t  &readData);
template Atams::Error_t read<uint32_t>(const uint8_t blockID, const uint16_t varID, uint32_t &readData);
template Atams::Error_t read<int32_t >(const uint8_t blockID, const uint16_t varID, int32_t  &readData);
template Atams::Error_t read<float   >(const uint8_t blockID, const uint16_t varID, float    &readData);

Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                const uint8_t   blockID,
                                const uint16_t  varID,
                                uint8_t * const dataStoragePtr,
                                const uint8_t   length)
{
  if (blockID >= s_memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (s_dataBlocks[blockID].externalTransfer(accessRequest, varID, dataStoragePtr, length));
}

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= s_memoryMap.noOfDataBlocks)
  {
    lengthReturn.status = Atams::ERROR_BLOCK_ID;
    return (lengthReturn); /* Early Return */
  }

  return (s_dataBlocks[blockID].getMemberLength(varID));
}

bool getWatchdogFault(void)
{
  return (s_watchdogHandler.getWatchdogFault());
}

DataBlock * getBlockPtr(const uint8_t blockID)
{
  return ((blockID < s_memoryMap.noOfDataBlocks) ? &s_dataBlocks[blockID] : nullptr);
}


} /* End Namespace - Atams */

/**
  * @}End of File
  */
