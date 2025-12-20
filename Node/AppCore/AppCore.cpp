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

#include "AppCore.hpp"

#include "string.h"

#include "AppPlatform.hpp"
#include "Atams/Node/Developer/NodeUtilities.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../../Shared/Utilities/AtamsUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


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

/**
 * @brief   Initialises the Node Application Core for dual-core platforms.
 *
 * @details Validates and stores a reference to the provided Memory Map, then synchronises with the Communications Core. 
 *          This function must be called from the Application Core at system startup before accessing any Node variables.
 *
 * @param memoryMap Reference to a @c MemoryMap_t structure defining the Node's variable layout and properties.
 *   
 * @retval @c ERROR_NONE       Initialisation successful.
 * @retval @c ERROR_MEMORY_MAP Memory Map validation failed (invalid structure, length, universal block, or checksum).
 *
 * @note    This function does not initialise variable values or NVM data; it only validates the Memory Map and synchronises
 *          with the Communications Core. Variable and NVM initialisation are handled by the Communications Core.
 *          Once this function returns, see value of BlockUniversal::VAR_STORAGE_STATUS for NVM initialisation status.
 *          
 * @warning This function will block until the Communications Core has successfully completed its initialisation.
 */
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

/**
 * @brief Sets the value of a variable in the Node's variable storage.
 *
 * Writes a value to the specified variable in the Node's variable storage in a type-safe manner.
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 *
 * @param varID      The ID of the variable to set.
 * @param writeValue The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE     Variable successfully set.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t setVar(const uint16_t varID, const T writeValue)
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

template Atams::Error_t setVar<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template Atams::Error_t setVar<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template Atams::Error_t setVar<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template Atams::Error_t setVar<int16_t >(const uint16_t varID, const int16_t  writeValue);
template Atams::Error_t setVar<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template Atams::Error_t setVar<int32_t >(const uint16_t varID, const int32_t  writeValue);
template Atams::Error_t setVar<float   >(const uint16_t varID, const float    writeValue);

/**
 * @brief Retrieves the value of a variable from the Node's variable storage.
 *
 * Copies the value of the specified variable from the Node's variable storage to the provided 
 * output reference in a type-safe manner.
 *
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T        The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 *
 * @param varID     The ID of the variable to retrieve.
 * @param outputRef Reference to a variable where the retrieved value will be stored.
 *
 * @retval @c ERROR_NONE     Variable successfully retrieved.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t getVar(const uint16_t varID, T &outputRef)
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

template Atams::Error_t getVar<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template Atams::Error_t getVar<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template Atams::Error_t getVar<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template Atams::Error_t getVar<int16_t >(const uint16_t varID, int16_t  &outputRef);
template Atams::Error_t getVar<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template Atams::Error_t getVar<int32_t >(const uint16_t varID, int32_t  &outputRef);
template Atams::Error_t getVar<float   >(const uint16_t varID, float    &outputRef);

} /* End Namespace - Atams */

/**
  * @}End of File
  */
