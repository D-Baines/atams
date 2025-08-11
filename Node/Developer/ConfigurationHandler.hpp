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

  /* Default Constructor */
  ConfigurationHandler(void) = delete;

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

  void initConfiguration(void);

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

  WatchdogHandler             &watchdogHandler_;
  PasscodeChecker_t            configPasscodeCheckers_[ConfigurationHandler::NUMBER_OF_PASSCODES];
  bool                         updateRequired_     = false;
  Atams::ConfigurationStatus_t configurationState_ = Atams::CONFIGURATION_STATUS_INACTIVE;

  /* Configuration Parameters */
  uint8_t localNodeID_     = 0U;
  uint8_t prevSyncNodeID_  = 0U;
  uint8_t finalSyncNodeID_ = 0U;
  uint8_t firstSyncNodeID_ = 0U;
  uint8_t bitrateOption_   = static_cast<uint8_t>(Atams::BITRATE_OPTION_0);

  /*-- Private Function Declarations -------------------------------------------------*/

  void checkConfigurationStateEntry(void);

  void checkConfigurationStateExit(void);

  void checkConfigurationPasscodes(void);

  void applyUniversalConfiguration(void);

  void cancelConfigurationValueChange(void);
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


