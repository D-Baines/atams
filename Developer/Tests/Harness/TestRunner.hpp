/**
  ******************************************************************************
  * @file    TestRunner.hpp
  *
  * @author  D. Baines
  *
  * @brief   Test runner for the Atams host test harness.
  *
  * @details TestRunner.cpp holds main(). It runs the tests in the registry returned
  *          by getTestList() one after another in this process, each in the modes
  *          it selects, and reports a result line per run.
  *
  *          Arguments:
  *            --test <name>  Run only this test
  *            --seed <N>     Seed for randomised tests (random if absent)
  *            --list         Print the registered tests
  *            --help         Print usage
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

#include "HarnessTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

/* Defined in Developer/Tests/TestList.cpp */
const TestList_t & getTestList(void);

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
