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

static CRC32 _commsCRC(Atams::CRC32_POLYNOMIAL);

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

const char* getErrorString(Error_t errorID)
{
  static_cast<void>(errorID);
  //return (AtamsErrorStrings[errorID]);

  return (nullptr);
}

void swapEndiannessRaw(uint8_t* buffer, uint8_t bufferLength)
{ 
  for (uint8_t i = 0U; i < bufferLength / 2U; ++i)
  {
    uint8_t temp                  = buffer[i];
    buffer[i]                     = buffer[bufferLength - i - 1U];
    buffer[bufferLength - i - 1U] = temp;
  }
}

uint32_t bufferToUint32(const uint8_t* buffer)
{
  return ((static_cast<uint32_t>(buffer[0U]) << THREE_BYTE_SHIFT ) |
          (static_cast<uint32_t>(buffer[1U]) << TWO_BYTE_SHIFT   ) |
          (static_cast<uint32_t>(buffer[2U]) << SINGLE_BYTE_SHIFT) |
          (static_cast<uint32_t>(buffer[3U])                     ) );
}

void uint32ToBuffer(const uint32_t value, uint8_t* buffer)
{
  buffer[0U] = static_cast<uint8_t>((value >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
  buffer[1U] = static_cast<uint8_t>((value >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  buffer[2U] = static_cast<uint8_t>((value >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  buffer[3U] = static_cast<uint8_t>((value                     ) & SINGLE_BYTE_MASK);
}

Atams::Error_t decodeMeshPacket(const uint8_t  * const inputBuffer,
                                const uint16_t         inputBufferLength,
                                      uint8_t  * const decodedBuffer,
                                const uint16_t         decodedBufferMaxLength,
                                      uint16_t        &decodedLength)
{
  COBS::Result_t COBSDecodeResult = COBS::decode(inputBuffer, inputBufferLength, decodedBuffer, decodedBufferMaxLength);

  if (COBSDecodeResult.status != COBS::ERROR_NONE)
  {
    return (Atams::ERROR_DECODE);
  }

  if (COBSDecodeResult.outputLength < MESH_SIZE_HEADER)
  {
    return (Atams::ERROR_DECODE);
  }

  uint32_t packetCRC = bufferToUint32(&decodedBuffer[MESH_INDEX_CRC]);

  memset(&decodedBuffer[MESH_INDEX_CRC], 0U, MESH_SIZE_CRC);

  if (packetCRC != _commsCRC.calculateCRC(decodedBuffer, COBSDecodeResult.outputLength))
  {
    return (Atams::ERROR_DECODE);
  }

  decodedLength = COBSDecodeResult.outputLength;

  return (Atams::ERROR_NONE);
}

Error_t encodeMeshPacket(      uint8_t  * const inputBuffer,
                         const uint16_t         inputLength,
                               uint8_t  * const encodedBuffer,
                         const uint16_t         encodedBufferMaxLength,
                               uint16_t        &encodedLength)
{
  if (inputLength < MESH_SIZE_HEADER)
  {
    return (Atams::ERROR_ENCODE);
  }

  memset(&inputBuffer[MESH_INDEX_CRC], 0U, MESH_SIZE_CRC);

  uint32_t CRCResult = _commsCRC.calculateCRC(inputBuffer, inputLength);

  uint32ToBuffer(CRCResult, &inputBuffer[MESH_INDEX_CRC]);

  COBS::Result_t COBSEncodeResult = COBS::encode(inputBuffer, inputLength, encodedBuffer, encodedBufferMaxLength);

  if (COBSEncodeResult.status != COBS::ERROR_NONE)
  {
    return (Atams::ERROR_ENCODE);
  }

  encodedLength = COBSEncodeResult.outputLength;

  return (Atams::ERROR_NONE);
}

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t* buffer)
{
  buffer[0U] = static_cast<uint8_t>(((datagramHeader.command << DATAGRAM_HEADER_SHIFT_COMMAND  ) & DATAGRAM_HEADER_MASK_COMMAND  ) |
                                    ((datagramHeader.varID   << DATAGRAM_HEADER_SHIFT_VAR_ID_HI) & DATAGRAM_HEADER_MASK_VAR_ID_HI) );

  buffer[1U] = static_cast<uint8_t>((datagramHeader.varID    << DATAGRAM_HEADER_SHIFT_VAR_ID_LO) & DATAGRAM_HEADER_MASK_VAR_ID_LO);
}

void bufferToDatagramHeader(const uint8_t *buffer, DatagramHeader_t &datagramHeader)
{
  datagramHeader.command = static_cast<uint8_t>(buffer[0U] & DATAGRAM_HEADER_MASK_COMMAND) >> DATAGRAM_HEADER_SHIFT_COMMAND;

  datagramHeader.varID   = static_cast<uint16_t>(((buffer[0U] & DATAGRAM_HEADER_MASK_VAR_ID_HI) << DATAGRAM_HEADER_SHIFT_VAR_ID_HI) |
                                                 ((buffer[1U] & DATAGRAM_HEADER_MASK_VAR_ID_LO) << DATAGRAM_HEADER_SHIFT_VAR_ID_LO) );
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
