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
#include "DataBlockUniversal.hpp"

/*************************************************************************************/
/* MEMORY MAP NAMESPACE                                                              */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
typedef enum: uint8_t
{
  BLOCK_ID_EXAMPLE1 = 1U,
  BLOCK_ID_EXAMPLE2 = 2U,

  NUMBER_OF_DATA_BLOCKS
} DataBlockID_t;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(void);

Error_t initLimits(void);

/*************************************************************************************/
/* PUBLIC EXTERN CONSTANTS                                                           */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr float    AUTOGEN_ATAMS_VERSION_NUMBER = 0.1F;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_DAY          = 15U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MONTH        = 10U;
inline constexpr uint16_t AUTOGEN_MAP_GEN_YEAR         = 2024U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_HOUR         = 11U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MINUTE       = 33U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_SECOND       = 20U;
inline constexpr uint8_t  AUTOGEN_MAP_NUMBER_OF_BLOCKS = 2U;
inline constexpr uint32_t AUTOGEN_MAP_CHECKSUM         = 32457U;

/*************************************************************************************/
/* DATA BLOCK DEFINITIONS                                                            */
/*************************************************************************************/

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
inline constexpr uint32_t DEFAULT_MEMORY_MAP_GEN_DATE = 49856UL;
inline constexpr float    DEFAULT_MEMORY_MAP_GEN_TIME = 4.0F;
inline constexpr int8_t   DEFAULT_MEMORY_MAP_CHECKSUM = -7;

/*--- Minimum Limits ---*/
inline constexpr uint8_t  MIN_LIMIT_NODE_ID = 0U;

/*--- Maximum Limits ---*/
inline constexpr uint8_t  MAX_LIMIT_NODE_ID = NODE_ID_MAX;

/*--- Descriptor ---*/
inline constexpr DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockExample1::NUMBER_OF_EXAMPLE1_DATA_MEMBERS,
  /* .dataMemberInfo  = */
  {
    [BlockExample1::MEMBER_ID_NODE_ID] =
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_STORE_NVM] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_RESTORE_FACTORY_NVM] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_STORE_OTP] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_WATCHDOG_TIMEOUT] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_DATE] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_TIME] =
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
    [BlockExample1::MEMBER_ID_MEMORY_MAP_CHECKSUM] =
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
  }
};

} /* End Namespace - BlockExample1 */

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
inline constexpr uint32_t DEFAULT_MEMORY_MAP_GEN_DATE = 49856UL;
inline constexpr float    DEFAULT_MEMORY_MAP_GEN_TIME = 4.0F;
inline constexpr int8_t   DEFAULT_MEMORY_MAP_CHECKSUM = -7;

/*--- Minimum Limits ---*/
inline constexpr uint8_t  MIN_LIMIT_NODE_ID = 0U;

/*--- Maximum Limits ---*/
inline constexpr uint8_t  MAX_LIMIT_NODE_ID = NODE_ID_MAX;

/*--- Descriptor ---*/
inline constexpr DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockExample2::NUMBER_OF_EXAMPLE2_DATA_MEMBERS,
  /* .dataMemberInfo  = */
  {
    [BlockExample2::MEMBER_ID_NODE_ID] =
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_STORE_NVM] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_RESTORE_FACTORY_NVM] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_STORE_OTP] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_WATCHDOG_TIMEOUT] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_MEMORY_MAP_GEN_DATE] =
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_MEMORY_MAP_GEN_TIME] =
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
    [BlockExample2::MEMBER_ID_MEMORY_MAP_CHECKSUM] =
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false
    },
  }
};

} /* End Namespace - BlockExample2 */



} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
