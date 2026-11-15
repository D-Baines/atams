/**
  ******************************************************************************
  * @file    HarnessTypedefs.hpp
  *
  * @author  D. Baines
  *
  * @brief   Types shared by the Atams host test harness and the tests.
  *
  * @details Defines the test run modes, the options passed to each test function,
  *          and the test case entries that make up the test registry.
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
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** @brief How the Nodes and the router are driven during a test run */
enum Mode_t : uint8_t
{
  MODE_POLLING  = 0U, /* Everything stepped from the test thread */
  MODE_THREADED = 1U  /* Nodes and router on their own threads, Hub on the test thread */
};

/** @brief Bitmask of the modes a test runs in by default */
enum ModeSelection_t : uint8_t
{
  MODES_POLLING  = 1U,
  MODES_THREADED = 2U,
  MODES_BOTH     = 3U
};

/** @brief Options passed to every test function */
struct TestOptions_t
{
  Mode_t   mode;
  uint32_t seed;
};

/** @brief Test function signature */
typedef void (*TestFunction_t)(const TestOptions_t &options);

/** @brief One entry in the test registry */
struct TestCase_t
{
  const char      *name;
  TestFunction_t   function;
  ModeSelection_t  modes {MODES_POLLING};
};

/** @brief The test registry, in run order */
struct TestList_t
{
  const TestCase_t *cases;
  uint16_t          count;
};

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
