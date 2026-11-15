/**
  ******************************************************************************
  * @file    HarnessSelfTests.cpp
  *
  * @author  D. Baines
  *
  * @brief   Checks that the machine running the tests behaves as the harness expects.
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

#include <chrono>
#include <thread>

#include "Harness/Assertions.hpp"
#include "Harness/HarnessTypedefs.hpp"
#include "Harness/TestUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTests {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief The platform clock moves forward and sleeps last at least as long as asked,
 *        so test timeouts behave as expected.
 */
void testHarnessClockDeadline(const AtamsTestHarness::TestOptions_t &options)
{
  static_cast<void>(options);

  const uint32_t             startTime {AtamsTestHarness::getMillis()};
  AtamsTestHarness::Deadline deadline(20U);

  HARNESS_CHECK(!deadline.expired());

  std::this_thread::sleep_for(std::chrono::milliseconds(30));

  HARNESS_CHECK(deadline.expired());
  HARNESS_CHECK((AtamsTestHarness::getMillis() - startTime) >= 30U);
}

} /* End Namespace - AtamsTests */

/**
  * @}End of File
  */
