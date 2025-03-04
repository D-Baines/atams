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

  enum class Error: uint8_t
  {
    NONE                 = 0U,
    FULL                 = 1U,
    EMPTY                = 2U,
    OUTPUT_BUFFER_LENGTH = 3U,
    NO_EOL_FOUND         = 4U,
    NO_EOL_BUFFER_FULL   = 5U,
    NULLPTR              = 6U,
  };

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  CircularBuffer(void);

  /* Constructor */
  CircularBuffer(const uint8_t endOfLineChar);

  void setEOLChar(const uint8_t endOfLineChar);

  /* Copy Constructor */
  CircularBuffer(const CircularBuffer &other) = delete;

  /* Copy Assignment Operator */
  CircularBuffer & operator=(const CircularBuffer &other) = delete;

  /* Destructor */
  virtual ~CircularBuffer(void);
  void reset(void);

  CircularBuffer::Error getPacket(      uint8_t  *targetBuffer,
                                  const uint16_t  maxOutputLength,
                                        uint16_t &outputLength);

  CircularBuffer::Error pushHead(const uint8_t *inputBuffer,
                                 const uint16_t inputLength);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static inline constexpr uint16_t STATIC_BUFFER_SIZE = Platform::COMMS_BUFFER_SIZE;

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  uint16_t _headIndex       = 0U;
  uint16_t _tailIndex       = 0U;
  uint16_t _eolSearchIndex  = 0U;
  uint16_t _atomicByteCount = 0U;
  uint16_t _eolToHead       = 0U;
  uint16_t _eolToTail       = 0U;

  uint8_t _buffer[STATIC_BUFFER_SIZE];
  uint8_t _eolChar = DEFAULT_EOL_CHAR;

  /*-- Private Function Declarations ------------------------------------------------*/

  inline void incrementEOLIndex(void);

  inline void increaseHeadIndex(uint16_t length);

  inline void increaseTailIndex(uint16_t length);

  inline void resetEOLIndex(void);

  inline CircularBuffer::Error eolSearch(void);

};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


