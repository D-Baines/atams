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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

CircularBuffer::CircularBuffer(const uint8_t endOfLineChar,
                               void (&lockFunction)(void),
                               void (&unlockFunction)(void)) :
_eolChar(endOfLineChar),
acquireLock(lockFunction),
releaseLock(unlockFunction)
{
  _headIndex       = 0U;
  _tailIndex       = 0U;
  _atomicByteCount = 0U;
}


CircularBuffer::~CircularBuffer(void)
{
  /* Do Nothing - No dynamic allocation */
}


void CircularBuffer::reset(void)
{
  acquireLock();
  _headIndex       = 0U;
  _tailIndex       = 0U;
  _eolSearchIndex  = 0U;
  _atomicByteCount = 0U;
  releaseLock();
}


CircularBuffer::Error_t CircularBuffer::getPacket(      uint8_t  *targetBuffer,
                                                  const uint16_t  maxOutputLength,
                                                        uint16_t &outputLength)
{
  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if (_atomicByteCount == 0U)
  {
    return (ERROR_EMPTY);
  }

  acquireLock();

  Error_t eolSearchResult = eolSearch();

  /* Early return if no EOL byte found */
  if (eolSearchResult != ERROR_NONE) return (eolSearchResult);

  outputLength = _eolToTail;

  if (maxOutputLength < outputLength)
  {
    resetEOLIndex();
    releaseLock();
    return (ERROR_OUTPUT_BUFFER_LENGTH);
  }

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

  releaseLock();

  return (ERROR_NONE);
}

CircularBuffer::Error_t CircularBuffer::pushHead(const uint8_t *inputBuffer,
                                                 const uint16_t inputLength)
{
  if (inputBuffer == nullptr)                                return (ERROR_NULLPTR);

  /* No lock required - _atomicByteCount read must be atomic on target platform */
  if((_atomicByteCount + inputLength) >= STATIC_BUFFER_SIZE) return (ERROR_FULL);

  acquireLock();

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

  releaseLock();

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
