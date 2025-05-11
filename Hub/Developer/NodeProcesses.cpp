/**
  ******************************************************************************
  * @file    NodeProcesses.cpp
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

#include "NodeProcesses.hpp"
#include "../Node.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

NodeActions::NodeActions(void)
{
  /* Do Nothing */
}

NodeActions::~NodeActions(void)
{
  /* Do Nothing */
}

void NodeActions::beginValidateGenInfoProcess(Node *node)
{
  if (node != nullptr) 
  {
    validateGenInfoProcess_.assignNode(node);
    validateGenInfoProcess_.resetProcess();
  }
}

void NodeActions::beginCollectNodeIDsProcess(Node *node)
{
  if (node != nullptr) 
  {
    collectNodeIDsProcess_.assignNode(node);
    collectNodeIDsProcess_.resetProcess();
  }
}

void NodeActions::beginConfigEntryProcess(Node *node)
{
  if (node != nullptr) 
  {
    configEntryProcess_.assignNode(node);
    configEntryProcess_.resetProcess();
  }
}

void NodeActions::beginConfigExitProcess(Node *node, bool applyChangesOnExit)
{
  if (node != nullptr) 
  {
    applyChanges_ = applyChangesOnExit;
    configExitProcess_.assignNode(node);
    configExitProcess_.resetProcess();
  }
}

void NodeActions::beginSetNodeID(Node *node, const uint8_t nodeID)
{
  if (node != nullptr) 
  {
    nodeIDToSet_ = nodeID;
    setConfigVarProcess_.assignNode(node);
    setConfigVarProcess_.resetProcess();
  }
}

void NodeActions::beginSetBitrate(Node *node, const Atams::BitrateOption_t bitrateOption)
{
  if (node != nullptr) 
  {
    bitrateOptionToSet_ = bitrateOption;
    setConfigVarProcess_.assignNode(node);
    setConfigVarProcess_.resetProcess();
  }
}

void NodeActions::beginSetWatchdogPeriod(Node *node, const uint32_t watchdogPeriod)
{
  if (node != nullptr) 
  {
    watchdogPeriodToSet_ = watchdogPeriod;
    setConfigVarProcess_.assignNode(node);
    setConfigVarProcess_.resetProcess();
  }
}

void NodeActions::beginStorageProcess(Node *node)
{
  if (node != nullptr) 
  {
    storageProcess_.assignNode(node);
    storageProcess_.resetProcess();
  }
}

void NodeActions::beginResetNodeProcess(Node *node)
{
  if (node != nullptr) 
  {
    resetNodeProcess_.assignNode(node);
    resetNodeProcess_.resetProcess();
  }
}

Atams::ProcessState NodeActions::updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid)
{
  NodeActions::ProcessHandler<ValidateGenInfoState> &process = validateGenInfoProcess_;

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
    return (process.processState); /* Early Return */
  }

  switch (process.specificState)
  {
    case ValidateGenInfoState::START:
      process.specificState = ValidateGenInfoState::COLLECT;
      break;
    case ValidateGenInfoState::COLLECT:
      startReadGenInfo(*process.nodePtr);
      process.specificState = ValidateGenInfoState::VALIDATE;
      break;
    case ValidateGenInfoState::VALIDATE:
      process.error = validateCollectedGenInfo(*process.nodePtr, genInfoIsValid);
      if (process.error) process.terminate(error);
      else               process.setProcessComplete();
      break; 
    case ValidateGenInfoState::COMPLETE:
    case ValidateGenInfoState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;
  
  return (process.processState);
}

Atams::ProcessState NodeActions::updateCollectNodeIDs(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<CollectNodeIDsState> &process = collectNodeIDsProcess_;

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
    return (process.processState); /* Early Return */
  }

  switch (process.specificState)
  {
    case CollectNodeIDsState::START:
      process.specificState = CollectNodeIDsState::COLLECT;
      break;
    case CollectNodeIDsState::COLLECT:
      startReadAllNodeIDs(*process.nodePtr);
      process.specificState = CollectNodeIDsState::CHECK_NEW;
      break;
    case CollectNodeIDsState::CHECK_NEW:
      process.error = validateReadAllNodeIDs(*process.nodePtr);
      if (process.error) process.terminate(error);
      else               process.setProcessComplete();
      break;
    case CollectNodeIDsState::COMPLETE:
    case CollectNodeIDsState::ERROR:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
    default:
      process.terminate(Atams::ERROR_INVALID_CASE);
      break;
  }

  error = process.error;

  return (process.processState);
}

Atams::ProcessState NodeActions::updateConfigurationStateEntry(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigEntryState> &process = configEntryProcess_;

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
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
      else if (configState == Atams::CONFIGURATION_STATUS_INACTIVE) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
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

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
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

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState = process.subProcessState;
  bool                 ackReceived     = false;

  switch (process.specificState)
  {
    case ResetNodeState::START:
      beginConfigEntryProcess(process.nodePtr);
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

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

template<typename T>
void NodeActions::cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error)
{
  process.specificState = T::CANCEL_CONFIG;
  process.cancelError   = error;
  beginConfigExitProcess(process.nodePtr, false);
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

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
    return (process.processState); /* Early Return */
  }

  Node                &node            = *process.nodePtr;
  Atams::ProcessState &subProcessState = process.subProcessState;
  bool                 ackReceived     = false;
  T                    readConfigVar;

  switch (process.specificState)
  {
    case SetConfigVarState::START:
      beginConfigEntryProcess(process.nodePtr);
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
      beginConfigExitProcess(process.nodePtr, true);
      process.specificState = SetConfigVarState::APPLY_CONFIG;
      break;
    case SetConfigVarState::APPLY_CONFIG:
      subProcessState = updateConfigurationStateExit(process.error);
      if      (subProcessState == Atams::ProcessState::COMPLETE) process.setProcessComplete();
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

  if (process.nodePtr == nullptr) 
  {
    process.terminate(Atams::ERROR_NULLPTR);
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
      beginConfigEntryProcess(process.nodePtr);
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
      static_cast<void>(node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete));
      static_cast<void>(node.stopStreamReadIfDataReady(BlockUniversal::VAR_ID_STORAGE_STATUS,           storageStatus));
      if (storeComplete == Atams::ATAMS_TRUE)
      {
        if (storageStatus == Atams::ERROR_NONE) process.specificState = StorageProcessState::BEGIN_EXIT_CONFIG;
        else                                    cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      }
      else if (currentTime - process.prevEventTime > NodeActions::STORAGE_TIMEOUT)
      {
        cancelConfigProcess(process, Atams::ERROR_PROCESS_TIMEOUT);
      }
      break;
    case StorageProcessState::CANCEL_CONFIG:
      updateCancelConfigState(process);   
      break;    
    case StorageProcessState::BEGIN_EXIT_CONFIG:
      beginConfigExitProcess(process.nodePtr, false);
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

void NodeActions::startReadGenInfo(Node &node)
{
  for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
       varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
       varID++)
  {
   static_cast<void>(node.clearDataReadyStartRead(varID));
  }
}

Atams::Error_t NodeActions::validateCollectedGenInfo(Node &node, bool &genInfoIsValid)
{
  Atams::Error_t error        = Atams::ERROR_NONE;
  bool           newDataReady = false;

  for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
       varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
       varID++)
  {
    static_cast<void>(node.stopStreamGetDataReady(varID, newDataReady)); 
    if (!newDataReady) error = Atams::ERROR_NEW_DATA_NOT_READY;
  }

  if (!error) genInfoIsValid = node.validateGenInfo();
  
  return (error);
}

void NodeActions::startReadAllNodeIDs(Node &node)
{
  for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
       varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
       varID++)
  {
    static_cast<void>(node.clearDataReadyStartRead(varID));
  }
}

Atams::Error_t NodeActions::validateReadAllNodeIDs(Node &node)
{
  Atams::Error_t error        = Atams::ERROR_NONE;
  bool           newDataReady = false;

  for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; 
       varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; 
       varID++)
  {
    static_cast<void>(node.stopStreamGetDataReady(varID, newDataReady));
    if (!newDataReady) error = Atams::ERROR_NEW_DATA_NOT_READY;
  }

  return (error);
}


/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
Atams::Error_t NodeActions::ProcessHandler<T>::assignNode(Node *node)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (node == nullptr) statusReturn  = Atams::ERROR_NULLPTR;
  else                 this->nodePtr = node;
  
  return (statusReturn);
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
void NodeActions::ProcessHandler<T>::resetProcess(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::START;
  this->processState  = Atams::ProcessState::IN_PROGRESS;
  if (this->nodePtr != nullptr) this->nodePtr->resetRequestPacket();
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
