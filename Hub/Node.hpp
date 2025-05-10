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
#include "Developer/NodeProcesses.hpp"

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

class Node
{
  /*-- Friend Class Declarations ----------------------------------------------------*/

  friend class Bus;

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct MemoryMap_t
  {
    SharedMemoryMap_t sharedMemoryMap;
  };

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
  
  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Atams::Error_t setRequestPattern(const uint16_t         varID,
                                   const Access_t         accessRequest,
                                   const RequestPattern_t requestPattern);

  Atams::Error_t getRequestPattern(const uint16_t   varID,
                                   Access_t         &accessRequest,
                                   RequestPattern_t &requestPattern);

  void resetRequestPacket(void);

  void setNodeID(const uint8_t nodeID);

  uint8_t getNodeID(void);

  Atams::Error_t getBusError(void);

  bool validateGenInfo(void);

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

  Platform::MemoryLock m_requestPacketLock;
  Platform::MemoryLock m_varStorageLock;
  Platform::MemoryLock m_busErrorLock;
  NodeActions          m_nodeProcessHandler;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t                  _nodeID;
  const SharedMemoryMap_t *m_memoryMap;
  uint16_t                 m_validVarCount = 0U;
  Var_t                    m_varStorage[Platform::NODE_NUMBER_OF_VARS];
  Atams::Error_t           _busError = Atams::ERROR_NONE;
  uint16_t                 _errorCounts[NUMBER_OF_ATAMS_ERRORS] = {0U};
  RequestPacket_t          _requestPacket;
  uint8_t                  _responseBuffer[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t                 _responseLength   = 0U;
  bool                     _newResponseReady = false;
  
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

  Atams::Error_t getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                         uint8_t * const            outputBuffer,
                                         const uint16_t             outputBufferMaxLength, 
                                         uint16_t                  &outputLength);

  void responseReceived(uint8_t *inputBuffer, uint16_t inputLength);

  void reportBusError(Atams::Error_t busError);

  void clearBusError(void);

  void processAbortedResponse(void);

  bool processDatagramRead(const DatagramHeader_t datagramHeader,
                           uint16_t              &datagramStartIndex,
                           const uint8_t          payloadLength);

  bool processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex);

  bool processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex);

  Atams::Error_t validateResponseBuffer(uint8_t * const responsePacket, const uint16_t responsePacketLength);

  void processResponseBuffer(void);

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
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

