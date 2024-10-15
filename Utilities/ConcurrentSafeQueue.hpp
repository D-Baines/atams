/**
  ******************************************************************************
  * @file    ConcurrentSafeQueue.hpp
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
#include "main.h"

/*************************************************************************************/
/* TEMPLATE DEFINITION                                                               */
/*************************************************************************************/

template<typename ElementType_t>
class ConcurrentSafeQueue
{
  public:

  /* Public Typedefs ----------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    ERROR_NONE  = 0U,
    ERROR_FULL  = 1U,
    ERROR_EMPTY = 2U,
  } Error_t;

  typedef struct
  {
    ElementType_t data;
    Error_t       status;
  } Return_t;


  /* Public Function Definitions ----------------------------------------------------*/

  ConcurrentSafeQueue(void (&lockFunction)(void), void (&unlockFunction)(void)) :
  acquireLock(lockFunction),
  releaseLock(unlockFunction)
  {
    _frontIndex         = 0U;
    _rearIndex          = 0U;
    _atomicElementCount = 0U;
  }

  ConcurrentSafeQueue(const ConcurrentSafeQueue &other)             = delete;
  ConcurrentSafeQueue & operator=(const ConcurrentSafeQueue &other) = delete;

  Return_t getFront(void)
  {
    Return_t errorReturn =
    {
      .data   = _elementArray[_frontIndex],
      .status = ERROR_EMPTY
    };

    if (_atomicElementCount == 0U)
    {
      return (errorReturn);
    }

    acquireLock();

    Return_t successReturn =
    {
      .data   = _elementArray[_frontIndex],
      .status = ERROR_NONE
    };

    releaseLock();

    return (successReturn);
  }

  Error_t pushRear(ElementType_t &element)
  {
    if(_atomicElementCount >= STATIC_QUEUE_SIZE)
    {
      return (ERROR_FULL);
    }

    acquireLock();

    _elementArray[_rearIndex] = element;
    _rearIndex++;
    _atomicElementCount++;

    if (_rearIndex >= STATIC_QUEUE_SIZE)
    {
      _rearIndex = 0U;
    }

    releaseLock();

    return (ERROR_NONE);
  }

  void popFront(void)
  {
    if (_atomicElementCount == 0)
    {
      return;
    }

    acquireLock();

    _frontIndex++;
    _atomicElementCount--;

    if (_frontIndex >= STATIC_QUEUE_SIZE)
    {
      _frontIndex = 0U;
    }

    releaseLock();
  }


  private:

  /* Private Constants ---------------------------------------------------------------*/

  static const uint8_t STATIC_QUEUE_SIZE = 10U;


  /* Private Variables ---------------------------------------------------------------*/

  ElementType_t     _elementArray[STATIC_QUEUE_SIZE];
  volatile uint8_t  _frontIndex;
  volatile uint8_t  _rearIndex;
  volatile uint8_t  _atomicElementCount;


  /* Lock Function References --------------------------------------------------------*/
  void (&acquireLock)(void);
  void (&releaseLock)(void);

};


/**
  * @}End of File
  */


