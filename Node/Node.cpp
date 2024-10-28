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

#include "Devices/DataBlockUniversal.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "../Utilities/CircularBuffer.hpp"
#include "../Utilities/COBS.hpp"
#include "../Utilities/CRC32.hpp"



/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint32_t CRC32_POLYNOMIAL                 = 0x04C11DB7;
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

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

/*-- Node --*/

static MemoryMap_t _memoryMap;
static DataBlock   _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS + 1U];
static DataBlock  &_universalBlock                      = _dataBlocks[BLOCK_ID_UNIVERSAL];
static Error_t     _latestError                         = ERROR_NONE;
static uint16_t    _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};

/*-- Comms --*/
CircularBuffer _circularBuffer(EOL_BYTE,
                               Platform::acquireCommsBufferLock,
                               Platform::releaseCommsBufferLock);

static CRC32    _crcAtams(CRC32_POLYNOMIAL);
static uint8_t  _meshPacketRXBuffer[MAX_MESH_PACKET_SIZE] = {0U};
static uint8_t  _responseBuffer[MAX_NODE_PACKET_SIZE]     = {0U};
static uint32_t _meshPacketRXLength                       = 0U;
static uint16_t _responseBufferIndex                      = 0U;
static bool     _responseAborted                          = false;
static bool     _systemIsBigEndian                        = false;
static uint8_t  _localNodeID                              = 0x00U;
static uint8_t  _prevSyncNodeID                           = NODE_ID_NULL;
static uint8_t  _finalSyncNodeID                          = NODE_ID_NULL;
static uint8_t  _firstSyncNodeID                          = NODE_ID_NULL;
static uint32_t _crcErrorCount                            = 0U;
static uint32_t _cobsErrorCount                           = 0U;

/* Core Init Synchronisation */
static CoreInitStatus_t _controlCoreInitComplete = CORE_INIT_IN_PROGRESS;
static uint32_t         _previousCoreCheckTime   = 0U;

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void receiveCallback(      uint8_t       *rxBufferPtr,
                            const uint16_t       rxBufferLength,
                            const CommsStorage_t storageMethod)
{
  _circularBuffer.pushHead(rxBufferPtr, rxBufferLength);
}

static Error_t decodeMeshPacket(const uint8_t  *inputPacket,
                                const uint16_t  inputPacketLength,
                                      uint8_t  *decodedPacket,
                                const uint16_t  decodedPacketMaxLength,
                                      uint16_t &decodedLength)
{
  COBS::Result_t COBSDecodeResult = COBS::decode(inputPacket, inputPacketLength, decodedPacket, decodedPacketMaxLength);

  if (COBSDecodeResult.status != COBS::ERROR_NONE)
  {
    _cobsErrorCount++;
    return (ERROR_DECODE);
  }

  if (COBSDecodeResult.outputLength < MESH_SIZE_HEADER)
  {
    return (ERROR_DECODE);
  }

  uint32_t packetCRC = *reinterpret_cast<uint32_t*>(&decodedPacket[MESH_INDEX_CRC]);

  *reinterpret_cast<uint32_t*>(&decodedPacket[MESH_INDEX_CRC]) = 0U;

  if (packetCRC != _crcAtams.calculateCRC32(decodedPacket, COBSDecodeResult.outputLength))
  {
    _crcErrorCount++;
    return (ERROR_DECODE);
  }

  decodedLength = COBSDecodeResult.outputLength;

  return (ERROR_NONE);
}

static Error_t encodeMeshPacket(      uint8_t  *txPacket,
                                const uint16_t  txLength,
                                      uint8_t  *encodedPacket,
                                const uint16_t  encodedPacketMaxLength,
                                      uint16_t &encodedLength)
{
  if (txLength < MESH_SIZE_HEADER)
  {
    return (ERROR_ENCODE);
  }

  *reinterpret_cast<uint32_t*>(&txPacket[MESH_INDEX_CRC]) = 0U;

  uint32_t CRCResult = _crcAtams.calculateCRC32(txPacket, txLength);

  if (_systemIsBigEndian) swapEndiannessType<uint32_t>(CRCResult);

  *reinterpret_cast<uint32_t*>(&txPacket[MESH_INDEX_CRC]) = CRCResult;

  COBS::Result_t COBSEncodeResult = COBS::encode(txPacket, txLength, encodedPacket, encodedPacketMaxLength);

  if (COBSEncodeResult.status != COBS::ERROR_NONE)
  {
    return (ERROR_ENCODE);
  }

  encodedLength = COBSEncodeResult.outputLength;

  return (ERROR_NONE);
}

static inline void resetResponse(void)
{
  _responseAborted                     = false;
  _responseBuffer[MESH_INDEX_NODE_ID]  = _localNodeID;
  _responseBuffer[MESH_INDEX_MSG_TYPE] = MESSAGE_RESPONSE_SYNCED;
  _responseBufferIndex                 = MESH_INDEX_FIRST_DATAGRAM;
}

static inline void abortResponse(Error_t error)
{
  _responseBuffer[MESH_INDEX_NODE_ID]  = _localNodeID;
  _responseBuffer[MESH_INDEX_MSG_TYPE] = MESSAGE_ABORTED_RESPONSE;
  _responseBufferIndex                 = MESH_SIZE_HEADER;
  _responseAborted                     = true;

  _responseBuffer[_responseBufferIndex] = error;
  _responseBufferIndex += sizeof(error);
}

static void sendResponsePacket(void)
{
  static uint8_t  encodedResponseBuffer[MAX_MESH_PACKET_SIZE] = {0U};
  static uint16_t encodedLength                               = 0U;

  if (_responseAborted)
  {
    if ((_responseBuffer[MESH_INDEX_MSG_TYPE] != MESSAGE_ABORTED_RESPONSE) ||
        (_responseBufferIndex                 != ABORT_RESPONSE_SIZE     ) )
    {
      abortResponse(ERROR_ABORT_FAILURE);
    }
  }

  if (encodeMeshPacket(_responseBuffer,
                       _responseBufferIndex,
                       encodedResponseBuffer,
                       sizeof(encodedResponseBuffer),
                       encodedLength                 ) == ERROR_NONE)
  {
    Platform::transmitBuffer(encodedResponseBuffer, encodedLength);
  }
}


static inline void processDatagramRead(DatagramHeader_t datagramHeader,
                                       uint8_t          payloadLength)
{
  Error_t transferStatus = externalTransfer(ACCESS_READ,
                                            datagramHeader.blockID,
                                            datagramHeader.varID,
                                            &_responseBuffer[_responseBufferIndex + DATAGRAM_SIZE_HEADER],
                                            payloadLength);

  if (transferStatus != ERROR_NONE)
  {
    datagramHeader.command = ACCESS_NONE;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER;
    _responseBuffer[_responseBufferIndex] = transferStatus;
    _responseBufferIndex += sizeof(transferStatus);
  }
  else
  {
    datagramHeader.command = ACCESS_READ;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER + payloadLength;
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex.
 *           This function should only ever be called after checks have been completed in DMIB_ProcessNodePacket
 */
static inline void processDatagramWrite(DatagramHeader_t datagramHeader,
                                        uint8_t         *datagramPayload,
                                        uint8_t          payloadLength)
{
  Error_t transferStatus = externalTransfer(ACCESS_WRITE,
                                            datagramHeader.blockID,
                                            datagramHeader.varID,
                                            datagramPayload,
                                            payloadLength);

  if (transferStatus != ERROR_NONE)
  {
    datagramHeader.command = ACCESS_NONE;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER;
    _responseBuffer[_responseBufferIndex] = transferStatus;
    _responseBufferIndex += sizeof(transferStatus);
  }
  else
  {
    datagramHeader.command = ACCESS_WRITE;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER;
    memcpy(&_responseBuffer[_responseBufferIndex], datagramPayload, payloadLength);
  }
}

static inline void processRequestPacket(uint8_t *meshPacket,
                                        uint16_t meshPacketLength,
                                        bool     universalBroadcast)
{
  bool             cancelProcessing   = false;
  volatile uint8_t datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while (datagramStartIndex + DATAGRAM_SIZE_HEADER <= meshPacketLength)
  {
    DatagramHeader_t datagramHeader;

    memcpy(&datagramHeader, &meshPacket[datagramStartIndex], DATAGRAM_SIZE_HEADER);

    if (systemIsBigEndian()) swapEndiannessType(datagramHeader); /* TODO:: CAN BE DONE BETTER WITH SHIFTS/MASKS */

    if ((universalBroadcast                               ) &&
        (datagramHeader.blockID != BLOCK_ID_UNIVERSAL) )
    {
      abortResponse(ERROR_BLOCK_ID);
      return; /* Early Return */
    }

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.blockID,
                                                            datagramHeader.varID);

    if (varLength.status != ERROR_NONE)
    {
      abortResponse(varLength.status);
      return; /* Early Return */
    }

    uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + varLength.data;
    uint16_t remainingOutputLength = sizeof(_responseBuffer) - _responseBufferIndex;

    if (datagramLength > remainingOutputLength)
    {
      abortResponse(ERROR_RESPONSE_BUFFER_LENGTH);
      return; /* Early Return */
    }

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ:
        processDatagramRead(datagramHeader, varLength.data);
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;

      case ACCESS_WRITE:
      {
        uint16_t remainingInputLength = meshPacketLength - datagramStartIndex;

        if (datagramLength > remainingInputLength)
        {
          abortResponse(ERROR_REQUEST_BUFFER_LENGTH);
          return; /* Early Return */
        }
        else
        {
          processDatagramWrite(datagramHeader, &meshPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD], varLength.data);
          datagramStartIndex += (DATAGRAM_SIZE_HEADER + varLength.data);
        }

        break;
      }

      case ACCESS_NONE:
      //case ACCESS_FATAL:
      default:
        abortResponse(ERROR_ACCESS_INVALID);
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

static void processEncodedMeshPacket(uint8_t *packetBufferPtr,
                                     uint16_t packetLength)
{
  static uint8_t  decodedPacket[MAX_MESH_PACKET_SIZE];
  static uint8_t  syncPacket[MAX_MESH_PACKET_SIZE];
  static uint16_t decodedLength  = 0U;
  static uint16_t syncLength     = 0U;
  static uint8_t  localSyncCount = 0U;

  if (decodeMeshPacket(packetBufferPtr,
                       packetLength,
                       &decodedPacket[0U],
                       sizeof(decodedPacket),
                       decodedLength         ) == ERROR_NONE)
  {
    MessageType_t messageType     = static_cast<MessageType_t>(decodedPacket[MESH_INDEX_MSG_TYPE]);
    uint8_t       packetNodeID    = decodedPacket[MESH_INDEX_NODE_ID];
    uint8_t       packetSyncCount = decodedPacket[MESH_INDEX_SYNC];

    switch (messageType)
    {
      case MESSAGE_BROADCAST_UNIVERSAL:
        resetResponse();
        processRequestPacket(decodedPacket, decodedLength, true);
        sendResponsePacket();
        break;
      case MESSAGE_REQUEST:
        if (packetNodeID == _localNodeID)
        {
          resetResponse();
          processRequestPacket(decodedPacket, decodedLength, false);
          sendResponsePacket();
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == _localNodeID)
        {
          resetResponse();
          localSyncCount = packetSyncCount;
          if (_localNodeID == _finalSyncNodeID)
          {
            processRequestPacket(decodedPacket, decodedLength, false);
          }
          else
          {
            memcpy(syncPacket, decodedPacket, decodedLength);
            syncLength = decodedLength;
          }
          if (_localNodeID == _firstSyncNodeID) sendResponsePacket();
        }
        else if (packetNodeID == _finalSyncNodeID)
        {
          processRequestPacket(syncPacket, syncLength, false);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
        if (packetSyncCount != localSyncCount ) abortResponse(ERROR_SYNC_COUNT);
        if (packetNodeID    == _prevSyncNodeID) sendResponsePacket();
        break;
      case MESSAGE_SYNC_JOG:
        if (packetSyncCount != localSyncCount) abortResponse(ERROR_SYNC_COUNT);
        if (packetNodeID    == _localNodeID  ) sendResponsePacket();
        break;
      default:
        /* Do Nothing */
        break;
    }
  }
}

static void processRawMeshData(void)
{
  uint16_t packetLength = 0U;

  CircularBuffer::Error_t bufferStatus = _circularBuffer.getPacket(&_meshPacketRXBuffer[_meshPacketRXLength],
                                                                   sizeof(_meshPacketRXBuffer),
                                                                   packetLength);

  /* Early return if no packets ready */
  if (bufferStatus != CircularBuffer::ERROR_NONE)
  {
    if (bufferStatus == CircularBuffer::ERROR_NO_EOL_BUFFER_FULL ||
        bufferStatus == CircularBuffer::ERROR_OUTPUT_BUFFER_LENGTH)
    {
      /* Reset the buffer if no valid packet has been found or if
       * the valid packet is too long for the mesh packet buffer */
      _circularBuffer.reset();
    }
  }
  else
  {
    /* Process the packet that has been copied into the mesh packet buffer */
    processEncodedMeshPacket(_meshPacketRXBuffer, packetLength);
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

  else if (sizeof(float) != TYPE_LENGTHS[TYPE_FLOAT])
  {
    return (ERROR_PLATFORM); /* Early Return */
  }

  _memoryMap.noOfDataBlocks = memoryMap;

  Error_t initStatus = _dataBlocks[BLOCK_ID_UNIVERSAL].initDescriptor(BlockUniversal::blockDescriptor);

  for (uint8_t blockIndex = USER_DATA_BLOCK_ID_START; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
  {
    const DataBlock::BlockDescriptor_t &blockDescriptor = memoryMap.blockDescriptors[blockIndex];
          DataBlock                    &block           = _dataBlocks[blockIndex];

    if (initStatus == ERROR_NONE) initStatus = block.initDescriptor(blockDescriptor);
  }

  return (ERROR_NONE);
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

      volatile CoreInitStatus_t status = _controlCoreInitComplete;

      Platform::releaseMemoryLock();
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
  }
  else
  {
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

    if ((blockDescriptor.initDefaults == nullptr) ||
        (blockDescriptor.initLimits   == nullptr) )
    {
      initStatus = ERROR_NULL_PTR;
    }

    if (initStatus == ERROR_NONE) initStatus = blockDescriptor.initDefaults(dataBlock);
    if (initStatus == ERROR_NONE) initStatus = blockDescriptor.initLimits(dataBlock);
  }

  if (initStatus == ERROR_NONE) initStatus = memory

  /* NVM Init Here */

  if (initStatus == ERROR_NONE)
  {
    Platform::acquireMemoryLock();
    _controlCoreInitComplete = CORE_INIT_COMPLETE;
    Platform::releaseMemoryLock();
  }
  else
  {
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

DataBlock * getBlockPointer(const uint8_t blockID)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (nullptr);

  return (&_dataBlocks[blockID]);
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
