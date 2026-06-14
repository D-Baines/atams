/**
  ******************************************************************************
  * @file    WatchdogHandler.hpp
  *
  * @author  D. Baines
  *
  * @brief   Software watchdog handler for detecting Atams Hub communication dropouts.
  *
  * @details Defines the WatchdogHandler class, which monitors the time elapsed since
  *          the last valid Atams Hub message was received. If the elapsed time exceeds
  *          the configured watchdog period, a fault is raised internally and flagged 
  *          in the Universal Data Block. The fault can be cleared by writing the
  *          correct passcode to the watchdog reset variable.
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../../Shared/AtamsTypedefs.hpp"

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
  WatchdogHandler(void) = default;

  /* Destructor */
  ~WatchdogHandler(void) = default;

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

  uint32_t getWatchdogFault(void);

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


