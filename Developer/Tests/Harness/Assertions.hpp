/**
  ******************************************************************************
  * @file    Assertions.hpp
  *
  * @author  D. Baines
  *
  * @brief   Test checks for the Atams host test harness.
  *
  * @details HARNESS_CHECK* record a failure and let the test carry on.
  *          HARNESS_REQUIRE* and HARNESS_FAIL record a failure and end the test by
  *          throwing TestFailure_t, which the test runner catches.
  *
  *          Only use these on the test thread, and never inside a function the
  *          Atams library calls (callbacks, Platform functions). Code on other
  *          threads reports problems with reportBackgroundFailure() instead.
  *
  *          For test and harness code only. The Atams library never uses these.
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
#include <string>
#include <type_traits>

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

#define HARNESS_CHECK(condition) \
  static_cast<void>(AtamsTestHarness::check(static_cast<bool>(condition), __FILE__, __LINE__, "CHECK failed: " #condition))

#define HARNESS_REQUIRE(condition) \
  AtamsTestHarness::require(static_cast<bool>(condition), __FILE__, __LINE__, "REQUIRE failed: " #condition)

#define HARNESS_CHECK_EQ(actual, expected) \
  static_cast<void>(AtamsTestHarness::checkEqual((actual), (expected), __FILE__, __LINE__, "CHECK_EQ failed: " #actual " == " #expected))

#define HARNESS_REQUIRE_EQ(actual, expected) \
  AtamsTestHarness::requireEqual((actual), (expected), __FILE__, __LINE__, "REQUIRE_EQ failed: " #actual " == " #expected)

#define HARNESS_FAIL(message) \
  AtamsTestHarness::fail(__FILE__, __LINE__, (message))

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** @brief Thrown by HARNESS_REQUIRE* and HARNESS_FAIL to end the current test */
struct TestFailure_t {};

/** @brief Stops template argument deduction, so the expected value converts to the actual value's type */
template <typename T>
struct NonDeduced_t
{
  using type = T;
};

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

void recordFailure(const char *file, const int line, const char *message);

void reportBackgroundFailure(const char *file, const int line, const char *message);

bool hasBackgroundFailure(void);

uint32_t getFailureCount(void);

void resetFailures(void);

bool check(const bool condition, const char *file, const int line, const char *message);

void require(const bool condition, const char *file, const int line, const char *message);

[[noreturn]] void fail(const char *file, const int line, const char *message);

/*************************************************************************************/
/* PUBLIC TEMPLATE FUNCTION DEFINITIONS                                              */
/*************************************************************************************/

/**
 * @brief  Convert a checked value to text for a failure message. Integers and enums
 *         print as numbers, so a uint8_t never prints as a character.
 */
template <typename T>
std::string valueToString(const T &value)
{
  static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                "HARNESS_*_EQ only supports integer, enum, bool and floating point values");

  if constexpr (std::is_enum<T>::value)
  {
    return (std::to_string(static_cast<long long>(value)));
  }
  else if constexpr (std::is_floating_point<T>::value)
  {
    return (std::to_string(value));
  }
  else if constexpr (std::is_signed<T>::value)
  {
    return (std::to_string(static_cast<long long>(value)));
  }
  else
  {
    return (std::to_string(static_cast<unsigned long long>(value)));
  }
}

/**
 * @brief  Record a failure if actual and expected differ.
 *
 * @return true if they are equal.
 */
template <typename T>
bool checkEqual(const T                               &actual,
                const typename NonDeduced_t<T>::type  &expected,
                const char                            *file,
                const int                              line,
                const char                            *expression)
{
  if (actual == expected) return (true); /* Early Return */

  const std::string message {std::string(expression) +
                             " (actual "   + valueToString(actual)   +
                             ", expected " + valueToString(expected) + ")"};

  recordFailure(file, line, message.c_str());

  return (false);
}

/**
 * @brief  Record a failure and end the test if actual and expected differ.
 */
template <typename T>
void requireEqual(const T                               &actual,
                  const typename NonDeduced_t<T>::type  &expected,
                  const char                            *file,
                  const int                              line,
                  const char                            *expression)
{
  if (!checkEqual(actual, expected, file, line, expression)) throw TestFailure_t{};
}

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
