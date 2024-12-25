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

static CRC32 _crcAtams(CRC32_POLYNOMIAL);

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

const char* getErrorString(Error_t errorID)
{
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

Error_t decodeMeshPacket(const uint8_t  *inputPacket,
                         const uint16_t  inputPacketLength,
                               uint8_t  *decodedPacket,
                         const uint16_t  decodedPacketMaxLength,
                               uint16_t &decodedLength)
{
  COBS::Result_t COBSDecodeResult = COBS::decode(inputPacket, inputPacketLength, decodedPacket, decodedPacketMaxLength);

  if (COBSDecodeResult.status != COBS::ERROR_NONE)
  {
    return (ERROR_DECODE);
  }

  if (COBSDecodeResult.outputLength < MESH_SIZE_HEADER)
  {
    return (ERROR_DECODE);
  }

  uint32_t packetCRC = bufferToUint32(&decodedPacket[MESH_INDEX_CRC]);

  memset(&decodedPacket[MESH_INDEX_CRC], 0U, MESH_SIZE_CRC);

  if (packetCRC != _crcAtams.calculateCRC32(decodedPacket, COBSDecodeResult.outputLength))
  {
    return (ERROR_DECODE);
  }

  decodedLength = COBSDecodeResult.outputLength;

  return (ERROR_NONE);
}

Error_t encodeMeshPacket(      uint8_t  *txPacket,
                         const uint16_t  txLength,
                               uint8_t  *encodedPacket,
                         const uint16_t  encodedPacketMaxLength,
                               uint16_t &encodedLength)
{
  if (txLength < MESH_SIZE_HEADER)
  {
    return (ERROR_ENCODE);
  }

  memset(&txPacket[MESH_INDEX_CRC], 0U, MESH_SIZE_CRC);

  uint32_t CRCResult = _crcAtams.calculateCRC32(txPacket, txLength);

  uint32ToBuffer(CRCResult, &txPacket[MESH_INDEX_CRC]);

  COBS::Result_t COBSEncodeResult = COBS::encode(txPacket, txLength, encodedPacket, encodedPacketMaxLength);

  if (COBSEncodeResult.status != COBS::ERROR_NONE)
  {
    return (ERROR_ENCODE);
  }

  encodedLength = COBSEncodeResult.outputLength;

  return (ERROR_NONE);
}

void datagramHeaderToBuffer(const DatagramHeader_t &datagramHeader, uint8_t* buffer)
{
  buffer[0U] = (((datagramHeader.command << DATAGRAM_HEADER_SHIFT_COMMAND  ) & DATAGRAM_HEADER_MASK_COMMAND  ) |
                ((datagramHeader.blockID << DATAGRAM_HEADER_SHIFT_BLOCK_ID ) & DATAGRAM_HEADER_MASK_BLOCK_ID ) |
                ((datagramHeader.varID   << DATAGRAM_HEADER_MASK_VAR_ID_HI ) & DATAGRAM_HEADER_MASK_VAR_ID_HI) );
  buffer[1U] =  ((datagramHeader.varID   << DATAGRAM_HEADER_SHIFT_VAR_ID_LO) & DATAGRAM_HEADER_MASK_VAR_ID_LO);
}

void bufferToDatagramHeader(const uint8_t *buffer, DatagramHeader_t &datagramHeader)
{
  datagramHeader.command =   (buffer[0U] & DATAGRAM_HEADER_MASK_COMMAND  ) >> DATAGRAM_HEADER_SHIFT_COMMAND;
  datagramHeader.blockID =   (buffer[0U] & DATAGRAM_HEADER_MASK_BLOCK_ID ) >> DATAGRAM_HEADER_SHIFT_BLOCK_ID;
  datagramHeader.varID   = (((buffer[0U] & DATAGRAM_HEADER_MASK_VAR_ID_HI) << DATAGRAM_HEADER_SHIFT_VAR_ID_HI) &
                            ((buffer[1U] & DATAGRAM_HEADER_MASK_VAR_ID_LO) << DATAGRAM_HEADER_SHIFT_VAR_ID_LO) );
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
