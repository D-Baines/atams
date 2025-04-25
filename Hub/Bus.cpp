/**
  ******************************************************************************
  * @file    Bus.cpp
  *
  * @author  D. Baines
  *
  * @brief
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

#include "Bus.hpp"
#include "Node.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Maps/BlockUniversal.hpp"
#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Bus::Bus(Platform::BusPeripheral::UserData_t userData) :
Platform::BusPeripheral(userData)
{
  for (uint8_t ptrIndex = 0U; ptrIndex < Platform::NUMBER_OF_NODES_PER_BUS; ptrIndex++)
  {
    _nodePtrs[ptrIndex] = nullptr; 
  }
}


Bus::~Bus(void)
{

}

Atams::Error_t Bus::addNodeToBus(Node &node)
{
  if (_noOfNodesOnBus >= Platform::NUMBER_OF_NODES_PER_BUS)
  {
    return (Atams::ERROR_BUS_FULL); /* Early Return */
  }

  _nodePtrs[_noOfNodesOnBus] = &node;
  _noOfNodesOnBus++;

  return (Atams::ERROR_NONE);
}

void Bus::removeNodeFromBus(Node &node)
{
  uint8_t ptrIndex  = 0U;
  bool    nodeFound = false;

  for (Node * nodePtr : _nodePtrs)
  {
    if (nodePtr == &node)
    {
      nodeFound = true;
      break;
    }
    
    ptrIndex++;
  }

  if (nodeFound)
  {
    while (ptrIndex < (Platform::NUMBER_OF_NODES_PER_BUS - 1U))
    {
      _nodePtrs[ptrIndex     ] = _nodePtrs[ptrIndex + 1U];
      _nodePtrs[ptrIndex + 1U] = nullptr;
    }

    _noOfNodesOnBus--;
  }
}

Atams::Error_t Bus::startUpdateCycle(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  
  if (_updateState == Bus::UPDATE_STATE_READY)
  {
    for (Node * nodePtr : _nodePtrs)
    {
      if (nodePtr != nullptr) nodePtr->clearBusError();
    }

    m_activeNodeIndex = 0U;
    _updateState     = Bus::UPDATE_STATE_SEND_REQUESTS;
  }
  else if (_updateState == Bus::UPDATE_STATE_INIT_REQUIRED)
  {
    statusReturn = Atams::ERROR_INIT_REQUIRED;
  }
  else
  {
    statusReturn = Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS;
  }

  return (statusReturn);
}

bool Bus::runUpdateCycle(void)
{
  uint64_t currentTime   = Platform::getMillis();
  Node    *activeNodePtr = nullptr;

  if ((m_activeNodeIndex < sizeof(_nodePtrs)) &&
      (m_activeNodeIndex < _noOfNodesOnBus  ) )
  {
    activeNodePtr = _nodePtrs[m_activeNodeIndex];
  }

  bool allNodesHandled = (activeNodePtr == nullptr);

  BusPeripheral::update();
  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  switch (_updateState)
  {
    case Bus::UPDATE_STATE_INIT_REQUIRED:
      /* Do Nothing */
      break;
    case Bus::UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case Bus::UPDATE_STATE_SEND_REQUESTS:
      if (allNodesHandled)
      {
        m_activeNodeIndex  = 0U;
        _prevResponseTime = currentTime;
        _updateState      = Bus::UPDATE_STATE_COLLECT_RESPONSES;
      }
      else if (Platform::BusPeripheral::transmitReady())
      {
        if (activeNodePtr->getEncodedRequestPacket(Atams::MESSAGE_REQUEST_SYNCED,
                                                   _encodedBuffer, 
                                                   sizeof(_encodedBuffer), 
                                                   _encodedLength        ) == Atams::ERROR_NONE)
        {
          Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
        }
        m_activeNodeIndex++;
      }
      break;
    case Bus::UPDATE_STATE_COLLECT_RESPONSES:
      if (allNodesHandled)
      {
        m_activeNodeIndex = 0U;
        _updateState     = Bus::UPDATE_STATE_CYCLE_COMPLETE;
      }
      else if (m_circularBuffer.getPacket(_rxBuffer, sizeof(_rxBuffer), _rxLength) == CircularBuffer::ERROR_NONE)
      {
        if (validateAndStoreResponsePacket(*activeNodePtr, Atams::MESSAGE_RESPONSE_SYNCED) != Atams::ERROR_DECODE)
        {
          m_activeNodeIndex++;
        }
      }
      else if (currentTime - _prevResponseTime > Platform::BUS_RESPONSE_TIMEOUT)
      { 
        activeNodePtr->reportBusError(ERROR_RESPONSE_TIMEOUT);
        m_activeNodeIndex++;
        _updateState = Bus::UPDATE_STATE_JOG_NODE;
      }
      break;
    case Bus::UPDATE_STATE_JOG_NODE:
      if (allNodesHandled)
      {
        m_activeNodeIndex   = 0U;
        _updateState        = Bus::UPDATE_STATE_CYCLE_COMPLETE;
      }
      else
      {
        _jogBuffer[MESH_INDEX_NODE_ID] = _nodePtrs[m_activeNodeIndex]->getNodeID();

        if (Platform::BusPeripheral::transmitReady()) 
        {
          if (encodeMeshPacket(_jogBuffer, 
                               MESH_SIZE_HEADER, 
                               _encodedBuffer, 
                               sizeof(_encodedBuffer), 
                               _encodedLength) == Atams::ERROR_NONE)
          {
            Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
          }
          _prevResponseTime = currentTime;
          _updateState      = Bus::UPDATE_STATE_COLLECT_RESPONSES;
        }
      }
      break;
    case Bus::UPDATE_STATE_CYCLE_COMPLETE:
      break;
    default:
      /* TODO:: Handle error correctly */
      break;
  }

  return (_updateState == UPDATE_STATE_CYCLE_COMPLETE);
}

Atams::Error_t Bus::processBuffers(void)
{
  Atams::Error_t processStatus = Atams::ERROR_NONE;

  if (_updateState != UPDATE_STATE_CYCLE_COMPLETE) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS);
  }

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr != nullptr) 
    {
      nodePtr->processResponseBuffer();
      if (nodePtr->getBusError() != Atams::ERROR_NONE) processStatus = Atams::ERROR_BUS_PROCESSING;
    }
  }

  _updateState = UPDATE_STATE_READY;

  return (processStatus);
}

Atams::ProcessState_t Bus::updateBusInitProcess(Atams::Error_t &error)
{
  switch (_initStateMajor)
  {
    case Atams::PROCESS_STATE_READY:
      _initStateMajor = Atams::PROCESS_STATE_IN_PROGRESS;
      break;
    case Atams::PROCESS_STATE_IN_PROGRESS:
      Bus::updateInitProcessMinor(error);
      if      (_initStateMinor == Bus::INIT_STATE_SUCCESS) _initStateMajor = Atams::PROCESS_STATE_COMPLETE; 
      else if (_initStateMinor == Bus::INIT_STATE_ERROR)   _initStateMajor = Atams::PROCESS_STATE_ERROR;
      break;
    case Atams::PROCESS_STATE_COMPLETE:
      _initStateMajor = Atams::PROCESS_STATE_READY;
      break;
    case Atams::PROCESS_STATE_ERROR:
      _initStateMajor = Atams::PROCESS_STATE_READY;
    default:
      break;
  }

  return (_initStateMajor);
}

Atams::ProcessState_t Bus::updateSetNodeConfigProcess(const uint8_t         nodeIDToSet, 
                                                    const BitrateOption_t bitrateOption,
                                                    const uint32_t        watchdogPeriod,
                                                    Atams::Error_t       &error)
{

}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void Bus::updateInitProcessMinor(Atams::Error_t &error)
{
  switch (_initStateMinor)
  {
    case Bus::INIT_STATE_START_UPDATE_CYCLE:
      error           = startUpdateCycle();
      _initStateMinor = INIT_STATE_BUS_UPDATE;
      break;
    case Bus::INIT_STATE_BUS_UPDATE:
      if (updateNoSync() == Bus::UPDATE_STATE_CYCLE_COMPLETE)
      {
        error = processBuffers();
        _initStateMinor = _nextInitStateMinor;
      }
      break;
    case Bus::INIT_STATE_START_PERIPHERAL:
      if (!BusPeripheral::startPeripheral()) error = Atams::ERROR_PLATFORM;
      else                                   error = triggerGenInfoCollectionAllNodes();
      _nextInitStateMinor = Bus::INIT_STATE_VALIDITY_CHECKS;
      _initStateMinor     = Bus::INIT_STATE_START_UPDATE_CYCLE;
      break;

  }

  if (error != Atams::ERROR_NONE)
  {
    _initStateMinor = Bus::INIT_STATE_ERROR;
  }
}


void updateSetNodeConfigProcessMinor(const uint8_t         nodeIDToSet, 
                                     const BitrateOption_t bitrateOption,
                                     const uint32_t        watchdogPeriod,
                                     Atams::Error_t       &error)
{
 
}

void Bus::rxCallback(      uint8_t  *rxBufferPtr,
                     const uint16_t  rxBufferLength)
{
  m_circularBuffer.pushHead(rxBufferPtr, rxBufferLength);
  //Platform::CommsSemaphore::signal(); 
}

Atams::Error_t Bus::validateAndStoreResponsePacket(Node &node, const MessageType_t responseType)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (decodeMeshPacket(_rxBuffer, 
                       _rxLength, 
                       _decodedBuffer, 
                       sizeof(_decodedBuffer), 
                       _decodedLength) == Atams::ERROR_NONE)
  {
    uint8_t packetNodeID    = _decodedBuffer[MESH_INDEX_NODE_ID];
    uint8_t packetSyncCount = _decodedBuffer[MESH_INDEX_SYNC];
    uint8_t messageType     = _decodedBuffer[MESH_INDEX_MSG_TYPE];

    if      (packetSyncCount != _activeSyncCount)            statusReturn = Atams::ERROR_SYNC_COUNT;
    else if (packetNodeID    != node.getNodeID())            statusReturn = Atams::ERROR_SYNC_NODE;
    else if ((messageType    != responseType            ) &&
             (messageType    != MESSAGE_ABORTED_RESPONSE) )  statusReturn = Atams::ERROR_MESSAGE_TYPE;
    else                                                     node.responseReceived(_decodedBuffer, _decodedLength);

    if (statusReturn != Atams::ERROR_NONE) node.reportBusError(statusReturn);
  }
  else 
  {
    statusReturn = Atams::ERROR_DECODE;
  }
  
  return (statusReturn);
}

Bus::UpdateState_t Bus::updateNoSync(void)
{
  uint64_t currentTime   = Platform::getMillis();
  Node    *activeNodePtr = nullptr;
  
  if ((m_activeNodeIndex < sizeof(_nodePtrs)) &&
      (m_activeNodeIndex < _noOfNodesOnBus  ) )
  {
    activeNodePtr = _nodePtrs[m_activeNodeIndex];
  }
  else if (_updateState != Bus::UPDATE_STATE_READY)
  {
    m_activeNodeIndex = 0U;
    _updateState      = UPDATE_STATE_CYCLE_COMPLETE;
  }

  BusPeripheral::update();
  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  switch (_updateState)
  {
    case Bus::UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case Bus::UPDATE_STATE_SEND_REQUESTS:
      if (Platform::BusPeripheral::transmitReady())
      {
        if (activeNodePtr->getEncodedRequestPacket(Atams::MESSAGE_REQUEST,
                                                   _encodedBuffer, 
                                                   sizeof(_encodedBuffer), 
                                                   _encodedLength        ) == Atams::ERROR_NONE)
        {
          Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
          _prevRequestTime = currentTime;
          _updateState     = UPDATE_STATE_COLLECT_RESPONSES;
        }
        else m_activeNodeIndex++;
      }
      break;
    case Bus::UPDATE_STATE_COLLECT_RESPONSES:
      if (m_circularBuffer.getPacket(_rxBuffer, sizeof(_rxBuffer), _rxLength) == CircularBuffer::ERROR_NONE)
      {
        if (validateAndStoreResponsePacket(*activeNodePtr, Atams::MESSAGE_RESPONSE) != Atams::ERROR_DECODE)
        {
          m_activeNodeIndex++;
          _updateState = UPDATE_STATE_SEND_REQUESTS;
        }
      }
      else if (currentTime - _prevRequestTime > Platform::BUS_RESPONSE_TIMEOUT)
      { 
        activeNodePtr->reportBusError(Atams::ERROR_RESPONSE_TIMEOUT);
        m_activeNodeIndex++;
        _updateState = UPDATE_STATE_SEND_REQUESTS;
      }
      break;
    case UPDATE_STATE_CYCLE_COMPLETE:
      /* Do Nothing */
      break;
    default:
      /* TODO:: Handle error correctly */
      break;
  } 

  return (_updateState);
}

Atams::Error_t Bus::triggerGenInfoCollectionAllNodes(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr == nullptr) break;
    
    for (uint16_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; varID <= BlockUniversal::VAR_ID_MAP_CHECKSUM; varID++)
    {
      statusReturn = nodePtr->setRequestPattern(varID, 
                                                Atams::ACCESS_READ, 
                                                Atams::REQUEST_UNTIL_ACK);

      if (statusReturn != Atams::ERROR_NONE) 
      {
        return (statusReturn); /* Early Return */
      }
    }
    
  }

  return (statusReturn);
}

Atams::Error_t Bus::checkGenInfoAllNodes(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (Node *nodePtr : _nodePtrs)
  {
    if (( nodePtr != nullptr        ) &&
        (!nodePtr->validateGenInfo()) )
    { 
      statusReturn = Atams::ERROR_MEMORY_MAP;
    }
  }

  return (statusReturn);
}

Atams::Error_t Bus::assignNodeIDs(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if ((_nodePtrs[0U]                   == nullptr) ||
      (_nodePtrs[_noOfNodesOnBus - 1U] == nullptr) )
  {
    return (Atams::ERROR_NULL_PTR); /* Early Return */
  }

  const uint8_t firstNodeID    = _nodePtrs[0U]->getNodeID();
  const uint8_t lastNodeID     = _nodePtrs[_noOfNodesOnBus - 1U]->getNodeID();
  uint8_t       previousNodeID = firstNodeID;

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr == nullptr) break;

    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_FIRST_NODE_ID,    firstNodeID);
    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_LAST_NODE_ID,     lastNodeID);
    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, previousNodeID);

    previousNodeID = nodePtr->getNodeID();
  }

  return (error);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
