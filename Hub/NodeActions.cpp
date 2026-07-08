/**
  ******************************************************************************
  * @file    NodeActions.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the NodeActions multi-step state mahcine processes.
  *
  * @details Implements the state machines in NodeActions that drive Hub-to-Node
  *          multi-step processes. All processes use an internal ProcessHandler
  *          template to manage state, errors, and the Node pointer. Each public update
  *          function advances its corresponding state machine by one step; the caller
  *          is expected to poll it each update cycle until Atams::PROCESS_COMPLETE or
  *          Atams::PROCESS_ERROR is returned.
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

#include "NodeActions.hpp"

#include "Node.hpp"
#include "../Shared/Maps/BlockUniversal.hpp"
#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

void NodeActions::beginConfigStateEntry(Node &node)
{
  if (configEntryProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  configEntryProcess_.resetAndAssignNode(node);
}

void NodeActions::beginConfigStateExit(Node &node, bool applyChangesOnExit, std::optional<uint8_t> newNodeID)
{
  if (configExitProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  applyChanges_     = applyChangesOnExit;
  configExitNodeID_ = newNodeID;
  configExitProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetNodeID(Node &node, const uint8_t nodeID)
{
  if (setConfigVarProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  nodeIDToSet_      = nodeID;
  configExitNodeID_ = nodeID;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetBitrate(Node &node, const Atams::BitrateOption_t bitrateOption)
{
  if (setConfigVarProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  bitrateOptionToSet_ = bitrateOption;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetWatchdogPeriod(Node &node, const uint32_t watchdogPeriod)
{
  if (setConfigVarProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  watchdogPeriodToSet_ = watchdogPeriod;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetNodeConfig(Node &node, const Atams::NodeConfig_t &userConfig)
{
  if (setMultiConfigProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  nodeConfigToSet_  = userConfig;
  configExitNodeID_ = userConfig.newNodeID;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::beginStorageProcess(Node &node)
{
  if (storageProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  storageProcess_.resetAndAssignNode(node);
}

void NodeActions::beginResetNode(Node &node)
{
  if (resetNodeProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  resetNodeProcess_.resetAndAssignNode(node);
}

void NodeActions::beginClearWatchdogFault(Node &node)
{
  if (clearWatchdogFaultProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  clearWatchdogFaultProcess_.resetAndAssignNode(node);
}

void NodeActions::beginValidateAtamsVersion(Node &node)
{
  if (busReadMultiVars_.process.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(busReadMultiVars_, node, BlockUniversal::VAR_ATAMS_VERSION_MAJOR, BlockUniversal::VAR_ATAMS_VERSION_PATCH);
}

void NodeActions::beginValidateBusIDs(Node &node, const Atams::BusIDs_t busIDs)
{
  if (busReadMultiVars_.process.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  busIDsToSet_ = busIDs;
  beginReadMultiVarsOrderedImpl(busReadMultiVars_, node, BlockUniversal::VAR_FIRST_NODE_ID, BlockUniversal::VAR_PREVIOUS_NODE_ID);
}

void NodeActions::beginReadUniversalBlock(Node &node)
{
  if (busReadMultiVars_.process.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(busReadMultiVars_,
                                node,
                                BlockUniversal::VAR_ATAMS_VERSION_MAJOR,
                                BlockUniversal::VAR_COBS_ERROR_COUNT);
}

void NodeActions::beginReadMultiVarsOrdered(Node &node, uint16_t firstVarID, uint16_t lastVarID)
{
  if (userReadMultiVars_.process.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(userReadMultiVars_, node, firstVarID, lastVarID);
}

void NodeActions::beginSetBusIDs(Node &node, const Atams::BusIDs_t busIDs)
{
  if (setMultiConfigProcess_.processState == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  busIDsToSet_      = busIDs;
  configExitNodeID_ = std::nullopt;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

Atams::ProcessState_t NodeActions::updateConfigStateEntry(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigEntryState> &process {configEntryProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node   &node        {*process.nodePtr};
  uint8_t configState {Atams::CONFIGURATION_STATUS_INACTIVE};

  switch (process.specificState)
  {
    case ConfigEntryState::START:
      process.specificState = ConfigEntryState::COLLECT_STATUS_PRE;
      break;
    case ConfigEntryState::COLLECT_STATUS_PRE:
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_PRE;
      break;
    case ConfigEntryState::CHECK_STATUS_PRE:
      process.error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (process.error)                                     process.terminate(process.error);
      else if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) process.setProcessComplete();
      else                                                        process.specificState = ConfigEntryState::WRITE_PASSCODE;
      break;
    case ConfigEntryState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY,
                                                Atams::CONFIGURATION_PASSKEY_ACCESS));
      process.specificState = ConfigEntryState::COLLECT_STATUS_POST;
      break;
    case ConfigEntryState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY));
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_POST;
      break;
    case ConfigEntryState::CHECK_STATUS_POST:
      process.error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (process.error)                                       process.terminate(process.error);
      else if (configState == Atams::CONFIGURATION_STATUS_ACTIVE)   process.setProcessComplete();
      else if (configState == Atams::CONFIGURATION_STATUS_DENIED)   process.terminate(Atams::ERROR_CONFIGURATION_STATE_DENIED);
      else                                                          process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      break;
    case ConfigEntryState::COMPLETE:
    case ConfigEntryState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateConfigStateExit(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigExitState> &process {configExitProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node    &node          {*process.nodePtr};
  uint8_t  configState   {Atams::CONFIGURATION_STATUS_ACTIVE};
  uint32_t configPasskey {applyChanges_ ? Atams::CONFIGURATION_PASSKEY_APPLY : Atams::CONFIGURATION_PASSKEY_CANCEL};

  switch (process.specificState)
  {
    case ConfigExitState::START:
      process.specificState = ConfigExitState::COLLECT_STATUS_PRE;
      break;
    case ConfigExitState::COLLECT_STATUS_PRE:
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_PRE;
      break;
    case ConfigExitState::CHECK_STATUS_PRE:
      process.error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (process.error)                                     process.terminate(process.error);
      else if (configState != Atams::CONFIGURATION_STATUS_ACTIVE) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      else                                                        process.specificState = ConfigExitState::WRITE_PASSCODE;
      break;
    case ConfigExitState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY, configPasskey));
      process.specificState = ConfigExitState::COLLECT_STATUS_POST;
      break;
    case ConfigExitState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY));
      if (configExitNodeID_.has_value()) node.setNodeID(configExitNodeID_.value());
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_POST;
      break;
    case ConfigExitState::CHECK_STATUS_POST:
      process.error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (process.error)                                            process.terminate(process.error);
      else if ((configState   == Atams::CONFIGURATION_STATUS_INACTIVE) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_CANCEL ) ) process.setProcessComplete();
      else if ((configState   == Atams::CONFIGURATION_STATUS_APPLIED) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_APPLY ) )  process.setProcessComplete();
      else                                                               process.terminate(Atams::ERROR_CONFIGURATION_EXIT);
      break;
    case ConfigExitState::COMPLETE:
    case ConfigExitState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateSetNodeID(Atams::Error_t &error)
{
  return (updateSetConfigVar<uint8_t>(error, BlockUniversal::VAR_NODE_ID, nodeIDToSet_));
}

Atams::ProcessState_t NodeActions::updateSetBitrate(Atams::Error_t &error)
{
  return (updateSetConfigVar<uint8_t>(error, BlockUniversal::VAR_BITRATE, static_cast<uint8_t>(bitrateOptionToSet_)));
}

Atams::ProcessState_t NodeActions::updateSetWatchdogPeriod(Atams::Error_t &error)
{
  return (updateSetConfigVar<uint32_t>(error, BlockUniversal::VAR_WATCHDOG_PERIOD, watchdogPeriodToSet_));
}

Atams::ProcessState_t NodeActions::updateSetNodeConfig(Atams::Error_t &error)
{
  return (updateSetMultipleConfig(error, setUserConfigFunctions_));
}

Atams::ProcessState_t NodeActions::updateStoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_STORE_ALL, Atams::STORE_ALL_PASSCODE));
}

Atams::ProcessState_t NodeActions::updateRestoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_RESTORE_ALL, Atams::RESTORE_ALL_PASSCODE));
}

Atams::ProcessState_t NodeActions::updateRestoreUserBlocks(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_RESTORE_USER_BLOCKS, Atams::RESTORE_USER_BLOCKS_PASSCODE));
}

Atams::ProcessState_t NodeActions::updateResetNode(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ResetNodeState> &process {resetNodeProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t &subProcessState {process.subProcessState};
  bool                   ackReceived     {false};

  switch (process.specificState)
  {
    case ResetNodeState::START:
      beginConfigStateEntry(node);
      process.specificState = ResetNodeState::ENTER_CONFIG;
      break;
    case ResetNodeState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = ResetNodeState::WRITE_PASSCODE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case ResetNodeState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_RESET_NODE, Atams::RESET_NODE_PASSCODE));
      process.specificState = ResetNodeState::CHECK_ACK;
      break;
    case ResetNodeState::CHECK_ACK:
      static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_RESET_NODE, ackReceived));
      if (!ackReceived) cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      else              process.setProcessComplete();
      break;
    case ResetNodeState::CANCEL_CONFIG:
      updateCancelConfigState(process);
      break;
    case ResetNodeState::COMPLETE:
    case ResetNodeState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateClearWatchdogFault(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ClearWatchdogFaultState> &process {clearWatchdogFaultProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node &node        {*process.nodePtr};
  bool  ackReceived {false};

  switch (process.specificState)
  {
    case ClearWatchdogFaultState::START:
      static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_WATCHDOG_RESET, Atams::WATCHDOG_RESET_PASSCODE));
      process.specificState = ClearWatchdogFaultState::CHECK_ACK;
      break;
    case ClearWatchdogFaultState::CHECK_ACK:
      static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_WATCHDOG_RESET, ackReceived));
      if (!ackReceived) process.terminate(Atams::ERROR_ACK_NOT_RECEIVED);
      else              process.setProcessComplete();
      break;
    case ClearWatchdogFaultState::COMPLETE:
    case ClearWatchdogFaultState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateValidateAtamsVersion(Atams::Error_t &error, bool &versionIsCompatible)
{
  Atams::ProcessState_t processState {updateReadMultiVarsOrderedImpl(busReadMultiVars_, error)};

  if (processState == Atams::PROCESS_COMPLETE)
  {
    versionIsCompatible = (busReadMultiVars_.process.nodePtr->validateAtamsVersion() == Atams::ERROR_NONE);
  }

  return (processState);
}

Atams::ProcessState_t NodeActions::updateValidateBusIDs(Atams::Error_t &error, bool &busIDsAreValid)
{
  Atams::ProcessState_t processState {updateReadMultiVarsOrderedImpl(busReadMultiVars_, error)};

  if (processState == Atams::PROCESS_COMPLETE)
  {
    Node     &node {*busReadMultiVars_.process.nodePtr};
    BusIDs_t  collectedBusIDs;

    busIDsAreValid = false;

    if (!error) error = node.getVar(BlockUniversal::VAR_FIRST_NODE_ID,    collectedBusIDs.firstNodeID);
    if (!error) error = node.getVar(BlockUniversal::VAR_LAST_NODE_ID,     collectedBusIDs.lastNodeID);
    if (!error) error = node.getVar(BlockUniversal::VAR_PREVIOUS_NODE_ID, collectedBusIDs.previousNodeID);

    if (error)
    {
      error        = Atams::ERROR_MEMORY_MAP;
      processState = Atams::PROCESS_ERROR;
    }
    else
    {
      busIDsAreValid = (collectedBusIDs == busIDsToSet_);
    }
  }

  return (processState);
}

Atams::ProcessState_t NodeActions::updateReadUniversalBlock(Atams::Error_t &error)
{
  return (updateReadMultiVarsOrderedImpl(busReadMultiVars_, error));
}

Atams::ProcessState_t NodeActions::updateReadMultiVarsOrdered(Atams::Error_t &error)
{
  return (updateReadMultiVarsOrderedImpl(userReadMultiVars_, error));
}

Atams::ProcessState_t NodeActions::updateSetBusIDs(Atams::Error_t &error)
{
  return (updateSetMultipleConfig(error, setBusIDsFunctions_));
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

/* Warning - no nullptr check before process.nodePtr is dereferenced. Check must be handled by calling function */
template<typename T>
void NodeActions::cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error)
{
  process.specificState = T::CANCEL_CONFIG;
  process.cancelError   = error;
  beginConfigStateExit(*process.nodePtr, false, std::nullopt);
}

template<typename T>
void NodeActions::updateCancelConfigState(ProcessHandler<T> &process)
{
  process.subProcessState = updateConfigStateExit(process.error);
  if      (process.subProcessState == Atams::PROCESS_COMPLETE) process.terminate(process.cancelError);
  else if (process.subProcessState == Atams::PROCESS_ERROR)    process.terminate(Atams::ERROR_CONFIGURATION_CANCEL_FAILED);
}

template<typename T>
Atams::ProcessState_t NodeActions::updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value)
{
  NodeActions::ProcessHandler<SetConfigVarState> &process {setConfigVarProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t &subProcessState {process.subProcessState};
  bool                   ackReceived     {false};
  T                      readConfigVar;
  std::optional<uint8_t> newNodeID;

  switch (process.specificState)
  {
    case SetConfigVarState::START:
      beginConfigStateEntry(node);
      process.specificState = SetConfigVarState::ENTER_CONFIG;
      break;
    case SetConfigVarState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetConfigVarState::WRITE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case SetConfigVarState::WRITE:
      static_cast<void>(node.clearAckSetWriteStream(varID, value));
      process.specificState = SetConfigVarState::CHECK_ACK;
      break;
    case SetConfigVarState::CHECK_ACK:
      static_cast<void>(node.stopStreamGetWriteAck(varID, ackReceived));
      if (!ackReceived) cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      else              process.specificState = SetConfigVarState::BEGIN_APPLY_CONFIG;
      break;
    case SetConfigVarState::CANCEL_CONFIG:
      updateCancelConfigState(process);
      break;
    case SetConfigVarState::BEGIN_APPLY_CONFIG:
      newNodeID = (varID == BlockUniversal::VAR_NODE_ID) ? configExitNodeID_ : std::nullopt;
      beginConfigStateExit(node, true, newNodeID);
      process.specificState = SetConfigVarState::APPLY_CONFIG;
      break;
    case SetConfigVarState::APPLY_CONFIG:
      subProcessState = updateConfigStateExit(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetConfigVarState::READ;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case SetConfigVarState::READ:
      static_cast<void>(node.clearDataReadySetReadStream(varID));
      process.specificState = SetConfigVarState::CHECK_VALUE;
      break;
    case SetConfigVarState::CHECK_VALUE:
      process.error = node.stopStreamGetVarIfDataReady(varID, readConfigVar);
      if      (process.error)          process.terminate(process.error);
      else if (readConfigVar == value) process.setProcessComplete();
      else                             process.terminate(Atams::ERROR_SET_CONFIG_VAR_FAILED);
      break;
    case SetConfigVarState::COMPLETE:
    case SetConfigVarState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateStorageProcess(Atams::Error_t &error, const uint16_t passcodeVarID, uint32_t passcode)
{
  NodeActions::ProcessHandler<StorageProcessState> &process {storageProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t &subProcessState {process.subProcessState};
  uint8_t                storeComplete   {Atams::ATAMS_FALSE};
  uint8_t                storageStatus   {Atams::ERROR_STORAGE_PROCESS_FAILED};
  uint32_t               currentTime     {Platform::getMillis()};

  switch (process.specificState)
  {
    case StorageProcessState::START:
      beginConfigStateEntry(node);
      process.specificState = StorageProcessState::ENTER_CONFIG;
      break;
    case StorageProcessState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(process.cancelError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = StorageProcessState::PROGRESS_CLEAR;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.cancelError);
      break;
    case StorageProcessState::PROGRESS_CLEAR:
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE));
      process.specificState = StorageProcessState::PROGRESS_READ_PRE;
      break;
    case StorageProcessState::PROGRESS_READ_PRE:
      /* Stream active - second read should occur */
      process.specificState = StorageProcessState::PROGRESS_CHECK_PRE;
      break;
    case StorageProcessState::PROGRESS_CHECK_PRE:
      process.error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, storeComplete);
      if      (process.error)                       cancelConfigProcess(process, process.error);
      else if (storeComplete == Atams::ATAMS_FALSE) process.specificState = StorageProcessState::CLEAR_PASSCODE;
      else                                          cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      break;
    case StorageProcessState::CLEAR_PASSCODE:
      static_cast<void>(node.clearAckSetWriteStream(passcodeVarID, static_cast<uint32_t>(0U)));
      process.specificState = StorageProcessState::WRITE_PASSCODE;
      break;
    case StorageProcessState::WRITE_PASSCODE:
      process.error = node.setVar(passcodeVarID, passcode);
      if (process.error) cancelConfigProcess(process, Atams::ERROR_MEMORY_MAP);
      else               process.specificState = StorageProcessState::STATUS_STREAM;
      break;
    case StorageProcessState::STATUS_STREAM:
      static_cast<void>(node.stopStream(passcodeVarID));
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_STORAGE_STATUS));
      process.prevEventTime = currentTime;
      process.specificState = StorageProcessState::STATUS_CHECK_POST;
      break;
    case StorageProcessState::STATUS_CHECK_POST:
      static_cast<void>(node.getVarIfDataReady(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, storeComplete));
      static_cast<void>(node.getVarIfDataReady(BlockUniversal::VAR_STORAGE_STATUS,           storageStatus));
      if (storeComplete == Atams::ATAMS_TRUE)
      {
        static_cast<void>(node.stopStream(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE));
        static_cast<void>(node.stopStream(BlockUniversal::VAR_STORAGE_STATUS));
        if (storageStatus == Atams::ERROR_NONE) process.specificState = StorageProcessState::BEGIN_EXIT_CONFIG;
        else                                    cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      }
      else if ((currentTime - process.prevEventTime) > Platform::NVM_STORAGE_TIMEOUT)
      {
        static_cast<void>(node.stopStream(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE));
        static_cast<void>(node.stopStream(BlockUniversal::VAR_STORAGE_STATUS));
        cancelConfigProcess(process, Atams::ERROR_PROCESS_TIMEOUT);
      }
      break;
    case StorageProcessState::CANCEL_CONFIG:
      updateCancelConfigState(process);
      break;
    case StorageProcessState::BEGIN_EXIT_CONFIG:
      beginConfigStateExit(node, false, std::nullopt);
      process.specificState = StorageProcessState::EXIT_CONFIG;
      break;
    case StorageProcessState::EXIT_CONFIG:
      subProcessState = updateConfigStateExit(process.cancelError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.cancelError);
      break;
    case StorageProcessState::COMPLETE:
    case StorageProcessState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState_t NodeActions::updateSetMultipleConfig(Atams::Error_t &error, SetMultiConfigFtns_t &specificFunctions)
{
  NodeActions::ProcessHandler<SetMultiConfigState> &process {setMultiConfigProcess_};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t &subProcessState {process.subProcessState};
  bool                   successFlag     {false};

  switch (process.specificState)
  {
    case SetMultiConfigState::START:
      static_cast<void>(beginConfigStateEntry(*process.nodePtr));
      process.specificState = SetMultiConfigState::ENTER_CONFIG;
      break;
    case SetMultiConfigState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetMultiConfigState::WRITE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case SetMultiConfigState::WRITE:
      specificFunctions.writeFunction(*this, node);
      process.specificState = SetMultiConfigState::CHECK_ACK;
      break;
    case SetMultiConfigState::CHECK_ACK:
      successFlag = specificFunctions.ackFunction(node);
      if (successFlag) process.specificState = SetMultiConfigState::BEGIN_APPLY_CONFIG;
      else             cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      break;
    case SetMultiConfigState::CANCEL_CONFIG:
      updateCancelConfigState(process);
      break;
    case SetMultiConfigState::BEGIN_APPLY_CONFIG:
      static_cast<void>(beginConfigStateExit(node, true, configExitNodeID_));
      process.specificState = SetMultiConfigState::APPLY_CONFIG;
      break;
    case SetMultiConfigState::APPLY_CONFIG:
      subProcessState = updateConfigStateExit(process.error);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetMultiConfigState::READ;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(process.error);
      break;
    case SetMultiConfigState::READ:
      specificFunctions.readFunction(node);
      process.specificState = SetMultiConfigState::CHECK_VALUE;
      break;
    case SetMultiConfigState::CHECK_VALUE:
      process.error = specificFunctions.checkFunction(*this, node, successFlag);
      if      (process.error) process.terminate(process.error);
      else if (successFlag)   process.setProcessComplete();
      else                    process.terminate(Atams::ERROR_SET_CONFIG_VAR_FAILED);
      break;
    case SetMultiConfigState::COMPLETE:
    case SetMultiConfigState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

void NodeActions::setUserConfigWrite(NodeActions &actionsHandler, Node &node)
{
  NodeConfig_t &userConfig {actionsHandler.nodeConfigToSet_};

  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_NODE_ID,         userConfig.newNodeID));
  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_BITRATE,         static_cast<uint8_t>(userConfig.bitrateOption)));
  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_WATCHDOG_PERIOD, userConfig.watchdogPeriod));
}

bool NodeActions::setUserConfigGetAck(Node &node)
{
  bool ackReceived {true};

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_NODE_ID,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_BITRATE,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_WATCHDOG_PERIOD, ackReceived));

  return (ackReceived);
}

void NodeActions::setUserConfigRead(Atams::Node &node)
{
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_NODE_ID));
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_BITRATE));
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_WATCHDOG_PERIOD));
}

Atams::Error_t NodeActions::setUserConfigCheck(NodeActions &actionsHandler, Atams::Node &node, bool &userConfigIsValid)
{
  Atams::Error_t error           {Atams::ERROR_NONE};
  NodeConfig_t  &requestedConfig {actionsHandler.nodeConfigToSet_};

  uint8_t  nodeID;
  uint8_t  bitrateRaw;
  uint32_t watchdogPeriod;

  userConfigIsValid = false;

  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_NODE_ID,         nodeID);
  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_BITRATE,         bitrateRaw);
  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_WATCHDOG_PERIOD, watchdogPeriod);

  if (!error)
  {
    userConfigIsValid = ((requestedConfig.newNodeID      == nodeID                                         ) &&
                         (requestedConfig.bitrateOption  == static_cast<Atams::BitrateOption_t>(bitrateRaw)) &&
                         (requestedConfig.watchdogPeriod == watchdogPeriod                                 ) );
  }

  return (error);
}

void NodeActions::setBusIDsWrite(NodeActions &actionsHandler, Atams::Node &node)
{
  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_FIRST_NODE_ID,    actionsHandler.busIDsToSet_.firstNodeID));
  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_LAST_NODE_ID,     actionsHandler.busIDsToSet_.lastNodeID));
  static_cast<void>(node.clearAckSetWriteStream(BlockUniversal::VAR_PREVIOUS_NODE_ID, actionsHandler.busIDsToSet_.previousNodeID));
}

bool NodeActions::setBusIDsGetAck(Node &node)
{
  bool ackReceived {true};

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_FIRST_NODE_ID,    ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_LAST_NODE_ID,     ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_PREVIOUS_NODE_ID, ackReceived));

  return (ackReceived);
}

void NodeActions::setBusIDsRead(Atams::Node &node)
{
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_FIRST_NODE_ID));
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_LAST_NODE_ID));
  static_cast<void>(node.clearDataReadySetReadStream(BlockUniversal::VAR_PREVIOUS_NODE_ID));
}

Atams::Error_t NodeActions::setBusIDsCheck(NodeActions &actionsHandler, Atams::Node &node, bool &allIDsValid)
{
  Atams::Error_t error {Atams::ERROR_NONE};
  BusIDs_t       collectedBusIDs;

  allIDsValid = false;

  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_FIRST_NODE_ID,    collectedBusIDs.firstNodeID);
  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_LAST_NODE_ID,     collectedBusIDs.lastNodeID);
  if (!error) error = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_PREVIOUS_NODE_ID, collectedBusIDs.previousNodeID);

  if (!error) allIDsValid = (collectedBusIDs == actionsHandler.busIDsToSet_);

  return (error);
}

void NodeActions::beginReadMultiVarsOrderedImpl(ReadMultiVarsContext &context, Node &node, uint16_t firstVarID, uint16_t lastVarID)
{
  context.groupStart = firstVarID;
  context.lastID     = lastVarID;
  context.process.resetAndAssignNode(node);
}

Atams::ProcessState_t NodeActions::updateReadMultiVarsOrderedImpl(ReadMultiVarsContext &context, Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ReadMultiVarsState> &process {context.process};

  if (process.nullptrCheck(error)) return (process.processState); /* Early Return */

  Node           &node        {*process.nodePtr};
  bool            allReady    {true};
  bool            varReady    {false};
  Atams::Error_t  streamError {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case ReadMultiVarsState::START:
      context.groupEnd = computeReadGroupEnd(node, context.groupStart, context.lastID);
      for (uint16_t varID {context.groupStart}; varID <= context.groupEnd; varID++)
      {
        static_cast<void>(node.clearDataReadySetReadStream(varID));
      }
      process.specificState = ReadMultiVarsState::CHECK_GROUP;
      break;

    case ReadMultiVarsState::STREAM_GROUP:
      for (uint16_t varID {context.groupStart}; varID <= context.groupEnd; varID++)
      {
        static_cast<void>(node.clearDataReadySetReadStream(varID));
      }
      process.specificState = ReadMultiVarsState::CHECK_GROUP;
      break;

    case ReadMultiVarsState::CHECK_GROUP:
      for (uint16_t varID {context.groupStart}; varID <= context.groupEnd; varID++)
      {
        streamError = node.stopStreamIsDataReady(varID, varReady);
        if      (streamError) process.terminate(Atams::ERROR_MEMORY_MAP);
        else if (!varReady)   allReady = false;
      }
      if (allReady)
      {
        if (context.groupEnd >= context.lastID)
        {
          process.setProcessComplete();
        }
        else
        {
          context.groupStart    = context.groupEnd + 1U;
          context.groupEnd      = computeReadGroupEnd(node, context.groupStart, context.lastID);
          process.specificState = ReadMultiVarsState::STREAM_GROUP;
        }
      }
      else
      {
        process.specificState = ReadMultiVarsState::STREAM_GROUP;
      }
      break;

    case ReadMultiVarsState::COMPLETE:
    case ReadMultiVarsState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;

    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

uint16_t NodeActions::computeReadGroupEnd(Node &node, uint16_t groupStart, uint16_t lastVarID)
{
  constexpr uint16_t available {static_cast<uint16_t>(Atams::MINIMUM_MAXIMUM_BUS_PAYLOAD_SIZE)};

  uint16_t bytesUsed {0U};
  uint16_t groupEnd  {groupStart};
  uint8_t  varLen    {0U};

  for (uint16_t varID {groupStart}; varID <= lastVarID; varID++)
  {
    if (node.getVarLength(varID, varLen) != Atams::ERROR_NONE) break;

    const uint16_t cost {static_cast<uint16_t>(static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER) + static_cast<uint16_t>(varLen))};

    if ((bytesUsed + cost) > available) break;

    bytesUsed += cost;
    groupEnd   = varID;
  }

  return (groupEnd);
}

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
void NodeActions::ProcessHandler<T>::resetAndAssignNode(Node &node)
{
  this->nodePtr       = &node;
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::START;
  this->processState  = Atams::PROCESS_IN_PROGRESS;
  this->nodePtr->clearAllRequestPatterns();
}

template <typename T>
void NodeActions::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  this->error         = exitError;
  this->specificState = T::ERROR;
  this->processState  = Atams::PROCESS_ERROR;
  if (this->nodePtr != nullptr) this->nodePtr->clearAllRequestPatterns();
}

template <typename T>
void NodeActions::ProcessHandler<T>::setProcessComplete(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState  = Atams::PROCESS_COMPLETE;
  if (this->nodePtr != nullptr) this->nodePtr->clearAllRequestPatterns();
}

template <typename T>
bool NodeActions::ProcessHandler<T>::nullptrCheck(Atams::Error_t &errorReturn)
{
  bool ptrIsNull {false};

  if (this->nodePtr == nullptr)
  {
    this->terminate(Atams::ERROR_NULLPTR);
    ptrIsNull   = true;
    errorReturn = Atams::ERROR_NULLPTR;
  }

  return (ptrIsNull);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
