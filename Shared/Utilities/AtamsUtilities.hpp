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
#include "../Maps/BlockUniversal.hpp"
#include "CRC32.hpp"

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

Atams::CRC32 &getAtamsCRC32(void);

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

/* Internal helpers for validateMemoryMap() below - parameterised on VarInfoT so they work for
 * both Node's full VarInfo_t and Hub's slimmer HubVarInfo_t (see Hub/Node.hpp). Not intended to
 * be called directly - not marked static so identical instantiations can be merged by the
 * linker across translation units, same as any other function template. */
template<typename VarInfoT>
inline bool validateAtamsVersion(const SharedMemoryMap_t<VarInfoT> &memoryMap)
{
  return ((memoryMap.genInfo.atamsVersionMajor == Atams::ATAMS_VERSION_MAJOR) &&
          (memoryMap.genInfo.atamsVersionMinor == Atams::ATAMS_VERSION_MINOR) );
}

template<typename VarInfoT>
inline bool validateMapLength(const SharedMemoryMap_t<VarInfoT> &memoryMap, const uint32_t varStorageLength)
{
  const uint16_t numberOfVars {memoryMap.genInfo.noOfVars};
  bool           lengthValid  {true};

  if ((numberOfVars > varStorageLength              ) ||
      (numberOfVars > Atams::MAX_NUMBER_OF_VARS     ) ||
      (numberOfVars < BlockUniversal::NUMBER_OF_VARS) )
  {
    lengthValid = false;
  }

  for (uint16_t varID {0U}; varID < numberOfVars; varID++)
  {
    const VarInfoT varInfo {memoryMap.varInfoList[varID]};

    if ((varInfo.type           > Atams::NUMBER_OF_VAR_TYPES) ||
        (varInfo.externalAccess > Atams::ACCESS_WRITE       ) ||
        (varInfo.type           == Atams::TYPE_NULL         ) ||
        (varInfo.externalAccess == Atams::ACCESS_NONE       ) )
    {
      lengthValid = false;
      break;
    }
  }

  return (lengthValid);
}

template<typename VarInfoT>
inline bool validateUniversalBlock(const SharedMemoryMap_t<VarInfoT> &memoryMap)
{
  bool universalBlockValid {true};

  if (memoryMap.genInfo.noOfVars < BlockUniversal::NUMBER_OF_VARS)
  {
    return (false); /* Early Return */
  }

  for (uint16_t varIndex {0U}; varIndex < BlockUniversal::NUMBER_OF_VARS; varIndex++)
  {
    const VarInfoT  &mapVarInfo       {memoryMap.varInfoList[varIndex]};
    const VarInfo_t &universalVarInfo {BlockUniversal::varInfoList[varIndex]};

    if ((mapVarInfo.type           != universalVarInfo.type          ) ||
        (mapVarInfo.externalAccess != universalVarInfo.externalAccess) )
    {
      universalBlockValid = false;
    }
  }

  return (universalBlockValid);
}

template<typename VarInfoT>
inline bool validateMapChecksum(const SharedMemoryMap_t<VarInfoT> &memoryMap)
{
  Atams::CRC32 &crc {getAtamsCRC32()};

  crc.beginRollingCRC();

  for (uint16_t varID {BlockUniversal::NUMBER_OF_VARS}; varID < memoryMap.genInfo.noOfVars; varID++)
  {
    const VarInfoT varInfo {memoryMap.varInfoList[varID]};

    crc.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
    crc.updateRollingCRC(static_cast<uint8_t>(varInfo.externalAccess));
  }

  uint32_t calculatedChecksum {crc.getRollingCRC()};

  return (memoryMap.genInfo.genChecksum == calculatedChecksum);
}

template<typename VarInfoT>
inline Atams::Error_t validateMemoryMap(const SharedMemoryMap_t<VarInfoT> &memoryMap, const uint32_t varStorageLength)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (memoryMap.varInfoList == nullptr)
  {
    error = Atams::ERROR_MEMORY_MAP;
  }
  else if (validateAtamsVersion(memoryMap) == false)
  {
    error = Atams::ERROR_ATAMS_VERSION_MISMATCH;
  }
  else if ((validateMapLength(memoryMap, varStorageLength) == false) ||
           (validateUniversalBlock(memoryMap)              == false) ||
           (validateMapChecksum(memoryMap)                 == false) )
  {
    error = Atams::ERROR_MEMORY_MAP;
  }

  return (error);
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */
