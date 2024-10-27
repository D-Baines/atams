/**
  ******************************************************************************
  * @file    MemoryMapTest.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing public function definitions
  *          for an Atams Memory Map with name: Test.
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

#include "MemoryMapTest.hpp"
#include "../DataBlockUniversal.hpp"
#include "DataBlockExample1.hpp"
#include "DataBlockExample2.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* GLOBAL CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap(NUMBER_OF_DATA_BLOCKS,                          
                            initDefaults,
                            initLimits,
                            {BlockExample1::blockDescriptor,
                             BlockExample2::blockDescriptor });

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/





} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
