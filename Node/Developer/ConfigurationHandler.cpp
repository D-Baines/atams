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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

ConfigurationHandler::ConfigurationHandler(DataBlock &universalDataBlock,
                                           WatchdogHandler &watchdogHandler,
                                           PasscodeProtectedFunction_t storeFunctionFn,
                                           PasscodeProtectedFunction_t restoreUserBlocksFn,
                                           PasscodeProtectedFunction_t restoreAllFn,
                                           PasscodeProtectedFunction_t resetNodeFn) :
m_universalBlock(universalDataBlock),
m_watchdogHandler(watchdogHandler),
m_configPasscodeCheckers
{
  /* [PASSCODE_ID_STORE_ALL] = */
  {
    /* .universalVarID   = */ BlockUniversal::VAR_ID_STORE_ALL,
    /* .requiredPasscode = */ Atams::STORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ storeFunctionFn
  },
  /* [PASSCODE_ID_RESTORE_USER_BLOCKS] = */
  {
    /* .universalVarID   = */ BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS,
    /* .requiredPasscode = */ Atams::RESTORE_USER_BLOCKS_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ restoreUserBlocksFn
  },
  /* [PASSCODE_ID_RESTORE_ALL] = */
  {
    /* .universalVarID   = */ BlockUniversal::VAR_ID_RESTORE_ALL,
    /* .requiredPasscode = */ Atams::RESTORE_ALL_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ restoreAllFn
  },
  /* [PASSCODE_ID_RESET_NODE] = */
  {
    /* .universalVarID   = */ BlockUniversal::VAR_ID_RESTORE_ALL,
    /* .requiredPasscode = */ Atams::RESET_NODE_PASSCODE,
    /* .passcode         = */ 0U,
    /* .prevPasscode     = */ 0U,
    /* .processFunction  = */ resetNodeFn
  },
}
{

}

ConfigurationHandler::~ConfigurationHandler(void)
{
  /* Do Nothing */
}

void ConfigurationHandler::update(void)
{
  if (m_updateRequired == false)
  {
    return; /* Early Return */
  }

  m_updateRequired = false;

  switch (m_configurationState)
  {
    case CONFIGURATION_STATUS_INACTIVE:
    case CONFIGURATION_STATUS_DENIED:
      checkConfigurationStateEntry(m_configurationState);
      break;

    case CONFIGURATION_STATUS_ACTIVE:
      checkConfigurationPasscodes();
      checkConfigurationStateExit(m_configurationState);
      break;

    default:
      /* Do Nothing */
      break;
  }

  m_universalBlock.write(BlockUniversal::VAR_ID_CONFIGURATION_STATUS, static_cast<uint8_t>(m_configurationState));
}

void ConfigurationHandler::setUpdateRequired(void)
{
  m_updateRequired = true;
}

bool ConfigurationHandler::getConfigurationActive(void)
{
  return (m_configurationState == CONFIGURATION_STATUS_ACTIVE);
}

uint8_t ConfigurationHandler::getLocalNodeID(void)
{
  return (m_localNodeID);
}

uint8_t ConfigurationHandler::getPrevSyncNodeID(void)
{
  return (m_prevSyncNodeID);
}

uint8_t ConfigurationHandler::getFirstSyncNodeID(void)
{
  return (m_firstSyncNodeID);
}

uint8_t ConfigurationHandler::getFinalSyncNodeID(void)
{
  return (m_finalSyncNodeID);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void ConfigurationHandler::checkConfigurationStateEntry(Atams::ConfigurationStatus_t &configurationState)
{
  static uint32_t prevConfigStatePasskey = 0U;
  uint32_t        configStatePasskey     = 0U;

  if (configurationState == Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, configStatePasskey));

  if ((configStatePasskey     == Atams::CONFIGURATION_PASSKEY_ACCESS) &&
      (prevConfigStatePasskey != Atams::CONFIGURATION_PASSKEY_ACCESS) )
  {
    if (Platform::enterConfigurationState() == true)
    {
      configurationState = Atams::CONFIGURATION_STATUS_ACTIVE;
    }
    else
    {
      configurationState = Atams::CONFIGURATION_STATUS_DENIED;
    }
  }
}

void ConfigurationHandler::checkConfigurationStateExit(Atams::ConfigurationStatus_t &configurationState)
{
  uint32_t configStatePasskey = 0U;

  if (configurationState != Atams::CONFIGURATION_STATUS_ACTIVE)
  {
    return; /* Early Return */
  }

  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY, configStatePasskey));

  if (configStatePasskey == Atams::CONFIGURATION_PASSKEY_APPLY)
  {
    applyUniversalConfiguration();
    Platform::exitConfigurationState();
    configurationState = Atams::CONFIGURATION_STATUS_INACTIVE;
  }
  else if (configStatePasskey != Atams::CONFIGURATION_PASSKEY_CANCEL)
  {
    cancelConfigurationValueChange();
    Platform::exitConfigurationState();
    configurationState = Atams::CONFIGURATION_STATUS_INACTIVE;
  }
}

void ConfigurationHandler::checkConfigurationPasscodes(void)
{
  for (PasscodeChecker_t &checker : m_configPasscodeCheckers)
  {
    static_cast<void>(m_universalBlock.read(checker.universalVarID, checker.passcode));

    if ((checker.passcode     == checker.requiredPasscode) &&
        (checker.prevPasscode != checker.requiredPasscode) )
    {
      checker.processFunction();
    }

    checker.prevPasscode = checker.passcode;
  }
}

void ConfigurationHandler::applyUniversalConfiguration(void)
{
  uint32_t watchdogPeriod = WatchdogHandler::MINIMUM_VALID_WATCHDOG_PERIOD;

  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_NODE_ID,          m_localNodeID));
  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_FIRST_NODE_ID,    m_firstSyncNodeID));
  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, m_prevSyncNodeID));
  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_LAST_NODE_ID,     m_finalSyncNodeID));
  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_BITRATE,          m_bitrateOption));
  static_cast<void>(m_universalBlock.read(BlockUniversal::VAR_ID_WATCHDOG_PERIOD,  watchdogPeriod));

  Platform::setBitrate(static_cast<Atams::BitrateOption_t>(m_bitrateOption));

  m_watchdogHandler.setWatchdogPeriod(watchdogPeriod);
}

void ConfigurationHandler::cancelConfigurationValueChange(void)
{
  uint32_t watchdogPeriod = m_watchdogHandler.getWatchdogPeriod();

  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_NODE_ID,          m_localNodeID));
  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_FIRST_NODE_ID,    m_firstSyncNodeID));
  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, m_prevSyncNodeID));
  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_LAST_NODE_ID,     m_finalSyncNodeID));
  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_BITRATE,          m_bitrateOption));
  static_cast<void>(m_universalBlock.write(BlockUniversal::VAR_ID_WATCHDOG_PERIOD,  watchdogPeriod));
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */
