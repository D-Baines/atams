/**
  ******************************************************************************
  * @file    Assertions.cpp
  *
  * @author  D. Baines
  *
  * @brief   Test checks for the Atams host test harness.
  *
  * @details To stop in a debugger at a failing check, set a breakpoint in
  *          recordFailure(), or enable breaking on thrown C++ exceptions to stop at
  *          a failing REQUIRE before the stack unwinds.
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

#include "Assertions.hpp"

#include <atomic>
#include <cstdio>
#include <mutex>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static std::atomic<uint32_t> s_failureCount           {0U};
static std::atomic<uint32_t> s_backgroundFailureCount {0U};
static std::mutex            s_outputMutex;

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void printFailure(const char *prefix, const char *file, const int line, const char *message)
{
  std::lock_guard<std::mutex> lock(s_outputMutex);

  std::printf("  %s:%d: %s%s\n", file, line, prefix, message);
  static_cast<void>(std::fflush(stdout));
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief Record a failure on the test thread. Every check failure passes through
 *        here, so a breakpoint in this function catches all of them.
 */
void recordFailure(const char *file, const int line, const char *message)
{
  s_failureCount.fetch_add(1U);

  printFailure("", file, line, message);
}

/**
 * @brief Record a failure from a thread other than the test thread. Never throws.
 *        The test fails at its next StepLoop step, or when it returns.
 */
void reportBackgroundFailure(const char *file, const int line, const char *message)
{
  s_backgroundFailureCount.fetch_add(1U);

  printFailure("background: ", file, line, message);
}

bool hasBackgroundFailure(void)
{
  return (s_backgroundFailureCount.load() > 0U);
}

uint32_t getFailureCount(void)
{
  return (s_failureCount.load());
}

/**
 * @brief Clear all recorded failures. Called by the test runner before each test run.
 */
void resetFailures(void)
{
  s_failureCount.store(0U);
  s_backgroundFailureCount.store(0U);
}

/**
 * @brief  Record a failure if the condition is false.
 *
 * @return The condition.
 */
bool check(const bool condition, const char *file, const int line, const char *message)
{
  if (!condition) recordFailure(file, line, message);

  return (condition);
}

/**
 * @brief Record a failure and end the test if the condition is false.
 */
void require(const bool condition, const char *file, const int line, const char *message)
{
  if (!check(condition, file, line, message)) throw TestFailure_t{};
}

/**
 * @brief Record a failure and end the test.
 */
void fail(const char *file, const int line, const char *message)
{
  recordFailure(file, line, message);

  throw TestFailure_t{};
}

} /* End Namespace - AtamsTestHarness */

/**
  * @}End of File
  */
