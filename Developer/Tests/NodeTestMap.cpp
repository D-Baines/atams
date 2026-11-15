/**
  ******************************************************************************
  * @file    NodeTestMap.cpp
  *
  * @author  D. Baines
  *
  * @brief   Access to the Node MapTest Memory Map for test files.
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

#include "NodeTestMap.hpp"

#include "CommsPlatform.hpp"
#include "Maps/Node/MapTest/MapTest.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTests {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

/* The harness NVM_STORAGE_SIZE is a fixed platform maximum, so check the test map fits it */
static_assert((Atams::MapTest::REQUIRED_NVM_SIZE + Atams::NodePlatform::NVM_UNIT_SIZE) <= Atams::NodePlatform::NVM_STORAGE_SIZE,
              "Harness NVM_STORAGE_SIZE is too small for the Node MapTest Memory Map");

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

const Atams::MemoryMap_t & getNodeTestMap(void)
{
  return (Atams::MapTest::nodeMemoryMap);
}

} /* End Namespace - AtamsTests */


/**
  * @}End of File
  */
