/**
  ******************************************************************************
  * @file    BinarySemaphore.hpp
  *
  * @author  D. Baines
  *
  * @brief   Binary semaphore used by the harness Hub and Node Platforms.
  *
  * @details Blocks a thread until another thread calls release(), or until a timeout.
  *          Binary: several release() calls before a wait wake it only once, as on an
  *          RTOS binary semaphore.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include <condition_variable>
#include <mutex>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class BinarySemaphore
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  BinarySemaphore(void) = default;

  /* Default Destructor */
  ~BinarySemaphore(void) = default;

  /* Copy Constructor */
  BinarySemaphore(const BinarySemaphore &other) = delete;

  /* Copy Assignment Operator */
  BinarySemaphore & operator=(const BinarySemaphore &other) = delete;

  /* Move Constructor */
  BinarySemaphore(BinarySemaphore &&other) = delete;

  /* Move Assignment Operator */
  BinarySemaphore & operator=(BinarySemaphore &&other) = delete;

  void waitWithTimeout(const uint32_t timeoutMs);

  void release(void);

  private:

  /*-- Private Variables ------------------------------------------------------------*/

  std::mutex              mutex_;
  std::condition_variable condition_;
  bool                    released_ {false};
};

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
