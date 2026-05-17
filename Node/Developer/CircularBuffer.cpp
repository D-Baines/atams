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

#include "CircularBuffer.hpp"

#include <string.h>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

CircularBuffer::CircularBuffer(void)
{
  for (uint8_t &byte :buffer_) byte = 0U;
}

CircularBuffer::CircularBuffer(const uint8_t                       endOfLineChar,
                               const Platform::CommsPeripheralID_t channelToLock)
{
  eolChar_      = endOfLineChar;
  lockArgument_ = channelToLock;
  for (uint8_t &byte :buffer_) byte = 0U;
}

void CircularBuffer::setEOLChar(const uint8_t endOfLineChar)
{
  eolChar_ = endOfLineChar;
}

void CircularBuffer::setLockArgument(const Platform::CommsPeripheralID_t channelToLock)
{
  lockArgument_ = channelToLock;
}

void CircularBuffer::reset(void)
{
  Platform::acquireCommsBufferLock(lockArgument_);
  headIndex_      = 0U;
  tailIndex_      = 0U;
  eolSearchIndex_ = 0U;
  byteCount_      = 0U;
  newDataReady_.store(!CircularBuffer::NEW_DATA_READY);
  Platform::releaseCommsBufferLock(lockArgument_);
}

CircularBuffer::Error_t CircularBuffer::getPacket(uint8_t * const targetBuffer,
                                                  const uint16_t  maxOutputLength,
                                                  uint16_t       &outputLength)
{
  if (newDataReady_.load() != CircularBuffer::NEW_DATA_READY)
  {
    return (CircularBuffer::ERROR_NO_NEW_DATA); /* Early Return */
  }

  Platform::acquireCommsBufferLock(lockArgument_);

  CircularBuffer::Error_t eolSearchResult {eolSearch()};

  if (eolSearchResult != CircularBuffer::ERROR_NONE)
  {
    Platform::releaseCommsBufferLock(lockArgument_);
    return (eolSearchResult); /* Early Return */
  }

  outputLength = calcEolToTail();

  if (maxOutputLength < outputLength)
  {
    /* Discard the data that is too long */
    increaseTailIndex(outputLength);
    Platform::releaseCommsBufferLock(lockArgument_);
    return (CircularBuffer::ERROR_OUTPUT_BUFFER_LENGTH); /* Early Return */
  }

  uint16_t preWrapLength {static_cast<uint16_t>(STATIC_BUFFER_SIZE - tailIndex_)};

  if (outputLength < preWrapLength)
  {
    memcpy(&targetBuffer[0U], &buffer_[tailIndex_], outputLength);
  }
  else
  {
    uint16_t postWrapLength {static_cast<uint16_t>(outputLength - preWrapLength)};
    memcpy(&targetBuffer[0U],            &buffer_[tailIndex_], preWrapLength);
    memcpy(&targetBuffer[preWrapLength], &buffer_[0U],         postWrapLength);
  }

  increaseTailIndex(outputLength);

  if (byteCount_ == 0U) newDataReady_.store(!CircularBuffer::NEW_DATA_READY);

  Platform::releaseCommsBufferLock(lockArgument_);

  return (CircularBuffer::ERROR_NONE);
}

CircularBuffer::Error_t CircularBuffer::pushHead(const uint8_t * const   inputBuffer,
                                                 volatile const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (CircularBuffer::ERROR_NULLPTR);             /* Early Return */
  }

  if ((inputLength >  STATIC_BUFFER_SIZE) ||
      (inputLength == 0U                ) )
  {
    return (CircularBuffer::ERROR_INPUT_BUFFER_LENGTH); /* Early Return */
  }

  Platform::acquireCommsBufferLock(lockArgument_);

  if ((byteCount_ + inputLength) >= STATIC_BUFFER_SIZE)
  {
    Platform::releaseCommsBufferLock(lockArgument_);
    return (CircularBuffer::ERROR_FULL);                /* Early Return */
  }

  uint16_t preWrapLength {static_cast<uint16_t>(STATIC_BUFFER_SIZE - headIndex_)};

  if (inputLength < preWrapLength)
  {
    memcpy(&buffer_[headIndex_], inputBuffer, inputLength);
  }
  else
  {
    uint16_t postWrapLength {static_cast<uint16_t>(inputLength - preWrapLength)};
    memcpy(&buffer_[headIndex_], inputBuffer, preWrapLength);
    memcpy(&buffer_[0U], &inputBuffer[preWrapLength], postWrapLength);
  }

  increaseHeadIndex(inputLength);

  newDataReady_.store(CircularBuffer::NEW_DATA_READY);

  Platform::releaseCommsBufferLock(lockArgument_);

  return (CircularBuffer::ERROR_NONE);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

inline void CircularBuffer::increaseHeadIndex(volatile const uint16_t length)
{
  headIndex_  = (headIndex_ + length) % STATIC_BUFFER_SIZE;
  byteCount_ += length;
}

inline void CircularBuffer::increaseTailIndex(volatile const uint16_t length)
{
  tailIndex_  = (tailIndex_ + length) % STATIC_BUFFER_SIZE;
  byteCount_ -= length;
}

inline void CircularBuffer::incrementEOLIndex(void)
{
  eolSearchIndex_ = (eolSearchIndex_ + 1U) % STATIC_BUFFER_SIZE;
}

inline void CircularBuffer::resetEOLIndex(void)
{
  eolSearchIndex_ = tailIndex_;
}

inline uint16_t CircularBuffer::calcEolToTail(void)
{
    return ((eolSearchIndex_ >= tailIndex_) ?
            (eolSearchIndex_ - tailIndex_)  :
            (STATIC_BUFFER_SIZE - tailIndex_ + eolSearchIndex_));
}

inline uint16_t CircularBuffer::calcEolToHead(void)
{
    return ((headIndex_ >= eolSearchIndex_) ?
            (headIndex_ - eolSearchIndex_)  :
            (STATIC_BUFFER_SIZE - eolSearchIndex_ + headIndex_));
}

inline CircularBuffer::Error_t CircularBuffer::eolSearch(void)
{
  bool eolFound {false};

  for (uint16_t searchLength {calcEolToHead()}; searchLength > 0U; searchLength--)
  {
    eolFound = (buffer_[eolSearchIndex_] == eolChar_);

    incrementEOLIndex();

    if (eolFound)
    {
      return (CircularBuffer::ERROR_NONE);
    }
  }

  if (byteCount_ >= STATIC_BUFFER_SIZE) return (CircularBuffer::ERROR_NO_EOL_BUFFER_FULL);
  else                                  return (CircularBuffer::ERROR_NO_EOL_FOUND);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
