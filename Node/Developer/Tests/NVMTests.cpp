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
  *          public CommsCore API, against today's positional NVM packing.
  *          This is a baseline: it is intended to keep passing unchanged as
  *          NVM automatic migration is introduced, before any migration
  *          scenario tests are added.
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

  if (!condition) s_testsFailed++;
}

static void setTestValues(void)
{
  checkCondition("Set VAR_NODE_ID",          Atams::setVar<uint8_t> (Atams::BlockUniversal::VAR_NODE_ID,          TEST_NODE_ID)          == Atams::ERROR_NONE);
  checkCondition("Set VAR_FIRST_NODE_ID",    Atams::setVar<uint8_t> (Atams::BlockUniversal::VAR_FIRST_NODE_ID,    TEST_FIRST_NODE_ID)    == Atams::ERROR_NONE);
  checkCondition("Set VAR_LAST_NODE_ID",     Atams::setVar<uint8_t> (Atams::BlockUniversal::VAR_LAST_NODE_ID,     TEST_LAST_NODE_ID)     == Atams::ERROR_NONE);
  checkCondition("Set VAR_PREVIOUS_NODE_ID", Atams::setVar<uint8_t> (Atams::BlockUniversal::VAR_PREVIOUS_NODE_ID, TEST_PREVIOUS_NODE_ID) == Atams::ERROR_NONE);
  checkCondition("Set VAR_BITRATE",          Atams::setVar<uint8_t> (Atams::BlockUniversal::VAR_BITRATE,          TEST_BITRATE)          == Atams::ERROR_NONE);
  checkCondition("Set VAR_WATCHDOG_PERIOD",  Atams::setVar<uint32_t>(Atams::BlockUniversal::VAR_WATCHDOG_PERIOD,  TEST_WATCHDOG_PERIOD)  == Atams::ERROR_NONE);
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

  Atams::getVar<uint8_t> (Atams::BlockUniversal::VAR_NODE_ID,          nodeID);
  Atams::getVar<uint8_t> (Atams::BlockUniversal::VAR_FIRST_NODE_ID,    firstNodeID);
  Atams::getVar<uint8_t> (Atams::BlockUniversal::VAR_LAST_NODE_ID,     lastNodeID);
  Atams::getVar<uint8_t> (Atams::BlockUniversal::VAR_PREVIOUS_NODE_ID, previousNodeID);
  Atams::getVar<uint8_t> (Atams::BlockUniversal::VAR_BITRATE,          bitrate);
  Atams::getVar<uint32_t>(Atams::BlockUniversal::VAR_WATCHDOG_PERIOD,  watchdogPeriod);

  checkCondition("VAR_NODE_ID matches",          nodeID         == expectedNodeID);
  checkCondition("VAR_FIRST_NODE_ID matches",    firstNodeID    == expectedFirstNodeID);
  checkCondition("VAR_LAST_NODE_ID matches",     lastNodeID     == expectedLastNodeID);
  checkCondition("VAR_PREVIOUS_NODE_ID matches", previousNodeID == expectedPreviousNodeID);
  checkCondition("VAR_BITRATE matches",          bitrate        == expectedBitrate);
  checkCondition("VAR_WATCHDOG_PERIOD matches",  watchdogPeriod == expectedWatchdogPeriod);
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

  return (s_testsFailed);
}

} } /* End Namespace - Atams::NVMTests */
