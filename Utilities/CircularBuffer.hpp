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


/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class CircularBuffer
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

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

  /* Constructor */
  CircularBuffer(const uint8_t endOfLineChar,
                 void (&lockFunction)(void),
                 void (&unlockFunction)(void));

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

  static constexpr uint16_t STATIC_BUFFER_SIZE = 1024U;


  /*-- Private Constants ------------------------------------------------------------*/

  const uint8_t _eolChar;


  /*-- Private Constants ------------------------------------------------------------*/


  /*-- Private Typedefs -------------------------------------------------------------*/


  /*-- Private Variables ------------------------------------------------------------*/

           uint8_t  _buffer[STATIC_BUFFER_SIZE];
  volatile uint16_t _headIndex;
  volatile uint16_t _tailIndex;
  volatile uint16_t _eolSearchIndex;
  volatile uint16_t _atomicByteCount;
  volatile uint16_t _eolToHead;
  volatile uint16_t _eolToTail;


  /*-- Lock Function References -----------------------------------------------------*/
  void (&acquireLock)(void);
  void (&releaseLock)(void);


  /*-- Private Function Declarations ------------------------------------------------*/

  inline void incrementEOLIndex(void);

  inline void increaseHeadIndex(uint16_t length);

  inline void increaseTailIndex(uint16_t length);

  inline void resetEOLIndex(void);

  inline CircularBuffer::Error_t eolSearch(void);


};



/**
  * @}End of File
  */


