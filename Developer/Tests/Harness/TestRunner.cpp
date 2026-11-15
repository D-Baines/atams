/**
  ******************************************************************************
  * @file    TestRunner.cpp
  *
  * @author  D. Baines
  *
  * @brief   Test runner and main() for the Atams host test harness.
  *
  * @details A [RUN ] line is printed before each test run and a result line after it,
  *          so a run that never finishes (e.g. a deadlock) is the last [RUN ] line
  *          without a result. Unattended runs rely on the CTest TIMEOUT to end a hang.
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

#include "TestRunner.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <random>
#include <string>

#include "Assertions.hpp"
#include "TestUtilities.hpp"
#include "CommsPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr int EXIT_CODE_PASS  {0};
static constexpr int EXIT_CODE_FAIL  {1};
static constexpr int EXIT_CODE_USAGE {2};

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

struct RunnerSettings_t
{
  const char *testName  {nullptr};
  bool        seedGiven {false};
  uint32_t    seed      {0U};
  bool        listOnly  {false};
  bool        helpOnly  {false};
};

struct RunTotals_t
{
  uint16_t runs   {0U};
  uint16_t passed {0U};
};

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static const char * getModeName(const Mode_t mode)
{
  return ((mode == MODE_THREADED) ? "threaded" : "polling");
}

/* The Node Platforms keep file-scope state, so return each Node copy's to power-on before
   every test. The Hub Platform's state lives in each test's Atams::Bus objects. */
static void resetNodePlatforms(void)
{
  Atams::NodePlatform::resetForNextTest();
}

static void printUsage(void)
{
  std::printf("Usage: AtamsHostTests [options]\n"
              "  --test <name>  Run only this test\n"
              "  --seed <N>     Seed for randomised tests (random if absent)\n"
              "  --list         Print the registered tests\n"
              "  --help         Print this message\n");
}

static bool parseSeed(const char *text, uint32_t &seed)
{
  char                    *end   {nullptr};
  const unsigned long long value {std::strtoull(text, &end, 10)};

  if ((end == text) || (*end != '\0') || (value > UINT32_MAX)) return (false); /* Early Return */

  seed = static_cast<uint32_t>(value);

  return (true);
}

static bool parseArguments(const int argc, char *argv[], RunnerSettings_t &settings)
{
  for (int argIndex {1}; argIndex < argc; argIndex++)
  {
    const char *arg      {argv[argIndex]};
    const bool  hasValue {(argIndex + 1) < argc};

    if ((std::strcmp(arg, "--test") == 0) && hasValue)
    {
      argIndex++;
      settings.testName = argv[argIndex];
    }
    else if ((std::strcmp(arg, "--seed") == 0) && hasValue)
    {
      argIndex++;
      settings.seedGiven = true;

      if (!parseSeed(argv[argIndex], settings.seed)) return (false); /* Early Return */
    }
    else if (std::strcmp(arg, "--list") == 0)
    {
      settings.listOnly = true;
    }
    else if (std::strcmp(arg, "--help") == 0)
    {
      settings.helpOnly = true;
    }
    else
    {
      return (false); /* Early Return */
    }
  }

  return (true);
}

static const TestCase_t * findTest(const TestList_t &testList, const char *name)
{
  for (uint16_t testIndex {0U}; testIndex < testList.count; testIndex++)
  {
    if (std::strcmp(testList.cases[testIndex].name, name) == 0) return (&testList.cases[testIndex]); /* Early Return */
  }

  return (nullptr);
}

static void printTestList(const TestList_t &testList)
{
  for (uint16_t testIndex {0U}; testIndex < testList.count; testIndex++)
  {
    const TestCase_t &testCase {testList.cases[testIndex]};

    std::printf("%s%s%s\n",
                testCase.name,
                ((testCase.modes & MODES_POLLING)  != 0U) ? " [polling]"  : "",
                ((testCase.modes & MODES_THREADED) != 0U) ? " [threaded]" : "");
  }
}

static void callTestFunction(const TestCase_t &testCase, const TestOptions_t &options)
{
  try
  {
    testCase.function(options);
  }
  catch (const TestFailure_t &)
  {
    /* Do Nothing - Failure already recorded by the failing REQUIRE or FAIL */
  }
  catch (const std::exception &exception)
  {
    const std::string message {std::string("Unexpected exception: ") + exception.what()};
    recordFailure(__FILE__, __LINE__, message.c_str());
  }
  catch (...)
  {
    recordFailure(__FILE__, __LINE__, "Unexpected exception of unknown type");
  }
}

static bool runTest(const TestCase_t &testCase, const Mode_t mode, const uint32_t seed)
{
  const TestOptions_t options {mode, seed};

  resetFailures();

  resetNodePlatforms();

  std::printf("[RUN ] %s [%s]\n", testCase.name, getModeName(mode));
  static_cast<void>(std::fflush(stdout));

  const uint32_t startTime {getMillis()};

  callTestFunction(testCase, options);

  const uint32_t elapsedTime {getMillis() - startTime};
  const bool     passed      {(getFailureCount() == 0U) && !hasBackgroundFailure()};

  std::printf("[%s] %s [%s] (%u ms)\n",
              passed ? "PASS" : "FAIL",
              testCase.name,
              getModeName(mode),
              static_cast<unsigned int>(elapsedTime));
  static_cast<void>(std::fflush(stdout));

  return (passed);
}

static void runTestInSelectedModes(const TestCase_t &testCase, const RunnerSettings_t &settings, RunTotals_t &totals)
{
  const Mode_t modes[] {MODE_POLLING, MODE_THREADED};

  for (const Mode_t mode : modes)
  {
    const uint8_t modeBit  {static_cast<uint8_t>(1U << mode)};
    if ((testCase.modes & modeBit) != 0U)
    {
      totals.runs++;
      if (runTest(testCase, mode, settings.seed)) totals.passed++;
    }
  }
}

static int runTestExecutable(const int argc, char *argv[])
{
  RunnerSettings_t settings;

  if (!parseArguments(argc, argv, settings))
  {
    printUsage();
    return (EXIT_CODE_USAGE); /* Early Return */
  }

  if (settings.helpOnly)
  {
    printUsage();
    return (EXIT_CODE_PASS); /* Early Return */
  }

  const TestList_t &testList {getTestList()};

  if (settings.listOnly)
  {
    printTestList(testList);
    return (EXIT_CODE_PASS); /* Early Return */
  }

  const TestCase_t *selectedTest {nullptr};

  if (settings.testName != nullptr)
  {
    selectedTest = findTest(testList, settings.testName);

    if (selectedTest == nullptr)
    {
      std::printf("Unknown test: %s (use --list to see the registered tests)\n", settings.testName);
      return (EXIT_CODE_USAGE); /* Early Return */
    }
  }

  if (!settings.seedGiven)
  {
    std::random_device randomDevice;
    settings.seed = static_cast<uint32_t>(randomDevice());
  }

  std::printf("Seed: %u (rerun with --seed %u)\n",
              static_cast<unsigned int>(settings.seed),
              static_cast<unsigned int>(settings.seed));

  RunTotals_t totals;

  if (selectedTest != nullptr)
  {
    runTestInSelectedModes(*selectedTest, settings, totals);
  }
  else
  {
    for (uint16_t testIndex {0U}; testIndex < testList.count; testIndex++)
    {
      runTestInSelectedModes(testList.cases[testIndex], settings, totals);
    }
  }

  const uint16_t failed {static_cast<uint16_t>(totals.runs - totals.passed)};

  std::printf("%u runs: %u passed, %u failed\n",
              static_cast<unsigned int>(totals.runs),
              static_cast<unsigned int>(totals.passed),
              static_cast<unsigned int>(failed));

  return ((failed == 0U) ? EXIT_CODE_PASS : EXIT_CODE_FAIL);
}

} /* End Namespace - AtamsTestHarness */

/*************************************************************************************/
/* MAIN                                                                              */
/*************************************************************************************/

int main(int argc, char *argv[])
{
  return (AtamsTestHarness::runTestExecutable(argc, argv));
}

/**
  * @}End of File
  */
