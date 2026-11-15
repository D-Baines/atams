/**
  ******************************************************************************
  * @file    BinarySemaphore.cpp
  *
  * @author  D. Baines
  *
  * @brief   Binary semaphore used by the harness Hub and Node Platforms.
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

#include "BinarySemaphore.hpp"

#include <chrono>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief Block until release() is called or timeoutMs passes. Consumes the release.
 */
void BinarySemaphore::waitWithTimeout(const uint32_t timeoutMs)
{
  const std::chrono::steady_clock::time_point deadline {std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs)};

  std::unique_lock<std::mutex> lock(mutex_);

  /* Loops because a condition variable can wake without being notified */
  while (!released_)
  {
    if (condition_.wait_until(lock, deadline) == std::cv_status::timeout) break;
  }

  released_ = false;
}

/**
 * @brief Wake the waiting thread, or the next thread to wait.
 */
void BinarySemaphore::release(void)
{
  mutex_.lock();

  released_ = true;

  mutex_.unlock();

  condition_.notify_one();
}

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
