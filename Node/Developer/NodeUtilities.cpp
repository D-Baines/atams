/**
  ******************************************************************************
  * @file    NodeUtilities.cpp
  *
  * @author  D. Baines
  *
  * @brief   Utility function definitions for the Atams Node library.
  *
  * @details Implements utility functions shared between the Atams Node Comms Core
  *          and Application Core, including Node Memory Map validation.
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "NodeUtilities.hpp"

#include "../SharedPlatform.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../../Shared/Utilities/AtamsUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NodeUtilities {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

/* Warning - relies on validateSharedMemoryMap() passing first (non-null list, noOfVars in range) */
static bool validateUniversalBlockNVM(const NodeSharedMemoryMap_t &memoryMap)
{
  bool universalBlockNVMValid {true};

  for (uint16_t varIndex {0U}; varIndex < BlockUniversal::NUMBER_OF_VARS; varIndex++)
  {
    const NodeVarInfo_t &mapVarInfo       {memoryMap.varInfoList[varIndex]};
    const NodeVarInfo_t &universalVarInfo {BlockUniversal::varInfoList[varIndex]};

    if ((mapVarInfo.NVMStorage != universalVarInfo.NVMStorage) ||
        (mapVarInfo.nvmHash    != universalVarInfo.nvmHash   ) )
    {
      universalBlockNVMValid = false;
      break;
    }
  }

  return (universalBlockNVMValid);
}

/* Warning - relies on validateSharedMemoryMap() passing first (non-null list, noOfVars in range) */
static bool validateGenNvmChecksum(const MemoryMap_t &memoryMap)
{
  const NodeSharedMemoryMap_t &sharedMap {memoryMap.sharedMap};
  Atams::CRC32                &crc       {Atams::getAtamsCRC32()};

  crc.beginRollingCRC();

  for (uint16_t varID {BlockUniversal::NUMBER_OF_VARS}; varID < sharedMap.genInfo.noOfVars; varID++)
  {
    const NodeVarInfo_t &varInfo {sharedMap.varInfoList[varID]};
    uint8_t              nvmHashBytes[sizeof(varInfo.nvmHash)];

    Atams::uint32ToBuffer(varInfo.nvmHash, nvmHashBytes);

    crc.updateRollingCRC(varInfo.NVMStorage);

    for (const uint8_t nvmHashByte : nvmHashBytes)
    {
      crc.updateRollingCRC(nvmHashByte);
    }
  }

  return (memoryMap.genNvmChecksum == crc.getRollingCRC());
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief Validates a Node Memory Map before it is used to initialise a Node core.
 *
 * Runs the checks shared with the Hub against this Node's var storage length, then the
 * Node-only checks: the Universal Block's NVM fields and the generated NVM field checksum.
 *
 * @param memoryMap Reference to the Node Memory Map to validate.
 *
 * @retval @c ERROR_NONE                   Memory Map valid.
 * @retval @c ERROR_MEMORY_MAP             Memory Map invalid (structure, length, universal block, checksum, or NVM fields).
 * @retval @c ERROR_ATAMS_VERSION_MISMATCH Memory Map generated for a different Atams version.
 */
Atams::Error_t validateMemoryMap(const MemoryMap_t &memoryMap)
{
  Atams::Error_t error {Atams::validateSharedMemoryMap(memoryMap.sharedMap, Platform::NODE_NUMBER_OF_VARS)};

  if (error != Atams::ERROR_NONE)
  {
    return (error); /* Early Return - Node-only checks index varInfoList, so need the shared checks to pass */
  }

  if ((validateUniversalBlockNVM(memoryMap.sharedMap) == false) ||
      (validateGenNvmChecksum(memoryMap)              == false) )
  {
    error = Atams::ERROR_MEMORY_MAP;
  }

  return (error);
}


} } /* End Namespace - Atams::NodeUtilities */

/**
  * @}End of File
  */
