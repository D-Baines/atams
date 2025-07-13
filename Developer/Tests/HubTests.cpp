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

#include "HubTests.hpp"
#include "../../Hub/Bus.hpp"
#include "TestNode.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace HubTests {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

const Atams::Platform::BusPeripheral::UserData_t userData_;

/*************************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/

static void errorHandler(const Atams::Error_t error, const char * errorMessage);

/*************************************************************************************/
/* PRIVATE OBJECTS                                                                   */
/*************************************************************************************/

Atams::Bus      testBus_(userData_);
Atams::TestNode testNode1_(0U, errorHandler);
Atams::TestNode testNode2_(1U, errorHandler);

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void errorHandler(const Atams::Error_t error, const char * errorMessage)
{
  printf("Atams Tests Failed with Error: %d\n", error);

  if (errorMessage != nullptr) printf("Message: %s\n", errorMessage);

  while(true) {}; /* Infinite Loop */
}

static Atams::Error_t testBusInit(void)
{
  Atams::Error_t      error     = Atams::ERROR_NONE;
  Atams::ProcessState initState = Atams::ProcessState::IN_PROGRESS;

  while (initState == Atams::ProcessState::IN_PROGRESS)
  {
    initState = testBus_.updateBusInitProcess(error);
  }

  if ((initState == Atams::ProcessState::ERROR) || 
      (error     != Atams::ERROR_NONE         ) ) 
  {
    errorHandler(error, "Bus Init Failed");
  }
}

static Atams::Error_t runUpdateCycleTests(void)
{
  Atams::Error_t error = testBus_.beginUpdateCycle();

  if (error) errorHandler(error, "Begin Update Cycle Failed");

  while(true)
  { 
    Atams::ProcessState updateState = testBus_.runUpdateCycleSync(error);

    if (updateState != Atams::ProcessState::IN_PROGRESS)
    { 
      if (updateState != Atams::ProcessState::COMPLETE) errorHandler(error, "Update Cycle Failed");

      error = testBus_.processBuffers();

      if (error != Atams::ERROR_NONE) errorHandler(error, "Process Buffers Failed");

      testNode1_.runUpdateCycleTests();
      testNode2_.runUpdateCycleTests();

      testBus_.beginUpdateCycle();
    }
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

void runTests(void)
{
  testNode1_.runFunctionArgTests();

  testBusInit();

  runUpdateCycleTests();
}


} } /* End Namespace - Atams::HubTests */

/**
  * @}End of File
  */
