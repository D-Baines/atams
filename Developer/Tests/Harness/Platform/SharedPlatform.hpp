/**
  ******************************************************************************
  * @file    SharedPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Harness Platform constants and macros shared by the Atams Node cores.
  *
  * @details The harness builds the Comms Core only, so the dual-core attribute is empty.
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

/** @brief Empty: the harness Node is single-core */
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** @brief The maximum, so any test Memory Map fits */
constexpr uint16_t NODE_NUMBER_OF_VARS {Atams::MAX_NUMBER_OF_VARS};


} } /* End Namespace - Atams::NodePlatform */


/**
  * @}End of File
  */
