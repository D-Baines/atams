/**
  ******************************************************************************
  * @file    ConfigurationHandler.hpp
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "WatchdogHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class ConfigurationHandler
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  using PasscodeProtectedFunction_t = Atams::Error_t (&)(void);

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  ConfigurationHandler(WatchdogHandler &watchdogHandler);

  /* Destructor */
  ~ConfigurationHandler(void) = default;

  /* Copy Constructor */
  ConfigurationHandler(const ConfigurationHandler &other) = delete;

  /* Copy Assignment Operator */
  ConfigurationHandler & operator=(const ConfigurationHandler &other) = delete;

  /* Move Constructor */
  ConfigurationHandler(ConfigurationHandler &&other) = delete;

  /* Move Assignment Operator */
  ConfigurationHandler & operator=(ConfigurationHandler &&other) = delete;

  void update(void);

  void setUpdateRequired(void);

  bool getConfigurationActive(void);

  uint8_t getLocalNodeID(void);

  uint8_t getPrevSyncNodeID(void);

  uint8_t getFirstSyncNodeID(void);

  uint8_t getFinalSyncNodeID(void);

  void notifyStorageProcessComplete(Atams::Error_t processStatus);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum StoragePasscodeID_t: uint8_t
  {
    PASSCODE_ID_STORE_ALL           = 0U,
    PASSCODE_ID_RESTORE_USER_BLOCKS = 1U,
    PASSCODE_ID_RESTORE_ALL         = 2U,
    PASSCODE_ID_RESET_NODE          = 3U,
    NUMBER_OF_PASSCODES
  };

  struct PasscodeChecker_t
  {
    const uint8_t                     passcodeVarID;
    const uint32_t                    requiredPasscode;
    uint32_t                          passcode;
    uint32_t                          prevPasscode;
    const PasscodeProtectedFunction_t processFunction;
    bool                              isStorageProcess;

    PasscodeChecker_t(void)                                      = delete;
    PasscodeChecker_t(const PasscodeChecker_t &other)            = delete;
    PasscodeChecker_t& operator=(const PasscodeChecker_t &other) = delete;
  };

  /*-- Private Variables ------------------------------------------------------------*/

  WatchdogHandler             &m_watchdogHandler;
  PasscodeChecker_t            m_configPasscodeCheckers[ConfigurationHandler::NUMBER_OF_PASSCODES];
  bool                         m_updateRequired     = false;
  Atams::ConfigurationStatus_t m_configurationState = Atams::CONFIGURATION_STATUS_INACTIVE;

  /* Configuration Parameters */
  uint8_t m_localNodeID     = 0U;
  uint8_t m_prevSyncNodeID  = 0U;
  uint8_t m_finalSyncNodeID = 0U;
  uint8_t m_firstSyncNodeID = 0U;
  uint8_t m_bitrateOption   = static_cast<uint8_t>(Atams::BITRATE_OPTION_0);

  /*-- Private Function Declarations -------------------------------------------------*/

  void checkConfigurationStateEntry(ConfigurationStatus_t &configurationState);

  void checkConfigurationStateExit(ConfigurationStatus_t &configurationState);

  void checkConfigurationPasscodes(void);

  void applyUniversalConfiguration(void);

  void cancelConfigurationValueChange(void);
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


