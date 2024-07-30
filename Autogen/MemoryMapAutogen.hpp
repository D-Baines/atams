/**
  ******************************************************************************
  * @file    MemoryMap$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an $$$AUTOGEN$$$FRAMEWORK_NAME$$$AUTOGEN$$$ Memory Map with name: $$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$.
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
#include "../Node.hpp"

/*************************************************************************************/
/* MEMORY MAP NAMESPACE                                                              */
/*************************************************************************************/

namespace SMI { namespace Map$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$ {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
typedef enum: uint8_t
{
$$$AUTOGEN$$$BLOCK_ID_LIST$$$AUTOGEN$$$
  NUMBER_OF_DATA_BLOCKS
} DataBlockID_t;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

$$$AUTOGEN$$$INIT_DEFAULTS_DECLARATION$$$AUTOGEN$$$

$$$AUTOGEN$$$INIT_LIMITS_DECLARATION$$$AUTOGEN$$$

/*************************************************************************************/
/* GLOBAL CONSTANTS                                                                  */
/*************************************************************************************/

extern const Node::MemoryMap_t memoryMap;

/*************************************************************************************/
/* DATA BLOCK DEFINITIONS                                                            */
/*************************************************************************************/

$$$AUTOGEN$$$DATA_BLOCK_DEFINITIONS$$$AUTOGEN$$$
} } /* End Namespace - SMI::Map$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$ */

/**
  * @}End of File
  */