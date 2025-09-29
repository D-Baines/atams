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
#include "../../Shared/AtamsTypedefs.hpp"

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

typedef Atams::Error_t (&InitUniversalDataFn_t)(void);
typedef Atams::Error_t (&InitDefaultsFn_t)(void);

struct MemoryMap_t :
public SharedMemoryMap_t
{
  const InitUniversalDataFn_t initGenInfo;
  const InitDefaultsFn_t      initUserDefaults;

  MemoryMap_t(const SharedMemoryMap_t    &initSharedMemoryMap,
              const InitUniversalDataFn_t initGenInfoFn,
              const InitDefaultsFn_t      initUserDefaultsFn) :
  SharedMemoryMap_t(initSharedMemoryMap),
  initGenInfo(initGenInfoFn),
  initUserDefaults(initUserDefaultsFn){};

  /* Default Constructor */
  MemoryMap_t(void) = delete;

  /* Default Destructor */
  ~MemoryMap_t(void){};

  /* Copy Constructor */
  MemoryMap_t(const MemoryMap_t &other) = delete;

  /* Copy Assignment Operator */
  MemoryMap_t & operator=(const MemoryMap_t &other) = delete;

  /* Move Constructor */
  MemoryMap_t(MemoryMap_t &&other) = delete;

  /* Move Assignment Operator */
  MemoryMap_t & operator=(MemoryMap_t &&other) = delete;
};

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/


} /* End Namespace - Atams */


/**
  * @}End of File
  */


