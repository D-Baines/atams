/**
  ******************************************************************************
  * @file    CRC16.hpp
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
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "CRC16.hpp"


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/



/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CRC16::CRC16(uint16_t generatorPolynomial)
{
  /* iterate over all byte values 0 - 255 */
  for (uint16_t divident = 0U; divident < DECIMAL_WIDTH_8_BIT; divident++)
  {
    uint16_t currentByte = (divident << CRC16_BITSHIFT);

    /* calculate the CRC-16 value for current byte */
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


uint16_t CRC16::calculateCRC16(volatile const uint8_t *byteBuffer, uint16_t length)
{
  uint16_t crc = 0U;

  for (uint8_t index = 0U; index < length; index++)
  {
    uint8_t pos = static_cast<uint8_t>((crc >> CRC16_BITSHIFT) ^ byteBuffer[index]);

    crc = (crc << BITS_IN_A_BYTE) ^ _CRCTable[pos];
  }

  return (crc);
}


/**
  * @}End of File
  */
