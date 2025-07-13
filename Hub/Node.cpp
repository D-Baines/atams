/**
  ******************************************************************************
  * @file    Node.cpp
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


/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "Platform.hpp"
#include <type_traits>
#include "Node.hpp"
#include "../Shared/Utilities/AtamsUtilities.hpp"
#include "../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Node::Node(const uint8_t nodeID) :
nodeID_(nodeID)
{

}

Atams::Error_t Node::init(const MemoryMap_t &memoryMap)
{
  Atams::Error_t statusReturn = Atams::validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (statusReturn == Atams::ERROR_NONE)
  {
    if ((varStorageLock_.init()   ) &&
        (requestPacketLock_.init()) &&
        (busErrorLock_.init()     ) )
    {
      memoryMap_     = &memoryMap;
      validVarCount_ = memoryMap.noOfVars;
    }
    else 
    {
      invalidateMemoryMap();
      statusReturn = ERROR_PLATFORM;
    }
  }

  return (statusReturn);
}

template <typename T>
Atams::Error_t Node::write(const uint16_t varID, const T writeData)
{
  if (varID >= validVarCount_) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)        return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_WRITE       > varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();

  memcpy(var.storage, &writeData, sizeof(writeData));

  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t Node::write<uint8_t >(const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t Node::write<int8_t  >(const uint16_t varID, const int8_t   writeData);
template Atams::Error_t Node::write<uint16_t>(const uint16_t varID, const uint16_t writeData);
template Atams::Error_t Node::write<int16_t >(const uint16_t varID, const int16_t  writeData);
template Atams::Error_t Node::write<uint32_t>(const uint16_t varID, const uint32_t writeData);
template Atams::Error_t Node::write<int32_t >(const uint16_t varID, const int32_t  writeData);
template Atams::Error_t Node::write<float   >(const uint16_t varID, const float    writeData);

template <typename T>
Atams::Error_t Node::read(const uint16_t varID, T &readData)
{
  if (varID >= validVarCount_) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)        return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();

  memcpy(&readData, var.storage, sizeof(readData));

  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t Node::read<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::read<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::read<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::read<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::read<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::read<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::read<float   >(const uint16_t varID, float    &readData);

template <typename T>
Atams::Error_t Node::readIfDataReady(const uint16_t varID, T &readData)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)        return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t   &var          = varStorage_[varID];
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  varStorageLock_.acquireLock();

  if (var.newDataReady) memcpy(&readData, &var.storage, sizeof(readData));
  else                  statusReturn = Atams::ERROR_NEW_DATA_NOT_READY;

  var.newDataReady = false;

  varStorageLock_.releaseLock();

  return (statusReturn);
}

template Atams::Error_t Node::readIfDataReady<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::readIfDataReady<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::readIfDataReady<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::readIfDataReady<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::readIfDataReady<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::readIfDataReady<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::readIfDataReady<float   >(const uint16_t varID, float    &readData);

template <typename T>
Atams::Error_t Node::writeRequestUntilAck(const uint16_t varID, const T writeData)
{
  Atams::Error_t statusReturn = Node::write(varID, writeData);

  if (statusReturn == Atams::ERROR_NONE) 
  {
    statusReturn = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_UNTIL_ACK);
  }

  return (statusReturn);
}

template Atams::Error_t Node::writeRequestUntilAck<uint8_t >(const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t Node::writeRequestUntilAck<int8_t  >(const uint16_t varID, const int8_t   writeData);
template Atams::Error_t Node::writeRequestUntilAck<uint16_t>(const uint16_t varID, const uint16_t writeData);
template Atams::Error_t Node::writeRequestUntilAck<int16_t >(const uint16_t varID, const int16_t  writeData);
template Atams::Error_t Node::writeRequestUntilAck<uint32_t>(const uint16_t varID, const uint32_t writeData);
template Atams::Error_t Node::writeRequestUntilAck<int32_t >(const uint16_t varID, const int32_t  writeData);
template Atams::Error_t Node::writeRequestUntilAck<float   >(const uint16_t varID, const float    writeData);

Atams::Error_t Node::readRequestUntilAck(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_UNTIL_ACK));
}

Atams::Error_t Node::startWriteStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM));
}

Atams::Error_t Node::startReadStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_STREAM));
}

Atams::Error_t Node::stopStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_NONE, Atams::REQUEST_INACTIVE));
}

Atams::Error_t Node::getDataReadyFlag(const uint16_t varID, bool &newDataReady)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();
  newDataReady = var.newDataReady;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::clearDataReadyFlag(const uint16_t varID)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();
  var.newDataReady = false;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::getAckFlag(const uint16_t varID, bool &ackReceived)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();
  ackReceived = var.ackReceived;
  varStorageLock_.releaseLock();

  if (ackReceived) return (Atams::ERROR_NONE);
  else             return (Atams::ERROR_ACK_NOT_RECEIVED);
}

Atams::Error_t Node::clearAckFlag(const uint16_t varID)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();
  var.ackReceived = false;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::clearDataReadyStartRead(const uint16_t varID)
{
  Atams::Error_t error = Node::clearDataReadyFlag(varID);
  if (!error)    error = Node::startReadStream(varID);

  return (error);
}

Atams::Error_t Node::stopStreamGetDataReady(const uint16_t varID, bool &newDataReady)
{
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::getDataReadyFlag(varID, newDataReady);

  return (error);
}

Atams::Error_t Node::stopStreamGetAckFlag(const uint16_t varID, bool &ackReceived)
{
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::getAckFlag(varID, ackReceived);

  return (error);
}

template<typename T>
Atams::Error_t Node::stopStreamReadIfDataReady(const uint16_t varID, T &readData)
{
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::readIfDataReady(varID, readData);

  return (error);
}

template Atams::Error_t Node::stopStreamReadIfDataReady<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::stopStreamReadIfDataReady<float   >(const uint16_t varID, float    &readData);

template<typename T>
Atams::Error_t Node::clearAckStartWrite(const uint16_t varID, const T writeData)
{
  Atams::Error_t error = Node::clearAckFlag(varID);
  if (!error)    error = Node::write(varID, writeData);
  if (!error)    error = Node::startWriteStream(varID);

  return (error);
}

template Atams::Error_t Node::clearAckStartWrite<uint8_t >(const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t Node::clearAckStartWrite<int8_t  >(const uint16_t varID, const int8_t   writeData);
template Atams::Error_t Node::clearAckStartWrite<uint16_t>(const uint16_t varID, const uint16_t writeData);
template Atams::Error_t Node::clearAckStartWrite<int16_t >(const uint16_t varID, const int16_t  writeData);
template Atams::Error_t Node::clearAckStartWrite<uint32_t>(const uint16_t varID, const uint32_t writeData);
template Atams::Error_t Node::clearAckStartWrite<int32_t >(const uint16_t varID, const int32_t  writeData);
template Atams::Error_t Node::clearAckStartWrite<float   >(const uint16_t varID, const float    writeData);

Atams::Error_t Node::stopStreamGetWriteAck(const uint16_t varID, bool &ackReceived)
{
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::getAckFlag(varID, ackReceived);

  return (error);
}

/* RE-EVAULATE THIS AND ALL REQUEST PACKET HANDLING */
Atams::Error_t Node::setRequestPattern(const uint16_t         varID,
                                       const Access_t         accessRequest,
                                       const RequestPattern_t requestPattern)
{ 
  if (varID          >= validVarCount_)                   return (Atams::ERROR_VAR_ID);                  /* Early Return */
  if (requestPattern >= Atams::NUMBER_OF_REQUEST_PATTERNS) return (Atams::ERROR_REQUEST_PATTERN_INVALID); /* Early Return */
  
  Node::Var_t     &var     = varStorage_[varID];
  const VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (accessRequest > varInfo.accessLevel)
  {
    return (Atams::ERROR_ACCESS_INVALID); /* Early Return */
  }

  requestPacketLock_.acquireLock();

  Atams::Error_t statusReturn = processRequestPacketChange(varID, accessRequest, requestPattern);
  
  if (statusReturn == Atams::ERROR_NONE)
  {
    var.requestAccess  = accessRequest;
    var.requestPattern = requestPattern;
  }

  requestPacketLock_.releaseLock();

  return (statusReturn);
}

Atams::Error_t Node::getRequestPattern(const uint16_t   varID,
                                       Access_t         &accessRequest,
                                       RequestPattern_t &requestPattern)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  requestPacketLock_.acquireLock();
 
  accessRequest  = var.requestAccess;
  requestPattern = var.requestPattern;

  requestPacketLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

void Node::resetRequestPacket(void)
{
  requestPacketLock_.acquireLock();

  resetRequestPacketNoLock();

  requestPacketLock_.releaseLock();
}

uint16_t Node::getRequestPacketLength(void)
{
  requestPacketLock_.acquireLock();

  uint16_t requestPacketLength = requestPacket_.length;

  requestPacketLock_.releaseLock();

  return (requestPacketLength);
}

void Node::setNodeID(const uint8_t nodeID)
{
  nodeID_ = nodeID;
}

Atams::Error_t Node::getVarLength(const uint16_t varID, uint8_t &length)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  length = TYPE_LENGTHS[varInfo.type];

  return (Atams::ERROR_NONE);
}

uint8_t Node::getNodeID(void)
{
  return (nodeID_);
}

Atams::Error_t Node::getBusError(void)
{
  Atams::Error_t errorReturn = Atams::ERROR_NONE;

  busErrorLock_.acquireLock();
  errorReturn = busError_;
  busErrorLock_.releaseLock();

  return (errorReturn);
}

/*************************************************************************************/
/* PRIVATE CONSTEXPR FUNCTION DEFINITIONS                                            */
/*************************************************************************************/

template <typename T>
constexpr Atams::VarType_t Node::getAtamsType(void)
{
    if      constexpr (std::is_same<T, uint8_t>::value)  return (Atams::TYPE_UINT8);
    else if constexpr (std::is_same<T, int8_t>::value)   return (Atams::TYPE_INT8);
    else if constexpr (std::is_same<T, uint16_t>::value) return (Atams::TYPE_UINT16);
    else if constexpr (std::is_same<T, int16_t>::value)  return (Atams::TYPE_INT16);
    else if constexpr (std::is_same<T, uint32_t>::value) return (Atams::TYPE_UINT32);
    else if constexpr (std::is_same<T, int32_t>::value)  return (Atams::TYPE_INT32);
    else if constexpr (std::is_same<T, float>::value)    return (Atams::TYPE_FLOAT);
    else    static_assert(!std::is_same<T, T>::value,   "Invalid type passed to Node::getAtamsType(void)");
    return (Atams::TYPE_NULL);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

bool Node::validateGenInfo(void)
{
  if (getMemoryMapIsValid() == false) return (false); /* Early Return */
  
  const GenInfo_t nullGenInfo;
  bool            genInfoMatch = false;
  GenInfo_t       genInfo;
  
  static_cast<void>(read(BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR, genInfo.atamsVersionMajor));
  static_cast<void>(read(BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR, genInfo.atamsVersionMinor));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_DAY,         genInfo.genDay));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_MONTH,       genInfo.genMonth));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_YEAR,        genInfo.genYear));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_HOUR,        genInfo.genHour));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_MINUTE,      genInfo.genMinute));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_GEN_SECOND,      genInfo.genSecond));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_CHECKSUM,        genInfo.genChecksum));
  static_cast<void>(read(BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS,  genInfo.noOfVars));

  if ((genInfo != nullGenInfo         ) &&
      (genInfo == memoryMap_->genInfo) )
  {
    genInfoMatch = true;
  }

  return (genInfoMatch);
}

Atams::Error_t Node::externalTransfer(const Access_t  accessRequest,
                                      const uint16_t  varID,
                                      uint8_t * const bytesPtr,
                                      const uint8_t   length)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length ) return (Atams::ERROR_VAR_LENGTH); /* Early Return */
  if (bytesPtr                   == nullptr) return (Atams::ERROR_NULLPTR);   /* Early Return */

  Node::Var_t    &var        = varStorage_[varID];
  Atams::Error_t accessError = Atams::ERROR_NONE;

  varStorageLock_.acquireLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(bytesPtr, var.storage, TYPE_LENGTHS[varInfo.type]);
      if (systemIsBigEndian()) swapEndiannessRaw(bytesPtr, TYPE_LENGTHS[varInfo.type]);
      break;

    case ACCESS_WRITE:
      if (systemIsBigEndian()) swapEndiannessRaw(bytesPtr, TYPE_LENGTHS[varInfo.type]);
      memcpy(var.storage, bytesPtr, TYPE_LENGTHS[varInfo.type]);
      var.newDataReady = true;
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  varStorageLock_.releaseLock();

  return (accessError);
}

void Node::resetVars(void)
{
  varStorageLock_.acquireLock();

  for (Var_t &var : varStorage_) memset(var.storage, 0U, sizeof(var.storage));

  varStorageLock_.acquireLock();
}

bool Node::getMemoryMapIsValid(void)
{
  return ((memoryMap_    != nullptr) &&
          (validVarCount_ > 0U     ) );
}

void Node::invalidateMemoryMap(void)
{
  validVarCount_ = 0U;
  memoryMap_     = nullptr;
  resetVars();
}

/* Warning - no check of remaining response buffer length before copy, *
 * validateResponseBuffer must be called before using this function    */
bool Node::processDatagramRead(const DatagramHeader_t datagramHeader,
                                         uint16_t              &datagramStartIndex,
                                         const uint8_t          payloadLength)
{
  Atams::Error_t status = updateRequestPatternOnReceive(datagramHeader.varID);

  if (status == Atams::ERROR_NONE) status = externalTransfer(Atams::ACCESS_WRITE,
                                                           datagramHeader.varID,
                                                           &responseBuffer_[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                                                           payloadLength); 

  if (status == Atams::ERROR_NONE) datagramStartIndex += (DATAGRAM_SIZE_HEADER + payloadLength);
  else                             reportBusError(status);

  return (status != Atams::ERROR_NONE);
}

bool Node::processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t status = updateRequestPatternOnReceive(datagramHeader.varID);
  
  if (status == Atams::ERROR_NONE)
  {
    /* VarID validity confirmed in updateRequestPatternOnReceive*/
    varStorageLock_.acquireLock();
    varStorage_[datagramHeader.varID].ackReceived = true;
    varStorageLock_.releaseLock();
  }

  if (status == Atams::ERROR_NONE) datagramStartIndex += DATAGRAM_SIZE_HEADER;
  else                            reportBusError(status);
  
  return (status != Atams::ERROR_NONE);
}

bool Node::processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t statusReturn     = Atams::ERROR_NONE;
  bool           cancelProcessing = false;

  if (datagramHeader.varID < BlockUniversal::NUMBER_OF_UNIVERSAL_VARS)
  {
    Atams::Error_t statusReturn = updateRequestPatternOnReceive(datagramHeader.varID);

    if (statusReturn == Atams::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_CONFIGURATION_STATE_INACTIVE;
      datagramStartIndex += DATAGRAM_SIZE_HEADER;
    }
  }
  else 
  {
    cancelProcessing = true;
    statusReturn = Atams::ERROR_INVALID_NACK;
  }

  if (statusReturn != Atams::ERROR_NONE) reportBusError(statusReturn);

  return (cancelProcessing);
}

Atams::Error_t Node::validateResponseBuffer(uint8_t * const responsePacket,
                                            const uint16_t  responsePacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;
  uint8_t          varLength          = 0U;

  while (datagramStartIndex + DATAGRAM_SIZE_HEADER <= responsePacketLength)
  {
    bufferToDatagramHeader(&responsePacket[datagramStartIndex], datagramHeader);

    Atams::Error_t lengthStatus = getVarLength(datagramHeader.varID, varLength);

    if (lengthStatus != Atams::ERROR_NONE)
    {
      return (lengthStatus); /* Early Return */
    }

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {
      case Atams::RESPONSE_ACK_READ:
        datagramStartIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength);
        break;
      case Atams::RESPONSE_ACK_WRITE:
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;
      case Atams::RESPONSE_NACK:
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;
      default:
        return (Atams::ERROR_ACCESS_INVALID); /* Early Return */
        break;
    }
  }

  if (datagramStartIndex != responsePacketLength)
  {
    return (Atams::ERROR_RESPONSE_BUFFER_LENGTH); /* Early Return */
  }

  return (Atams::ERROR_NONE);
}

void Node::processResponseBuffer(void)
{
  if (!newResponseReady_)
  {
    reportBusError(Atams::ERROR_NO_RESPONSE);
    return; /* Early Return */
  }

  newResponseReady_ = false;

  if (responseBuffer_[MESH_INDEX_MSG_TYPE] == Atams::MESSAGE_ABORTED_RESPONSE)
  {
    processAbortedResponse();
    return; /* Early Return */
  }

  Atams::Error_t packetStatus = validateResponseBuffer(responseBuffer_, responseLength_);

  if (packetStatus != Atams::ERROR_NONE)
  {
    reportBusError(packetStatus);
    return; /* Early Return */
  }

  bool     cancelProcessing   = false;
  uint16_t datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= responseLength_) &&
         (cancelProcessing                          == false          ) )
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&responseBuffer_[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateResponseBuffer */
    uint8_t varLength = TYPE_LENGTHS[memoryMap_->varInfoList[datagramHeader.varID].type];

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {      
      case RESPONSE_ACK_READ:
        cancelProcessing = processDatagramRead(datagramHeader,
                                               datagramStartIndex,
                                               varLength);
        break;
      case RESPONSE_ACK_WRITE:
        cancelProcessing = processDatagramWrite(datagramHeader, datagramStartIndex);
        break;
      case RESPONSE_NACK:
        cancelProcessing = processDatagramNack(datagramHeader, datagramStartIndex);
        break;
      default:
        reportBusError(Atams::ERROR_ACCESS_RESPONSE_INVALID);
        cancelProcessing = true;
        break;
    }
  }
}

DataStatusReturn_t<bool> Node::findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig)
{
  DataStatusReturn_t<bool> statusReturn;
  statusReturn.status = Atams::ERROR_NONE;
  statusReturn.data   = false;
  uint8_t varLength   = 0U;

  changeConfig.datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while (changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacket_.length)    
  {
    bufferToDatagramHeader(&requestPacket_.buffer[changeConfig.datagramStartIndex], changeConfig.currentDatagramHeader);

    changeConfig.currentDatagramLength = DATAGRAM_SIZE_HEADER;

    if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE)
    {
      Atams::Error_t lengthStatus = getVarLength(changeConfig.currentDatagramHeader.varID, varLength);

      if (lengthStatus != Atams::ERROR_NONE)
      {
        statusReturn.status = Atams::ERROR_REQUEST_PACKET_FATAL;
        return (statusReturn); /* Early Return */
      }

      changeConfig.currentDatagramLength += varLength;
    }

    if (changeConfig.currentDatagramHeader.varID == changeConfig.newDatagramHeader.varID)
    {
      statusReturn.data = true;
      return (statusReturn); /* Early Return */
    }
    else 
    {
      changeConfig.datagramStartIndex += changeConfig.currentDatagramLength;
    }
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength)
{  
  if (static_cast<uint16_t>(requestPacket_.length + shiftLength) < Atams::MESH_SIZE_HEADER)
  {
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  }

  if (static_cast<uint16_t>(requestPacket_.length + shiftLength) > sizeof(requestPacket_.buffer))
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  memmove(&requestPacket_.buffer[shiftIndex + shiftLength], 
          &requestPacket_.buffer[shiftIndex], 
          (requestPacket_.length - shiftIndex));

  requestPacket_.length += shiftLength;

  requestPacket_.writeList.updateIndexes(shiftIndex, shiftLength);

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  uint16_t shiftIndex  =  changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength = -changeConfig.currentDatagramLength;
    
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  
  if (requestPacketShift(shiftIndex, shiftLength) != Atams::ERROR_NONE)
  {
    statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
  }
  else if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE) 
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.currentDatagramHeader.varID);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t  statusReturn = Atams::ERROR_NONE;
  uint16_t shiftIndex   = changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength  = changeConfig.newDatagramLength  - changeConfig.currentDatagramLength;

  bool writeListAdditionRequired = ((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
                                    (changeConfig.requestPattern == Atams::REQUEST_STREAM) );

  if (writeListAdditionRequired)
  {
    WriteList::WriteConfig_t writeConfig;

    writeConfig.varID              = changeConfig.newDatagramHeader.varID;
    writeConfig.requestPacketIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER);
    writeConfig.dataLength         = static_cast<uint8_t> (changeConfig.writePayloadLength);

    if (requestPacket_.writeList.addConfig(writeConfig) != WriteList::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_WRITE_LIST_FULL;
    }
  }
  else 
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  if ((statusReturn == Atams::ERROR_NONE) &&
      (shiftLength  != 0                ) )
  { 
    statusReturn = requestPacketShift(shiftIndex, shiftLength);
  }

  if (statusReturn == Atams::ERROR_NONE)
  {
    /* Copy new datagram into available space */
    memcpy(&requestPacket_.buffer[changeConfig.datagramStartIndex], 
           changeConfig.newDatagramBuffer, 
           changeConfig.newDatagramLength);
  }
  else if (writeListAdditionRequired)
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig)
{
  if ((requestPacket_.length + changeConfig.newDatagramLength) > sizeof(requestPacket_.buffer))
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  changeConfig.datagramStartIndex = requestPacket_.length;

  if ((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
      (changeConfig.requestPattern == Atams::REQUEST_STREAM) )
  {
    WriteList::WriteConfig_t writeConfigToAdd;

    writeConfigToAdd.varID              = changeConfig.newDatagramHeader.varID;
    writeConfigToAdd.requestPacketIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER);
    writeConfigToAdd.dataLength         = static_cast<uint8_t> (changeConfig.writePayloadLength);

    if (requestPacket_.writeList.addConfig(writeConfigToAdd) != WriteList::ERROR_NONE)
    {
      return (Atams::ERROR_WRITE_LIST_FULL); /* Early Return */
    }
  }

  /* Copy new datagram into available space + update request packet length */
  memcpy(&requestPacket_.buffer[requestPacket_.length], changeConfig.newDatagramBuffer, changeConfig.newDatagramLength);
  requestPacket_.length += changeConfig.newDatagramLength;

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::constructDatagramBuffer(RequestChangeConfig_t &changeConfig)
{
  Error_t statusReturn = Atams::ERROR_NONE;

  datagramHeaderToBuffer(changeConfig.newDatagramHeader, changeConfig.newDatagramBuffer);

  changeConfig.newDatagramLength = Atams::DATAGRAM_SIZE_HEADER;

  if (changeConfig.accessRequest == Atams::ACCESS_WRITE)
  {
    if (externalTransfer(Atams::ACCESS_READ, 
                         changeConfig.newDatagramHeader.varID, 
                         &changeConfig.newDatagramBuffer[DATAGRAM_INDEX_PAYLOAD], 
                         changeConfig.writePayloadLength))
    {
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
   
    changeConfig.newDatagramLength += changeConfig.writePayloadLength;
  }

  return (statusReturn);
}

void Node::resetRequestPacketNoLock(void)
{
  requestPacket_.length = Atams::MESH_SIZE_HEADER;
  requestPacket_.writeList.reset();
  for (uint16_t varID = 0U; varID < validVarCount_; varID++)
  {
    Node::Var_t &var = varStorage_[varID];
    var.requestAccess  = Atams::ACCESS_NONE;
    var.requestPattern = Atams::REQUEST_INACTIVE;
  }
}

/* Warning - No OOR checks, should be completed by calling function */
Atams::Error_t Node::processRequestPacketChange(const uint16_t         varID,
                                                const Access_t         accessRequest,
                                                const RequestPattern_t requestPattern)
{
  if (varID >= validVarCount_) 
  {
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }

  RequestChangeConfig_t packetChangeConfig;
  Atams::Error_t        statusReturn           = Atams::ERROR_NONE;
  packetChangeConfig.accessRequest             = accessRequest; 
  packetChangeConfig.requestPattern            = requestPattern;
  packetChangeConfig.writePayloadLength        = TYPE_LENGTHS[memoryMap_->varInfoList[varID].type];
  packetChangeConfig.newDatagramHeader.command = accessRequest;
  packetChangeConfig.newDatagramHeader.varID   = varID;

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramFoundInPacket = findDatagramMatchInPacket(packetChangeConfig);

  if (datagramFoundInPacket.status != Atams::ERROR_NONE)
  {
    resetRequestPacketNoLock();
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  } 

  if ((requestPattern == Atams::REQUEST_INACTIVE) ||
      (accessRequest  == Atams::ACCESS_NONE     ) )
  {
    if ((datagramFoundInPacket.data                             == true             ) &&
        (requestPacketRemoveCurrentDatagram(packetChangeConfig) != Atams::ERROR_NONE) )
    {
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
  }
  else
  {
    if (constructDatagramBuffer(packetChangeConfig) != Atams::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
    else if (datagramFoundInPacket.data == true) 
    {
      statusReturn = requestPacketAdjustCurrentDatagram(packetChangeConfig);
    }
    else                                         
    {
      statusReturn = requestPacketAppendDatagram(packetChangeConfig);
    }
  }

  if (statusReturn == Atams::ERROR_REQUEST_PACKET_FATAL) resetRequestPacketNoLock();

  return (statusReturn);
}

Atams::Error_t Node::updateRequestPatternOnReceive(const uint16_t varID)
{
  if (varID >= validVarCount_)
  {
    reportBusError(Atams::ERROR_VAR_ID);
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }
  
  requestPacketLock_.acquireLock();

  Node::Var_t &var = varStorage_[varID];

  Atams::Error_t   statusReturn          = Atams::ERROR_NONE;
  RequestPattern_t currentRequestPattern = var.requestPattern;
  
  if (currentRequestPattern == Atams::REQUEST_UNTIL_ACK)
  {
    statusReturn = processRequestPacketChange(varID, ACCESS_NONE, REQUEST_INACTIVE);

    if (statusReturn == Atams::ERROR_NONE)
    {
      var.requestAccess  = ACCESS_NONE;
      var.requestPattern = REQUEST_INACTIVE;
    }
  }

  requestPacketLock_.releaseLock();

  return (statusReturn);
}

/* Mesh Packet access must be properly locked before using this function */
Atams::Error_t Node::updateRequestPacketWriteData(void)
{
  WriteList::Return_t listReturn;

  uint16_t writeListLength = requestPacket_.writeList.getConfigCount();
  
  for (uint16_t writeListIndex = 0U; writeListIndex < writeListLength; writeListIndex++)
  {
    listReturn = requestPacket_.writeList.getConfigAtIndex(writeListIndex);
    
    if (listReturn.status != WriteList::ERROR_NONE)
    {
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
    else if (externalTransfer(Atams::ACCESS_READ,
                              listReturn.writeConfig.varID, 
                              &requestPacket_.buffer[listReturn.writeConfig.requestPacketIndex], 
                              listReturn.writeConfig.dataLength)!= Atams::ERROR_NONE)
    {
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
  }

  return (Atams::ERROR_NONE);
}

void Node::reportBusError(Atams::Error_t busError)
{
  busErrorLock_.acquireLock();
  if (busError_ == Atams::ERROR_NONE) busError_ = busError;
  busErrorLock_.releaseLock();
}

void Node::clearBusError(void)
{
  busErrorLock_.acquireLock();
  busError_ = Atams::ERROR_NONE;
  busErrorLock_.releaseLock();
}

void Node::responseReceived(uint8_t *inputBuffer, uint16_t inputLength)
{
  if ((inputBuffer != nullptr                ) &&
      (inputLength <= sizeof(responseBuffer_)) ) 
  {
    memcpy(responseBuffer_, inputBuffer, inputLength);
    responseLength_   = inputLength;
    newResponseReady_ = true;
  }
  else
  {
    reportBusError(Atams::ERROR_RESPONSE_BUFFER_LENGTH);
  }
}

void Node::processAbortedResponse(void)
{
  uint8_t  bufferVarIDHi = responseBuffer_[Atams::ABORT_INDEX_VAR_ID_HI];
  uint8_t  bufferVarIDLo = responseBuffer_[Atams::ABORT_INDEX_VAR_ID_LO];
  uint8_t  errorByte     = responseBuffer_[Atams::ABORT_INDEX_ERROR];
  uint16_t varID         = ((static_cast<uint16_t>(bufferVarIDHi & Atams::ABORT_MASK_VAR_ID_HI) << Atams::ABORT_SHIFT_VAR_ID_HI) |
                            (static_cast<uint16_t>(bufferVarIDLo & Atams::ABORT_MASK_VAR_ID_LO) << Atams::ABORT_SHIFT_VAR_ID_HI) );

  if ((responseLength_ != Atams::ABORT_SIZE_PACKET) ||
      (errorByte       >= NUMBER_OF_ATAMS_ERRORS  ) ||
      (varID           >= validVarCount_         ) ) 
  {
    reportBusError(Atams::ERROR_ABORT_FAILURE);
  }
  else
  {
    if (errorByte == Atams::ERROR_VAR_ID) updateRequestPatternOnReceive(varID);
    reportBusError(static_cast<Atams::Error_t>(errorByte));
  }
}

Atams::Error_t Node::getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                             const uint8_t              syncCount,
                                             uint8_t * const            outputBuffer,
                                             const uint16_t             outputBufferMaxLength, 
                                             uint16_t                  &outputLength)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  requestPacketLock_.acquireLock();

  if (updateRequestPacketWriteData() != Atams::ERROR_NONE)
  {
    resetRequestPacket();
    statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
  }
  else 
  {
    requestPacket_.buffer[MESH_INDEX_MSG_TYPE] = requestType;
    requestPacket_.buffer[MESH_INDEX_NODE_ID ] = nodeID_;
    requestPacket_.buffer[MESH_INDEX_SYNC]     = syncCount;
  
    statusReturn = encodeMeshPacket(requestPacket_.buffer, 
                                    requestPacket_.length, 
                                    outputBuffer, 
                                    outputBufferMaxLength, 
                                    outputLength);
  }

  requestPacketLock_.releaseLock();

  if (statusReturn != Atams::ERROR_NONE) reportBusError(statusReturn);

  return (statusReturn);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


