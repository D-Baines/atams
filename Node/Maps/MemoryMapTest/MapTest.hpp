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
#include "../../Node.hpp"
#include "../BlockUniversal.hpp"
#include "BlockExample1.hpp"
#include "BlockExample2.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
enum DataBlockID_t: uint8_t
{
  BLOCK_ID_UNIVERSAL = 0U,
  BLOCK_ID_EXAMPLE1  = 1U,
  BLOCK_ID_EXAMPLE2  = 2U,

  NUMBER_OF_DATA_BLOCKS
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr float    AUTOGEN_ATAMS_VERSION_NUMBER = 0.1F;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_DAY          = 12U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MONTH        = 3U;
inline constexpr uint16_t AUTOGEN_MAP_GEN_YEAR         = 2025U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_HOUR         = 20U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MINUTE       = 24U;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_SECOND       = 16U;
inline constexpr uint32_t AUTOGEN_MAP_CHECKSUM         = 1061080793U;

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
