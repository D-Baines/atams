/**
  ******************************************************************************
  * @file    NodeActions.cpp
  *
  * @author  D. Baines
  *
  * @brief
  *
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "NodeActions.hpp"
#include "../Node.hpp"
#include "NodeCallbackHandler.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

void NodeActions::beginConfigEntryProcess(Node &node)
{
  return (configEntryProcess_.resetAndAssignNode(node));
}

void NodeActions::beginConfigExitProcess(Node &node, bool applyChangesOnExit)
{
  applyChanges_ = applyChangesOnExit;
  configExitProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetNodeID(Node &node, const uint8_t nodeID)
{
  nodeIDToSet_ = nodeID;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetBitrate(Node &node, const Atams::BitrateOption_t bitrateOption)
{
  bitrateOptionToSet_ = bitrateOption;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetWatchdogPeriod(Node &node, const uint32_t watchdogPeriod)
{
  watchdogPeriodToSet_ = watchdogPeriod;
  setConfigVarProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetUserConfig(Node &node, Atams::NodeUserConfig_t &userConfig)
{
  userConfigToSet_ = userConfig;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::beginStorageProcess(Node &node)
{
  storageProcess_.resetAndAssignNode(node);
}

void NodeActions::beginResetNodeProcess(Node &node)
{
  resetNodeProcess_.resetAndAssignNode(node);
}

void NodeActions::beginValidateGenInfoProcess(Node &node)
{
  validateMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::beginValidateBusIDsProcess(Node &node, Atams::BusIDs_t busIDs)
{
  busIDsToSet_ = busIDs;
  validateMultiConfigProcess_.resetAndAssignNode(node);
}

void NodeActions::beginSetBusIDs(Node &node, const Atams::BusIDs_t busIDs)
{
  busIDsToSet_ = busIDs;
  setMultiConfigProcess_.resetAndAssignNode(node);
}

Atams::ProcessState NodeActions::updateConfigurationStateEntry(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigEntryState> &process = configEntryProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node   &node        = *process.nodePtr;
  uint8_t configState = Atams::CONFIGURATION_STATUS_INACTIVE;

  switch (process.specificState)
  {
    case ConfigEntryState::START:
      process.specificState = ConfigEntryState::COLLECT_STATUS_PRE;
      break;
    case ConfigEntryState::COLLECT_STATUS_PRE:
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_PRE;
      break;
    case ConfigEntryState::CHECK_STATUS_PRE:
      process.error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if      (process.error)                                     process.terminate(process.error);
      else if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) process.setProcessComplete();
      else                                                        process.specificState = ConfigEntryState::WRITE_PASSCODE;
      break; 
    case ConfigEntryState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, 
                                                Atams::CONFIGURATION_PASSKEY_ACCESS));
      process.specificState = ConfigEntryState::COLLECT_STATUS_POST;
      break;
    case ConfigEntryState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_POST;
      break;
    case ConfigEntryState::CHECK_STATUS_POST:
      process.error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
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

Atams::ProcessState NodeActions::updateConfigurationStateExit(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigExitState> &process = configExitProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node    &node          = *process.nodePtr;
  uint8_t  configState   = Atams::CONFIGURATION_STATUS_ACTIVE;
  uint32_t configPasskey = applyChanges_ ? Atams::CONFIGURATION_PASSKEY_APPLY : Atams::CONFIGURATION_PASSKEY_CANCEL; 

  switch (process.specificState)
  {
    case ConfigExitState::START:
      process.specificState = ConfigExitState::COLLECT_STATUS_PRE;
      break;
    case ConfigExitState::COLLECT_STATUS_PRE:
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_PRE;
      break;
    case ConfigExitState::CHECK_STATUS_PRE:
      process.error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if      (process.error)                                     process.terminate(process.error);
      else if (configState != Atams::CONFIGURATION_STATUS_ACTIVE) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      else                                                        process.specificState = ConfigExitState::WRITE_PASSCODE;           
      break; 
    case ConfigExitState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, configPasskey));
      process.specificState = ConfigExitState::COLLECT_STATUS_POST;
      break;
    case ConfigExitState::COLLECT_STATUS_POST:
      static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_POST;
      break;
    case ConfigExitState::CHECK_STATUS_POST:
      process.error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
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

Atams::ProcessState NodeActions::updateSetNodeID(Atams::Error_t &error)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_NODE_ID, nodeIDToSet_));
}

Atams::ProcessState NodeActions::updateSetBitrate(Atams::Error_t &error)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_BITRATE, static_cast<uint8_t>(bitrateOptionToSet_)));
}

Atams::ProcessState NodeActions::updateSetWatchdogPeriod(Atams::Error_t &error)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_WATCHDOG_PERIOD, watchdogPeriodToSet_));
}

Atams::ProcessState NodeActions::updateSetUserConfig(Atams::Error_t &error)
{
  return (updateSetMultipleConfig(error, setUserConfigFunctions_));
}

Atams::ProcessState NodeActions::updateStoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_STORE_ALL, Atams::STORE_ALL_PASSCODE));
}

Atams::ProcessState NodeActions::updateRestoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_RESTORE_ALL, Atams::RESTORE_ALL_PASSCODE));
}

Atams::ProcessState NodeActions::updateRestoreUserBlocks(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS, Atams::RESTORE_USER_BLOCKS_PASSCODE));
}

Atams::ProcessState NodeActions::updateResetNode(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ResetNodeState> &process = resetNodeProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState = process.subProcessState;
  bool                 ackReceived     = false;

  switch (process.specificState)
  {
    case ResetNodeState::START:
      beginConfigEntryProcess(node);
      process.specificState = ResetNodeState::ENTER_CONFIG;
      break;
    case ResetNodeState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = ResetNodeState::WRITE_PASSCODE;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case ResetNodeState::WRITE_PASSCODE:
      static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_RESET_NODE, Atams::RESET_NODE_PASSCODE));
      process.specificState = ResetNodeState::CHECK_ACK;
      break;
    case ResetNodeState::CHECK_ACK:
      static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_RESET_NODE, ackReceived));
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

Atams::ProcessState NodeActions::updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid)
{
  return (updateValidateMultipleConfig(error, genInfoIsValid, validateGenInfoFunctions_));
}

Atams::ProcessState NodeActions::updateValidateBusIDs(Atams::Error_t &error, bool &allIDsValid)
{
  return (updateValidateMultipleConfig(error, allIDsValid, validateBusIDsFunctions_));
}

Atams::ProcessState NodeActions::updateSetBusIDs(Atams::Error_t &error)
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
  beginConfigExitProcess(*process.nodePtr, false);
}

template<typename T>
void NodeActions::updateCancelConfigState(ProcessHandler<T> &process)
{
  process.subProcessState = updateConfigurationStateExit(process.error);
  if      (process.subProcessState == Atams::ProcessState::COMPLETE) process.terminate(process.cancelError);
  else if (process.subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);       
}

template<typename T>
Atams::ProcessState NodeActions::updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value)
{
  NodeActions::ProcessHandler<SetConfigVarState> &process = setConfigVarProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState = process.subProcessState;
  bool                 ackReceived     = false;
  T                    readConfigVar;

  switch (process.specificState)
  {
    case SetConfigVarState::START:
      beginConfigEntryProcess(node);
      process.specificState = SetConfigVarState::ENTER_CONFIG;
      break;
    case SetConfigVarState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = SetConfigVarState::WRITE;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
      break;
    case SetConfigVarState::WRITE:
      static_cast<void>(node.clearAckStartWrite(varID, value));
      process.specificState = SetConfigVarState::READ;
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
      beginConfigExitProcess(node, true);
      process.specificState = SetConfigVarState::APPLY_CONFIG;
      break;
    case SetConfigVarState::APPLY_CONFIG:
      subProcessState = updateConfigurationStateExit(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = SetConfigVarState::READ;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);   
      break;
    case SetConfigVarState::READ:
      static_cast<void>(node.clearDataReadyStartRead(varID));
      process.specificState = SetConfigVarState::CHECK_VALUE;
      break;
    case SetConfigVarState::CHECK_VALUE:
      process.error = node.stopStreamReadIfDataReady(varID, readConfigVar);
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

Atams::ProcessState NodeActions::updateStorageProcess(Atams::Error_t &error, const uint16_t passcodeVarID, uint32_t passcode)
{
  NodeActions::ProcessHandler<StorageProcessState> &process = storageProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState = process.subProcessState;
  uint8_t              storeComplete   = Atams::ATAMS_FALSE;
  uint8_t              storageStatus   = Atams::ERROR_STORAGE_PROCESS_FAILED;
  uint32_t             currentTime     = Platform::getMillis();

  switch (process.specificState)
  {
    case StorageProcessState::START:
      beginConfigEntryProcess(node);
      process.specificState = StorageProcessState::ENTER_CONFIG;
      break;
    case StorageProcessState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(process.cancelError);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = StorageProcessState::PROGRESS_CLEAR;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.cancelError);
      break;
    case StorageProcessState::PROGRESS_CLEAR:
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      process.specificState = StorageProcessState::PROGRESS_READ_PRE;
      break;
    case StorageProcessState::PROGRESS_READ_PRE:
      /* Stream active - second read should occur */
      process.specificState = StorageProcessState::PROGRESS_CHECK_PRE;
      break;
    case StorageProcessState::PROGRESS_CHECK_PRE:
      process.error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete);
      if      (process.error)                       cancelConfigProcess(process, process.error);
      else if (storeComplete == Atams::ATAMS_FALSE) process.specificState = StorageProcessState::CLEAR_PASSCODE;
      else                                          cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      break;
    case StorageProcessState::CLEAR_PASSCODE:
      static_cast<void>(node.clearAckStartWrite(passcodeVarID, static_cast<uint32_t>(0U)));
      process.specificState = StorageProcessState::WRITE_PASSCODE;
      break;
    case StorageProcessState::WRITE_PASSCODE:
      static_cast<void>(node.write(passcodeVarID, passcode));
      process.specificState = StorageProcessState::STATUS_STREAM;
      break;
    case StorageProcessState::STATUS_STREAM:
      static_cast<void>(node.stopStream(passcodeVarID));
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_STORAGE_STATUS));
      process.prevEventTime = currentTime;
      process.specificState = StorageProcessState::STATUS_CHECK_POST;
      break;
    case StorageProcessState::STATUS_CHECK_POST:
      static_cast<void>(node.readIfDataReady(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete));
      static_cast<void>(node.readIfDataReady(BlockUniversal::VAR_ID_STORAGE_STATUS,           storageStatus));
      if (storeComplete == Atams::ATAMS_TRUE)
      {
        static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
        static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_STORAGE_STATUS));
        if (storageStatus == Atams::ERROR_NONE) process.specificState = StorageProcessState::BEGIN_EXIT_CONFIG;
        else                                    cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      }
      else if ((currentTime - process.prevEventTime) > Platform::NVM_STORAGE_TIMEOUT)
      {
        static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
        static_cast<void>(node.stopStream(BlockUniversal::VAR_ID_STORAGE_STATUS));
        cancelConfigProcess(process, Atams::ERROR_PROCESS_TIMEOUT);
      }
      break;
    case StorageProcessState::CANCEL_CONFIG:
      updateCancelConfigState(process);   
      break;    
    case StorageProcessState::BEGIN_EXIT_CONFIG:
      beginConfigExitProcess(node, false);
      process.specificState = StorageProcessState::EXIT_CONFIG;
      break;
    case StorageProcessState::EXIT_CONFIG:
      subProcessState = updateConfigurationStateExit(process.cancelError);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.cancelError);   
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

Atams::ProcessState NodeActions::updateValidateMultipleConfig(Atams::Error_t            &error, 
                                                              bool                      &allVarsValid, 
                                                              ValidateMultiConfigFtns_t &specificFunctions)
{
  NodeActions::ProcessHandler<ValidateMultiConfigState> &process = validateMultiConfigProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node &node = *process.nodePtr;

  switch (process.specificState)
  {
    case ValidateMultiConfigState::START:
      process.specificState = ValidateMultiConfigState::COLLECT;
      break;
    case ValidateMultiConfigState::COLLECT:
      specificFunctions.readFunction(node);
      process.specificState = ValidateMultiConfigState::VALIDATE;
      break;
    case ValidateMultiConfigState::VALIDATE:
      process.error = specificFunctions.checkFunction(*this, node, allVarsValid);
      if (process.error) process.terminate(error);
      else               process.setProcessComplete();
      break; 
    case ValidateMultiConfigState::COMPLETE:
    case ValidateMultiConfigState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;
  
  return (process.processState);
}

Atams::ProcessState NodeActions::updateSetMultipleConfig(Atams::Error_t &error, SetMultiConfigFtns_t &specificFunctions)
{
  NodeActions::ProcessHandler<SetMultiConfigState> &process = setMultiConfigProcess_;

  if (process.nullptrCheck())
  {
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState =  process.subProcessState;
  bool                 successFlag     =  false;      

  switch (process.specificState)
  {
    case SetMultiConfigState::START:
      static_cast<void>(beginConfigEntryProcess(*process.nodePtr));
      process.specificState = SetMultiConfigState::ENTER_CONFIG;
      break;
    case SetMultiConfigState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = SetMultiConfigState::WRITE;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);
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
      static_cast<void>(beginConfigExitProcess(node, true));
      process.specificState = SetMultiConfigState::APPLY_CONFIG;
      break;
    case SetMultiConfigState::APPLY_CONFIG:
      subProcessState = updateConfigurationStateExit(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.specificState = SetMultiConfigState::READ;
      else if (subProcessState == Atams::ProcessState::ERROR)    process.terminate(process.error);   
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

void NodeActions::validateGenInfoRead(Node &node)
{
  for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
       varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
       varID++)
  {
    static_cast<void>(node.clearDataReadyStartRead(varID));
  }
}

void NodeActions::validateBusIDsRead(Node &node)
{
  for (uint8_t varID = BlockUniversal::VAR_ID_FIRST_NODE_ID; 
       varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; 
       varID++)
  {
    static_cast<void>(node.clearDataReadyStartRead(varID));
  }
}

Atams::Error_t NodeActions::validateGenInfoCheck(NodeActions &processHandler, Node &node, bool &genInfoIsValid)
{
  static_cast<void>(processHandler);

  NodeCallbackHandler &callbackHandler = node;
  Atams::Error_t       error           = Atams::ERROR_NONE;
  bool                 newDataReady    = false;

  for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
       varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
       varID++)
  {
    static_cast<void>(node.stopStreamGetDataReady(varID, newDataReady)); 
    if (!newDataReady) error = Atams::ERROR_NEW_DATA_NOT_READY;
  }

  if (!error) genInfoIsValid = callbackHandler.validateGenInfo();
  
  return (error);
}

Atams::Error_t NodeActions::validateBusIDsCheck(NodeActions &processHandler, Node &node, bool &allIDsValid)
{
  Atams::Error_t error       = Atams::ERROR_NONE;
  uint8_t        firstNodeID = 0U;
  uint8_t        lastNodeID  = 0U;
  uint8_t        prevNodeID  = 0U;

  allIDsValid = false;

  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_FIRST_NODE_ID,    firstNodeID);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_LAST_NODE_ID,     lastNodeID);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, prevNodeID);

  if (!error) allIDsValid = ((firstNodeID == processHandler.busIDsToSet_.firstNodeID   ) &&
                             (lastNodeID  == processHandler.busIDsToSet_.lastNodeID    ) &&
                             (prevNodeID  == processHandler.busIDsToSet_.previousNodeID) );

  return (error);
}

void NodeActions::setUserConfigWrite(NodeActions &processHandler, Node &node)
{
  NodeUserConfig_t &userConfig = processHandler.userConfigToSet_;

  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_NODE_ID,         userConfig.nodeID));
  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_BITRATE,         static_cast<uint8_t>(userConfig.bitrateOption)));
  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, userConfig.watchdogPeriod));
}

bool NodeActions::setUserConfigGetAck(Node &node)
{
  bool ackReceived = true;

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_NODE_ID,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_BITRATE,         ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, ackReceived));

  return (ackReceived);
}

void NodeActions::setUserConfigRead(Atams::Node &node)
{
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_NODE_ID));
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_BITRATE));
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_WATCHDOG_PERIOD));
}

Atams::Error_t NodeActions::setUserConfigCheck(NodeActions &processHandler, Atams::Node &node, bool &userConfigIsValid)
{
  Atams::Error_t   error = Atams::ERROR_NONE;
  NodeUserConfig_t collectedConfig;
  uint8_t          bitrateRaw;

  userConfigIsValid = false;

  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_NODE_ID,         collectedConfig.nodeID);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_BITRATE,         bitrateRaw);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_WATCHDOG_PERIOD, collectedConfig.watchdogPeriod);

  if (!error) 
  {
    collectedConfig.bitrateOption = static_cast<Atams::BitrateOption_t>(bitrateRaw);
    userConfigIsValid = (collectedConfig == processHandler.userConfigToSet_);
  }

  return (error);
}

void NodeActions::setBusIDsWrite(NodeActions &processHandler, Atams::Node &node)
{
  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_FIRST_NODE_ID,    processHandler.busIDsToSet_.firstNodeID));
  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_LAST_NODE_ID,     processHandler.busIDsToSet_.lastNodeID));
  static_cast<void>(node.clearAckStartWrite(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, processHandler.busIDsToSet_.previousNodeID));
}

bool NodeActions::setBusIDsGetAck(Node &node)
{
  bool ackReceived = true;

  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_FIRST_NODE_ID,    ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_LAST_NODE_ID,     ackReceived));
  if (ackReceived) static_cast<void>(node.stopStreamGetWriteAck(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, ackReceived));

  return (ackReceived);
}

void NodeActions::setBusIDsRead(Atams::Node &node)
{
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_FIRST_NODE_ID));
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_LAST_NODE_ID));
  static_cast<void>(node.clearDataReadyStartRead(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID));
}

Atams::Error_t NodeActions::setBusIDsCheck(NodeActions &processHandler, Atams::Node &node, bool &allIDsValid)
{
  Atams::Error_t error = Atams::ERROR_NONE;
  BusIDs_t       collectedBusIDs;

  allIDsValid = false;

  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_FIRST_NODE_ID,    collectedBusIDs.firstNodeID);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_LAST_NODE_ID,     collectedBusIDs.lastNodeID);
  if (!error) error = node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, collectedBusIDs.previousNodeID);

  if (!error) allIDsValid = (collectedBusIDs == processHandler.busIDsToSet_);
  
  return (error);
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
  this->processState  = Atams::ProcessState::IN_PROGRESS;
  this->nodePtr->resetRequestPacket();
}

template <typename T>
void NodeActions::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  this->error         = exitError;
  this->specificState = T::ERROR;
  this->processState  = Atams::ProcessState::ERROR;
  if (this->nodePtr != nullptr) this->nodePtr->resetRequestPacket();
}

template <typename T>
void NodeActions::ProcessHandler<T>::setProcessComplete(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState  = Atams::ProcessState::COMPLETE;
  if (this->nodePtr != nullptr) this->nodePtr->resetRequestPacket();
}

template <typename T>
bool NodeActions::ProcessHandler<T>::getProcessTerminated(void)
{
  return (this->processState == Atams::ProcessState::ERROR);
}

template <typename T>
bool NodeActions::ProcessHandler<T>::nullptrCheck(void)
{
  bool ptrIsNull = false;

  if (this->nodePtr == nullptr) 
  {
    this->terminate(Atams::ERROR_NULLPTR);
    ptrIsNull = true;
  }

  return (ptrIsNull);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
