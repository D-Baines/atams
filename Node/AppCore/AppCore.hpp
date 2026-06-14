/**
  ******************************************************************************
  * @file    AppCore.hpp
  *
  * @author  D. Baines
  *
  * @brief   Public interfaces for the Atams Node Application Core on dual-core platforms.
  *
  * @details Declares the App Core initialisation function and typed variable accessor
  *          functions for use on the application processor core of a dual-core Atams
  *          Node platform. The App Core shares variable storage with the Communications
  *          Core through a platform-defined shared memory region. On single-core
  *          platforms, use the Comms Core interfaces instead.
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

#include "../../Shared/AtamsTypedefs.hpp"
#include "../Developer/NodeTypedefs.hpp"

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
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Atams::Error_t initAppCore(const MemoryMap_t &memoryMap);

template <typename T>
Atams::Error_t setVar(const uint16_t varID, const T writeValue) = delete;

template <> Atams::Error_t setVar<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template <> Atams::Error_t setVar<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template <> Atams::Error_t setVar<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template <> Atams::Error_t setVar<int16_t >(const uint16_t varID, const int16_t  writeValue);
template <> Atams::Error_t setVar<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template <> Atams::Error_t setVar<int32_t >(const uint16_t varID, const int32_t  writeValue);
template <> Atams::Error_t setVar<float   >(const uint16_t varID, const float    writeValue);

template <typename T>
Atams::Error_t getVar(const uint16_t varID, T &outputRef) = delete;

template <> Atams::Error_t getVar<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template <> Atams::Error_t getVar<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template <> Atams::Error_t getVar<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template <> Atams::Error_t getVar<int16_t >(const uint16_t varID, int16_t  &outputRef);
template <> Atams::Error_t getVar<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template <> Atams::Error_t getVar<int32_t >(const uint16_t varID, int32_t  &outputRef);
template <> Atams::Error_t getVar<float   >(const uint16_t varID, float    &outputRef);

bool getWatchdogFault(void);


} /* End Namespace - Atams */


/**
  * @}End of File
  */


