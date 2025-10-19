/**
  ******************************************************************************
  * @file    CommsCore.hpp
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

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap);

template <typename T>
Atams::Error_t write(const uint16_t varID, const T writeValue);

template <typename T>
Atams::Error_t read(const uint16_t  varID, T &outputRef);

void updateCommsPolling(void);

void updateCommsBlocking(void);

Atams::Error_t restoreAll(void);

Atams::Error_t restoreUser(void);

Atams::Error_t storeAll(void);


} /* End Namespace - Atams */


/**
  * @}End of File
  */


