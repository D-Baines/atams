/**
  ******************************************************************************
  * @file    SharedPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Shared platform constants and macros for dual-core Atams Node platforms.
  *
  * @details Defines the platform constants and preprocessor macros shared between the
  *          Application Core and Communications Core on dual-core Atams Node platforms.
  *          Includes NODE_NUMBER_OF_VARS for variable storage sizing, the
  *          ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE linker section attribute for
  *          placing shared data in a dedicated memory region accessible by both cores,
  *          and the COMMS_CORE_DETECT macro for identifying the communications core
  *          at compile time.
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
*            12 + (4 * Platform::NODE_NUMBER_OF_VARS) bytes.
*
*   @warning The targeted memory region must be configured as non-cacheable in
*            the MPU (or equivalent) on both cores. If the region is cacheable,
*            each core may read from its own locally cached copy rather than
*            RAM, causing stale reads and silent data corruption.
*
*   @note    ATAMS PLATFORM REQUIREMENT - DUAL-CORE
*/
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE

/**
*   @brief Replace ATAMS_COMMS_CORE in the #ifdef below with the preprocessor symbol your
*          build system defines for the core responsible for Atams communications, or add
*          -D ATAMS_COMMS_CORE to the Comms Core build target only to use it as-is.
*
*          Example: STM32CubeIDE automatically defines CORE_CM4 / CORE_CM7 per core project.
*          If the M4 handles Atams communications, replace ATAMS_COMMS_CORE with CORE_CM4.
*
*   @note  ATAMS PLATFORM REQUIREMENT - DUAL-CORE
*/
#ifdef ATAMS_COMMS_CORE
#define COMMS_CORE_DETECT true
#endif

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** 
*   @brief The number of vars in the Atams Memory Map that will be used to initialise the Node.
*          This value must be less than or equal to @ref Atams::MAX_NUMBER_OF_VARS.
*/
constexpr uint16_t NODE_NUMBER_OF_VARS {Atams::MAX_NUMBER_OF_VARS};


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
