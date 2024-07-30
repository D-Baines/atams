/**
  ******************************************************************************
  * @file    COBS.hpp
  *
  * @author  D. Baines
  *
  * @brief   File contains defines, type declarations, and function prototypes
  *          for interfacing with an RLS Orbis SPI encoder.
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


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace COBS
{


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef enum
{
  ERROR_NONE                 = 0U,
  ERROR_NULL_PTR             = 1U,
  ERROR_BUFFER_OVERFLOW      = 2U,
  ERROR_NO_END_CHAR          = 3U,
  ERROR_BUFFER_CONTAINS_ZERO = 4U,
} Error_t;

typedef struct
{
  Error_t  status       = ERROR_NONE;
  uint16_t outputLength = 0U;
} Result_t;


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Result_t encode(const uint8_t *sourceBufferPtr,
                const uint16_t sourceBufferLength,
                      uint8_t *destBufferPtr,
                const uint16_t destBufferLength);


Result_t decode(const uint8_t *sourceBufferPtr,
                const uint16_t sourceBufferLength,
                      uint8_t *destBufferPtr,
                const uint16_t destBufferLength);

} /* End Namespace - COBS */

/**
  * @}End of File
  */


