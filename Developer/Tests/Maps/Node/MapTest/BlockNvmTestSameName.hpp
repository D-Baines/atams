/**
  ******************************************************************************
  * @file    BlockNvmTestSameName.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: NvmTestSameName.
  *          The Data Block is part of an Atams Memory Map with name: Test.
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

namespace Atams { namespace MapTest { namespace BlockNvmTestSameName {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {1U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_NVM_VAR_CONSTANT {98U};

/*------------------------- Defaults List --------------------------*/

inline constexpr uint8_t DEFAULT_NVM_VAR_CONSTANT {50U};


} } } /* End Namespace - Atams::MapTest::BlockNvmTestSameName */

/**
  * @}End of File
  */
