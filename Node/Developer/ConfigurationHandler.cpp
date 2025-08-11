/**
  ******************************************************************************
  * @file    ConfigurationHandler.cpp
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

#include "ConfigurationHandler.hpp"
#include "../Node.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

ConfigurationHandler::ConfigurationHandler(WatchdogHandler &watchdogHandler) :
watchdogHandler_(watchdogHandler),
configPasscodeCheckers_
{
  /* [PASSCODE_ID_STORE_ALL] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_STORE_ALL,
    /* .requiredPasscode = */ Atams::STORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::storeAll,
    /* .isStorageProcess = */ true
  },
  /* [PASSCODE_ID_RESTORE_USER_BLOCKS] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESTORE_USER_BLOCKS,
    /* .requiredPasscode = */ Atams::RESTORE_USER_BLOCKS_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::restoreUser,
    /* .isStorageProcess = */ true
  },
  /* [PASSCODE_ID_RESTORE_ALL] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESTORE_ALL,
    /* .requiredPasscode = */ Atams::RESTORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ Atams::restoreAll,
    /* .isStorageProcess = */ true
  },
  /* [PASSCODE_ID_RESET_NODE] = */
  {
    /* .passcodeVarID    = */ BlockUniversal::VAR_RESTORE_ALL,
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

void ConfigurationHandler::initConfiguration(void)
{
  applyUniversalConfiguration();
}

void ConfigurationHandler::update(void)
{
  if (updateRequired_ == false)
  {
    return; /* Early Return */
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

  Atams::write(BlockUniversal::VAR_CONFIGURATION_STATUS, static_cast<uint8_t>(configurationState_));
}

void ConfigurationHandler::setUpdateRequired(void)
{
  updateRequired_ = true;
}

bool ConfigurationHandler::getConfigurationActive(void)
{
  return (configurationState_ == CONFIGURATION_STATUS_ACTIVE);
}

uint8_t ConfigurationHandler::getLocalNodeID(void)
{
  return (localNodeID_);
}

uint8_t ConfigurationHandler::getPrevSyncNodeID(void)
{
  return (prevSyncNodeID_);
}

uint8_t ConfigurationHandler::getFirstSyncNodeID(void)
{
  return (firstSyncNodeID_);
}

uint8_t ConfigurationHandler::getFinalSyncNodeID(void)
{
  return (finalSyncNodeID_);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void ConfigurationHandler::checkConfigurationStateEntry(void)
{
  static uint32_t prevConfigStatePasskey = 0U;
  uint32_t        configStatePasskey     = 0U;

  if (configurationState_ == Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(Atams::read(BlockUniversal::VAR_CONFIGURATION_PASSKEY, configStatePasskey));

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

void ConfigurationHandler::checkConfigurationStateExit(void)
{
  uint32_t configStatePasskey = 0U;

  if (configurationState_ != Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(Atams::read(BlockUniversal::VAR_CONFIGURATION_PASSKEY, configStatePasskey));

  if (configStatePasskey == Atams::CONFIGURATION_PASSKEY_APPLY)
  {
    applyUniversalConfiguration();
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

void ConfigurationHandler::checkConfigurationPasscodes(void)
{
  for (PasscodeChecker_t &checker : configPasscodeCheckers_)
  {
    static_cast<void>(Atams::read(checker.passcodeVarID, checker.passcode));

    if ((checker.passcode     == checker.requiredPasscode) &&
        (checker.prevPasscode != checker.requiredPasscode) )
    {
      Atams::Error_t processStatus = checker.processFunction();

      if (checker.isStorageProcess == true)
      {
        notifyStorageProcessComplete(processStatus);
      }
    }

    checker.prevPasscode = checker.passcode;
  }
}

void ConfigurationHandler::applyUniversalConfiguration(void)
{
  uint32_t watchdogPeriod = WatchdogHandler::MINIMUM_VALID_WATCHDOG_PERIOD;

  static_cast<void>(Atams::read(BlockUniversal::VAR_NODE_ID,          localNodeID_));
  static_cast<void>(Atams::read(BlockUniversal::VAR_FIRST_NODE_ID,    firstSyncNodeID_));
  static_cast<void>(Atams::read(BlockUniversal::VAR_PREVIOUS_NODE_ID, prevSyncNodeID_));
  static_cast<void>(Atams::read(BlockUniversal::VAR_LAST_NODE_ID,     finalSyncNodeID_));
  static_cast<void>(Atams::read(BlockUniversal::VAR_BITRATE,          bitrateOption_));
  static_cast<void>(Atams::read(BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod));

  Platform::setBitrate(static_cast<Atams::BitrateOption_t>(bitrateOption_));

  watchdogHandler_.setWatchdogPeriod(watchdogPeriod);
}

void ConfigurationHandler::cancelConfigurationValueChange(void)
{
  uint32_t watchdogPeriod = watchdogHandler_.getWatchdogPeriod();

  static_cast<void>(Atams::write(BlockUniversal::VAR_NODE_ID,          localNodeID_));
  static_cast<void>(Atams::write(BlockUniversal::VAR_FIRST_NODE_ID,    firstSyncNodeID_));
  static_cast<void>(Atams::write(BlockUniversal::VAR_PREVIOUS_NODE_ID, prevSyncNodeID_));
  static_cast<void>(Atams::write(BlockUniversal::VAR_LAST_NODE_ID,     finalSyncNodeID_));
  static_cast<void>(Atams::write(BlockUniversal::VAR_BITRATE,          bitrateOption_));
  static_cast<void>(Atams::write(BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod));
}

void ConfigurationHandler::notifyStorageProcessComplete(Atams::Error_t processStatus)
{
  static_cast<void>(Atams::write(BlockUniversal::VAR_STORAGE_STATUS,           static_cast<uint8_t>(processStatus)));
  static_cast<void>(Atams::write(BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE, static_cast<uint8_t>(ATAMS_TRUE)));
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
