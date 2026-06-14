/**
  ******************************************************************************
  * @file    AppPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform interface declarations for the Atams Node Application Core.
  *
  * @details Declares the platform functions required by the Application Core on
  *          dual-core Atams Node platforms. The implementing file must provide
  *          getMillis(), acquireVarStorageLock(), and releaseVarStorageLock(),
  *          which are used to synchronise variable storage access between the
  *          application and communications processor cores.
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

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
