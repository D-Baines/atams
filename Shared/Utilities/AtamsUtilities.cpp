/**
  ******************************************************************************
  * @file    AtamsUtilities.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of shared Atams packet encoding, decoding, and validation utilities.
  *
  * @details Implements the utility functions declared in AtamsUtilities.hpp. Bus packets
  *          are COBS-encoded with a CRC32 in the packet header; decoding reverses this
  *          and validates the checksum. Memory Map validation checks the Atams version,
  *          variable count, universal block layout, and user block checksum. A
  *          module-level CRC32 instance is shared across encode, decode, and validation.
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "AtamsUtilities.hpp"

#include <string.h>

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

static CRC32 atamsCRC_(Atams::CRC32_POLYNOMIAL);

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/* Gives the templated validate*() functions in AtamsUtilities.hpp access to the one shared
 * CRC32 instance (and its precomputed lookup table) also used by decodeBusPacket/
 * encodeBusPacket below, instead of each template instantiation building its own table. */
Atams::CRC32 &getAtamsCRC32(void)
{
  return (atamsCRC_);
}

uint32_t bufferToUint32(const uint8_t * const buffer)
{
  /* Little endian: LSB first */
  return ((static_cast<uint32_t>(buffer[ZERO_BYTE_INDEX ])                     ) |
          (static_cast<uint32_t>(buffer[ONE_BYTE_INDEX  ]) << SINGLE_BYTE_SHIFT) |
          (static_cast<uint32_t>(buffer[TWO_BYTE_INDEX  ]) << TWO_BYTE_SHIFT   ) |
          (static_cast<uint32_t>(buffer[THREE_BYTE_INDEX]) << THREE_BYTE_SHIFT ) );
}

void uint32ToBuffer(const uint32_t value, uint8_t * const buffer)
{
  /* Little endian: LSB first */
  buffer[ZERO_BYTE_INDEX ] = static_cast<uint8_t>((value                     ) & SINGLE_BYTE_MASK);
  buffer[ONE_BYTE_INDEX  ] = static_cast<uint8_t>((value >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  buffer[TWO_BYTE_INDEX  ] = static_cast<uint8_t>((value >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  buffer[THREE_BYTE_INDEX] = static_cast<uint8_t>((value >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
}

Atams::Error_t decodeBusPacket(const uint8_t  * const inputBuffer,
                               const uint16_t         inputBufferLength,
                                     uint8_t  * const decodedBuffer,
                               const uint16_t         decodedBufferMaxLength,
                                     uint16_t        &decodedLength)
{
  COBS::Result_t COBSDecodeResult {COBS::decode(inputBuffer, inputBufferLength, decodedBuffer, decodedBufferMaxLength)};

  if (COBSDecodeResult.status != COBS::ERROR_NONE)
  {
    return (Atams::ERROR_DECODE_FRAMING);
  }

  if (COBSDecodeResult.outputLength < Atams::PACKET_HEADER_SIZE)
  {
    return (Atams::ERROR_DECODE_FRAMING);
  }

  uint32_t packetCRC {bufferToUint32(&decodedBuffer[Atams::HEADER_INDEX_CRC])};

  memset(&decodedBuffer[Atams::HEADER_INDEX_CRC], 0U, Atams::HEADER_SIZE_CRC);

  if (packetCRC != atamsCRC_.calculateCRC(decodedBuffer, COBSDecodeResult.outputLength))
  {
    decodedLength = COBSDecodeResult.outputLength;

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
  if (inputLength < PACKET_HEADER_SIZE)
  {
    return (Atams::ERROR_ENCODE);
  }

  memset(&inputBuffer[HEADER_INDEX_CRC], 0U, HEADER_SIZE_CRC);

  uint32_t CRCResult {atamsCRC_.calculateCRC(inputBuffer, inputLength)};

  uint32ToBuffer(CRCResult, &inputBuffer[HEADER_INDEX_CRC]);

  COBS::Result_t COBSEncodeResult {COBS::encode(inputBuffer, inputLength, encodedBuffer, encodedBufferMaxLength)};

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

} /* End Namespace - Atams */


/**
  * @}End of File
  */
