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
