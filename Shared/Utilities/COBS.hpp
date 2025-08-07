/**
  ******************************************************************************
  * @file    COBS.hpp
  *
  * @author  D. Baines
  *
  * @brief
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace COBS {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef enum
{
  ERROR_NONE                 = 0U,
  ERROR_NULL_PTR             = 1U,
  ERROR_BUFFER_OVERFLOW      = 2U,
  ERROR_NO_END_OF_LINE       = 3U,
  ERROR_BUFFER_CONTAINS_ZERO = 4U,
} Error_t;

struct Result_t
{
  COBS::Error_t status       = COBS::ERROR_NONE;
  uint16_t      outputLength = 0U;
};


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

COBS::Result_t encode(const uint8_t *sourceBufferPtr,
                      const uint16_t sourceBufferLength,
                            uint8_t *destBufferPtr,
                      const uint16_t destBufferLength);


COBS::Result_t decode(const uint8_t *sourceBufferPtr,
                      const uint16_t sourceBufferLength,
                            uint8_t *destBufferPtr,
                      const uint16_t destBufferLength);

} } /* End Namespace - Atams::COBS */

/**
  * @}End of File
  */


