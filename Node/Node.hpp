/**
  ******************************************************************************
  * @file    Node.hpp
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
#include "../AtamsTypedefs.hpp"
#include "Platform.hpp"

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

struct MemoryMap_t
{
  const uint16_t              noOfVars = 0U;
  const GenInfo_t             genInfo;
  const InitUniversalDataFn_t initGenInfo;
  const InitDefaultsFn_t      initUserDefaults;
  const VarInfo_t            (&varInfoList)[Platform::NODE_NUMBER_OF_VARS];

  MemoryMap_t(const uint16_t              inputNoOfVars,
              const GenInfo_t             inputGenInfo,
              const InitUniversalDataFn_t initGenInfoFn,
              const InitDefaultsFn_t      initUserDefaultsFn,
              const VarInfo_t            (&initVarInfoList)[Platform::NODE_NUMBER_OF_VARS]) :
  noOfVars(inputNoOfVars),
  genInfo(inputGenInfo),
  initGenInfo(initGenInfoFn),
  initUserDefaults(initUserDefaultsFn),
  varInfoList(initVarInfoList){};

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

Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);

Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap);

Atams::Error_t initControlCore(const MemoryMap_t &memoryMap);

Atams::Error_t initNVM(void);

Atams::Error_t restoreAll(void);

Atams::Error_t restoreUser(void);

Atams::Error_t storeAll(void);

void updateCommsPolling(void);

void updateCommsBlocking(void);

template <typename T>
Atams::Error_t write(const uint16_t varID, const T writeData);

template <typename T>
Atams::Error_t read(const uint16_t varID, T &readData);

Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                const uint16_t  varID,
                                uint8_t * const bytesPtr,
                                const uint8_t   length);

DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

bool getWatchdogFault(void);

} /* End Namespace - Atams */


/**
  * @}End of File
  */


