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
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CRC32::CRC32(uint32_t generatorPolynomial)
{
  for (uint32_t byteValue = 0U; byteValue < DECIMAL_WIDTH_8_BIT; byteValue++) 
  {
    uint32_t crc = byteValue;

    for (uint8_t bitIndex = 0U; bitIndex < BITS_IN_A_BYTE; ++bitIndex) 
    {
        if (crc & 1U) 
        {
          crc = (crc >> 1U) ^ generatorPolynomial;
        } 
        else 
        {
          crc >>= 1U;
        }
    }

    _crcTable[byteValue] = crc;
  }
}

uint32_t CRC32::calculateCRC(volatile const uint8_t *byteBuffer, uint16_t length)
{
  uint32_t crc = CRC32::CRC_RESET_VALUE;

  for (uint16_t byteIndex = 0U; byteIndex < length; byteIndex++) 
  {
    crc = (crc >> BITS_IN_A_BYTE) ^ _crcTable[(crc ^ byteBuffer[byteIndex]) & BYTE_MASK];
  }

  return (reflect(crc ^ FINAL_XOR_VALUE, NUMBER_OF_CRC_BITS));
}

void CRC32::beginRollingCRC(void)
{
  _rollingCRC = CRC32::CRC_RESET_VALUE;
}

void CRC32::updateRollingCRC(const uint8_t byte)
{
  _rollingCRC = (_rollingCRC >> BITS_IN_A_BYTE) ^ _crcTable[(_rollingCRC ^ byte) & BYTE_MASK];
}

uint32_t CRC32::getRollingCRC(void)
{
  return (reflect(_rollingCRC ^ FINAL_XOR_VALUE, NUMBER_OF_CRC_BITS));
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

uint32_t CRC32::reflect(const uint32_t data, const uint8_t bitCount)
{
  uint32_t reflection = 0U;

  for (uint8_t bitIndex = 0U; bitIndex < bitCount; bitIndex++) 
  {
    if (data & (1U << bitIndex)) 
    {
      reflection |= (1U << ((bitCount - 1U) - bitIndex));
    }
  }

  return (reflection);
}

/**
  * @}End of File
  */
