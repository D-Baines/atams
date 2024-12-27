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

class Node :
private Platform::MemoryLock
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
          varInfo.accessLevel = ACCESS_NONE;
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

  Atams::Error_t init(const MemoryMap_t &memoryMap);
     
  template <typename T>
  Atams::Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);
  
  template <typename T>
  Atams::Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);
  
  DataBlock * getBlockPtr(const uint8_t blockID);
  
  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint8_t   blockID,
                                  const uint16_t  memberID,
                                  uint8_t * const dataStoragePtr,
                                  const uint8_t   length);
  
  DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t blockID, const uint16_t memberID);

  Atams::Error_t setRequestPattern(const uint8_t          blockID,
                                   const uint16_t         varID,
                                   const Access_t         accessRequest,
                                   const RequestPattern_t requestPattern);

  uint8_t getNodeID(void);

  Atams::Error_t getEncodedRequestPacket(uint8_t  *outputBuffer,
                                         uint16_t  outputBufferMaxLength, 
                                         uint16_t &outputLength);

  Atams::Error_t responseReceived(uint8_t *inputBuffer, uint16_t inputLength);

  Atams::Error_t processResponseBuffer(void);

  /*-- PRIVATE -----------------------------------------------------------------------*/

  private:

  /*-- PRIVATE CONSTANTS --------------*/

  /*-- PRIVATE TYPEDEFS ---------------*/

  struct RequestPacket_t 
  {
    uint8_t   buffer[MAX_MESH_PACKET_SIZE] = {0U};
    uint16_t  length                       = MESH_SIZE_HEADER;
    WriteList writeList;
  };

  struct RequestChangeConfig_t
  {
    Access_t         accessRequest;
    RequestPattern_t requestPattern;
    uint8_t          blockID; 
    uint16_t         varID;
    DatagramHeader_t newDatagramHeader;
    uint8_t          newDatagramBuffer[DATAGRAM_SIZE_HEADER + MAX_TYPE_SIZE];
    uint8_t          newDatagramLength      = 0U;
    DatagramHeader_t currentDatagramHeader;
    uint8_t          currentDatagramLength  = 0U;
    uint16_t         datagramStartIndex     = 0U;
  };

  /*-- PRIVATE VARIABLES --------------*/

  uint8_t          _nodeID;
  Atams::Bus      &_bus;
  MemoryMap_t      _memoryMap;
  DataBlock        _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS + 1U];
  DataBlock       &_universalBlock                      = _dataBlocks[BLOCK_ID_UNIVERSAL];
  Error_t          _latestError                         = ERROR_NONE;
  uint16_t         _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};
  RequestPacket_t  _requestPacket;
  uint8_t          _responseBuffer[MAX_MESH_PACKET_SIZE];
  uint16_t         _responseLength;

  /*-- PRIVATE FUNCTION DECLARATIONS --*/

  DataStatusReturn_t<bool> findDatagramMatchInPacket(RequestChangeConfig_t requestChangeConfig);

  Atams::Error_t requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength);

  Atams::Error_t packetRemoveCurrentDatagram(RequestChangeConfig_t requestChangeConfig);

  Atams::Error_t packetAdjustCurrentDatagram(RequestChangeConfig_t requestChangeConfig);

  Atams::Error_t packetAppendDatagramToNode(RequestChangeConfig_t requestChanegConfig);

  Atams::Error_t constructDatagram(RequestChangeConfig_t &requestChangeConfig);
  
  Atams::Error_t processPacketChange(const uint8_t          blockID,
                                     const uint16_t         varID,
                                     const Access_t         accessRequest,
                                     const RequestPattern_t requestPattern);

  Atams::Error_t updateRequestPattern(const uint8_t  blockID,
                                      const uint16_t varID,
                                      const Access_t accessRequest);
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

