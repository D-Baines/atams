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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

NodeProcesses::NodeProcesses(Node &node) :
m_node(node)
{
  /* Do Nothing */
}

NodeProcesses::~NodeProcesses(void)
{
  /* Do Nothing */
}

Atams::ProcessState_t NodeProcesses::updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  bool                  newDataReady  = false;

  switch (m_validateGenInfoState)
  {
    case ValidateGenInfoState::START:
      m_validateGenInfoState = ValidateGenInfoState::COLLECT;
      break;

    case ValidateGenInfoState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; varID++)
      {
        static_cast<void>(m_node.clearNewDataFlag(varID));
        static_cast<void>(m_node.startReadStream(varID));
      }
      m_validateGenInfoState = ValidateGenInfoState::VALIDATE;
      break;

    case ValidateGenInfoState::VALIDATE:
      for (uint8_t varID = BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR; varID <= BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS; varID++)
      {
        static_cast<void>(m_node.stopStream(varID));
        static_cast<void>(m_node.isNewDataReady(varID, newDataReady));
        if (newDataReady == false) error = Atams::ERROR_OLD_DATA;
      }
      genInfoIsValid = m_node.validateGenInfo();
      m_validateGenInfoState = ValidateGenInfoState::COMPLETE;
      break; 

    case ValidateGenInfoState::COMPLETE:
      m_node.resetRequestPacket();
      processReturn          = Atams::PROCESS_STATE_COMPLETE;
      m_validateGenInfoState = ValidateGenInfoState::START;
      break;
    
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    m_node.resetRequestPacket();
    processReturn          = Atams::PROCESS_STATE_ERROR;
    m_validateGenInfoState = ValidateGenInfoState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateCollectNodeIdentifiers(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  bool                  newDataReady  = false;

  switch (m_collectNodeIDsState)
  {
    case CollectNodeIDsState::START:
      m_collectNodeIDsState = CollectNodeIDsState::COLLECT;
      break;
    case CollectNodeIDsState::COLLECT:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; varID++)
      {
        static_cast<void>(m_node.clearNewDataFlag(varID));
        static_cast<void>(m_node.startReadStream(varID));
      }
    case CollectNodeIDsState::CHECK_NEW:
      for (uint8_t varID = BlockUniversal::VAR_ID_NODE_ID; varID <= BlockUniversal::VAR_ID_PREVIOUS_NODE_ID; varID++)
      {
        static_cast<void>(m_node.stopStream(varID));
        static_cast<void>(m_node.isNewDataReady(varID, newDataReady));
        if (newDataReady == false) error = Atams::ERROR_OLD_DATA;
      }
      m_collectNodeIDsState = CollectNodeIDsState::COMPLETE;
      break;
    case CollectNodeIDsState::COMPLETE:
      m_node.resetRequestPacket();
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_collectNodeIDsState = CollectNodeIDsState::START;
      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    m_node.resetRequestPacket();
    processReturn         = Atams::PROCESS_STATE_ERROR;
    m_collectNodeIDsState = CollectNodeIDsState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateConfigurationStateEntry(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;
  uint8_t               configState   = Atams::CONFIGURATION_STATUS_INACTIVE;

  switch (m_configurationEntryState)
  {
    case ConfigurationEntryState::START:
      m_configurationEntryState = ConfigurationEntryState::COLLECT_STATUS_PRE;
      break;
    case ConfigurationEntryState::COLLECT_STATUS_PRE:
      static_cast<void>(m_node.clearNewDataFlag(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      static_cast<void>(m_node.startReadStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      break;
    case ConfigurationEntryState::CHECK_STATUS_PRE:
      static_cast<void>(m_node.stopStream(BlockUniversal::VAR_ID_CONFIGURATION_STATUS));
      error = m_node.readIfNew(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, configState);
      if (configState == Atams::CONFIGURATION_STATUS_ACTIVE)
      {
        m_configurationEntryState = ConfigurationEntryState::COMPLETE;
      }
      else 
      {
        m_configurationEntryState = ConfigurationEntryState::WRITE_PASSCODE;
      }
      break; 
    case ConfigurationEntryState::WRITE_PASSCODE:

      break;
    case ConfigurationEntryState::COLLECT_STATUS_POST:

      break;
    case ConfigurationEntryState::CHECK_STATUS_POST:

      break;
    case ConfigurationEntryState::COMPLETE:

      break;
    default:
      error = Atams::ERROR_INVALID_CASE;
      break;
  }

  if (error)
  {
    m_node.resetRequestPacket();
    processReturn         = Atams::PROCESS_STATE_ERROR;
    m_collectNodeIDsState = CollectNodeIDsState::START;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateConfigurationStateExit(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateSetNodeID(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateSetBitrate(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateSetWatchdogPeriod(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateStoreAll(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

Atams::ProcessState_t NodeProcesses::updateResetNode(Atams::Error_t &error)
{
  Atams::ProcessState_t processReturn = Atams::PROCESS_STATE_IN_PROGRESS;

  switch (m_processState)
  {
    case _START:
      
      break;
    case _SUCCESS:
      processReturn         = Atams::PROCESS_STATE_COMPLETE;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
    case _ERROR:
    default:
      processReturn         = Atams::PROCESS_STATE_ERROR;
      m_processState = NodeProcesses::VALIDATE_GEN_INFO_START;
      break;
  }

  return (processReturn);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

} /* End Namespace - Atams */


/**
  * @}End of File
  */
