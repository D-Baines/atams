/**
  ******************************************************************************
  * @file    WatchdogHandler.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams Node software watchdog handler.
  *
  * @details Implements the WatchdogHandler class. update() increments an 
  *          internal counter every milliseconds. When the counter exceeds the 
  *          watchdog period, a fault is is flagged internally and in the 
  *          Universal Data Block. Calls to toggleWatchdog() reset the counter.
  *          The watchdog fault is cleared if BlockUniversal::VAR_WATCHDOG_RESET
  *          is set to Atams::WATCHDOG_RESET_PASSCODE.
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "WatchdogHandler.hpp"
#include "../CommsCore/CommsCore.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

void WatchdogHandler::setWatchdogPeriod(uint32_t period)
{
  m_watchdogPeriod = period;
}

uint32_t WatchdogHandler::getWatchdogPeriod(void)
{
  return (m_watchdogPeriod);
}

void WatchdogHandler::toggleWatchdog(void)
{
  m_watchdogCount = 0U;
}

void WatchdogHandler::update(const uint32_t currentTime)
{

  if ((currentTime - m_prevWatchdogUpdateTime) > WATCHDOG_UPDATE_PERIOD)
  {
    switch (m_watchdogStatus)
    {
      case WATCHDOG_FAULT_INACTIVE:

        if (m_watchdogCount < Atams::MAX_UINT32) m_watchdogCount++;

        if ((m_watchdogPeriod > 0U              ) &&
            (m_watchdogCount  > m_watchdogPeriod) )
        {
          m_watchdogStatus = WATCHDOG_FAULT_ACTIVE;
        }

        break;

      case WATCHDOG_FAULT_ACTIVE:
      {
        uint32_t watchdogReset {0U};

        static_cast<void>(Atams::getVar(BlockUniversal::VAR_WATCHDOG_RESET, watchdogReset));

        if ((watchdogReset       == Atams::WATCHDOG_RESET_PASSCODE) &&
            (m_prevWatchdogReset != Atams::WATCHDOG_RESET_PASSCODE) )
        {
          m_watchdogStatus = WATCHDOG_FAULT_INACTIVE;
        }

        m_prevWatchdogReset = watchdogReset;

        break;
      }
      default:

        m_watchdogStatus = WATCHDOG_FAULT_ACTIVE;

        break;
    }

    Atams::setVar(BlockUniversal::VAR_WATCHDOG_FAULT_ACTIVE, static_cast<uint8_t>(m_watchdogStatus));

    m_prevWatchdogUpdateTime = currentTime;
  }
}

uint32_t WatchdogHandler::getWatchdogFault(void)
{
  return (m_watchdogStatus == WATCHDOG_FAULT_ACTIVE);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
