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

void Node::copyToActiveBuffer(uint8_t *buffer, uint16_t length)
{
  if (length <= MAX_MESH_PACKET_SIZE) memcpy(_activePacketPtr->buffer, buffer, length);

  _activePacketPtr->length = length;
}

void Node::swapAndProcessBuffers(void)
{
  Platform::MemoryLock::acquireLock();
  
  /* Swap the inactive/active mesh packet pointers */
  Packet_t* tempPacketPtr = _activePacketPtr;
  _activePacketPtr        = _inactivePacketPtr;
  _inactivePacketPtr      = tempPacketPtr;

   /* Copy contents of the now active mesh packet into the now inactive mesh packet for editing */
  *_inactivePacketPtr = *_activePacketPtr;

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


} /* End Namespace - Atams */


/**
  * @}End of File
  */


