/**
  ******************************************************************************
  * @file    AtamsUtilities.hpp
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "../AtamsTypedefs.hpp"
#include <stdint.h>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint8_t THREE_BYTE_SHIFT  = 24U;
inline constexpr uint8_t TWO_BYTE_SHIFT    = 16U;
inline constexpr uint8_t SINGLE_BYTE_SHIFT = 8U;
inline constexpr uint8_t SINGLE_BYTE_MASK  = 0xFFU;

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

const char* getErrorString(Error_t errorID);

inline bool systemIsBigEndian(void)
{
  uint32_t asUINT32 = 0x0001;
  uint8_t *asPtr    = reinterpret_cast<uint8_t*>(&asUINT32);

  return (asPtr[3U] == 1U);
}

void swapEndiannessRaw(uint8_t* buffer, uint8_t bufferLength);

uint32_t bufferToUint32(const uint8_t* buffer);

void uint32ToBuffer(const uint32_t value, uint8_t* buffer);

Error_t decodeMeshPacket(const uint8_t  *inputBuffer,
                         const uint16_t  inputBufferLength,
                               uint8_t  *decodedBuffer,
                         const uint16_t  decodedBufferMaxLength,
                               uint16_t &decodedLength);

Error_t encodeMeshPacket(      uint8_t  *inputBuffer,
                         const uint16_t  inputLength,
                               uint8_t  *encodedBuffer,
                         const uint16_t  encodedBufferMaxLength,
                               uint16_t &encodedLength);

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t* buffer);

void bufferToDatagramHeader(const uint8_t *buffer, DatagramHeader_t &datagramHeader);


} /* End Namespace - Atams */


/**
  * @}End of File
  */
