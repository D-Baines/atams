/**
  ******************************************************************************
  * @file    Node.hpp
  *
  * @author  D. Baines
  *
  * @brief   Public interface for an Atams Hub Node instance.
  *
  * @details Defines the Atams::Node class, which represents a single Atams Node
  *          device managed by an Atams::Bus. The public API provides typed 
  *          variable access, configurable request patterns, and combined 
  *          convenience functions.
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

#include "../Shared/AtamsTypedefs.hpp"
#include "../Shared/Utilities/CRC32.hpp"
#include "Platform.hpp"
#include "Developer/FramingConstants.hpp"
#include "Developer/WriteList.hpp"
#include "Developer/NodeCallbackHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class Node :
public NodeCallbackHandler
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct MemoryMap_t :
  public SharedMemoryMap_t {};

  /*-- Public Function Declarations -------------------------------------------------*/

  Node(const uint8_t nodeID);

  /* Default Constructor */
  Node(void) = delete;

  /* Default Destructor */
  ~Node(void) = default;

  /* Copy Constructor */
  Node(const Node &other) = delete;

  /* Copy Assignment Operator */
  Node & operator=(const Node &other) = delete;

  /* Move Constructor */
  Node(Node &&other) = delete;

  /* Move Assignment Operator */
  Node & operator=(Node &&other) = delete;

  Atams::Error_t init(const MemoryMap_t &memoryMap);

  void setNodeID(const uint8_t nodeID);

  uint8_t getNodeID(void);
     
  template <typename T>
  Atams::Error_t setVar(const uint16_t varID, const T writeValue);
  
  template <typename T>
  Atams::Error_t getVar(const uint16_t varID, T &readData);

  Atams::Error_t setRequestPattern(const uint16_t                varID,
                                   const Atams::Access_t         accessRequest,
                                   const Atams::RequestPattern_t requestPattern);

  Atams::Error_t getRequestPattern(const uint16_t           varID,
                                   Atams::Access_t         &accessRequest,
                                   Atams::RequestPattern_t &requestPattern);
  
  void clearAllRequestPatterns(void);

  Atams::Error_t getBusError(void);

  uint16_t getNodeMaxPacketSize(void) const;
  
  template <typename T>
  Atams::Error_t setWriteUntilAck(const uint16_t varID, const T writeValue);

  Atams::Error_t setReadUntilAck(const uint16_t varID);

  Atams::Error_t setWriteStream(const uint16_t varID);

  Atams::Error_t setReadStream(const uint16_t varID);

  Atams::Error_t stopStream(const uint16_t varID);

  Atams::Error_t isDataReady(const uint16_t varID, bool &newDataReady);

  Atams::Error_t clearDataReady(const uint16_t varID);

  Atams::Error_t isWriteAcked(const uint16_t varID, bool &ackReceived);

  Atams::Error_t clearWriteAck(const uint16_t varID);

  Atams::Error_t clearDataReadySetReadStream(const uint16_t varID);

  Atams::Error_t stopStreamIsDataReady(const uint16_t varID, bool &newDataReady);

  template <typename T>
  Atams::Error_t getVarIfDataReady(const uint16_t varID, T &outputRef);

  template<typename T>
  Atams::Error_t stopStreamGetVarIfDataReady(const uint16_t varID, T &readData);

  template<typename T>
  Atams::Error_t clearAckSetWriteStream(const uint16_t varID, const T writeData);
  
  Atams::Error_t stopStreamGetWriteAck(const uint16_t varID, bool &ackReceived);

  Atams::Error_t getVarLength(const uint16_t varID, uint8_t &length);

  uint16_t getRequestPacketLength(void);

  uint16_t getExpectedResponseLength(void);

  uint16_t getWriteListLength(void);

  Atams::AbortedResponseDetails_t getAbortedResponseDetails(void);

  Atams::Error_t validateGenInfo(void);

  Atams::Error_t validateAtamsVersion(void);

  #if (DEVELOPER_TOOLS == true)
  void injectBusError(const Atams::Error_t errorToInject, 
                      const uint16_t       readOnlyVarID,
                      uint16_t            &varIDUsed);   
  
  void clearInjectedBusError(const Atams::Error_t errorToClear, const uint16_t readOnlyVarID);
  #endif

  private:

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct RequestPacket_t 
  {
    uint8_t   buffer[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING] {0U};
    uint16_t  length                                            {HEADER_SIZE_HEADER};
    WriteList writeList;
  };

  struct RequestChangeConfig_t
  {
    Atams::Access_t         accessRequest;
    Atams::RequestPattern_t requestPattern;
    Atams::DatagramHeader_t newDatagramHeader;
    uint8_t                 newDatagramBuffer[DATAGRAM_SIZE_HEADER + MAX_TYPE_SIZE];
    uint8_t                 newDatagramLength      {0U};
    Atams::DatagramHeader_t currentDatagramHeader;
    uint8_t                 currentDatagramLength  {0U};
    uint16_t                datagramStartIndex     {0U};
    uint8_t                 writePayloadLength     {0U};
  };

  struct Var_t
  {
    Atams::VarStorage_t     storage                {0U, 0U, 0U, 0U};
    Atams::Access_t         requestAccess          {Atams::ACCESS_NONE};
    Atams::RequestPattern_t requestPattern         {Atams::REQUEST_INACTIVE};
    bool                    newDataReady           {false};
    bool                    ackReceived            {false};
  };

  /*-- Static Private Objects -------------------------------------------------------*/

  static Atams::CRC32 s_nodeCRC;

  /*-- Private Objects --------------------------------------------------------------*/

  Platform::MemoryLock requestPacketLock_;
  Platform::MemoryLock varStorageLock_;
  Platform::MemoryLock busErrorLock_;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t                  nodeID_;
  const SharedMemoryMap_t *memoryMap_;

  Atams::Error_t           busError_               {Atams::ERROR_NONE};
  uint16_t                 validVarCount_          {0U};
  uint16_t                 responseLength_         {0U};
  uint16_t                 nodeMaxPacketSize_      {0U};
  uint16_t                 expectedResponseLength_ {Atams::HEADER_SIZE_HEADER};
  bool                     newResponseReady_       {false};
  AbortedResponseDetails_t abortedResponseDetails_ {Atams::VAR_ID_NULL, Atams::ERROR_NONE};

  RequestPacket_t          requestPacket_;
  uint8_t                  responseBuffer_[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  Var_t                    varStorage_[Platform::NODE_NUMBER_OF_VARS];

  /*-- Private Function Declarations ------------------------------------------------*/

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  varID,
                                  uint8_t * const bytesPtr,
                                  const uint8_t   length);
                              
  void resetVars(void);

  bool getMemoryMapIsValid(void);

  void invalidateMemoryMap(void);  

  Atams::Error_t initBlockDescriptors(const MemoryMap_t &memoryMap);

  Atams::Error_t initUniversalData(const MemoryMap_t &memoryMap);

  void processAbortedResponse(void);

  void reportAbortedResponse(const uint16_t varID, const Atams::Error_t error);

  bool processDatagramRead(const DatagramHeader_t datagramHeader,
                           uint16_t              &datagramStartIndex,
                           const uint8_t          payloadLength);

  bool processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex);

  bool processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex);

  Atams::Error_t validateResponseBuffer(uint8_t * const responsePacket, const uint16_t responsePacketLength);


  DataStatusReturn_t<bool> findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength);

  Atams::Error_t requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig);

  Atams::Error_t constructDatagramBuffer(RequestChangeConfig_t &changeConfig);
  
  void resetRequestPacketNoLock(void);

  Atams::Error_t processRequestPacketChange(const uint16_t                varID,
                                            const Atams::Access_t         accessRequest,
                                            const Atams::RequestPattern_t requestPattern);

  Atams::Error_t updateRequestPatternOnReceive(const uint16_t varID);

  bool wouldExceedResponseBuffer(const Atams::VarInfo_t        &varInfo,
                                 const Atams::Access_t          accessRequest,
                                 const Atams::RequestPattern_t  requestPattern,
                                 const Atams::Access_t          currentAccess) const;

  virtual Atams::Error_t updateRequestPacketWriteData(void);

  virtual void setNodeMaxPacketSize(const uint16_t maxPacketSize) final;

  virtual void reportBusError(Atams::Error_t busError) final;
  
  virtual void clearBusError(void) final;

  virtual void clearAbortDetails(void) final;

  virtual void responseReceived(uint8_t *inputBuffer, uint16_t inputLength) final;

  virtual void processResponseBuffer(void) final;

  virtual void getRequestPacket(const Atams::MessageType_t requestType,
                                const uint8_t              syncCount,
                                uint8_t * const            outputBuffer,
                                uint16_t                  &outputLength) final;
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

