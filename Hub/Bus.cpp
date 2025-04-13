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
CircularBuffer(CircularBuffer::DEFAULT_EOL_CHAR),
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

Bus::ProcessState_t Bus::updateInitProcess(Atams::Error_t &error)
{
  if (_initProcessState == Bus::PROCESS_STATE_COMPLETE) return (_initProcessState);

  switch (_busInitState)
  {
    case Bus::INIT_STATE_START:
      _initProcessState = Bus::PROCESS_STATE_IN_PROGRESS;
      if (!BusPeripheral::startPeripheral()) error = Atams::ERROR_PLATFORM;
      else                                   error = triggerGenInfoCollectionAllNodes();
      if (error == Atams::ERROR_NONE)
      {
        _nextInitState = Bus::INIT_STATE_VALIDITY_CHECKS;
        _busInitState  = Bus::INIT_STATE_START_UPDATE_CYCLE;
      }
      break;
    case Bus::INIT_STATE_START_UPDATE_CYCLE:
      if (startUpdateCycle() == Atams::ERROR_NONE) _busInitState = INIT_STATE_BUS_UPDATE;
      else                                         _busInitState = INIT_STATE_FAILURE;
      break;
    case Bus::INIT_STATE_BUS_UPDATE:
      if (updateNoSync() == Bus::UPDATE_STATE_CYCLE_COMPLETE)
      {
        if (processBuffers() == Atams::ERROR_NONE) _busInitState = _nextInitState;
        else                                       _busInitState = Bus::INIT_STATE_FAILURE;
      }
      break;
    case Bus::INIT_STATE_VALIDITY_CHECKS:
      error = checkGenInfoAllNodes();
      if (error == Atams::ERROR_NONE)
      {
        _nextInitState = Bus::INIT_STATE_ID_ASSIGNMENT;
        _busInitState  = Bus::INIT_STATE_START_UPDATE_CYCLE;
      }
      break;
    case Bus::INIT_STATE_ID_ASSIGNMENT:
      error = assignNodeIDs();
      if (error == Atams::ERROR_NONE)
      {
        _nextInitState = Bus::INIT_STATE_SUCCESS;
        _busInitState  = Bus::INIT_STATE_START_UPDATE_CYCLE;
      }
      break;
    case Bus::INIT_STATE_FAILURE:
      _busInitState = Bus::INIT_STATE_START;
      break;
    case Bus::INIT_STATE_SUCCESS:
      _initProcessState = Bus::PROCESS_STATE_COMPLETE;
    default:
      /* Do Nothing */
      break;
  }

  if (error != Atams::ERROR_NONE)
  {
    _busInitState     = Bus::INIT_STATE_FAILURE;
    _initProcessState = Bus::PROCESS_STATE_ERROR;
  }

  return (_initProcessState);
}

Atams::Error_t Bus::startUpdateCycle(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (_initProcessState != Bus::PROCESS_STATE_COMPLETE)
  {
    statusReturn = Atams::ERROR_INIT_REQUIRED;
  }
  else if (_updateState == UPDATE_STATE_READY)
  {
    for (Node * nodePtr : _nodePtrs)
    {
      if (nodePtr != nullptr) nodePtr->clearBusError();
    }

    _activeNodeIndex = 0U;
    _updateState     = UPDATE_STATE_SEND_REQUESTS;
  }
  else
  {
    statusReturn = Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS;
  }

  return (statusReturn);
}

void Bus::update(void)
{
  uint64_t currentTime   = Platform::getMillis();
  Node    *activeNodePtr = nullptr;

  if ((_activeNodeIndex < sizeof(_nodePtrs)) &&
      (_activeNodeIndex < _noOfNodesOnBus  ) )
  {
    activeNodePtr = _nodePtrs[_activeNodeIndex];
  }

  BusPeripheral::update();
  Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  switch (_updateState)
  {
    case UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case UPDATE_STATE_SEND_REQUESTS:
      if (activeNodePtr == nullptr)
      {
        _activeNodeIndex  = 0U;
        _prevResponseTime = currentTime;
        _updateState      = UPDATE_STATE_COLLECT_RESPONSES;
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
        _activeNodeIndex++;
      }
      break;
    case UPDATE_STATE_COLLECT_RESPONSES:
      if (activeNodePtr == nullptr)
      {
        _activeNodeIndex = 0U;
        _updateState     = UPDATE_STATE_CYCLE_COMPLETE;
      }
      else if (CircularBuffer::getPacket(_rxBuffer, 
                                         sizeof(_rxBuffer),
                                         _rxLength        ) == CircularBuffer::ERROR_NONE)
      {
        if (validateAndStoreResponsePacket(*activeNodePtr, Atams::MESSAGE_RESPONSE_SYNCED) != Atams::ERROR_DECODE)
        {
          _activeNodeIndex++;
        }
      }
      else if (currentTime - _prevResponseTime > Platform::BUS_RESPONSE_TIMEOUT)
      { 
        _activeNodeIndex++;
        _updateState = UPDATE_STATE_JOG_NODE;
      }
      break;
    case UPDATE_STATE_JOG_NODE:
      if (activeNodePtr == nullptr)
      {
        _activeNodeIndex = 0U;
        _updateState     = UPDATE_STATE_CYCLE_COMPLETE;
      }
      else
      {
        _jogBuffer[MESH_INDEX_NODE_ID] = _nodePtrs[_activeNodeIndex]->getNodeID();

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
          _updateState      = UPDATE_STATE_COLLECT_RESPONSES;
        }
      }
      break;
    case UPDATE_STATE_CYCLE_COMPLETE:
      /* Do Nothing */
      break;
    default:
      /* TODO:: Handle error correctly */
      break;
  }
}

bool Bus::updateCycleComplete(void)
{
  /* Wait on condition variable for update cycle complete */

  return (_updateState == UPDATE_STATE_CYCLE_COMPLETE);
}

bool Bus::processBuffers(void)
{
  bool errorFound = false;

  if (_updateState != UPDATE_STATE_CYCLE_COMPLETE) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS);
  }

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr != nullptr) 
    {
      nodePtr->processResponseBuffer();
      if (nodePtr->getBusError() != Atams::ERROR_NONE) errorFound = true;
    }
  }

  _updateState = UPDATE_STATE_READY;

  return (errorFound);
}


Bus::ProcessState_t Bus::updateSetNodeIDProcess(const uint8_t nodeIDToSet, Atams::Error_t &error)
{
  static Bus::UpdateState_t updateState = Bus::UPDATE_STATE_READY;

  static uint8_t nodeIDSetPacket[MESH_SIZE_HEADER + DATAGRAM_SIZE_HEADER + ];

  switch (updateState)
  {
    case UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case UPDATE_STATE_SEND_REQUESTS:

      break;
    case UPDATE_STATE_COLLECT_RESPONSES:

      break;
    case UPDAT
  }

  if (Platform::BusPeripheral::transmitReady())
  {
    outputBuffer.buffer[MESH_INDEX_MSG_TYPE] = MESSAGE_BROADCAST_UNIVERSAL;
    _requestPacket.buffer[MESH_INDEX_NODE_ID ] = 0U;
    
    Atams::Error_t statusReturn = encodeMeshPacket(nodeIDSetPacket, 
                                                   sizeof(nodeIDSetPacket), 
                                                   _encodedBuffer, 
                                                   sizeof(_encodedBuffer), 
                                                   _encodedLength);

    if ((statusReturn == Atams::ERROR_NONE       ) &&
        (Platform::BusPeripheral::transmitReady()) )
    {
      Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
      _updateState     = UPDATE_STATE_COLLECT_RESPONSES;
    }
  }
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

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

void Bus::rxCallback(      uint8_t  *rxBufferPtr,
                     const uint16_t  rxBufferLength)
{
  CircularBuffer::pushHead(rxBufferPtr, rxBufferLength);
  Platform::CommsSemaphore::signal(); 
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
  
  if ((_activeNodeIndex < sizeof(_nodePtrs)) &&
      (_activeNodeIndex < _noOfNodesOnBus  ) )
  {
    activeNodePtr = _nodePtrs[_activeNodeIndex];
  }
  else if (_updateState != Bus::UPDATE_STATE_READY)
  {
    _activeNodeIndex = 0U;
    _updateState     = UPDATE_STATE_CYCLE_COMPLETE;
  }

  BusPeripheral::update();
  Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

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
        else
        {
          _activeNodeIndex++;
        }
      }
      break;
    case Bus::UPDATE_STATE_COLLECT_RESPONSES:
      if (CircularBuffer::getPacket(_rxBuffer, 
                                    sizeof(_rxBuffer),
                                    _rxLength        ) == CircularBuffer::ERROR_NONE)
      {
        if (validateAndStoreResponsePacket(*activeNodePtr, Atams::MESSAGE_RESPONSE) != Atams::ERROR_DECODE)
        {
          _activeNodeIndex++;
          _updateState = UPDATE_STATE_SEND_REQUESTS;
        }
      }
      else if (currentTime - _prevRequestTime > Platform::BUS_RESPONSE_TIMEOUT)
      { 
        _activeNodeIndex++;
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
}

Atams::Error_t Bus::triggerGenInfoCollectionAllNodes(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr != nullptr)
    {
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; varID <= BlockUniversal::VAR_ID_MAP_CHECKSUM; varID++)
      {
        statusReturn = nodePtr->setRequestPattern(BLOCK_ID_UNIVERSAL, 
                                                  varID, 
                                                  Atams::ACCESS_READ, 
                                                  Atams::REQUEST_UNTIL_ACK);
  
        if (statusReturn != Atams::ERROR_NONE) 
        {
          return (statusReturn); /* Early Return */
        }
      }
    }
  }

  return (statusReturn);
}

Atams::Error_t Bus::checkGenInfoAllNodes(void)
{
  Atams::Error_t statusReturn =  Atams::ERROR_NONE;

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
  Atams::Error_t errorStatus = Atams::ERROR_NONE;

  if ((_nodePtrs[0U]              == nullptr) ||
      (_nodePtrs[_noOfNodesOnBus] == nullptr) )
  {
    return (Atams::ERROR_NULL_PTR); /* Early Return */
  }

  const uint8_t firstNodeID    = _nodePtrs[0U]->getNodeID();
  const uint8_t lastNodeID     = _nodePtrs[_noOfNodesOnBus]->getNodeID();
  uint8_t       previousNodeID = firstNodeID;

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr != nullptr)
    {
      if (!errorStatus) errorStatus = nodePtr->write(Atams::BLOCK_ID_UNIVERSAL, BlockUniversal::VAR_ID_FIRST_NODE_ID,    firstNodeID);
      if (!errorStatus) errorStatus = nodePtr->write(Atams::BLOCK_ID_UNIVERSAL, BlockUniversal::VAR_ID_LAST_NODE_ID,     lastNodeID);
      if (!errorStatus) errorStatus = nodePtr->write(Atams::BLOCK_ID_UNIVERSAL, BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, previousNodeID);
      previousNodeID = nodePtr->getNodeID();
    }
  }

  return (errorStatus);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
