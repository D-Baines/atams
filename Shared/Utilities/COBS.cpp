/**
  ******************************************************************************
  * @file    COBS.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams COBS encoder and decoder.
  *
  * @details Implements COBS encoding and decoding. The encoder scans a source buffer,
  *          replacing each zero byte with a distance-to-next-zero overhead byte and
  *          appending a zero terminator. The decoder reverses this transformation,
  *          restoring the original payload bytes.
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

constexpr uint8_t MAX_CODE_VALUE = 255U;

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

COBS::Result_t encode(const uint8_t * const srcBufferPtr,
                      const uint16_t        srcBufferLength,
                            uint8_t * const destBufferPtr,
                      const uint16_t        destBufferLength)
{
  COBS::Result_t result {COBS::ERROR_NONE, 0U};

  if ((srcBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    result.status = COBS::ERROR_NULL_PTR;

    return (result); /* Early Return */
  }

  if (srcBufferLength == 0U)
  {
    result.status = COBS::ERROR_SOURCE_LENGTH;

    return (result); /* Early Return */
  }

  uint16_t destIndex          {1U};
  uint16_t codeIndex          {0U};
  uint8_t  distanceToNextZero {1U};

  /* Iterate over all bytes in source buffer */
  for (uint16_t srcIndex = 0U; srcIndex < srcBufferLength; srcIndex++)
  {
    /* Check to see if the destination buffer has run out of space */
    if (destIndex >= destBufferLength)
    {
      result.status = COBS::ERROR_DESTINATION_LENGTH;

      return (result); /* Early Return */
    }

    uint8_t sourceByte = srcBufferPtr[srcIndex];

    if (sourceByte == 0U)
    {
      /* Zero byte found - log location at last code index and reset distance to next zero */
      destBufferPtr[codeIndex] = distanceToNextZero;
      codeIndex                = destIndex;
      distanceToNextZero       = 1U;
      destIndex++;
    }
    else
    {
      /* Copy non-zero byte from source to destination buffer */
      destBufferPtr[destIndex++] = sourceByte;
      distanceToNextZero++;

      if (distanceToNextZero == COBS::MAX_CODE_VALUE)
      {
        /* No zero within 255 bytes, set previous code to 255 and reset distance to next zero */
        destBufferPtr[codeIndex] = distanceToNextZero;
        codeIndex                = destIndex;
        distanceToNextZero       = 1U;
        destIndex++;
      }
    }
  }

  /* Write final code byte */
  if (destIndex >= destBufferLength)
  {
    result.status = COBS::ERROR_DESTINATION_LENGTH;

    return (result); /* Early Return */
  }

  /* Set zero locator value for zero end byte */
  destBufferPtr[codeIndex]   = distanceToNextZero;
  destBufferPtr[destIndex++] = 0U;

  /* Set output length and success status */
  result.outputLength = destIndex;
  result.status       = COBS::ERROR_NONE;

  return (result);
}


COBS::Result_t decode(const uint8_t * const srcBufferPtr,
                      const uint16_t        srcBufferLength,
                            uint8_t * const destBufferPtr,
                      const uint16_t        destBufferLength)
{
  COBS::Result_t result {COBS::ERROR_NONE, 0U};

  if ((srcBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    result.status = COBS::ERROR_NULL_PTR;

    return (result); /* Early Return */
  }

  if (srcBufferLength == 0U)
  {
    result.status = COBS::ERROR_SOURCE_LENGTH;

    return (result); /* Early Return */
  }

  if (srcBufferPtr[srcBufferLength - 1U] != 0U)
  {
    result.status = COBS::ERROR_SOURCE_LENGTH;

    return (result); /* Early Return */
  }

  uint16_t srcIndex  {0U};
  uint16_t destIndex {0U};

  while (srcIndex < (srcBufferLength - 1U))
  {
    uint8_t code = srcBufferPtr[srcIndex++];

    if (code == 0U)
    {
      result.status = COBS::ERROR_INVALID_ZERO;

      return (result); /* Early Return */
    }

    uint8_t blockLength = static_cast<uint8_t>(code - 1U);

    if ((srcIndex + blockLength) >= srcBufferLength)
    {
      result.status = COBS::ERROR_SOURCE_LENGTH;

      return (result); /* Early Return */
    }

    for (uint8_t blockIndex = 0; blockIndex < blockLength; blockIndex++)
    {
      if (destIndex >= destBufferLength)
      {
        result.status = COBS::ERROR_DESTINATION_LENGTH;

        return (result); /* Early Return */
      }

      uint8_t sourceByte = srcBufferPtr[srcIndex++];

      if (sourceByte == 0U)
      {
        result.status = COBS::ERROR_INVALID_ZERO;

        return (result); /* Early Return */
      }

      destBufferPtr[destIndex++] = sourceByte;
    }

    if (code < COBS::MAX_CODE_VALUE && (srcIndex < (srcBufferLength - 1U)))
    {
      if (destIndex >= destBufferLength)
      {
        result.status = COBS::ERROR_DESTINATION_LENGTH;

        return (result); /* Early Return */
      }

      destBufferPtr[destIndex++] = 0U;
    }
  }

  result.outputLength = destIndex;
  result.status       = COBS::ERROR_NONE;

  return (result);
}


} } /* End Namespace - Atams::COBS */

/**
  * @}End of File
  */
