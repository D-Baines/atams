/**
  ******************************************************************************
  * @file    TestUtilities.cpp
  *
  * @author  D. Baines
  *
  * @brief   Shared utilities for the Atams host test harness and the tests.
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

#include "TestUtilities.hpp"

#include <chrono>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief  Get the time in milliseconds since this function was first called.
 *
 * @return Milliseconds since the first call. Wraps at UINT32_MAX, as a device's
 *         millisecond tick would. Used as getMillis() by both harness Platform
 *         layers, so the Hub and Node sides share one clock.
 */
uint32_t getMillis(void)
{
  static const std::chrono::steady_clock::time_point s_epoch {std::chrono::steady_clock::now()};

  const std::chrono::steady_clock::duration elapsed {std::chrono::steady_clock::now() - s_epoch};

  return (static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()));
}

/**
 * @brief Start a deadline.
 *
 * @param timeoutMs Time in milliseconds after which expired() returns true.
 */
Deadline::Deadline(const uint32_t timeoutMs) :
startTime_(getMillis()),
timeoutMs_(timeoutMs)
{
  /* Do Nothing */
}

/**
 * @brief  Check whether the deadline has passed.
 *
 * @return true once at least timeoutMs has elapsed since construction. Correct across
 *         a wrap of the millisecond clock.
 */
bool Deadline::expired(void) const
{
  return ((getMillis() - startTime_) >= timeoutMs_);
}

} /* End Namespace - AtamsTestHarness */

/**
  * @}End of File
  */
