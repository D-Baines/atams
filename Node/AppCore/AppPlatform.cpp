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

/*************************************************************************************/
/* ATAMS INCLUDES                                                                    */
/*************************************************************************************/

#include "AppPlatform.hpp"

/*************************************************************************************/
/* USER INCLUDES                                                                     */
/*************************************************************************************/

#include "main.h"
#include "stm32h7xx_hal.h"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

uint32_t getMillis(void)
{
  return (HAL_GetTick());
}

void acquireVarStorageLock(void)
{
  while (HAL_HSEM_FastTake(0U) != HAL_OK)
  {
    /* Wait */
  };
}

void releaseVarStorageLock(void)
{
  HAL_HSEM_Release(0U, 0);
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
