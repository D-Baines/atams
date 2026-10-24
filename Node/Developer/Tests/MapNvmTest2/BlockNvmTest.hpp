/**
  ******************************************************************************
  * @file    BlockNvmTest.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: NvmTest.
  *          The Data Block is part of an Atams Memory Map with name: NvmTest2.
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

#include "../../../../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapNvmTest2 { namespace BlockNvmTest {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

constexpr uint16_t NUMBER_OF_VARS {3U};

/*-------------------------- Var ID List ---------------------------*/

constexpr VarID_t VAR_NVM_VAR_CONSTANT      {30U};
constexpr VarID_t VAR_NVM_VAR_NAME_CHANGE_2 {31U};
constexpr VarID_t VAR_NVM_VAR_TYPE_CHANGE   {32U};

/*------------------------- Defaults List --------------------------*/

constexpr uint8_t  DEFAULT_NVM_VAR_CONSTANT      {4U};
constexpr uint16_t DEFAULT_NVM_VAR_NAME_CHANGE_2 {5U};
constexpr float    DEFAULT_NVM_VAR_TYPE_CHANGE   {6.0F};


} } } /* End Namespace - Atams::MapNvmTest2::BlockNvmTest */

/**
  * @}End of File
  */
