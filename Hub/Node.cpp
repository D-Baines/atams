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

Node::Node(Bus &bus) :
_bus(bus)
{
  _activePacketPtr   = &_primaryPacket;
  _inactivePacketPtr = &_secondaryPacket;
  _bus.addNodeToBus(*this);
}

Error_t Node::init(const MemoryMap_t &memoryMap)
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

  _memoryMap.noOfDataBlocks = memoryMap.noOfDataBlocks;

  Error_t initStatus = ERROR_NONE;

  for (uint8_t blockIndex = 0U; blockIndex <= Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
  {
    const DataBlock::BlockDescriptor_t &blockDescriptor = memoryMap.blockDescriptors[blockIndex];
          DataBlock                    &block           = _dataBlocks[blockIndex];

    if (initStatus == ERROR_NONE) initStatus = block.initDescriptor(blockDescriptor);
  }

  if (initStatus != ERROR_NONE)
  {
    _memoryMap.noOfDataBlocks = 0U;
    for (DataBlock &dataBlock : _dataBlocks) dataBlock.deinit();
  }

  return (initStatus);
}

template <typename T>
Error_t Node::write(const uint8_t  blockID,
                    const uint16_t varID,
                    const T        writeData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].write(varID, writeData));
}

template Error_t Node::write<uint8_t >(const uint8_t blockID, const uint16_t varID, const uint8_t  writeData);
template Error_t Node::write<int8_t  >(const uint8_t blockID, const uint16_t varID, const int8_t   writeData);
template Error_t Node::write<uint16_t>(const uint8_t blockID, const uint16_t varID, const uint16_t writeData);
template Error_t Node::write<int16_t >(const uint8_t blockID, const uint16_t varID, const int16_t  writeData);
template Error_t Node::write<uint32_t>(const uint8_t blockID, const uint16_t varID, const uint32_t writeData);
template Error_t Node::write<int32_t >(const uint8_t blockID, const uint16_t varID, const int32_t  writeData);
template Error_t Node::write<float   >(const uint8_t blockID, const uint16_t varID, const float    writeData);


template <typename T>
Error_t Node::read(const uint8_t   blockID,
                   const uint16_t  varID,
                         T        &readData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].read(varID, readData));
}

template Error_t Node::read<uint8_t >(const uint8_t blockID, const uint16_t varID, uint8_t  &readData);
template Error_t Node::read<int8_t  >(const uint8_t blockID, const uint16_t varID, int8_t   &readData);
template Error_t Node::read<uint16_t>(const uint8_t blockID, const uint16_t varID, uint16_t &readData);
template Error_t Node::read<int16_t >(const uint8_t blockID, const uint16_t varID, int16_t  &readData);
template Error_t Node::read<uint32_t>(const uint8_t blockID, const uint16_t varID, uint32_t &readData);
template Error_t Node::read<int32_t >(const uint8_t blockID, const uint16_t varID, int32_t  &readData);
template Error_t Node::read<float   >(const uint8_t blockID, const uint16_t varID, float    &readData);

DataBlock * Node::getBlockPtr(const uint8_t blockID)
{
  if (blockID < _memoryMap.noOfDataBlocks)
  {
    return (&_dataBlocks[blockID]);
  }

  return (nullptr);
}

Error_t Node::externalTransfer(const Access_t  accessRequest,
                               const uint8_t   blockID,
                               const uint16_t  varID,
                               uint8_t * const dataStoragePtr,
                               const uint8_t   length)
{
  if (blockID  >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

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

Error_t Node::setRequestPattern(const uint8_t          blockID,
                                const uint16_t         varID,
                                const Access_t         accessRequest,
                                const RequestPattern_t requestPattern)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  Error_t                  meshChangeReturn;
  DataStatusReturn_t<bool> requestReturn = _dataBlocks[blockID].setRequestPattern(varID, accessRequest, requestPattern);

  if (requestReturn.data == true)
  {

    meshChangeReturn = processPacketChange(blockID,
                                           varID,
                                           accessRequest, 
                                           requestPattern);
  }
  
  return (meshChangeReturn);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void Node::copyToResponseBuffer(uint8_t *buffer, uint16_t length)
{
  if (length <= sizeof(_responseBuffer)) 
  {
    memcpy(_responseBuffer, buffer, length);
    _responseBufferLength = length;
  }
}

void Node::processNodePacket(uint8_t *buffer, uint16_t length)
{
  bool                        cancelProcessing = false;
  uint8_t                     datagramIndex    = MESH_INDEX_FIRST_DATAGRAM;
  uint8_t                     nodeID           = buffer[MESH_INDEX_NODE_ID];
  Error_t                     transferStatus   = ERROR_NONE;
  MessageType_t               messageType      = static_cast<MessageType_t>(buffer[MESH_INDEX_MSG_TYPE]);
  DatagramHeader_t            datagramHeader;
  DataStatusReturn_t<uint8_t> datagramPayloadLength;

  if (messageType != MESSAGE_REQUEST_SYNCED)
  {
    /* Register error and return early */
    return;
  }

  while ((datagramIndex + DATAGRAM_SIZE_HEADER <= length) &&
         (cancelProcessing                     == false ) )
  {
    bufferToDatagramHeader(&buffer[datagramIndex], datagramHeader);

    if (datagramHeader.blockID == BLOCK_ID_ERROR_INDICATOR ||
        datagramHeader.blockID >= _memoryMap.noOfDataBlocks)
    {
      datagramIndex += DATAGRAM_SIZE_HEADER;
      Error_t error = static_cast<Error_t>(buffer[datagramIndex]);
      //recordError(ERROR_PACKET_PROCESSING);
      break;
    }

    DataBlock &datablock = _dataBlocks[datagramHeader.blockID];
  
    datagramPayloadLength = datablock.getMemberLength(datagramHeader.varID);
  
    if (datagramPayloadLength.status != ERROR_NONE)
    {
      //recordError(ERROR_PACKET_PROCESSING);
      break;
    }

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {      
      case RESPONSE_NACK:
      {
        datagramIndex += DATAGRAM_SIZE_HEADER;
        Error_t accessError = static_cast<Error_t>(buffer[datagramIndex]);
        //recordError(ERROR_PACKET_PROCESSING);
        datagramIndex += sizeof(accessError);
        break;
      }

      case RESPONSE_ACK_READ:
      {
        /* Mesh packet data is sent with little endian byte order, order must be swapped when data enters/exits the mesh packet on big endian systems */
        if (systemIsBigEndian()) swapEndiannessRaw(&buffer[datagramIndex + DATAGRAM_INDEX_PAYLOAD], datagramPayloadLength.data);

        transferStatus = datablock.externalTransfer(ACCESS_WRITE,
                                                    datagramHeader.varID,
                                                    &buffer[datagramIndex + DATAGRAM_INDEX_PAYLOAD],
                                                    datagramPayloadLength.data);

        if (transferStatus == ERROR_NONE)
        {
          updateCommandPattern(ACCESS_READ, nodeID, datagramHeader.blockID, datagramHeader.varID);
        }
        else 
        {
          //recordError(ERROR_PACKET_PROCESSING);
        }

        datagramIndex += (DATAGRAM_SIZE_HEADER + datagramPayloadLength.data);
        
        break;
      }

      case RESPONSE_ACK_WRITE:
        updateCommandPattern(ACCESS_WRITE, nodeID, datagramHeader.blockID, datagramHeader.varID);
        datagramIndex += DATAGRAM_SIZE_HEADER;
        break;

      case RESPONSE_FATAL:
        /* Fatal Error - Inappropriate access command received by node */
      default:
        /* Fatal Error - Inappropriate response */
        //recordError(ERROR_COMMAND_RESPONSE_INVALID);
        cancelProcessing = true;
        break;
    }
  }
}

void Node::swapAndProcessBuffers(void)
{
  Platform::MemoryLock::acquireLock();
  
  /* Swap the inactive/active mesh packet pointers */
  Packet_t* tempPacketPtr = _activePacketPtr;
  _activePacketPtr        = _inactivePacketPtr;
  _inactivePacketPtr      = tempPacketPtr;

  /* Copy contents of the now active mesh packet into the now inactive mesh packet for processing and editing */
  *_inactivePacketPtr = *_activePacketPtr;

  processNodePacket(_responseBuffer, _responseBufferLength);

  Platform::MemoryLock::releaseLock();
}

/* Warning - No OOR checks, should be completed before calling this function */
Error_t Node::processPacketChange(const uint8_t          blockID,
                                  const uint16_t         varID,
                                  const Access_t         accessRequest,
                                  const RequestPattern_t requestPattern)
{
  PacketChangeConfig_t packetChangeConfig;

  packetChangeConfig.accessRequest  = accessRequest;
  packetChangeConfig.commandPattern = commandPattern;
  packetChangeConfig.nodeType       = _meshNodes[nodeID].getNodeType();
  packetChangeConfig.nodeID         = nodeID;
  packetChangeConfig.fieldID        = fieldID;
  packetChangeConfig.memberID       = memberID;

  MeshPacket_t *meshPacket = _inactiveMeshPacketPtr;
  Error_t   statusReturn;

  statusReturn = meshPacketConstructDatagram(meshChangeConfig);

  if (statusReturn != ERROR_NONE)
  {
    return (statusReturn);
  }

  /* Begin editing of inactive mesh packet */
  Platform::MemoryLock::acquireLock();

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramSearchResult = packetFindDatagramMatch(packetChangeConfig);
  
  if (datagramSearchResult.status != ERROR_NONE)
  {
    return (datagramSearchResult.status);
  }

  if (datagramSearchResult.data == true)
  { /* Datagram already exists in Node packet but either needs editing or removing */

    if (requestPattern == REQUEST_INACTIVE)
    { /* Datagram needs to be removed from Mesh packet */

      statusReturn = packetRemoveCurrentDatagram(packetChangeConfig);

      if (statusReturn != ERROR_NONE)
      {
        return (statusReturn);
      }

    }
    else /* commandPattern != COMMAND_INACTIVE */
    {
      statusReturn = packetAdjustCurrentDatagram(packetChangeConfig);

      if (statusReturn != ERROR_NONE)
      {
        return (statusReturn);
      }
    }
  }
  else 
  { /* No matching datagram already in Node/Mesh packet */
    statusReturn = packetAppendDatagramToNode(packetChangeConfig);

    if (statusReturn != ERROR_NONE)
    {
      return (statusReturn);
    }
  }

  Platform::MemoryLock::releaseLock();

  return (ERROR_NONE);
}

/* MESH CONTROLLER EXAMPLES*/

Error_t MeshController::setCommandPattern(const Access_t         accessRequest,
                                          const CommandPattern_t commandPattern,
                                          const uint8_t          nodeID,
                                          const uint8_t          fieldID, 
                                          const uint16_t         memberID)
{
  DataStatusReturn_t<bool> commandReturn;
  bool                     commandChanged = &commandReturn.data;
  Error_t                  meshChangeReturn;

  if (nodeID > NODE_ID_MAX)
  {
    commandReturn.status = recordError(ERROR_NODE_ID_OOR);

    return (commandReturn.status);
  }

  commandReturn = _meshNodes[nodeID].setCommandPattern(accessRequest,
                                                       commandPattern,
                                                       fieldID,
                                                       memberID);

  /* If setMemberCommand returned an error or the new command was not different from the last, return */
  if (commandReturn.status != ERROR_NONE)
  {
    recordError(commandReturn.status);
    return (commandReturn.status);
  }

  if (commandChanged == false)
  {
    return (commandReturn.status);
  }

  meshChangeReturn = processMeshPacketChange(accessRequest,
                                             commandPattern,
                                             nodeID,
                                             fieldID, 
                                             memberID);
  
  return (meshChangeReturn);
}

void MeshController::updateCommandPattern(const Access_t  access,
                                          const uint8_t   nodeID,
                                          const uint8_t   fieldID, 
                                          const uint16_t  memberID)
{ 
  if (nodeID > NODE_ID_MAX)
  {
    recordError(ERROR_NODE_ID_OOR);
    return;
  }

  Error_t                          meshChangeStatus;
  DataStatusReturn_t<CommandPattern_t> commandPattern;
  
  commandPattern = _meshNodes[nodeID].getCommandPattern(access,
                                                        fieldID,
                                                        memberID);

  
  if (commandPattern.status != ERROR_NONE)
  {
    recordError(ERROR_PATTERN_AUTO_UPDATE);
    return;
  }

  if (commandPattern.data == COMMAND_UNTIL_ACK)
  {
    meshChangeStatus = setCommandPattern(access, COMMAND_INACTIVE, nodeID, fieldID, memberID);

    if (meshChangeStatus != ERROR_NONE)
    {
      recordError(ERROR_PATTERN_AUTO_UPDATE);
    }
  }
}

void MeshController::processPingResponse(uint8_t *pingResponseBuffer, const uint16_t pingResponseLength)
{
  if (pingResponseBuffer == nullptr)
  {
    recordError(ERROR_NULL_PTR);
    return;
  }

  if (pingResponseLength < MESH_SIZE_HEADER + NODE_SIZE_HEADER) 
  {
    recordError(ERROR_MESH_BUFFER_LENGTH);
    return;
  }

  const uint8_t   syncCount             = pingResponseBuffer[MESH_INDEX_SYNC];
  const uint8_t   nodeID                = pingResponseBuffer[MESH_INDEX_FIRST_NODE + NODE_INDEX_ID];
  const uint16_t  nodeType              = pingResponseBuffer[MESH_INDEX_FIRST_NODE + NODE_INDEX_TYPE];
  const uint8_t   nodePacketLength      = pingResponseBuffer[MESH_INDEX_FIRST_NODE + NODE_INDEX_LENGTH];
  const uint16_t  remainingBufferLength = pingResponseLength - MESH_INDEX_FIRST_NODE;
        Node      &node                  = _meshNodes[nodeID];

  if (nodeID > NODE_ID_MAX)
  {
    recordError(ERROR_NODE_ID_OOR);
    return;
  }

  if ((nodePacketLength > remainingBufferLength ) ||
      (nodePacketLength < NODE_SIZE_HEADER      ) )
  {
    node.recordError(ERROR_NODE_BUFFER_LENGTH);
    recordError(ERROR_PACKET_PROCESSING);
    return;
  }

  std::unique_lock<std::mutex> lock(_meshPacketAccess);

  //if (syncCount != _activeMeshPacketPtr->syncCount)
  //{
  //  /* Received mesh packet is out of date */
  //  node.recordError(ERROR_SYNC_COUNT);
  //  recordError(ERROR_PACKET_PROCESSING);
  //  return;
  //}   

  NodeType_t internalNodeType = _meshNodes[nodeID].getNodeType();

  if ((internalNodeType != NODE_TYPE_UNIVERSAL) &&
      (nodeType         != internalNodeType   ) )
  {
    /* Received node packet was sent from an unexpected node type */
    node.recordError(ERROR_NODE_TYPE);
    recordError(ERROR_PACKET_PROCESSING);
    return;
  }
  
  if (nodeID != _activeMeshPacketPtr->pingResponseCount)
  {
    /* Received node packet was sent from the unexpected node ID */
    recordError(ERROR_SYNC_NODE);
    return;
  }

  meshUpdateCycleStep();

  _meshPacketAccess.unlock();

  processNodePacket(&pingResponseBuffer[MESH_INDEX_FIRST_NODE], nodePacketLength);
}

/* Warning - nullptr, length, nodeType, nodeID checks not performed, completed in processPingResponse */
void MeshController::processNodePacket(uint8_t *nodePacket, const uint8_t nodePacketLength)
{
  bool                        cancelProcessing = false;
  uint8_t                     datagramIndex    = NODE_INDEX_FIRST_DATAGRAM;
  uint8_t                     nodeID           = nodePacket[NODE_INDEX_ID];
  Error_t                     transferStatus   = ERROR_NONE;
  DatagramHeader_t            datagramHeader;
  DataStatusReturn_t<uint8_t> datagramPayloadLength;
  Node                       &node             = _meshNodes[nodeID];

  /* datagramIndex must always be at the start of a datagram header when the while loop check is reached */
  while ((datagramIndex + DATAGRAM_SIZE_HEADER <= nodePacketLength) &&
         (cancelProcessing == false)                                )
  {
    /* Mesh packet data is sent with little endian byte order, order must be swapped when data enters/exits the mesh packet on big endian systems */
    if (systemIsBigEndian()) swapEndiannessRaw(&nodePacket[datagramIndex], DATAGRAM_SIZE_HEADER);

    memcpy(&datagramHeader.asUINT16, &nodePacket[datagramIndex], DATAGRAM_SIZE_HEADER);

    if (datagramHeader.asData.fieldID == SUBSYSTEM_ERROR_INDICATOR)
    {
      datagramIndex += DATAGRAM_SIZE_HEADER;
      Error_t error = static_cast<Error_t>(nodePacket[datagramIndex]);
      //recordError(error);
      break;
    }
  
    datagramPayloadLength = getMemberLength(nodeID,
                                            datagramHeader.asData.fieldID,
                                            datagramHeader.asData.memberID);
  
    if (datagramPayloadLength.status != ERROR_NONE)
    {
      recordError(ERROR_PACKET_PROCESSING);
      break;
    }

    switch (static_cast<Access_t>(datagramHeader.asData.command))
    {      
      case ACCESS_NONE_NACK:
      {
        datagramIndex += DATAGRAM_SIZE_HEADER;
        Error_t nodeMemoryMapError = static_cast<Error_t>(nodePacket[datagramIndex]);
        node.recordError(nodeMemoryMapError);
        recordError(ERROR_PACKET_PROCESSING);
        datagramIndex += sizeof(nodeMemoryMapError);
        break;
      }

      case ACCESS_READ_ACK:
      {
        /* Mesh packet data is sent with little endian byte order, order must be swapped when data enters/exits the mesh packet on big endian systems */
        if (systemIsBigEndian()) swapEndiannessRaw(&nodePacket[datagramIndex + DATAGRAM_INDEX_PAYLOAD], datagramPayloadLength.data);

        transferStatus = internalTransferRaw(ACCESS_WRITE_ACK, 
                                             nodeID, 
                                             datagramHeader.asData.fieldID, 
                                             datagramHeader.asData.memberID, 
                                             &nodePacket[datagramIndex + DATAGRAM_INDEX_PAYLOAD],
                                             datagramPayloadLength.data);

        if (transferStatus == ERROR_NONE)
        {
          updateCommandPattern(ACCESS_READ_ACK, nodeID, datagramHeader.asData.fieldID, datagramHeader.asData.memberID);
        }
        else 
        {
          recordError(ERROR_PACKET_PROCESSING);
        }

        datagramIndex += (DATAGRAM_SIZE_HEADER + datagramPayloadLength.data);
        
        break;
      }

      case ACCESS_WRITE_ACK:
        updateCommandPattern(ACCESS_WRITE_ACK, nodeID, datagramHeader.asData.fieldID, datagramHeader.asData.memberID);
        datagramIndex += DATAGRAM_SIZE_HEADER;
        break;

      case ACCESS_FATAL:
        /* Fatal Error - Inappropriate access command received by node */
      default:
        /* Fatal Error - Inappropriate response */
        recordError(ERROR_COMMAND_RESPONSE_INVALID);
        cancelProcessing = true;
        break;
    }
  }
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */


