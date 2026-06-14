/**
  ******************************************************************************
  * @file    CRC32.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams CRC32 checksum calculator.
  *
  * @details Implements CRC32 computation using a 256-entry software lookup table
  *          generated at initialisation. The accumulate() method updates a running
  *          checksum one byte at a time; reset() clears the accumulator for reuse.
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

#include "CRC32.hpp"
#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CRC32::CRC32(void)
{
  generateLookupTable(Atams::CRC32_POLYNOMIAL);
}

CRC32::CRC32(uint32_t generatorPolynomial)
{
  generateLookupTable(generatorPolynomial);
}

uint32_t CRC32::calculateCRC(const uint8_t *byteBuffer, uint32_t length)
{
  uint32_t crc = CRC32::CRC_RESET_VALUE;

  for (uint32_t byteIndex = 0U; byteIndex < length; byteIndex++)
  {
    crc = (crc >> BITS_IN_A_BYTE) ^ _crcTable[(crc ^ byteBuffer[byteIndex]) & BYTE_MASK];
  }

  return (crc ^ FINAL_XOR_VALUE);
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
  return (_rollingCRC ^ FINAL_XOR_VALUE);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void CRC32::generateLookupTable(const uint32_t generatorPolynomial)
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


} /* End Namespace - Atams */

/**
  * @}End of File
  */
