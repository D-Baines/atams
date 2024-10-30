/**
  ******************************************************************************
  * @file    CircularBuffer.hpp
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

class CircularBuffer
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  static inline constexpr uint8_t                  DEFAULT_EOL_CHAR      = 0U;
  static inline constexpr Platform::CommsChannel_t DEFAULT_LOCK_ARGUMENT = static_cast<Platform::CommsChannel_t>(0U);

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    ERROR_NONE                 = 0U,
    ERROR_FULL                 = 1U,
    ERROR_EMPTY                = 2U,
    ERROR_OUTPUT_BUFFER_LENGTH = 3U,
    ERROR_NO_EOL_FOUND         = 4U,
    ERROR_NO_EOL_BUFFER_FULL   = 5U,
    ERROR_NULLPTR              = 6U,

  } Error_t;

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  CircularBuffer(void);

  /* Constructor */
  CircularBuffer(const uint8_t                  endOfLineChar,
                 const Platform::CommsChannel_t channelToLock);

  void setEOLChar(const uint8_t endOfLineChar);

  void setLockArgument(const Platform::CommsChannel_t channelToLock);

  /* Copy Constructor */
  CircularBuffer(const CircularBuffer &other) = delete;

  /* Copy Assignment Operator */
  CircularBuffer & operator=(const CircularBuffer &other) = delete;

  /* Destructor */
  virtual ~CircularBuffer(void);

  void reset(void);

  Error_t getPacket(      uint8_t  *targetBuffer,
                    const uint16_t  maxOutputLength,
                          uint16_t &outputLength);

  Error_t pushHead(const uint8_t *inputBuffer,
                   const uint16_t inputLength);


  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static inline constexpr uint16_t STATIC_BUFFER_SIZE = 1024U;

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

  uint8_t _buffer[STATIC_BUFFER_SIZE];

  uint8_t                  _eolChar      = DEFAULT_EOL_CHAR;
  Platform::CommsChannel_t _lockArgument = DEFAULT_LOCK_ARGUMENT;

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


