/**
  ******************************************************************************
  * @file    MemoryMapTest.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Memory Map with name: Test.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../../Node.hpp"
#include "../BlockUniversal.hpp"
#include "BlockExample1.hpp"
#include "BlockExample2.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
enum DataBlockID_t: uint8_t
{
  BLOCK_ID_UNIVERSAL = 0U,
  BLOCK_ID_EXAMPLE1  = 1U,
  BLOCK_ID_EXAMPLE2  = 2U,

  NUMBER_OF_DATA_BLOCKS
};

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
