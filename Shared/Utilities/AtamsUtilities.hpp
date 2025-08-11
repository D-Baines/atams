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

const char* getErrorString(Atams::Error_t errorID);

uint32_t bufferToUint32(const uint8_t * const buffer);

void uint32ToBuffer(const uint32_t value, uint8_t * const buffer);

Atams::Error_t decodeMeshPacket(const uint8_t  *inputBuffer,
                                const uint16_t  inputBufferLength,
                                      uint8_t  *decodedBuffer,
                                const uint16_t  decodedBufferMaxLength,
                                      uint16_t &decodedLength);

Atams::Error_t encodeMeshPacket(      uint8_t  *inputBuffer,
                                const uint16_t  inputLength,
                                      uint8_t  *encodedBuffer,
                                const uint16_t  encodedBufferMaxLength,
                                      uint16_t &encodedLength);

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t * const buffer);

void bufferToDatagramHeader(const uint8_t * const buffer, DatagramHeader_t &datagramHeader);

Atams::Error_t validateMemoryMap(const SharedMemoryMap_t &memoryMap, const uint32_t varStorageLength);


} /* End Namespace - Atams */


/**
  * @}End of File
  */
