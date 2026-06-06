/**
  ******************************************************************************
  * @file    CommsCore.hpp
  *
  * @author  D. Baines
  *
  * @brief   Public interfaces for the Atams Node Communications Core on single
  *          and dual-core platforms.
  *
  * @details The Atams Node Comms Core is responsible for updating the Node's
  *          communications interface - allowing external Hub devices to access
  *          the Node's variable storage, and trigger Node processes.
  *          On single-core platforms, user application code can use the Comms
  *          Core setter and getter functions to access the Node variable storage.
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

#include "../../Shared/AtamsTypedefs.hpp"
#include "../Developer/NodeTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError);

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError);

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

void updateCommsPolling(void);

void updateCommsBlocking(void);

Atams::Error_t restoreAll(void);

Atams::Error_t restoreUser(void);

Atams::Error_t storeAll(void);

bool getWatchdogFault(void);


} /* End Namespace - Atams */


/**
  * @}End of File
  */


