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

Bus::InitState_t Bus::updateInitProcedure(void)
{
  BusPeripheral::startPeripheral();
  return (INIT_STATE_SUCCESSFUL);
}

Atams::Error_t Bus::startUpdateCycle(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (_updateState == UPDATE_STATE_READY)
  {
    _activeNodeIndex = 0U;
    _updateState     = UPDATE_STATE_SEND_REQUESTS;
  }
  else
  {
    statusReturn = Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS;
  }

  return (statusReturn);
}

Atams::Error_t Bus::update(void)
{
  Atams::Error_t statusReturn  = Atams::ERROR_NONE;
  Node          *activeNodePtr = _nodePtrs[_activeNodeIndex];
  uint64_t       currentTime   = Platform::getMillis();

  BusPeripheral::update();

  switch (_updateState)
  {
    case UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case UPDATE_STATE_SEND_REQUESTS:
      if (activeNodePtr == nullptr)
      {
        _activeNodeIndex      = 0U;
        _previousResponseTime = currentTime;
        _updateState          = UPDATE_STATE_COLLECT_RESPONSES;
      }
      else if (Platform::BusPeripheral::transmitReady())
      {
        if (activeNodePtr->getEncodedRequestPacket(_encodedBuffer, 
                                                   sizeof(_encodedBuffer), 
                                                   _encodedLength        ) == Atams::ERROR_NONE)
        {
          Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
        }
        else
        {
          statusReturn = Atams::ERROR_ENCODE;
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
        if (decodeMeshPacket(_rxBuffer, 
                             _rxLength, 
                             _decodedBuffer, 
                             sizeof(_decodedBuffer), 
                             _decodedLength        ) == Atams::ERROR_NONE)
        {
          uint8_t packetNodeID    = _decodedBuffer[MESH_INDEX_NODE_ID];
          uint8_t packetSyncCount = _decodedBuffer[MESH_INDEX_SYNC];
          uint8_t messageType     = _decodedBuffer[MESH_INDEX_MSG_TYPE];
          if (packetSyncCount != _activeSyncCount) 
          {
            statusReturn = Atams::ERROR_SYNC_COUNT;
          }
          else if (messageType != MESSAGE_RESPONSE_SYNCED ||
                   messageType != MESSAGE_ABORTED_RESPONSE)
          {
            statusReturn = Atams::ERROR_MESSAGE_TYPE;
          }
          else
          {
            for (Node *nodePtr : _nodePtrs)
            {
              if ((nodePtr              != nullptr     ) &&
                  (nodePtr->getNodeID() == packetNodeID) ) 
              {
                if (nodePtr->responseReceived(_decodedBuffer, _decodedLength) != Atams::ERROR_NONE)
                {
                  statusReturn = Atams::ERROR_RESPONSE_BUFFER_LENGTH;
                }
              }
            }
          }
        }
        _activeNodeIndex++;
      }
      else if (currentTime - _previousResponseTime > Platform::BUS_RESPONSE_TIMEOUT)
      { 
        activeNodePtr->flagNoResponse();
        _activeNodeIndex++;
        _updateState = UPDATE_STATE_JOG_NODE;
      }
      break;
    case UPDATE_STATE_JOG_NODE:
      if (activeNodePtr == nullptr)
      {
        _updateState = UPDATE_STATE_CYCLE_COMPLETE;
      }
      else
      {
        _jogBuffer[MESH_INDEX_NODE_ID] = _nodePtrs[_activeNodeIndex]->getNodeID();

        if (Platform::BusPeripheral::transmitReady()) 
        {
          encodeMeshPacket(_jogBuffer, MESH_SIZE_HEADER, _encodedBuffer, sizeof(_encodedBuffer), _encodedLength);
          Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
          _updateState = UPDATE_STATE_COLLECT_RESPONSES;
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

  return (statusReturn);
}

bool Bus::updateCycleComplete(void)
{
  /* Wait on condition variable for update cycle complete */

  return (_updateState == UPDATE_STATE_CYCLE_COMPLETE);
}

Atams::Error_t Bus::processBuffers(void)
{
  if (_updateState != UPDATE_STATE_CYCLE_COMPLETE) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS);
  }

  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  for (Node *nodePtr : _nodePtrs)
  {
    if ((nodePtr                          != nullptr          ) &&
        (nodePtr->processResponseBuffer() != Atams::ERROR_NONE) )
    {
      statusReturn = Atams::ERROR_RESPONSE_BUFFER_LENGTH; //TODO:: New error required
    }
  }

  _updateState = UPDATE_STATE_READY;

  return (statusReturn);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::Error_t Bus::addNodeToBus(Node &node)
{
  if (_noOfNodesOnBus >= Platform::NUMBER_OF_NODES_PER_BUS)
  {
    return (Atams::ERROR_BUS_FULL);
  }

  _nodePtrs[_noOfNodesOnBus] = &node;

  _noOfNodesOnBus++;

  return (Atams::ERROR_NONE);
}

void Bus::removeNodeFromBus(Node &node)
{
  uint8_t ptrIndex  = 0U;
  bool    nodeFound = false;

  for (ptrIndex = 0U; ptrIndex < Platform::NUMBER_OF_NODES_PER_BUS; ptrIndex++)
  {
    if (_nodePtrs[ptrIndex] == &node)
    {
      nodeFound = true;
      break;
    }
  }

  if (nodeFound)
  {
    for (; ptrIndex < (Platform::NUMBER_OF_NODES_PER_BUS - 1U); ptrIndex++)
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
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
