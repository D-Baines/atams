/**
  ******************************************************************************
  * @file    AtamsUtilities.hpp
  *
  * @author  D. Baines
  *
  * @brief   Shared utility function declarations for Atams packet encoding, decoding, and validation.
  *
  * @details Declares utility functions used by both Hub and Node libraries for COBS
  *          encoding and decoding of bus packets, CRC32 computation and verification,
  *          datagram header serialisation, and Memory Map validation. Also provides
  *          the getAtamsType<T>() template for mapping C++ primitive types to
  *          Atams VarType_t values.
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
#include <string.h>
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

template<typename T>
inline void writeToVarStorage(const T inputVar, Atams::VarStorage_t &varStorage)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in writeToVarStorage");

  uint32_t tempVar;

  if constexpr (std::is_same<T, float>::value) memcpy(&tempVar, &inputVar, sizeof(tempVar));
  else                                         tempVar = static_cast<uint32_t>(inputVar);

  /* Little endian: LSB first */
  varStorage[ZERO_BYTE_INDEX ] = static_cast<uint8_t>((tempVar                     ) & SINGLE_BYTE_MASK);
  varStorage[ONE_BYTE_INDEX  ] = static_cast<uint8_t>((tempVar >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  varStorage[TWO_BYTE_INDEX  ] = static_cast<uint8_t>((tempVar >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  varStorage[THREE_BYTE_INDEX] = static_cast<uint8_t>((tempVar >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
}

template<typename T>
inline void readFromVarStorage(T &outputVar, const Atams::VarStorage_t &varStorage)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in readFromVarStorage");

  /* Little endian: LSB first */
  uint32_t tempVar {((static_cast<uint32_t>(varStorage[ZERO_BYTE_INDEX ])                     ) |
                     (static_cast<uint32_t>(varStorage[ONE_BYTE_INDEX  ]) << SINGLE_BYTE_SHIFT) |
                     (static_cast<uint32_t>(varStorage[TWO_BYTE_INDEX  ]) << TWO_BYTE_SHIFT   ) |
                     (static_cast<uint32_t>(varStorage[THREE_BYTE_INDEX]) << THREE_BYTE_SHIFT ) )};

  if constexpr (std::is_same<T, float>::value) memcpy(&outputVar, &tempVar, sizeof(outputVar));
  else                                         outputVar = static_cast<T>(tempVar);
}

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
    else
    {
      static_assert(!std::is_same<T, T>::value, "Variable type used is incompatible with Atams");
      return (Atams::TYPE_NULL);
    }
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
