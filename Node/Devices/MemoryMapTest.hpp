/**
  ******************************************************************************
  * @file    MemoryMapTest.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Memory Map with name: Test.
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

namespace SMI { namespace MapTest {


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
typedef enum: uint8_t
{
  BLOCK_ID_UNIVERSAL = 0U,
  BLOCK_ID_EXAMPLE1  = 1U,
  BLOCK_ID_EXAMPLE2  = 2U,

  NUMBER_OF_DATA_BLOCKS
} DataBlockID_t;


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(void);

Error_t initLimits(void);


/*************************************************************************************/
/* GLOBAL CONSTANTS                                                                  */
/*************************************************************************************/

extern const Node::MemoryMap_t memoryMap;


/*************************************************************************************/
/* DATA BLOCK DEFINITIONS                                                            */
/*************************************************************************************/

/*--- DATA BLOCK TEMPLATE -----------------------------------------------------------*/

namespace BlockUniversal {

/*--- Member List ---*/

  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,
/*--- DATA BLOCK TEMPLATE -----------------------------------------------------------*/

namespace BlockExample1 {

/*--- Member List ---*/

  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,
/*--- DATA BLOCK TEMPLATE -----------------------------------------------------------*/

namespace BlockExample2 {

/*--- Member List ---*/

  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,



} } /* End Namespace - SMI::MapTest */

/**
  * @}End of File
  */
