/**
  ******************************************************************************
  * @file    TestList.cpp
  *
  * @author  D. Baines
  *
  * @brief   Registry of the Atams host tests, in run order.
  *
  * @details To add a test, declare its function below and add an entry to
  *          s_testList. Entries default to polling mode only.
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

#include "Harness/TestRunner.hpp"

/*************************************************************************************/
/* TEST FUNCTION DECLARATIONS                                                        */
/*************************************************************************************/

namespace AtamsTests {

/* HarnessSelfTests.cpp */
void testHarnessClockDeadline(const AtamsTestHarness::TestOptions_t &options);

/* RouterTests.cpp */
void testRouterBroadcast(const AtamsTestHarness::TestOptions_t &options);
void testRouterChunking(const AtamsTestHarness::TestOptions_t &options);
void testRouterConnection(const AtamsTestHarness::TestOptions_t &options);
void testRouterTrafficLog(const AtamsTestHarness::TestOptions_t &options);
void testRouterWaitForTraffic(const AtamsTestHarness::TestOptions_t &options);

} /* End Namespace - AtamsTests */

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static const TestCase_t s_testList[] =
{
  /* Platform checks */
  {"HarnessClockDeadline", &AtamsTests::testHarnessClockDeadline},

  /* Router checks */
  {"RouterBroadcast",      &AtamsTests::testRouterBroadcast},
  {"RouterChunking",       &AtamsTests::testRouterChunking},
  {"RouterConnection",     &AtamsTests::testRouterConnection},
  {"RouterTrafficLog",     &AtamsTests::testRouterTrafficLog},
  {"RouterWaitForTraffic", &AtamsTests::testRouterWaitForTraffic},
};

static const TestList_t s_testListInfo {s_testList, static_cast<uint16_t>(sizeof(s_testList) / sizeof(s_testList[0U]))};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

const TestList_t & getTestList(void)
{
  return (s_testListInfo);
}

} /* End Namespace - AtamsTestHarness */

/**
  * @}End of File
  */
