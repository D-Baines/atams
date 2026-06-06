/**
  ******************************************************************************
  * @file    AppPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform implementation for the Atams Node Application Core.
  *
  * @details Implements the platform abstraction layer (PAL) for the Atams Node
  *          Application Core. Function definitions are left empty for the user 
  *          to populate with their platform-specific implementation.
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

/**
 * @brief  Get system time in milliseconds since startup.
 *
 * @return System time in milliseconds since startup.
 *
 * @note   ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
uint32_t getMillis(void)
{
  return (HAL_GetTick());
}

/**
 * @brief   Acquire the lock protecting the Node variable storage from concurrent access.
 *
 * @details This function will be called before Node variable storage access. The variable storage
 *          may be accessed from multiple threads or cores depending on the user's platform. The user 
 *          must ensure that the lock is held until Platform::releaseVarStorageLock() is called.
 *
 *          For multi-threaded platforms, the user should use a mutex or similar mechanism. For dual-core 
 *          platforms, the user should use a hardware semaphore or similar mechanism. A combination of both
 *          may be required for multi-threaded dual-core platforms.
 *         
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 *          ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
void acquireVarStorageLock(void)
{
  while (HAL_HSEM_FastTake(0U) != HAL_OK)
  {
    /* Wait */
  };
}

/**
 * @brief   Release the lock protecting the Node variable storage from concurrent access.
 *
 * @details This function will be called after Node variable storage access. The user must 
 *          release or unlock the mechanism locked in Platform::acquireVarStorageLock().
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 *          ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
void releaseVarStorageLock(void)
{
  HAL_HSEM_Release(0U, 0);
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
