/**
  ******************************************************************************
  * @file    HubTests.cpp
  *
  * @author  D. Baines
  *
  * @brief
  *
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "HILTests.hpp"
#include "../../Hub/Bus.hpp"
#include "TestNode.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace HILTests {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

inline constexpr uint8_t NUMBER_OF_ERRORS_TO_INJECT  = 5U;
inline constexpr uint8_t ERROR_INJECTION_PROBABILITY = 20U;

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static asio::io_context ioContext_;

static asio::serial_port serialPort_(ioContext_);

static const Atams::Platform::BusPeripheral::UserData_t userData_ = 
{
  .ioContext  {ioContext_},
  .serialPort {serialPort_}
};

/*************************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/

static void errorHandler(const Atams::Error_t error, const char * errorMessage);

/*************************************************************************************/
/* PRIVATE OBJECTS                                                                   */
/*************************************************************************************/

static Atams::Bus      testBus_(userData_);
static Atams::TestNode testNode1_(0U, errorHandler);
static Atams::TestNode testNode2_(1U, errorHandler);
static Atams::TestNode testNode3_(2U, errorHandler);

static Atams::TestNode *testNodes[] = 
{
  &testNode1_,
  &testNode2_
};

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void errorHandler(const Atams::Error_t error, const char * errorMessage)
{
  printf("Atams Tests Failed with Error: %d\n", error);

  if (errorMessage != nullptr) printf("Message: %s\n", errorMessage);

  tcflush(serialPort_.lowest_layer().native_handle(), TCIOFLUSH);

  serialPort_.cancel();

  serialPort_.close();

  for(;;) {}; /* Infinite Loop */
}

static void testBusInit(void)
{
  Atams::Error_t      error         = Atams::ERROR_NONE;
  Atams::Error_t      expectedError = Atams::ERROR_NONE;
  Atams::ProcessState initState     = Atams::ProcessState::IN_PROGRESS;

  expectedError = Atams::ERROR_BUS_EMPTY;
  error         = testBus_.beginBusInitProcess();
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::beginBusInitProcess");

  expectedError = Atams::ERROR_NONE;
  error         = testBus_.addNodeToBus(testNode1_);
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::addNodeToBus");

  expectedError = Atams::ERROR_NODE_ALREADY_ON_BUS;
  error         = testBus_.addNodeToBus(testNode1_);
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::addNodeToBus");

  expectedError = Atams::ERROR_NONE;
  error         = testBus_.addNodeToBus(testNode2_);
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::addNodeToBus");

  expectedError = Atams::ERROR_BUS_FULL;
  error         = testBus_.addNodeToBus(testNode3_);
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::addNodeToBus");

  expectedError = Atams::ERROR_INIT_ORDER;
  error         = testBus_.beginUpdateCycle();
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::beginUpdateCycle");

  expectedError  = Atams::ERROR_INIT_ORDER;
  error          = Atams::ERROR_NONE;
  initState      = testBus_.runUpdateCycleAsync(error);
  if ((initState != Atams::ProcessState::ERROR) ||
      (error     != expectedError             ) ) errorHandler(error, "Unexpected Return from Bus::runUpdateCycleAsync");

  expectedError  = Atams::ERROR_INIT_ORDER;
  error          = Atams::ERROR_NONE;
  initState      = testBus_.runUpdateCycleSync(error);
  if ((initState != Atams::ProcessState::ERROR) ||
      (error     != expectedError             ) ) errorHandler(error, "Unexpected Return from Bus::runUpdateCycleSync");

  expectedError = Atams::ERROR_INIT_ORDER;
  error          = Atams::ERROR_NONE;
  initState     = testBus_.updateBusInitProcess(error);
  if ((initState != Atams::ProcessState::ERROR) ||
      (error     != expectedError             ) ) errorHandler(error, "Unexpected Return from Bus::updateBusInitProcess");

  expectedError = Atams::ERROR_NONE;
  error         = testBus_.beginBusInitProcess();
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::beginBusInitProcess");

  do 
  {
    initState = testBus_.updateBusInitProcess(error);
  }
  while (initState == Atams::ProcessState::IN_PROGRESS);

  if ((initState == Atams::ProcessState::ERROR) || 
      (error     != expectedError             ) ) 
  {
    errorHandler(error, "Unexpected Return from Bus::updateBusInitProcess");
  }

  expectedError = Atams::ERROR_NONE;
  error         = testBus_.beginUpdateCycle();
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::beginUpdateCycle");

  expectedError = Atams::ERROR_UPDATE_CYCLE_IN_PROGRESS;
  error         = testBus_.beginUpdateCycle();
  if (error != expectedError) errorHandler(error, "Unexpected Error Return from Bus::beginUpdateCycle");

  expectedError = Atams::ERROR_NONE;
  error         = Atams::ERROR_NONE;

  do 
  {
    initState = testBus_.runUpdateCycleSync(error);
  }
  while (initState == Atams::ProcessState::IN_PROGRESS);

  if ((initState == Atams::ProcessState::ERROR) || 
      (error     != expectedError             ) ) 
  {
    errorHandler(error, "Unexpected Return from Bus::runUpdateCycleSync");
  }
}

static Atams::Error_t runUpdateCycleTests(void)
{
  Atams::Error_t error = testBus_.beginUpdateCycle();

  if (error) errorHandler(error, "Begin Update Cycle Failed");

  for(;;)
  { 
    Atams::ProcessState updateState = testBus_.runUpdateCycleSync(error);

    if (updateState != Atams::ProcessState::IN_PROGRESS)
    { 
      if (updateState != Atams::ProcessState::COMPLETE) errorHandler(error, "Unexpected Update Cycle Error");

      error = testBus_.processBuffers();
     
      for (TestNode *&testNodePtr : testNodes)
      {
        if (error == testNodePtr->getExpectedBusError())
        {
          error = Atams::ERROR_NONE;
        }
      } 
      
      if (error) errorHandler(error, "Unexpected Bus Error"); 
      
      for (TestNode *&testNodePtr : testNodes)
      {
        testNodePtr->runUpdateCycleTests();
      }
     

      testBus_.beginUpdateCycle();
    }
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

void runTests(void)
{
  serialPort_.open("/dev/cu.usbserial-AQ02Y2T5");
  serialPort_.set_option(asio::serial_port_base::baud_rate(230400));
  serialPort_.set_option(asio::serial_port::character_size(8));
  serialPort_.set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
  serialPort_.set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));
  serialPort_.set_option(asio::serial_port::parity(asio::serial_port::parity::none));
  tcflush(serialPort_.lowest_layer().native_handle(), TCIOFLUSH);

  Atams::Error_t error = testNode1_.initMemoryMap(Atams::MapTest::memoryMap);

  if (!error) error = testNode2_.initMemoryMap(Atams::MapTest::memoryMap);

  if (error) errorHandler(error, "Node Init Failed");

  testNode1_.runFunctionArgTests();

  testBusInit();

  runUpdateCycleTests();
}


} } /* End Namespace - Atams::HubTests */

/**
  * @}End of File
  */
