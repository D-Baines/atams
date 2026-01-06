/**
  ******************************************************************************
  * @file    SharedPlatform.hpp
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

#include "../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

/** 
*   @brief Define used in dual-core applications to place shared variables in a
*          dedicated shared memory section accessible by both cores. This section
*          should be defined in the application linker script.
*
*   @details The size of the shared memory section must be at least:
*            2 + (4 * Platform::NODE_NUMBER_OF_VARS) bytes.
*/
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE __attribute__((section(".atamsSharedRAM")))

/**
*   @brief Define used in dual-core applications to determine which core is
*          responsible for handling Atams communications. The define should only
*          exist on compilation of the code for the core handling Atams communications.
*/
#define COMMS_CORE_DETECT CORE_CM4

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** 
*   @brief The number of vars in the Atams Memory Map that will be used to initialise the Node.
*          This value must be less than or equal to @ref Atams::MAX_NUMBER_OF_VARS.
*/
constexpr uint16_t NODE_NUMBER_OF_VARS {100U};


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
