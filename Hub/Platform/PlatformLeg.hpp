/**
  ******************************************************************************
  * @file    PlatformOptions.hpp
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

#include "../../AtamsTypedefs.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef enum: uint8_t
{
  BUS_ID_TEST = 0U,
} BusID_t;

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

void    setReceiveCallback(CommsReceiveCallback_t receiveCallback);

Error_t transmitBuffer(uint8_t *buffer, uint16_t length);



} } /* End Atams Namespace */


/**
  * @}End of File
  */
