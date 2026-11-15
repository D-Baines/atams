/**
  ******************************************************************************
  * @file    TestUtilities.hpp
  *
  * @author  D. Baines
  *
  * @brief   Shared utilities for the Atams host test harness and the tests.
  *
  * @details Includes no Atams headers, so it can be used anywhere in the harness,
  *          including the harness Platform files compiled inside the Atams libraries.
  *          Utilities that drive the Hub library belong in HubTestUtilities instead.
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint32_t getMillis(void);

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

/**
 * @brief Time limit for a test loop. Starts timing when constructed.
 */
class Deadline
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  explicit Deadline(const uint32_t timeoutMs);

  /* Default Destructor */
  ~Deadline(void) = default;

  /* Copy Constructor */
  Deadline(const Deadline &other) = delete;

  /* Copy Assignment Operator */
  Deadline & operator=(const Deadline &other) = delete;

  /* Move Constructor */
  Deadline(Deadline &&other) = delete;

  /* Move Assignment Operator */
  Deadline & operator=(Deadline &&other) = delete;

  bool expired(void) const;

  private:

  /*-- Private Variables ------------------------------------------------------------*/

  const uint32_t startTime_;
  const uint32_t timeoutMs_;
};

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
