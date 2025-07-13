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
#include "../Shared/AtamsTypedefs.hpp"
#include "../Shared/Utilities/CRC32.hpp"
#include "Platform.hpp"
#include "Developer/WriteList.hpp"
#include "Developer/NodeCallbackHandler.hpp"
#include "Developer/NodeActions.hpp"

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

  /* Default Constructor */
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

  Atams::Error_t initDefaults(void);
     
  template <typename T>
  Atams::Error_t write(const uint16_t varID, const T writeData);
  
  template <typename T>
  Atams::Error_t read(const uint16_t varID, T &readData);

  template <typename T>
  Atams::Error_t readIfDataReady(const uint16_t varID, T &readData);

  template <typename T>
  Atams::Error_t writeRequestUntilAck(const uint16_t varID, const T writeData);

  Atams::Error_t readRequestUntilAck(const uint16_t varID);

  Atams::Error_t startWriteStream(const uint16_t varID);

  Atams::Error_t startReadStream(const uint16_t varID);

  Atams::Error_t stopStream(const uint16_t varID);

  Atams::Error_t getDataReadyFlag(const uint16_t varID, bool &newDataReady);

  Atams::Error_t clearDataReadyFlag(const uint16_t varID);

  Atams::Error_t getAckFlag(const uint16_t varID, bool &ackReceived);

  Atams::Error_t clearAckFlag(const uint16_t varID);

  Atams::Error_t clearDataReadyStartRead(const uint16_t varID);

  Atams::Error_t stopStreamGetDataReady(const uint16_t varID, bool &newDataReady);

  Atams::Error_t stopStreamGetAckFlag(const uint16_t varID, bool &ackReceived);

  template<typename T>
  Atams::Error_t stopStreamReadIfDataReady(const uint16_t varID, T &readData);

  template<typename T>
  Atams::Error_t clearAckStartWrite(const uint16_t varID, const T writeData);
  
  Atams::Error_t stopStreamGetWriteAck(const uint16_t varID, bool &ackReceived);

  Atams::Error_t setRequestPattern(const uint16_t         varID,
                                   const Access_t         accessRequest,
                                   const RequestPattern_t requestPattern);

  Atams::Error_t getRequestPattern(const uint16_t   varID,
                                   Access_t         &accessRequest,
                                   RequestPattern_t &requestPattern);

  void resetRequestPacket(void);

  uint16_t getRequestPacketLength(void);

  Atams::Error_t getVarLength(const uint16_t varID, uint8_t &length);

  void setNodeID(const uint8_t nodeID);

  virtual uint8_t getNodeID(void) final;

  virtual Atams::Error_t getBusError(void) final;

  //#if DEVELOPER_TOOLS 
  Atams::Error_t setRequestPatternNoChecks(const uint16_t         varID,
                                           const Access_t         accessRequest,
                                           const RequestPattern_t requestPattern);
                                           
  //#endif

  private:

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct RequestPacket_t 
  {
    uint8_t   buffer[Platform::MAX_BUS_PACKET_SIZE] = {0U};
    uint16_t  length                                = MESH_SIZE_HEADER;
    WriteList writeList;
  };

  struct RequestChangeConfig_t
  {
    Access_t         accessRequest;
    RequestPattern_t requestPattern;
    DatagramHeader_t newDatagramHeader;
    uint8_t          newDatagramBuffer[DATAGRAM_SIZE_HEADER + MAX_TYPE_SIZE];
    uint8_t          newDatagramLength      = 0U;
    DatagramHeader_t currentDatagramHeader;
    uint8_t          currentDatagramLength  = 0U;
    uint16_t         datagramStartIndex     = 0U;
    uint8_t          writePayloadLength     = 0U;
  };

  struct Var_t
  {
    uint8_t                 storage[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
    Atams::Access_t         requestAccess          = Atams::ACCESS_NONE;
    Atams::RequestPattern_t requestPattern         = Atams::REQUEST_INACTIVE;
    bool                    newDataReady           = false;
    bool                    ackReceived            = false;
  };

  /*-- Static Private Objects -------------------------------------------------------*/

  static Atams::CRC32 s_nodeCRC;

  /*-- Private Objects --------------------------------------------------------------*/

  Platform::MemoryLock requestPacketLock_;
  Platform::MemoryLock varStorageLock_;
  Platform::MemoryLock busErrorLock_;
  NodeActions          nodeProcessHandler_;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t                  nodeID_;
  const SharedMemoryMap_t *memoryMap_;
  uint16_t                 validVarCount_ = 0U;
  Var_t                    varStorage_[Platform::NODE_NUMBER_OF_VARS];
  Atams::Error_t           busError_ = Atams::ERROR_NONE;
  RequestPacket_t          requestPacket_;
  uint8_t                  responseBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t                 responseLength_   = 0U;
  bool                     newResponseReady_ = false;
  
  /*-- Private Constexpr Function Declarations --------------------------------------*/

  template <typename T>
  constexpr Atams::VarType_t getAtamsType(void);

  /*-- Private Function Declarations ------------------------------------------------*/

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  memberID,
                                  uint8_t * const bytesPtr,
                                  const uint8_t   length);
                              
  void resetVars(void);

  bool getMemoryMapIsValid(void);

  void invalidateMemoryMap(void);  

  Atams::Error_t initBlockDescriptors(const MemoryMap_t &memoryMap);

  Atams::Error_t initUniversalData(const MemoryMap_t &memoryMap);

  void processAbortedResponse(void);

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

  Atams::Error_t processRequestPacketChange(const uint16_t         varID,
                                            const Access_t         accessRequest,
                                            const RequestPattern_t requestPattern);

  Atams::Error_t updateRequestPatternOnReceive(const uint16_t varID);

  Atams::Error_t updateRequestPacketWriteData(void);

  virtual bool validateGenInfo(void);

  virtual void reportBusError(Atams::Error_t busError) final;
  
  virtual void clearBusError(void) final;

  virtual void responseReceived(uint8_t *inputBuffer, uint16_t inputLength) final;

  virtual void processResponseBuffer(void) final;

  virtual Atams::Error_t getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                                 const uint8_t              syncCount,
                                                 uint8_t * const            outputBuffer,
                                                 const uint16_t             outputBufferMaxLength, 
                                                 uint16_t                  &outputLength) final;
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

