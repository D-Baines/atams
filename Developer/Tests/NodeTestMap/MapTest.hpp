/**
  ******************************************************************************
  * @file    MemoryMapTest.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Memory Map with name: Test.
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

#include "../../../Node/Developer/NodeTypedefs.hpp"
#include "../../../Shared/Maps/BlockUniversal.hpp"
#include "BlockTest1.hpp"
#include "BlockTest2.hpp"
#include "BlockTest3.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/* Minimum Platform::NVM_STORAGE_SIZE required to store this map's NVMStorage variables. */
constexpr uint32_t REQUIRED_NVM_SIZE {162U};

/* Autogen's own mirrored size constants (used to compute REQUIRED_NVM_SIZE above) must match
 * the Atams library this map is being compiled against - if these fail, regenerate this map
 * against the current Atams library. */
static_assert(9U == Atams::NVM_HEADER_SIZE,           "Autogen NVM_HEADER_SIZE mirror out of sync with Atams::NVM_HEADER_SIZE");
static_assert(8U == Atams::NVM_FOOTER_SIZE,           "Autogen NVM_FOOTER_SIZE mirror out of sync with Atams::NVM_FOOTER_SIZE");
static_assert(5U == Atams::NVM_VAR_ENTRY_HEADER_SIZE, "Autogen NVM_VAR_ENTRY_HEADER_SIZE mirror out of sync with Atams::NVM_VAR_ENTRY_HEADER_SIZE");

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
