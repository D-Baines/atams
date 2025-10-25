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

class UniversalBlockManager
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  using PasscodeProtectedFunction_t = Atams::Error_t (&)(void);

  enum ProcessID_t: uint8_t
  {
    PROCESS_ID_STORE_ALL           = 0U,
    PROCESS_ID_RESTORE_USER_BLOCKS = 1U,
    PROCESS_ID_RESTORE_ALL         = 2U,
    PROCESS_ID_RESET_NODE          = 3U,
    NUMBER_OF_PROCESSES,
    PROCESS_ID_UNKNOWN
  };

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  UniversalBlockManager(WatchdogHandler &watchdogHandler);

  /* Default Constructor */
  UniversalBlockManager(void) = delete;

  /* Destructor */
  ~UniversalBlockManager(void) = default;

  /* Copy Constructor */
  UniversalBlockManager(const UniversalBlockManager &other) = delete;

  /* Copy Assignment Operator */
  UniversalBlockManager & operator=(const UniversalBlockManager &other) = delete;

  /* Move Constructor */
  UniversalBlockManager(UniversalBlockManager &&other) = delete;

  /* Move Assignment Operator */
  UniversalBlockManager & operator=(UniversalBlockManager &&other) = delete;

  void initConfiguration(void);

  UniversalBlockManager::ProcessID_t update(void);

  void triggerPendingProcess(void);

  void setUpdateRequired(void);

  bool checkWriteAccess(const uint16_t varID);

  void processRead(const uint16_t varID);

  uint8_t getLocalNodeID(void);

  uint8_t getPrevSyncNodeID(void);

  uint8_t getFirstSyncNodeID(void);

  uint8_t getFinalSyncNodeID(void);

  void notifyStorageProcessComplete(Atams::Error_t processStatus);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

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
  PasscodeChecker_t            configPasscodeCheckers_[NUMBER_OF_PROCESSES];
  bool                         updateRequired_     {false};
  Atams::ConfigurationStatus_t configurationState_ {Atams::CONFIGURATION_STATUS_INACTIVE};
  ProcessID_t                  pendingProcessID_   {PROCESS_ID_UNKNOWN};

  /* Configuration Parameters */
  uint8_t localNodeID_     {0U};
  uint8_t prevSyncNodeID_  {0U};
  uint8_t finalSyncNodeID_ {0U};
  uint8_t firstSyncNodeID_ {0U};
  uint8_t bitrateOption_   {static_cast<uint8_t>(Atams::BITRATE_OPTION_0)};

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


