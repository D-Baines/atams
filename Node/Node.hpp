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
  uint16_t                     noOfDataBlocks;
  InitUniversalDataFunction_t  initUniversalData;
  DataBlock::BlockDescriptor_t blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS];

  MemoryMap_t(void)
  {
    noOfDataBlocks    = 0U;
    initUniversalData = nullptr;
    for (DataBlock::BlockDescriptor_t &blockDescriptor : blockDescriptors)
    {
      blockDescriptor.noOfDataMembers = 0U;

      for (DataBlock::MemberInfo_t &varInfo : blockDescriptor.dataMemberInfo)
      {
        varInfo.type           = TYPE_NULL;
        varInfo.accessLevel = ACCESS_NONE;
        varInfo.NVMStorage     = false;
      }
    }
  }

  MemoryMap_t(const uint16_t                     initNoOfDataBlocks,
              const InitUniversalDataFunction_t  universalDataInitPtr,
              const DataBlock::BlockDescriptor_t (&initBlockDescriptors)[Platform::NODE_NUMBER_OF_DATA_BLOCKS])
  {
    noOfDataBlocks    = initNoOfDataBlocks;
    initUniversalData = universalDataInitPtr;

    for (uint16_t index = 0U; index < noOfDataBlocks; index++)
    {
      blockDescriptors[index] = initBlockDescriptors[index];
    }
  };

  MemoryMap_t(const MemoryMap_t &other)
  {
    noOfDataBlocks    = other.noOfDataBlocks;
    initUniversalData = other.initUniversalData;

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

Error_t initSingleCore(const MemoryMap_t &memoryMap);

Error_t initCommsCore(const MemoryMap_t &memoryMap);

Error_t initControlCore(const MemoryMap_t &memoryMap);

void updateComms(void);

template <typename T>
Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);

template <typename T>
Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);

bool watchdogFaultActive(void);

DataBlock * getBlockPtr(const uint8_t blockID);

Error_t externalTransfer(const Access_t  accessRequest,
                         const uint8_t   blockID,
                         const uint16_t  memberID,
                         uint8_t * const dataStoragePtr,
                         const uint8_t   length);

DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID);


} /* End Namespace - Atams */


/**
  * @}End of File
  */


