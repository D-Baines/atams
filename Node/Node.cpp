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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint8_t  ABORT_RESPONSE_SIZE          = Atams::MESH_SIZE_HEADER + sizeof(Atams::Error_t);
static constexpr uint32_t WATCHDOG_PERIOD_MILLISECONDS = 1U;
static constexpr uint8_t  WATCHDOG_FAULT_ACTIVE        = 1U;
static constexpr uint8_t  WATCHDOG_FAULT_INACTIVE      = 0U;
static constexpr uint32_t CORE_STATUS_CHECK_PERIOD     = 10U;

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

typedef enum: uint8_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
} CoreInitStatus_t;

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
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

/*-- Node --*/

static MemoryMap_t          _memoryMap;
static BlockOwnerInteractor _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS];
static DataBlock            &_universalBlock = _dataBlocks[BLOCK_ID_UNIVERSAL];
static uint8_t               _localNodeID    = 0U;
static CRC32                 _nodeCRC(Atams::CRC32_POLYNOMIAL);

/*-- Comms --*/
CircularBuffer _circularBuffer[Platform::NUMBER_OF_COMMS_CHANNELS];

/* Core Init Synchronisation */
static CoreInitStatus_t _coreInitComplete[Atams::NUMBER_OF_CORES] = {CORE_INIT_IN_PROGRESS,
                                                                     CORE_INIT_IN_PROGRESS}; /* THIS NEEDS TO BE IN SHARED MEMORY */
static uint32_t         _previousCoreCheckTime = 0U;

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void receiveCallback(const Platform::CommsChannel_t commsChannel,
                                  uint8_t                 *rxBufferPtr,
                            const uint16_t                 rxBufferLength)
{
  if (commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS)
  {
    _circularBuffer[commsChannel].pushHead(rxBufferPtr, rxBufferLength);
  }
}

static inline void resetResponse(ChannelResponse_t &response)
{
  response.aborted                     = false;
  response.buffer[MESH_INDEX_NODE_ID]  = _localNodeID;
  response.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_RESPONSE_SYNCED;
  response.index                       = MESH_INDEX_FIRST_DATAGRAM;
}

static inline void abortResponse(ChannelResponse_t &response, Atams::Error_t error)
{
  response.buffer[MESH_INDEX_NODE_ID]  = _localNodeID;
  response.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_ABORTED_RESPONSE;
  response.buffer[MESH_SIZE_HEADER]    = error;
  response.index                       = MESH_SIZE_HEADER + sizeof(error);
  response.aborted                     = true;
}

static void sendResponsePacket(Platform::CommsChannel_t commsChannel, ChannelResponse_t &response)
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

  if (encodeMeshPacket(response.buffer,
                       response.index,
                       encodedResponseBuffer,
                       sizeof(encodedResponseBuffer),
                       encodedLength                 ) == Atams::ERROR_NONE)
  {
    Platform::transmitBuffer(commsChannel, encodedResponseBuffer, encodedLength);
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex. */
static void processDatagramRead(ChannelResponse_t &response,
                                DatagramHeader_t   datagramHeader,
                                uint8_t            payloadLength)
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
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex. */
static void processDatagramWrite(ChannelResponse_t &response,
                                 DatagramHeader_t   datagramHeader,
                                 uint8_t           *datagramPayload,
                                 uint8_t            payloadLength)
{
  Atams::Error_t transferStatus = externalTransfer(ACCESS_WRITE,
                                                   datagramHeader.blockID,
                                                   datagramHeader.varID,
                                                   datagramPayload,
                                                   payloadLength);

  if (transferStatus != Atams::ERROR_NONE)
  {
    abortResponse(response, transferStatus);
  }
  else
  {
    datagramHeader.command = RESPONSE_ACK_WRITE;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += DATAGRAM_SIZE_HEADER;
  }
}

static void processRequestPacket(ChannelResponse_t &response,
                                 uint8_t           *meshPacket,
                                 uint16_t           meshPacketLength,
                                 bool               universalBroadcast)
{
  bool               cancelProcessing   = false;
  volatile uint8_t   datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  response.index = MESH_SIZE_HEADER;

  while (datagramStartIndex + DATAGRAM_SIZE_HEADER <= meshPacketLength)
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&meshPacket[datagramStartIndex], datagramHeader);

    if ((universalBroadcast                          ) &&
        (datagramHeader.blockID != BLOCK_ID_UNIVERSAL) )
    {
      abortResponse(response, Atams::ERROR_BLOCK_ID);
      return; /* Early Return */
    }

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.blockID,
                                                            datagramHeader.varID);

    if (varLength.status != Atams::ERROR_NONE)
    {
      abortResponse(response, varLength.status);
      return; /* Early Return */
    }

    uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + varLength.data;
    uint16_t remainingOutputLength = sizeof(response.buffer) - response.index;

    if (datagramLength > remainingOutputLength)
    {
      abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH);
      return; /* Early Return */
    }

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ:
        processDatagramRead(response, datagramHeader, varLength.data);
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;

      case ACCESS_WRITE:
      {
        uint16_t remainingInputLength = meshPacketLength - datagramStartIndex;

        if (datagramLength > remainingInputLength)
        {
          abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH);
          return; /* Early Return */
        }
        else
        {
          processDatagramWrite(response, datagramHeader, &meshPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD], varLength.data);
          datagramStartIndex += static_cast<uint8_t>(DATAGRAM_SIZE_HEADER + varLength.data);
        }

        break;
      }

      default:
        abortResponse(response, Atams::ERROR_ACCESS_INVALID);
        return;
    }
  }

  if (cancelProcessing == false)
  {
    //_watchdogCount = 0U;

    //Node::write(UniversalMemoryMap::BLOCK_ID_UNIVERSAL,
    //                    UniversalMemoryMap::MEMBER_ID_WATCHDOG_COUNT,
    //                    0U);
  }
}

static void processEncodedMeshPacket(Platform::CommsChannel_t commsChannel,
                                     uint8_t                 *packetBufferPtr,
                                     uint16_t                 packetLength)
{
  static uint8_t             decodedPacket[MAX_MESH_PACKET_SIZE];
  static uint16_t            decodedLength  = 0U;
  static ChannelSyncPacket_t commsChannelSyncPackets[Platform::NUMBER_OF_COMMS_CHANNELS];
  static ChannelResponse_t   commsChannelResponses[Platform::NUMBER_OF_COMMS_CHANNELS];
  static uint8_t             prevSyncNodeID  = NODE_ID_NULL;
  static uint8_t             finalSyncNodeID = 0U;
  static uint8_t             firstSyncNodeID = 0U;

  static_cast<void>(_universalBlock.read(BlockUniversal::VAR_ID_NODE_ID, _localNodeID));

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

    switch (messageType)
    {
      case MESSAGE_BROADCAST_UNIVERSAL:
        resetResponse(response);
        processRequestPacket(response, decodedPacket, decodedLength, true);
        sendResponsePacket(commsChannel, response);
        break;
      case MESSAGE_REQUEST:
        if (packetNodeID == _localNodeID)
        {
          resetResponse(response);
          processRequestPacket(response, decodedPacket, decodedLength, false);
          sendResponsePacket(commsChannel, response);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == _localNodeID)
        {
          resetResponse(response);
          syncPacket.syncCount = packetSyncCount;
          if (_localNodeID == finalSyncNodeID)
          {
            processRequestPacket(response, decodedPacket, decodedLength, false);
          }
          else
          {
            memcpy(syncPacket.buffer, decodedPacket, decodedLength);
            syncPacket.length = decodedLength;
          }
          if (_localNodeID == firstSyncNodeID)
          {
            sendResponsePacket(commsChannel, response);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length, false);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
        if (packetSyncCount != syncPacket.syncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT);
        if (packetNodeID    == prevSyncNodeID      ) sendResponsePacket(commsChannel, response);
        break;
      case MESSAGE_SYNC_JOG:
        if (packetSyncCount != syncPacket.syncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT);
        if (packetNodeID    == _localNodeID        ) sendResponsePacket(commsChannel, response);
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
    _universalBlock.write(BlockUniversal::VAR_ID_CRC_ERROR_COUNT, errorCount);
  }
}

static void processRawMeshData(void)
{
  static uint8_t  meshPacketRXBuffer[MAX_MESH_PACKET_SIZE] = {0U};
  static uint16_t meshPacketRXLength                       = 0U;

  for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
  {
    CircularBuffer::Error_t bufferStatus = _circularBuffer[commsChannel].getPacket(meshPacketRXBuffer,
                                                                                   sizeof(meshPacketRXBuffer),
                                                                                   meshPacketRXLength);

    /* Early return if no packets ready */
    if (bufferStatus != CircularBuffer::ERROR_NONE)
    {
      if (bufferStatus == CircularBuffer::ERROR_NO_EOL_BUFFER_FULL ||
          bufferStatus == CircularBuffer::ERROR_OUTPUT_BUFFER_LENGTH)
      {
        /* Reset the buffer if no valid packet has been found or if
         * the valid packet is too long for the mesh packet buffer */
        _circularBuffer[commsChannel].reset();
      }
    }
    else
    {
      /* Process the packet that has been copied into the mesh packet buffer */
      processEncodedMeshPacket(static_cast<Platform::CommsChannel_t>(commsChannel),
                               meshPacketRXBuffer,
                               meshPacketRXLength);
    }
  }
}

static void updateWatchdog(void)
{
  uint32_t        currentTime                   = Platform::getMillis();
  static uint32_t previousWatchdogIncrementTime = currentTime;
  static uint32_t watchdogCount                 = 0U;
  static uint32_t watchdogTimeout               = 0U;
  static uint8_t  prevWatchdogClear             = WATCHDOG_FAULT_INACTIVE;

  if ((currentTime - previousWatchdogIncrementTime) > WATCHDOG_PERIOD_MILLISECONDS)
  {
    if (watchdogCount < MAX_UINT32) watchdogCount++;

    if ((_universalBlock.read(BlockUniversal::VAR_ID_WATCHDOG_TIMEOUT, watchdogTimeout) != Atams::ERROR_NONE) ||
        ((watchdogCount    > watchdogTimeout) &&
         (watchdogTimeout != 0U             )                                                                  ) )
    {
      _universalBlock.write(BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE, WATCHDOG_FAULT_ACTIVE);
    }
    else
    {
      uint8_t watchdogClear = WATCHDOG_FAULT_INACTIVE;

      static_cast<void>(_universalBlock.read(BlockUniversal::VAR_ID_WATCHDOG_RESET, watchdogClear));

      if ((watchdogClear     == WATCHDOG_FAULT_ACTIVE  ) &&
          (prevWatchdogClear == WATCHDOG_FAULT_INACTIVE) )
      {
        _universalBlock.write(BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE, WATCHDOG_FAULT_INACTIVE);
      }

      prevWatchdogClear = watchdogClear;
    }

    previousWatchdogIncrementTime = currentTime;
  }
}

static Atams::Error_t validateMemoryMap(const MemoryMap_t &memoryMap)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((memoryMap.noOfDataBlocks > sizeof(_dataBlocks)       ) ||
      (memoryMap.noOfDataBlocks > MAX_NUMBER_OF_DATA_BLOCKS ) )
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  _nodeCRC.beginRollingCRC();

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

      _nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
      _nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.accessLevel));
      _nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.NVMStorage));

      varIndex++;
    }

    if (statusReturn != Atams::ERROR_NONE) break; /* Early Break */

    blockIndex++;
  }

  if ((statusReturn                  != Atams::ERROR_NONE       ) ||
      (memoryMap.genInfo.genChecksum != _nodeCRC.getRollingCRC()) )
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }

  return (statusReturn);
}

static Atams::Error_t initBlockDescriptors(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = Atams::ERROR_NONE;

  if (memoryMap.noOfDataBlocks > sizeof(_dataBlocks))
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  uint8_t blockIndex = 0U;

  for (const DataBlock::Descriptor_t * const blockDescriptor : memoryMap.blockDescriptors)
  {
    DataBlock &block = _dataBlocks[blockIndex];

    if (blockDescriptor != nullptr)
    {
      initStatus = block.initDescriptor(blockDescriptor);
      if (initStatus != Atams::ERROR_NONE) break;
    }
    else
    {
      break;
    }
    blockIndex++;
  }

  if (initStatus != Atams::ERROR_NONE)
  {
    for (DataBlock &block : _dataBlocks) block.deinitDescriptor();
  }

  return (initStatus);
}

static Atams::Error_t sharedInit(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = initBlockDescriptors(memoryMap);

  if (initStatus == Atams::ERROR_NONE) _memoryMap = memoryMap;

  return (initStatus);
}

static void waitForCoreInit(CoreID_t coreID)
{
  volatile CoreInitStatus_t coreInitStatus = CORE_INIT_IN_PROGRESS;

  while (coreInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime = Platform::getMillis();

    if (currentTime - _previousCoreCheckTime >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireVarStorageLock();

      coreInitStatus = _coreInitComplete[coreID];

      Platform::releaseVarStorageLock();

      _previousCoreCheckTime = currentTime;
    }
  }
}

static void signalCoreInitComplete(CoreID_t coreID)
{
  Platform::acquireVarStorageLock();
  _coreInitComplete[coreID] = CORE_INIT_COMPLETE;
  Platform::releaseVarStorageLock();
}

static Atams::Error_t validateNVMChecksum(const NVMHeader_t &nvmHeader)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  _nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex = sizeof(NVMHeader_t); nvmIndex < nvmHeader.length; nvmIndex++)
  {
    uint8_t nvmByte;

    if (Platform::readFromNVM(nvmIndex, sizeof(nvmByte), &nvmByte) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    _nodeCRC.updateRollingCRC(nvmByte);
  }

  if (nvmHeader.checksum != _nodeCRC.getRollingCRC())
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
  else if (nvmGenInfo != _memoryMap.genInfo)
  {
    statusReturn = Atams::ERROR_NVM_GEN_INFO;
  }

  return (statusReturn);
}

static uint32_t getNVMSpaceRequirement(void)
{
  uint32_t requiredNVMSpace = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (BlockOwnerInteractor &block : _dataBlocks)
  {
    requiredNVMSpace += block.getNVMSpaceRequirement();
  }

  return (requiredNVMSpace);
}

static Atams::Error_t constructAndWriteNVMHeader(const uint32_t nvmSpaceUsed)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  NVMHeader_t    nvmHeader;

  _nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex = sizeof(NVMHeader_t); nvmIndex < nvmSpaceUsed; nvmIndex++)
  {
    uint8_t nvmByte;

    if (Platform::readFromNVM(nvmIndex, sizeof(nvmByte), &nvmByte) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    _nodeCRC.updateRollingCRC(nvmByte);
  }

  nvmHeader.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmHeader.length     = nvmSpaceUsed;
  nvmHeader.checksum   = _nodeCRC.getRollingCRC();

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

  for (BlockOwnerInteractor &block : _dataBlocks)
  {
    if (nvmStatus == Atams::ERROR_NONE) nvmStatus = block.loadFromNVM(nvmHeader.length, nvmIndex);
  }

  return (nvmStatus);
}

static Atams::Error_t saveNVMAllBlocks(const uint32_t availableNVMSpace)
{
  Atams::Error_t nvmStatus = Atams::ERROR_NONE;
  uint32_t       nvmIndex  = sizeof(NVMHeader_t) + sizeof(GenInfo_t);

  for (BlockOwnerInteractor &block : _dataBlocks)
  {
    if (nvmStatus == Atams::ERROR_NONE) nvmStatus = block.saveToNVM(availableNVMSpace, nvmIndex);
  }

  return (nvmStatus);
}

static void invalidateMemoryMap(void)
{
  _memoryMap.noOfDataBlocks            = 0U;
  _memoryMap.genInfo.atamsVersionMajor = 0U;
  _memoryMap.genInfo.atamsVersionMinor = 0U;
  _memoryMap.genInfo.genDay            = 0U;
  _memoryMap.genInfo.genMonth          = 0U;
  _memoryMap.genInfo.genYear           = 0U;
  _memoryMap.genInfo.genHour           = 0U;
  _memoryMap.genInfo.genMinute         = 0U;
  _memoryMap.genInfo.genSecond         = 0U;
  _memoryMap.genInfo.genChecksum       = 0U;
  _memoryMap.initUniversalData         = nullptr;

   for (const DataBlock::Descriptor_t *&blockDescriptor : _memoryMap.blockDescriptors)
   {
     blockDescriptor = nullptr;
   }

   for (DataBlock &block : _dataBlocks)
   {
     block.deinitDescriptor();
   }
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
    _circularBuffer[commsChannel].setEOLChar(EOL_BYTE);
    _circularBuffer[commsChannel].setLockArgument(static_cast<Platform::CommsChannel_t>(commsChannel));
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

  Atams::Error_t initStatus = sharedInit(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = initUniversalData(memoryMap);

  if (initStatus == Atams::ERROR_NONE)
  {
    Platform::setReceiveCallback(receiveCallback);

    initCommsBuffers();

    signalCoreInitComplete(CORE_COMMS);
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

Atams::Error_t initDefaults(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (DataBlock &dataBlock : _dataBlocks)
  {
    if (statusReturn == Atams::ERROR_NONE) statusReturn = dataBlock.initDefaults();
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

  if (statusReturn != Atams::ERROR_NONE) initDefaults();

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

  /* Write genInfo to NVM */
  if (!Platform::writeToNVM(sizeof(Atams::NVMHeader_t), sizeof(Atams::GenInfo_t), reinterpret_cast<uint8_t*>(&_memoryMap.genInfo)))
  {
    return (Atams::ERROR_PLATFORM);          /* Early Return */
  }

  statusReturn = saveNVMAllBlocks(requiredNVMSpace);

  /* Write valid header to validate NVM */
  if (statusReturn == Atams::ERROR_NONE) statusReturn = constructAndWriteNVMHeader(requiredNVMSpace);

  return (statusReturn);
}

void updateCommsPolling(void)
{
  Platform::update();
  updateWatchdog();
  processRawMeshData();
}

void updateCommsBlocking(void)
{
  Platform::waitOnCommsBufferSemaphore(Atams::WATCHDOG_PERIOD_MILLISECONDS);
  updateWatchdog();
  processRawMeshData();
}

template <typename T>
Atams::Error_t write(const uint8_t  blockID,
                     const uint16_t varID,
                     const T        writeData)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (_dataBlocks[blockID].write(varID, writeData));
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
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (_dataBlocks[blockID].read(varID, readData));
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
  if (blockID  >= _memoryMap.noOfDataBlocks)
  {
    return (Atams::ERROR_BLOCK_ID); /* Early Return */
  }

  return (_dataBlocks[blockID].externalTransfer(accessRequest, varID, dataStoragePtr, length));
}

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    lengthReturn.status = Atams::ERROR_BLOCK_ID;
    return (lengthReturn); /* Early Return */
  }

  return (_dataBlocks[blockID].getMemberLength(varID));
}

bool watchdogFaultActive(void)
{
  uint8_t watchdogFaultState = WATCHDOG_FAULT_ACTIVE;
  _universalBlock.read(BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE, watchdogFaultState);
  return (static_cast<bool>(watchdogFaultState));
}

DataBlock * getBlockPtr(const uint8_t blockID)
{
  return ((blockID < _memoryMap.noOfDataBlocks) ? &_dataBlocks[blockID] : nullptr);
}


} /* End Namespace - Atams */

/**
  * @}End of File
  */
