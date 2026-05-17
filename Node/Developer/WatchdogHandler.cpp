/**
  ******************************************************************************
  * @file    WatchdogHandler.cpp
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
