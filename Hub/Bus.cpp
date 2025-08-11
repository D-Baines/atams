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
#include "../Shared/Utilities/AtamsUtilities.hpp"
#include "../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Bus::Bus(Platform::BusPeripheral::UserData_t userData) : 
Platform::BusPeripheral(userData),
circularBuffer_(Atams::EOL_BYTE)
{
  for (Node *&nodePtr : nodePtrs_) nodePtr = nullptr; 
}

Atams::Error_t Bus::addNodeToBus(Node &node)
{
  if (noOfNodesOnBus_ >= Platform::NUMBER_OF_NODES_PER_BUS)
  {
    return (Atams::ERROR_BUS_FULL); /* Early Return */
  }

  if (findNodeOnBus(node) == true)
  {
    return (Atams::ERROR_NODE_ALREADY_ON_BUS); /* Early Return */
  }

  nodePtrs_[noOfNodesOnBus_] = &node;
  
  noOfNodesOnBus_++;

  return (Atams::ERROR_NONE);
}

void Bus::removeNodeFromBus(Node &node)
{
  for (uint16_t findIndex = 0U; findIndex < noOfNodesOnBus_; findIndex++)
  {
    if (nodePtrs_[findIndex] == &node)
    {
      for (uint16_t moveIndex = findIndex; moveIndex < noOfNodesOnBus_ - 1U; moveIndex++)
      {
        nodePtrs_[moveIndex] = nodePtrs_[moveIndex + 1U];
      }

      noOfNodesOnBus_--;
    }
  }
}

Atams::Error_t Bus::beginBusInitProcess(void)
{
  if (noOfNodesOnBus_ == 0U) 
  {
    return (Atams::ERROR_BUS_EMPTY); /* Early Return */
  }

  initProcessHandler_.resetProcess();
  initProcessHandler_.specificState = Bus::InitState::START;
  initProcessHandler_.error         = Atams::ERROR_NONE;
  initNodeIndex_                    = 0U;
  
  for (Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) nodePtr->resetRequestPacket();
  }

  return (Atams::ERROR_NONE);
}

Atams::ProcessState Bus::updateBusInitProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::InitState> &process         = initProcessHandler_;
  Atams::ProcessState                 &processState    = process.processState;
  Atams::Error_t                       cycleError      = Atams::ERROR_NONE;

  if ((runUpdateCycleAsync(cycleError) == Atams::ProcessState::IN_PROGRESS) ||
      (processState                    != Atams::ProcessState::IN_PROGRESS) )
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  Atams::ProcessState &subProcessState = process.subProcessState;
  Bus::InitState      &initState       = process.specificState;
  bool                 dataIsValid     = false; 
  Atams::Node         *initNodePtr     = nodePtrs_[initNodeIndex_];
  Atams::Node         *prevNodePtr     = (initNodeIndex_ == 0U) ? 
                                         initNodePtr            : 
                                         nodePtrs_[initNodeIndex_ - 1U];

  busIDsToSet_.firstNodeID    = nodePtrs_[0]->getNodeID();
  busIDsToSet_.lastNodeID     = nodePtrs_[noOfNodesOnBus_  - 1U]->getNodeID();
  busIDsToSet_.previousNodeID = prevNodePtr->getNodeID();

  Atams::Node &node = *initNodePtr;

  switch (initState)
  {
    case Bus::InitState::START:
      if (BusPeripheral::startReceive() == false) process.terminate(Atams::ERROR_PLATFORM);
      else                                        beginInitValidateGenInfo(node);
      break;
    case Bus::InitState::VALIDATE_GEN_INFO:
      subProcessState = nodeProcessHandler_.updateValidateGenInfo(process.error, dataIsValid);
      if      (dataIsValid     == true)                          beginInitValidateIDs(node, true, busIDsToSet_);
      else if (subProcessState == Atams::ProcessState::COMPLETE) process.terminate(Atams::ERROR_GEN_INFO_MISMATCH);
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);                                 
      break;
    case Bus::InitState::VALIDATE_IDS_PRE:
      subProcessState = nodeProcessHandler_.updateValidateBusIDs(process.error, dataIsValid);
      if      (dataIsValid     == true)                          startNextNodeInit();
      else if (subProcessState == Atams::ProcessState::COMPLETE) beginInitSetBusIDs(node, busIDsToSet_);
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::SET_BUS_IDS:
      subProcessState = nodeProcessHandler_.updateSetBusIDs(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) beginInitStore(node);
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::STORE_BUS_IDS:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) beginInitValidateIDs(node, false, busIDsToSet_);
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::VALIDATE_IDS_POST:
      subProcessState = nodeProcessHandler_.updateValidateBusIDs(process.error, dataIsValid);
      if      (dataIsValid     == true)                          startNextNodeInit();
      else if (subProcessState == Atams::ProcessState::COMPLETE) process.terminate(Atams::ERROR_GEN_INFO_MISMATCH);
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::COMPLETE:
    case Bus::InitState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  if ((!process.error                                  ) &&
      (processState == Atams::ProcessState::IN_PROGRESS) ) 
  {
    static_cast<void>(beginUpdateCyclePrivate());
  }

  error = process.error;

  return (processState);
}

Atams::Error_t Bus::beginUpdateCycle(void)
{
  if (initProcessHandler_.processState != Atams::ProcessState::COMPLETE)
  {
    return (Atams::ERROR_INIT_ORDER); /* Early Return */
  }

  return (beginUpdateCyclePrivate());
}

Atams::ProcessState Bus::runUpdateCycleSync(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::UpdateState> &process         = updateProcessHandler_;
  Atams::ProcessState                   &processState    = process.processState;
  Bus::UpdateState                      &updateState     = process.specificState;
  Node                                  *activeNodePtr   = getUpdateNodePtr();
  Bus::PollResult                       rxPollResult     = Bus::PollResult::WAITING;

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST_SYNCED) == true)
      {
        if      (tryNodeIncrementUpdate() == false) startResponseCollectionSync();
        //else if (process.error)               Platform::CommsSemaphore::release();
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      Platform::BusPeripheral::update();
      rxPollResult = pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE_SYNCED);

      if (rxPollResult != Bus::PollResult::WAITING)
      {
        if      (tryNodeIncrementUpdate() == false                   ) process.setProcessComplete();
        else if (rxPollResult             == Bus::PollResult::TIMEOUT) triggerJogSync();
      }
      break;
    case Bus::UpdateState::JOG_NODE:
      if (pollForJogTransmit(*activeNodePtr, process) == true) updateState = Bus::UpdateState::COLLECT_RESPONSES;
      break;
    case Bus::UpdateState::COMPLETE:
    case Bus::UpdateState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  if (processState == Atams::ProcessState::ERROR) error = process.error;

  return (processState);
}

Atams::ProcessState Bus::runUpdateCycleAsync(Atams::Error_t &error)
{ 
  Bus::ProcessHandler<Bus::UpdateState> &process                = updateProcessHandler_;
  Atams::ProcessState                   &processState           = process.processState;
  Bus::UpdateState                      &updateState            = process.specificState;
  Node                                  *activeNodePtr          = getUpdateNodePtr();
  NodeCallbackHandler                   *nodeCallbackHandlerPtr = activeNodePtr;
  Bus::PollResult                        rxPollResult           = Bus::PollResult::WAITING;

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST))
      {
        if (process.error) triggerNextRequestAsync();
        else               updateState = Bus::UpdateState::COLLECT_RESPONSES;
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      Platform::BusPeripheral::update();
      rxPollResult = pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE);

      if (rxPollResult != Bus::PollResult::WAITING)
      {
        nodeCallbackHandlerPtr->processResponseBuffer();
        triggerNextRequestAsync();
      }
      break;
    case Bus::UpdateState::COMPLETE:
    case Bus::UpdateState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  } 

  if (processState == Atams::ProcessState::ERROR) error = process.error;

  return (processState);
}

Atams::Error_t Bus::processBuffers(void)
{
  if (updateProcessHandler_.processState == Atams::ProcessState::IN_PROGRESS) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  Atams::Error_t firstError = Atams::ERROR_NONE;

  for (Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) 
    {
      NodeCallbackHandler &callbackHandler = *nodePtr;
      callbackHandler.processResponseBuffer();
      if (!firstError) firstError = callbackHandler.getBusError();
    }
  }

  return (firstError);
}

void Bus::beginSetNodeConfigProcess(const NodeUserConfig_t &userConfig)
{
  userConfigToSet_ = userConfig;
  configUpdateProcessHandler_.resetProcess();
  configUpdateProcessHandler_.specificState = Bus::ConfigUpdateState::START;
  dummyNode_.resetRequestPacket();
}

Atams::ProcessState Bus::updateSetNodeConfigProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           = configUpdateProcessHandler_;
  Atams::ProcessState                         &subProcessState   = process.subProcessState;
  Bus::ConfigUpdateState                      &configUpdateState = process.specificState;

  switch (configUpdateState)
  { 
    case Bus::ConfigUpdateState::START:
      if (!Platform::BusPeripheral::startReceive()) process.terminate(Atams::ERROR_PLATFORM);
      else                                          configUpdateState = Bus::ConfigUpdateState::INIT_NODE;
      break;
    case Bus::ConfigUpdateState::INIT_NODE:
      process.error = dummyNode_.init(dummyMemoryMap_);
      if (process.error) process.terminate(process.error);
      else               beginSetConfigWrite();
      break;
    case Bus::ConfigUpdateState::WRITE_CONFIG:
      subProcessState = nodeProcessHandler_.updateSetUserConfig(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) beginSetConfigStore();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      else                                                       startNextConfigCycle(configUpdateState);
      break;
    case Bus::ConfigUpdateState::STORE_CONFIG:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      else                                                       startNextConfigCycle(configUpdateState);
      break;
    case Bus::ConfigUpdateState::SEND_REQUEST:
      updateSetConfigSendRequest();
      break;
    case Bus::ConfigUpdateState::GET_RESPONSE:
      updateSetConfigGetResponse();
      break;
    case Bus::ConfigUpdateState::COMPLETE:
    case Bus::ConfigUpdateState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }
    
  error = process.error;

  return (process.processState);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

bool Bus::findNodeOnBus(Node &node)
{
  bool nodeFound = false;

  for (Node *&nodePtr : nodePtrs_)
  {    
    if (nodePtr == &node) nodeFound = true;
  }

  return (nodeFound);
}

Atams::Error_t Bus::beginUpdateCyclePrivate(void)
{
  if (updateProcessHandler_.processState == Atams::ProcessState::IN_PROGRESS)
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  clearAllBusErrors();
  circularBuffer_.reset();
  updateNodeIndex_ = 0U;
  activeSyncCount_++;
  updateProcessHandler_.resetProcess();
  updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;

  return (Atams::ERROR_NONE);
}

void Bus::clearAllBusErrors(void)
{
  for (Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) 
    {
      static_cast<NodeCallbackHandler*>(nodePtr)->clearBusError();
      static_cast<NodeCallbackHandler*>(nodePtr)->clearAbortDetails();
    }
  }
}

bool Bus::pollForRequestTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t requestType)
{
  uint32_t currentTime          = Platform::getMillis();
  bool     messageSendAttempted = false;
  
  if (Platform::BusPeripheral::transmitReady())
  {
    if (node.getEncodedRequestPacket(requestType,
                                     activeSyncCount_,
                                     encodedBuffer_, 
                                     sizeof(encodedBuffer_), 
                                     encodedLength_) == Atams::ERROR_NONE)
    {
      if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
      {
        node.reportBusError(Atams::ERROR_PLATFORM);
        process.error = Atams::ERROR_PLATFORM;
      }
      else 
      {
        lastSentMessageType_ = requestType;
      }
    }

    messageSendAttempted  = true;
    process.prevEventTime = currentTime;
  }

  return (messageSendAttempted);
}

bool Bus::pollForJogTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process)
{
  uint32_t currentTime          = Platform::getMillis();
  bool     messageSendAttempted = false;

  jogBuffer_[MESH_INDEX_NODE_ID]  = node.getNodeID();
  jogBuffer_[MESH_INDEX_MSG_TYPE] = Atams::MESSAGE_SYNC_JOG;
  jogBuffer_[MESH_INDEX_SYNC]     = activeSyncCount_;

  if (Platform::BusPeripheral::transmitReady())
  {
    if (encodeMeshPacket(jogBuffer_, 
                         MESH_SIZE_HEADER, 
                         encodedBuffer_, 
                         sizeof(encodedBuffer_), 
                         encodedLength_) == Atams::ERROR_NONE)
    {
      if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
      {
        node.reportBusError(Atams::ERROR_PLATFORM);
      }
      else 
      {
        lastSentMessageType_ = Atams::MESSAGE_SYNC_JOG;
      }
    }
    messageSendAttempted  = true;
    process.prevEventTime = currentTime;
  }

  return (messageSendAttempted);
}

Bus::PollResult Bus::pollForResponse(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t expectedResponse)
{
  uint32_t        currentTime  = Platform::getMillis();
  Bus::PollResult result       = Bus::PollResult::WAITING;

  if ((circularBuffer_.getPacket(rxBuffer_, sizeof(rxBuffer_), rxLength_) == CircularBuffer::ERROR_NONE) &&
      (validateAndStoreResponsePacket(node, expectedResponse)             == true                      ) )
  {
    result = Bus::PollResult::READY;
    process.prevEventTime = currentTime;
  }

  else if (currentTime - process.prevEventTime > Platform::BUS_RESPONSE_TIMEOUT)
  { 
    node.reportBusError(Atams::ERROR_RESPONSE_TIMEOUT);
    result = Bus::PollResult::TIMEOUT;
    process.prevEventTime = currentTime;
  }

  return (result);
}

void Bus::rxCallback(uint8_t *rxBufferPtr, const uint16_t rxBufferLength)
{
  static_cast<void>(circularBuffer_.pushHead(rxBufferPtr, rxBufferLength));
}

bool Bus::validateAndStoreResponsePacket(Atams::NodeCallbackHandler &node, const MessageType_t expectedResponse)
{
  Atams::Error_t error       = Atams::ERROR_NONE;
  bool           packetValid = true;

  error = decodeMeshPacket(rxBuffer_, 
                           rxLength_, 
                           decodedBuffer_, 
                           sizeof(decodedBuffer_), 
                           decodedLength_);

  if (error == Atams::ERROR_NONE)
  {
    uint8_t              packetNodeID    = decodedBuffer_[MESH_INDEX_NODE_ID];
    uint8_t              packetSyncCount = decodedBuffer_[MESH_INDEX_SYNC];
    Atams::MessageType_t messageType     = static_cast<MessageType_t>(decodedBuffer_[MESH_INDEX_MSG_TYPE]);

     bool messageIsAbort = ((messageType == Atams::MESSAGE_ABORT_RESPONSE       ) ||
                            (messageType == Atams::MESSAGE_ABORT_RESPONSE_SYNCED) );

    if      (messageType     == lastSentMessageType_)          packetValid = false;
    else if (packetSyncCount != activeSyncCount_)              error = Atams::ERROR_SYNC_COUNT;
    else if ((messageType    != MESSAGE_BROADCAST_RESPONSE) &&
             (packetNodeID   != node.getNodeID()          ) )  error = Atams::ERROR_SYNC_NODE;
    else if ((messageType    != expectedResponse          ) &&
             (messageIsAbort == false                     ) )  error = Atams::ERROR_MESSAGE_TYPE;
    else                                                       node.responseReceived(decodedBuffer_, decodedLength_);

    if (error != Atams::ERROR_NONE) node.reportBusError(error);
  }
  else 
  {
    packetValid = false;
  }

  return (packetValid);
}

Atams::Node * Bus::getUpdateNodePtr(void)
{
  Node *nodePtr = nullptr;

  if ((updateNodeIndex_            < Platform::NUMBER_OF_NODES_PER_BUS) &&
      (updateNodeIndex_            < noOfNodesOnBus_                  ) &&
      (nodePtrs_[updateNodeIndex_] != nullptr                         ) ) 
  {
    nodePtr = nodePtrs_[updateNodeIndex_];
  }
  
  return (nodePtr);
}

bool Bus::tryNodeIncrementUpdate(void)
{
  bool incrementValid = false;

  if (updateNodeIndex_ + 1U < noOfNodesOnBus_)
  {
    updateNodeIndex_++;
    incrementValid = true;
  }

  return (incrementValid);
}

void Bus::startResponseCollectionSync(void)
{
  updateNodeIndex_ = 0U;
  updateProcessHandler_.specificState = Bus::UpdateState::COLLECT_RESPONSES;
}

void Bus::triggerJogSync(void)
{
  //Platform::CommsSemaphore::release();
  updateProcessHandler_.specificState = Bus::UpdateState::JOG_NODE;
}

void Bus::triggerNextRequestAsync(void)
{
  if (tryNodeIncrementUpdate() == false) 
  {
    updateProcessHandler_.setProcessComplete();
  }
  else
  { 
    //Platform::CommsSemaphore::release();
    circularBuffer_.reset();
    updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;
  }
}

void Bus::beginInitValidateGenInfo(Atams::Node &node)
{
  initProcessHandler_.specificState = Bus::InitState::VALIDATE_GEN_INFO;
  nodeProcessHandler_.beginValidateGenInfoProcess(node);
}

void Bus::beginInitValidateIDs(Atams::Node &node, const bool preAssignment, const Atams::BusIDs_t busIDs)
{
  if (preAssignment) initProcessHandler_.specificState = Bus::InitState::VALIDATE_IDS_PRE;
  else               initProcessHandler_.specificState = Bus::InitState::VALIDATE_IDS_POST;
  nodeProcessHandler_.beginValidateBusIDsProcess(node, busIDs);
}

void Bus::beginInitSetBusIDs(Atams::Node &node, Atams::BusIDs_t busIDs)
{
  initProcessHandler_.specificState = Bus::InitState::SET_BUS_IDS;
  nodeProcessHandler_.beginSetBusIDs(node, busIDs);
}

void Bus::beginInitStore(Atams::Node &node)
{
  initProcessHandler_.specificState = Bus::InitState::STORE_BUS_IDS;
  nodeProcessHandler_.beginStorageProcess(node);
}

void Bus::startNextNodeInit(void)
{
  Bus::ProcessHandler<Bus::InitState> &process = initProcessHandler_;

  if ((initNodeIndex_ + 1U) < noOfNodesOnBus_)
  {
    initNodeIndex_++;

    Atams::Node *initNodePtr = nodePtrs_[initNodeIndex_];

    if (initNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);
    else                        beginInitValidateGenInfo(*initNodePtr);
  }
  else
  {
    initProcessHandler_.setProcessComplete();
  }
}

void Bus::beginSetConfigWrite(void)
{
  configUpdateProcessHandler_.specificState = Bus::ConfigUpdateState::WRITE_CONFIG;
  nodeProcessHandler_.beginSetUserConfig(dummyNode_, userConfigToSet_);
}

void Bus::beginSetConfigStore(void)
{
  configUpdateProcessHandler_.specificState = Bus::ConfigUpdateState::STORE_CONFIG;
  nodeProcessHandler_.beginStorageProcess(dummyNode_);
}

void Bus::updateSetConfigSendRequest(void)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           = configUpdateProcessHandler_;
  Bus::ConfigUpdateState                      &configUpdateState = process.specificState;

  if (pollForRequestTransmit(dummyNode_, process, MESSAGE_BROADCAST_UNIVERSAL) == true)
  {
    if (process.error) 
    {
      process.terminate(process.error);
    }
    else               
    {
      static_cast<NodeCallbackHandler&>(dummyNode_).clearBusError();
      configUpdateState = Bus::ConfigUpdateState::GET_RESPONSE;     
    }          
  }
}

void Bus::updateSetConfigGetResponse(void)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           = configUpdateProcessHandler_;
  Bus::ConfigUpdateState                      &configUpdateState = process.specificState;

  Platform::BusPeripheral::update();

  if (pollForResponse(dummyNode_, process, MESSAGE_BROADCAST_RESPONSE) != Bus::PollResult::WAITING)
  {
    dummyNodeCallbackHandler_.processResponseBuffer();
    configUpdateState = process.nextSpecificState;  
  }
}

void Bus::startNextConfigCycle(const Bus::ConfigUpdateState nextState)
{
  configUpdateProcessHandler_.nextSpecificState = nextState;
  configUpdateProcessHandler_.specificState     = Bus::ConfigUpdateState::SEND_REQUEST;
}

/*************************************************************************************/
/* PRIVATE STATIC CONSTANTS                                                          */
/*************************************************************************************/

const Atams::GenInfo_t Bus::blankGenInfo_;

const Node::MemoryMap_t Bus::dummyMemoryMap_ = 
{
  /* .sharedMemoryMap = */
  {
    /* .noOfVars    = */ BlockUniversal::NUMBER_OF_VARS,
    /* .genInfo     = */ Bus::blankGenInfo_,
    /* .varInfoList = */ BlockUniversal::varInfoList
  }
};

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
void Bus::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  this->error         = exitError;
  this->specificState = T::ERROR;
  this->processState  = Atams::ProcessState::ERROR;
}

template <typename T>
void Bus::ProcessHandler<T>::setProcessComplete(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState  = Atams::ProcessState::COMPLETE;
}

template <typename T>
bool Bus::ProcessHandler<T>::getProcessTerminated(void)
{
  return (this->processState == Atams::ProcessState::ERROR);
}

template <typename T>
void Bus::ProcessHandler<T>::resetProcess(void)
{
  this->error           = Atams::ERROR_NONE;
  this->processState    = Atams::ProcessState::IN_PROGRESS;
  this->subProcessState = Atams::ProcessState::IN_PROGRESS;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
