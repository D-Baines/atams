/**
  ******************************************************************************
  * @file    SharedPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform constants and macros shared by the Atams Node Comms Core and App Core.
  *
  * @details Defines the platform constants and preprocessor macros used by both the
  *          Communications Core and, on dual-core platforms, the Application Core.
  *          Includes NODE_NUMBER_OF_VARS for variable storage sizing and the
  *          ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE linker section attribute for
  *          placing shared data in a dedicated memory region accessible by both cores.
  *
  *          This file is a template. Copy the Node Platform files into the application
  *          project, complete them for the target platform, and point
  *          ATAMS_NODE_PLATFORM_DIR at the folder holding them (see Node/CMakeLists.txt).
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

#include "Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NodePlatform {

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

/**
*   @brief Define used in dual-core applications to place shared variables in a
*          dedicated shared memory section accessible by both cores. This section
*          should be defined in the application linker script, for example:
*          __attribute__((section(".atamsSharedRAM"))). Leave empty on single-core
*          platforms.
*
*   @details The size of the shared memory section must be at least:
*            12 + (4 * NodePlatform::NODE_NUMBER_OF_VARS) bytes.
*
*   @warning The targeted memory region must be configured as non-cacheable in
*            the MPU (or equivalent) on both cores. If the region is cacheable,
*            each core may read from its own locally cached copy rather than
*            RAM, causing stale reads and silent data corruption.
*
*   @note    ATAMS PLATFORM REQUIREMENT - DUAL-CORE
*/
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/**
*   @brief The number of vars in the Atams Memory Map that will be used to initialise the Node.
*          This value must be less than or equal to @ref Atams::MAX_NUMBER_OF_VARS.
*/
constexpr uint16_t NODE_NUMBER_OF_VARS {Atams::MAX_NUMBER_OF_VARS};


} } /* End Namespace - Atams::NodePlatform */


/**
  * @}End of File
  */
