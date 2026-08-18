/**
  ******************************************************************************
  * @file    CircularBuffer.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform-agnostic circular buffer template for Atams packet staging.
  *
  * @details Provides a statically-allocated circular byte buffer for staging
  *          incoming serial data prior to packet extraction. Bytes are pushed to
  *          the head from a receive callback (potentially from an ISR) and consumed
  *          from the tail via getPacket(), which scans for a configurable end-of-line
  *          character to delimit complete packets.
  *
  *          Concurrent access is protected via the LockPolicy template parameter,
  *          allowing the locking mechanism to be adapted to the target platform
  *          without duplicating buffer logic.
  *
  *          LockPolicy requirements:
  *            - using ArgumentType_t = <lock discriminator type>
  *            - static constexpr uint16_t BUFFER_SIZE
  *            - void acquireLock(void)
  *            - void releaseLock(void)
  *            - void setArgument(ArgumentType_t)
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include <atomic>
#include <string.h>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITION                                                                  */
/*************************************************************************************/

template <typename LockPolicy>
class CircularBufferBase
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  static constexpr uint8_t DEFAULT_EOL_CHAR {0U};

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    ERROR_NONE                 = 0U,
    ERROR_FULL                 = 1U,
    ERROR_NO_NEW_DATA          = 2U,
    ERROR_INPUT_BUFFER_LENGTH  = 3U,
    ERROR_OUTPUT_BUFFER_LENGTH = 4U,
    ERROR_NO_EOL_FOUND         = 5U,
    ERROR_NO_EOL_BUFFER_FULL   = 6U,
    ERROR_NULLPTR              = 7U,

  } Error_t;

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  CircularBufferBase(void);

  /* Parameterised Constructor */
  CircularBufferBase(const uint8_t endOfLineChar);

  /* Destructor */
  virtual ~CircularBufferBase(void) = default;

  /* Copy Constructor */
  CircularBufferBase(const CircularBufferBase &other) = delete;

  /* Copy Assignment Operator */
  CircularBufferBase & operator=(const CircularBufferBase &other) = delete;

  /* Move Constructor */
  CircularBufferBase(CircularBufferBase &&other) = delete;

  /* Move Assignment Operator */
  CircularBufferBase & operator=(CircularBufferBase &&other) = delete;

  void setEOLChar(const uint8_t endOfLineChar);

  void setLockArgument(const typename LockPolicy::ArgumentType_t arg);

  void reset(void);

  Error_t getPacket(uint8_t * const targetBuffer,
                    const uint16_t  maxOutputLength,
                    uint16_t       &outputLength);

  Error_t pushHead(const uint8_t * const   inputBuffer,
                   volatile const uint16_t inputLength);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static constexpr uint16_t STATIC_BUFFER_SIZE {LockPolicy::BUFFER_SIZE};
  static constexpr uint32_t NEW_DATA_READY     {1U};
  static constexpr uint32_t NEW_DATA_NOT_READY {0U};

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  volatile uint16_t headIndex_      {0U};
  volatile uint16_t tailIndex_      {0U};
  volatile uint16_t eolSearchIndex_ {0U};
  volatile uint16_t byteCount_      {0U};

  std::atomic<uint32_t> newDataReady_ {NEW_DATA_NOT_READY};

  uint8_t buffer_[STATIC_BUFFER_SIZE];
  uint8_t eolChar_ {DEFAULT_EOL_CHAR};

  LockPolicy lockPolicy_;

  /*-- Private Function Declarations ------------------------------------------------*/

  inline void     incrementEOLIndex(void);

  inline void     increaseHeadIndex(volatile const uint16_t length);

  inline void     increaseTailIndex(volatile const uint16_t length);

  inline void     resetEOLIndex(void);

  inline uint16_t calcEolToTail(void);

  inline uint16_t calcEolToHead(void);

  inline Error_t  eolSearch(void);

};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

template <typename LockPolicy>
CircularBufferBase<LockPolicy>::CircularBufferBase(void)
{
  for (uint8_t &byte : buffer_) byte = 0U;
}

template <typename LockPolicy>
CircularBufferBase<LockPolicy>::CircularBufferBase(const uint8_t endOfLineChar)
{
  eolChar_ = endOfLineChar;
  for (uint8_t &byte : buffer_) byte = 0U;
}

template <typename LockPolicy>
void CircularBufferBase<LockPolicy>::setEOLChar(const uint8_t endOfLineChar)
{
  eolChar_ = endOfLineChar;
}

template <typename LockPolicy>
void CircularBufferBase<LockPolicy>::setLockArgument(const typename LockPolicy::ArgumentType_t arg)
{
  lockPolicy_.setArgument(arg);
}

template <typename LockPolicy>
void CircularBufferBase<LockPolicy>::reset(void)
{
  lockPolicy_.acquireLock();
  headIndex_      = 0U;
  tailIndex_      = 0U;
  eolSearchIndex_ = 0U;
  byteCount_      = 0U;
  newDataReady_.store(NEW_DATA_NOT_READY);
  lockPolicy_.releaseLock();
}

template <typename LockPolicy>
typename CircularBufferBase<LockPolicy>::Error_t
CircularBufferBase<LockPolicy>::getPacket(uint8_t * const targetBuffer,
                                          const uint16_t  maxOutputLength,
                                          uint16_t       &outputLength)
{
  if (newDataReady_.load() != NEW_DATA_READY)
  {
    return (ERROR_NO_NEW_DATA); /* Early Return */
  }

  lockPolicy_.acquireLock();

  Error_t eolSearchResult {eolSearch()};

  if (eolSearchResult != CircularBufferBase::ERROR_NONE)
  {
    lockPolicy_.releaseLock();
    return (eolSearchResult); /* Early Return */
  }

  outputLength = calcEolToTail();

  if (maxOutputLength < outputLength)
  {
    /* Discard the data that is too long */
    increaseTailIndex(outputLength);
    lockPolicy_.releaseLock();
    return (ERROR_OUTPUT_BUFFER_LENGTH); /* Early Return */
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

  if (byteCount_ == 0U) newDataReady_.store(NEW_DATA_NOT_READY);

  lockPolicy_.releaseLock();

  return (ERROR_NONE);
}

template <typename LockPolicy>
typename CircularBufferBase<LockPolicy>::Error_t
CircularBufferBase<LockPolicy>::pushHead(const uint8_t * const   inputBuffer,
                                         volatile const uint16_t inputLength)
{
  if (inputBuffer == nullptr)
  {
    return (ERROR_NULLPTR);             /* Early Return */
  }

  if ((inputLength >  STATIC_BUFFER_SIZE) ||
      (inputLength == 0U                ) )
  {
    return (ERROR_INPUT_BUFFER_LENGTH); /* Early Return */
  }

  lockPolicy_.acquireLock();

  if ((byteCount_ + inputLength) >= STATIC_BUFFER_SIZE)
  {
    lockPolicy_.releaseLock();
    return (ERROR_FULL);                /* Early Return */
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

  newDataReady_.store(NEW_DATA_READY);

  lockPolicy_.releaseLock();

  return (ERROR_NONE);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

template <typename LockPolicy>
inline void CircularBufferBase<LockPolicy>::increaseHeadIndex(volatile const uint16_t length)
{
  headIndex_  = (headIndex_ + length) % STATIC_BUFFER_SIZE;
  byteCount_ += length;
}

template <typename LockPolicy>
inline void CircularBufferBase<LockPolicy>::increaseTailIndex(volatile const uint16_t length)
{
  tailIndex_  = (tailIndex_ + length) % STATIC_BUFFER_SIZE;
  byteCount_ -= length;
}

template <typename LockPolicy>
inline void CircularBufferBase<LockPolicy>::incrementEOLIndex(void)
{
  eolSearchIndex_ = (eolSearchIndex_ + 1U) % STATIC_BUFFER_SIZE;
}

template <typename LockPolicy>
inline void CircularBufferBase<LockPolicy>::resetEOLIndex(void)
{
  eolSearchIndex_ = tailIndex_;
}

template <typename LockPolicy>
inline uint16_t CircularBufferBase<LockPolicy>::calcEolToTail(void)
{
  return ((eolSearchIndex_ >= tailIndex_) ?
          (eolSearchIndex_ - tailIndex_)  :
          (STATIC_BUFFER_SIZE - tailIndex_ + eolSearchIndex_));
}

template <typename LockPolicy>
inline uint16_t CircularBufferBase<LockPolicy>::calcEolToHead(void)
{
  return ((headIndex_ >= eolSearchIndex_)  ?
          (headIndex_ - eolSearchIndex_)   :
          (STATIC_BUFFER_SIZE - eolSearchIndex_ + headIndex_));
}

template <typename LockPolicy>
inline typename CircularBufferBase<LockPolicy>::Error_t CircularBufferBase<LockPolicy>::eolSearch(void)
{
  bool eolFound {false};

  for (uint16_t searchLength {calcEolToHead()}; searchLength > 0U; searchLength--)
  {
    eolFound = (buffer_[eolSearchIndex_] == eolChar_);

    incrementEOLIndex();

    if (eolFound)
    {
      return (CircularBufferBase::ERROR_NONE);
    }
  }

  if (byteCount_ >= STATIC_BUFFER_SIZE) return (CircularBufferBase::ERROR_NO_EOL_BUFFER_FULL);
  else                                  return (CircularBufferBase::ERROR_NO_EOL_FOUND);
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */
