/**
  ******************************************************************************
  * @file    NodeUtilities.hpp
  *
  * @author  D. Baines
  *
  * @brief   Utility function declarations for the Atams Node library.
  *
  * @details Declares utility functions shared between the Atams Node Comms Core
  *          and Application Core, including Node Memory Map validation.
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../../Shared/AtamsTypedefs.hpp"
#include "NodeTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NodeUtilities {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Atams::Error_t validateMemoryMap(const MemoryMap_t &memoryMap);


} } /* End Namespace - Atams::NodeUtilities */


/**
  * @}End of File
  */
