/**
  ******************************************************************************
  * @file    List.hpp
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
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

#pragma once

/***************************************************************************/
/* INCLUDES                                                                */
/***************************************************************************/

#include <stdint.h>
#include <stdio.h>

/***************************************************************************/
/* TYPEDEFS                                                                */
/***************************************************************************/


/***************************************************************************/
/* CLASS DEFINITIONS                                                       */
/***************************************************************************/

template <typename T>
class List
{

  public:

  /*-- PUBLIC TYPEDEFS ----------------*/

  typedef enum: uint8_t
  {
    ERROR_NONE            = 0U,
    ERROR_INDEX_OOR       = 1U,
    ERROR_FULL            = 2U,
    ERROR_ELEMENT_IN_LIST = 3U,
  } Error_t;


  typedef struct SearchReturn_t
  {
    bool     elementFound;
    uint16_t elementIndex;
  } SearchResult_t;


  typedef bool (*CustomCompareFn_t)(const T elementOne, const T elementTwo);


  typedef struct ListReturn_t
  {
    Error_t status;
    T       element;
  } Return_t;


  /*-- PUBLIC FUNCTION DEFINITIONS ----*/

  List(uint16_t maxElementCount)
  {
    _maxElementCount = maxElementCount;
  }


  List::SearchResult_t findElement(T elementToFind)
  {
    SearchResult_t searchResult;
  
    for (uint16_t listIndex = 0U; listIndex < _elementCount; listIndex++)
    {
      if (elementToFind == _elementList[listIndex]) 
      {
        searchResult.elementIndex = listIndex;
        searchResult.elementFound = true;
        return (searchResult);
      }
    }   
  
    searchResult.elementFound = false;
  
    return (searchResult);
  }
  

  List::Error_t appendElement(const T &newElement)
  {
    if ((_elementCount >= LIST_MAX_LENGTH) ||
        (_elementCount >= _maxElementCount ) )
    {
      return (List::ERROR_FULL);
    }

    if (findElement(newElement).elementFound == true)
    {
      return (List::ERROR_ELEMENT_IN_LIST);
    }  
  
    _elementList[_elementCount] = newElement;
    _elementCount++;
  
    return (List::ERROR_NONE);
  }
  
  
  void removeElement(const T elementToRemove)
  {
    SearchResult_t searchResult = findElement(elementToRemove);

    if (searchResult.elementFound == true)
    {
      removeElementAtIndex(searchResult.elementIndex);
    }
  }
  
  
  uint16_t getElementCount(void)
  {
    return (_elementCount);
  }
  
  
  /* Not expected to return a valid pointer if return status is != LIST_ERROR_NONE */
  List::Return_t getElementAtIndex(uint16_t elementIndex)
  { 
    Return_t elementReturn;

    if (elementIndex >= _elementCount)
    {
      elementReturn.status = List::ERROR_INDEX_OOR;
      return (elementReturn);
    }
  
    elementReturn.status  = List::ERROR_NONE;
    elementReturn.element = _elementList[elementIndex];
  
    return (elementReturn);
  }


  private:

  /*-- PRIVATE CONSTANTS --------------*/

	static const uint16_t LIST_MAX_LENGTH = 1024U;

  /*-- PRIVATE VARIABLES --------------*/

  T _elementList[LIST_MAX_LENGTH];

  uint16_t _elementCount    = 0U;
  uint16_t _maxElementCount = 0U;

  /*-- PRIVATE FUNCTION DEFINITIONS ---*/

  void removeElementAtIndex(uint16_t elementIndex)
  {
    for (uint16_t index = elementIndex; index < (_elementCount - 1U); index++)
    {
      _elementList[index] = _elementList[index + 1U];
    }
  
    _elementCount--;
  }

};


/**
  * @}End of File
  */
