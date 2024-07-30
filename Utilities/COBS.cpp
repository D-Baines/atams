/**
  ******************************************************************************
  * @file    COBS.cpp
  *
  * @author  D. Baines
  *
  * @brief   File contains constants, variables, and function definitions
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "../../Atams/Utilities/COBS.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace COBS
{


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

constexpr uint8_t MAX_BLOCK_ELEMENTS = 254U;
constexpr uint8_t MAX_UINT8_DECIMAL  = 255U;


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Result_t encode(const uint8_t *sourceBufferPtr,
                const uint16_t sourceBufferLength,
                      uint8_t *destBufferPtr,
                const uint16_t destBufferLength)
{

  Result_t encodeResult;

  uint8_t *destWritePtr     = destBufferPtr + 1U;
  uint8_t *destEndPtr       = ((destBufferPtr + destBufferLength) - 1U);
  uint8_t *zeroLocator      = destBufferPtr;
  uint8_t  zeroSearchLength = 1U;

  /* Buffer pointer NULL checks */
  if ((sourceBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    encodeResult.status = ERROR_NULL_PTR;
    return (encodeResult);
  }

  /* Iterate over all bytes in source buffer */
  for (uint16_t index = 0U; index < sourceBufferLength; index++)
  {
    /* Check to see if the destination buffer has run out of space */
    if (destWritePtr >= destEndPtr)
    {
      encodeResult.status = ERROR_BUFFER_OVERFLOW;
      return (encodeResult);
    }

    uint8_t sourceByte = sourceBufferPtr[index];

    if (sourceByte == 0U)
    {
      /* Zero byte found - log location in last zero locator and update new zero locator position */
      *zeroLocator     = zeroSearchLength;
      zeroLocator      = destWritePtr++;
      zeroSearchLength = 1U;
    }

    else
    {
      /* Copy non-zero byte from source to destination buffer */
      *destWritePtr = sourceByte;
      destWritePtr++;
      zeroSearchLength++;

      if (zeroSearchLength == MAX_UINT8_DECIMAL)
      {
        /* No zero within 255 bytes, set previous zero locator to 255 and update new zero locator position */
        *zeroLocator     = zeroSearchLength;
        zeroLocator      = destWritePtr++;
        zeroSearchLength = 1U;
      }
    }
  }

  /* Set zero locator value for zero end byte */
  *zeroLocator  = zeroSearchLength;
  *destWritePtr = 0U;
  destWritePtr++;

  /* Set output length and success status */
  encodeResult.outputLength = (destWritePtr - destBufferPtr);
  encodeResult.status       = ERROR_NONE;

  return (encodeResult);

}


Result_t decode(const uint8_t *sourceBufferPtr,
                const uint16_t sourceBufferLength,
                      uint8_t *destBufferPtr,
                const uint16_t destBufferLength)
{
  Result_t decodeResult;

  uint16_t sourceIndex        = 0U;
  uint16_t destIndex          = 0U;
  uint16_t noOfBlockElements  = 0U;

  /* -1U implemented as destIndex can be incremented twice after check */
  uint16_t destEndCheckLength = destBufferLength - 1U;

  /* Buffer pointer NULL checks */
  if ((sourceBufferPtr == nullptr) || (destBufferPtr == nullptr))
  {
    decodeResult.status = ERROR_NULL_PTR;
    return (decodeResult);
  }

  while (sourceIndex < sourceBufferLength)
  {
    /* Get number of block elements from first byte after block */
    noOfBlockElements = (sourceBufferPtr[sourceIndex] - 1U);
    sourceIndex++;

    /* Iterate over block elements */
    for (uint16_t index = 0U; index < noOfBlockElements; index++)
    {
      uint8_t sourceByte = sourceBufferPtr[sourceIndex];

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
      sourceIndex++;
    }

    if (sourceBufferPtr[sourceIndex] == 0U)
    {
      if (sourceIndex < (sourceBufferLength - 1U))
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
    if (noOfBlockElements < MAX_BLOCK_ELEMENTS)
    {
      destBufferPtr[destIndex] = 0U;
      destIndex++;
    }
  }

  /* Set output length and success status */
  decodeResult.outputLength = &destBufferPtr[destIndex] - destBufferPtr;
  decodeResult.status       = ERROR_NONE;

  return (decodeResult);
}


} /* End Namespace - COBS */

/**
  * @}End of File
  */
