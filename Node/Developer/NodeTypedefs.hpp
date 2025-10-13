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

static constexpr uint32_t CORE_STATUS_CHECK_PERIOD = 10U;

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

using VarStorage_t = uint8_t[Atams::MAX_TYPE_SIZE];

typedef Atams::Error_t (&InitUniversalDataFn_t)(void);
typedef Atams::Error_t (&InitDefaultsFn_t)(void);

enum CoreInitStatus_t: uint8_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
};

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


} /* End Namespace - Atams */


/**
  * @}End of File
  */


