/**
  ******************************************************************************
  * @file    DataBlockExample1.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Example1.
  *          The Data Block is part of an Atams Memory Map with name: Test.
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
#include "../../DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample1 {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

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

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

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
    /* [BlockExample1::MEMBER_ID_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_STORE_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_RESTORE_FACTORY_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_STORE_OTP] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_WATCHDOG_TIMEOUT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_DATE] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_TIME] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_MEMORY_MAP_CHECKSUM] = */
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_READ,
    },
  }
};



/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(Node &nodeToInit);

Error_t initLimits(Node &nodeToInit);


} } } /* End Namespace - Atams::MapTest::BlockExample1 */

/**
  * @}End of File
  */
