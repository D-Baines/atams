/**
  ******************************************************************************
  * @file    NamespaceTemplate.cpp
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

#include "../../Atams/Node/Node.hpp"

#include "string.h"

#include "../../Atams/Node/Devices/MemoryMapExample.hpp"
#include "../../Atams/Utilities/AtamsUtilities.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Node {


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static const MemoryMap_t            &_memoryMap                         = Atams::MapTEMPLATE::memoryMap;
static       DataBlock               _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS];
static       Error_t                 _latestError                       = ERROR_NONE;
static       uint16_t                _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static Error_t recordError(Error_t error)
{
  if (error > NUMBER_OF_ATAMS_ERRORS) error = ERROR_ERROR_MANAGEMENT;

  _latestError = error;
  _errorCounts[error]++;

  return (error);
}


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t init(const MemoryMap_t &memoryMap)
{
  if ((memoryMap.initDefaults == nullptr) ||
      (memoryMap.initLimits   == nullptr) )
  {
    return (ERROR_NULL_PTR);
  }

  if ((memoryMap.noOfDataBlocks > Platform::NODE_NUMBER_OF_DATA_BLOCKS) ||
      (memoryMap.noOfDataBlocks >           MAX_NUMBER_OF_DATA_BLOCKS ) )
  {
    return (ERROR_MEMORY);
  }

  for (uint8_t blockIndex = 0U; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
  {
    const DataBlock::BlockDescriptor_t &newBlockDescriptor = memoryMap.blockDescriptors[blockIndex];
          DataBlock                    &block              = _dataBlocks[blockIndex];

    block.init(newBlockDescriptor);
  }

  Error_t initStatus = ERROR_NONE;

  initStatus = memoryMap.initDefaults();

  if (initStatus == ERROR_NONE) initStatus = memoryMap.initLimits();

  if (initStatus == ERROR_NONE)
  {
    Comms::setNodeWriteCallback();
    Comms::setNodeReadCallback();
    initStatus = Comms::init();
  }

  return (initStatus);
}

template <typename T>
Error_t write(const uint8_t   blockID,
              const uint16_t  memberID,
              const T         writeData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].write(memberID, writeData));
}

template Error_t write<uint8_t >(const uint8_t blockID, const uint16_t memberID, const uint8_t  writeData);
template Error_t write<int8_t  >(const uint8_t blockID, const uint16_t memberID, const int8_t   writeData);
template Error_t write<uint16_t>(const uint8_t blockID, const uint16_t memberID, const uint16_t writeData);
template Error_t write<int16_t >(const uint8_t blockID, const uint16_t memberID, const int16_t  writeData);
template Error_t write<uint32_t>(const uint8_t blockID, const uint16_t memberID, const uint32_t writeData);
template Error_t write<int32_t >(const uint8_t blockID, const uint16_t memberID, const int32_t  writeData);
template Error_t write<float   >(const uint8_t blockID, const uint16_t memberID, const float    writeData);


template <typename T>
Error_t read(const uint8_t   blockID,
             const uint16_t  memberID,
                   T        &readData)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].read(memberID, readData));
}

template Error_t read<uint8_t >(const uint8_t blockID, const uint16_t memberID, uint8_t  &readData);
template Error_t read<int8_t  >(const uint8_t blockID, const uint16_t memberID, int8_t   &readData);
template Error_t read<uint16_t>(const uint8_t blockID, const uint16_t memberID, uint16_t &readData);
template Error_t read<int16_t >(const uint8_t blockID, const uint16_t memberID, int16_t  &readData);
template Error_t read<uint32_t>(const uint8_t blockID, const uint16_t memberID, uint32_t &readData);
template Error_t read<int32_t >(const uint8_t blockID, const uint16_t memberID, int32_t  &readData);
template Error_t read<float   >(const uint8_t blockID, const uint16_t memberID, float    &readData);

template <typename T>
Error_t assertLimits(const uint8_t   blockID,
                     const uint16_t  memberID,
                     const T         limitMax,
                     const T         limitMin)
{
  if (blockID >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].assertLimits(memberID, limitMax, limitMin));
}

template Error_t assertLimits<uint8_t >(const uint8_t blockID, const uint16_t memberID, const uint8_t  limitMax, const uint8_t  limitMin);
template Error_t assertLimits<int8_t  >(const uint8_t blockID, const uint16_t memberID, const int8_t   limitMax, const int8_t   limitMin);
template Error_t assertLimits<uint16_t>(const uint8_t blockID, const uint16_t memberID, const uint16_t limitMax, const uint16_t limitMin);
template Error_t assertLimits<int16_t >(const uint8_t blockID, const uint16_t memberID, const int16_t  limitMax, const int16_t  limitMin);
template Error_t assertLimits<uint32_t>(const uint8_t blockID, const uint16_t memberID, const uint32_t limitMax, const uint32_t limitMin);
template Error_t assertLimits<int32_t >(const uint8_t blockID, const uint16_t memberID, const int32_t  limitMax, const int32_t  limitMin);
template Error_t assertLimits<float   >(const uint8_t blockID, const uint16_t memberID, const float    limitMax, const float    limitMin);

Error_t setWriteLock(const uint8_t  blockID,
                     const uint16_t memberID,
                     const bool     writeLock)
{
  if (blockID  >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].setWriteLock(memberID, writeLock));
}

Error_t externalTransfer(const Access_t  accessRequest,
                         const uint8_t   blockID,
                         const uint16_t  memberID,
                         uint8_t * const dataStoragePtr,
                         const uint8_t   length)
{
  if (blockID  >= _memoryMap.noOfDataBlocks) return (ERROR_BLOCK_ID);

  return (_dataBlocks[blockID].externalTransfer(accessRequest, memberID, dataStoragePtr, length));
}

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= _memoryMap.noOfDataBlocks)
  {
    lengthReturn.status = ERROR_BLOCK_ID;
    return (lengthReturn);
  }

  return (_dataBlocks[blockID].getMemberLength(memberID));
}


} }  /* End Namespace - Atams::Node */

/**
  * @}End of File
  */
