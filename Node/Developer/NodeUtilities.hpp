/**
  ******************************************************************************
  * @file    NodeUtilities.hpp
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
#include <type_traits>
#include <string.h>

#include "../../Shared/AtamsTypedefs.hpp"
#include "NodeTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC TEMPLATE FUNCTION DEFINITIONS                                              */
/*************************************************************************************/

template<typename T>
inline void writeToVarStorage(const T inputVar, Atams::VarStorage_t &varStorage)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in writeToVarStorage");

  uint32_t tempVar;

  if constexpr (std::is_same<T, float>::value) memcpy(&tempVar, &inputVar, sizeof(tempVar));
  else                                         tempVar = static_cast<uint32_t>(inputVar);

  /* Little endian: LSB first */
  varStorage[0U] = static_cast<uint8_t>((tempVar                     ) & SINGLE_BYTE_MASK);
  varStorage[1U] = static_cast<uint8_t>((tempVar >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  varStorage[2U] = static_cast<uint8_t>((tempVar >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  varStorage[3U] = static_cast<uint8_t>((tempVar >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
}

template<typename T>
inline void readFromVarStorage(T &outputVar, const Atams::VarStorage_t &varStorage)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in readFromVarStorage");

  uint32_t tempVar;

  /* Little endian: LSB first */
  tempVar = ((static_cast<uint32_t>(varStorage[0U])                     ) |
             (static_cast<uint32_t>(varStorage[1U]) << SINGLE_BYTE_SHIFT) |
             (static_cast<uint32_t>(varStorage[2U]) << TWO_BYTE_SHIFT   ) |
             (static_cast<uint32_t>(varStorage[3U]) << THREE_BYTE_SHIFT ) );

  if constexpr (std::is_same<T, float>::value) memcpy(&outputVar, &tempVar, sizeof(outputVar));
  else                                         outputVar = static_cast<T>(tempVar);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


