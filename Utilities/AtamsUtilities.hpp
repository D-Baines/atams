/**
  ******************************************************************************
  * @file    Atams.hpp
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "../AtamsTypedefs.hpp"
#include <stdint.h>


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

const char* getErrorString(Error_t errorID);

constexpr bool systemIsBigEndian(void)
{
  uint32_t asUINT32 = 0x0001;
  uint8_t *asPtr    = reinterpret_cast<uint8_t*>(&asUINT32);

  return (asPtr[3U] == 1U);
}

template <typename T>
void swapEndiannessType(T& value)
{
  const size_t size = sizeof(value);
  
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);

  for (size_t i = 0U; i < (size / 2U); ++i)
  {
    uint8_t temp = bytes[i];
    bytes[i] = bytes[size - i - 1U];
    bytes[size - i - 1U] = temp;
  }
}


void swapEndiannessRaw(uint8_t* buffer, uint8_t bufferLength);


} /* End Namespace - Atams */


/**
  * @}End of File
  */
