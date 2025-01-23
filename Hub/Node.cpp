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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "string.h"
#include "Node.hpp"
#include "Bus.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Devices/DataBlockUniversal.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Node::Node(Bus &bus, uint8_t nodeID) :
_bus(bus),
_nodeID(nodeID)
{
  _bus.addNodeToBus(*this);
}

Atams::Error_t Node::init(const MemoryMap_t &memoryMap)
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

  for (uint8_t blockIndex = 0U; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
  {
    const DataBlock::BlockDescriptor_t &blockDescriptor = memoryMap.blockDescriptors[blockIndex];
          DataBlock                    &block           = _dataBlocks[blockIndex];

    if (initStatus == ERROR_NONE) initStatus = block.initDescriptor(blockDescriptor);
  }

  //if (initStatus == ERROR_NONE)
  //{
  //  if (_memoryMap.initUniversalData != nullptr) initStatus = _memoryMap.initUniversalData(*this);
  //  else                                         initStatus = ERROR_NULL_PTR;
  //}

  if (initStatus != ERROR_NONE)
  {
    _memoryMap.noOfDataBlocks = 0U;
    for (DataBlock &dataBlock : _dataBlocks) dataBlock.deinit();
  }

  return (initStatus);
}

template <typename T>
Atams::Error_t Node::write(const uint8_t  blockID,
                           const uint16_t varID,
                           const T        writeData)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID); 
  }

  return (_dataBlocks[blockID].write(varID, writeData));
}

template Atams::Error_t Node::write<uint8_t >(const uint8_t blockID, const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t Node::write<int8_t  >(const uint8_t blockID, const uint16_t varID, const int8_t   writeData);
template Atams::Error_t Node::write<uint16_t>(const uint8_t blockID, const uint16_t varID, const uint16_t writeData);
template Atams::Error_t Node::write<int16_t >(const uint8_t blockID, const uint16_t varID, const int16_t  writeData);
template Atams::Error_t Node::write<uint32_t>(const uint8_t blockID, const uint16_t varID, const uint32_t writeData);
template Atams::Error_t Node::write<int32_t >(const uint8_t blockID, const uint16_t varID, const int32_t  writeData);
template Atams::Error_t Node::write<float   >(const uint8_t blockID, const uint16_t varID, const float    writeData);

template <typename T>
Atams::Error_t Node::read(const uint8_t   blockID,
                          const uint16_t  varID,
                                T        &readData)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID); 
  }

  return (_dataBlocks[blockID].read(varID, readData));
}

template Atams::Error_t Node::read<uint8_t >(const uint8_t blockID, const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::read<int8_t  >(const uint8_t blockID, const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::read<uint16_t>(const uint8_t blockID, const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::read<int16_t >(const uint8_t blockID, const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::read<uint32_t>(const uint8_t blockID, const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::read<int32_t >(const uint8_t blockID, const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::read<float   >(const uint8_t blockID, const uint16_t varID, float    &readData);

template <typename T>
Atams::Error_t Node::readIfNew(const uint8_t   blockID,
                               const uint16_t  varID,
                                     T        &readData)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID); 
  }

  return (_dataBlocks[blockID].readIfNew(varID, readData));
}

template Atams::Error_t Node::readIfNew<uint8_t >(const uint8_t blockID, const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::readIfNew<int8_t  >(const uint8_t blockID, const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::readIfNew<uint16_t>(const uint8_t blockID, const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::readIfNew<int16_t >(const uint8_t blockID, const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::readIfNew<uint32_t>(const uint8_t blockID, const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::readIfNew<int32_t >(const uint8_t blockID, const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::readIfNew<float   >(const uint8_t blockID, const uint16_t varID, float    &readData);

DataBlock * Node::getBlockPtr(const uint8_t blockID)
{
  if (blockID < _memoryMap.noOfDataBlocks)
  {
    return (&_dataBlocks[blockID]);
  }

  return (nullptr);
}

Atams::Error_t Node::externalTransfer(const Access_t  accessRequest,
                                      const uint8_t   blockID,
                                      const uint16_t  varID,
                                      uint8_t * const dataStoragePtr,
                                      const uint8_t   length)
{
  if (blockID  >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  return (_dataBlocks[blockID].externalTransfer(accessRequest, varID, dataStoragePtr, length));
}

DataStatusReturn_t<uint8_t> Node::getMemberLength(const uint8_t blockID, const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    lengthReturn.status = ERROR_BLOCK_ID;
    return (lengthReturn);
  }

  return (_dataBlocks[blockID].getMemberLength(varID));
}

Atams::Error_t Node::setRequestPattern(const uint8_t          blockID,
                                       const uint16_t         varID,
                                       const Access_t         accessRequest,
                                       const RequestPattern_t requestPattern)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  Atams::Error_t           statusReturn  = Atams::ERROR_NONE;
  DataStatusReturn_t<bool> requestReturn = _dataBlocks[blockID].setRequestPattern(varID, accessRequest, requestPattern);

  if ((requestReturn.status == Atams::ERROR_NONE) &&
      (requestReturn.data   == true             ) )
  {
    /* TODO:: Handle situation when process packet change fails but setRequestPattern does not */
    statusReturn = processRequestPacketChange(blockID,
                                              varID,
                                              accessRequest, 
                                              requestPattern);
  }
  else
  {
    statusReturn = requestReturn.status;
  }
  
  return (statusReturn);
}

Atams::Error_t Node::getRequestPattern(const uint8_t    blockID,
                                       const uint16_t   varID,
                                       Access_t        &accessRequest,
                                      RequestPattern_t &requestPattern)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  return (_dataBlocks[blockID].getRequestPattern(varID, accessRequest, requestPattern));
}

Atams::Error_t Node::setRequestPatternNoChecks(const uint8_t          blockID,
                                               const uint16_t         varID,
                                               const Access_t         accessRequest,
                                               const RequestPattern_t requestPattern)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  Atams::Error_t           statusReturn  = Atams::ERROR_NONE;
  DataStatusReturn_t<bool> requestReturn = _dataBlocks[blockID].setRequestPattern(varID, accessRequest, requestPattern);

  if ((requestReturn.status == Atams::ERROR_NONE) &&
      (requestReturn.data   == true             ) )
  {
    /* TODO:: Handle situation when process packet change fails but setRequestPattern does not */
    statusReturn = processRequestPacketChange(blockID,
                                              varID,
                                              accessRequest, 
                                              requestPattern);
  }
  else
  {
    statusReturn = requestReturn.status;
  }
  
  return (statusReturn);
}

uint8_t Node::getNodeID(void)
{
  return (_nodeID);
}

Atams::Error_t Node::getLatestError(void)
{
  return (_latestError);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::Error_t Node::getEncodedRequestPacket(uint8_t  *outputBuffer,
                                             uint16_t  outputBufferMaxLength, 
                                             uint16_t &outputLength)
{
  Platform::MemoryLock::acquireLock();

  updateRequestPacketWriteData();
  _requestPacket.buffer[MESH_INDEX_MSG_TYPE] = Atams::MESSAGE_REQUEST_SYNCED;
  _requestPacket.buffer[MESH_INDEX_NODE_ID ] = _nodeID;

  Atams::Error_t statusReturn = encodeMeshPacket(_requestPacket.buffer, 
                                                 _requestPacket.length, 
                                                 outputBuffer, 
                                                 outputBufferMaxLength, 
                                                 outputLength);

  Platform::MemoryLock::releaseLock();

  return (statusReturn);
}

Atams::Error_t Node::responseReceived(uint8_t *inputBuffer, uint16_t inputLength)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((inputBuffer != nullptr                ) &&
      (inputLength <= sizeof(_responseBuffer)) ) 
  {
    memcpy(_responseBuffer, inputBuffer, inputLength);
    _responseLength = inputLength;
  }
  else
  {
    statusReturn = Atams::ERROR_RESPONSE_BUFFER_LENGTH; //TODO:: Review error
  }

  return (statusReturn);
}

void Node::flagNoResponse(void)
{
  _latestError = ERROR_NO_RESPONSE;
}

void Node::processAbortedResponse(void)
{
  if ((_responseLength != (MESH_SIZE_HEADER + sizeof(Atams::Error_t))) ||
      (_responseBuffer[MESH_SIZE_HEADER] >= NUMBER_OF_ATAMS_ERRORS   ) ) 
  {
    _latestError = ERROR_ABORT_FAILURE;
  }
  else
  {
    _latestError = static_cast<Atams::Error_t>(_responseBuffer[MESH_SIZE_HEADER]);
  }
}

Atams::Error_t Node::processResponseBuffer(void)
{
  Atams::Error_t              statusReturn     = Atams::ERROR_NONE;
  bool                        cancelProcessing = false;
  uint8_t                     datagramIndex    = MESH_INDEX_FIRST_DATAGRAM;
  Error_t                     transferStatus   = ERROR_NONE;
  DatagramHeader_t            datagramHeader;
  DataStatusReturn_t<uint8_t> datagramPayloadLength;

  if (_responseBuffer[MESH_INDEX_MSG_TYPE] == MESSAGE_ABORTED_RESPONSE)
  {
    processAbortedResponse();
    return (_latestError); /* Early Return */
  }

  while ((datagramIndex + DATAGRAM_SIZE_HEADER <= _responseLength) &&
         (cancelProcessing                     == false          ) )
  {
    bufferToDatagramHeader(&_responseBuffer[datagramIndex], datagramHeader);

    if (datagramHeader.blockID >= _memoryMap.noOfDataBlocks)
    {
      statusReturn = ERROR_BLOCK_ID;
      break;
    }

    if (datagramHeader.blockID == BLOCK_ID_ERROR_INDICATOR)
    {
      datagramIndex += DATAGRAM_SIZE_HEADER;
      statusReturn = static_cast<Atams::Error_t>(_responseBuffer[datagramIndex]);
      break;
    }

    DataBlock &datablock = _dataBlocks[datagramHeader.blockID];
  
    datagramPayloadLength = datablock.getMemberLength(datagramHeader.varID);
  
    if (datagramPayloadLength.status != ERROR_NONE)
    {
      statusReturn = datagramPayloadLength.status;
      break;
    }

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {      
      case RESPONSE_ACK_READ:
      {
        transferStatus = datablock.externalTransfer(ACCESS_WRITE,
                                                    datagramHeader.varID,
                                                    &_responseBuffer[datagramIndex + DATAGRAM_INDEX_PAYLOAD],
                                                    datagramPayloadLength.data);

        if (transferStatus == ERROR_NONE) updateRequestPattern(datagramHeader.blockID, datagramHeader.varID, ACCESS_READ);
        else                              statusReturn = transferStatus;

        datagramIndex += (DATAGRAM_SIZE_HEADER + datagramPayloadLength.data);
        break;
      }

      case RESPONSE_ACK_WRITE:
        updateRequestPattern(datagramHeader.blockID, datagramHeader.varID, ACCESS_WRITE);
        datagramIndex += DATAGRAM_SIZE_HEADER;
        break;

      default:
        _latestError = ERROR_ERROR_MANAGEMENT;
        cancelProcessing = true;
        break;
    }
  }

  return (statusReturn);
}

DataStatusReturn_t<bool> Node::findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig)
{
  DataStatusReturn_t<bool> searchResult = 
  {
    .data   = false,
    .status = ERROR_NONE,
  };

  changeConfig.datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while (changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER <= _requestPacket.length)    
  {
    bufferToDatagramHeader(&_requestPacket.buffer[changeConfig.datagramStartIndex], changeConfig.currentDatagramHeader);

    changeConfig.currentDatagramLength = DATAGRAM_SIZE_HEADER;

    if (changeConfig.currentDatagramHeader.command == ACCESS_WRITE)
    {
      DataStatusReturn_t<uint8_t> payloadLengthReturn = getMemberLength(changeConfig.currentDatagramHeader.blockID, 
                                                                        changeConfig.currentDatagramHeader.varID);
  
      if (payloadLengthReturn.status != ERROR_NONE)
      {
        searchResult.status = payloadLengthReturn.status;
        return (searchResult);
      }
  
      changeConfig.currentDatagramLength += payloadLengthReturn.data;
    }

    if ((changeConfig.currentDatagramHeader.blockID == changeConfig.newDatagramHeader.blockID) &&
        (changeConfig.currentDatagramHeader.varID   == changeConfig.newDatagramHeader.varID  ) )
    {
      searchResult.data = true;
      return (searchResult);
    }
    else /* Current datagram != new datagram */
    {
      changeConfig.datagramStartIndex += changeConfig.currentDatagramLength;
    }
  }

  return (searchResult);
}

Atams::Error_t Node::requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength)
{  
  if ((_requestPacket.length + shiftLength) > MAX_MESH_PACKET_SIZE)
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH);
  }

  memmove(&_requestPacket.buffer[shiftIndex + shiftLength], 
          &_requestPacket.buffer[shiftIndex], 
          (_requestPacket.length - shiftIndex));

  _requestPacket.length += shiftLength;

  _requestPacket.writeList.updateIndexes(shiftIndex, shiftLength);

  return (ERROR_NONE);
}

Atams::Error_t Node::requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  /* Node packet still contains other datagrams - only remove datagram from Mesh packet */
  uint16_t shiftIndex  =  changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength = -changeConfig.currentDatagramLength;
    
  Error_t statusReturn = requestPacketShift(shiftIndex, shiftLength);

  if (statusReturn != ERROR_NONE) return (statusReturn); /* Early Return */
  
  if (changeConfig.currentDatagramHeader.command == ACCESS_WRITE)
  {
    WriteList::WriteConfig_t writeConfigToRemove =
    {
      .blockID = changeConfig.currentDatagramHeader.blockID,
      .varID   = changeConfig.currentDatagramHeader.varID, 
    };

    _requestPacket.writeList.removeConfigIfFound(writeConfigToRemove);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t  statusReturn = ERROR_NONE;
  uint16_t shiftIndex   = changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength  = changeConfig.newDatagramLength  - changeConfig.currentDatagramLength;

  WriteList::WriteConfig_t writeConfig =
  {
    /*.blockID             = */ changeConfig.currentDatagramHeader.blockID,
    /*.varID               = */ changeConfig.currentDatagramHeader.varID, 
    /*.meshPacketDataIndex = */ static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER),
    /*.dataLength          = */ static_cast<uint8_t> (changeConfig.newDatagramLength  - DATAGRAM_SIZE_HEADER)
  };

  if ((changeConfig.accessRequest  == ACCESS_WRITE  ) &&
      (changeConfig.requestPattern == REQUEST_ACTIVE) )
  {
    if (_requestPacket.writeList.addConfig(writeConfig) != WriteList::ERROR_NONE) statusReturn = Atams::ERROR_WRITE_LIST; 
  }
  else
  {
    _requestPacket.writeList.removeConfigIfFound(writeConfig);
  }

  if ((statusReturn == Atams::ERROR_NONE) &&
      (shiftLength  != 0                ) )
  { 
    statusReturn = requestPacketShift(shiftIndex, shiftLength);
  }

  if ((statusReturn                != Atams::ERROR_NONE) &&
      (changeConfig.accessRequest  == ACCESS_WRITE     ) &&
      (changeConfig.requestPattern == REQUEST_ACTIVE   ) )
  {
    _requestPacket.writeList.removeConfigIfFound(writeConfig);
  }
  else
  {
      /* Copy new datagram into available space */
    memcpy(&_requestPacket.buffer[changeConfig.datagramStartIndex], 
           changeConfig.newDatagramBuffer, 
           changeConfig.newDatagramLength);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig)
{
  if ((_requestPacket.length + changeConfig.newDatagramLength) > MAX_NODE_PACKET_SIZE)
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  changeConfig.datagramStartIndex = _requestPacket.length;

  if ((changeConfig.accessRequest  == ACCESS_WRITE  ) &&
      (changeConfig.requestPattern == REQUEST_ACTIVE) )
  {
    WriteList::WriteConfig_t writeConfigToAdd =
    {
      .blockID             = changeConfig.newDatagramHeader.blockID,
      .varID               = changeConfig.newDatagramHeader.varID, 
      .meshPacketDataIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER),
      .dataLength          = static_cast<uint8_t> (changeConfig.newDatagramLength  - DATAGRAM_SIZE_HEADER)
    };

    if (_requestPacket.writeList.addConfig(writeConfigToAdd) != WriteList::ERROR_NONE)
    {
      return (ERROR_WRITE_LIST);
    }
  }

  /* Copy new datagram into available space */
  memcpy(&_requestPacket.buffer[_requestPacket.length], changeConfig.newDatagramBuffer, changeConfig.newDatagramLength);

  /* Update Node packet length in Mesh packet */
  _requestPacket.length += changeConfig.newDatagramLength;

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::constructDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t statusReturn = ERROR_NONE;

  /* Construct new datagram to be added to mesh packet */
  changeConfig.newDatagramHeader.command  = changeConfig.accessRequest;
  changeConfig.newDatagramHeader.blockID  = changeConfig.blockID;
  changeConfig.newDatagramHeader.varID    = changeConfig.varID;

  datagramHeaderToBuffer(changeConfig.newDatagramHeader, changeConfig.newDatagramBuffer);

  changeConfig.newDatagramLength = DATAGRAM_SIZE_HEADER;

  if (changeConfig.accessRequest == ACCESS_WRITE)
  {
    DataStatusReturn_t<uint8_t> datagramPayloadLength = getMemberLength(changeConfig.blockID, changeConfig.varID);
  
    if (datagramPayloadLength.status != ERROR_NONE)
    {
      return (datagramPayloadLength.status);
    }

    statusReturn = externalTransfer(ACCESS_READ, 
                                    changeConfig.blockID, 
                                    changeConfig.varID, 
                                    &changeConfig.newDatagramBuffer[DATAGRAM_INDEX_PAYLOAD], 
                                    datagramPayloadLength.data);

    if (statusReturn != ERROR_NONE)
    {
      return (statusReturn);
    }
   
    changeConfig.newDatagramLength += datagramPayloadLength.data;
  }

  return (statusReturn);
}

/* Warning - No OOR checks, should be completed by calling function */
Atams::Error_t Node::processRequestPacketChange(const uint8_t          blockID,
                                                const uint16_t         varID,
                                                const Access_t         accessRequest,
                                                const RequestPattern_t requestPattern)
{
  RequestChangeConfig_t packetChangeConfig;
  Error_t               statusReturn = ERROR_NONE;
  packetChangeConfig.accessRequest   = accessRequest;
  packetChangeConfig.requestPattern  = requestPattern;
  packetChangeConfig.blockID         = blockID;
  packetChangeConfig.varID           = varID;

  statusReturn = constructDatagram(packetChangeConfig);

  if (statusReturn != ERROR_NONE)
  {
    return (statusReturn);
  }

  /* Begin editing of request packet */
  Platform::MemoryLock::acquireLock();

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramSearchResult = findDatagramMatchInPacket(packetChangeConfig);
  
  if (datagramSearchResult.status == ERROR_NONE)
  {
    if ((requestPattern == REQUEST_INACTIVE) ||
        (accessRequest  == ACCESS_NONE     ) )
    {
      if (datagramSearchResult.data == true) 
      {
        statusReturn = requestPacketRemoveCurrentDatagram(packetChangeConfig);
      }
    }
    else /* commandPattern != COMMAND_INACTIVE && accessRequest != ACCESS_NONE */
    {
      if (datagramSearchResult.data == true)
      {
        statusReturn = requestPacketAdjustCurrentDatagram(packetChangeConfig);
      }
      else                                   
      {
        statusReturn = requestPacketAppendDatagram(packetChangeConfig);
      }
    }
  }
  else
  {
    statusReturn = datagramSearchResult.status;
  }

  Platform::MemoryLock::releaseLock();

  return (statusReturn);
}

Atams::Error_t Node::updateRequestPattern(const uint8_t  blockID,
                                          const uint16_t varID,
                                          const Access_t accessRequest)
{
  if (blockID > _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  Atams::Error_t           statusReturn = Atams::ERROR_NONE;
  DataStatusReturn_t<bool> updateStatus = _dataBlocks[blockID].updateRequestPattern(varID);

  if (updateStatus.status != Atams::ERROR_NONE)
  {
    return (updateStatus.status);
  }

  if (updateStatus.data == true)
  {
    statusReturn = processRequestPacketChange(blockID,
                                              varID,
                                              accessRequest, 
                                              REQUEST_INACTIVE);
  }

  return (statusReturn);
}

/* Mesh Packet access must be properly locked before using this function */
void Node::updateRequestPacketWriteData(void)
{
  //Error_t             statusReturn = ERROR_NONE;
  //WriteList::Return_t listReturn;
//
  //uint16_t writeListLength = _requestPacket.writeList.getConfigCount();
  //
  //for (uint16_t writeListIndex = 0U; writeListIndex < writeListLength; writeListIndex++)
  //{
  //  listReturn = _requestPacket.writeList.getConfigAtIndex(writeListIndex);
  //  
  //  if (listReturn.status == WriteList::ERROR_NONE)
  //  {
  //    static_cast<void>(externalTransfer(ACCESS_READ,
  //                                       listReturn.writeConfig.blockID, 
  //                                       listReturn.writeConfig.varID, 
  //                                       &_requestPacket.buffer[listReturn.writeConfig.meshPacketDataIndex], 
  //                                       listReturn.writeConfig.dataLength));
  //  }
  //}
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


