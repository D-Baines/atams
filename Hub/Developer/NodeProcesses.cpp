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
storageProcess_(node)
{
  /* Do Nothing */
}

NodeActions::~NodeActions(void)
{
  /* Do Nothing */
}

Atams::ProcessState_t NodeActions::updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  bool                  newDataReady  = false;

  switch (validateGenInfoProcess_.minorState_)
  {
    case ValidateGenInfoState::START:
      node_.resetRequestPacket();
      validateGenInfoProcess_.minorState = ValidateGenInfoState::COLLECT;
      break;

    case ValidateGenInfoState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
           varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
           varID++)
      {
        static_cast<void>(node_.clearNewDataFlag(varID));
        static_cast<void>(node_.startReadStream(varID));
      }
      validateGenInfoProcess_.minorState = ValidateGenInfoState::VALIDATE;
      break;

    case ValidateGenInfoState::VALIDATE:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; 
           varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; 
           varID++)
      {
        static_cast<void>(node_.stopStream(varID));
        static_cast<void>(node_.isNewDataReady(varID, newDataReady));
        if (!newDataReady) validateGenInfoProcess_.endProcess(Atams::ERROR_NEW_DATA_NOT_READY);
      }
      if (!error)
      {
        genInfoIsValid        = node_.validateGenInfo();
        validateGenInfoProcess_ = ValidateGenInfoState::COMPLETE;
      }
      break; 
    case ValidateGenInfoState::COMPLETE:
      processReturn = Atams::PROCESS_STATE_COMPLETE;
      break;
    case ValidateGenInfoState::ERROR:
      processReturn = Atams::PROCESS_STATE_ERROR;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error) processReturn  = Atams::PROCESS_STATE_ERROR;

  return (processReturn);
}

Atams::ProcessState_t NodeActions::updateCollectNodeIdentifiers(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  bool                  newDataReady  = false;

  switch (collectNodeIDsProcess_)
  {
    case CollectNodeIDsState::START:
      node_.resetRequestPacket();
      collectNodeIDsProcess_ = CollectNodeIDsState::COLLECT;
      break;
    case CollectNodeIDsState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; varID++)
      {
        static_cast<void>(node_.clearNewDataFlag(varID));
        static_cast<void>(node_.startReadStream(varID));
      }
      collectNodeIDsProcess_ = CollectNodeIDsState::CHECK_NEW;
      break;
    case CollectNodeIDsState::CHECK_NEW:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; varID++)
      {
        static_cast<void>(node_.stopStream(varID));
        static_cast<void>(node_.isNewDataReady(varID, newDataReady));
        if (newDataReady == false) error = Atams::ERROR_NEW_DATA_NOT_READY;
      }
      collectNodeIDsProcess_ = CollectNodeIDsState::COMPLETE;
      break;
    case CollectNodeIDsState::COMPLETE:
      node_.resetRequestPacket();
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      collectNodeIDsProcess_ = CollectNodeIDsState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    node_.resetRequestPacket();
    processReturn         = Atams::PROCESS_STATE_ERROR;
    collectNodeIDsProcess_ = CollectNodeIDsState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeActions::updateConfigurationStateEntry(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  uint8_t               configState   = Atams::CONFIGURATION_STATUS_INACTIVE;

  switch (configEntryProcess_)
  {
    case ConfigEntryState::START:
      node_.resetRequestPacket();
      configEntryProcess_ = ConfigEntryState::COLLECT_STATUS_PRE;
      break;
    case ConfigEntryState::COLLECT_STATUS_PRE:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      configEntryProcess_ = ConfigEntryState::CHECK_STATUS_PRE;
      break;
    case ConfigEntryState::CHECK_STATUS_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) configEntryProcess_ = ConfigEntryState::COMPLETE;
      else                                                   configEntryProcess_ = ConfigEntryState::WRITE_PASSCODE;
      break; 
    case ConfigEntryState::WRITE_PASSCODE:
      static_cast<void>(node_.write(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, Atams::CONFIGURATION_PASSKEY_ACCESS));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      configEntryProcess_ = ConfigEntryState::COLLECT_STATUS_POST;
      break;
    case ConfigEntryState::COLLECT_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      configEntryProcess_ = ConfigEntryState::CHECK_STATUS_POST;
      break;
    case ConfigEntryState::CHECK_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if (configState == Atams::CONFIGURATION_STATUS_ACTIVE) configEntryProcess_ = ConfigEntryState::COMPLETE;
      else                                                   error = Atams::ERROR_CONFIGURATION_ENTRY;
      break;
    case ConfigEntryState::COMPLETE:
      node_.resetRequestPacket();
      processReturn      = Atams::PROCESS_STATE_COMPLETE;
      configEntryProcess_ = ConfigEntryState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    node_.resetRequestPacket();
    processReturn      = Atams::PROCESS_STATE_ERROR;
    configEntryProcess_ = ConfigEntryState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeActions::updateConfigurationStateExit(Atams::Error_t &error, bool applyChanges)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  uint8_t  configState   = Atams::CONFIGURATION_STATUS_INACTIVE;
  uint32_t configPasskey = applyChanges ? Atams::CONFIGURATION_PASSKEY_APPLY : Atams::CONFIGURATION_PASSKEY_CANCEL; 

  switch (configExitProcess_)
  {
    case ConfigExitState::START:
      node_.resetRequestPacket();
      configExitProcess_ = ConfigExitState::COLLECT_STATUS_PRE;
      break;
    case ConfigExitState::COLLECT_STATUS_PRE:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      configExitProcess_ = ConfigExitState::CHECK_STATUS_PRE;
      break;
    case ConfigExitState::CHECK_STATUS_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if (error == Atams::ERROR_NONE)     
      {
        if ((configState == Atams::CONFIGURATION_STATUS_INACTIVE) ||
            (configState == Atams::CONFIGURATION_STATUS_DENIED  ) ||
            (configState == Atams::CONFIGURATION_STATUS_APPLIED ) ) configExitProcess_ = ConfigExitState::COMPLETE;
        else                                                        configExitProcess_ = ConfigExitState::WRITE_PASSCODE;
      }            
      break; 
    case ConfigExitState::WRITE_PASSCODE:
      static_cast<void>(node_.write(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, configPasskey));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      configExitProcess_ = ConfigExitState::COLLECT_STATUS_POST;
      break;
    case ConfigExitState::COLLECT_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      configExitProcess_ = ConfigExitState::CHECK_STATUS_POST;
      break;
    case ConfigExitState::CHECK_STATUS_POST:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = node_.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if      ((configState   == Atams::CONFIGURATION_STATUS_INACTIVE) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_CANCEL ) ) configExitProcess_ = ConfigExitState::COMPLETE;
      else if ((configState   == Atams::CONFIGURATION_STATUS_APPLIED) &&
               (configPasskey == Atams::CONFIGURATION_PASSKEY_APPLY ) )  configExitProcess_ = ConfigExitState::COMPLETE;
      else error = Atams::ERROR_CONFIGURATION_EXIT;
      break;
    case ConfigExitState::COMPLETE:
      node_.resetRequestPacket();
      processReturn     = Atams::PROCESS_STATE_COMPLETE;
      configExitProcess_ = ConfigExitState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    node_.resetRequestPacket();
    processReturn     = Atams::PROCESS_STATE_ERROR;
    configExitProcess_ = ConfigExitState::START;
  }

  return (processReturn);
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
Atams::ProcessState_t NodeActions::updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value)
{
  Atams::ProcessState_t processReturn   = Atams::PROCESS_STATE_IN_PROGRESS;
  Atams::ProcessState_t subProcessState = Atams::PROCESS_STATE_READY;
  Atams::Error_t        readError       = Atams::ERROR_NONE;
  T                     readConfigVar;

  switch (setConfigVarProcess_)
  {
    case SetConfigVarState::START:
      node_.resetRequestPacket();
      setConfigVarProcess_ = SetConfigVarState::ENTER_CONFIG;
      break;
    case SetConfigVarState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(error);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) setConfigVarProcess_ = SetConfigVarState::WRITE;
      break;
    case SetConfigVarState::WRITE:
      static_cast<void>(node_.write(varID, value));
      static_cast<void>(node_.startWriteStream(BlockUniversal::VAR_ID_NODE_ID));
      setConfigVarProcess_ = SetConfigVarState::READ;
      break;
    case SetConfigVarState::READ:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_NODE_ID));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_NODE_ID));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_NODE_ID));
      setConfigVarProcess_ = SetConfigVarState::CHECK;
      break;
    case SetConfigVarState::CHECK:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_NODE_ID));
      readError = node_.readIfNew(BlockUniversal::VAR_ID_NODE_ID, readConfigVar);
      if (readError)
      {
        setConfigVarProcess_ = SetConfigVarState::CANCEL_CONFIG;
      }
      else if (readConfigVar == value) 
      {
        setConfigVarProcess_ = SetConfigVarState::APPLY_CONFIG;
      }
      else 
      {
        readError = Atams::ERROR_SET_CONFIG_VAR_FAILED;
        setConfigVarProcess_ = SetConfigVarState::CANCEL_CONFIG;
      }
      break;
    case SetConfigVarState::CANCEL_CONFIG:
      subProcessState = updateConfigurationStateExit(error, false);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) error = readError;          
      break;    
    case SetConfigVarState::APPLY_CONFIG:
      subProcessState = updateConfigurationStateExit(error, true);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) setConfigVarProcess_ = SetConfigVarState::COMPLETE;
      break;
    case SetConfigVarState::COMPLETE:
      node_.resetRequestPacket();
      processReturn       = Atams::PROCESS_STATE_COMPLETE;
      setConfigVarProcess_ = SetConfigVarState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    node_.resetRequestPacket();
    processReturn       = Atams::PROCESS_STATE_ERROR;
    setConfigVarProcess_ = SetConfigVarState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeActions::updateStorageProcess(Atams::Error_t &error, const uint16_t varID, uint32_t passcode)
{
  Atams::ProcessState_t processReturn   = Atams::PROCESS_STATE_IN_PROGRESS;
  Atams::ProcessState_t subProcessState = Atams::PROCESS_STATE_READY;
  uint8_t               storeComplete   = Atams::ATAMS_FALSE;
  uint8_t               storageStatus   = Atams::ERROR_STORAGE_PROCESS_FAILED;
  Atams::Error_t        subError        = Atams::ERROR_NONE;
  uint32_t              currentTime     = Platform::getMillis();

  switch (storageProcess_)
  {
    case StorageProcessState::START:
      node_.resetRequestPacket();
      storageProcess_ = StorageProcessState::ENTER_CONFIG;
      break;
    case StorageProcessState::ENTER_CONFIG:
      subProcessState = updateConfigurationStateEntry(error);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) storageProcess_ = StorageProcessState::PROGRESS_CLEAR;
      break;
    case StorageProcessState::PROGRESS_CLEAR:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      storageProcess_ = StorageProcessState::PROGRESS_READ_PRE;
      break;
    case StorageProcessState::PROGRESS_READ_PRE:
      /* Stream active - second read should occur */
      break;
    case StorageProcessState::PROGRESS_CHECK_PRE:
      static_cast<void>(node_.stopStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      subError = node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete);
      if      (subError)                            storageProcess_ = StorageProcessState::CANCEL_CONFIG;
      else if (storeComplete == Atams::ATAMS_FALSE) storageProcess_ = StorageProcessState::WRITE_PASSCODE;
      else                                          storageProcess_ = StorageProcessState::CANCEL_CONFIG;
      break;
    case StorageProcessState::WRITE_PASSCODE:
      static_cast<void>(node_.write(varID, passcode));
      static_cast<void>(node_.startWriteStream(varID));
      storageProcess_ = StorageProcessState::STATUS_STREAM;
      break;
    case StorageProcessState::STATUS_STREAM:
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.clearNewDataFlag(BlockUniversal::VAR_ID_STORAGE_STATUS));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE));
      static_cast<void>(node_.startReadStream(BlockUniversal::VAR_ID_STORAGE_STATUS));
      storageRequestedTime_ = currentTime;
      storageProcess_  = StorageProcessState::STATUS_CHECK_POST;
      break;
    case StorageProcessState::STATUS_CHECK_POST:
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE, storeComplete));
      static_cast<void>(node_.readIfNew(BlockUniversal::VAR_ID_STORAGE_STATUS,           storageStatus));
      if (storeComplete == Atams::ATAMS_TRUE)
      {
        if (storageStatus == Atams::ERROR_NONE) storageProcess_ = StorageProcessState::APPLY_CONFIG;
        else                                    storageProcess_ = StorageProcessState::CANCEL_CONFIG;
      }
      else if (currentTime - storageRequestedTime_ > NodeActions::STORAGE_TIMEOUT)
      {
        subError              = Atams::ERROR_STORAGE_PROCESS_FAILED;
        storageProcess_ = StorageProcessState::CANCEL_CONFIG;
      }
      break;
    case StorageProcessState::CANCEL_CONFIG:
      if (!subError) subError = Atams::ERROR_STORAGE_PROCESS_FAILED;
      node_.resetRequestPacket();
      subProcessState = updateConfigurationStateExit(error, false);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) error = subError;          
      break;    
    case StorageProcessState::APPLY_CONFIG:
      node_.resetRequestPacket();
      subProcessState = updateConfigurationStateExit(error, true);
      if (subProcessState == Atams::PROCESS_STATE_COMPLETE) setConfigVarProcess_ = SetConfigVarState::COMPLETE;
      break;
    case StorageProcessState::COMPLETE:
      node_.resetRequestPacket();
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      storageProcess_ = StorageProcessState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    node_.resetRequestPacket();
    processReturn         = Atams::PROCESS_STATE_ERROR;
    storageProcess_ = StorageProcessState::START;
  }

  return (processReturn);
}

/*************************************************************************************/
/* PRIVATE HELPER STRUCT FUNCTION DEFINITIONS                                        */
/*************************************************************************************/

template <typename T>
void NodeActions::ProcessHandler<T>::endProcess(Atams::Error_t error)
{
  if (error) exitMinorState = T::ERROR;
  else       exitMinorState = T::COMPLETE;


  this->error = error;
  minorState  = T::CLEAR_BUFFER;
}

template <typename T>
bool NodeActions::ProcessHandler<T>::processTerminatred(void)
{
  return (error != Atams::ERROR_NONE);
}

template <typename T>
void NodeActions::ProcessHandler<T>::handleBufferClear(void)
{
  node_.resetRequestPacket();
  minorState = exitMinorState;
  if (minorState == T::COMPLETE)
  {
    processState = Atams::PROCESS_STATE_COMPLETE;
  }
  else if (minorState == T::ERROR)
  {
    processState = Atams::PROCESS_STATE_ERROR;
  }
}

} /* End Namespace - Atams */


Node node1;
Node node2;

NodeActions nodeArray[2] = {NodeProcesses(node1), NodeProcesses(node2)};

Atams::ProcessState_t processState = Atams::PROCESS_STATE_IN_PROGRESS;

bool allProcessesComplete = false;

for (NodeProcesses &nodeProcesses : nodeArray)
{
  /* This sets the process state of each of the state machines to start */
  /* The process state machines will no longer go back to the start automatically*/
  nodeProcesses.beginProcess();
}

while (allProcessesComplete == false)
{
  allProcessesComplete = true;
  
  for (NodeProcesses &nodeProcesses : nodeArray)
  {
    if (nodeProcesses->updateValidateGenInfo(error, genInfoIsValid) == Atams::PROCESS_STATE_IN_PROGRESS)
    {
      allProcessesComplete = false;
    }
  }
}

//Check error if any process is in error state


/**
  * @}End of File
  */
