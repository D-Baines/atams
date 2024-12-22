/**
  ******************************************************************************
  * @file    CircularBuffer.cpp
  *
  * @author  D. Baines
  *
  * @brief
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
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CircularBuffer::CircularBuffer(void)
{
  _eolChar      = DEFAULT_EOL_CHAR;
  _lockArgument = DEFAULT_LOCK_ARGUMENT;
  acquireLock   = nullptr;
  releaseLock   = nullptr;
}

CircularBuffer::CircularBuffer(const uint8_t endOfLineChar,
                               LockFunction_t lockFunction,
                               LockFunction_t unlockFunction,
                               const uint8_t lockArgument)
{
  _eolChar      = endOfLineChar;
  _lockArgument = lockArgument;
  acquireLock   = lockFunction;
  releaseLock   = unlockFunction;
}

void CircularBuffer::setEOLChar(const uint8_t endOfLineChar)
{
  _eolChar = endOfLineChar;
}

void CircularBuffer::setLockFunctions(LockFunction_t lockFunction,
                                      LockFunction_t unlockFunction,
                                      const uint8_t  lockArgument)
{
  acquireLock   = lockFunction;
  releaseLock   = unlockFunction;
  _lockArgument = lockArgument;
}

CircularBuffer::~CircularBuffer(void)
{
  /* Do Nothing - No dynamic allocation */
}

void CircularBuffer::reset(void)
{
  if (acquireLock != nullptr) acquireLock(_lockArgument);
  _headIndex       = 0U;
  _tailIndex       = 0U;
  _eolSearchIndex  = 0U;
  _atomicByteCount = 0U;
  if (releaseLock != nullptr) releaseLock(_lockArgument);
}

CircularBuffer::Error_t CircularBuffer::getPacket(      uint8_t  *targetBuffer,
                                                  const uint16_t  maxOutputLength,
                                                        uint16_t &outputLength)
{
  Error_t statusReturn = ERROR_NONE;

  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if (_atomicByteCount == 0U)
  {
    statusReturn = ERROR_EMPTY;
    return (statusReturn);
  }

  if (acquireLock != nullptr) acquireLock(_lockArgument);

  Error_t eolSearchResult = eolSearch();

  /* Early return if no EOL byte found */
  if (eolSearchResult != ERROR_NONE) return (eolSearchResult);

  outputLength = _eolToTail;

  if (maxOutputLength < outputLength)
  {
    resetEOLIndex();
    statusReturn = ERROR_OUTPUT_BUFFER_LENGTH;
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

  if (releaseLock != nullptr) releaseLock(_lockArgument);

  return (ERROR_NONE);
}

CircularBuffer::Error_t CircularBuffer::pushHead(const uint8_t *inputBuffer,
                                                 const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (ERROR_NULLPTR);
  }

  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if((_atomicByteCount + inputLength) >= STATIC_BUFFER_SIZE)
  {
    return (ERROR_FULL);
  }

  if (acquireLock != nullptr) acquireLock(_lockArgument);

  uint16_t preWrapLength  = STATIC_BUFFER_SIZE - _headIndex;

  if (inputLength < preWrapLength)
  {
    memcpy(&_buffer[_headIndex], inputBuffer, inputLength);
  }
  else
  {
    uint16_t postWrapLength = inputLength - preWrapLength;
    memcpy(&_buffer[_headIndex], inputBuffer, preWrapLength);
    memcpy(&_buffer[0U],         inputBuffer, postWrapLength);
  }

  increaseHeadIndex(inputLength);

  if (releaseLock != nullptr) releaseLock(_lockArgument);

  return (ERROR_NONE);
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
    if (eolFound) return (ERROR_NONE);
  }

  if (_atomicByteCount == STATIC_BUFFER_SIZE) return (ERROR_NO_EOL_BUFFER_FULL);
  else                                        return (ERROR_NO_EOL_FOUND);
}


/**
  * @}End of File
  */
