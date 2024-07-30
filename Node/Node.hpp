/**
  ******************************************************************************
  * @file    Node.hpp
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../../Atams/AtamsTypedefs.hpp"
#include "../../Atams/Node/DataBlock.hpp"
#include "../../Atams/Node/Platform.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Node {


/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef Error_t (*InitDefaultsFunction_t)(void);

typedef Error_t (*InitLimitsFunction_t  )(void);

struct MemoryMap_t
{
  uint16_t                     noOfDataBlocks;
  InitDefaultsFunction_t       initDefaults;
  InitLimitsFunction_t         initLimits;
  DataBlock::BlockDescriptor_t blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS];

  MemoryMap_t(void) = delete;

  MemoryMap_t(const uint16_t                     initNoOfDataFields,
              const DataBlock::BlockDescriptor_t (&initBlockDescriptors)[Platform::NODE_NUMBER_OF_DATA_BLOCKS],
              const InitDefaultsFunction_t       initDefaultsPtr,
              const InitLimitsFunction_t         initLimitsPtr)
  {
    noOfDataBlocks = initNoOfDataFields;
    initDefaults   = initDefaultsPtr;
    initLimits     = initLimitsPtr;

    for (uint16_t index = 0U; index < noOfDataBlocks; index++)
    {
      blockDescriptors[index] = initBlockDescriptors[index];
    }
  };

  MemoryMap_t(const MemoryMap_t &other)
  {
    noOfDataBlocks = other.noOfDataBlocks;
    initDefaults   = other.initDefaults;
    initLimits     = other.initLimits;

    for (uint16_t index = 0U; index < noOfDataBlocks; index++)
    {
      blockDescriptors[index] = other.blockDescriptors[index];
    }
  };

  MemoryMap_t& operator=(const MemoryMap_t &other) = delete;
};


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t init(const MemoryMap_t &memoryMap);

template <typename T>
Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);

template <typename T>
Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);

template <typename T>
Error_t assertLimits(const uint8_t blockID, const uint16_t memberID, const T limitMax, const T limitMin);

Error_t setWriteLock(const uint8_t blockID, const uint16_t memberID, const bool writeLock);

Error_t externalTransfer(const Access_t  accessRequest,
                         const uint8_t   blockID,
                         const uint16_t  memberID,
                         uint8_t * const dataStoragePtr,
                         const uint8_t   length);

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID);


} } /* End Namespace - Atams::Node */


/**
  * @}End of File
  */


