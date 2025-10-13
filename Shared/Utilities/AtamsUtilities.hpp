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

#include <stdint.h>
#include <type_traits>

#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

const char* getErrorString(const Atams::Error_t errorID);

uint32_t bufferToUint32(const uint8_t * const buffer);

void uint32ToBuffer(const uint32_t value, uint8_t * const buffer);

Atams::Error_t decodeBusPacket(const uint8_t  *inputBuffer,
                               const uint16_t  inputBufferLength,
                                     uint8_t  *decodedBuffer,
                               const uint16_t  decodedBufferMaxLength,
                                     uint16_t &decodedLength);

Atams::Error_t encodeBusPacket(      uint8_t  *inputBuffer,
                               const uint16_t  inputLength,
                                     uint8_t  *encodedBuffer,
                               const uint16_t  encodedBufferMaxLength,
                                     uint16_t &encodedLength);

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t * const buffer);

void bufferToDatagramHeader(const uint8_t * const buffer, DatagramHeader_t &datagramHeader);

Atams::Error_t validateMemoryMap(const SharedMemoryMap_t &memoryMap, const uint32_t varStorageLength);

/*************************************************************************************/
/* PUBLIC TEMPLATE FUNCTION DEFINITIONS                                              */
/*************************************************************************************/

template <typename T>
constexpr Atams::VarType_t getAtamsType(void)
{
    if      constexpr (std::is_same<T, uint8_t>::value)  return (Atams::TYPE_UINT8);
    else if constexpr (std::is_same<T, int8_t>::value)   return (Atams::TYPE_INT8);
    else if constexpr (std::is_same<T, uint16_t>::value) return (Atams::TYPE_UINT16);
    else if constexpr (std::is_same<T, int16_t>::value)  return (Atams::TYPE_INT16);
    else if constexpr (std::is_same<T, uint32_t>::value) return (Atams::TYPE_UINT32);
    else if constexpr (std::is_same<T, int32_t>::value)  return (Atams::TYPE_INT32);
    else if constexpr (std::is_same<T, float>::value)    return (Atams::TYPE_FLOAT);
    return (Atams::TYPE_NULL);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
