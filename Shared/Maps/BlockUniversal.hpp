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
  VAR_ID_ATAMS_VERSION_MAJOR      = 0U,
  VAR_ID_ATAMS_VERSION_MINOR      = 1U,
  VAR_ID_MAP_GEN_DAY              = 2U,
  VAR_ID_MAP_GEN_MONTH            = 3U,
  VAR_ID_MAP_GEN_YEAR             = 4U,
  VAR_ID_MAP_GEN_HOUR             = 5U,
  VAR_ID_MAP_GEN_MINUTE           = 6U,
  VAR_ID_MAP_GEN_SECOND           = 7U,
  VAR_ID_MAP_CHECKSUM             = 8U,
  VAR_ID_MAP_NUMBER_OF_VARS       = 9U,
  VAR_ID_CONFIGURATION_PASSKEY    = 10U,
  VAR_ID_CONFIGURATION_STATUS     = 11U,
  VAR_ID_NODE_ID                  = 12U,
  VAR_ID_FIRST_NODE_ID            = 13U,
  VAR_ID_LAST_NODE_ID             = 14U,
  VAR_ID_PREVIOUS_NODE_ID         = 15U,
  VAR_ID_BITRATE                  = 16U,
  VAR_ID_WATCHDOG_PERIOD          = 17U,
  VAR_ID_STORE_ALL                = 18U,
  VAR_ID_RESTORE_USER_BLOCKS      = 19U,
  VAR_ID_RESTORE_ALL              = 20U,
  VAR_ID_RESET_NODE               = 21U,
  VAR_ID_STORAGE_STATUS           = 22U,
  VAR_ID_STORAGE_PROCESS_COMPLETE = 23U,
  VAR_ID_WATCHDOG_FAULT_ACTIVE    = 24U,
  VAR_ID_WATCHDOG_RESET           = 25U,
  VAR_ID_CRC_ERROR_COUNT          = 26U,
  VAR_ID_COBS_ERROR_COUNT         = 27U,

  NUMBER_OF_UNIVERSAL_VARS
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

extern const VarInfo_t varInfoList[BlockUniversal::NUMBER_OF_UNIVERSAL_VARS];


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
