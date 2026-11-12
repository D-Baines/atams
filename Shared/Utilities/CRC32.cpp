/**
  ******************************************************************************
  * @file    CRC32.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams CRC32 checksum calculator.
  *
  * @details Implements CRC32 computation using a 256-entry software lookup table,
  *          built by the constexpr constructor in CRC32.hpp. The rolling functions
  *          update a running checksum held by the caller, one byte at a time.
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
/* INCLUDES                                                                         */
/*************************************************************************************/

#include "CRC32.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

uint32_t CRC32::calculateCRC(const uint8_t *byteBuffer, uint32_t length) const
{
  uint32_t crc {CRC32::CRC_RESET_VALUE};

  for (uint32_t byteIndex {0U}; byteIndex < length; byteIndex++)
  {
    crc = (crc >> BITS_IN_A_BYTE) ^ crcTable_[(crc ^ byteBuffer[byteIndex]) & BYTE_MASK];
  }

  return (crc ^ FINAL_XOR_VALUE);
}

void CRC32::beginRollingCRC(uint32_t &rollingCRC) const
{
  rollingCRC = CRC32::CRC_RESET_VALUE;
}

void CRC32::updateRollingCRC(uint32_t &rollingCRC, const uint8_t byte) const
{
  rollingCRC = (rollingCRC >> BITS_IN_A_BYTE) ^ crcTable_[(rollingCRC ^ byte) & BYTE_MASK];
}

uint32_t CRC32::getRollingCRC(const uint32_t rollingCRC) const
{
  return (rollingCRC ^ FINAL_XOR_VALUE);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
