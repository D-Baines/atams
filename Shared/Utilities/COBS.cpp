/**
  ******************************************************************************
  * @file    COBS.cpp
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "COBS.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace COBS {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

constexpr uint8_t MAX_BLOCK_ELEMENTS = 254U;
constexpr uint8_t MAX_UINT8_DECIMAL  = 255U;

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

COBS::Result_t encode(const uint8_t *sourceBufferPtr,
                      const uint16_t sourceBufferLength,
                            uint8_t *destBufferPtr,
                      const uint16_t destBufferLength)
{

  COBS::Result_t encodeResult;
  uint16_t       destIndex        = 1U;
  uint16_t       blockStartIndex  = 0U;
  uint8_t        zeroSearchLength = 1U;

  /* Buffer pointer NULL checks */
  if ((sourceBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    encodeResult.status = ERROR_NULL_PTR;
    return (encodeResult);
  }

  /* Iterate over all bytes in source buffer */
  for (uint16_t srcIndex = 0U; srcIndex < sourceBufferLength; srcIndex++)
  {
    /* Check to see if the destination buffer has run out of space */
    if (destIndex >= destBufferLength)
    {
      encodeResult.status = ERROR_BUFFER_OVERFLOW;
      return (encodeResult);
    }

    uint8_t sourceByte = sourceBufferPtr[srcIndex];

    if (sourceByte == 0U)
    {
      /* Zero byte found - log location in last zero locator and update new zero locator position */
      destBufferPtr[blockStartIndex] = zeroSearchLength;
      blockStartIndex                = destIndex;
      zeroSearchLength               = 1U;
      destIndex++;
    }
    else
    {
      /* Copy non-zero byte from source to destination buffer */
      destBufferPtr[destIndex] = sourceByte;
      destIndex++;
      zeroSearchLength++;

      if (zeroSearchLength == MAX_UINT8_DECIMAL)
      {
        /* No zero within 255 bytes, set previous zero locator to 255 and update new zero locator position */
        destBufferPtr[blockStartIndex] = zeroSearchLength;
        blockStartIndex                = destIndex;
        zeroSearchLength               = 1U;
        destIndex++;
      }
    }
  }

  /* Set zero locator value for zero end byte */
  destBufferPtr[blockStartIndex] = zeroSearchLength;
  destBufferPtr[destIndex]      = 0U;
  destIndex++;

  /* Set output length and success status */
  encodeResult.outputLength = destIndex;
  encodeResult.status       = ERROR_NONE;

  return (encodeResult);

}


COBS::Result_t decode(const uint8_t *sourceBufferPtr,
                      const uint16_t sourceBufferLength,
                            uint8_t *destBufferPtr,
                      const uint16_t destBufferLength)
{
  Result_t decodeResult;

  uint16_t srcIndex    = 0U;
  uint16_t destIndex   = 0U;
  uint16_t blockLength = 0U;

  /* -1U implemented as destIndex can be incremented twice after check */
  uint16_t destEndCheckLength = static_cast<uint16_t>(destBufferLength - 1U);

  /* Buffer pointer NULL checks */
  if ((sourceBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    decodeResult.status = ERROR_NULL_PTR;
    return (decodeResult);
  }

  while (srcIndex < sourceBufferLength)
  {
    blockLength = static_cast<uint16_t>(sourceBufferPtr[srcIndex] - 1U);
    srcIndex++;

    /* Iterate over block elements */
    for (uint16_t index = 0U; index < blockLength; index++)
    {
      uint8_t sourceByte = sourceBufferPtr[srcIndex];

      if (sourceByte == 0U)
      {
        /* Zero found within block */
        decodeResult.status = ERROR_BUFFER_CONTAINS_ZERO;
        return (decodeResult);
      }

      /* Check to see if the destination buffer has run out of space before copying */
      if (destIndex >= destEndCheckLength)
      {
        decodeResult.status = ERROR_BUFFER_OVERFLOW;
        return (decodeResult);
      }

      /* Copy non-zero locator byte into destination buffer */
      destBufferPtr[destIndex] = sourceByte;
      destIndex++;
      srcIndex++;
    }

    if (sourceBufferPtr[srcIndex] == 0U)
    {
      if (srcIndex < (sourceBufferLength - 1U))
      {
        /* Termination character found before end of packet */
        decodeResult.status = ERROR_BUFFER_CONTAINS_ZERO;
        return (decodeResult);
      }

      /* Termination character found at end of packet - decode successful */
      break;
    }

    /* If the block length is maximum, it was created due to it reaching maximum size.
     * Otherwise, the end of the block length indicates a 0 byte needs to be placed
     * in the decoded buffer.
     */
    if (blockLength < MAX_BLOCK_ELEMENTS)
    {
      destBufferPtr[destIndex] = 0U;
      destIndex++;
    }
  }

  /* Set output length and success status */
  decodeResult.outputLength = destIndex;
  decodeResult.status       = ERROR_NONE;

  return (decodeResult);
}


} } /* End Namespace - Atams::COBS */

/**
  * @}End of File
  */
