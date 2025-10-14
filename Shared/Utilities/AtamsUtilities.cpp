/**
  ******************************************************************************
  * @file    AtamsUtilities.cpp
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <string.h>
#include "AtamsUtilities.hpp"
#include "../AtamsTypedefs.hpp"
#include "../Maps/BlockUniversal.hpp"
#include "COBS.hpp"
#include "CRC32.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static CRC32 atamsCRC_(Atams::CRC32_POLYNOMIAL);

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static inline bool validateAtamsVersion(const SharedMemoryMap_t &memoryMap)
{
  return ((memoryMap.genInfo.atamsVersionMajor == Atams::ATAMS_VERSION_MAJOR) &&
          (memoryMap.genInfo.atamsVersionMinor == Atams::ATAMS_VERSION_MINOR) );
}

static bool validateMapLength(const SharedMemoryMap_t &memoryMap, const uint32_t varStorageLength)
{
  bool     lengthValid = true;
  uint16_t varIndex    = 0U;

  if ((memoryMap.noOfVars > varStorageLength                        ) ||
      (memoryMap.noOfVars > Atams::MAX_NUMBER_OF_VARS               ) ||
      (memoryMap.noOfVars < BlockUniversal::NUMBER_OF_VARS) )
  {
    lengthValid = false;
  }

  for (uint16_t varID = 0U; varID < memoryMap.noOfVars; varID++)
  {
    const VarInfo_t varInfo = memoryMap.varInfoList[varID];

    if ((varInfo.type           > Atams::NUMBER_OF_VAR_TYPES) ||
        (varInfo.externalAccess > Atams::ACCESS_WRITE       ) ||
        (varInfo.NVMStorage     > Atams::ATAMS_TRUE         ) )
    {
      lengthValid = false;
      break;
    }

    if ((varInfo.type           == Atams::TYPE_NULL  ) ||
        (varInfo.externalAccess == Atams::ACCESS_NONE) )
    {
      if (varIndex != memoryMap.noOfVars) lengthValid = false;
      break;
    }

    varIndex++;
  }

  return (lengthValid);
}

static bool validateUniversalBlock(const SharedMemoryMap_t &memoryMap)
{
  bool     universalValid = true;
  uint16_t varIndex       = 0U;

  if (memoryMap.noOfVars < BlockUniversal::NUMBER_OF_VARS)
  {
    return (false); /* Early Return */
  }

  for (VarInfo_t universalVarInfo : BlockUniversal::varInfoList)
  {
    VarInfo_t mapVarInfo = memoryMap.varInfoList[varIndex];

    if (universalVarInfo != mapVarInfo) universalValid = false;

    varIndex++;
  }

  return (universalValid);
}

static bool validateMapChecksum(const SharedMemoryMap_t &memoryMap)
{
  atamsCRC_.beginRollingCRC();

  for (uint16_t varID = BlockUniversal::NUMBER_OF_VARS; varID < memoryMap.noOfVars; varID++)
  {
    const VarInfo_t varInfo = memoryMap.varInfoList[varID];

    atamsCRC_.updateRollingCRC(static_cast<uint8_t>(varInfo.type));
    atamsCRC_.updateRollingCRC(static_cast<uint8_t>(varInfo.externalAccess));
    atamsCRC_.updateRollingCRC(static_cast<uint8_t>(varInfo.NVMStorage));
  }

  uint32_t calculatedChecksum = atamsCRC_.getRollingCRC();

  return (memoryMap.genInfo.genChecksum == calculatedChecksum);
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

uint32_t bufferToUint32(const uint8_t * const buffer)
{
  /* Little endian: LSB first */
  return ((static_cast<uint32_t>(buffer[0U])                     ) |
          (static_cast<uint32_t>(buffer[1U]) << SINGLE_BYTE_SHIFT) |
          (static_cast<uint32_t>(buffer[2U]) << TWO_BYTE_SHIFT   ) |
          (static_cast<uint32_t>(buffer[3U]) << THREE_BYTE_SHIFT ) );
}

void uint32ToBuffer(const uint32_t value, uint8_t * const buffer)
{
  /* Little endian: LSB first */
  buffer[0U] = static_cast<uint8_t>((value                     ) & SINGLE_BYTE_MASK);
  buffer[1U] = static_cast<uint8_t>((value >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  buffer[2U] = static_cast<uint8_t>((value >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  buffer[3U] = static_cast<uint8_t>((value >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
}

Atams::Error_t decodeBusPacket(const uint8_t  * const inputBuffer,
                               const uint16_t         inputBufferLength,
                                     uint8_t  * const decodedBuffer,
                               const uint16_t         decodedBufferMaxLength,
                                     uint16_t        &decodedLength)
{
  COBS::Result_t COBSDecodeResult = COBS::decode(inputBuffer, inputBufferLength, decodedBuffer, decodedBufferMaxLength);

  if (COBSDecodeResult.status != COBS::ERROR_NONE)
  {
    return (Atams::ERROR_DECODE_FRAMING);
  }

  if (COBSDecodeResult.outputLength < HEADER_SIZE_HEADER)
  {
    return (Atams::ERROR_DECODE_FRAMING);
  }

  uint32_t packetCRC = bufferToUint32(&decodedBuffer[HEADER_INDEX_CRC]);

  memset(&decodedBuffer[HEADER_INDEX_CRC], 0U, HEADER_SIZE_CRC);

  if (packetCRC != atamsCRC_.calculateCRC(decodedBuffer, COBSDecodeResult.outputLength))
  {
    return (Atams::ERROR_DECODE_CHECKSUM);
  }

  decodedLength = COBSDecodeResult.outputLength;

  return (Atams::ERROR_NONE);
}

Atams::Error_t encodeBusPacket(      uint8_t  * const inputBuffer,
                               const uint16_t         inputLength,
                                     uint8_t  * const encodedBuffer,
                               const uint16_t         encodedBufferMaxLength,
                                     uint16_t        &encodedLength)
{
  if (inputLength < HEADER_SIZE_HEADER)
  {
    return (Atams::ERROR_ENCODE);
  }

  memset(&inputBuffer[HEADER_INDEX_CRC], 0U, HEADER_SIZE_CRC);

  uint32_t CRCResult = atamsCRC_.calculateCRC(inputBuffer, inputLength);

  uint32ToBuffer(CRCResult, &inputBuffer[HEADER_INDEX_CRC]);

  COBS::Result_t COBSEncodeResult = COBS::encode(inputBuffer, inputLength, encodedBuffer, encodedBufferMaxLength);

  if (COBSEncodeResult.status != COBS::ERROR_NONE)
  {
    return (Atams::ERROR_ENCODE);
  }

  encodedLength = COBSEncodeResult.outputLength;

  return (Atams::ERROR_NONE);
}

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t * const buffer)
{
  buffer[0U] = static_cast<uint8_t>(((datagramHeader.command << DATAGRAM_HEADER_SHIFT_COMMAND  ) & DATAGRAM_HEADER_MASK_COMMAND  ) |
                                    ((datagramHeader.varID   >> DATAGRAM_HEADER_SHIFT_VAR_ID_HI) & DATAGRAM_HEADER_MASK_VAR_ID_HI) );

  buffer[1U] = static_cast<uint8_t>((datagramHeader.varID    << DATAGRAM_HEADER_SHIFT_VAR_ID_LO) & DATAGRAM_HEADER_MASK_VAR_ID_LO);
}

void bufferToDatagramHeader(const uint8_t * const buffer, DatagramHeader_t &datagramHeader)
{
  datagramHeader.command = static_cast<uint8_t>(buffer[0U] & DATAGRAM_HEADER_MASK_COMMAND) >> DATAGRAM_HEADER_SHIFT_COMMAND;

  datagramHeader.varID   = static_cast<uint16_t>((static_cast<uint16_t>(buffer[0U] & DATAGRAM_HEADER_MASK_VAR_ID_HI) << DATAGRAM_HEADER_SHIFT_VAR_ID_HI) |
                                                 (static_cast<uint16_t>(buffer[1U] & DATAGRAM_HEADER_MASK_VAR_ID_LO) >> DATAGRAM_HEADER_SHIFT_VAR_ID_LO) );
}

Atams::Error_t validateMemoryMap(const SharedMemoryMap_t &memoryMap, const uint32_t varStorageLength)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  if (memoryMap.varInfoList == nullptr)
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }
  else if (validateAtamsVersion(memoryMap) == false)
  {
    statusReturn = Atams::ERROR_ATAMS_VERSION_MISMATCH;
  }
  else if ((validateMapLength(memoryMap, varStorageLength) == false) ||
           (validateUniversalBlock(memoryMap)              == false) ||
           (validateMapChecksum(memoryMap)                 == false) )
  {
    statusReturn = Atams::ERROR_MEMORY_MAP;
  }

  return (statusReturn);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
