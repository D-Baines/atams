/**
  ******************************************************************************
  * @file    CircularBuffer.cpp
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

#include "string.h"
#include "CircularBuffer.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CircularBuffer::CircularBuffer(void)
{
  for (uint8_t &byte :_buffer) byte = 0U;
}

CircularBuffer::CircularBuffer(const uint8_t endOfLineChar)
{
  _eolChar      = endOfLineChar;
  for (uint8_t &byte :_buffer) byte = 0U;
}

void CircularBuffer::setEOLChar(const uint8_t endOfLineChar)
{
  _eolChar = endOfLineChar;
}

CircularBuffer::~CircularBuffer(void)
{
  /* Do Nothing - No dynamic allocation */
}

void CircularBuffer::reset(void)
{
  Platform::CommsLock::acquireLock();
  _headIndex       = 0U;
  _tailIndex       = 0U;
  _eolSearchIndex  = 0U;
  _atomicByteCount = 0U;
  Platform::CommsLock::releaseLock();
}

CircularBuffer::Error CircularBuffer::getPacket(      uint8_t  *targetBuffer,
                                                const uint16_t  maxOutputLength,
                                                      uint16_t &outputLength)
{
  CircularBuffer::Error statusReturn = Error::NONE;

  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if (_atomicByteCount == 0U)
  {
    statusReturn = Error::EMPTY;
    return (statusReturn);
  }

  Platform::CommsLock::acquireLock();

  CircularBuffer::Error eolSearchResult = eolSearch();

  /* Early return if no EOL byte found */
  if (eolSearchResult != CircularBuffer::Error::NONE) 
  {
    Platform::CommsLock::releaseLock();
    return (eolSearchResult);
  }

  outputLength = _eolToTail;

  if (maxOutputLength < outputLength)
  {
    resetEOLIndex();
    statusReturn = CircularBuffer::Error::OUTPUT_BUFFER_LENGTH;
  }

  else
  {
    uint16_t preWrapLength = STATIC_BUFFER_SIZE - _tailIndex;

    if (outputLength < preWrapLength)
    {
      memcpy(&targetBuffer[0U], &_buffer[_tailIndex], outputLength);
    }
    else
    {
      uint16_t postWrapLength = outputLength - preWrapLength;
      memcpy(&targetBuffer[0U],            &_buffer[_tailIndex], preWrapLength);
      memcpy(&targetBuffer[preWrapLength], &_buffer[0U],         postWrapLength);
    }

    increaseTailIndex(outputLength);
  }

  Platform::CommsLock::releaseLock();

  return (CircularBuffer::Error::NONE);
}

CircularBuffer::Error CircularBuffer::pushHead(const uint8_t *inputBuffer,
                                               const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (CircularBuffer::Error::NULLPTR);
  }

  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if((_atomicByteCount + inputLength) >= STATIC_BUFFER_SIZE)
  {
    return (CircularBuffer::Error::FULL);
  }

  Platform::CommsLock::acquireLock();

  uint16_t preWrapLength  = STATIC_BUFFER_SIZE - _headIndex;

  if (inputLength < preWrapLength)
  {
    memcpy(&_buffer[_headIndex], inputBuffer, inputLength);
  }
  else
  {
    uint16_t postWrapLength = inputLength - preWrapLength;
    memcpy(&_buffer[_headIndex], inputBuffer, preWrapLength);
    memcpy(&_buffer[0U], &inputBuffer[preWrapLength], postWrapLength);
  }

  increaseHeadIndex(inputLength);

  Platform::CommsLock::releaseLock();

  return (CircularBuffer::Error::NONE);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

inline void CircularBuffer::increaseHeadIndex(uint16_t length)
{
  _headIndex        = (_headIndex + length) % STATIC_BUFFER_SIZE;
  _atomicByteCount += length;
  _eolToHead       += length;
}

inline void CircularBuffer::increaseTailIndex(uint16_t length)
{
  _tailIndex        = (_tailIndex + length) % STATIC_BUFFER_SIZE;
  _atomicByteCount -= length;
  _eolToTail       -= length;
}

inline void CircularBuffer::incrementEOLIndex(void)
{
  _eolSearchIndex = (_eolSearchIndex + 1U) % STATIC_BUFFER_SIZE;
  _eolToHead--;
  _eolToTail++;
}

inline void CircularBuffer::resetEOLIndex(void)
{
  _eolSearchIndex  = _tailIndex;
  _eolToTail       = 0U;
  _eolToHead       = _atomicByteCount;
}

inline CircularBuffer::Error CircularBuffer::eolSearch(void)
{
  bool eolFound = false;

  for (uint16_t searchLength = _eolToHead; searchLength > 0U; searchLength--)
  {
    eolFound = (_buffer[_eolSearchIndex] == _eolChar);
    incrementEOLIndex();
    if (eolFound) return (CircularBuffer::Error::NONE);
  }

  if (_atomicByteCount == STATIC_BUFFER_SIZE) return (CircularBuffer::Error::NO_EOL_BUFFER_FULL);
  else                                        return (CircularBuffer::Error::NO_EOL_FOUND);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
