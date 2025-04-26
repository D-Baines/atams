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
#include "../Maps/BlockUniversal.hpp"
#include "../Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

NodeActions::NodeActions(Node &node) :
node_(node),
validateGenInfoProcess_(node),
collectNodeIDsProcess_(node),
configEntryProcess_(node),
configExitProcess_(node),
setConfigVarProcess_(node),
storageProcess_(node),
resetNodeProcess_(node)
{
  /* Do Nothing */
}

NodeActions::~NodeActions(void)
{
  /* Do Nothing */
}

void NodeActions::beginValidateGenInfoProcess(void)
{
  validateGenInfoProcess_.resetProcess();
}

void NodeActions::beginCollectNodeIDsProcess(void)
{
  collectNodeIDsProcess_.resetProcess();
}

void NodeActions::beginConfigEntryProcess(void)
{
  configEntryProcess_.resetProcess();
}

void NodeActions::beginConfigExitProcess(void)
{
  configExitProcess_.resetProcess();
}

void NodeActions::beginSetConfigVarProcess(void)
{
  setConfigVarProcess_.resetProcess();
}

void NodeActions::beginStorageProcess(void)
{
  storageProcess_.resetProcess();
}

void NodeActions::beginResetNodeProcess(void)
{
  resetNodeProcess_.resetProcess();
}

Atams::ProcessState_t NodeActions::updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid)
{
  NodeActions::ProcessHandler<ValidateGenInfoState> &process = validateGenInfoProcess_;

  switch (process.specificState)
  {
    case ValidateGenInfoState::START:
      process.specificState = ValidateGenInfoState::COLLECT;
      break;

    case ValidateGenInfoState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
           varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
           varID++)
      {
        static_cast<void>(node_.clearNewDataFlag(varID));
        static_cast<void>(node_.startReadStream(varID));
      }
      process.specificState = ValidateGenInfoState::VALIDATE;
      break;

    case ValidateGenInfoState::VALIDATE:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
           varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
           varID++)
      {
        bool newDataReady = false;
        static_cast<void>(node_.stopStream(varID));
        static_cast<void>(node_.getNewDataFlag(varID, newDataReady));
        if (!newDataReady) process.terminate(Atams::ERROR_NEW_DATA_NOT_READY);
      }
      if (process.getProcessTerminated() == false)
      {
        genInfoIsValid = node_.validateGenInfo();
        process.setProcessComplete();
      }
      break; 
    case ValidateGenInfoState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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

Atams::ProcessState_t NodeActions::updateCollectNodeIdentifiers(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<CollectNodeIDsState> &process = collectNodeIDsProcess_;

  switch (process.specificState)
  {
    case CollectNodeIDsState::START:
      process.specificState = CollectNodeIDsState::COLLECT;
      break;
    case CollectNodeIDsState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; 
           varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; 
           varID++)
      {
        static_cast<void>(node_.clearNewDataFlag(varID));
        static_cast<void>(node_.startReadStream(varID));
      }
      process.specificState = CollectNodeIDsState::CHECK_NEW;
      break;
    case CollectNodeIDsState::CHECK_NEW:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; 
           varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; 
           varID++)
      {
        bool newDataReady = false;
        static_cast<void>(node_.stopStream(varID));
        static_cast<void>(node_.getNewDataFlag(varID, newDataReady));
        if (newDataReady == false) process.terminate(Atams::ERROR_NEW_DATA_NOT_READY);
      }
      process.specificState = CollectNodeIDsState::COMPLETE;
      break;
    case CollectNodeIDsState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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

Atams::ProcessState_t NodeActions::updateConfigurationStateEntry(Atams::Error_t &error)
{
  NodeActions::ProcessHandler<ConfigEntryState> &process = configEntryProcess_;

  Atams::ConfigurationStatus_t configState = Atams::CONFIGURATION_STATUS_INACTIVE;

  switch (process.specificState)
  {
    case ConfigEntryState::START:
      process.specificState = ConfigEntryState::COLLECT_STATUS_PRE;
      break;
    case ConfigEntryState::COLLECT_STATUS_PRE:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_PRE;
      break;
    case ConfigEntryState::CHECK_STATUS_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState));
      if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) process.setProcessComplete();
      else                                                   process.specificState = ConfigEntryState::WRITE_PASSCODE;
      break; 
    case ConfigEntryState::WRITE_PASSCODE:
      static_cast<void>(node_.write(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, Atams::CONFIGURATION_PASSKEY_ACCESS));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      process.specificState = ConfigEntryState::COLLECT_STATUS_POST;
      break;
    case ConfigEntryState::COLLECT_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigEntryState::CHECK_STATUS_POST;
      break;
    case ConfigEntryState::CHECK_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if      (configState == Atams::CONFIGURATION_STATUS_ACTIVE)   process.specificState = ConfigEntryState::COMPLETE;
      else if (configState == Atams::CONFIGURATION_STATUS_DENIED)   process.terminate(Atams::ERROR_CONFIGURATION_STATE_DENIED);
      else if (configState == Atams::CONFIGURATION_STATUS_INACTIVE) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      break;
    case ConfigEntryState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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

Atams::ProcessState_t NodeActions::updateConfigurationStateExit(Atams::Error_t &error, bool applyChanges)
{
  NodeActions::ProcessHandler<ConfigExitState> &process = configExitProcess_;

  uint8_t  configState   = Atams::CONFIGURATION_STATUS_ACTIVE;
  uint32_t configPasskey = applyChanges ? Atams::CONFIGURATION_PASSKEY_APPLY : Atams::CONFIGURATION_PASSKEY_CANCEL; 

  switch (process.specificState)
  {
    case ConfigExitState::START:
      process.specificState = ConfigExitState::COLLECT_STATUS_PRE;
      break;
    case ConfigExitState::COLLECT_STATUS_PRE:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_PRE;
      break;
    case ConfigExitState::CHECK_STATUS_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState));
      if ((configState == Atams::CONFIGURATION_STATUS_INACTIVE) ||
          (configState == Atams::CONFIGURATION_STATUS_DENIED  ) ||
          (configState == Atams::CONFIGURATION_STATUS_APPLIED ) ) process.terminate(Atams::ERROR_CONFIGURATION_STATE_INACTIVE);
      else                                                        process.specificState = ConfigExitState::WRITE_PASSCODE;           
      break; 
    case ConfigExitState::WRITE_PASSCODE:
      static_cast<void>(node_.write(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, configPasskey));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      process.specificState = ConfigExitState::COLLECT_STATUS_POST;
      break;
    case ConfigExitState::COLLECT_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      process.specificState = ConfigExitState::CHECK_STATUS_POST;
      break;
    case ConfigExitState::CHECK_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState));
      if      ((configState   == Atams::CONFIGURATION_STATUS_INACTIVE) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_CANCEL ) ) process.setProcessComplete();
      else if ((configState   == Atams::CONFIGURATION_STATUS_APPLIED) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_APPLY ) )  process.setProcessComplete();
      else process.terminate(Atams::ERROR_CONFIGURATION_EXIT);
      break;
    case ConfigExitState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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

Atams::ProcessState_t NodeActions::updateSetNodeID(Atams::Error_t &error, const uint8_t nodeID)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_NODE_ID, nodeID));
}

Atams::ProcessState_t NodeActions::updateSetBitrate(Atams::Error_t &error, Atams::BitrateOption_t bitrateOption)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_BITRATE, static_cast<uint8_t>(bitrateOption)));
}

Atams::ProcessState_t NodeActions::updateSetWatchdogPeriod(Atams::Error_t &error, uint32_t watchdogPeriod)
{
  return (updateSetConfigVar(error, BlockUniversal::VAR_ID_WATCHDOG_PERIOD, watchdogPeriod));
}

Atams::ProcessState_t NodeActions::updateStoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_STORE_ALL, Atams::STORE_ALL_PASSCODE));
}

Atams::ProcessState_t NodeActions::updateRestoreAll(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_RESTORE_ALL, Atams::RESTORE_ALL_PASSCODE));
}

Atams::ProcessState_t NodeActions::updateRestoreUserBlocks(Atams::Error_t &error)
{
  return(updateStorageProcess(error, BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS, Atams::RESTORE_USER_BLOCKS_PASSCODE));
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

template<typename T>
void NodeActions::cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error)
{
  process.specificState = T::CANCEL_CONFIG;
  process.minorError    = error;
}

template<typename T>
void NodeActions::updateCancelConfigState(ProcessHandler<T> &process)
{
  process.subProcessState = updateConfigurationStateExit(process.error, false);
  if      (process.subProcessState == Atams::PROCESS_STATE_COMPLETE) process.terminate(process.minorError);
  else if (process.subProcessState == Atams::PROCESS_STATE_ERROR)    process.terminate(process.error);       
}

template<typename T>
Atams::ProcessState_t NodeActions::updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value)
{
  NodeActions::ProcessHandler<SetConfigVarState> &process = setConfigVarProcess_;

  Atams::ProcessState_t &subProcessState = process.subProcessState;
  bool                   ackReceived     = false;
  T                      readConfigVar;

  switch (process.specificState)
  {
    case SetConfigVarState::START:
      process.specificState = SetConfigVarState::ENTER_CONFIG;
      break;
    case SetConfigVarState::ENTER_CONFIG:
    subProcessState = updateConfigurationStateEntry(process.minorError);
      if      (subProcessState == Atams::PROCESS_STATE_COMPLETE) process.specificState = SetConfigVarState::WRITE;
      else if (subProcessState == Atams::PROCESS_STATE_ERROR)    process.terminate(process.minorError);
      break;
    case SetConfigVarState::WRITE:
      static_cast<void>(node_.clearAckFlag(BlockUniversal::VAR_ID_NODE_ID));
      static_cast<void>(node_.write(varID, value));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_NODE_ID));
      process.specificState = SetConfigVarState::READ;
      break;
    case SetConfigVarState::CHECK_ACK:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_NODE_ID));
      static_cast<void>(node_.getAckFlag(BlockUniversal::VAR_ID_NODE_ID, ackReceived));
      if (!ackReceived) cancelConfigProcess(process, Atams::ERROR_ACK_NOT_RECEIVED);
      else              process.specificState = SetConfigVarState::APPLY_CONFIG;
      break;
    case SetConfigVarState::CANCEL_CONFIG:
      updateCancelConfigState(process);
      break;    
    case SetConfigVarState::APPLY_CONFIG:
      subProcessState = updateConfigurationStateExit(process.minorError, true);
      if      (subProcessState == Atams::PROCESS_STATE_COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::PROCESS_STATE_ERROR)    process.terminate(process.minorError);   
      break;
    case SetConfigVarState::READ:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_NODE_ID));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_NODE_ID));
      process.specificState = SetConfigVarState::CHECK_VALUE;
      break;
    case SetConfigVarState::CHECK_VALUE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_NODE_ID));
      process.minorError = node_.readIfNew(BlockUniversal::VAR_ID_NODE_ID, readConfigVar);
      if      (process.minorError)     process.terminate(process.minorError);
      else if (readConfigVar == value) process.setProcessComplete();
      else                             process.terminate(Atams::ERROR_SET_CONFIG_VAR_FAILED);
      break;
    case SetConfigVarState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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
  NodeActions::ProcessHandler<StorageProcessState> &process = storageProcess_;

  Atams::ProcessState_t &subProcessState = process.subProcessState;
  uint8_t                storeComplete   = Atams::ATAMS_FALSE;
  uint8_t                storageStatus   = Atams::ERROR_STORAGE_PROCESS_FAILED;
  Atams::Error_t         cancelError     = Atams::ERROR_NONE;
  uint32_t               currentTime     = Platform::getMillis();

  switch (process.specificState)
  {
    case StorageProcessState::START:
      process.specificState = StorageProcessState::ENTER_CONFIG;
      break;
    case StorageProcessState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(process.minorError);
      if      (subProcessState == Atams::PROCESS_STATE_COMPLETE) process.specificState = StorageProcessState::PROGRESS_CLEAR;
      else if (subProcessState == Atams::PROCESS_STATE_ERROR)    process.terminate(process.minorError);
      break;
    case StorageProcessState::PROGRESS_CLEAR:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      process.specificState = StorageProcessState::PROGRESS_READ_PRE;
      break;
    case StorageProcessState::PROGRESS_READ_PRE:
      /* Stream active - second read should occur */
      process.specificState = StorageProcessState::PROGRESS_CHECK_PRE;
      break;
    case StorageProcessState::PROGRESS_CHECK_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      cancelError = node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete);
      if      (cancelError)                         cancelConfigProcess(process, cancelError);
      else if (storeComplete == Atams::ATAMS_FALSE) process.specificState = StorageProcessState::WRITE_PASSCODE;
      else                                          cancelConfigProcess(process, Atams::ERROR_STORAGE_PROCESS_FAILED);
      break;
    case StorageProcessState::WRITE_PASSCODE:
      static_cast<void>(node_.write(passcodeVarID, passcode));
      static_cast<void>(node_.startWriteStream(passcodeVarID));
      process.specificState = StorageProcessState::STATUS_STREAM;
      break;
    case StorageProcessState::STATUS_STREAM:
      static_cast<void>(node_.stopStream(passcodeVarID));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_STATUS));
      process.prevEventTime = currentTime;
      process.specificState = StorageProcessState::STATUS_CHECK_POST;
      break;
    case StorageProcessState::STATUS_CHECK_POST:
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete));
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_STATUS,           storageStatus));
      if (storeComplete == Atams::ATAMS_TRUE)
      {
        if (storageStatus == Atams::ERROR_NONE) process.specificState = StorageProcessState::EXIT_CONFIG;
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
    case StorageProcessState::EXIT_CONFIG:
    subProcessState = updateConfigurationStateExit(process.minorError, false);
      if      (subProcessState == Atams::PROCESS_STATE_COMPLETE) process.setProcessComplete();
      else if (subProcessState == Atams::PROCESS_STATE_ERROR)    process.terminate(process.minorError);   
      break;
    case StorageProcessState::COMPLETE:
      /* Do Nothing - Transitions handled by ProcessHandler */
      break;
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

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
void NodeActions::ProcessHandler<T>::terminate(Atams::Error_t exitError)
{
  this->error         = exitError;
  this->specificState = T::ERROR;
  this->processState  = Atams::PROCESS_STATE_ERROR;
  this->node.resetRequestPacket();
}

template <typename T>
void NodeActions::ProcessHandler<T>::setProcessComplete(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::COMPLETE;
  this->processState  = Atams::PROCESS_STATE_COMPLETE;
  this->node.resetRequestPacket();
}

template <typename T>
bool NodeActions::ProcessHandler<T>::getProcessTerminated(void)
{
  return (this->processState == Atams::PROCESS_STATE_ERROR);
}

template <typename T>
void NodeActions::ProcessHandler<T>::resetProcess(void)
{
  this->error         = Atams::ERROR_NONE;
  this->specificState = T::START;
  this->processState  = Atams::PROCESS_STATE_IN_PROGRESS;
  this->node.resetRequestPacket();
}


} /* End Namespace - Atams */


//Node node1;
//Node node2;
//
//NodeActions nodeArray[2] = {NodeProcesses(node1), NodeProcesses(node2)};
//
//Atams::ProcessState_t processState = Atams::PROCESS_STATE_IN_PROGRESS;
//
//bool allProcessesComplete = false;
//
//for (NodeProcesses &nodeProcesses : nodeArray)
//{
//  /* This sets the process state of each of the state machines to start */
//  /* The process state machines will no longer go back to the start automatically*/
//  nodeProcesses.beginProcess();
//}
//
//while (allProcessesComplete == false)
//{
//  allProcessesComplete = true;
//  
//  for (NodeProcesses &nodeProcesses : nodeArray)
//  {
//    if (nodeProcesses->updateValidateGenInfo(error, genInfoIsValid) == Atams::PROCESS_STATE_IN_PROGRESS)
//    {
//      allProcessesComplete = false;
//    }
//  }
//}


//Check error if any process is in error state


/**
  * @}End of File
  */
