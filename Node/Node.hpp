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

typedef Error_t (*InitDefaultsFunction_t)(void);

typedef Error_t (*InitLimitsFunction_t  )(void);

struct MemoryMap_t
{
  uint16_t                     noOfDataBlocks;
  InitDefaultsFunction_t       initDefaults;
  InitLimitsFunction_t         initLimits;
  DataBlock::BlockDescriptor_t blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS];

  MemoryMap_t(void)
  {
    noOfDataBlocks = 0U;
    initDefaults   = nullptr;
    initLimits     = nullptr;

    for (DataBlock::BlockDescriptor_t &blockDescriptor : blockDescriptors)
    {
      blockDescriptor.noOfDataMembers = 0U;

      for (DataBlock::MemberInfo_t &varInfo : blockDescriptor.dataMemberInfo)
      {
        varInfo.type           = TYPE_NULL;
        varInfo.externalAccess = ACCESS_NONE;
        varInfo.NVMStorage     = false;
      }
    }
  }

  MemoryMap_t(const uint16_t                     initNoOfDataFields,
              const InitDefaultsFunction_t       initDefaultsPtr,
              const InitLimitsFunction_t         initLimitsPtr,
              const DataBlock::BlockDescriptor_t (&initBlockDescriptors)[Platform::NODE_NUMBER_OF_DATA_BLOCKS])
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

Error_t initSingleCore(const MemoryMap_t &memoryMap);

Error_t initCommsCore(const MemoryMap_t &memoryMap);

Error_t initControlCore(const MemoryMap_t &memoryMap);

void updateComms(void);

template <typename T>
Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);

template <typename T>
Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);

template <typename T>
Error_t assertLimits(const uint8_t blockID, const uint16_t memberID, const T limitMax, const T limitMin);

Error_t setWriteLock(const uint8_t blockID, const uint16_t memberID, const bool writeLock);

bool watchdogFaultActive(void);

DataBlock * getBlockPointer(const uint8_t blockID);

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


