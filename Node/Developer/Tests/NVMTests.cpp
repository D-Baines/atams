/**
  ******************************************************************************
  * @file    NVMTests.cpp
  *
  * @author  D. Baines
  *
  * @brief   Manual test entry point for exercising the Node's NVM store/load
  *          behaviour on real hardware.
  *
  * @details Exercises storeAll()/restoreAll()/setVar()/getVar() through the
  *          public CommsCore API. Also exercises NVM automatic migration,
  *          using two real, independently generated Memory Maps
  *          (MapNvmTest1/MapNvmTest2) that share NVM flash content across a
  *          re-init the same way two firmware builds would across a real
  *          reflash - a variable unchanged between the two maps, one renamed,
  *          and one retyped (same byte size, to specifically exercise
  *          type-based rather than length-based mismatch detection).
  *
  *          There is no hardware reset here - re-calling initSingleCore()
  *          mid-run is a valid stand-in for a reboot, since it resets the
  *          Node's RAM-side variable storage and re-runs NVM load against
  *          whatever is actually stored in flash, exactly as a real boot
  *          would.
  *
  *          The embedded target has no printf, so runTests() returns the
  *          number of failed checks instead. To find out which check failed,
  *          set a breakpoint in checkCondition()'s failure branch below -
  *          testName and condition are both in scope there.
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

#include "NVMTests.hpp"

#include "../../CommsCore/CommsCore.hpp"
#include "../../Maps/MapTest/MapTest.hpp"
#include "../../../Shared/Maps/BlockUniversal.hpp"
#include "MapNvmTest1/MapNvmTest1.hpp"
#include "MapNvmTest2/MapNvmTest2.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NVMTests {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

constexpr uint8_t  TEST_NODE_ID          {42U};
constexpr uint8_t  TEST_FIRST_NODE_ID    {1U};
constexpr uint8_t  TEST_LAST_NODE_ID     {10U};
constexpr uint8_t  TEST_PREVIOUS_NODE_ID {41U};
constexpr uint8_t  TEST_BITRATE          {2U};
constexpr uint32_t TEST_WATCHDOG_PERIOD  {5000U};

constexpr uint8_t  TEST_NVM_CONSTANT_VALUE    {99U};
constexpr uint16_t TEST_NVM_NAME_CHANGE_VALUE {999U};

/* Bit pattern of 100.0f, stored under NvmTest1's uint32_t type. If a retype were ever incorrectly
 * reinterpreted instead of defaulted, this would read back as 100.0f under NvmTest2's float type -
 * clearly distinct from DEFAULT_NVM_VAR_TYPE_CHANGE (6.0f), so the failure is unambiguous. */
constexpr uint32_t TEST_NVM_TYPE_CHANGE_VALUE {0x42C80000U};

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t s_testsFailed {0U};

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void checkCondition(const char * const testName, const bool condition)
{
  (void)testName; /* Breakpoint on the line below to catch the failing check - testName is in scope. */

  if (!condition) 
  {
    s_testsFailed++;
  }
}

static void setTestValues(void)
{
  checkCondition("Set VAR_NODE_ID",          Atams::setVar(Atams::BlockUniversal::VAR_NODE_ID,          TEST_NODE_ID)          == Atams::ERROR_NONE);
  checkCondition("Set VAR_FIRST_NODE_ID",    Atams::setVar(Atams::BlockUniversal::VAR_FIRST_NODE_ID,    TEST_FIRST_NODE_ID)    == Atams::ERROR_NONE);
  checkCondition("Set VAR_LAST_NODE_ID",     Atams::setVar(Atams::BlockUniversal::VAR_LAST_NODE_ID,     TEST_LAST_NODE_ID)     == Atams::ERROR_NONE);
  checkCondition("Set VAR_PREVIOUS_NODE_ID", Atams::setVar(Atams::BlockUniversal::VAR_PREVIOUS_NODE_ID, TEST_PREVIOUS_NODE_ID) == Atams::ERROR_NONE);
  checkCondition("Set VAR_BITRATE",          Atams::setVar(Atams::BlockUniversal::VAR_BITRATE,          TEST_BITRATE)          == Atams::ERROR_NONE);
  checkCondition("Set VAR_WATCHDOG_PERIOD",  Atams::setVar(Atams::BlockUniversal::VAR_WATCHDOG_PERIOD,  TEST_WATCHDOG_PERIOD)  == Atams::ERROR_NONE);
}

static void checkValuesMatch(const uint8_t  expectedNodeID,
                             const uint8_t  expectedFirstNodeID,
                             const uint8_t  expectedLastNodeID,
                             const uint8_t  expectedPreviousNodeID,
                             const uint8_t  expectedBitrate,
                             const uint32_t expectedWatchdogPeriod)
{
  uint8_t  nodeID         {0U};
  uint8_t  firstNodeID    {0U};
  uint8_t  lastNodeID     {0U};
  uint8_t  previousNodeID {0U};
  uint8_t  bitrate        {0U};
  uint32_t watchdogPeriod {0U};

  Atams::getVar(Atams::BlockUniversal::VAR_NODE_ID,          nodeID);
  Atams::getVar(Atams::BlockUniversal::VAR_FIRST_NODE_ID,    firstNodeID);
  Atams::getVar(Atams::BlockUniversal::VAR_LAST_NODE_ID,     lastNodeID);
  Atams::getVar(Atams::BlockUniversal::VAR_PREVIOUS_NODE_ID, previousNodeID);
  Atams::getVar(Atams::BlockUniversal::VAR_BITRATE,          bitrate);
  Atams::getVar(Atams::BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod);

  checkCondition("VAR_NODE_ID matches",          nodeID         == expectedNodeID);
  checkCondition("VAR_FIRST_NODE_ID matches",    firstNodeID    == expectedFirstNodeID);
  checkCondition("VAR_LAST_NODE_ID matches",     lastNodeID     == expectedLastNodeID);
  checkCondition("VAR_PREVIOUS_NODE_ID matches", previousNodeID == expectedPreviousNodeID);
  checkCondition("VAR_BITRATE matches",          bitrate        == expectedBitrate);
  checkCondition("VAR_WATCHDOG_PERIOD matches",  watchdogPeriod == expectedWatchdogPeriod);
}

/* MapNvmTest1 and MapNvmTest2 are two independently generated Memory Maps sharing NVM flash
 * content across the re-init below, the same way two firmware builds share flash content across
 * a real reflash. Between them: VAR_NVM_VAR_CONSTANT is unchanged (same name, same type),
 * VAR_NVM_VAR_NAME_CHANGE_1/2 is renamed (different name, so a different hash), and
 * VAR_NVM_VAR_TYPE_CHANGE keeps its name (same hash) but changes type from uint32_t to float. */
static void testNvmMigration(void)
{
  Atams::Error_t seedNvmError {Atams::ERROR_NONE};
  Atams::Error_t error        {Atams::initSingleCore(Atams::MapNvmTest1::memoryMap, seedNvmError)};

  checkCondition("Migration seed init succeeds", error == Atams::ERROR_NONE);

  checkCondition("Set VAR_NVM_VAR_CONSTANT",
                 Atams::setVar(Atams::MapNvmTest1::BlockNvmTest::VAR_NVM_VAR_CONSTANT,
                               TEST_NVM_CONSTANT_VALUE) == Atams::ERROR_NONE);
  checkCondition("Set VAR_NVM_VAR_NAME_CHANGE_1",
                 Atams::setVar(Atams::MapNvmTest1::BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_1,
                               TEST_NVM_NAME_CHANGE_VALUE) == Atams::ERROR_NONE);
  checkCondition("Set VAR_NVM_VAR_TYPE_CHANGE",
                 Atams::setVar(Atams::MapNvmTest1::BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE,
                               TEST_NVM_TYPE_CHANGE_VALUE) == Atams::ERROR_NONE);

  checkCondition("Migration seed storeAll succeeds", Atams::storeAll() == Atams::ERROR_NONE);

  /* Re-init under NvmTest2 - same NVM flash content, a genuinely different Memory Map. */
  Atams::Error_t migratedNvmError {Atams::ERROR_NONE};

  error = Atams::initSingleCore(Atams::MapNvmTest2::memoryMap, migratedNvmError);

  checkCondition("Migration re-init succeeds",    error            == Atams::ERROR_NONE);
  checkCondition("Migration re-init NVM load ok", migratedNvmError == Atams::ERROR_NONE);

  uint8_t  constantValue   {0U};
  uint16_t nameChangeValue {0U};
  float    typeChangeValue {0.0F};

  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_CONSTANT,      constantValue);
  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_2, nameChangeValue);
  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE,   typeChangeValue);

  checkCondition("Unchanged var keeps its stored value across migration",
                 constantValue == TEST_NVM_CONSTANT_VALUE);
  checkCondition("Renamed var resets to its new default, not the old stored value",
                 nameChangeValue == Atams::MapNvmTest2::BlockNvmTest::DEFAULT_NVM_VAR_NAME_CHANGE_2);
  checkCondition("Retyped var resets to its new default, not a reinterpretation of the old bytes",
                 typeChangeValue == Atams::MapNvmTest2::BlockNvmTest::DEFAULT_NVM_VAR_TYPE_CHANGE);

  /* Re-init again under the same (already migrated) map - confirms the post-migration
     compaction rewrite left NVM in a stable state, with no further migration needed. */
  Atams::Error_t stableNvmError {Atams::ERROR_NONE};

  error = Atams::initSingleCore(Atams::MapNvmTest2::memoryMap, stableNvmError);

  checkCondition("Re-init after migration succeeds",    error          == Atams::ERROR_NONE);
  checkCondition("Re-init after migration NVM load ok", stableNvmError == Atams::ERROR_NONE);

  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_CONSTANT,      constantValue);
  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_2, nameChangeValue);
  Atams::getVar(Atams::MapNvmTest2::BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE,   typeChangeValue);

  checkCondition("Post-migration reboot: unchanged var still correct",
                 constantValue == TEST_NVM_CONSTANT_VALUE);
  checkCondition("Post-migration reboot: renamed var still at default",
                 nameChangeValue == Atams::MapNvmTest2::BlockNvmTest::DEFAULT_NVM_VAR_NAME_CHANGE_2);
  checkCondition("Post-migration reboot: retyped var still at default",
                 typeChangeValue == Atams::MapNvmTest2::BlockNvmTest::DEFAULT_NVM_VAR_TYPE_CHANGE);
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

uint16_t runTests(void)
{
  s_testsFailed = 0U;

  /* Inspect in a debugger if the initial load result is of interest - not asserted on,
     since prior test runs on the same hardware can leave NVM in several valid states. */
  Atams::Error_t initialNvmError {Atams::ERROR_NONE};
  Atams::Error_t error           {Atams::initSingleCore(Atams::MapTest::memoryMap, initialNvmError)};

  checkCondition("Initial init succeeds", error == Atams::ERROR_NONE);

  setTestValues();

  checkCondition("storeAll succeeds", Atams::storeAll() == Atams::ERROR_NONE);

  /* Simulate a reboot - re-init reloads NVM contents actually persisted to flash. */
  Atams::Error_t postStoreNvmError {Atams::ERROR_NONE};

  error = Atams::initSingleCore(Atams::MapTest::memoryMap, postStoreNvmError);

  checkCondition("Re-init after storeAll succeeds",    error             == Atams::ERROR_NONE);
  checkCondition("Re-init after storeAll NVM load ok", postStoreNvmError == Atams::ERROR_NONE);

  checkValuesMatch(TEST_NODE_ID,
                   TEST_FIRST_NODE_ID,
                   TEST_LAST_NODE_ID,
                   TEST_PREVIOUS_NODE_ID,
                   TEST_BITRATE,
                   TEST_WATCHDOG_PERIOD);

  checkCondition("restoreAll succeeds", Atams::restoreAll() == Atams::ERROR_NONE);

  /* Simulate a second reboot - confirms restoreAll() actually persisted the defaults. */
  Atams::Error_t postRestoreNvmError {Atams::ERROR_NONE};

  error = Atams::initSingleCore(Atams::MapTest::memoryMap, postRestoreNvmError);

  checkCondition("Re-init after restoreAll succeeds",    error               == Atams::ERROR_NONE);
  checkCondition("Re-init after restoreAll NVM load ok", postRestoreNvmError == Atams::ERROR_NONE);

  checkValuesMatch(Atams::BlockUniversal::DEFAULT_NODE_ID,
                   Atams::BlockUniversal::DEFAULT_FIRST_NODE_ID,
                   Atams::BlockUniversal::DEFAULT_LAST_NODE_ID,
                   Atams::BlockUniversal::DEFAULT_PREVIOUS_NODE_ID,
                   Atams::BlockUniversal::DEFAULT_BITRATE,
                   Atams::BlockUniversal::DEFAULT_WATCHDOG_PERIOD);

  testNvmMigration();

  return (s_testsFailed);
}

} } /* End Namespace - Atams::NVMTests */
