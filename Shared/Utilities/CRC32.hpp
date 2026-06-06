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
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class CRC32
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- PUBLIC FUNCTION DECLARATIONS ---*/

  CRC32(void);

  CRC32(uint32_t generatorPolynomial);

  uint32_t calculateCRC(const uint8_t *byteBuffer, uint32_t length);

  void beginRollingCRC(void);

  void updateRollingCRC(const uint8_t byte);

  uint32_t getRollingCRC(void);

  /*-- Private -----------------------------------------------------------------------*/

  private:

  /*-- PRIVATE CONSTANTS --------------*/

  static constexpr uint32_t CRC_RESET_VALUE     = 0xFFFFFFFFU;
  static constexpr uint16_t DECIMAL_WIDTH_8_BIT = 256U;
  static constexpr uint8_t  BYTE_MASK           = 0xFFU;
  static constexpr uint32_t FINAL_XOR_VALUE     = 0xFFFFFFFFU;
  static constexpr uint8_t  BITS_IN_A_BYTE      = 8U;

  /*-- PRIVATE VARIABLES --------------*/

  uint32_t _crcTable[DECIMAL_WIDTH_8_BIT];
  uint32_t _rollingCRC = CRC_RESET_VALUE;

  /*-- PRIVATE FUNCTION DECLARATIONS --*/

  void generateLookupTable(const uint32_t generatorPolynomial);
};

} /* End Namespace - Atams */

/**
  * @}End of File
  */


