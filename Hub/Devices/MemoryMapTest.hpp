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

Error_t initDefaults(Node &nodeToInit);

Error_t initLimits(Node &nodeToInit);


/*************************************************************************************/
/* GLOBAL CONSTANTS                                                                  */
/*************************************************************************************/

extern const Node::MemoryMap_t memoryMap;


/*************************************************************************************/
/* DATA BLOCK DEFINITIONS                                                            */
/*************************************************************************************/

/*--- DATA BLOCK UNIVERSAL -----------------------------------------------------------*/

namespace BlockUniversal {

/*--- Member List ---*/
typedef enum: uint16_t
{
  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,
  NUMBER_OF_UNIVERSAL_DATA_MEMBERS
} DataMemberID_t;

/*--- Defaults ---*/
constexpr inline uint32_t DEFAULT_MEMORY_MAP_GEN_DATE = AUTOGEN;
constexpr inline uint32_t DEFAULT_MEMORY_MAP_GEN_TIME = AUTOGEN;
constexpr inline uint32_t DEFAULT_MEMORY_MAP_CHECKSUM = AUTOGEN;
/*--- DATA BLOCK EXAMPLE1 -----------------------------------------------------------*/

namespace BlockExample1 {

/*--- Member List ---*/
typedef enum: uint16_t
{
  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,
  NUMBER_OF_EXAMPLE1_DATA_MEMBERS
} DataMemberID_t;

/*--- Defaults ---*/
constexpr inline uint32_t DEFAULT_MEMORY_MAP_GEN_DATE = 49856UL;
constexpr inline float    DEFAULT_MEMORY_MAP_GEN_TIME = 4.0F;
constexpr inline int8_t   DEFAULT_MEMORY_MAP_CHECKSUM = -7;
/*--- DATA BLOCK EXAMPLE2 -----------------------------------------------------------*/

namespace BlockExample2 {

/*--- Member List ---*/
typedef enum: uint16_t
{
  MEMBER_ID_NODE_ID             = 0U,
  MEMBER_ID_STORE_NVM           = 1U,
  MEMBER_ID_RESTORE_FACTORY_NVM = 2U,
  MEMBER_ID_STORE_OTP           = 3U,
  MEMBER_ID_WATCHDOG_TIMEOUT    = 4U,
  MEMBER_ID_MEMORY_MAP_GEN_DATE = 5U,
  MEMBER_ID_MEMORY_MAP_GEN_TIME = 6U,
  MEMBER_ID_MEMORY_MAP_CHECKSUM = 7U,
  NUMBER_OF_EXAMPLE2_DATA_MEMBERS
} DataMemberID_t;

/*--- Defaults ---*/
constexpr inline uint16_t DEFAULT_MEMORY_MAP_GEN_DATE = 64563U;
constexpr inline int32_t  DEFAULT_MEMORY_MAP_GEN_TIME = -655346L;
constexpr inline float    DEFAULT_MEMORY_MAP_CHECKSUM = 0.0001F;



} } /* End Namespace - SMI::MapTest */

/**
  * @}End of File
  */
