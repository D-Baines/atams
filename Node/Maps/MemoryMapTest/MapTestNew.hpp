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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
enum VarID_t: uint16_t
{
  VAR_ID_WRITE_UINT8  = 0U,
  VAR_ID_WRITE_INT8   = 1U,
  VAR_ID_READ_UINT8   = 2U,
  VAR_ID_READ_INT8    = 3U,
  VAR_ID_WRITE_UINT16 = 4U,
  VAR_ID_WRITE_INT16  = 5U,
  VAR_ID_READ_UINT16  = 6U,
  VAR_ID_READ_INT16   = 7U,
  VAR_ID_WRITE_UINT32 = 8U,
  VAR_ID_WRITE_INT32  = 9U,
  VAR_ID_READ_UINT32  = 10U,
  VAR_ID_READ_INT32   = 11U,
  VAR_ID_WRITE_FLOAT  = 12U,
  VAR_ID_READ_FLOAT   = 13U,

  NUMBER_OF_NODE_VARS
};

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
