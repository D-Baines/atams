/**
  ******************************************************************************
  * @file    Bus.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams Hub Bus class.
  *
  * @details The Atams::Bus class coordinates communication and synchronisation 
  *          between the Hub and multiple Node instances. All Bus member 
  *          functions must be called from the same thread or context to ensure
  *          correct operation. Node functions (such as @c Node::setVar, 
  *          @c Node::getVar, and @c Node::setRequestPattern) may be called from
  *          other threads or contexts, provided the user has correctly filled 
  *          the appropriate multi-threading/concurrency function definitions in 
  *          the Platform files.
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
 * Initialises the Bus and its base @c Platform::BusPeripheral with the provided @ref Platform::BusPeripheral::UserData_t
 * structure. The @ref Platform::BusPeripheral::UserData_t struct is defined by the user in @c Platform.hpp inside the
 * @c BusPeripheral class definition, and must be passed to the Bus constructor to provide access to any data required
 * for the user's implementation of @c Platform::BusPeripheral. All Node pointers are initialised to @c nullptr.
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
  if (safeToRemoveNode() == false)
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  bool nodeFound {false};

  for (uint16_t findIndex {0U}; findIndex < noOfNodesOnBus_; findIndex++)
  {
    if (nodePtrs_[findIndex] == &node)
    {
      for (uint16_t moveIndex {findIndex}; moveIndex < (noOfNodesOnBus_ - 1U); moveIndex++)
      {
        nodePtrs_[moveIndex] = nodePtrs_[moveIndex + 1U];
      }

      noOfNodesOnBus_--;
      nodeFound = true;
    }
  }

  Atams::Error_t error {Atams::ERROR_NONE};

  if (!nodeFound)
  {
    error = Atams::ERROR_NODE_NOT_ON_BUS;
  }
  else 
  {
    initProcessHandler_.specificState = Bus::InitState::ERROR;
    initProcessHandler_.processState  = Atams::PROCESS_ERROR;
    initProcessHandler_.error         = Atams::ERROR_INIT_ORDER;
  }

  return (error);
}

/**
 * @brief Begins the Bus initialisation process.
 *
 * Sets the internal state to start the Bus initialisation sequence for all registered Nodes. This function does not perform the full
 * initialisation itself; the process is progressed through repeated calls to @ref Bus::updateBusInitProcess.
 *
 * The initialisation process validates that the Memory Map used by each physical Node device matches the Memory Map of the corresponding
 * internal Node instance. For Atams synchronous communication, each Node is configured with the IDs of the first, last, and previous
 * Node on the Bus. These IDs are stored in the Universal Data Block of each Node during initialisation and saved to non-volatile
 * memory, ensuring synchronous communication remains functional even after a Node is reset.
 *
 * The Bus initialisation process does not set the Node ID that a Node uses for all communications. Individual device Node IDs must first
 * be set using the Node configuration process (@ref Bus::beginSetNodeConfigProcess and @ref Bus::updateSetNodeConfigProcess), which
 * should be performed when Nodes are added to the Bus one by one.
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

  Atams::Node *firstNodePtr {nodePtrs_[0U]};
  Atams::Node *lastNodePtr  {nodePtrs_[noOfNodesOnBus_ - 1U]};

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

/**
 * @brief Progresses the Bus initialisation process.
 *
 * Advances the Bus initialisation sequence started by @ref Bus::beginBusInitProcess. This function should be called repeatedly until 
 * the process completes or an error occurs. During initialisation, the function validates that the Memory Map of each physical Node 
 * matches the internal Node instance, and configures the IDs required for Atams synchronous communication (first, last, and previous
 * Node IDs) in each Node's Universal Data Block.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::PROCESS_ERROR, this will be
 *              set to the error that caused the failure; otherwise, the value can be ignored.
 *
 * @return The current @c Atams::ProcessState_t of the initialisation process:
 *         - @c PROCESS_IN_PROGRESS: Initialisation is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Initialisation has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The Bus initialisation process must be completed before running any Bus update cycles.
 */
Atams::ProcessState_t Bus::updateBusInitProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::InitState> &process         {initProcessHandler_};
  Atams::ProcessState_t               &processState    {process.processState};
  Atams::ProcessState_t               &subProcessState {process.subProcessState};
  Bus::InitState                      &initState       {process.specificState};
  Atams::Error_t                       cycleError      {Atams::ERROR_NONE};
  bool                                 dataIsValid     {false}; 
  Atams::Node                        *&initNodePtr     {process.activeNodePtr};

  if (initNodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_INIT_ORDER);
    error = process.error;
    return (processState); /* Early Return */
  }

  if (processState != Atams::PROCESS_IN_PROGRESS)
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  if ((initState != Bus::InitState::START                                            ) &&
      (runSingleNodeUpdateCycle(cycleError, *initNodePtr) == Atams::PROCESS_IN_PROGRESS) )
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  Atams::Node &node {*initNodePtr};

  switch (initState)
  {
    case Bus::InitState::START:
      beginInitValidateAtamsVersion(node);
      break;
    case Bus::InitState::VALIDATE_ATAMS_VERSION:
      subProcessState = nodeProcessHandler_.updateValidateAtamsVersion(process.error, dataIsValid);
      if      (dataIsValid     == true)                    beginInitValidateUniversalBlock(node);
      else if (subProcessState == Atams::PROCESS_COMPLETE) process.terminate(Atams::ERROR_ATAMS_VERSION_MISMATCH);
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::VALIDATE_UNIVERSAL_BLOCK:
      subProcessState = nodeProcessHandler_.updateReadUniversalBlock(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) validateNodeUniversalBlock(node);
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::SET_BUS_IDS:
      subProcessState = nodeProcessHandler_.updateSetBusIDs(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) beginInitStore(node);
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::STORE_BUS_IDS:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) beginInitValidateIDsPost(node, busIDsToSet_);
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::VALIDATE_IDS_POST:
      subProcessState = nodeProcessHandler_.updateValidateBusIDs(process.error, dataIsValid);
      if      (dataIsValid     == true)                    startNextNodeInit();
      else if (subProcessState == Atams::PROCESS_COMPLETE) process.terminate(Atams::ERROR_GEN_INFO_MISMATCH);
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::InitState::COMPLETE:
    case Bus::InitState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  /* Only start a new Bus cycle if this step actually queued a variable request; several transitions above (e.g.
     moving on to the next group of a multi-var read, or a sub-process's own bookkeeping-only steps) merely set up
     the next state without queuing anything, so a cycle here would just send a header-only request. Skipping it
     costs nothing: with no cycle in progress, the guard above will let the next call straight through to run the
     next switch case. */
  if ((process.error == Atams::ERROR_NONE           ) &&
      (processState  == Atams::PROCESS_IN_PROGRESS  ) &&
      (nodeHasQueuedRequestData(*initNodePtr)        ) )
  {
    static_cast<void>(beginSingleNodeUpdateCyclePrivate(*initNodePtr));
  }

  error = process.error;

  return (processState);
}

/**
 * @brief Begins an update cycle for all Nodes on the Bus.
 *
 * Starts the process of sending request packets to all registered Nodes and collecting their responses. This function is used to
 * initiate both synchronous and asynchronous update cycles. The Bus must be fully initialised before calling this function; otherwise,
 * the update cycle will not start.
 *
 * After calling this function, the update cycle is progressed through repeated calls to either @ref Bus::runUpdateCycleSync or
 * @ref Bus::runUpdateCycleAsync, depending on the desired update mode.
 *
 * @retval @c ERROR_NONE                     Update cycle started successfully.
 * @retval @c ERROR_INIT_ORDER               The Bus has not been initialised; call @ref Bus::beginBusInitProcess and complete the 
 *                                           initialisation before starting an update cycle.
 * @retval @c ERROR_UPDATE_CYCLE_IN_PROGRESS An update cycle is already in progress.
 */
Atams::Error_t Bus::beginUpdateCycle(void)
{
  if (initProcessHandler_.processState != Atams::PROCESS_COMPLETE)
  {
    return (Atams::ERROR_INIT_ORDER); /* Early Return */
  }

  return (beginUpdateCyclePrivate());
} 

/**
 * @brief Begins an update cycle for a single Node on the Bus.
 *
 * Starts the process of sending request packets to the specified Node and collecting its responses.
 * The Bus must be fully initialised before calling this function; otherwise, the update cycle will not start.
 *
 * After calling this function, the update cycle is progressed through repeated calls to @ref Bus::runSingleNodeUpdateCycle.
 *
 * @param node Reference to the @c Node instance to update.
 *
 * @retval @c ERROR_NONE       Update cycle started successfully.
 * @retval @c ERROR_INIT_ORDER The Bus has not been initialised; call @ref Bus::beginBusInitProcess and complete the initialisation before starting an update cycle.
 * @retval @c ERROR_UPDATE_CYCLE_IN_PROGRESS An update cycle is already in progress.
 */
Atams::Error_t Bus::beginSingleNodeUpdateCycle(Atams::Node &node)
{
  if (initProcessHandler_.processState != Atams::PROCESS_COMPLETE)
  {
    return (Atams::ERROR_INIT_ORDER); /* Early Return */
  }

  return (beginSingleNodeUpdateCyclePrivate(node));
}

/**
 * @brief Progresses the Synchronous Update Cycle for all Nodes on the Bus (non-blocking).
 *
 * Advances the update cycle started by @ref Bus::beginUpdateCycle in synchronous mode. This function should be called repeatedly
 * until the update cycle completes or an error occurs. Each call advances the internal state machine by a single step, such as
 * sending a request, waiting for a response, or jogging a Node if a timeout occurs.
 *
 * During the Synchronous Update Cycle, Request Packets are sent to all Node devices, which store them in a Sync Buffer. Once the
 * final Node device receives it's Request Packet, all Nodes process their stored Request Packets simultaneously: variables are
 * written to variable storage, and read data and write acknowledgments are transferred to Response Packets. The first Node
 * transmits it's Response Packet immediately and each subsequent Node device transmits its response after receiving the previous
 * Node's Response Packet. If a Node does not respond before a timeout, the Hub's Bus Update Cycle detects the timeout and sends
 * a Jog Packet to the next Node device in the response order, ensuring the Update Cycle continues.
 *
 * This function does not process incoming response data during the update. To process all received response packets at a time 
 * of your choosing, call @ref Bus::processSyncBuffers after the update cycle completes.
 *
 * This function operates in non-blocking (polling) mode. To block the calling thread while waiting for packet transmissions
 * and responses, use @ref Bus::runUpdateCycleSyncBlocking instead.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::ProcessState::ERROR, this will be
 *              set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 *
 * @return The current @c Atams::ProcessState of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runUpdateCycleSync(Atams::Error_t &error)
{
  return (runUpdateCycleSyncCore(error, false));
}

/**
 * @brief Progresses the Synchronous Update Cycle for all Nodes on the Bus (blocking).
 *
 * Blocking variant of @ref Bus::runUpdateCycleSync. During transmit phases the calling thread is blocked
 * inside @c pollForRequestTransmit via @c Platform::BinarySemaphore::waitWithTimeout on a transmit semaphore
 * released by @c txCallback when each packet transmission completes. Any transmit failure — whether
 * @c Platform::BusPeripheral::transmit returns @c false or @c txCallback is not called within
 * @c Platform::BUS_TRANSMIT_TIMEOUT milliseconds — terminates the cycle with @c Atams::ERROR_PLATFORM,
 * as both indicate the platform peripheral is in a broken state. During receive phases the thread is blocked
 * inside @c pollForResponse, which waits on a receive semaphore released by @c rxCallback whenever bytes
 * arrive. The semaphore is re-waited with the remaining timeout budget after each partial delivery, so the
 * overall timeout is measured from when the request was sent rather than per-byte-chunk received.
 *
 * All other behaviour is identical to @ref Bus::runUpdateCycleSync.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::ProcessState::ERROR, this will be
 *              set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 *
 * @return The current @c Atams::ProcessState of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runUpdateCycleSyncBlocking(Atams::Error_t &error)
{
  return (runUpdateCycleSyncCore(error, true));
}

/**
 * @brief Progresses the asynchronous update cycle for all Nodes on the Bus (non-blocking).
 *
 * Advances the update cycle started by @ref Bus::beginUpdateCycle in asynchronous mode. This function should be called repeatedly
 * until the update cycle completes or an error occurs. Each call advances the internal state machine by a single step, such as
 * sending a request, waiting for a response with a timeout, or processing a response once received.
 *
 * In the asynchronous update cycle, each Node responds immediately to its request packet with a response packet. Node transactions
 * are handled one after another: the Bus sends a request to a Node, waits for and processes its response, then proceeds to the next Node.
 *
 * This function operates in non-blocking (polling) mode. To block the calling thread while waiting for packet transmissions
 * and responses, use @ref Bus::runUpdateCycleAsyncBlocking instead.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::ProcessState::ERROR, this will be
 *              set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 *
 * @return The current @c Atams::ProcessState of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runUpdateCycleAsync(Atams::Error_t &error)
{
  return (runUpdateCycleAsyncCore(error, false));
}

/**
 * @brief Progresses the asynchronous update cycle for all Nodes on the Bus (blocking).
 *
 * Blocking variant of @ref Bus::runUpdateCycleAsync. The calling thread is blocked inside
 * @c pollForResponse, which waits on a receive semaphore released by @c rxCallback whenever bytes arrive.
 * The semaphore is re-waited with the remaining timeout budget after each partial delivery, so the overall
 * timeout is measured from when the request was sent rather than per-byte-chunk received.
 *
 * All other behaviour is identical to @ref Bus::runUpdateCycleAsync.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::ProcessState::ERROR, this will be
 *              set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 *
 * @return The current @c Atams::ProcessState of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runUpdateCycleAsyncBlocking(Atams::Error_t &error)
{
  return (runUpdateCycleAsyncCore(error, true));
}

/**
 * @brief Progresses the update cycle for a single Node on the Bus (non-blocking).
 *
 * Advances the update cycle started by @ref Bus::beginSingleNodeUpdateCycle. This function should be called repeatedly
 * until the update cycle completes or an error occurs. Each call advances the internal state machine by a single step,
 * such as sending a request, waiting for a response with a timeout, or processing a received response.
 *
 * For single-node updates, the Bus sends a request packet to the specified Node, waits for and processes its response,
 * and then completes the cycle.
 *
 * This function operates in non-blocking (polling) mode. To block the calling thread while waiting for packet transmissions
 * and responses, use @ref Bus::runSingleNodeUpdateCycleBlocking instead.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::ProcessState::ERROR,
 *              this will be set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 * @param node  Reference to the @c Node instance being updated. This must be the same @c Node passed to
 *              @ref Bus::beginSingleNodeUpdateCycle.
 *
 * @return The current @c Atams::ProcessState_t of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginSingleNodeUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runSingleNodeUpdateCycle(Atams::Error_t &error, Atams::Node &node)
{
  return (runSingleNodeUpdateCycleCore(error, node, false));
}

/**
 * @brief Progresses the update cycle for a single Node on the Bus (blocking).
 *
 * Blocking variant of @ref Bus::runSingleNodeUpdateCycle. The calling thread is blocked inside
 * @c pollForResponse, which waits on a receive semaphore released by @c rxCallback whenever bytes arrive.
 * The semaphore is re-waited with the remaining timeout budget after each partial delivery, so the overall
 * timeout is measured from when the request was sent rather than per-byte-chunk received.
 *
 * All other behaviour is identical to @ref Bus::runSingleNodeUpdateCycle.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::PROCESS_ERROR,
 *              this will be set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 * @param node  Reference to the @c Node instance being updated. This must be the same @c Node passed to
 *              @ref Bus::beginSingleNodeUpdateCycle.
 *
 * @return The current @c Atams::ProcessState_t of the update cycle:
 *         - @c PROCESS_IN_PROGRESS: Update is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Update has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The update cycle must be started by calling @ref Bus::beginSingleNodeUpdateCycle before calling this function.
 */
Atams::ProcessState_t Bus::runSingleNodeUpdateCycleBlocking(Atams::Error_t &error, Atams::Node &node)
{
  return (runSingleNodeUpdateCycleCore(error, node, true));
}

/**
 * @brief Processes all response packets received during the last update cycle.
 *
 * This function should be called after a Bus Update Cycle is complete (run with @ref Bus::runUpdateCycleSync, 
 * or @ref Bus::runUpdateCycleAsync). It processes the Response Buffers for all registered Nodes by calling 
 * @ref Node::processResponseBuffer on each Node, moving received data into variable storage and updating 
 * write acknowledgement and new data ready flags.
 *
 * @return An @c Atams::Error_t code:
 *         - @c ERROR_NONE if all response buffers were processed successfully and no bus errors occurred.
 *         - @c ERROR_UPDATE_CYCLE_IN_PROGRESS if the synchronous update cycle is still in progress (not related to a Node).
 *         - Otherwise, the first error code encountered while processing a Node's response buffer (other Nodes may also have errors).
 *
 * @note This function is only required for synchronous update cycles. Asynchronous and single-node update cycles process
 *       response data as it is received, so this function does not need to be called in those cases.
 *
 * @attention If this function returns an error other than @c Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS, it corresponds to the first Node 
 *            on the Bus that experienced a bus error during the update cycle. Other Nodes may also have experienced errors. To check 
 *            for errors on individual Nodes, call @ref Node::getBusError on each Node after calling this function.
 */
Atams::Error_t Bus::processResponseBuffers(void)
{
  if (updateProcessHandler_.processState == Atams::PROCESS_IN_PROGRESS) 
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  Atams::Error_t firstError {Atams::ERROR_NONE};

  for (Atams::Node *&nodePtr : nodePtrs_)
  {
    if (nodePtr != nullptr) 
    {
      Atams::Node         &node          {*nodePtr};     
      NodeCallbackHandler &nodeCallbacks {node};

      nodeCallbacks.processResponseBuffer();
      
      if (!firstError) firstError = node.getBusError();
    }
  }

  return (firstError);
}

/**
 * @brief Begins the Node configuration process for a single device.
 *
 * Prepares the Bus to adjust configuration values in the Universal Data Block of a Node device, including Node ID, communications
 * bitrate, and watchdog period. This function only sets internal states and begins reception on the bus peripheral; the configuration
 * process itself is progressed through repeated calls to @ref Bus::updateSetNodeConfigProcess.
 *
 * The user must provide the current Node ID and the new Node ID to set, along with other configuration parameters, in the 
 * @c NodeConfig_t structure. Atams Nodes default to a Node ID of 0 if configuration has never been set, allowing new Nodes to be 
 * added to the bus one at a time and reconfigured before adding the next Node.
 *
 * @param userConfig Structure containing the current Node ID, desired Node ID, and other configuration parameters.
 *
 * @retval @c ERROR_NONE                     Configuration process started successfully.
 * @retval @c ERROR_UPDATE_CYCLE_IN_PROGRESS An update cycle is currently in progress; configuration cannot be started.
 * @retval @c ERROR_PLATFORM                 Failed to start data reception on the platform bus peripheral.
 */
Atams::Error_t Bus::beginSetNodeConfigProcess(const Atams::NodeConfig_t &userConfig)
{
  if (updateProcessHandler_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); 
  }

  if (BusPeripheral::startReceive() == false) 
  {
    return (Atams::ERROR_PLATFORM); /* Early Return */
  }

  userConfigToSet_ = userConfig;
  dummyNode_.setNodeID(userConfig.currentNodeID);
  configUpdateProcessHandler_.readyProcess();
  configUpdateProcessHandler_.specificState  = Bus::ConfigUpdateState::START;
  configUpdateProcessHandler_.activeNodePtr  = &dummyNode_;
  dummyNode_.clearAllRequestPatterns();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Progresses the Node configuration process for a single device.
 *
 * Advances the configuration sequence started by @ref Bus::beginSetNodeConfigProcess. This function should be called repeatedly
 * until the configuration process completes or an error occurs. Each call advances the internal state machine by a single step,
 * such as writing configuration data, storing it to non-volatile memory, or verifying the result.
 *
 * The configuration process adjusts values in the Universal Data Block of the Node, including Node ID, communications bitrate,
 * and watchdog period. The user must provide the current and desired Node IDs, as well as other parameters, in the @c NodeConfig_t structure.
 *
 * @param error Reference to an @c Atams::Error_t variable. If the process state becomes @c Atams::PROCESS_ERROR, this will be
 *              set to the error that caused the failure; otherwise, it will be set to @c Atams::ERROR_NONE.
 *
 * @return The current @c Atams::ProcessState_t of the configuration process:
 *         - @c PROCESS_IN_PROGRESS: Configuration is ongoing and this function should be called again.
 *         - @c PROCESS_COMPLETE:    Configuration has finished successfully.
 *         - @c PROCESS_ERROR:       An error has occurred; check @p error for details.
 *
 * @note The configuration process must be started by calling @ref Bus::beginSetNodeConfigProcess before calling this function.
 *       New Nodes default to a Node ID of 0 if configuration has never been set, allowing them to be added and reconfigured one at a time.
 */
Atams::ProcessState_t Bus::updateSetNodeConfigProcess(Atams::Error_t &error)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           {configUpdateProcessHandler_};
  Atams::ProcessState_t                       &processState      {process.processState};
  Atams::ProcessState_t                       &subProcessState   {process.subProcessState};
  Bus::ConfigUpdateState                      &configUpdateState {process.specificState};
  Atams::Error_t                               cycleError        {Atams::ERROR_NONE};

  if (processState != Atams::PROCESS_IN_PROGRESS)
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  if ((configUpdateState != Bus::ConfigUpdateState::START                             ) &&
      (runSingleNodeUpdateCycle(cycleError, dummyNode_) == Atams::PROCESS_IN_PROGRESS) )
  {
    error = process.error;
    return (processState); /* Early Return */
  }

  switch (configUpdateState)
  {
    case Bus::ConfigUpdateState::START:
      process.error = dummyNode_.init(s_dummyMemoryMap);
      if (process.error) process.terminate(process.error);
      else               beginSetConfigWrite();
      break;
    case Bus::ConfigUpdateState::WRITE_CONFIG:
      subProcessState = nodeProcessHandler_.updateSetNodeConfig(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) beginSetConfigStore();
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case Bus::ConfigUpdateState::STORE_CONFIG:
      subProcessState = nodeProcessHandler_.updateStoreAll(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
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

  /* See the equivalent guard in updateBusInitProcess: only start a new Bus cycle if this step actually queued a
     variable request. Several of these states (e.g. START, or ENTER_CONFIG's own internal START step) only set up
     the next state without queuing anything - skipping the cycle there costs nothing, since the guard above lets
     the next call straight through to run the next switch case. */
  if ((process.error == Atams::ERROR_NONE         ) &&
      (processState  == Atams::PROCESS_IN_PROGRESS) &&
      (nodeHasQueuedRequestData(dummyNode_)        ) )
  {
    static_cast<void>(beginSingleNodeUpdateCyclePrivate(dummyNode_));
  }

  error = process.error;

  return (process.processState);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::ProcessState_t Bus::runUpdateCycleSyncCore(Atams::Error_t &error, bool blocking)
{
  Bus::ProcessHandler<Bus::UpdateState> &process       {updateProcessHandler_};
  Atams::ProcessState_t                 &processState  {process.processState};
  Bus::UpdateState                      &updateState   {process.specificState};
  Atams::Node                          *&activeNodePtr {process.activeNodePtr};
  Bus::PollResult                        rxPollResult  {Bus::PollResult::WAITING};

  activeNodePtr = getUpdateNodePtr();

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  Platform::BusPeripheral::update();

  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST_SYNCED, blocking) == true)
      {
        if ((process.error            == Atams::ERROR_NONE) &&
            (tryNodeIncrementUpdate() == false            ) ) 
        {
          startResponseCollectionSync();
        }
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      rxPollResult = pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE_SYNCED, blocking);

      if (rxPollResult != Bus::PollResult::WAITING)
      {
        if      (tryNodeIncrementUpdate() == false                   ) process.setProcessComplete();
        else if (rxPollResult             == Bus::PollResult::TIMEOUT) triggerJogSync();
      }
      break;
    case Bus::UpdateState::JOG_NODE:
      if (pollForJogTransmit(*activeNodePtr, process, blocking) == true)
      {
        if (!process.error) updateState = Bus::UpdateState::COLLECT_RESPONSES;
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

  if (processState == Atams::PROCESS_ERROR)
  {
    error = process.error;
  }

  return (processState);
}

Atams::ProcessState_t Bus::runUpdateCycleAsyncCore(Atams::Error_t &error, bool blocking)
{
  Bus::ProcessHandler<Bus::UpdateState> &process       {updateProcessHandler_};
  Atams::ProcessState_t                 &processState  {process.processState};
  Bus::UpdateState                      &updateState   {process.specificState};
  Atams::Node                          *&activeNodePtr {process.activeNodePtr};
  Bus::PollResult                        rxPollResult  {Bus::PollResult::WAITING};

  activeNodePtr = getUpdateNodePtr();

  if (activeNodePtr == nullptr) process.terminate(Atams::ERROR_NULLPTR);

  Platform::BusPeripheral::update();

  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(*activeNodePtr, process, Atams::MESSAGE_REQUEST, blocking))
      {
        if (!process.error) updateState = Bus::UpdateState::COLLECT_RESPONSES;
      }
      break;
    case Bus::UpdateState::COLLECT_RESPONSES:
      rxPollResult = pollForResponse(*activeNodePtr, process, Atams::MESSAGE_RESPONSE, blocking);

      if (rxPollResult != Bus::PollResult::WAITING) triggerNextRequestAsync();
      break;
    case Bus::UpdateState::COMPLETE:
    case Bus::UpdateState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  if (processState == Atams::PROCESS_ERROR) error = process.error;

  return (processState);
}

Atams::ProcessState_t Bus::runSingleNodeUpdateCycleCore(Atams::Error_t &error, Atams::Node &node, bool blocking)
{
  Bus::ProcessHandler<Bus::UpdateState> &process             {singleNodeUpdateProcessHandler_};
  Atams::ProcessState_t                 &processState        {process.processState};
  Bus::UpdateState                      &updateState         {process.specificState};
  NodeCallbackHandler                   &nodeCallbackHandler {node};
  Bus::PollResult                        rxPollResult        {Bus::PollResult::WAITING};

  Platform::BusPeripheral::update();

  switch (updateState)
  {
    case Bus::UpdateState::SEND_REQUESTS:
      if (pollForRequestTransmit(node, process, Atams::MESSAGE_REQUEST, blocking))
      {
        if (process.error) process.terminate(process.error);
        else               updateState = Bus::UpdateState::COLLECT_RESPONSES;
      }
      break;

    case Bus::UpdateState::COLLECT_RESPONSES:
      rxPollResult = pollForResponse(node, process, Atams::MESSAGE_RESPONSE, blocking);

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

  if (processState == Atams::PROCESS_ERROR) error = process.error;

  return (processState);
}

void Bus::validateNodeUniversalBlock(Node &node)
{
  Bus::ProcessHandler<Bus::InitState> &process {initProcessHandler_};

  uint16_t             maxPacketSize {0U};
  uint8_t              firstNodeID   {0U};
  uint8_t              lastNodeID    {0U};
  uint8_t              prevNodeID    {0U};
  NodeCallbackHandler &nodeCallbacks {node};

  if (node.validateGenInfo() != Atams::ERROR_NONE)
  {
    process.terminate(Atams::ERROR_MEMORY_MAP);
  }
  else if ((node.getVar(BlockUniversal::VAR_MAX_BUS_PACKET_SIZE,  maxPacketSize) != Atams::ERROR_NONE) ||  
           (node.getVar(BlockUniversal::VAR_FIRST_NODE_ID,        firstNodeID)   != Atams::ERROR_NONE) || 
           (node.getVar(BlockUniversal::VAR_LAST_NODE_ID,         lastNodeID)    != Atams::ERROR_NONE) || 
           (node.getVar(BlockUniversal::VAR_PREVIOUS_NODE_ID,     prevNodeID)    != Atams::ERROR_NONE) )
  {
    process.terminate(Atams::ERROR_MEMORY_MAP);
  } 
  else
  {
    nodeCallbacks.setNodeMaxPacketSize(maxPacketSize);

    if ((firstNodeID == busIDsToSet_.firstNodeID   ) &&
        (lastNodeID  == busIDsToSet_.lastNodeID    ) &&
        (prevNodeID  == busIDsToSet_.previousNodeID))
    {
      startNextNodeInit();
    }
    else 
    {
      beginInitSetBusIDs(node, busIDsToSet_);
    }
  }
}

bool Bus::safeToRemoveNode(void) 
{
    return ((updateProcessHandler_.processState           != Atams::PROCESS_IN_PROGRESS) &&
            (singleNodeUpdateProcessHandler_.processState != Atams::PROCESS_IN_PROGRESS) &&
            (initProcessHandler_.processState             != Atams::PROCESS_IN_PROGRESS) &&
            (configUpdateProcessHandler_.processState     != Atams::PROCESS_IN_PROGRESS) );
}

bool Bus::findNodeOnBus(Node &node)
{
  bool nodeFound {false};

  for (Node *&nodePtr : nodePtrs_)
  {    
    if (nodePtr == &node) nodeFound = true;
  }

  return (nodeFound);
}

Atams::Error_t Bus::beginUpdateCyclePrivate(void)
{
  if ((updateProcessHandler_.processState           == Atams::PROCESS_IN_PROGRESS) ||
      (singleNodeUpdateProcessHandler_.processState == Atams::PROCESS_IN_PROGRESS) )
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  Atams::Error_t error {Atams::ERROR_NONE};

  for (Node *&nodePtr : nodePtrs_)
  {
    if ((nodePtr != nullptr          ) &&
        (error   == Atams::ERROR_NONE) )
    {
      error = static_cast<NodeCallbackHandler&>(*nodePtr).updateRequestPacketWriteData();
    }
  }

  if (error == Atams::ERROR_NONE)
  {
    clearAllBusErrors();
    circularBuffer_.reset();
    txReady_ = true;
    txSemaphore_.waitWithTimeout(0U);
    updateNodeIndex_ = 0U;
    activeSyncCount_++;
    updateProcessHandler_.readyProcess();
    updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;
  }

  return (error);
}

Atams::Error_t Bus::beginSingleNodeUpdateCyclePrivate(Atams::Node &node)
{
  if ((updateProcessHandler_.processState           == Atams::PROCESS_IN_PROGRESS) ||
      (singleNodeUpdateProcessHandler_.processState == Atams::PROCESS_IN_PROGRESS) )
  {
    return (Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS); /* Early Return */
  }

  Atams::Error_t error {Atams::ERROR_NONE};
  
  error = static_cast<NodeCallbackHandler&>(node).updateRequestPacketWriteData();

  if (error == Atams::ERROR_NONE)
  {
    static_cast<NodeCallbackHandler&>(node).clearBusError();
    static_cast<NodeCallbackHandler&>(node).clearAbortDetails();
    circularBuffer_.reset();
    txReady_ = true;
    txSemaphore_.waitWithTimeout(0U);
    activeSyncCount_++;
    singleNodeUpdateProcessHandler_.activeNodePtr = &node;
    singleNodeUpdateProcessHandler_.readyProcess();
    singleNodeUpdateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;
  }

  return (error);
}

bool Bus::nodeHasQueuedRequestData(Atams::Node &node)
{
  return (node.getRequestPacketLength() > Atams::HEADER_SIZE_HEADER);
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
                                 const Atams::MessageType_t  requestType,
                                 const bool                  blocking)
{
  if (!awaitTxReady(process, blocking)) return (false); /* Early Return */
  if (process.error)                    return (true);  /* Early Return */

  Atams::NodeCallbackHandler &nodeCallbacks {node};
  uint16_t                    rawLength     {0U};

  nodeCallbacks.getRequestPacket(requestType,
                                 activeSyncCount_,
                                 rawTxBuffer_,
                                 rawLength);

  if (encodeBusPacket(rawTxBuffer_,
                      rawLength,
                      encodedBuffer_,
                      sizeof(encodedBuffer_),
                      encodedLength_) != Atams::ERROR_NONE)
  {
    process.terminate(Atams::ERROR_ENCODE);
    return (true); /* Early Return */
  }

  doTransmit(process, requestType);

  return (true);
}

bool Bus::pollForJogTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process, const bool blocking)
{
  if (!awaitTxReady(process, blocking)) return (false); /* Early Return */
  if (process.error)                    return (true);  /* Early Return */

  jogBuffer_[HEADER_INDEX_NODE_ID]  = node.getNodeID();
  jogBuffer_[HEADER_INDEX_MSG_TYPE] = Atams::MESSAGE_SYNC_JOG;
  jogBuffer_[HEADER_INDEX_SYNC]     = activeSyncCount_;

  if (encodeBusPacket(jogBuffer_, HEADER_SIZE_HEADER,
                      encodedBuffer_, sizeof(encodedBuffer_),
                      encodedLength_) != Atams::ERROR_NONE)
  {
    process.terminate(Atams::ERROR_ENCODE);
    return (true);
  }

  doTransmit(process, Atams::MESSAGE_SYNC_JOG);

  return (true);
}

bool Bus::awaitTxReady(Bus::ProcessHandlerBase &process, const bool blocking)
{
  if (txReady_) return (true); /* Early Return */

  uint32_t elapsedTime {Platform::getMillis() - process.prevEventTime};

  if (elapsedTime >= Platform::BUS_TRANSMIT_TIMEOUT) 
  { 
    process.terminate(Atams::ERROR_PLATFORM);   
  }
  else if (blocking)
  {
    txSemaphore_.waitWithTimeout(Platform::BUS_TRANSMIT_TIMEOUT - elapsedTime);
    
    if (!txReady_)
    {
      process.terminate(Atams::ERROR_PLATFORM);
    }
  }
  else
  {
    return (false); /* Early Return */
  }

  return (true);
}

void Bus::doTransmit(Bus::ProcessHandlerBase &process, const Atams::MessageType_t messageType)
{
  txReady_ = false;

  if (Platform::BusPeripheral::transmit(encodedBuffer_, encodedLength_) == false)
  {
    process.terminate(Atams::ERROR_PLATFORM);
  }
  else
  {
    lastSentMessageType_ = messageType;
  }

  process.prevEventTime = Platform::getMillis();
}

Bus::PollResult Bus::pollForResponse(Atams::Node               &node,
                                     Bus::ProcessHandlerBase   &process,
                                     const Atams::MessageType_t expectedResponse,
                                     const bool                 blocking)
{
  Atams::NodeCallbackHandler &nodeCallbacks {node};
  Bus::PollResult             result        {Bus::PollResult::WAITING};

  do
  {
    uint32_t currentTime {Platform::getMillis()};
    uint32_t elapsedTime {currentTime - process.prevEventTime};

    if ((circularBuffer_.getPacket(rxBuffer_, sizeof(rxBuffer_), rxLength_) == CircularBuffer::ERROR_NONE) &&
        (validateAndStoreResponsePacket(node, expectedResponse)             == true                      ) )
    {
      result = Bus::PollResult::READY;
      process.prevEventTime = currentTime;
      break;
    }

    if (elapsedTime >= Platform::BUS_RESPONSE_TIMEOUT)
    {
      nodeCallbacks.reportBusError(Atams::ERROR_RESPONSE_TIMEOUT);
      result = Bus::PollResult::TIMEOUT;
      process.prevEventTime = currentTime;
      break;
    }

    if (blocking)
    {
      uint32_t remainingTime {Platform::BUS_RESPONSE_TIMEOUT - elapsedTime};
      rxSemaphore_.waitWithTimeout(remainingTime);
    }

  } while (blocking);

  return (result);
}

void Bus::rxCallback(uint8_t *rxBufferPtr, const uint16_t rxBufferLength)
{
  if (circularBuffer_.pushHead(rxBufferPtr, rxBufferLength) == CircularBuffer::ERROR_FULL)
  {
    circularBuffer_.reset();
  }
  rxSemaphore_.release();
}

void Bus::txCallback(void)
{
  txReady_ = true;
  txSemaphore_.release();
}

bool Bus::validateAndStoreResponsePacket(Atams::Node &node, const MessageType_t expectedResponse)
{
  Atams::NodeCallbackHandler &nodeCallbacks {node};
  Atams::Error_t              error         {Atams::ERROR_NONE};
  bool                        packetValid   {true};

  error = decodeBusPacket(rxBuffer_, 
                          rxLength_, 
                          decodedBuffer_, 
                          sizeof(decodedBuffer_), 
                          decodedLength_);

  if (error == Atams::ERROR_NONE)
  {
    uint8_t              packetNodeID    {decodedBuffer_[HEADER_INDEX_NODE_ID]};
    uint8_t              packetSyncCount {decodedBuffer_[HEADER_INDEX_SYNC]};
    Atams::MessageType_t messageType     {static_cast<MessageType_t>(decodedBuffer_[HEADER_INDEX_MSG_TYPE])};

    bool messageIsAbort {((messageType == Atams::MESSAGE_ABORT_RESPONSE       ) ||
                          (messageType == Atams::MESSAGE_ABORT_RESPONSE_SYNCED) )};

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
  Atams::Node *nodePtr {nullptr};

  if ((updateNodeIndex_ < Platform::NUMBER_OF_NODES_PER_BUS) &&
      (updateNodeIndex_ < noOfNodesOnBus_                  ) ) 
  {
    nodePtr = nodePtrs_[updateNodeIndex_];
  }
  
  return (nodePtr);
}

bool Bus::tryNodeIncrementUpdate(void)
{
  bool incrementValid {false};

  if ((updateNodeIndex_ + 1U) < noOfNodesOnBus_)
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
    circularBuffer_.reset();
    updateProcessHandler_.specificState = Bus::UpdateState::SEND_REQUESTS;
  }
}

void Bus::beginInitValidateAtamsVersion(Atams::Node &node)
{
  initProcessHandler_.specificState = Bus::InitState::VALIDATE_ATAMS_VERSION;
  nodeProcessHandler_.beginValidateAtamsVersion(node);
}

void Bus::beginInitValidateUniversalBlock(Atams::Node &node)
{
  initProcessHandler_.specificState = Bus::InitState::VALIDATE_UNIVERSAL_BLOCK;
  nodeProcessHandler_.beginReadUniversalBlock(node);
}

void Bus::beginInitValidateIDsPost(Atams::Node &node, const Atams::BusIDs_t busIDs)
{
  initProcessHandler_.specificState = Bus::InitState::VALIDATE_IDS_POST;
  nodeProcessHandler_.beginValidateBusIDs(node, busIDs);
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
  Bus::ProcessHandler<Bus::InitState> &process {initProcessHandler_};

  if ((initNodeIndex_ + 1U) < noOfNodesOnBus_)
  {
    initNodeIndex_++;

    process.activeNodePtr = nodePtrs_[initNodeIndex_];

    Atams::Node *prevNodePtr  {nodePtrs_[initNodeIndex_ - 1U]};
    Atams::Node *firstNodePtr {nodePtrs_[0]};
    Atams::Node *lastNodePtr  {nodePtrs_[noOfNodesOnBus_ - 1U]};

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
      beginInitValidateAtamsVersion(*process.activeNodePtr);
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
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           {configUpdateProcessHandler_};
  Bus::ConfigUpdateState                      &configUpdateState {process.specificState};

  if (pollForRequestTransmit(dummyNode_, process, MESSAGE_REQUEST, false) == true)
  {
    if (!process.error)
    {
      static_cast<NodeCallbackHandler&>(dummyNode_).clearBusError();
      configUpdateState = Bus::ConfigUpdateState::GET_RESPONSE;
    }
  }
}

void Bus::updateSetConfigGetResponse(void)
{
  Bus::ProcessHandler<Bus::ConfigUpdateState> &process           {configUpdateProcessHandler_};
  Bus::ConfigUpdateState                      &configUpdateState {process.specificState};

  Platform::BusPeripheral::update();

  if (pollForResponse(dummyNode_, process, MESSAGE_RESPONSE, false) != Bus::PollResult::WAITING)
  {
    dummyNodeCallbackHandler_.processResponseBuffer();
    configUpdateState = process.nextSpecificState;  
  }
}

/*************************************************************************************/
/* PRIVATE STATIC CONSTANTS                                                          */
/*************************************************************************************/

const Atams::GenInfo_t Bus::s_dummyGenInfo
{
  /* .atamsVersionMajor  = */ ATAMS_VERSION_MAJOR,
  /* .atamsVersionMinor  = */ ATAMS_VERSION_MINOR,
  /* .genDay             = */ 0U,
  /* .genMonth           = */ 0U,
  /* .genYear            = */ 0U,
  /* .genHour            = */ 0U,
  /* .genMinute          = */ 0U,
  /* .genSecond          = */ 0U,
  /* .genChecksum        = */ 0U,
  /* .noOfVars           = */ BlockUniversal::NUMBER_OF_VARS,
};

const Node::MemoryMap_t Bus::s_dummyMemoryMap 
{
  /* .sharedMemoryMap = */
  {
    /* .genInfo     = */ Bus::s_dummyGenInfo,
    /* .varInfoList = */ BlockUniversal::varInfoList
  }
};

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

void Bus::ProcessHandlerBase::terminate(Atams::Error_t exitError)
{
  this->error        = exitError;
  this->processState = Atams::PROCESS_ERROR;

  if (this->activeNodePtr != nullptr)
  {
    static_cast<NodeCallbackHandler*>(this->activeNodePtr)->reportBusError(exitError);
  }
}

template <typename T>
void Bus::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  ProcessHandlerBase::terminate(exitError);
  this->specificState = T::ERROR;
}

template <typename T>
void Bus::ProcessHandler<T>::setProcessComplete(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState  = Atams::PROCESS_COMPLETE;
}

template <typename T>
void Bus::ProcessHandler<T>::readyProcess(void)
{
  this->error           = Atams::ERROR_NONE;
  this->processState    = Atams::PROCESS_IN_PROGRESS;
  this->subProcessState = Atams::PROCESS_IN_PROGRESS;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
