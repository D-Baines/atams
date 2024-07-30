/**
  ******************************************************************************
  * @file    Comms.cpp
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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "../../Atams/Node/Comms.hpp"

#include <string.h>

#include "../../Atams/Node/Node.hpp"
#include "../../Atams/Node/Platform.hpp"
#include "../../Atams/Utilities/AtamsUtilities.hpp"
#include "../../Atams/Utilities/ConcurrentSafeQueue.hpp"
#include "../../Atams/Utilities/COBS.hpp"
#include "../../Atams/Utilities/CRC32.hpp"



/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Comms {


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint32_t CRC32_POLYNOMIAL                 = 0x04C11DB7;
static constexpr uint8_t  ABORT_RESPONSE_SIZE              = MESH_SIZE_HEADER + sizeof(Error_t);
static constexpr uint32_t WATCHDOG_INCREMENT_PERIOD_MILLIS = 1U;


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

ConcurrentSafeQueue<ZeroCopyRXMessage_t> _zeroCopyRXQueue(Platform::acquireCommsBufferLock,
                                                          Platform::releaseCommsBufferLock);


/*-- Encode/decode objects ----------------------------------------------------------*/

static CRC32    _crcAtams(CRC32_POLYNOMIAL);

static uint32_t _crcErrorCount  = 0U;
static uint32_t _cobsErrorCount = 0U;


/* RX Mesh packet elements */
static uint8_t  _meshPacketRXBuffer[MAX_MESH_PACKET_SIZE] = {0U};
static uint32_t _meshPacketRXLength;

static bool     _systemIsBigEndian = false;

static uint8_t                 _localNodeID                       = 0x00U;
static uint8_t                 _responseBufferIndex                 = 0U;
static bool                    _responseAborted               = false;
static uint8_t                 _responseBuffer[MAX_NODE_PACKET_SIZE];
static CommsTransmitCallback_t _transmitCallback                  = nullptr;

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void receiveCallback(      uint8_t       *rxBufferPtr,
                            const uint16_t       rxBufferLength,
                            const CommsStorage_t storageMethod)
{
  if ((rxBufferPtr    == nullptr             ) ||
      (rxBufferLength == 0U                  ) ||
      (rxBufferLength >  MAX_MESH_PACKET_SIZE) )
  {
    return;
  }

  switch (storageMethod)
  {
    case COMMS_STORAGE_ZERO_COPY:
    {
      ZeroCopyRXMessage_t queueMessage(rxBufferPtr, rxBufferLength);

      _zeroCopyRXQueue.pushRear(queueMessage);

      break;
    }

    default:
      /* Do Nothing */
      break;
  }
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

  uint32_t packetCRC;

  packetCRC = *reinterpret_cast<uint32_t*>(&decodedPacket[MESH_INDEX_CRC]);

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
  _responseBuffer[MESH_INDEX_MSG_TYPE] = MESSAGE_RESPONSE;
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
                       encodedLength         ) == ERROR_NONE)
  {
    Platform::transmitBuffer(encodedResponseBuffer, encodedLength);
  }
}

/* WARNING - No checks done on datagramHeader sybsystemID or memberIndex.
 *           This function should only ever be called after checks have been completed in DMIB_ProcessNodePacket
 */
static void processDatagramRead(DatagramHeader_t datagramHeader, uint8_t payloadLength)
{
  Error_t transferStatus;

  uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + payloadLength;
  uint16_t remainingBufferLength = static_cast<uint8_t>(sizeof(_responseBuffer)) - _responseBufferIndex;

  if (datagramLength > remainingBufferLength)
  {
    /* fatal error - ping response buffer overflow */
    abortResponse(ERROR_RESPONSE_BUFFER_OVERFLOW);
    return;
  }

  uint8_t datagramHeaderIndex = _responseBufferIndex;

  _responseBufferIndex += DATAGRAM_SIZE_HEADER;

  transferStatus = Node::externalTransfer(ACCESS_READ_ACK,
                                          datagramHeader.blockID,
                                          datagramHeader.memberID,
                                          &_responseBuffer[_responseBufferIndex],
                                          payloadLength);

  if (transferStatus != ERROR_NONE)
  {
    datagramHeader.command = ACCESS_NONE_NACK;
    memcpy(&_responseBuffer[datagramHeaderIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBuffer[_responseBufferIndex] = transferStatus;
    _responseBufferIndex += sizeof(transferStatus);
  }
  else
  {
    datagramHeader.command = ACCESS_READ_ACK;
    memcpy(&_responseBuffer[datagramHeaderIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += payloadLength;
  }
}

/* WARNING - No checks done on datagramHeader subsystemID or memberIndex.
 *           This function should only ever be called after checks have been completed in DMIB_ProcessNodePacket
 */
static void processDatagramWrite(DatagramHeader_t datagramHeader, uint8_t *datagramPayload, uint8_t datagramPayloadLength)
{
  Error_t transferStatus;

  uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + datagramPayloadLength;
  uint16_t remainingBufferLength = static_cast<uint8_t>(sizeof(_responseBuffer)) - _responseBufferIndex;

  if (datagramLength > remainingBufferLength)
  {
    /* fatal error - ping response buffer overflow */
    abortResponse(ERROR_RESPONSE_BUFFER_OVERFLOW);
    return;
  }

  transferStatus = Node::externalTransfer(ACCESS_WRITE_ACK,
                                          datagramHeader.blockID,
                                          datagramHeader.memberID,
                                          datagramPayload,
                                          datagramPayloadLength);

  if (transferStatus != ERROR_NONE)
  {
    //recordError(transferStatus);
    datagramHeader.command = ACCESS_NONE_NACK;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER;
    _responseBuffer[_responseBufferIndex] = transferStatus;
    _responseBufferIndex += sizeof(transferStatus);
  }

  else
  {
    datagramHeader.command = ACCESS_WRITE_ACK;
    memcpy(&_responseBuffer[_responseBufferIndex], &datagramHeader, DATAGRAM_SIZE_HEADER);
    _responseBufferIndex += DATAGRAM_SIZE_HEADER;
  }
}

static inline void processRequestPacket(uint8_t *meshPacket,
                                        uint16_t meshPacketLength)
{
  if (meshPacket == nullptr) return;

  bool             cancelProcessing   = false;
  volatile uint8_t datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= meshPacketLength) &&
         (cancelProcessing                          == false           ) )
  {
    DatagramHeader_t datagramHeader;

    memcpy(&datagramHeader, &meshPacket[datagramStartIndex], DATAGRAM_SIZE_HEADER);

    if (systemIsBigEndian()) swapEndiannessType(datagramHeader);

    DataStatusReturn_t<uint8_t> datagramPayloadLength = Node::getMemberLength(datagramHeader.blockID,
                                                                              datagramHeader.memberID);

    if (datagramPayloadLength.status != ERROR_NONE)
    {
      /* Fatal Error - Data member doesn't exist */
      abortResponse(datagramPayloadLength.status);
      cancelProcessing = true;
      break;
    }

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ_ACK:
        processDatagramRead(datagramHeader, datagramPayloadLength.data);
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;

      case ACCESS_WRITE_ACK:
      {
        uint16_t datagramLength        = DATAGRAM_SIZE_HEADER + datagramPayloadLength.data;
        uint16_t remainingBufferLength = meshPacketLength     - datagramStartIndex;

        if (datagramLength > remainingBufferLength)
        {
          /* Fatal Error - Data member overflows node packet */
          abortResponse(ERROR_DATAGRAM_BUFFER_LENGTH);
          cancelProcessing = true;
        }
        else
        {
          processDatagramWrite(datagramHeader, &meshPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD], datagramPayloadLength.data);
          datagramStartIndex += (DATAGRAM_SIZE_HEADER + datagramPayloadLength.data);
        }

        break;
      }

      case ACCESS_NONE_NACK:
      case ACCESS_FATAL:
      default:
        /* Fatal Error - Inappropriate access command */
        abortResponse(ERROR_ACCESS_INVALID);
        cancelProcessing = true;
        break;
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

static void processEncodedMeshPacket(void)
{
  static uint8_t  decodedPacket[MAX_MESH_PACKET_SIZE];
  static uint8_t  syncPacket[MAX_MESH_PACKET_SIZE];
  static uint16_t decodedLength  = 0U;
  static uint16_t syncLength     = 0U;
  static uint8_t  localSyncCount = 0U;
  static uint8_t  _prevSyncNodeID = NODE_ID_NULL; /* This will need to be expanded to file scope */
  static uint8_t  _finalSyncNodeID = 0U;
  static uint8_t  _firstSyncNodeID = 0U;

  if (decodeMeshPacket(_meshPacketRXBuffer,
                       _meshPacketRXLength,
                       &decodedPacket[0U],
                       sizeof(decodedPacket),
                       decodedLength         ) == ERROR_NONE)
  {
    MessageType_t messageType     = static_cast<MessageType_t>(decodedPacket[MESH_INDEX_MSG_TYPE]);
    uint8_t       packetNodeID    = decodedPacket[MESH_INDEX_NODE_ID];
    uint8_t       packetSyncCount = decodedPacket[MESH_INDEX_SYNC];

    switch (messageType)
    {
      case MESSAGE_REQUEST:
        if (packetNodeID == _localNodeID)
        {
          resetResponse();
          localSyncCount = packetSyncCount;
          if (_localNodeID == _finalSyncNodeID)
          {
            processRequestPacket(decodedPacket, decodedLength);
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
          processRequestPacket(syncPacket, syncLength);
        }
        break;

      case MESSAGE_RESPONSE:
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

static void processRawMeshData(const volatile ZeroCopyRXMessage_t &rxMessage)
{
  if ((_meshPacketRXLength + rxMessage.bufferLength) > MAX_MESH_PACKET_SIZE)
  {
    /* Mesh packet length is greater than maximum - reset buffer */
    _meshPacketRXLength = 0U;
  }

  uint16_t rxChunkStartIndex = 0U;
  uint16_t rxChunkLength     = 0U;

  for (uint16_t index = 0U; index < rxMessage.bufferLength; index++)
  {
    if (rxMessage.bufferPtr[index] == EOL_BYTE)
    {
      rxChunkLength = (index - rxChunkStartIndex) + 1U;

      memcpy(&_meshPacketRXBuffer[_meshPacketRXLength],
             &rxMessage.bufferPtr[rxChunkStartIndex],
             rxChunkLength);

      _meshPacketRXLength += rxChunkLength;
      rxChunkStartIndex   += rxChunkLength;

      processEncodedMeshPacket();

      _meshPacketRXLength = 0U;
    }

    else if (index == (rxMessage.bufferLength - 1U))
    {
      rxChunkLength = (index - rxChunkStartIndex) + 1U;

      memcpy(&_meshPacketRXBuffer[_meshPacketRXLength],
             &rxMessage.bufferPtr[rxChunkStartIndex],
             rxChunkLength);

      _meshPacketRXLength += rxChunkLength;
    }
  }
}


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t init(void)
{
  _systemIsBigEndian = systemIsBigEndian();
  Platform::commsInit();
  //Node::setCommsTransmitCallback(sendMeshPacket);
  Platform::setReceiveCallback(receiveCallback);

  return (ERROR_NONE);
}

void beginUpdateLoop(void)
{
  for(;;)
  {

    uint32_t        currentTime                   = Platform::getMillis();
    static uint32_t previousWatchdogIncrementTime = currentTime;

    if (currentTime - previousWatchdogIncrementTime > WATCHDOG_INCREMENT_PERIOD_MILLIS)
    {
      Node::incrementWatchdog();
      previousWatchdogIncrementTime = currentTime;
    }

    ConcurrentSafeQueue<ZeroCopyRXMessage_t>::Return_t zeroCopyQueueReturn = _zeroCopyRXQueue.getFront();

    if (zeroCopyQueueReturn.status == ConcurrentSafeQueue<ZeroCopyRXMessage_t>::ERROR_NONE)
    {
      processRawMeshData(zeroCopyQueueReturn.data);
      _zeroCopyRXQueue.popFront();
    }
  }
}


} } /* End Atams::Comms Namespace */

/**
  * @}End of File
  */
