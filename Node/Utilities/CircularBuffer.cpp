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
  _headIndex      = 0U;
  _tailIndex      = 0U;
  _eolSearchIndex = 0U;
  _byteCount      = 0U;
  _newDataReady   = !CircularBuffer::NEW_DATA_READY;
  Platform::releaseCommsBufferLock(_lockArgument);
}

CircularBuffer::Error_t CircularBuffer::getPacket(uint8_t        *targetBuffer,
                                                  const uint16_t  maxOutputLength,
                                                  uint16_t       &outputLength)
{
  CircularBuffer::Error_t statusReturn = CircularBuffer::ERROR_NONE;

  if (_newDataReady != CircularBuffer::NEW_DATA_READY)
  {
    statusReturn = CircularBuffer::ERROR_NO_NEW_DATA;
    return (statusReturn);
  }

  Platform::acquireCommsBufferLock(_lockArgument);

  CircularBuffer::Error_t eolSearchResult = eolSearch();

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

  if (_byteCount == 0U) _newDataReady = !CircularBuffer::NEW_DATA_READY;

  Platform::releaseCommsBufferLock(_lockArgument);

  return (CircularBuffer::ERROR_NONE);
}

CircularBuffer::Error_t CircularBuffer::pushHead(const uint8_t          *inputBuffer,
                                                 volatile const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (CircularBuffer::ERROR_NULLPTR);
  }

  Platform::acquireCommsBufferLock(_lockArgument);

  if (inputLength > STATIC_BUFFER_SIZE)
  {
    Platform::releaseCommsBufferLock(_lockArgument);
    return (CircularBuffer::ERROR_INPUT_BUFFER_LENGTH); /* Early Return */
  }

  if ((_byteCount + inputLength) >= STATIC_BUFFER_SIZE)
  {
    Platform::releaseCommsBufferLock(_lockArgument);
    return (CircularBuffer::ERROR_FULL); /* Early Return */
  }

  uint16_t preWrapLength = STATIC_BUFFER_SIZE - _headIndex;

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

  Platform::releaseCommsBufferLock(_lockArgument);

  return (CircularBuffer::ERROR_NONE);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

inline void CircularBuffer::increaseHeadIndex(volatile const uint16_t length)
{
  _headIndex  = (_headIndex + length) % STATIC_BUFFER_SIZE;
  _byteCount += length;
  _eolToHead += length;
}

inline void CircularBuffer::increaseTailIndex(volatile const uint16_t length)
{
  _tailIndex  = (_tailIndex + length) % STATIC_BUFFER_SIZE;
  _byteCount -= length;
  _eolToTail -= length;
}

inline void CircularBuffer::incrementEOLIndex(void)
{
  _eolSearchIndex = (_eolSearchIndex + 1U) % STATIC_BUFFER_SIZE;
  _eolToHead--;
  _eolToTail++;
}

inline void CircularBuffer::resetEOLIndex(void)
{
  _eolSearchIndex = _tailIndex;
  _eolToTail      = 0U;
  _eolToHead      = _byteCount;
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

  if (_byteCount >= STATIC_BUFFER_SIZE) return (CircularBuffer::ERROR_NO_EOL_BUFFER_FULL);
  else                                  return (CircularBuffer::ERROR_NO_EOL_FOUND);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
