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
#include "Utilities/WriteList.hpp"

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
  /*-- Friend Declarations ----------------------------------------------------------*/

  friend class Bus;

  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- PUBLIC CONSTANTS ---------------*/

  /*-- PUBLIC TYPEDEFS ----------------*/

  typedef Atams::Error_t (*InitUniversalDataFunction_t)(Node &nodeToInit);

  struct MemoryMap_t
  {
    uint16_t                            noOfDataBlocks    = 0U;
    MapGenInfo_t                        genInfo;
    InitUniversalDataFunction_t         initUniversalData = nullptr;
    const DataBlock::BlockDescriptor_t *blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS];
  
    MemoryMap_t(void)
    {
      for (const DataBlock::BlockDescriptor_t *&blockDescriptor : blockDescriptors) blockDescriptor = nullptr;
    }
  
    MemoryMap_t(const uint16_t                      initNoOfDataBlocks,
                const MapGenInfo_t                  initGenInfo,
                const InitUniversalDataFunction_t   universalDataInitFnPtr,
                const DataBlock::BlockDescriptor_t *blockDescriptorPtrs[Platform::NODE_NUMBER_OF_DATA_BLOCKS])
    {
      noOfDataBlocks    = initNoOfDataBlocks;
      genInfo           = initGenInfo;
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
      genInfo           = other.genInfo;
      initUniversalData = other.initUniversalData;
      for (uint16_t blockIndex = 0U; blockIndex < Platform::NODE_NUMBER_OF_DATA_BLOCKS; blockIndex++)
      {
        blockDescriptors[blockIndex] = other.blockDescriptors[blockIndex];
      }
  
      return (*this);
    }
  };

  /*-- PUBLIC FUNCTION DECLARATIONS ---*/

  Node(Bus &bus, const uint8_t nodeID);

  Node(const Node &other) = delete;

  Node & operator=(const Node &other) = delete;

  Atams::Error_t init(const MemoryMap_t &memoryMap);

  Atams::Error_t initDefaults(void);
     
  template <typename T>
  Atams::Error_t write(const uint8_t blockID, const uint16_t memberID, const T writeData);
  
  template <typename T>
  Atams::Error_t read(const uint8_t blockID, const uint16_t memberID, T &readData);

  template <typename T>
  Atams::Error_t readIfNew(const uint8_t blockID, const uint16_t memberID, T &readData);

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

  Atams::Error_t getRequestPattern(const uint8_t     blockID,
                                   const uint16_t    varID,
                                   Access_t         &accessRequest,
                                   RequestPattern_t &requestPattern);

  Atams::Error_t setRequestPatternNoChecks(const uint8_t          blockID,
                                           const uint16_t         varID,
                                           const Access_t         accessRequest,
                                           const RequestPattern_t requestPattern);

  uint8_t getNodeID(void);

  Atams::Error_t getBusError(void);

  DataBlock * getBlockPtr(const uint8_t blockID);

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

  Atams::Bus        &_bus;
  uint8_t            _nodeID;
  MemoryMap_t        _memoryMap;
  DataBlock          _dataBlocks[Platform::NODE_NUMBER_OF_DATA_BLOCKS + 1U];
  DataBlock         &_universalBlock                      = _dataBlocks[BLOCK_ID_UNIVERSAL];
  Error_t            _busError                            = ERROR_NONE;
  uint16_t           _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};
  RequestPacket_t    _requestPacket;
  uint8_t            _responseBuffer[MAX_MESH_PACKET_SIZE];
  uint16_t           _responseLength   = 0U;
  bool               _newResponseReady = false;

  /*-- PRIVATE FUNCTION DECLARATIONS --*/

  Atams::Error_t getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                         uint8_t * const            outputBuffer,
                                         const uint16_t             outputBufferMaxLength, 
                                         uint16_t                  &outputLength);

  Atams::Error_t responseReceived(uint8_t *inputBuffer, uint16_t inputLength);

  void reportBusError(Atams::Error_t busError);

  void clearBusError(void);

  void processAbortedResponse(void);

  void processResponseBuffer(void);

  DataStatusReturn_t<bool> findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength);

  Atams::Error_t requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t constructDatagram(RequestChangeConfig_t &changeConfig);
  
  Atams::Error_t processRequestPacketChange(const uint8_t          blockID,
                                            const uint16_t         varID,
                                            const Access_t         accessRequest,
                                            const RequestPattern_t requestPattern);

  Atams::Error_t updateRequestPattern(const uint8_t  blockID,
                                      const uint16_t varID,
                                      const Access_t accessRequest);

  void updateRequestPacketWriteData(void);

  bool validateGenInfo(void);
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

