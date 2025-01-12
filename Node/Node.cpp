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
#include "Devices/DataBlockUniversal.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Utilities/CircularBuffer.hpp"
#include "../Utilities/COBS.hpp"
#include "../Utilities/CRC32.hpp"



/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint8_t  ABORT_RESPONSE_SIZE              = MESH_SIZE_HEADER + sizeof(Error_t);
static constexpr uint32_t WATCHDOG_INCREMENT_PERIOD_MILLIS = 1U;
static constexpr uint8_t  WATCHDOG_FAULT_ACTIVE            = 1U;
static constexpr uint8_t  WATCHDOG_FAULT_INACTIVE          = 0U;
static constexpr uint32_t CORE_STATUS_CHECK_PERIOD         = 10U;

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

static MemoryMap_t _memoryMap;
static DataBlock   _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS];
static DataBlock  &_universalBlock                      = _dataBlocks[BLOCK_ID_UNIVERSAL];
static Error_t     _latestError                         = ERROR_NONE;
static uint16_t    _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};

/*-- Comms --*/
CircularBuffer _circularBuffer[Platform::NUMBER_OF_COMMS_CHANNELS];

static bool     _systemIsBigEndian = false;

/* Core Init Synchronisation */
static CoreInitStatus_t _controlCoreInitComplete = CORE_INIT_IN_PROGRESS;
static uint32_t         _previousCoreCheckTime   = 0U;

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

static inline void resetResponse(ChannelResponse_t &response, uint8_t localNodeID)
{
  response.aborted                     = false;
  response.buffer[MESH_INDEX_NODE_ID]  = localNodeID;
  response.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_RESPONSE_SYNCED;
  response.index                       = MESH_INDEX_FIRST_DATAGRAM;
}

static inline void abortResponse(ChannelResponse_t &response, uint8_t localNodeID, Error_t error)
{
  response.buffer[MESH_INDEX_NODE_ID]  = localNodeID;
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
      //abortResponse(response, ERROR_ABORT_FAILURE);
    }
  }

  if (encodeMeshPacket(response.buffer,
                       response.index,
                       encodedResponseBuffer,
                       sizeof(encodedResponseBuffer),
                       encodedLength                 ) == ERROR_NONE)
  {
    Platform::transmitBuffer(commsChannel, encodedResponseBuffer, encodedLength);
  }
}


static inline void processDatagramRead(ChannelResponse_t &response,
                                       DatagramHeader_t   datagramHeader,
                                       uint8_t            payloadLength)
{
  Error_t transferStatus = externalTransfer(ACCESS_READ,
                                            datagramHeader.blockID,
                                            datagramHeader.varID,
                                            &response.buffer[response.index + DATAGRAM_SIZE_HEADER],
                                            payloadLength);

  if (transferStatus != ERROR_NONE)
  {
    datagramHeader.command = RESPONSE_NACK;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += DATAGRAM_SIZE_HEADER;
    response.buffer[response.index] = transferStatus;
    response.index += sizeof(transferStatus);
  }
  else
  {
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex.
 *           This function should only ever be called after checks have been completed in DMIB_ProcessNodePacket
 */
static inline void processDatagramWrite(ChannelResponse_t &response,
                                        DatagramHeader_t   datagramHeader,
                                        uint8_t           *datagramPayload,
                                        uint8_t            payloadLength)
{
  Error_t transferStatus = externalTransfer(ACCESS_WRITE,
                                            datagramHeader.blockID,
                                            datagramHeader.varID,
                                            datagramPayload,
                                            payloadLength);

  if (transferStatus != ERROR_NONE)
  {
    datagramHeader.command = RESPONSE_NACK;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += DATAGRAM_SIZE_HEADER;
    response.buffer[response.index] = transferStatus;
    response.index += sizeof(transferStatus);
  }
  else
  {
    datagramHeader.command = RESPONSE_ACK_WRITE;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += DATAGRAM_SIZE_HEADER;
  }
}

static inline void processRequestPacket(ChannelResponse_t &response,
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
      //abortResponse(response, ERROR_BLOCK_ID);
      return; /* Early Return */
    }

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.blockID,
                                                            datagramHeader.varID);

    if (varLength.status != ERROR_NONE)
    {
      //abortResponse(response, varLength.status);
      return; /* Early Return */
    }

    uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + varLength.data;
    uint16_t remainingOutputLength = sizeof(response.buffer) - response.index;

    if (datagramLength > remainingOutputLength)
    {
      //abortResponse(response, ERROR_RESPONSE_BUFFER_LENGTH);
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
          //abortResponse(response, ERROR_REQUEST_BUFFER_LENGTH);
          return; /* Early Return */
        }
        else
        {
          processDatagramWrite(response, datagramHeader, &meshPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD], varLength.data);
          datagramStartIndex += static_cast<uint8_t>(DATAGRAM_SIZE_HEADER + varLength.data);
        }

        break;
      }

      case ACCESS_NONE:
      //case ACCESS_FATAL:
      default:
        //abortResponse(response, ERROR_ACCESS_INVALID);
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
  static uint8_t             localNodeID     = 0U;
  static uint8_t             prevSyncNodeID  = NODE_ID_NULL;
  static uint8_t             finalSyncNodeID = 0U;
  static uint8_t             firstSyncNodeID = 0U;

  if (decodeMeshPacket(packetBufferPtr,
                       packetLength,
                       &decodedPacket[0U],
                       sizeof(decodedPacket),
                       decodedLength         ) == ERROR_NONE)
  {
    MessageType_t        messageType     = static_cast<MessageType_t>(decodedPacket[MESH_INDEX_MSG_TYPE]);
    uint8_t              packetNodeID    = decodedPacket[MESH_INDEX_NODE_ID];
    uint8_t              packetSyncCount = decodedPacket[MESH_INDEX_SYNC];
    ChannelSyncPacket_t &syncPacket      = commsChannelSyncPackets[commsChannel];
    ChannelResponse_t   &response        = commsChannelResponses[commsChannel];

    switch (messageType)
    {
      case MESSAGE_BROADCAST_UNIVERSAL:
        resetResponse(response, localNodeID);
        processRequestPacket(response, decodedPacket, decodedLength, true);
        sendResponsePacket(commsChannel, response);
        break;
      case MESSAGE_REQUEST:
        if (packetNodeID == localNodeID)
        {
          resetResponse(response, localNodeID);
          processRequestPacket(response, decodedPacket, decodedLength, false);
          sendResponsePacket(commsChannel, response);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == localNodeID)
        {
          resetResponse(response, localNodeID);
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
            sendResponsePacket(commsChannel, response);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length, false);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
        if (packetSyncCount != syncPacket.syncCount) abortResponse(response, localNodeID, ERROR_SYNC_COUNT);
        if (packetNodeID    == prevSyncNodeID      ) sendResponsePacket(commsChannel, response);
        break;
      case MESSAGE_SYNC_JOG:
        if (packetSyncCount != syncPacket.syncCount) abortResponse(response, localNodeID, ERROR_SYNC_COUNT);
        if (packetNodeID    == localNodeID         ) sendResponsePacket(commsChannel, response);
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
    _universalBlock.write(BlockUniversal::MEMBER_ID_CRC_ERROR_COUNT, errorCount);
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

static Error_t recordError(Error_t error)
{
  if (error > NUMBER_OF_ATAMS_ERRORS) error = ERROR_ERROR_MANAGEMENT;

  _latestError = error;
  _errorCounts[error]++;

  return (error);
}

static void updateWatchdog(void)
{
  uint32_t        currentTime                   = Platform::getMillis();
  static uint32_t previousWatchdogIncrementTime = currentTime;
  static uint32_t watchdogCount                 = 0U;
  static uint32_t watchdogTimeout               = 0U;
  static uint8_t  prevWatchdogClear             = WATCHDOG_FAULT_INACTIVE;

  if ((currentTime - previousWatchdogIncrementTime) > WATCHDOG_INCREMENT_PERIOD_MILLIS)
  {
    if (watchdogCount < MAX_UINT32) watchdogCount++;

    if ((_universalBlock.read(BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT, watchdogTimeout) != ERROR_NONE) ||
        ((watchdogCount    > watchdogTimeout) &&
         (watchdogTimeout != 0U             )                                                           ) )
    {
      _universalBlock.write(BlockUniversal::MEMBER_ID_WATCHDOG_FAULT_ACTIVE, WATCHDOG_FAULT_ACTIVE);
    }
    else
    {
      uint8_t watchdogClear = WATCHDOG_FAULT_INACTIVE;

      static_cast<void>(_universalBlock.read(BlockUniversal::MEMBER_ID_WATCHDOG_RESET, watchdogClear));

      if ((watchdogClear     == WATCHDOG_FAULT_ACTIVE  ) &&
          (prevWatchdogClear == WATCHDOG_FAULT_INACTIVE) )
      {
        _universalBlock.write(BlockUniversal::MEMBER_ID_WATCHDOG_FAULT_ACTIVE, WATCHDOG_FAULT_INACTIVE);
      }

      prevWatchdogClear = watchdogClear;
    }

    previousWatchdogIncrementTime = currentTime;
  }
}

static Error_t sharedInit(const MemoryMap_t &memoryMap)
{
  if ((memoryMap.noOfDataBlocks > Platform::NODE_NUMBER_OF_DATA_BLOCKS) ||
      (memoryMap.noOfDataBlocks >           MAX_NUMBER_OF_DATA_BLOCKS ) )
  {
    return (ERROR_MEMORY);   /* Early Return */
  }

  if (sizeof(float) != TYPE_LENGTHS[TYPE_FLOAT])
  {
    return (ERROR_PLATFORM); /* Early Return */
  }

  _memoryMap.noOfDataBlocks    = memoryMap.noOfDataBlocks;
  _memoryMap.initUniversalData = memoryMap.initUniversalData;

  Error_t initStatus = ERROR_NONE;

  for (uint8_t blockIndex = BLOCK_ID_UNIVERSAL; blockIndex < memoryMap.noOfDataBlocks; blockIndex++)
  {
    const DataBlock::BlockDescriptor_t &blockDescriptor = memoryMap.blockDescriptors[blockIndex];
          DataBlock                    &block           = _dataBlocks[blockIndex];

    if (initStatus == ERROR_NONE) initStatus = block.initDescriptor(blockDescriptor);
  }

  return (initStatus);
}

static void waitForControlCoreInit(void)
{
  volatile CoreInitStatus_t coreInitStatus = CORE_INIT_IN_PROGRESS;

  while (coreInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime = Platform::getMillis();

    if (currentTime - _previousCoreCheckTime >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireMemoryLock();

      coreInitStatus = _controlCoreInitComplete;

      Platform::releaseMemoryLock();

      _previousCoreCheckTime = currentTime;
    }
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initSingleCore(const MemoryMap_t &memoryMap)
{
  /* sharedInit() will run in each of the core init functions - no negative impact */

  Error_t initStatus = initControlCore(memoryMap);

  if (initStatus == ERROR_NONE) initStatus = initCommsCore(memoryMap);

  return (initStatus);
}

Error_t initCommsCore(const MemoryMap_t &memoryMap)
{
  waitForControlCoreInit();

  Error_t initStatus = sharedInit(memoryMap);

  if (initStatus == ERROR_NONE)
  {
    _systemIsBigEndian = systemIsBigEndian();
    Platform::setReceiveCallback(receiveCallback);

    for (uint8_t commsChannel = 0U; commsChannel < Platform::NUMBER_OF_COMMS_CHANNELS; commsChannel++)
    {
      _circularBuffer[commsChannel].setEOLChar(EOL_BYTE);
      _circularBuffer[commsChannel].setLockArgument(static_cast<Platform::CommsChannel_t>(commsChannel));
    }
  }
  else
  {
    _memoryMap.noOfDataBlocks = 0U;
    for (DataBlock &dataBlock : _dataBlocks) dataBlock.deinit();
  }


  return (initStatus);
}

Error_t initControlCore(const MemoryMap_t &memoryMap)
{
  Error_t initStatus = sharedInit(memoryMap);

  if (initStatus == ERROR_NONE)
  {
    for (DataBlock &dataBlock : _dataBlocks) dataBlock.resetDataMembers();
  }

  for (uint8_t blockIndex = BLOCK_ID_UNIVERSAL; blockIndex < memoryMap.noOfDataBlocks; blockIndex++)
  {
    DataBlock                          &dataBlock       = _dataBlocks[blockIndex];
    const DataBlock::BlockDescriptor_t &blockDescriptor = memoryMap.blockDescriptors[blockIndex];

    //if ((blockDescriptor.initDefaults == nullptr) ||
    //    (blockDescriptor.initLimits   == nullptr) )
    //{
    //  initStatus = ERROR_NULL_PTR;
    //}
    //
    //if (initStatus == ERROR_NONE) initStatus = blockDescriptor.initDefaults(dataBlock);
    //if (initStatus == ERROR_NONE) initStatus = blockDescriptor.initLimits(dataBlock);
  }

  if (initStatus == ERROR_NONE)
  {
    if (_memoryMap.initUniversalData != nullptr) initStatus = _memoryMap.initUniversalData();
    else                                         initStatus = ERROR_NULL_PTR;
  }

  /* NVM Init Here */

  if (initStatus == ERROR_NONE)
  {
    Platform::acquireMemoryLock();
    _controlCoreInitComplete = CORE_INIT_COMPLETE;
    Platform::releaseMemoryLock();
  }
  else
  {
    _memoryMap.noOfDataBlocks = 0U;
    for (DataBlock &dataBlock : _dataBlocks) dataBlock.deinit();
  }

  return (initStatus);
}

void updateComms(void)
{
  Platform::update();

  updateWatchdog();

  processRawMeshData();
}

template <typename T>
Error_t write(const uint8_t  blockID,
              const uint16_t varID,
              const T        writeData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].write(varID, writeData));
}

template Error_t write<uint8_t >(const uint8_t blockID, const uint16_t varID, const uint8_t  writeData);
template Error_t write<int8_t  >(const uint8_t blockID, const uint16_t varID, const int8_t   writeData);
template Error_t write<uint16_t>(const uint8_t blockID, const uint16_t varID, const uint16_t writeData);
template Error_t write<int16_t >(const uint8_t blockID, const uint16_t varID, const int16_t  writeData);
template Error_t write<uint32_t>(const uint8_t blockID, const uint16_t varID, const uint32_t writeData);
template Error_t write<int32_t >(const uint8_t blockID, const uint16_t varID, const int32_t  writeData);
template Error_t write<float   >(const uint8_t blockID, const uint16_t varID, const float    writeData);


template <typename T>
Error_t read(const uint8_t   blockID,
             const uint16_t  varID,
                   T        &readData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].read(varID, readData));
}

template Error_t read<uint8_t >(const uint8_t blockID, const uint16_t varID, uint8_t  &readData);
template Error_t read<int8_t  >(const uint8_t blockID, const uint16_t varID, int8_t   &readData);
template Error_t read<uint16_t>(const uint8_t blockID, const uint16_t varID, uint16_t &readData);
template Error_t read<int16_t >(const uint8_t blockID, const uint16_t varID, int16_t  &readData);
template Error_t read<uint32_t>(const uint8_t blockID, const uint16_t varID, uint32_t &readData);
template Error_t read<int32_t >(const uint8_t blockID, const uint16_t varID, int32_t  &readData);
template Error_t read<float   >(const uint8_t blockID, const uint16_t varID, float    &readData);

template <typename T>
Error_t assertLimits(const uint8_t   blockID,
                     const uint16_t  varID,
                     const T         limitMax,
                     const T         limitMin)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].assertLimits(varID, limitMax, limitMin));
}

template Error_t assertLimits<uint8_t >(const uint8_t blockID, const uint16_t varID, const uint8_t  limitMax, const uint8_t  limitMin);
template Error_t assertLimits<int8_t  >(const uint8_t blockID, const uint16_t varID, const int8_t   limitMax, const int8_t   limitMin);
template Error_t assertLimits<uint16_t>(const uint8_t blockID, const uint16_t varID, const uint16_t limitMax, const uint16_t limitMin);
template Error_t assertLimits<int16_t >(const uint8_t blockID, const uint16_t varID, const int16_t  limitMax, const int16_t  limitMin);
template Error_t assertLimits<uint32_t>(const uint8_t blockID, const uint16_t varID, const uint32_t limitMax, const uint32_t limitMin);
template Error_t assertLimits<int32_t >(const uint8_t blockID, const uint16_t varID, const int32_t  limitMax, const int32_t  limitMin);
template Error_t assertLimits<float   >(const uint8_t blockID, const uint16_t varID, const float    limitMax, const float    limitMin);

Error_t setWriteLock(const uint8_t  blockID,
                     const uint16_t varID,
                     const bool     writeLock)
{
  if (blockID  >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].setWriteLock(varID, writeLock));
}

bool watchdogFaultActive(void)
{
  uint8_t watchdogFaultState = WATCHDOG_FAULT_ACTIVE;
  _universalBlock.read(BlockUniversal::MEMBER_ID_WATCHDOG_FAULT_ACTIVE, watchdogFaultState);
  return (static_cast<bool>(watchdogFaultState));
}

DataBlock * getBlockPtr(const uint8_t blockID)
{
  if (blockID < _memoryMap.noOfDataBlocks)
  {
    return (&_dataBlocks[blockID]);
  }

  return (nullptr);
}

Error_t externalTransfer(const Access_t  accessRequest,
                         const uint8_t   blockID,
                         const uint16_t  varID,
                         uint8_t * const dataStoragePtr,
                         const uint8_t   length)
{
  if (blockID  >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].externalTransfer(accessRequest, varID, dataStoragePtr, length));
}

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    lengthReturn.status = ERROR_BLOCK_ID;
    return (lengthReturn);
  }

  return (_dataBlocks[blockID].getMemberLength(varID));
}


} /* End Namespace - Atams::Node */

/**
  * @}End of File
  */
