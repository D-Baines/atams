/**
  ******************************************************************************
  * @file    NodeTypedefs.hpp
  *
  * @author  D. Baines
  *
  * @brief   Internal type definitions and data structures for the Atams Node library.
  *
  * @details Defines types shared between the Atams Node Comms Core and Application Core.
  *          Includes VarStorage_t for per-variable byte storage, MemoryMap_t which
  *          extends the shared Memory Map with Node-specific init function references,
  *          and SharedData_t which holds the dual-core shared variable storage array
  *          and the watchdog fault flag.
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
#include "../SharedPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint32_t CORE_STATUS_CHECK_PERIOD {10U};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef Atams::Error_t (&InitUniversalDataFn_t)(void);
typedef Atams::Error_t (&InitDefaultsFn_t)(void);

enum CoreInitStatus_t: uint32_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
};

struct MemoryMap_t
{
  const SharedMemoryMap_t     sharedMap;
  InitUniversalDataFn_t genInfoInitFn;
  InitDefaultsFn_t      userDefaultsInitFn;

  MemoryMap_t(const SharedMemoryMap_t &sharedMemoryMapInput,
              InitUniversalDataFn_t    genInfoInitFnInput,
              InitDefaultsFn_t         initUserDefaultsFnInput) :
  sharedMap(sharedMemoryMapInput),
  genInfoInitFn(genInfoInitFnInput),
  userDefaultsInitFn(initUserDefaultsFnInput){};

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

struct SharedData_t
{
  CoreInitStatus_t      coreInitComplete[Atams::NUMBER_OF_CORES];
  Atams::VarStorage_t   varStorage[Platform::NODE_NUMBER_OF_VARS];
  std::atomic<uint32_t> watchdogFault;
};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


