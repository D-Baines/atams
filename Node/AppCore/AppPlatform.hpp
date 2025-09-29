/**
  ******************************************************************************
  * @file    AppPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief
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
#include "../../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

/* Dual core shared memory space requirement: 2 + (4 * Platform::NODE_NUMBER_OF_VARS) */
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE __attribute__((section(".atamsSharedRAM")))

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NODE_NUMBER_OF_VARS = 100U;  /* Must be <= Atams::MAX_NUMBER_OF_VARS */

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint32_t getMillis(void);

void acquireVarStorageLock(void);

void releaseVarStorageLock(void);


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
