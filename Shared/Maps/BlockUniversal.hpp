/**
  ******************************************************************************
  * @file    BlockUniversal.hpp
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
#include "../../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

enum VarID_t: uint16_t
{
  VAR_ATAMS_VERSION_MAJOR      = 0U,
  VAR_ATAMS_VERSION_MINOR      = 1U,
  VAR_ATAMS_VERSION_PATCH      = 2U,
  VAR_MAP_GEN_DAY              = 3U,
  VAR_MAP_GEN_MONTH            = 4U,
  VAR_MAP_GEN_YEAR             = 5U,
  VAR_MAP_GEN_HOUR             = 6U,
  VAR_MAP_GEN_MINUTE           = 7U,
  VAR_MAP_GEN_SECOND           = 8U,
  VAR_MAP_CHECKSUM             = 9U,
  VAR_MAP_NUMBER_OF_VARS       = 10U,
  VAR_MAX_BUS_PACKET_SIZE      = 11U,
  VAR_CONFIGURATION_PASSKEY    = 12U,
  VAR_CONFIGURATION_STATUS     = 13U,
  VAR_NODE_ID                  = 14U,
  VAR_FIRST_NODE_ID            = 15U,
  VAR_LAST_NODE_ID             = 16U,
  VAR_PREVIOUS_NODE_ID         = 17U,
  VAR_BITRATE                  = 18U,
  VAR_WATCHDOG_PERIOD          = 19U,
  VAR_STORE_ALL                = 20U,
  VAR_RESTORE_USER_BLOCKS      = 21U,
  VAR_RESTORE_ALL              = 22U,
  VAR_RESET_NODE               = 23U,
  VAR_STORAGE_STATUS           = 24U,
  VAR_STORAGE_PROCESS_COMPLETE = 25U,
  VAR_WATCHDOG_FAULT_ACTIVE    = 26U,
  VAR_WATCHDOG_RESET           = 27U,
  VAR_CRC_ERROR_COUNT          = 28U,
  VAR_COBS_ERROR_COUNT         = 29U,

  NUMBER_OF_VARS
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint8_t  DEFAULT_NODE_ID          {0};
inline constexpr uint8_t  DEFAULT_FIRST_NODE_ID    {0};
inline constexpr uint8_t  DEFAULT_LAST_NODE_ID     {0};
inline constexpr uint8_t  DEFAULT_PREVIOUS_NODE_ID {0};
inline constexpr uint8_t  DEFAULT_BITRATE          {0};
inline constexpr uint32_t DEFAULT_WATCHDOG_PERIOD  {0};

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const VarInfo_t varInfoList[BlockUniversal::NUMBER_OF_VARS];


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
