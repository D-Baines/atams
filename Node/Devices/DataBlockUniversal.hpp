/**
  ******************************************************************************
  * @file    DataBlockUniversal.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Universal.
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
#include "../DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/*--- Member List ---*/
typedef enum: uint16_t
{
  MEMBER_ID_ATAMS_VERSION_NUMBER  = 0U,
  MEMBER_ID_UNIVERSAL_UNLOCK      = 1U,
  MEMBER_ID_MAP_GEN_DAY           = 2U,
  MEMBER_ID_MAP_GEN_MONTH         = 3U,
  MEMBER_ID_MAP_GEN_YEAR          = 4U,
  MEMBER_ID_MAP_GEN_HOUR          = 5U,
  MEMBER_ID_MAP_GEN_MINUTE        = 6U,
  MEMBER_ID_MAP_GEN_SECOND        = 7U,
  MEMBER_ID_MAP_CHECKSUM          = 8U,
  MEMBER_ID_NODE_ID               = 9U,
  MEMBER_ID_FIRST_NODE_ID         = 10U,
  MEMBER_ID_LAST_NODE_ID          = 11U,
  MEMBER_ID_PREVIOUS_NODE_ID      = 12U,
  MEMBER_ID_WATCHDOG_TIMEOUT      = 13U,
  MEMBER_ID_WATCHDOG_COUNTER      = 14U,
  MEMBER_ID_WATCHDOG_FAULT_ACTIVE = 15U,
  MEMBER_ID_WATCHDOG_RESET        = 16U,
  MEMBER_ID_CRC_ERROR_COUNT       = 17U,
  MEMBER_ID_COBS_ERROR_COUNT      = 18U,
  MEMBER_ID_STORE_NVM             = 19U,
  MEMBER_ID_RESTORE_FACTORY_NVM   = 20U,
  MEMBER_ID_NVM_STATUS            = 21U,

  NUMBER_OF_UNIVERSAL_DATA_MEMBERS
} DataMemberID_t;

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*--- Defaults ---*/
inline constexpr uint8_t  DEFAULT_NODE_ID          = 0U;
inline constexpr uint8_t  DEFAULT_FIRST_NODE_ID    = 0U;
inline constexpr uint8_t  DEFAULT_LAST_NODE_ID     = 0U;
inline constexpr uint8_t  DEFAULT_PREVIOUS_NODE_ID = 0U;
inline constexpr uint32_t DEFAULT_WATCHDOG_TIMEOUT = 0UL;


/*--- Minimum Limits ---*/
inline constexpr uint8_t  MIN_LIMIT_NODE_ID          = 0U;
inline constexpr uint8_t  MIN_LIMIT_FIRST_NODE_ID    = 0U;
inline constexpr uint8_t  MIN_LIMIT_LAST_NODE_ID     = 0U;
inline constexpr uint8_t  MIN_LIMIT_PREVIOUS_NODE_ID = 0U;


/*--- Maximum Limits ---*/
inline constexpr uint8_t  MAX_LIMIT_NODE_ID          = NODE_ID_MAX;
inline constexpr uint8_t  MAX_LIMIT_FIRST_NODE_ID    = NODE_ID_MAX;
inline constexpr uint8_t  MAX_LIMIT_LAST_NODE_ID     = NODE_ID_MAX;
inline constexpr uint8_t  MAX_LIMIT_PREVIOUS_NODE_ID = NODE_ID_MAX;


/*--- Descriptor ---*/
inline constexpr DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockUniversal::NUMBER_OF_UNIVERSAL_DATA_MEMBERS,
  /* .initDefaults    = */ nullptr, 
  /* .dataMemberInfo  = */
  {
    /* [BlockUniversal::MEMBER_ID_ATAMS_VERSION_NUMBER] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_UNIVERSAL_UNLOCK] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_DAY] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_MONTH] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_YEAR] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_HOUR] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_MINUTE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_SECOND] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_CHECKSUM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ true,
    },
    /* [BlockUniversal::MEMBER_ID_FIRST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ true,
    },
    /* [BlockUniversal::MEMBER_ID_LAST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ true,
    },
    /* [BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ true,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ true,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_COUNTER] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_FAULT_ACTIVE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_RESET] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_CRC_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_COBS_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_STORE_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_RESTORE_FACTORY_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::MEMBER_ID_NVM_STATUS] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
  }
};

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(DataBlock &blockToInit);

Error_t initLimits(DataBlock &blockToInit);


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
