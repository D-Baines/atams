/**
  ******************************************************************************
  * @file    CRC32.hpp
  *
  * @author  D. Baines
  *
  * @brief   CRC32 checksum calculator for Atams packet integrity verification.
  *
  * @details Defines the CRC32 class, which computes a 32-bit cyclic redundancy check
  *          using a software lookup table. Provides an incremental interface so that
  *          a checksum can be accumulated across multiple byte sequences before being
  *          finalised and compared against a reference value.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class CRC32
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructors are constexpr so a constexpr instance has its table built at compile time and placed in read-only memory */

  /* Constructor */
  constexpr CRC32(const uint32_t generatorPolynomial) :
  crcTable_{}
  {
    generateLookupTable(generatorPolynomial);
  }

  /* Default Constructor */
  constexpr CRC32(void) :
  CRC32(Atams::CRC32_POLYNOMIAL)
  {
    /* Do Nothing */
  }

  /* Default Destructor */
  ~CRC32(void) = default;

  /* Copy Constructor */
  CRC32(const CRC32 &other) = delete;

  /* Copy Assignment Operator */
  CRC32 & operator=(const CRC32 &other) = delete;

  /* Move Constructor */
  CRC32(CRC32 &&other) = delete;

  /* Move Assignment Operator */
  CRC32 & operator=(CRC32 &&other) = delete;

  uint32_t calculateCRC(const uint8_t *byteBuffer, uint32_t length) const;

  /* The caller holds the rolling value, so one instance can serve any number of rolling checksums at once */
  void beginRollingCRC(uint32_t &rollingCRC) const;

  void updateRollingCRC(uint32_t &rollingCRC, const uint8_t byte) const;

  uint32_t getRollingCRC(const uint32_t rollingCRC) const;

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static constexpr uint32_t CRC_RESET_VALUE     = 0xFFFFFFFFU;
  static constexpr uint16_t DECIMAL_WIDTH_8_BIT = 256U;
  static constexpr uint8_t  BYTE_MASK           = 0xFFU;
  static constexpr uint32_t FINAL_XOR_VALUE     = 0xFFFFFFFFU;
  static constexpr uint8_t  BITS_IN_A_BYTE      = 8U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  uint32_t crcTable_[DECIMAL_WIDTH_8_BIT];

  /*-- Private Function Declarations ------------------------------------------------*/

  constexpr void generateLookupTable(const uint32_t generatorPolynomial)
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

      crcTable_[byteValue] = crc;
    }
  }

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */

