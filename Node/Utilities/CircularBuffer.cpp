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

// _atomicByteCount is accessed by two threads:
// - Producer thread: only increments, and reads to check buffer fullness.
// - Consumer thread: only decrements, and reads to check buffer emptiness.
// All modifications are done inside a lock.
// Reads are done outside the lock, but this is safe because:
// - Each thread only modifies in one direction (increment or decrement).
// - Reads are used for early exit only; actual data access is locked.
// Assumes 1-byte atomicity on target platform. `volatile` ensures no compiler caching.

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CircularBuffer::CircularBuffer(void)
{
  for (uint8_t &byte :_buffer) byte = 0U;
}

CircularBuffer::CircularBuffer(const uint8_t                  endOfLineChar,
                               const Platform::CommsChannel_t channelToLock)
{
  _eolChar      = endOfLineChar;
  _lockArgument = channelToLock;
  for (uint8_t &byte :_buffer) byte = 0U;
}

void CircularBuffer::setEOLChar(const uint8_t endOfLineChar)
{
  _eolChar = endOfLineChar;
}

void CircularBuffer::setLockArgument(const Platform::CommsChannel_t channelToLock)
{
  _lockArgument = channelToLock;
}

CircularBuffer::~CircularBuffer(void)
{
  /* Do Nothing */
}

void CircularBuffer::reset(void)
{
  Platform::acquireCommsBufferLock(_lockArgument);
  _headIndex       = 0U;
  _tailIndex       = 0U;
  _eolSearchIndex  = 0U;
  _atomicByteCount = 0U;
  _newDataReady    = !CircularBuffer::NEW_DATA_READY;
  Platform::releaseCommsBufferLock(_lockArgument);
}

CircularBuffer::Error_t CircularBuffer::getPacket(      uint8_t  *targetBuffer,
                                                  const uint16_t  maxOutputLength,
                                                        uint16_t &outputLength)
{
  Error_t statusReturn = CircularBuffer::ERROR_NONE;

  if (_newDataReady != CircularBuffer::NEW_DATA_READY)
  {
    statusReturn = CircularBuffer::ERROR_NO_NEW_DATA;
    return (statusReturn);
  }

  Platform::acquireCommsBufferLock(_lockArgument);

  _newDataReady = !CircularBuffer::NEW_DATA_READY;

  Error_t eolSearchResult = eolSearch();

  /* Early return if no EOL byte found */
  if (eolSearchResult != CircularBuffer::ERROR_NONE)
  {
    Platform::releaseCommsBufferLock(_lockArgument);
    return (eolSearchResult);
  }

  outputLength = _eolToTail;

  if (maxOutputLength < outputLength)
  {
    resetEOLIndex();
    statusReturn = CircularBuffer::ERROR_OUTPUT_BUFFER_LENGTH;
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

  Platform::releaseCommsBufferLock(_lockArgument);

  return (CircularBuffer::ERROR_NONE);
}

CircularBuffer::Error_t CircularBuffer::pushHead(const uint8_t *inputBuffer,
                                                 const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (CircularBuffer::ERROR_NULLPTR);
  }

  Platform::acquireCommsBufferLock(_lockArgument);

  if((_atomicByteCount + inputLength) >= STATIC_BUFFER_SIZE)
  {
    return (CircularBuffer::ERROR_FULL);
  }

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

  _newDataReady = CircularBuffer::NEW_DATA_READY;

  Platform::signalCommsBufferSemaphore();

  Platform::releaseCommsBufferLock(_lockArgument);

  return (CircularBuffer::ERROR_NONE);
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

inline CircularBuffer::Error_t CircularBuffer::eolSearch(void)
{
  bool eolFound = false;

  for (uint16_t searchLength = _eolToHead; searchLength > 0U; searchLength--)
  {
    eolFound = (_buffer[_eolSearchIndex] == _eolChar);
    incrementEOLIndex();
    if (eolFound) return (CircularBuffer::ERROR_NONE);
  }

  if (_atomicByteCount == STATIC_BUFFER_SIZE) return (CircularBuffer::ERROR_NO_EOL_BUFFER_FULL);
  else                                        return (CircularBuffer::ERROR_NO_EOL_FOUND);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
