/**
  ******************************************************************************
  * @file    Bus.cpp
  *
  * @author  D. Baines
  *
  * @brief   Atams Bus class for managing Node communication.
  *
  * @details The Bus class coordinates communication and synchronisation between the Hub and multiple Node instances.
  *          All Bus member functions must be called from the same thread or context to ensure correct operation.
  *          Node functions (such as @c Node::setVar, @c Node::getVar, and @c Node::setRequestPattern) may be called
  *          from other threads or contexts, provided the user has correctly filled the multi-threading/concurrency
  *          function definitions in the Platform files.
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

/**
 * @brief Constructs a Bus instance with user-defined platform data.
 *
 * Initialises the Bus and its base @c Platform::BusPeripheral with the provided @ref Platform::BusPeripheral::UserData_t "UserData_t" 
 * structure. The @ref Platform::BusPeripheral::UserData_t "UserData_t" struct is defined by the user in @c Platform.hpp inside the 
 * @c BusPeripheral class definition, and must be passed to the Bus constructor to provide access to any data required for the user's 
 * implementation of @c Platform::BusPeripheral. All Node pointers are initialised to @c nullptr.
 *
 * @param userData User-defined data required for initialising the underlying bus peripheral.
 */
Bus::Bus(Platform::BusPeripheral::UserData_t userData) : 
Platform::BusPeripheral(userData),
circularBuffer_(Atams::EOL_BYTE)
{
  for (Atams::Node *&nodePtr : nodePtrs_) nodePtr = nullptr; 
}

/**
 * @brief Adds a Node to the Bus.
 *
 * Registers the specified @c Node instance with this Bus. The Node pointer is stored internally, and the Node will participate in 
 * subsequent Bus operations. If the Bus is already full or the Node is already present, the function returns an appropriate error code 
 * and does not add the Node again.
 *
 * @param node Reference to the @c Node instance to add to the Bus.
 *
 * @retval @c ERROR_NONE                Node successfully added to the Bus.
 * @retval @c ERROR_BUS_FULL            The Bus has reached the maximum number of Nodes allowed.
 * @retval @c ERROR_NODE_ALREADY_ON_BUS The specified Node is already registered on this Bus.
 *
 * @note The @c Node object must exist for the entire period it is registered on the Bus. 
 *       Before destroying or removing a Node, call @c removeNodeFromBus and ensure it returns @c Atams::ERROR_NONE to confirm the Node 
 *       has been safely removed.
 */
Atams::Error_t Bus::addNodeToBus(Atams::Node &node)
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

/**
 * @brief Removes a Node from the Bus.
 *
 * Deregisters the specified @c Node instance from this Bus, so it will no longer participate in Bus operations.
 * The function will fail if the Node is not currently registered, or if a Bus update cycle or related process is in progress.
 * Removing a Node invalidates the current Bus initialisation; the Bus initialisation process must be completed again before 
 * further Bus operations can proceed.
 *
 * @param node Reference to the @c Node instance to remove from the Bus.
 *
 * @retval @c ERROR_NONE                     Node successfully removed from the Bus.
 * @retval @c ERROR_NODE_NOT_ON_BUS          The specified Node is not registered on this Bus.
 * @retval @c ERROR_UPDATE_CYCLE_IN_PROGRESS A Bus update cycle or related process is currently in progress; try again when the Bus is idle.
 *
 * @note Ensure that @c removeNodeFromBus returns @c Atams::ERROR_NONE before destroying or removing the Node object.
 *       It is not safe to remove a Node while any Bus update cycle or related process is active.
 *       After removing a Node, you must re-run the Bus initialisation procedure before starting any further Bus operations.
 */
Atams::Error_t Bus::removeNodeFromBus(Node &node)
{
  if (canRemoveNode() == false)
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  bool nodeFound = false;

  for (uint16_t findIndex = 0U; findIndex < noOfNodesOnBus_; findIndex++)
  {
    if (nodePtrs_[findIndex] == &node)
    {
      for (uint16_t moveIndex = findIndex; moveIndex < noOfNodesOnBus_ - 1U; moveIndex++)
      {
        nodePtrs_[moveIndex] = nodePtrs_[moveIndex + 1U];
      }

      noOfNodesOnBus_--;
      nodeFound = true;
    }
  }

  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (!nodeFound)
  {
    statusReturn = Atams::ERROR_NODE_NOT_ON_BUS;
  }
  else 
  {
    initProcessHandler_.specificState = Bus::InitState::ERROR;
    initProcessHandler_.processState  = Atams::ProcessState::ERROR;
    initProcessHandler_.error         = Atams::ERROR_INIT_ORDER;
    statusReturn                      = Atams::ERROR_NONE;
  }

  return (statusReturn);
}

/**
 * @brief Begins the Bus initialisation process.
 *
 * Sets the internal state to start the Bus initialisation sequence for all registered Nodes. This function does not perform the full
 * initialisation itself; the process is progressed through repeated calls to @ref Bus::updateBusInitProcess.
 *
 * The initialisation process validates that the Memory Map used by each physical Node device matches the Memory Map used to initialise
 * the corresponding internal Node instance. For Atams synchronous communication, each Node is configured with the IDs of the first, 
 * last, and previous Node on the Bus. These IDs are stored in the Universal Data Block of each Node during initialisation and saved to 
 * non-volatile memory, ensuring synchronous communication remains functional even after a Node is power-cycled.
 *
 * The Bus initialisation process does not set the Node ID that each Node uses for all communications. Individual device Node IDs must
 * first be set using the Node configuration process (@ref Bus::beginSetNodeConfigProcess and @ref Bus::updateSetNodeConfigProcess),
 * which should be performed when Nodes are added to the Bus one by one.
 *
 * @note The initialisation procedure will be significantly faster after the first time, provided the set of Nodes on the physical Bus
 *       remains unchanged. If Nodes are added or removed, the full initialisation process will be required again.
 *
 * @retval @c ERROR_NONE      Bus initialisation process successfully started.
 * @retval @c ERROR_BUS_EMPTY No Nodes are registered on the Bus.
 * @retval @c ERROR_NULLPTR   A registered Node pointer is null.
 * @retval @c ERROR_PLATFORM  Failed to start data reception on the platform bus peripheral.
 */
Atams::Error_t Bus::beginBusInitProcess(void)
{
  if (noOfNodesOnBus_ == 0U) 
  {
    return (Atams::ERROR_BUS_EMPTY); /* Early Return */
  }

  Atams::Node *firstNodePtr = nodePtrs_[0U];
  Atams::Node *lastNodePtr  = nodePtrs_[noOfNodesOnBus_ - 1U];

  if ((firstNodePtr == nullptr) ||
      (lastNodePtr  == nullptr) ) 
  {
    return (Atams::ERROR_NULLPTR); /* Early Return */
  }

  if (BusPeripheral::startReceive() == false) 
  {
    return (Atams::ERROR_PLATFORM); /* Early Return */
  }
  
  initProcessHandler_.activeNodePtr = firstNodePtr;
  busIDsToSet_.firstNodeID          = firstNodePtr->getNodeID();
  busIDsToSet_.lastNodeID           = lastNodePtr->getNodeID();
  busIDsToSet_.previousNodeID       = firstNodePtr->getNodeID();

  initProcessHandler_.readyProcess();
  initProcessHandler_.error         = Atams::ERROR_NONE;
  initProcessHandler_.specificState = Bus::InitState::START;
  initNodeIndex_                    = 0U;
  
  for (Atams::Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) nodePtr->clearAllRequestPatterns();
  }

  return (Atams::ERROR_NONE);
}

Atams::ProcessState Bus::updateBusInitProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::InitState> &process         {initProcessHandler_};
  Atams::ProcessState                 &processState    {process.processState};
  Atams::Error_t                       cycleError      {Atams::ERROR_NONE};
  Atams::ProcessState                 &subProcessState {process.subProcessState};
  Bus::InitState                      &initState       {process.specificState};
  bool                                 dataIsValid     {false}; 
  Atams::Node                        *&initNodePtr     {process.activeNodePtr};

  if (initNodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
    error = process.error;
    return (processState);
  }

  Atams::Node &node {*initNodePtr};

  if ((runSingleNodeUpdateCycle(cycleError, node) == Atams::ProcessState::IN_PROGRESS) ||
      (processState                               != Atams::ProcessState::IN_PROGRESS) )
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  switch (initState)
  {
    case Bus::InitState::START:
      beginInitValidateGenInfo(node);
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

  if ((process.error == Atams::ERROR_NONE               ) &&
      (processState  == Atams::ProcessState::IN_PROGRESS) ) 
  {
    static_cast<void>(beginSingleNodeUpdateCyclePrivate(node));
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

Atams::Error_t Bus::beginSingleNodeUpdateCycle(Atams::Node &node)
{
  if (initProcessHandler_.processState != Atams::ProcessState::COMPLETE)
  {
    return (Atams::ERROR_INIT_ORDER); /* Early Return */
  }

  return (beginSingleNodeUpdateCyclePrivate(node));
}

Atams::ProcessState Bus::runUpdateCycleSync(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::UpdateState> &process       = updateProcessHandler_;
  Atams::ProcessState                   &processState  = process.processState;
  Bus::UpdateState                      &updateState   = process.specificState;
  Atams::Node                          *&activeNodePtr = process.activeNodePtr;
  Bus::PollResult                        rxPollResult  = Bus::PollResult::WAITING;

  activeNodePtr = getUpdateNodePtr();

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
  Atams::Node                          *&activeNodePtr          = process.activeNodePtr;
  NodeCallbackHandler                   *nodeCallbackHandlerPtr = activeNodePtr;
  Bus::PollResult                        rxPollResult           = Bus::PollResult::WAITING;

  activeNodePtr = getUpdateNodePtr();

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

Atams::ProcessState Bus::runSingleNodeUpdateCycle(Atams::Error_t &error, Atams::Node &node)
{ 
  Bus::ProcessHandler<Bus::UpdateState> &process             = singleNodeUpdateProcessHandler_;
  Atams::ProcessState                   &processState        = process.processState;
  Bus::UpdateState                      &updateState         = process.specificState;
  NodeCallbackHandler                   &nodeCallbackHandler = node;
  Bus::PollResult                        rxPollResult        = Bus::PollResult::WAITING;

  //Platform::CommsSemaphore::waitWithTimeout(Atams::WATCHDOG_PERIOD_MILLISECONDS);

   Platform::BusPeripheral::update();
   
  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(node, process, Atams::MESSAGE_REQUEST))
      {
        if (process.error) process.terminate(process.error);
        else               updateState = Bus::UpdateState::COLLECT_RESPONSES;
      }
      break;

    case Bus::UpdateState::COLLECT_RESPONSES:
      
      rxPollResult = pollForResponse(node, process, Atams::MESSAGE_RESPONSE);

      if (rxPollResult != Bus::PollResult::WAITING)
      {
        nodeCallbackHandler.processResponseBuffer();

        process.error = node.getBusError();

        if (process.error) process.terminate(process.error);
        else               process.setProcessComplete();
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

Atams::Error_t Bus::processSyncBuffers(void)
{
  if (updateProcessHandler_.processState == Atams::ProcessState::IN_PROGRESS) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  Atams::Error_t firstError = Atams::ERROR_NONE;

  for (Atams::Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) 
    {
      Atams::Node         &node          = *nodePtr;     
      NodeCallbackHandler &nodeCallbacks = node;
      nodeCallbacks.processResponseBuffer();
      if (!firstError) firstError = node.getBusError();
    }
  }

  return (firstError);
}

Atams::Error_t Bus::beginSetNodeConfigProcess(const NodeConfig_t &userConfig)
{
  if (BusPeripheral::startReceive() == false) 
  {
    return (Atams::ERROR_PLATFORM); /* Early Return */
  }

  userConfigToSet_ = userConfig;
  dummyNode_.setNodeID(userConfig.currentNodeID);
  configUpdateProcessHandler_.readyProcess();
  configUpdateProcessHandler_.specificState = Bus::ConfigUpdateState::START;
  dummyNode_.clearAllRequestPatterns();

  return (Atams::ERROR_NONE);
}

Atams::ProcessState Bus::updateSetNodeConfigProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           {configUpdateProcessHandler_};
  Atams::ProcessState                         &processState      {process.processState};
  Atams::ProcessState                         &subProcessState   {process.subProcessState};
  Bus::ConfigUpdateState                      &configUpdateState {process.specificState};
  Atams::Error_t                               cycleError        {Atams::ERROR_NONE};

  if ((runSingleNodeUpdateCycle(cycleError, dummyNode_) == Atams::ProcessState::IN_PROGRESS) ||
      (processState                                     != Atams::ProcessState::IN_PROGRESS) )
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  switch (configUpdateState) 
  { 
    case Bus::ConfigUpdateState::START:
      process.error = dummyNode_.init(dummyMemoryMap_);
      if (process.error) process.terminate(process.error);
      else               beginSetConfigWrite();
      break;
    case Bus::ConfigUpdateState::WRITE_CONFIG:
      subProcessState = nodeProcessHandler_.updateSetNodeConfig(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) beginSetConfigStore();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case Bus::ConfigUpdateState::STORE_CONFIG:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
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
    
  if ((process.error == Atams::ERROR_NONE               ) &&
      (processState  == Atams::ProcessState::IN_PROGRESS) ) 
  {
    static_cast<void>(beginSingleNodeUpdateCyclePrivate(dummyNode_));
  }

  error = process.error;

  return (process.processState);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

bool Bus::canRemoveNode(void) 
{
    return ((updateProcessHandler_.processState           != Atams::ProcessState::IN_PROGRESS) &&
            (singleNodeUpdateProcessHandler_.processState != Atams::ProcessState::IN_PROGRESS) &&
            (initProcessHandler_.processState             != Atams::ProcessState::IN_PROGRESS) &&
            (configUpdateProcessHandler_.processState     != Atams::ProcessState::IN_PROGRESS) );
}

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
  updateProcessHandler_.readyProcess();
  updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;

  return (Atams::ERROR_NONE);
}

Atams::Error_t Bus::beginSingleNodeUpdateCyclePrivate(Atams::Node &node)
{
  if (singleNodeUpdateProcessHandler_.processState == Atams::ProcessState::IN_PROGRESS)
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  static_cast<NodeCallbackHandler&>(node).clearBusError();
  static_cast<NodeCallbackHandler&>(node).clearAbortDetails();
  circularBuffer_.reset();
  activeSyncCount_++;
  singleNodeUpdateProcessHandler_.activeNodePtr = &node;
  singleNodeUpdateProcessHandler_.readyProcess();
  singleNodeUpdateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;

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

bool Bus::pollForRequestTransmit(Atams::Node                &node, 
                                 Bus::ProcessHandlerBase    &process,
                                 const Atams::MessageType_t  requestType)
{
  Atams::NodeCallbackHandler &nodeCallbacks        = node; 
  uint32_t                    currentTime          = Platform::getMillis();
  bool                        messageSendAttempted = false;
  
  if (Platform::BusPeripheral::transmitReady())
  {
    if (nodeCallbacks.getEncodedRequestPacket(requestType,
                                              activeSyncCount_,
                                              encodedBuffer_, 
                                              sizeof(encodedBuffer_), 
                                              encodedLength_) == Atams::ERROR_NONE)
    {
      if (encodedLength_ < 7U)
      {
        encodedLength_ = 0U;
      }
      if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
      {
        nodeCallbacks.reportBusError(Atams::ERROR_PLATFORM);
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

bool Bus::pollForJogTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process)
{
  Atams::NodeCallbackHandler &nodeCallbacks        = node;
  uint32_t                    currentTime          = Platform::getMillis();
  bool                        messageSendAttempted = false;

  jogBuffer_[HEADER_INDEX_NODE_ID]  = node.getNodeID();
  jogBuffer_[HEADER_INDEX_MSG_TYPE] = Atams::MESSAGE_SYNC_JOG;
  jogBuffer_[HEADER_INDEX_SYNC]     = activeSyncCount_;

  if (Platform::BusPeripheral::transmitReady())
  {
    if (encodeBusPacket(jogBuffer_, 
                        HEADER_SIZE_HEADER, 
                        encodedBuffer_, 
                        sizeof(encodedBuffer_), 
                        encodedLength_) == Atams::ERROR_NONE)
    {
      if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
      {
        nodeCallbacks.reportBusError(Atams::ERROR_PLATFORM);
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

Bus::PollResult Bus::pollForResponse(Atams::Node &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t expectedResponse)
{
  Atams::NodeCallbackHandler &nodeCallbacks = node;
  uint32_t                    currentTime   = Platform::getMillis();
  Bus::PollResult             result        = Bus::PollResult::WAITING;

  if ((circularBuffer_.getPacket(rxBuffer_, sizeof(rxBuffer_), rxLength_) == CircularBuffer::ERROR_NONE) &&
      (validateAndStoreResponsePacket(node, expectedResponse)             == true                      ) )
  {
    result = Bus::PollResult::READY;
    process.prevEventTime = currentTime;
  }

  else if (currentTime - process.prevEventTime > Platform::BUS_RESPONSE_TIMEOUT)
  { 
    nodeCallbacks.reportBusError(Atams::ERROR_RESPONSE_TIMEOUT);
    result = Bus::PollResult::TIMEOUT;
    process.prevEventTime = currentTime;
  }

  return (result);
}

void Bus::rxCallback(uint8_t *rxBufferPtr, const uint16_t rxBufferLength) 
{
  static_cast<void>(circularBuffer_.pushHead(rxBufferPtr, rxBufferLength));
}

bool Bus::validateAndStoreResponsePacket(Atams::Node &node, const MessageType_t expectedResponse)
{
  Atams::NodeCallbackHandler &nodeCallbacks = node;
  Atams::Error_t              error         = Atams::ERROR_NONE;
  bool                        packetValid   = true;

  error = decodeBusPacket(rxBuffer_, 
                          rxLength_, 
                          decodedBuffer_, 
                          sizeof(decodedBuffer_), 
                          decodedLength_);

  if (error == Atams::ERROR_NONE)
  {
    uint8_t              packetNodeID    = decodedBuffer_[HEADER_INDEX_NODE_ID];
    uint8_t              packetSyncCount = decodedBuffer_[HEADER_INDEX_SYNC];
    Atams::MessageType_t messageType     = static_cast<MessageType_t>(decodedBuffer_[HEADER_INDEX_MSG_TYPE]);

     bool messageIsAbort = ((messageType == Atams::MESSAGE_ABORT_RESPONSE       ) ||
                            (messageType == Atams::MESSAGE_ABORT_RESPONSE_SYNCED) );

    if      (messageType     == lastSentMessageType_) packetValid = false;
    else if (packetSyncCount != activeSyncCount_)     error = Atams::ERROR_SYNC_COUNT;
    else if (packetNodeID    != node.getNodeID())     error = Atams::ERROR_SYNC_NODE;
    else if ((messageType    != expectedResponse) &&
             (messageIsAbort == false           ) )   error = Atams::ERROR_MESSAGE_TYPE;
    else                                              nodeCallbacks.responseReceived(decodedBuffer_, decodedLength_);

    if (error != Atams::ERROR_NONE) nodeCallbacks.reportBusError(error);
  }
  else 
  {
    packetValid = false;
  }

  return (packetValid);
}

Atams::Node * Bus::getUpdateNodePtr(void)
{
  Atams::Node *nodePtr = nullptr;

  if ((updateNodeIndex_ < Platform::NUMBER_OF_NODES_PER_BUS) &&
      (updateNodeIndex_ < noOfNodesOnBus_                  ) ) 
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

    process.activeNodePtr = nodePtrs_[initNodeIndex_];

    Atams::Node *prevNodePtr  = nodePtrs_[initNodeIndex_ - 1U];
    Atams::Node *firstNodePtr = nodePtrs_[0];
    Atams::Node *lastNodePtr  = nodePtrs_[noOfNodesOnBus_ - 1U];

    if ((process.activeNodePtr == nullptr) ||
        (prevNodePtr           == nullptr) ||
        (firstNodePtr          == nullptr) ||
        (lastNodePtr           == nullptr) ) 
    {
      process.terminate(Atams::ERROR_NULLPTR);
    }
    else 
    {
      busIDsToSet_.firstNodeID    = firstNodePtr->getNodeID();
      busIDsToSet_.lastNodeID     = lastNodePtr->getNodeID();
      busIDsToSet_.previousNodeID = prevNodePtr->getNodeID();
      beginInitValidateGenInfo(*process.activeNodePtr);
    }
  }
  else
  {
    process.setProcessComplete();
  }
}

void Bus::beginSetConfigWrite(void)
{
  configUpdateProcessHandler_.specificState = Bus::ConfigUpdateState::WRITE_CONFIG;
  nodeProcessHandler_.beginSetNodeConfig(dummyNode_, userConfigToSet_);
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

  if (pollForRequestTransmit(dummyNode_, process, MESSAGE_REQUEST) == true)
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

  if (pollForResponse(dummyNode_, process, MESSAGE_RESPONSE) != Bus::PollResult::WAITING)
  {
    dummyNodeCallbackHandler_.processResponseBuffer();
    configUpdateState = process.nextSpecificState;  
  }
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

  if (this->activeNodePtr != nullptr) 
  {
    static_cast<NodeCallbackHandler*>(this->activeNodePtr)->reportBusError(exitError);
  }
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
void Bus::ProcessHandler<T>::readyProcess(void)
{
  this->error           = Atams::ERROR_NONE;
  this->processState    = Atams::ProcessState::IN_PROGRESS;
  this->subProcessState = Atams::ProcessState::IN_PROGRESS;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
