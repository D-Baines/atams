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
#include "../CommsCore/CommsPlatform.hpp"

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

  static constexpr uint8_t DEFAULT_EOL_CHAR {0U};

  static constexpr Platform::CommsPeripheralID_t DEFAULT_LOCK_ARGUMENT {static_cast<Platform::CommsPeripheralID_t>(0U)};

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
  CircularBuffer(void);

  /* Constructor */
  CircularBuffer(const uint8_t                       endOfLineChar,
                 const Platform::CommsPeripheralID_t channelToLock);

  void setEOLChar(const uint8_t endOfLineChar);

  void setLockArgument(const Platform::CommsPeripheralID_t channelToLock);

  /* Copy Constructor */
  CircularBuffer(const CircularBuffer &other) = delete;

  /* Copy Assignment Operator */
  CircularBuffer & operator=(const CircularBuffer &other) = delete;

  /* Destructor */
  virtual ~CircularBuffer(void);

  void reset(void);

  Error_t getPacket(uint8_t        *targetBuffer,
                    const uint16_t  maxOutputLength,
                    uint16_t       &outputLength);

  Error_t pushHead(const uint8_t          *inputBuffer,
                   volatile const uint16_t inputLength);


  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static constexpr uint16_t STATIC_BUFFER_SIZE {Platform::CIRCULAR_BUFFER_SIZE};
  static constexpr uint8_t  NEW_DATA_READY     {1U};

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  volatile uint16_t headIndex_       {0U};
  volatile uint16_t tailIndex_       {0U};
  volatile uint16_t eolSearchIndex_  {0U};
  volatile uint16_t byteCount_       {0U};
  volatile uint16_t eolToHead_       {0U};
  volatile uint16_t eolToTail_       {0U};
  volatile uint8_t  newDataReady_    {!CircularBuffer::NEW_DATA_READY}; /* UINT8_T MUST BE ATOMIC ON TARGET PLATFORM */

  uint8_t buffer_[STATIC_BUFFER_SIZE];

  uint8_t                       eolChar_      {DEFAULT_EOL_CHAR};
  Platform::CommsPeripheralID_t lockArgument_ {DEFAULT_LOCK_ARGUMENT};

  /*-- Private Function Declarations ------------------------------------------------*/

  inline void incrementEOLIndex(void);

  inline void increaseHeadIndex(volatile const uint16_t length);

  inline void increaseTailIndex(volatile const uint16_t length);

  inline void resetEOLIndex(void);

  inline uint16_t calcEolToTail(void);

  inline uint16_t calcEolToHead(void);

  inline CircularBuffer::Error_t eolSearch(void);

};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


