/**
  ******************************************************************************
  * @file    AppCore.cpp
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

#include "string.h"
#include <type_traits>
#include "AppCore.hpp"
#include "AppPlatform.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../../Shared/Utilities/AtamsUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static constexpr uint32_t CORE_STATUS_CHECK_PERIOD = 10U;

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

enum CoreInitStatus_t: uint8_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
};

using VarStorage_t = uint8_t[Atams::MAX_TYPE_SIZE];

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static const MemoryMap_t *s_memoryMapPtr;

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t s_validVarCount = 0U;

/* Core Init Synchronisation */
ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static CoreInitStatus_t s_coreInitComplete[Atams::NUMBER_OF_CORES] {CORE_INIT_IN_PROGRESS, CORE_INIT_IN_PROGRESS};

ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static Atams::VarStorage_t s_varStorage[Platform::NODE_NUMBER_OF_VARS];

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

template <typename T>
constexpr Atams::VarType_t getAtamsType(void)
{
    if      constexpr (std::is_same<T, uint8_t>::value)  return (Atams::TYPE_UINT8);
    else if constexpr (std::is_same<T, int8_t>::value)   return (Atams::TYPE_INT8);
    else if constexpr (std::is_same<T, uint16_t>::value) return (Atams::TYPE_UINT16);
    else if constexpr (std::is_same<T, int16_t>::value)  return (Atams::TYPE_INT16);
    else if constexpr (std::is_same<T, uint32_t>::value) return (Atams::TYPE_UINT32);
    else if constexpr (std::is_same<T, int32_t>::value)  return (Atams::TYPE_INT32);
    else if constexpr (std::is_same<T, float>::value)    return (Atams::TYPE_FLOAT);
    return (Atams::TYPE_NULL);
}

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

static void resetVars(void)
{
  Platform::acquireVarStorageLock();

  for (VarStorage_t &varStroage : s_varStorage)
  {
    memset(varStroage, 0U, sizeof(varStroage));
  }

  Platform::releaseVarStorageLock();
}

static void invalidateMemoryMap(void)
{
  s_validVarCount = 0U;
  s_memoryMapPtr  = nullptr;
  resetVars();
}

static void syncWithCommsCoreInit(void)
{
  uint32_t                  previousCoreCheckTime = 0U;
  volatile CoreInitStatus_t commsInitStatus       = CORE_INIT_IN_PROGRESS;

  Platform::acquireVarStorageLock();
  s_coreInitComplete[CORE_COMMS] = CORE_INIT_IN_PROGRESS;
  Platform::releaseVarStorageLock();

  while (commsInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime = Platform::getMillis();

    if (currentTime - previousCoreCheckTime >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireVarStorageLock();

      s_coreInitComplete[CORE_APP] = CORE_INIT_COMPLETE;
      commsInitStatus = s_coreInitComplete[CORE_COMMS];

      Platform::releaseVarStorageLock();

      previousCoreCheckTime = currentTime;
    }
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Atams::Error_t initAppCore(const MemoryMap_t &memoryMap)
{
  Atams::Error_t initStatus = validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (initStatus == Atams::ERROR_NONE)
  {
    s_memoryMapPtr  = &memoryMap;
    s_validVarCount =  memoryMap.noOfVars;

    syncWithCommsCoreInit();
  }
  else
  {
    invalidateMemoryMap();
  }

  return (initStatus);
}

template <typename T>
Atams::Error_t write(const uint16_t varID, const T writeValue)
{
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &varStorage = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  writeToVarStorage(writeValue, varStorage);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t write<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template Atams::Error_t write<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template Atams::Error_t write<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template Atams::Error_t write<int16_t >(const uint16_t varID, const int16_t  writeValue);
template Atams::Error_t write<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template Atams::Error_t write<int32_t >(const uint16_t varID, const int32_t  writeValue);
template Atams::Error_t write<float   >(const uint16_t varID, const float    writeValue);

template <typename T>
Atams::Error_t read(const uint16_t  varID, T &outputRef)
{
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = s_memoryMapPtr->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &var = s_varStorage[varID];

  Platform::acquireVarStorageLock();

  readFromVarStorage(outputRef, var);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t read<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template Atams::Error_t read<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template Atams::Error_t read<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template Atams::Error_t read<int16_t >(const uint16_t varID, int16_t  &outputRef);
template Atams::Error_t read<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template Atams::Error_t read<int32_t >(const uint16_t varID, int32_t  &outputRef);
template Atams::Error_t read<float   >(const uint16_t varID, float    &outputRef);

} /* End Namespace - Atams */

/**
  * @}End of File
  */
