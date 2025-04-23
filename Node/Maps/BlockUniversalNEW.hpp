/**
  ******************************************************************************
  * @file    Universal.hpp
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
#include "../Node.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Universal {

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
  VAR_ID_CONFIGURATION_PASSKEY    = 9U,
  VAR_ID_CONFIGURATION_STATUS     = 10U,
  VAR_ID_NODE_ID                  = 11U,
  VAR_ID_FIRST_NODE_ID            = 12U,
  VAR_ID_LAST_NODE_ID             = 13U,
  VAR_ID_PREVIOUS_NODE_ID         = 14U,
  VAR_ID_BITRATE                  = 15U,
  VAR_ID_WATCHDOG_PERIOD          = 16U,
  VAR_ID_STORE_ALL                = 17U,
  VAR_ID_RESTORE_USER_BLOCKS      = 18U,
  VAR_ID_RESTORE_ALL              = 19U,
  VAR_ID_STORAGE_STATUS           = 20U,
  VAR_ID_STORAGE_PROCESS_COMPLETE = 21U,
  VAR_ID_WATCHDOG_FAULT_ACTIVE    = 22U,
  VAR_ID_WATCHDOG_RESET           = 23U,
  VAR_ID_CRC_ERROR_COUNT          = 24U,
  VAR_ID_COBS_ERROR_COUNT         = 25U,

  NUMBER_OF_UNIVERSAL_VARS
};

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const VarInfo_t universalVarInfo[NUMBER_OF_UNIVERSAL_VARS];


} } /* End Namespace - Atams::Universal */

/**
  * @}End of File
  */
