/**
  ******************************************************************************
  * @file    AtamsUtilities.cpp
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

#include "../../Atams/Utilities/AtamsUtilities.hpp"

#include <string.h>

#include "../AtamsTypedefs.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams
{

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE OBJECTS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

const char* getErrorString(Error_t errorID)
{
  //return (AtamsErrorStrings[errorID]);

  return (nullptr);
}

void swapEndiannessRaw(uint8_t* buffer, uint8_t bufferLength)
{ 
  for (uint8_t i = 0U; i < bufferLength / 2U; ++i)
  {
    uint8_t temp                  = buffer[i];
    buffer[i]                     = buffer[bufferLength - i - 1U];
    buffer[bufferLength - i - 1U] = temp;
  }
}

uint32_t bufferToUint32(const uint8_t* buffer)
{
  return ((static_cast<uint32_t>(buffer[0U]) << THREE_BYTE_SHIFT ) |
          (static_cast<uint32_t>(buffer[1U]) << TWO_BYTE_SHIFT   ) |
          (static_cast<uint32_t>(buffer[2U]) << SINGLE_BYTE_SHIFT) |
          (static_cast<uint32_t>(buffer[3U])                     ) );
}

void uint32ToBuffer(const uint32_t value, uint8_t* buffer)
{
  buffer[0U] = static_cast<uint8_t>((value >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
  buffer[1U] = static_cast<uint8_t>((value >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  buffer[2U] = static_cast<uint8_t>((value >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  buffer[3U] = static_cast<uint8_t>((value                     ) & SINGLE_BYTE_MASK);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
