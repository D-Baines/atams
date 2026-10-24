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
  if (configEntryProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  configEntryProcess_.resetAndAssignNode(node);
}

void NodeActions::beginConfigStateExit(Node &node, bool applyChangesOnExit, std::optional<uint8_t> newNodeID)
{
  if (configExitProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  applyChanges_     = applyChangesOnExit;
  configExitNodeID_ = newNodeID;
  configExitProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetNodeID(Node &node, const uint8_t nodeID)
{
  if (setConfigVarProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  nodeIDToSet_      = nodeID;
  configExitNodeID_ = nodeID;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetBitrate(Node &node, const Atams::BitrateOption_t bitrateOption)
{
  if (setConfigVarProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  bitrateOptionToSet_ = bitrateOption;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetWatchdogPeriod(Node &node, const uint32_t watchdogPeriod)
{
  if (setConfigVarProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  watchdogPeriodToSet_ = watchdogPeriod;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetNodeConfig(Node &node, const Atams::NodeConfig_t &userConfig)
{
  if (setMultiConfigProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  nodeConfigToSet_  = userConfig;
  configExitNodeID_ = userConfig.newNodeID;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::beginStorageProcess(Node &node)
{
  if (storageProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  storageProcess_.resetAndAssignNode(node);
}

void NodeActions::beginResetNode(Node &node)
{
  if (resetNodeProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  resetNodeProcess_.resetAndAssignNode(node);
}

void NodeActions::beginClearWatchdogFault(Node &node)
{
  if (clearWatchdogFaultProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  clearWatchdogFaultProcess_.resetAndAssignNode(node);
}

void NodeActions::beginValidateAtamsVersion(Node &node)
{
  if (busReadMultiVars_.process.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(busReadMultiVars_, 
                                node, 
                                BlockUniversal::VAR_ATAMS_VERSION_MAJOR, 
                                BlockUniversal::VAR_ATAMS_VERSION_PATCH, false);
}

void NodeActions::beginValidateBusIDs(Node &node, const Atams::BusIDs_t busIDs)
{
  if (busReadMultiVars_.process.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  busIDsToSet_ = busIDs;
  beginReadMultiVarsOrderedImpl(busReadMultiVars_, node, BlockUniversal::VAR_FIRST_NODE_ID, BlockUniversal::VAR_PREVIOUS_NODE_ID, false);
}

void NodeActions::beginReadUniversalBlock(Node &node)
{
  if (busReadMultiVars_.process.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(busReadMultiVars_,
                                node,
                                BlockUniversal::VAR_ATAMS_VERSION_MAJOR,
                                BlockUniversal::VAR_COBS_ERROR_COUNT,
                                false);
}

void NodeActions::beginReadMultiVarsOrdered(Node &node, Atams::VarID_t firstVarID, Atams::VarID_t lastVarID, bool skipActiveReads)
{
  if (userReadMultiVars_.process.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  beginReadMultiVarsOrderedImpl(userReadMultiVars_, node, firstVarID, lastVarID, skipActiveReads);
}

void NodeActions::beginSetBusIDs(Node &node, const Atams::BusIDs_t busIDs)
{
  if (setMultiConfigProcess_.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    return; /* Early Return */
  }

  busIDsToSet_      = busIDs;
  configExitNodeID_ = std::nullopt;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::abort(const Atams::Error_t error)
{
  abortProcess(configEntryProcess_,         error);
  abortProcess(configExitProcess_,          error);
  abortProcess(setConfigVarProcess_,        error);
  abortProcess(storageProcess_,             error);
  abortProcess(resetNodeProcess_,           error);
  abortProcess(clearWatchdogFaultProcess_,  error);
  abortProcess(setMultiConfigProcess_,      error);
  abortProcess(busReadMultiVars_.process,   error);
  abortProcess(userReadMultiVars_.process,  error);
}

Atams::ProcessState_t NodeActions::updateConfigStateEntry(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigEntryState> &process {configEntryProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node          &node        {*process.nodePtr};
  uint8_t        configState {Atams::CONFIGURATION_STATUS_INACTIVE};
  Atams::Error_t stepError   {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case ConfigEntryState::START:
    case ConfigEntryState::COLLECT_STATUS_PRE:
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_STATUS);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigEntryState::CHECK_STATUS_PRE;
      break;
    case ConfigEntryState::CHECK_STATUS_PRE:
      stepError = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (stepError)                                         process.terminate(stepError);
      else if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) process.setProcessComplete();
      else                                                        process.specificState = ConfigEntryState::WRITE_PASSCODE;
      break;
    case ConfigEntryState::WRITE_PASSCODE:
      stepError = clearAckSetWriteStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_PASSKEY,
                                              Atams::CONFIGURATION_PASSKEY_ACCESS);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigEntryState::COLLECT_STATUS_POST;
      break;
    case ConfigEntryState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY));
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_STATUS);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigEntryState::CHECK_STATUS_POST;
      break;
    case ConfigEntryState::CHECK_STATUS_POST:
      stepError = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (stepError)                                           process.terminate(stepError);
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

  error = process.getError();

  return (process.getProcessState());
}

Atams::ProcessState_t NodeActions::updateConfigStateExit(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigExitState> &process {configExitProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node          &node          {*process.nodePtr};
  uint8_t        configState   {Atams::CONFIGURATION_STATUS_ACTIVE};
  uint32_t       configPasskey {applyChanges_ ? Atams::CONFIGURATION_PASSKEY_APPLY : Atams::CONFIGURATION_PASSKEY_CANCEL};
  Atams::Error_t stepError     {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case ConfigExitState::START:
    case ConfigExitState::COLLECT_STATUS_PRE:
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_STATUS);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigExitState::CHECK_STATUS_PRE;
      break;
    case ConfigExitState::CHECK_STATUS_PRE:
      stepError = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (stepError)                                         process.terminate(stepError);
      else if (configState != Atams::CONFIGURATION_STATUS_ACTIVE) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      else                                                        process.specificState = ConfigExitState::WRITE_PASSCODE;
      break;
    case ConfigExitState::WRITE_PASSCODE:
      stepError = clearAckSetWriteStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_PASSKEY, configPasskey);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigExitState::COLLECT_STATUS_POST;
      break;
    case ConfigExitState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_CONFIGURATION_PASSKEY));
      if (configExitNodeID_.has_value()) node.setNodeID(configExitNodeID_.value());
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_CONFIGURATION_STATUS);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ConfigExitState::CHECK_STATUS_POST;
      break;
    case ConfigExitState::CHECK_STATUS_POST:
      stepError = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_CONFIGURATION_STATUS, configState);
      if      (stepError)                                                process.terminate(stepError);
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

  error = process.getError();

  return (process.getProcessState());
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

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t  subProcessState {Atams::PROCESS_IN_PROGRESS};
  bool                   ackReceived     {false};
  Atams::Error_t         stepError       {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case ResetNodeState::START:
      beginConfigStateEntry(node);
      process.specificState = ResetNodeState::ENTER_CONFIG;
      break;
    case ResetNodeState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = ResetNodeState::WRITE_PASSCODE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case ResetNodeState::WRITE_PASSCODE:
      stepError = clearAckSetWriteStreamOwned(node, &process, BlockUniversal::VAR_RESET_NODE, Atams::RESET_NODE_PASSCODE);
      if (stepError) cancelConfigProcess(process, stepError);
      else           process.specificState = ResetNodeState::CHECK_ACK;
      break;
    case ResetNodeState::CHECK_ACK:
      stepError = node.stopStreamGetWriteAck(BlockUniversal::VAR_RESET_NODE, ackReceived);
      if      (stepError)    cancelConfigProcess(process, stepError);
      else if (!ackReceived) cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      else                   process.setProcessComplete();
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

  error = process.getError();

  return (process.getProcessState());
}

Atams::ProcessState_t NodeActions::updateClearWatchdogFault(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ClearWatchdogFaultState> &process {clearWatchdogFaultProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node          &node        {*process.nodePtr};
  bool           ackReceived {false};
  Atams::Error_t stepError   {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case ClearWatchdogFaultState::START:
      stepError = clearAckSetWriteStreamOwned(node, &process, BlockUniversal::VAR_WATCHDOG_RESET, Atams::WATCHDOG_RESET_PASSCODE);
      if (stepError) process.terminate(stepError);
      else           process.specificState = ClearWatchdogFaultState::CHECK_ACK;
      break;
    case ClearWatchdogFaultState::CHECK_ACK:
      stepError = node.stopStreamGetWriteAck(BlockUniversal::VAR_WATCHDOG_RESET, ackReceived);
      if      (stepError)    process.terminate(stepError);
      else if (!ackReceived) process.terminate(Atams::ERROR_ACK_NOT_RECEIVED);
      else                   process.setProcessComplete();
      break;
    case ClearWatchdogFaultState::COMPLETE:
    case ClearWatchdogFaultState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.getError();

  return (process.getProcessState());
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
    Node          &node      {*busReadMultiVars_.process.nodePtr};
    Atams::Error_t stepError {Atams::ERROR_NONE};
    BusIDs_t       collectedBusIDs;

    if (!stepError) stepError = node.getVar(BlockUniversal::VAR_FIRST_NODE_ID,    collectedBusIDs.firstNodeID);
    if (!stepError) stepError = node.getVar(BlockUniversal::VAR_LAST_NODE_ID,     collectedBusIDs.lastNodeID);
    if (!stepError) stepError = node.getVar(BlockUniversal::VAR_PREVIOUS_NODE_ID, collectedBusIDs.previousNodeID);

    busIDsAreValid = (!stepError) && (collectedBusIDs == busIDsToSet_);
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

template<typename T>
void NodeActions::abortProcess(ProcessHandler<T> &process, const Atams::Error_t error)
{
  if (process.getProcessState() == Atams::PROCESS_IN_PROGRESS)
  {
    process.terminate(error);
  }
}

/* Warning - no nullptr check before process.nodePtr is dereferenced. Check must be handled by calling function */
template<typename T>
void NodeActions::cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error)
{
  process.specificState = T::CANCEL_CONFIG;
  process.cancelError   = error;
  process.nodePtr->clearRequestPatternsOwned(&process); /* Stop any parent requests before exiting config */
  beginConfigStateExit(*process.nodePtr, false, std::nullopt);
}

template<typename T>
void NodeActions::updateCancelConfigState(ProcessHandler<T> &process)
{
  Atams::Error_t        exitError       {Atams::ERROR_NONE}; /* Not reported - cancelError or CANCEL_FAILED is reported instead */
  Atams::ProcessState_t subProcessState {updateConfigStateExit(exitError)};

  if      (subProcessState == Atams::PROCESS_COMPLETE) process.terminate(process.cancelError);
  else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(Atams::ERROR_CONFIGURATION_CANCEL_FAILED);
}

template<typename T>
Atams::ProcessState_t NodeActions::updateSetConfigVar(Atams::Error_t &error, const Atams::VarID_t varID, const T value)
{
  NodeActions::ProcessHandler<SetConfigVarState> &process {setConfigVarProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t  subProcessState {Atams::PROCESS_IN_PROGRESS};
  bool                   ackReceived     {false};
  T                      readConfigVar;
  std::optional<uint8_t> newNodeID;
  Atams::Error_t         stepError       {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case SetConfigVarState::START:
      beginConfigStateEntry(node);
      process.specificState = SetConfigVarState::ENTER_CONFIG;
      break;
    case SetConfigVarState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetConfigVarState::WRITE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case SetConfigVarState::WRITE:
      stepError = clearAckSetWriteStreamOwned(node, &process, varID, value);
      if (stepError) cancelConfigProcess(process, stepError);
      else           process.specificState = SetConfigVarState::CHECK_ACK;
      break;
    case SetConfigVarState::CHECK_ACK:
      stepError = node.stopStreamGetWriteAck(varID, ackReceived);
      if      (stepError)    cancelConfigProcess(process, stepError);
      else if (!ackReceived) cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      else                   process.specificState = SetConfigVarState::BEGIN_APPLY_CONFIG;
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
      subProcessState = updateConfigStateExit(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetConfigVarState::READ;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case SetConfigVarState::READ:
      stepError = clearDataReadySetReadStreamOwned(node, &process, varID);
      if (stepError) process.terminate(stepError);
      else           process.specificState = SetConfigVarState::CHECK_VALUE;
      break;
    case SetConfigVarState::CHECK_VALUE:
      stepError = node.stopStreamGetVarIfDataReady(varID, readConfigVar);
      if      (stepError)              process.terminate(stepError);
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

  error = process.getError();

  return (process.getProcessState());
}

Atams::ProcessState_t NodeActions::updateStorageProcess(Atams::Error_t &error, const Atams::VarID_t passcodeVarID, uint32_t passcode)
{
  NodeActions::ProcessHandler<StorageProcessState> &process {storageProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t  subProcessState {Atams::PROCESS_IN_PROGRESS};
  uint8_t                storeComplete   {Atams::ATAMS_FALSE};
  uint8_t                storageStatus   {Atams::ERROR_STORAGE_PROCESS_FAILED};
  uint32_t               currentTime     {Platform::getMillis()};
  Atams::Error_t         stepError       {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case StorageProcessState::START:
      beginConfigStateEntry(node);
      process.specificState = StorageProcessState::ENTER_CONFIG;
      break;
    case StorageProcessState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = StorageProcessState::PROGRESS_CLEAR;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case StorageProcessState::PROGRESS_CLEAR:
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE);
      if (stepError) cancelConfigProcess(process, stepError);
      else           process.specificState = StorageProcessState::PROGRESS_READ_PRE;
      break;
    case StorageProcessState::PROGRESS_READ_PRE:
      /* Stream active - second read should occur */
      process.specificState = StorageProcessState::PROGRESS_CHECK_PRE;
      break;
    case StorageProcessState::PROGRESS_CHECK_PRE:
      stepError = node.stopStreamGetVarIfDataReady(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, storeComplete);
      if      (stepError)                           cancelConfigProcess(process, stepError);
      else if (storeComplete == Atams::ATAMS_FALSE) process.specificState = StorageProcessState::CLEAR_PASSCODE;
      else                                          cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      break;
    case StorageProcessState::CLEAR_PASSCODE:
      stepError = clearAckSetWriteStreamOwned(node, &process, passcodeVarID, static_cast<uint32_t>(0U));
      if (stepError) cancelConfigProcess(process, stepError);
      else           process.specificState = StorageProcessState::WRITE_PASSCODE;
      break;
    case StorageProcessState::WRITE_PASSCODE:
      stepError = node.setVar(passcodeVarID, passcode);
      if (stepError) cancelConfigProcess(process, Atams::ERROR_MEMORY_MAP);
      else           process.specificState = StorageProcessState::STATUS_STREAM;
      break;
    case StorageProcessState::STATUS_STREAM:
      static_cast<void>(node.stopStream(passcodeVarID));
      stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE);
      if (!stepError) stepError = clearDataReadySetReadStreamOwned(node, &process, BlockUniversal::VAR_STORAGE_STATUS);
      if (stepError)
      {
        cancelConfigProcess(process, stepError);
      }
      else
      {
        process.prevEventTime = currentTime;
        process.specificState = StorageProcessState::STATUS_CHECK_POST;
      }
      break;
    case StorageProcessState::STATUS_CHECK_POST:
      /* Results discarded - NEW_DATA_NOT_READY is expected while polling, and any other failure ends in the timeout below */
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
      subProcessState = updateConfigStateExit(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case StorageProcessState::COMPLETE:
    case StorageProcessState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.getError();

  return (process.getProcessState());
}

Atams::ProcessState_t NodeActions::updateSetMultipleConfig(Atams::Error_t &error, SetMultiConfigFtns_t &specificFunctions)
{
  NodeActions::ProcessHandler<SetMultiConfigState> &process {setMultiConfigProcess_};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  Node                  &node            {*process.nodePtr};
  Atams::ProcessState_t  subProcessState {Atams::PROCESS_IN_PROGRESS};
  bool                   successFlag     {false};
  Atams::Error_t         stepError       {Atams::ERROR_NONE};

  switch (process.specificState)
  {
    case SetMultiConfigState::START:
      static_cast<void>(beginConfigStateEntry(*process.nodePtr));
      process.specificState = SetMultiConfigState::ENTER_CONFIG;
      break;
    case SetMultiConfigState::ENTER_CONFIG:
      subProcessState = updateConfigStateEntry(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetMultiConfigState::WRITE;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case SetMultiConfigState::WRITE:
      stepError = specificFunctions.writeFunction(*this, node, &process);
      if (stepError) cancelConfigProcess(process, stepError);
      else           process.specificState = SetMultiConfigState::CHECK_ACK;
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
      subProcessState = updateConfigStateExit(stepError);
      if      (subProcessState == Atams::PROCESS_COMPLETE) process.specificState = SetMultiConfigState::READ;
      else if (subProcessState == Atams::PROCESS_ERROR)    process.terminate(stepError);
      break;
    case SetMultiConfigState::READ:
      stepError = specificFunctions.readFunction(node, &process);
      if (stepError) process.terminate(stepError);
      else           process.specificState = SetMultiConfigState::CHECK_VALUE;
      break;
    case SetMultiConfigState::CHECK_VALUE:
      stepError = specificFunctions.checkFunction(*this, node, successFlag);
      if      (stepError)   process.terminate(stepError);
      else if (successFlag) process.setProcessComplete();
      else                  process.terminate(Atams::ERROR_SET_CONFIG_VAR_FAILED);
      break;
    case SetMultiConfigState::COMPLETE:
    case SetMultiConfigState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.getError();

  return (process.getProcessState());
}

Atams::Error_t NodeActions::setUserConfigWrite(NodeActions &actionsHandler, Node &node, const RequestOwner *owner)
{
  NodeConfig_t  &userConfig {actionsHandler.nodeConfigToSet_};
  Atams::Error_t error      {Atams::ERROR_NONE};

  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_NODE_ID,         userConfig.newNodeID);
  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_BITRATE,         static_cast<uint8_t>(userConfig.bitrateOption));
  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_WATCHDOG_PERIOD, userConfig.watchdogPeriod);

  return (error);
}

/* stopStreamGetWriteAck errors discarded - the same vars were written without error in the WRITE step */
bool NodeActions::setUserConfigGetAck(Node &node)
{
  bool ackReceived {true};

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_NODE_ID,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_BITRATE,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_WATCHDOG_PERIOD, ackReceived));

  return (ackReceived);
}

Atams::Error_t NodeActions::setUserConfigRead(Atams::Node &node, const RequestOwner *owner)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_NODE_ID);
  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_BITRATE);
  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_WATCHDOG_PERIOD);

  return (error);
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

Atams::Error_t NodeActions::setBusIDsWrite(NodeActions &actionsHandler, Atams::Node &node, const RequestOwner *owner)
{
  BusIDs_t      &busIDs {actionsHandler.busIDsToSet_};
  Atams::Error_t error  {Atams::ERROR_NONE};

  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_FIRST_NODE_ID,    busIDs.firstNodeID);
  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_LAST_NODE_ID,     busIDs.lastNodeID);
  if (!error) error = clearAckSetWriteStreamOwned(node, owner, BlockUniversal::VAR_PREVIOUS_NODE_ID, busIDs.previousNodeID);

  return (error);
}

/* stopStreamGetWriteAck errors discarded - the same vars were written without error in the WRITE step */
bool NodeActions::setBusIDsGetAck(Node &node)
{
  bool ackReceived {true};

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_FIRST_NODE_ID,    ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_LAST_NODE_ID,     ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_PREVIOUS_NODE_ID, ackReceived));

  return (ackReceived);
}

Atams::Error_t NodeActions::setBusIDsRead(Atams::Node &node, const RequestOwner *owner)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_FIRST_NODE_ID);
  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_LAST_NODE_ID);
  if (!error) error = clearDataReadySetReadStreamOwned(node, owner, BlockUniversal::VAR_PREVIOUS_NODE_ID);

  return (error);
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

void NodeActions::beginReadMultiVarsOrderedImpl(ReadMultiVarsContext &context, Node &node, Atams::VarID_t firstVarID, Atams::VarID_t lastVarID, bool skipActiveReads)
{
  context.groupStart      = firstVarID;
  context.lastID          = lastVarID;
  context.groupEnd        = firstVarID; /* Set when the group is streamed */
  context.skipActiveReads = skipActiveReads;
  context.skippedMask     = 0U;
  context.process.resetAndAssignNode(node);
}

Atams::ProcessState_t NodeActions::updateReadMultiVarsOrderedImpl(ReadMultiVarsContext &context, Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ReadMultiVarsState> &process {context.process};

  if (process.nullptrCheck(error)) return (process.getProcessState()); /* Early Return */

  switch (process.specificState)
  {
    case ReadMultiVarsState::START:
      if (context.groupStart > context.lastID) process.terminate(Atams::ERROR_VAR_ID); /* Invalid var range */
      else                                     streamReadGroup(context);
      break;

    case ReadMultiVarsState::STREAM_GROUP:
      streamReadGroup(context);
      break;

    case ReadMultiVarsState::CHECK_GROUP:
      checkReadGroup(context);
      break;

    case ReadMultiVarsState::COMPLETE:
    case ReadMultiVarsState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;

    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.getError();

  return (process.getProcessState());
}

/* Warning - no nullptr check before process.nodePtr is dereferenced. Check must be handled by calling function */
void NodeActions::streamReadGroup(ReadMultiVarsContext &context)
{
  NodeActions::ProcessHandler<ReadMultiVarsState> &process {context.process};

  Node          &node        {*process.nodePtr};
  uint16_t       varID       {context.groupStart};
  Atams::Error_t streamError {Atams::ERROR_NONE};

  context.skippedMask = 0U;

  /* Add vars until the packet is full, the group limit is reached, or the last var is reached */
  for (; (varID <= context.lastID) && ((varID - context.groupStart) < MAX_READ_GROUP_VARS); varID++)
  {
    if ((context.skipActiveReads == true) && (hasActiveRead(node, static_cast<Atams::VarID_t>(varID)) == true))
    {
      /* Existing read request supplies the value - leave its request and data ready flag untouched */
      context.skippedMask |= (1U << (varID - context.groupStart));
    }
    else
    {
      streamError = clearDataReadySetReadStreamOwned(node, &process, static_cast<Atams::VarID_t>(varID));

      if (streamError != Atams::ERROR_NONE) break;
    }
  }

  /* varID is now the first var not in the group */
  const bool packetFull {(streamError == Atams::ERROR_RESPONSE_BUFFER_LENGTH) ||
                         (streamError == Atams::ERROR_REQUEST_BUFFER_LENGTH )  };

  if      ((streamError != Atams::ERROR_NONE) && (packetFull == false)) process.terminate(streamError);
  else if (varID == context.groupStart)                                 process.terminate(streamError);
  else
  {
    context.groupEnd      = varID - 1U;
    process.specificState = ReadMultiVarsState::CHECK_GROUP;
  }
}

/* Warning - no nullptr check before process.nodePtr is dereferenced. Check must be handled by calling function */
void NodeActions::checkReadGroup(ReadMultiVarsContext &context)
{
  NodeActions::ProcessHandler<ReadMultiVarsState> &process {context.process};

  Node &node           {*process.nodePtr};
  bool  allReady       {true};
  bool  varReady       {false};
  bool  memoryMapError {false};

  for (uint16_t varID {context.groupStart}; varID <= context.groupEnd; varID++)
  {
    if ((context.skippedMask & (1U << (varID - context.groupStart))) != 0U) continue; /* Skipped var - value already read */

    if      (node.stopStreamIsDataReady(static_cast<Atams::VarID_t>(varID), varReady)) memoryMapError = true;
    else if (!varReady)                                                            allReady       = false;
  }

  if      (memoryMapError)                     process.terminate(Atams::ERROR_MEMORY_MAP);
  else if (!allReady)                          process.terminate(Atams::ERROR_NEW_DATA_NOT_READY);
  else if (context.groupEnd >= context.lastID) process.setProcessComplete();
  else
  {
    context.groupStart    = context.groupEnd + 1U;
    process.specificState = ReadMultiVarsState::STREAM_GROUP;
  }
}

bool NodeActions::hasActiveRead(Node &node, const Atams::VarID_t varID)
{
  Atams::Access_t         currentAccess  {Atams::ACCESS_NONE};
  Atams::RequestPattern_t currentPattern {Atams::REQUEST_INACTIVE};

  return ((node.getRequestPattern(varID, currentAccess, currentPattern) == Atams::ERROR_NONE      ) &&
          (currentAccess                                                == Atams::ACCESS_READ     ) &&
          (currentPattern                                               != Atams::REQUEST_INACTIVE) );
}

Atams::Error_t NodeActions::clearDataReadySetReadStreamOwned(Node &node, const RequestOwner *owner, const Atams::VarID_t varID)
{
  Atams::Error_t error {node.clearDataReady(varID)};

  if (!error) error = node.setRequestPatternOwned(owner, varID, Atams::ACCESS_READ, Atams::REQUEST_STREAM);

  return (error);
}

template <typename T>
Atams::Error_t NodeActions::clearAckSetWriteStreamOwned(Node &node, const RequestOwner *owner, const Atams::VarID_t varID, const T writeData)
{
  Atams::Error_t error {node.clearWriteAck(varID)};

  if (!error) error = node.setVar(varID, writeData);
  if (!error) error = node.setRequestPatternOwned(owner, varID, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM);

  return (error);
}

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
const Atams::ProcessState_t & NodeActions::ProcessHandler<T>::getProcessState(void) const
{
  return (this->processState_);
}

template <typename T>
Atams::Error_t NodeActions::ProcessHandler<T>::getError(void) const
{
  return (this->error_);
}

template <typename T>
void NodeActions::ProcessHandler<T>::resetAndAssignNode(Node &node)
{
  /* Clear any requests left on the previous Node by an abandoned process */
  if (this->nodePtr != nullptr) this->nodePtr->clearRequestPatternsOwned(this);

  this->nodePtr       = &node;
  this->cancelError   = Atams::ERROR_NONE;
  this->error_        = Atams::ERROR_NONE;
  this->specificState = T::START;
  this->processState_ = Atams::PROCESS_IN_PROGRESS;
}

template <typename T>
void NodeActions::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  this->error_        = exitError;
  this->specificState = T::ERROR;
  this->processState_ = Atams::PROCESS_ERROR;
  if (this->nodePtr != nullptr) this->nodePtr->clearRequestPatternsOwned(this);
}

template <typename T>
void NodeActions::ProcessHandler<T>::setProcessComplete(void)
{
  this->error_        = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState_ = Atams::PROCESS_COMPLETE;
  if (this->nodePtr != nullptr) this->nodePtr->clearRequestPatternsOwned(this);
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
