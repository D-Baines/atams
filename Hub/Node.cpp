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
#include "string.h"
#include "Node.hpp"
#include "Bus.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* STATIC CONSTANTS                                                                  */
/*************************************************************************************/

static const char * PLATFORM_TYPE_NAMES[Atams::NUMBER_OF_TYPES] =
{
  /* [TYPE_NULL  ] = */ "NULL",
  /* [TYPE_UINT8 ] = */ typeid(uint8_t ).name(),
  /* [TYPE_INT8  ] = */ typeid(int8_t  ).name(),
  /* [TYPE_UINT16] = */ typeid(uint16_t).name(),
  /* [TYPE_INT16 ] = */ typeid(int16_t ).name(),
  /* [TYPE_UINT32] = */ typeid(uint32_t).name(),
  /* [TYPE_INT32 ] = */ typeid(int32_t ).name(),
  /* [TYPE_FLOAT ] = */ typeid(float   ).name(),
};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Node::Node(Bus &bus, const uint8_t nodeID) :
_bus(bus),
_nodeID(nodeID)
{

}

Atams::Error_t Node::init(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap);

  if (initStatus == Atams::ERROR_NONE) initStatus = _bus.addNodeToBus(*this);

  if (initStatus != Atams::ERROR_NONE) invalidateMemoryMap();
  
  return (initStatus);
}

template <typename T>
Atams::Error_t Node::write(const uint16_t varID, const T writeData)
{
  if (varID >= m_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  if (PLATFORM_TYPE_NAMES[varInfo.type] != typeid(T).name()   ) return (ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_WRITE                       > varInfo.accessLevel) return (ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = m_varStorage[varID];

  m_varStorageLock.acquireLock();

  memcpy(var.storage, &writeData, sizeof(writeData));

  m_varStorageLock.releaseLock();

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
  if (varID >= m_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  if (PLATFORM_TYPE_NAMES[varInfo.type] != typeid(T).name()    ) return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ                        >  varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = m_varStorage[varID];

  m_varStorageLock.acquireLock();

  memcpy(&readData, var.storage, sizeof(readData));

  m_varStorageLock.releaseLock();

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
Atams::Error_t Node::readIfNew(const uint16_t varID, T &readData)
{
  if (varID >= m_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  if (PLATFORM_TYPE_NAMES[varInfo.type] != typeid(T).name()    ) return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ                        >  varInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t   &var          = m_varStorage[varID];
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  m_varStorageLock.acquireLock();

  if (var.newDataReady) memcpy(&readData, &var.storage, sizeof(readData));
  else                  statusReturn = Atams::ERROR_OLD_DATA;

  m_varStorageLock.releaseLock();

  return (statusReturn);
}

template Atams::Error_t Node::readIfNew<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::readIfNew<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::readIfNew<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::readIfNew<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::readIfNew<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::readIfNew<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::readIfNew<float   >(const uint16_t varID, float    &readData);

DataStatusReturn_t<uint8_t> Node::getMemberLength(const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (varID >= m_validVarCount)
  {
    lengthReturn.status = Atams::ERROR_VAR_ID;
    return (lengthReturn); /* Early Return */
  }

  const Atams::VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  lengthReturn.data   = TYPE_LENGTHS[varInfo.type];
  lengthReturn.status = Atams::ERROR_NONE;

  return (lengthReturn);
}

/* RE-EVAULATE THIS AND ALL REQUEST PACKET HANDLING */
Atams::Error_t Node::setRequestPattern(const uint16_t         varID,
                                       const Access_t         accessRequest,
                                       const RequestPattern_t requestPattern)
{ 
  if (varID          >= m_validVarCount)                   return (Atams::ERROR_VAR_ID);                  /* Early Return */
  if (requestPattern >= Atams::NUMBER_OF_REQUEST_PATTERNS) return (Atams::ERROR_REQUEST_PATTERN_INVALID); /* Early Return */
  
  Node::Var_t     &var     = m_varStorage[varID];
  const VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  if (accessRequest > varInfo.accessLevel)
  {
    return (Atams::ERROR_ACCESS_INVALID); /* Early Return */
  }

  m_requestPacketLock.acquireLock();

  Atams::Error_t statusReturn = processRequestPacketChange(varID, accessRequest, requestPattern);
  
  if (statusReturn == Atams::ERROR_NONE)
  {
    var.requestAccess  = accessRequest;
    var.requestPattern = requestPattern;
  }

  m_requestPacketLock.releaseLock();

  return (statusReturn);
}

Atams::Error_t Node::getRequestPattern(const uint16_t   varID,
                                       Access_t         &accessRequest,
                                       RequestPattern_t &requestPattern)
{
  if (varID >= m_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = m_varStorage[varID];

  m_requestPacketLock.acquireLock();
 
  accessRequest  = var.requestAccess;
  requestPattern = var.requestPattern;

  m_requestPacketLock.releaseLock();

  return (Atams::ERROR_NONE);
}

uint8_t Node::getNodeID(void)
{
  return (_nodeID);
}

Atams::Error_t Node::getBusError(void)
{
  return (_busError);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::Error_t Node::externalTransfer(const Access_t  accessRequest,
                                      const uint16_t  varID,
                                      uint8_t * const bytesPtr,
                                      const uint8_t   length)
{
  if (varID >= m_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = m_memoryMap->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length ) return (Atams::ERROR_VAR_LENGTH);
  if (bytesPtr                   == nullptr) return (Atams::ERROR_NULL_PTR);

  Node::Var_t    &var        = m_varStorage[varID];
  Atams::Error_t accessError = Atams::ERROR_NONE;

  m_varStorageLock.acquireLock();

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

  m_varStorageLock.releaseLock();

  return (accessError);
}

void Node::resetVars(void)
{
  m_varStorageLock.acquireLock();

  for (Var_t &var : m_varStorage) memset(var.storage, 0U, sizeof(var.storage));

  m_varStorageLock.acquireLock();
}

bool Node::getMemoryMapIsValid(void)
{
  return ((m_memoryMap    != nullptr) &&
          (m_validVarCount > 0U     ) );
}

void Node::invalidateMemoryMap(void)
{
  m_validVarCount = 0U;
  m_memoryMap     = nullptr;
  resetVars();
}

bool Node::validateMapLength(const MemoryMap_t &memoryMap)
{
  bool     lengthValid = true;
  uint16_t varIndex    = 0U;

  if ((memoryMap.noOfVars > sizeof(m_varStorage)                    ) ||
      (memoryMap.noOfVars > Atams::MAX_NUMBER_OF_VARS               ) ||
      (memoryMap.noOfVars < BlockUniversal::NUMBER_OF_UNIVERSAL_VARS) )
  {
    lengthValid = false;
  }

  for (const VarInfo_t &varInfo : memoryMap.varInfoList)
  {
    if ((varInfo.type        == Atams::TYPE_NULL  ) ||
        (varInfo.accessLevel == Atams::ACCESS_NONE) )
    {
      if (varIndex != memoryMap.noOfVars) lengthValid = false;
      break;
    }

    varIndex++;
  }

  return (lengthValid);
}

bool Node::validateUniversalBlock(const MemoryMap_t &memoryMap)
{
  bool     universalValid = true;
  uint16_t varIndex       = 0U;

  for (VarInfo_t universalVarInfo : BlockUniversal::varInfoList)
  {
    VarInfo_t mapVarInfo = memoryMap.varInfoList[varIndex];

    if (universalVarInfo != mapVarInfo) universalValid = false;

    varIndex++;
  }

  return (universalValid);
}

bool Node::validateMapChecksum(const MemoryMap_t &memoryMap)
{
  bool     checksumValid = false;
  uint16_t varIndex      = 0U;

  s_nodeCRC.beginRollingCRC();

  for (const VarInfo_t &varInfo : memoryMap.varInfoList)
  {
    if (varIndex == memoryMap.noOfVars) break;

    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.accessLevel));
    s_nodeCRC.updateRollingCRC(static_cast<uint8_t>(varInfo.NVMStorage));

    varIndex++;
  }

  if (memoryMap.genInfo.genChecksum == s_nodeCRC.getRollingCRC())
  {
    checksumValid = true;
  }

  return (checksumValid);
}

Atams::Error_t Node::validateMemoryMap(const MemoryMap_t &memoryMap)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((validateMapLength(memoryMap)      == false) ||
      (validateUniversalBlock(memoryMap) == false) ||
      (validateMapChecksum(memoryMap)    == false) )
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }
  else
  {
    m_memoryMap     = &memoryMap;
    m_validVarCount = memoryMap.noOfVars;
  }

  return (statusReturn);
}

Atams::Error_t Node::getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                             uint8_t * const            outputBuffer,
                                             const uint16_t             outputBufferMaxLength, 
                                             uint16_t                  &outputLength)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  m_requestPacketLock.acquireLock();

  if (updateRequestPacketWriteData() != Atams::ERROR_NONE)
  {
    statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
  }

  _requestPacket.buffer[MESH_INDEX_MSG_TYPE] = requestType;
  _requestPacket.buffer[MESH_INDEX_NODE_ID ] = _nodeID;

  statusReturn = encodeMeshPacket(_requestPacket.buffer, 
                                  _requestPacket.length, 
                                  outputBuffer, 
                                  outputBufferMaxLength, 
                                  outputLength);

  m_requestPacketLock.releaseLock();

  if (statusReturn != Atams::ERROR_NONE) reportBusError(statusReturn);

  return (statusReturn);
}

void Node::responseReceived(uint8_t *inputBuffer, uint16_t inputLength)
{
  if ((inputBuffer != nullptr                ) &&
      (inputLength <= sizeof(_responseBuffer)) ) 
  {
    memcpy(_responseBuffer, inputBuffer, inputLength);
    _responseLength   = inputLength;
    _newResponseReady = true;
  }
  else
  {
    reportBusError(Atams::ERROR_REQUEST_BUFFER_LENGTH);
  }
}

void Node::reportBusError(Atams::Error_t busError)
{
  if (_busError == Atams::ERROR_NONE) _busError = busError;
}

void Node::clearBusError(void)
{
  _busError = Atams::ERROR_NONE;
}

void Node::processAbortedResponse(void)
{
  if ((_responseLength != (MESH_SIZE_HEADER + sizeof(Atams::Error_t))) ||
      (_responseBuffer[MESH_SIZE_HEADER] >= NUMBER_OF_ATAMS_ERRORS   ) ) 
  {
    reportBusError(Atams::ERROR_ABORT_FAILURE);
  }
  else
  {
    reportBusError(static_cast<Atams::Error_t>(_responseBuffer[MESH_SIZE_HEADER]));
  }
}

bool Node::processDatagramRead(const DatagramHeader_t datagramHeader,
                               uint16_t              &datagramStartIndex,
                               const uint8_t          payloadLength)
{
  const uint16_t datagramLength       = DATAGRAM_SIZE_HEADER + payloadLength;
  const uint16_t remainingInputLength = _responseLength - datagramStartIndex;
  
  if (datagramLength > remainingInputLength)
  {
    reportBusError(Atams::ERROR_RESPONSE_BUFFER_LENGTH);
    return (true); /* Early Return */
  }

  Atams::Error_t transferStatus = externalTransfer(Atams::ACCESS_WRITE,
                                                   datagramHeader.varID,
                                                   &_responseBuffer[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                                                   payloadLength); 

  if (transferStatus == Atams::ERROR_NONE) 
  {
    transferStatus = updateRequestPatternOnReceive(datagramHeader.varID);
  }
  
  if (transferStatus != Atams::ERROR_NONE) 
  {
    reportBusError(transferStatus);
    return (true);
  }
  else 
  {
    datagramStartIndex += (DATAGRAM_SIZE_HEADER + payloadLength);
    return (false);
  }
}

bool Node::processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t transferStatus = updateRequestPatternOnReceive(datagramHeader.varID);

  if (transferStatus != Atams::ERROR_NONE) 
  {
    reportBusError(transferStatus);
    return (true);
  }
  else 
  {
    datagramStartIndex += DATAGRAM_SIZE_HEADER;
    return (false);
  }
}

bool Node::processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  if (datagramHeader.blockID == Atams::BLOCK_ID_UNIVERSAL)
  {
    reportBusError(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
    datagramStartIndex += DATAGRAM_SIZE_HEADER;
    return (false);
  }
  else 
  {
    reportBusError(Atams::ERROR_INVALID_NACK);
    return (true);
  }
}

void Node::processResponseBuffer(void)
{
  if (!_newResponseReady)
  {
    reportBusError(Atams::ERROR_NO_RESPONSE);
    return; /* Early Return */
  }

  if (_responseBuffer[MESH_INDEX_MSG_TYPE] == MESSAGE_ABORTED_RESPONSE)
  {
    processAbortedResponse();
    return; /* Early Return */
  }

  bool     cancelProcessing   = false;
  uint16_t datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= _responseLength) &&
         (cancelProcessing                          == false          ) )
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&_responseBuffer[datagramStartIndex], datagramHeader);

    DataStatusReturn_t<uint8_t> varLength = getMemberLength(datagramHeader.varID);
  
    if (varLength.status != Atams::ERROR_NONE)
    {
      reportBusError(varLength.status);
      cancelProcessing = true;
      break;
    }

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {      
      case RESPONSE_ACK_READ:
        cancelProcessing = processDatagramRead(datagramHeader,
                                               datagramStartIndex,
                                               varLength.data);
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

  /* TODO:: Move Length Check To Start */
  if ((cancelProcessing   == false          ) &&
      (datagramStartIndex != _responseLength) )
  {
    reportBusError(Atams::ERROR_RESPONSE_BUFFER_LENGTH);
  }
}

DataStatusReturn_t<bool> Node::findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig)
{
  DataStatusReturn_t<bool> statusReturn;
  statusReturn.status = Atams::ERROR_NONE;
  statusReturn.data   = false;

  changeConfig.datagramStartIndex = MESH_INDEX_FIRST_DATAGRAM;

  while (changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER <= _requestPacket.length)    
  {
    bufferToDatagramHeader(&_requestPacket.buffer[changeConfig.datagramStartIndex], changeConfig.currentDatagramHeader);

    changeConfig.currentDatagramLength = DATAGRAM_SIZE_HEADER;

    if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE)
    {
      DataStatusReturn_t<uint8_t> payloadLength = getMemberLength(changeConfig.currentDatagramHeader.varID);

      if (payloadLength.status != Atams::ERROR_NONE)
      {
        statusReturn.status = Atams::ERROR_REQUEST_PACKET_FATAL;
        return (statusReturn); /* Early Return */
      }

      changeConfig.currentDatagramLength += payloadLength.data;
    }

    if (changeConfig.currentDatagramHeader.varID == changeConfig.newDatagramHeader.varID)
    {
      statusReturn.data = true;
      return (statusReturn); /* Early Return */
    }
    else /* Current datagram != new datagram */
    {
      changeConfig.datagramStartIndex += changeConfig.currentDatagramLength;
    }
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength)
{  
  if ((_requestPacket.length + shiftLength) > MAX_MESH_PACKET_SIZE)
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH);
  }

  memmove(&_requestPacket.buffer[shiftIndex + shiftLength], 
          &_requestPacket.buffer[shiftIndex], 
          (_requestPacket.length - shiftIndex));

  _requestPacket.length += shiftLength;

  _requestPacket.writeList.updateIndexes(shiftIndex, shiftLength);

  return (ERROR_NONE);
}

Atams::Error_t Node::requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  /* Node packet still contains other datagrams - only remove datagram from Mesh packet */
  uint16_t shiftIndex  =  changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength = -changeConfig.currentDatagramLength;
    
  Atams::Error_t statusReturn = requestPacketShift(shiftIndex, shiftLength);
  
  if ((statusReturn                               == Atams::ERROR_NONE  ) &&
      (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE) )
  {
    _requestPacket.writeList.removeConfigIfFound(changeConfig.currentDatagramHeader.varID);
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
    WriteList::WriteConfig_t writeConfig =
    {
      /*.varID               = */ changeConfig.currentDatagramHeader.varID, 
      /*.meshPacketDataIndex = */ static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER),
      /*.dataLength          = */ static_cast<uint8_t> (changeConfig.writePayloadLength)
    };

    if (_requestPacket.writeList.addConfig(writeConfig) != WriteList::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_WRITE_LIST;
    }
  }
  else
  {
    _requestPacket.writeList.removeConfigIfFound(changeConfig.varID);
  }

  if ((statusReturn == Atams::ERROR_NONE) &&
      (shiftLength  != 0                ) )
  { 
    statusReturn = requestPacketShift(shiftIndex, shiftLength);
  }

  if (statusReturn == Atams::ERROR_NONE)
  {
    /* Copy new datagram into available space */
    memcpy(&_requestPacket.buffer[changeConfig.datagramStartIndex], 
           changeConfig.newDatagramBuffer, 
           changeConfig.newDatagramLength);
  }
  else if (writeListAdditionRequired)
  {
    _requestPacket.writeList.removeConfigIfFound(changeConfig.varID);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig)
{
  if ((_requestPacket.length + changeConfig.newDatagramLength) > MAX_NODE_PACKET_SIZE)
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  changeConfig.datagramStartIndex = _requestPacket.length;

  if ((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
      (changeConfig.requestPattern == Atams::REQUEST_STREAM) )
  {
    WriteList::WriteConfig_t writeConfigToAdd =
    {
      .varID               = changeConfig.newDatagramHeader.varID, 
      .meshPacketDataIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER),
      .dataLength          = static_cast<uint8_t> (changeConfig.writePayloadLength)
    };

    if (_requestPacket.writeList.addConfig(writeConfigToAdd) != WriteList::ERROR_NONE)
    {
      return (Atams::ERROR_WRITE_LIST); /* Early Return */
    }
  }

  /* Copy new datagram into available space + update request packet length */
  memcpy(&_requestPacket.buffer[_requestPacket.length], changeConfig.newDatagramBuffer, changeConfig.newDatagramLength);
  _requestPacket.length += changeConfig.newDatagramLength;

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::constructDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t statusReturn = Atams::ERROR_NONE;

  /* Construct new datagram to be added to mesh packet */
  changeConfig.newDatagramHeader.command  = changeConfig.accessRequest;
  changeConfig.newDatagramHeader.varID    = changeConfig.varID;

  datagramHeaderToBuffer(changeConfig.newDatagramHeader, changeConfig.newDatagramBuffer);

  changeConfig.newDatagramLength = Atams::DATAGRAM_SIZE_HEADER;

  if (changeConfig.accessRequest == Atams::ACCESS_WRITE)
  {
    statusReturn = externalTransfer(Atams::ACCESS_READ, 
                                    changeConfig.varID, 
                                    &changeConfig.newDatagramBuffer[DATAGRAM_INDEX_PAYLOAD], 
                                    changeConfig.writePayloadLength);
   
    changeConfig.newDatagramLength += changeConfig.writePayloadLength;
  }

  return (statusReturn);
}

/* Warning - No OOR checks, should be completed by calling function */
Atams::Error_t Node::processRequestPacketChange(const uint16_t         varID,
                                                const Access_t         accessRequest,
                                                const RequestPattern_t requestPattern)
{
  if (varID >= m_validVarCount) 
  {
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }

  RequestChangeConfig_t packetChangeConfig;
  Atams::Error_t        statusReturn    = Atams::ERROR_NONE;
  packetChangeConfig.accessRequest      = accessRequest; 
  packetChangeConfig.requestPattern     = requestPattern;
  packetChangeConfig.varID              = varID;
  packetChangeConfig.writePayloadLength = TYPE_LENGTHS[m_memoryMap->varInfoList[varID].type];

  statusReturn = constructDatagram(packetChangeConfig);

  if (statusReturn != Atams::ERROR_NONE)
  {
    return (statusReturn); /* Early Return */
  }

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramFoundInPacket = findDatagramMatchInPacket(packetChangeConfig);

  if (datagramFoundInPacket.status != Atams::ERROR_NONE)
  {
    return (datagramFoundInPacket.status); /* Early Return */
  } 
  
  if (datagramFoundInPacket.data == true)
  {
    if ((requestPattern == Atams::REQUEST_INACTIVE) ||
        (accessRequest  == Atams::ACCESS_NONE     ) )
    {
      statusReturn = requestPacketRemoveCurrentDatagram(packetChangeConfig);
    }
    else /* commandPattern != COMMAND_INACTIVE && accessRequest != ACCESS_NONE */
    {
      if (datagramFoundInPacket.data == true)
      {
        statusReturn = requestPacketAdjustCurrentDatagram(packetChangeConfig);
      }
      else                                   
      {
        statusReturn = requestPacketAppendDatagram(packetChangeConfig);
      }
    }
  }

  return (statusReturn);
}

Atams::Error_t Node::updateRequestPatternOnReceive(const uint16_t varID)
{
  if (varID >= m_validVarCount)
  {
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }
  
  m_requestPacketLock.acquireLock();

  Node::Var_t &var = m_varStorage[varID];

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

  m_requestPacketLock.releaseLock();

  return (statusReturn);
}

/* Mesh Packet access must be properly locked before using this function */
Atams::Error_t Node::updateRequestPacketWriteData(void)
{
  Atams::Error_t      statusReturn = Atams::ERROR_NONE;
  WriteList::Return_t listReturn;

  uint16_t writeListLength = _requestPacket.writeList.getConfigCount();
  
  for (uint16_t writeListIndex = 0U; writeListIndex < writeListLength; writeListIndex++)
  {
    listReturn = _requestPacket.writeList.getConfigAtIndex(writeListIndex);
    
    if (listReturn.status == WriteList::ERROR_NONE)
    {
      
      statusReturn = (externalTransfer(Atams::ACCESS_READ,
                                       listReturn.writeConfig.varID, 
                                       &_requestPacket.buffer[listReturn.writeConfig.meshPacketDataIndex], 
                                       listReturn.writeConfig.dataLength));
    }
    else
    {
      statusReturn = Atams::ERROR_WRITE_LIST;
    }
  }

  return (statusReturn);
}

bool Node::validateGenInfo(void)
{
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

  if ((genInfo != nullGenInfo         ) &&
      (genInfo == m_memoryMap->genInfo) )
  {
    genInfoMatch = true;
  }

  return (genInfoMatch);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


