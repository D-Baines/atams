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

Bus::Bus(BusPeripheral::UserData_t userData) :
CircularBuffer(CircularBuffer::DEFAULT_EOL_CHAR),
BusPeripheral(userData)
{
  for (Node *nodePtr : _nodePtrs) nodePtr = nullptr;
}


Bus::~Bus(void)
{

}

Atams::Error_t Bus::addNodeToBus(Node &node)
{

}

Atams::Error_t Bus::removeNodeFromBus(Node &node)
{

}

Bus::InitState_t Bus::updateInitProcedure(void)
{
  
}

Atams::Error_t Bus::startUpdateCycle(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if ((_updateState == UPDATE_STATE_READY         ) ||
      (_updateState == UPDATE_STATE_CYCLE_COMPLETE) )
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

  switch (_updateState)
  {
    case UPDATE_STATE_READY:
      /* Do Nothing */
      break;
    case UPDATE_STATE_SEND_REQUESTS:
      if (activeNodePtr == nullptr)
      {
        _updateState = UPDATE_STATE_REQUESTS_COMPLETE;
      }
      else if (Platform::BusPeripheral::transmitReady())
      {
        //TODO:: _request pack must be locked whiled encoding!!!
        //       call function to get encoded request packet from node,
        //       node can handle appropriate locks. This can be a public
        //       function. No bus friend class required in node.
        if (encodeMeshPacket(activeNodePtr->_requestPacket.buffer, 
                             activeNodePtr->_requestPacket.length, 
                             _encodedBuffer, 
                             sizeof(_encodedBuffer), 
                             _encodedLength) == ERROR_NONE)
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
    case UPDATE_STATE_REQUESTS_COMPLETE:
      if (_responseProcessingComplete)
      {
        _activeNodeIndex      = 0U;
        _previousResponseTime = currentTime;
      } 
      break;
    case UPDATE_STATE_COLLECT_RESPONSES:
      if (activeNodePtr == nullptr)
      {
        _activeNodeIndex = 0U;
        _updateState     = UPDATE_STATE_CYCLE_COMPLETE;
      }
      if (CircularBuffer::getPacket(_rxBuffer, 
                                    sizeof(_rxBuffer),
                                    _rxLength))
      {
        if (decodeMeshPacket(_rxBuffer, 
                             _rxLength, 
                             _decodedBuffer, 
                             sizeof(_decodedBuffer), 
                             _decodedLength        ) == ERROR_NONE)
        {
          uint8_t packetNodeID    = _decodedBuffer[MESH_INDEX_NODE_ID];
          uint8_t packetSyncCount = _decodedBuffer[MESH_INDEX_SYNC];
          if (packetSyncCount != _activeSyncCount) 
          {
            statusReturn = Atams::ERROR_SYNC_COUNT;
          }
          else
          {
            for (Node *nodePtr : _nodePtrs)
            {
              if (nodePtr->getNodeID() == packetNodeID) nodePtr->responseReceived(_decodedBuffer, _decodedLength);
            }
          }
        }

        _activeNodeIndex++;   
      }
      else if (currentTime - _previousResponseTime > RESPONSE_TIMEOUT)
      { 
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
        _jogBuffer[MESH_INDEX_NODE_ID] = _nodePtrs[_activeNodeIndex]->_nodeID;

        if (Platform::BusPeripheral::transmitReady()) 
        {
          encodeMeshPacket(_jogBuffer, MESH_SIZE_HEADER, _encodedBuffer, sizeof(_encodedBuffer), _encodedLength);
          Platform::BusPeripheral::transmit(_encodedBuffer, _encodedLength);
        }
      }
      break;
    case UPDATE_STATE_CYCLE_COMPLETE:
      /* Do Nothing */
      break;
  }
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

  for (Node *nodePtr : _nodePtrs)
  {
    if (nodePtr != nullptr) nodePtr->processResponseBuffer();
  }

  _responseProcessingComplete = true;

  return (Atams::ERROR_NONE);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/



} /* End Namespace - Atams */


/**
  * @}End of File
  */
