/**
  ******************************************************************************
  * @file    CRC32.cpp
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

#include "../../Atams/Utilities/CRC32.hpp"


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/



/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CRC32::CRC32(uint32_t generatorPolynomial)
{
  /* iterate over all byte values 0 - 255 */
  for (uint16_t divident = 0U; divident < DECIMAL_WIDTH_8_BIT; divident++)
  {
    uint32_t currentByte = (divident << CRC32_BITSHIFT);

    /* calculate the CRC-32 value for current byte */
    for (uint8_t bit = 0U; bit < BITS_IN_A_BYTE; bit++)
    {
      if ((currentByte & WORD_MSB_HIGH) != 0U)
      {
        currentByte <<= 1U;
        currentByte ^= generatorPolynomial;
      }
      else
      {
        currentByte <<= 1U;
      }
    }

    /* store CRC value in lookup table */
    _CRCTable[divident] = currentByte;
  }
}


uint32_t CRC32::calculateCRC32(volatile const uint8_t *byteBuffer, uint16_t length)
{
  uint32_t crc = 0U;

  for (uint32_t index = 0U; index < length; index++)
  {
    uint8_t pos = ((crc ^ (byteBuffer[index] << CRC32_BITSHIFT)) >> CRC32_BITSHIFT);

    crc = ((crc << BITS_IN_A_BYTE) ^ _CRCTable[pos]);
  }

  return (crc);
}


/**
  * @}End of File
  */
