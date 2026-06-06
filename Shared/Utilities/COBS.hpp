/**
  ******************************************************************************
  * @file    COBS.hpp
  *
  * @author  D. Baines
  *
  * @brief   COBS encoder and decoder for Atams bus packets.
  *
  * @details Defines the COBS class, which provides static encode and decode methods
  *          implementing Consistent Overhead Byte Stuffing. COBS eliminates zero bytes
  *          from a payload, allowing the zero byte to be used as an unambiguous
  *          end-of-packet delimiter on the serial bus.
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
  ERROR_NONE               = 0U,
  ERROR_NULL_PTR           = 1U,
  ERROR_SOURCE_LENGTH      = 2U,
  ERROR_DESTINATION_LENGTH = 3U,
  ERROR_NO_END_OF_LINE     = 4U,
  ERROR_INVALID_ZERO       = 5U,
} Error_t;

struct Result_t
{
  COBS::Error_t status       {COBS::ERROR_NONE};
  uint16_t      outputLength {0U};
};


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

COBS::Result_t encode(const uint8_t * const srcBufferPtr,
                      const uint16_t        srcBufferLength,
                            uint8_t * const destBufferPtr,
                      const uint16_t        destBufferLength);


COBS::Result_t decode(const uint8_t * const srcBufferPtr,
                      const uint16_t        srcBufferLength,
                            uint8_t * const destBufferPtr,
                      const uint16_t        destBufferLength);

} } /* End Namespace - Atams::COBS */

/**
  * @}End of File
  */


