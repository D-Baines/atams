/**
  ******************************************************************************
  * @file    WatchdogHandler.hpp
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
#include "../../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class WatchdogHandler
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  static constexpr uint32_t WATCHDOG_UPDATE_PERIOD        = 1U;
  static constexpr uint32_t MINIMUM_VALID_WATCHDOG_PERIOD = 1U;

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  WatchdogHandler(void);

  /* Destructor */
  ~WatchdogHandler(void);

  /* Copy Constructor */
  WatchdogHandler(const WatchdogHandler &other) = delete;

  /* Copy Assignment Operator */
  WatchdogHandler & operator=(const WatchdogHandler &other) = delete;

  /* Move Constructor */
  WatchdogHandler(WatchdogHandler &&other) = delete;

  /* Move Assignment Operator */
  WatchdogHandler & operator=(WatchdogHandler &&other) = delete;

  void setWatchdogPeriod(uint32_t period);

  uint32_t getWatchdogPeriod(void);

  void toggleWatchdog(void);

  void update(const uint32_t currentTime);

  bool getWatchdogFault(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum WatchdogFaultStatus_t: uint8_t
  {
    WATCHDOG_FAULT_INACTIVE = 0U,
    WATCHDOG_FAULT_ACTIVE   = 1U
  };

  /*-- Private Variables ------------------------------------------------------------*/

  uint32_t m_prevWatchdogUpdateTime = 0U;
  uint32_t m_prevWatchdogReset      = Atams::WATCHDOG_RESET_PASSCODE;
  uint32_t m_watchdogPeriod         = 0U;
  uint32_t m_watchdogCount          = 0U;
  uint32_t m_watchdogStatus         = WATCHDOG_FAULT_INACTIVE;

  /*-- Private Function Declarations -------------------------------------------------*/

};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


