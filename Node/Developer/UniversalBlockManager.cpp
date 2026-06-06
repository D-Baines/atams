/**
  ******************************************************************************
  * @file    ConfigurationHandler.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams Node UniversalBlockManager.
  *
  * @details Implements the state management logic for the Atams Node universal block.
  *          Monitors configuration state transitions and passcode-protected processes
  *          each update cycle, applies universal configuration to the platform on
  *          configuration exit, and notifies the Comms Core when storage processes
  *          complete.
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

#include "UniversalBlockManager.hpp"

#include "../CommsCore/CommsCore.hpp"
#include "../CommsCore/CommsPlatform.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

UniversalBlockManager::UniversalBlockManager(WatchdogHandler &watchdogHandler) :
watchdogHandler_(watchdogHandler),
configPasscodeCheckers_
{
  /* [PROCESS_ID_STORE_ALL] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_STORE_ALL,
    /* .requiredPasscode = */ Atams::STORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::storeAll,
    /* .isStorageProcess = */ true
  },
  /* [PROCESS_ID_RESTORE_USER_BLOCKS] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESTORE_USER_BLOCKS,
    /* .requiredPasscode = */ Atams::RESTORE_USER_BLOCKS_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::restoreUser,
    /* .isStorageProcess = */ true
  },
  /* [PROCESS_ID_RESTORE_ALL] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESTORE_ALL,
    /* .requiredPasscode = */ Atams::RESTORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::restoreAll,
    /* .isStorageProcess = */ true
  },
  /* [PROCESS_ID_RESET_NODE] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESET_NODE,
    /* .requiredPasscode = */ Atams::RESET_NODE_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Platform::resetNode,
    /* .isStorageProcess = */ false
  },
}
{
  /* Do Nothing */
}

UniversalBlockManager::ProcessID_t UniversalBlockManager::update(void)
{
  if (updateRequired_ == false)
  {
    return (PROCESS_ID_UNKNOWN); /* Early Return */
  }

  updateRequired_ = false;

  switch (configurationState_)
  {
    case CONFIGURATION_STATUS_INACTIVE:
    case CONFIGURATION_STATUS_DENIED:
    case CONFIGURATION_STATUS_APPLIED:
      checkConfigurationStateEntry();
      break;

    case CONFIGURATION_STATUS_ACTIVE:
      checkConfigurationPasscodes();
      checkConfigurationStateExit();
      break;

    default:
      /* Do Nothing */
      break;
  }

  static_cast<void>(Atams::setVar(BlockUniversal::VAR_CONFIGURATION_STATUS,
                    static_cast<uint8_t>(configurationState_)));

  return (pendingProcessID_);
}

Atams::Error_t UniversalBlockManager::applyUniversalConfiguration(void)
{
  uint16_t error {Atams::ERROR_NONE};

  uint32_t watchdogPeriod {WatchdogHandler::MINIMUM_VALID_WATCHDOG_PERIOD};

  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_NODE_ID,          localNodeID_));
  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_FIRST_NODE_ID,    firstSyncNodeID_));
  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_PREVIOUS_NODE_ID, prevSyncNodeID_));
  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_LAST_NODE_ID,     finalSyncNodeID_));
  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_BITRATE,          bitrateOption_));
  error |= static_cast<uint16_t>(Atams::getVar(BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod));

  Platform::setBitrate(static_cast<Atams::BitrateOption_t>(bitrateOption_));

  watchdogHandler_.setWatchdogPeriod(watchdogPeriod);

  return (error == Atams::ERROR_NONE ?
          Atams::ERROR_NONE          :
          Atams::ERROR_MEMORY_MAP    );
}


void UniversalBlockManager::runPendingProcess(void)
{
  if (pendingProcessID_ < NUMBER_OF_PROCESSES)
  {
    PasscodeChecker_t &checker       = configPasscodeCheckers_[pendingProcessID_];
    Atams::Error_t     processStatus = checker.processFunction();

    if (checker.isStorageProcess == true)
    {
      notifyStorageProcessComplete(processStatus);
    }

    pendingProcessID_ = PROCESS_ID_UNKNOWN;
  }
}

void UniversalBlockManager::setUpdateRequired(void)
{
  updateRequired_ = true;
}

bool UniversalBlockManager::checkWriteAccess(const uint16_t varID)
{
  return ((configurationState_ == CONFIGURATION_STATUS_ACTIVE              ) ||
          (varID               == BlockUniversal::VAR_CONFIGURATION_PASSKEY) ||
          (varID               == BlockUniversal::VAR_WATCHDOG_RESET       ) );
}

void UniversalBlockManager::processRead(const uint16_t varID)
{
  switch (varID)
  {
    case BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE:
      static_cast<void>(Atams::setVar(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE,
                                      static_cast<uint8_t>(false)));
      break;
    default:
      /* Do Nothing */
      break;
  }
}

uint8_t UniversalBlockManager::getLocalNodeID(void)
{
  return (localNodeID_);
}

uint8_t UniversalBlockManager::getPrevSyncNodeID(void)
{
  return (prevSyncNodeID_);
}

uint8_t UniversalBlockManager::getFirstSyncNodeID(void)
{
  return (firstSyncNodeID_);
}

uint8_t UniversalBlockManager::getFinalSyncNodeID(void)
{
  return (finalSyncNodeID_);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void UniversalBlockManager::checkConfigurationStateEntry(void)
{
  static uint32_t prevConfigStatePasskey = 0U;
  uint32_t        configStatePasskey     = 0U;

  if (configurationState_ == Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(Atams::getVar(BlockUniversal::VAR_CONFIGURATION_PASSKEY, configStatePasskey));

  if ((configStatePasskey     == Atams::CONFIGURATION_PASSKEY_ACCESS) &&
      (prevConfigStatePasskey != Atams::CONFIGURATION_PASSKEY_ACCESS) )
  {
    if (Platform::enterConfigurationState() == true)
    {
      configurationState_ = Atams::CONFIGURATION_STATUS_ACTIVE;
    }
    else
    {
      configurationState_ = Atams::CONFIGURATION_STATUS_DENIED;
    }
  }
}

void UniversalBlockManager::checkConfigurationStateExit(void)
{
  uint32_t configStatePasskey = 0U;

  if (configurationState_ != Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(Atams::getVar(BlockUniversal::VAR_CONFIGURATION_PASSKEY, configStatePasskey));

  if (configStatePasskey == Atams::CONFIGURATION_PASSKEY_APPLY)
  {
    static_cast<void>(applyUniversalConfiguration());
    Platform::exitConfigurationState();
    configurationState_ = Atams::CONFIGURATION_STATUS_APPLIED;
  }
  else if (configStatePasskey != Atams::CONFIGURATION_PASSKEY_ACCESS)
  {
    cancelConfigurationValueChange();
    Platform::exitConfigurationState();
    configurationState_ = Atams::CONFIGURATION_STATUS_INACTIVE;
  }
}

void UniversalBlockManager::checkConfigurationPasscodes(void)
{
  for (uint8_t processIndex = 0U; processIndex < NUMBER_OF_PROCESSES; processIndex++)
  {
    PasscodeChecker_t &checker = configPasscodeCheckers_[processIndex];

    static_cast<void>(Atams::getVar(checker.passcodeVarID, checker.passcode));

    if ((checker.passcode     == checker.requiredPasscode) &&
        (checker.prevPasscode != checker.requiredPasscode) )
    {
      pendingProcessID_ = static_cast<ProcessID_t>(processIndex);
    }

    checker.prevPasscode = checker.passcode;
  }
}

void UniversalBlockManager::cancelConfigurationValueChange(void)
{
  uint32_t watchdogPeriod = watchdogHandler_.getWatchdogPeriod();

  static_cast<void>(Atams::setVar(BlockUniversal::VAR_NODE_ID,          localNodeID_));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_FIRST_NODE_ID,    firstSyncNodeID_));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_PREVIOUS_NODE_ID, prevSyncNodeID_));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_LAST_NODE_ID,     finalSyncNodeID_));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_BITRATE,          bitrateOption_));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod));
}

void UniversalBlockManager::notifyStorageProcessComplete(Atams::Error_t processStatus)
{
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_STORAGE_STATUS,           static_cast<uint8_t>(processStatus)));
  static_cast<void>(Atams::setVar(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(ATAMS_TRUE)));
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
