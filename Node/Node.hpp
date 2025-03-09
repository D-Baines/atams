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
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../AtamsTypedefs.hpp"
#include "DataBlock.hpp"
#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef Error_t (*InitUniversalDataFunction_t)(void);

struct MemoryMap_t
{
  uint16_t                            noOfDataBlocks;
  InitUniversalDataFunction_t         initUniversalData;
  const DataBlock::BlockDescriptor_t *blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS];

  MemoryMap_t(void)
  {
    noOfDataBlocks    = 0U;
    initUniversalData = nullptr;
    for (const DataBlock::BlockDescriptor_t *&blockDescriptor : blockDescriptors)
    {
      blockDescriptor = nullptr;
    }
  }

  MemoryMap_t(const uint16_t                      initNoOfDataBlocks,
              const InitUniversalDataFunction_t   universalDataInitFnPtr,
              const DataBlock::BlockDescriptor_t *blockDescriptorPtrs[Platform::NODE_NUMBER_OF_DATA_BLOCKS])
  {
    noOfDataBlocks    = initNoOfDataBlocks;
    initUniversalData = universalDataInitFnPtr;
    for (uint16_t blockIndex = 0U; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
    {
      blockDescriptors[blockIndex] = blockDescriptorPtrs[blockIndex];
    }
  };

  MemoryMap_t(const MemoryMap_t &other) = delete;

  MemoryMap_t& operator=(const MemoryMap_t &other)
  {
    if (&other == this) return (*this);

    noOfDataBlocks    = other.noOfDataBlocks;
    initUniversalData = other.initUniversalData;
    for (uint16_t blockIndex = 0U; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
    {
      blockDescriptors[blockIndex] = other.blockDescriptors[blockIndex];
    }

    return (*this);
  }
};

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap);

Atams::Error_t initControlCore(const MemoryMap_t &memoryMap);

Atams::Error_t initDefaults(void);

void updateComms(void);

template <typename T>
Atams::Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);

template <typename T>
Atams::Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);

Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                const uint8_t   blockID,
                                const uint16_t  memberID,
                                uint8_t * const dataStoragePtr,
                                const uint8_t   length);

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID);

bool watchdogFaultActive(void);

DataBlock * getBlockPtr(const uint8_t blockID);

} /* End Namespace: Atams */


/**
  * @}End of File
  */


