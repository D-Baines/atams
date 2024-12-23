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
                                const uint16_t         memberID,
                                const RequestPattern_t updatePattern)
{
  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    return (ERROR_BLOCK_ID);
  }

  return(_bus.setRequestPattern(this, blockID, memberID, updatePattern));
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void processCommsBuffer(uint8_t buffer, uint16_t length);


} /* End Namespace - Atams */


/**
  * @}End of File
  */


