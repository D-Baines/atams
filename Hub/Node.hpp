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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../AtamsTypedefs.hpp"
#include "DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/


/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

class Bus;

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class Node 
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- PUBLIC CONSTANTS ---------------*/

  /*-- PUBLIC TYPEDEFS ----------------*/

  typedef Error_t (*InitUniversalDataFunction_t)(Node &nodeToInit);

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
          varInfo.externalAccess = ACCESS_NONE;
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

  /*-- PUBLIC FUNCTION DECLARATIONS ---*/

  Node(Bus &bus);

  Node(const Node &other)             = delete;

  Node & operator=(const Node &other) = delete;

  Error_t init(const MemoryMap_t &memoryMap);
     
  template <typename T>
  Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);
  
  template <typename T>
  Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);
  
  DataBlock * getBlockPtr(const uint8_t blockID);
  
  Error_t externalTransfer(const Access_t  accessRequest,
                           const uint8_t   blockID,
                           const uint16_t  memberID,
                           uint8_t * const dataStoragePtr,
                           const uint8_t   length);
  
  DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID);

  Error_t setRequestPattern(const uint8_t          blockID,
                            const uint16_t         memberID,
                            const RequestPattern_t updatePattern);


  /*-- PRIVATE -----------------------------------------------------------------------*/

  private:

  /*-- PRIVATE CONSTANTS --------------*/


  /*-- PRIVATE TYPEDEFS ---------------*/


  /*-- PRIVATE VARIABLES --------------*/

  Atams::Bus &_bus;
  MemoryMap_t _memoryMap;
  DataBlock   _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS + 1U];
  DataBlock  &_universalBlock                      = _dataBlocks[BLOCK_ID_UNIVERSAL];
  Error_t     _latestError                         = ERROR_NONE;
  uint16_t    _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};

  /*-- PRIVATE FUNCTION DECLARATIONS --*/

  void processCommsBuffer(uint8_t buffer, uint16_t length);

};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

