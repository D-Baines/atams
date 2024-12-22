/**
  ******************************************************************************
  * @file    CRC32.hpp
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class CRC32
{

  public:

  /*-- Public Prototypes ------------------------------------------------------------*/

  CRC32(uint32_t generatorPolynomial);

  uint32_t calculateCRC32(volatile const uint8_t *byteBuffer, uint16_t length);


  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static const uint16_t DECIMAL_WIDTH_8_BIT = 256U;

  static const uint32_t CRC32_BITSHIFT = 24U;

  static const uint32_t WORD_MSB_HIGH = 0x80000000;

  static const uint8_t  BITS_IN_A_BYTE = 8U;

  /*-- Private Variables ------------------------------------------------------------*/

  uint32_t _CRCTable[DECIMAL_WIDTH_8_BIT];

};

/**
  * @}End of File
  */


