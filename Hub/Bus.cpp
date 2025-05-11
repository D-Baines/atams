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
#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Bus::Bus(Platform::BusPeripheral::UserData_t userData)
: Platform::BusPeripheral(userData),
  circularBuffer_(Atams::EOL_BYTE),
  activeNodeIndex_(0U),
  noOfNodesOnBus_(0U),
  rxLength_(0U),
  decodedLength_(0U),
  encodedLength_(0U),
  activeSyncCount_(0U)
{
  for (Atams::Node *&nodePtr : nodePtrs_) nodePtr = nullptr; 
}

Atams::Error_t Bus::addNodeToBus(Node &node)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (noOfNodesOnBus_ < Platform::NUMBER_OF_NODES_PER_BUS) nodePtrs_[noOfNodesOnBus_++] = &node;
  else                                                     statusReturn = Atams::ERROR_BUS_FULL;

  return (statusReturn);
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
      break;
    }
  }
}

Atams::Error_t Bus::startPeripheral(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (BusPeripheral::startPeripheral() == false) statusReturn = Atams::ERROR_PLATFORM;

  return (statusReturn);
}

Atams::ProcessState Bus::updateBusInitProcess(Atams::Error_t &error)
{
  (void)error;
  //Bus::ProcessHandler<Bus::InitState> &process         = initProcessHandler_;
  //Atams::ProcessState                 &processState    = process.processState;
  //Atams::ProcessState                 &subProcessState = process.subProcessState;
  //Bus::InitState                      &initState       = process.specificState;
  //bool                                 validFlag       = false;
//
  //switch (initState)
  //{
  //  case Bus::InitState::START:
  //    if (BusPeripheral::startPeripheral() == false) process.terminate(Atams::ERROR_PLATFORM);
  //    else                                           initState = Bus::InitState::VALIDATE_GEN_INFO;
  //    break;
  //  case Bus::InitState::VALIDATE_GEN_INFO:
  //    subProcessState = updateValidateGenInfoAllNodes(process.error);
  //    if      (subProcessState == Atams::ProcessState::COMPLETE) initState = Bus::InitState::VALIDATE_IDS;
  //    else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
  //    break;
  //  case Bus::InitState::VALIDATE_IDS:
  //    subProcessState = updateValidateIDsAllNodes(process.error);
  //    if      (subProcessState == Atams::ProcessState::COMPLETE) initState = Bus::InitState::VALIDATE_IDS;
  //    else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
  //    break;
  //  case Bus::InitState::ENTER_CONFIG:
  //    subProcessState = updateConfigurationEntryAllNodes(process.error);
  //    if      (subProcessState == Atams::ProcessState::COMPLETE) initState = Bus::InitState::ASSIGN_NODE_IDS;
  //    else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
  //    break;
  //  case Bus::InitState::ASSIGN_NODE_IDS:
  //    static_cast<void>(assignNodeIDs());
  //    break;
  //  case Bus::InitState::SAVE_AND_EXIT:
  //    subProcessState = updateStoreAllNodes(process.error);
  //    if      (subProcessState == Atams::ProcessState::COMPLETE) initState = Bus::InitState::COLLECT_NODE_IDS_POST;
  //    else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
  //    break;
  //  case Bus::InitState::VALIDATE_IDS_POST:
  //    subProcessState = updateValidateIDsAllNodes(process.error);
  //    if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
  //    else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
  //    break;
  //  case Bus::InitState::START_UPDATE_CYCLE:
  //    error     = startUpdateCycle();
  //    initState = Bus::InitState::UPDATE_CYCLE;
  //    break;
  //  case Bus::InitState::UPDATE_CYCLE:
  //    if (updateNoSync() == Atams::ProcessState::COMPLETE)
  //    {
  //      error = processBuffers();
  //      _initStateMinor = _nextInitStateMinor;
  //    }
  //    break;
  //  case Bus::InitState::COMPLETE:
  //  case Bus::InitState::ERROR:
  //    /* Do Nothing - Transitions handled by ProcessHandler */
  //    break;
  //  default:
  //    process.terminate(Atams::ERROR_INVALID_CASE);
  //    break;
  //}
//
  //error = process.error;
//
  //return (processState);

  return (Atams::ProcessState::ERROR);
}

Atams::Error_t Bus::startUpdateCycle(void)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (initProcessHandler_.processState != Atams::ProcessState::COMPLETE)
  {
    statusReturn = Atams::ERROR_INIT_REQUIRED;
  }
  else if (updateProcessHandler_.processState == Atams::ProcessState::IN_PROGRESS)
  {
    statusReturn = Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS;
  }
  else
  {
    for (Node *&nodePtr : nodePtrs_)
    {
      if (nodePtr != nullptr) nodePtr->clearBusError();
    }

    activeNodeIndex_ = 0U;
    updateProcessHandler_.resetProcess();
  }

  return (statusReturn);
}

Atams::ProcessState Bus::runUpdateCycleSync(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::UpdateState> &process         = updateProcessHandler_;
  Atams::ProcessState                   &processState    = process.processState;
  Bus::UpdateState                      &updateState     = process.specificState;
  Node                                  *activeNodePtr   = getActiveNodePtr();

  BusPeripheral::update();
  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  switch (updateState)
  {
    case Bus::UpdateState::START: /* Fall-through */
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST_SYNCED) == true)
      {
        if (tryNodeIncrement() == false) startResponseCollectionSync();
        //else if (process.error)          Platform::CommsSemaphore::release();
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      if (pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE_SYNCED) == true)
      {
        if (tryNodeIncrement() == false) process.setProcessComplete();
        else                             triggerJogSync();
      }
      break;
    case Bus::UpdateState::JOG_NODE:
      if (pollForJogTransmit(*activeNodePtr, process) == true) updateState = Bus::UpdateState::COLLECT_RESPONSES;
      break;
    case Bus::UpdateState::COMPLETE: /* Fall-through */
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
  Bus::ProcessHandler<Bus::UpdateState> &process         = updateProcessHandler_;
  Atams::ProcessState                   &processState    = process.processState;
  Bus::UpdateState                      &updateState     = process.specificState;
  Node                                  *activeNodePtr   = getActiveNodePtr();

  BusPeripheral::update();
  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  switch (updateState)
  {
    case Bus::UpdateState::START: /* Fall-through */
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST) == true)
      {
        if (process.error) triggerNextRequestAsync();
        else               updateState = Bus::UpdateState::COLLECT_RESPONSES;
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      if (pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE) == true)
      {
        triggerNextRequestAsync();
      }
      break;
    case Bus::UpdateState::COMPLETE: /* Fall-through */
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
      nodePtr->processResponseBuffer();
      if (!firstError) firstError = nodePtr->getBusError();
    }
  }

  return (firstError);
}

void Bus::beginSetNodeConfigProcess(const uint8_t         nodeIDToSet, 
                                    const BitrateOption_t bitrateOption,
                                    const uint32_t        watchdogPeriod)
{
  setupConfig_.nodeID         = nodeIDToSet;
  setupConfig_.bitrateOption  = bitrateOption;
  setupConfig_.watchdogPeriod = watchdogPeriod;
  configUpdateProcessHandler_.resetProcess();
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
      if (!Platform::BusPeripheral::startPeripheral()) process.terminate(Atams::ERROR_PLATFORM);
      else                                             configUpdateState = Bus::ConfigUpdateState::INIT_NODE;
      break;
    case Bus::ConfigUpdateState::INIT_NODE:
      process.error = dummyNode_.init(dummyMemoryMap_);
      if (process.error) process.terminate(process.error);
      else               configUpdateState = Bus::ConfigUpdateState::BEGIN_CONFIG_ENTRY;
      break;
    case Bus::ConfigUpdateState::BEGIN_CONFIG_ENTRY:
      nodeProcessHandler_.beginConfigEntryProcess(&dummyNode_);
      configUpdateState = Bus::ConfigUpdateState::UPDATE_CONFIG_ENTRY;
      break;
    case Bus::ConfigUpdateState::UPDATE_CONFIG_ENTRY:
      subProcessState = nodeProcessHandler_.updateConfigurationStateEntry(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) configUpdateState = Bus::ConfigUpdateState::WRITE_CONFIG;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      else                                                       triggerNextSetNodeConfigCycle(configUpdateState);
      break;
    case Bus::ConfigUpdateState::WRITE_CONFIG:
      startWriteConfigVars();
      triggerNextSetNodeConfigCycle(Bus::ConfigUpdateState::CHECK_ACK);
      break;
    case Bus::ConfigUpdateState::CHECK_ACK:
      if (allConfigVarsAcknowledged()) configUpdateState = Bus::ConfigUpdateState::BEGIN_CONFIG_EXIT;
      else                             process.terminate(Atams::ERROR_ACK_NOT_RECEIVED);
      break;
    case Bus::ConfigUpdateState::BEGIN_CONFIG_EXIT:
      nodeProcessHandler_.beginConfigExitProcess(&dummyNode_, true);
      configUpdateState = Bus::ConfigUpdateState::UPDATE_CONFIG_EXIT;
      break;
    case Bus::ConfigUpdateState::UPDATE_CONFIG_EXIT:
      subProcessState = nodeProcessHandler_.updateConfigurationStateExit(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) configUpdateState = Bus::ConfigUpdateState::READ_CONFIG;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      else                                                       triggerNextSetNodeConfigCycle(configUpdateState);
      break;
    case Bus::ConfigUpdateState::READ_CONFIG:
      startReadConfigVars();
      triggerNextSetNodeConfigCycle(Bus::ConfigUpdateState::CHECK_CONFIG);
      break;
    case Bus::ConfigUpdateState::CHECK_CONFIG:
      process.error = validateConfigVars();
      if (process.error) process.terminate(process.error);
      else              configUpdateState = Bus::ConfigUpdateState::BEGIN_SAVE_ALL;
      break;
    case Bus::ConfigUpdateState::BEGIN_SAVE_ALL:
      nodeProcessHandler_.beginStorageProcess(&dummyNode_);
      configUpdateState = Bus::ConfigUpdateState::UPDATE_SAVE_ALL;
      break;
    case Bus::ConfigUpdateState::UPDATE_SAVE_ALL:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      else                                                       triggerNextSetNodeConfigCycle(configUpdateState);
      break;
    case Bus::ConfigUpdateState::SEND_REQUEST:
      if (pollForRequestTransmit(dummyNode_, process, MESSAGE_BROADCAST_UNIVERSAL) == true)
      {
        if (process.error) process.terminate(process.error);
        else               configUpdateState = Bus::ConfigUpdateState::GET_RESPONSE;               
      }
      break;
    case Bus::ConfigUpdateState::GET_RESPONSE:
      Platform::BusPeripheral::update();
      if (pollForResponse(dummyNode_, process, MESSAGE_BROADCAST_RESPONSE) == true)
      {
        if (process.error) process.terminate(process.error);
        else               
        {
          dummyNode_.processResponseBuffer();
          configUpdateState = process.nextSpecificState;        
        }
      }
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

bool Bus::pollForRequestTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t requestType)
{
  uint32_t currentTime          = Platform::getMillis();
  bool     messageSendAttempted = false;
  
  if (Platform::BusPeripheral::transmitReady())
  {
    if (node.getEncodedRequestPacket(requestType,
                                     encodedBuffer_, 
                                     sizeof(encodedBuffer_), 
                                     encodedLength_) == Atams::ERROR_NONE)
    {
      if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
      {
        node.reportBusError(Atams::ERROR_PLATFORM);
        process.error = Atams::ERROR_PLATFORM;
      }
    }

    messageSendAttempted  = true;
    process.prevEventTime = currentTime;
  }

  return (messageSendAttempted);
}

bool Bus::pollForJogTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process)
{
  uint32_t currentTime          = Platform::getMillis();
  bool     messageSendAttempted = false;

  jogBuffer_[MESH_INDEX_NODE_ID] = node.getNodeID();
  
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
    }
    messageSendAttempted  = true;
    process.prevEventTime = currentTime;
  }

  return (messageSendAttempted);
}

bool Bus::pollForResponse(Atams::Node &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t expectedResponse)
{
  uint32_t currentTime  = Platform::getMillis();
  bool     waitOverFlag = false;

  if ((circularBuffer_.getPacket(rxBuffer_, sizeof(rxBuffer_), rxLength_) == CircularBuffer::ERROR_NONE) &&
      (validateAndStoreResponsePacket(node, expectedResponse)             != Atams::ERROR_DECODE       )  )
  {  
    waitOverFlag          = true;
    process.prevEventTime = currentTime;
  }
  else if (currentTime - process.prevEventTime > Platform::BUS_RESPONSE_TIMEOUT)
  { 
    node.reportBusError(Atams::ERROR_RESPONSE_TIMEOUT);
    process.error         = Atams::ERROR_RESPONSE_TIMEOUT;
    waitOverFlag          = true;
    process.prevEventTime = currentTime;
  }

  return (waitOverFlag);
}

void Bus::rxCallback(uint8_t *rxBufferPtr, const uint16_t rxBufferLength)
{
  static_cast<void>(circularBuffer_.pushHead(rxBufferPtr, rxBufferLength));
}

Atams::Error_t Bus::validateAndStoreResponsePacket(Node &node, const MessageType_t expectedResponse)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (decodeMeshPacket(rxBuffer_, 
                       rxLength_, 
                       decodedBuffer_, 
                       sizeof(decodedBuffer_), 
                       decodedLength_) == Atams::ERROR_NONE)
  {
    uint8_t              packetNodeID    = decodedBuffer_[MESH_INDEX_NODE_ID];
    uint8_t              packetSyncCount = decodedBuffer_[MESH_INDEX_SYNC];
    Atams::MessageType_t messageType     = static_cast<MessageType_t>(decodedBuffer_[MESH_INDEX_MSG_TYPE]);

    if      (packetSyncCount != activeSyncCount_)              statusReturn = Atams::ERROR_SYNC_COUNT;
    else if ((messageType    != MESSAGE_BROADCAST_RESPONSE) &&
             (packetNodeID   != node.getNodeID()          ) )  statusReturn = Atams::ERROR_SYNC_NODE;
    else if ((messageType    != expectedResponse        ) &&
             (messageType    != MESSAGE_ABORTED_RESPONSE) )    statusReturn = Atams::ERROR_MESSAGE_TYPE;
    else                                                       node.responseReceived(decodedBuffer_, decodedLength_);

    if (statusReturn != Atams::ERROR_NONE) node.reportBusError(statusReturn);
  }
  else 
  {
    statusReturn = Atams::ERROR_DECODE;
  }
  
  return (statusReturn);
}

Atams::Error_t Bus::assignNodeIDs(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if ((nodePtrs_[0U]                   == nullptr) ||
      (nodePtrs_[noOfNodesOnBus_ - 1U] == nullptr) )
  {
    return (Atams::ERROR_NULLPTR); /* Early Return */
  }

  const uint8_t firstNodeID    = nodePtrs_[0U]->getNodeID();
  const uint8_t lastNodeID     = nodePtrs_[noOfNodesOnBus_ - 1U]->getNodeID();
  uint8_t       previousNodeID = firstNodeID;

  for (Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr == nullptr) break;

    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_FIRST_NODE_ID,    firstNodeID);
    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_LAST_NODE_ID,     lastNodeID);
    if (!error) error = nodePtr->write(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, previousNodeID);

    previousNodeID = nodePtr->getNodeID();
  }

  return (error);
}

void Bus::beginValidateGenInfoAllNodes(void)
{
  validateGenInfoProcessHandler_.resetProcess();
}

void beginValidateIDsAllNodes(void);

void beginConfigurationEntryAllNodes(void);

void beginStoreAllNodes(void);

Atams::ProcessState Bus::updateValidateGenInfoAllNodes(Atams::Error_t &error)
{
  (void)error;
  //Bus::ProcessHandler<Bus::ValidateState> &process          = validateGenInfoProcessHandler_;
  //Atams::ProcessState                     &subProcessState  = process.subProcessState;
  //Bus::ValidateState                      &validateState    = process.specificState;
  //bool                                    &allNodesComplete = process.allNodesComplete;
  //Atams::Error_t                           tempError        = Atams::ERROR_NONE;
  //bool                                     tempFlag         = false;
//
  //switch (validateState)
  //{
  //  case Bus::ValidateState::START:
  //    for (Node *&nodePtr : nodePtrs_) if (nodePtr != nullptr) nodePtr->beginValidateGenInfo();
  //    validateState = Bus::ValidateState::UPDATE;
  //    break;
  //  case Bus::ValidateState::UPDATE:
  //    allNodesComplete = true;
  //    for (Node *&nodePtr : nodePtrs_)
  //    {
  //      if ((nodePtr                                             != nullptr                         ) &&
  //          (nodePtr->updateValidateGenInfo(tempFlag, validFlag) == Atams::ProcessState::IN_PROGRESS) )
  //      {
  //        allNodesComplete = false;
  //      }
  //    }
  //    if (allNodesComplete == true) validateState = Bus::ValidateState::VALIDATE;
  //    break;
  //  case Bus::ValidateState::VALIDATE:
  //    for (Node *&nodePtr : nodePtrs_)
  //    {
  //      if (nodePtr != nullptr)
  //      {
  //        subProcessState = nodePtr->updateValidateGenInfo(tempError, validFlag);
  //        
  //        if (subProcessState == Atams::ProcessState::COMPLETE)
  //        {
  //          if (validFlag == false) process.terminate(Atams::ERROR_MEMORY_MAP);
  //          else                    process.setProcessComplete();
  //        }
  //        else if (subProcessState == Atams::ProcessState::ERROR)
  //        {
  //          process.terminate(tempError);
  //        }
  //      }
  //    }
  //    break;
  //  case Bus::ValidateState::COMPLETE:
  //  case Bus::ValidateState::ERROR:
  //    /* Do Nothing - Transitions handled by ProcessHandler */
  //    break;
  //  default:
  //    process.terminate(Atams::ERROR_INVALID_CASE);
  //    break;
  //}
//
  //error = process.error;
//
  //return (validateState);
  return (Atams::ProcessState::ERROR);
}

void Bus::startWriteConfigVars(void)
{
  static_cast<void>(dummyNode_.clearAckStartWrite(BlockUniversal::VAR_ID_NODE_ID,         setupConfig_.nodeID));
  static_cast<void>(dummyNode_.clearAckStartWrite(BlockUniversal::VAR_ID_BITRATE,         setupConfig_.bitrateOption));
  static_cast<void>(dummyNode_.clearAckStartWrite(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, setupConfig_.watchdogPeriod));
}

bool Bus::allConfigVarsAcknowledged(void)
{
  bool ackReceived = true;

  if (ackReceived) static_cast<void>(dummyNode_.stopStreamGetAckFlag(BlockUniversal::VAR_ID_NODE_ID,         ackReceived));
  if (ackReceived) static_cast<void>(dummyNode_.stopStreamGetAckFlag(BlockUniversal::VAR_ID_BITRATE,         ackReceived));
  if (ackReceived) static_cast<void>(dummyNode_.stopStreamGetAckFlag(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, ackReceived));

  return (ackReceived);
}

void Bus::startReadConfigVars(void)
{
  static_cast<void>(dummyNode_.clearDataReadyStartRead(BlockUniversal::VAR_ID_NODE_ID));
  static_cast<void>(dummyNode_.clearDataReadyStartRead(BlockUniversal::VAR_ID_BITRATE));
  static_cast<void>(dummyNode_.clearDataReadyStartRead(BlockUniversal::VAR_ID_WATCHDOG_PERIOD));
}

Atams::Error_t Bus::validateConfigVars(void)
{
  UniversalConfig_t nodeConfig;
  Atams::Error_t    error = Atams::ERROR_NONE;

  if (!error) error = dummyNode_.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_NODE_ID,         nodeConfig.nodeID);
  if (!error) error = dummyNode_.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_BITRATE,         nodeConfig.bitrateOption);
  if (!error) error = dummyNode_.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, nodeConfig.watchdogPeriod);

  if ((!error) && (nodeConfig != setupConfig_)) error = Atams::ERROR_SET_CONFIG_VAR_FAILED;

  return (error);
}

Atams::Node * Bus::getActiveNodePtr(void)
{
  Node *nodePtr = nullptr;

  if ((activeNodeIndex_            < Platform::NUMBER_OF_NODES_PER_BUS) &&
      (activeNodeIndex_            < noOfNodesOnBus_                  ) &&
      (nodePtrs_[activeNodeIndex_] != nullptr                         ) ) 
  {
    nodePtr = nodePtrs_[activeNodeIndex_];
  }
  
  return (nodePtr);
}

bool Bus::tryNodeIncrement(void)
{
  bool incrementValid = false;

  if (activeNodeIndex_ + 1U < noOfNodesOnBus_)
  {
    activeNodeIndex_++;
    incrementValid = true;
  }

  return (incrementValid);
}

void Bus::startResponseCollectionSync(void)
{
  activeNodeIndex_ = 0U;
  updateProcessHandler_.specificState = Bus::UpdateState::COLLECT_RESPONSES;
}

void Bus::triggerJogSync(void)
{
  //Platform::CommsSemaphore::release();
  updateProcessHandler_.specificState = Bus::UpdateState::JOG_NODE;
}

void Bus::triggerNextRequestAsync(void)
{
  if (tryNodeIncrement() == false) 
  {
    updateProcessHandler_.setProcessComplete();
  }
  else
  { 
    //Platform::CommsSemaphore::release();
    updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;
  }
}

void Bus::triggerNextSetNodeConfigCycle(const Bus::ConfigUpdateState nextState)
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
    /* .noOfVars    = */ BlockUniversal::NUMBER_OF_UNIVERSAL_VARS,
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
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::START;
  this->processState  = Atams::ProcessState::IN_PROGRESS;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
