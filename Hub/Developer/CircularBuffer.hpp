/**
  ******************************************************************************
  * @file    CircularBuffer.hpp
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class CircularBuffer :
private Platform::CommsLock
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  static inline constexpr uint8_t DEFAULT_EOL_CHAR = 0U;

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    ERROR_NONE                 = 0U,
    ERROR_FULL                 = 1U,
    ERROR_NO_NEW_DATA          = 2U,
    ERROR_OUTPUT_BUFFER_LENGTH = 3U,
    ERROR_NO_EOL_FOUND         = 4U,
    ERROR_NO_EOL_BUFFER_FULL   = 5U,
    ERROR_NULLPTR              = 6U,

  } Error_t;

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  CircularBuffer(void);

  /* Constructor */
  CircularBuffer(const uint8_t endOfLineChar);

  /* Destructor */
  virtual ~CircularBuffer(void);

  /* Copy Constructor */
  CircularBuffer(const CircularBuffer &other) = delete;

  /* Copy Assignment Operator */
  CircularBuffer & operator=(const CircularBuffer &other) = delete;

  /* Move Constructor */
  CircularBuffer(CircularBuffer &&other) = delete;

  /* Move Assignment Operator */
  CircularBuffer & operator=(CircularBuffer &&other) = delete;

  void setEOLChar(const uint8_t endOfLineChar);

  void reset(void);

  CircularBuffer::Error_t getPacket(      uint8_t  *targetBuffer,
                                    const uint16_t  maxOutputLength,
                                          uint16_t &outputLength);

  CircularBuffer::Error_t pushHead(const uint8_t *inputBuffer,
                                   const uint16_t inputLength);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static inline constexpr uint16_t STATIC_BUFFER_SIZE = Platform::CIRCULAR_BUFFER_SIZE;
  static inline constexpr uint8_t  NEW_DATA_READY     = 1U;

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  volatile uint16_t _headIndex       = 0U;
  volatile uint16_t _tailIndex       = 0U;
  volatile uint16_t _eolSearchIndex  = 0U;
  volatile uint16_t _atomicByteCount = 0U;
  volatile uint16_t _eolToHead       = 0U;
  volatile uint16_t _eolToTail       = 0U;
  volatile uint8_t  _newDataReady    = !CircularBuffer::NEW_DATA_READY; /* UINT8_T MUST BE ATOMIC ON TARGET PLATFORM */

  uint8_t _buffer[STATIC_BUFFER_SIZE];
  uint8_t _eolChar = DEFAULT_EOL_CHAR;

  /*-- Private Function Declarations ------------------------------------------------*/

  inline void incrementEOLIndex(void);

  inline void increaseHeadIndex(uint16_t length);

  inline void increaseTailIndex(uint16_t length);

  inline void resetEOLIndex(void);

  inline CircularBuffer::Error_t eolSearch(void);

};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


