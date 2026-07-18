/**
  ******************************************************************************
  * @file    NodeTypedefs.hpp
  *
  * @author  D. Baines
  *
  * @brief   Internal type definitions and data structures for the Atams Node library.
  *
  * @details Defines types shared between the Atams Node Comms Core and Application Core.
  *          Includes MemoryMap_t which extends the shared Memory Map with Node-specific
  *          init function references, SharedData_t which holds the dual-core shared
  *          variable storage array and the watchdog fault flag, and the NVM envelope/
  *          per-variable entry types (NVMHeader_t, NVMFooter_t, NVMVarEntryHeader_t).
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
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../SharedPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint32_t CORE_STATUS_CHECK_PERIOD {10U};

/* Gates whether stored NVM data can be parsed at all - bump only when NVMHeader_t/NVMFooter_t's
 * own layout changes, or the var entry encoding changes (see NVMVarEntryHeader_t). Deliberately
 * independent of Atams::ATAMS_VERSION_MAJOR/MINOR (AtamsTypedefs.hpp), which gates Bus/Hub-Node
 * compatibility and changes for reasons (e.g. a BlockUniversal variable added) that don't affect
 * whether stored NVM bytes can still be parsed - migration already handles ordinary
 * variable-level differences, so bumping ATAMS_VERSION_MAJOR/MINOR alone should never invalidate
 * NVM data. */
constexpr uint8_t NVM_FORMAT_VERSION {1U};

constexpr uint32_t NVM_HEADER_IDENTIFIER_INVALID {0x00000000U};
constexpr uint32_t NVM_HEADER_IDENTIFIER_VALID   {0xD0D0CACAU};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef Atams::Error_t (&InitUniversalDataFn_t)(void);
typedef Atams::Error_t (&InitDefaultsFn_t)(void);

using NodeVarInfo_t = VarInfo_t;

enum CoreInitStatus_t: uint32_t
{
  CORE_INIT_IN_PROGRESS = 0U,
  CORE_INIT_COMPLETE    = 1U
};

/* In-memory convenience only - see NVM_HEADER_SIZE below. */
struct NVMHeader_t
{
  uint32_t identifier       {NVM_HEADER_IDENTIFIER_INVALID};
  uint32_t length           {0U};
  uint8_t  nvmFormatVersion {NVM_FORMAT_VERSION};
};

static_assert(std::is_standard_layout_v<NVMHeader_t>);
static_assert(std::is_trivially_copyable_v<NVMHeader_t>);

enum NVMHeaderFieldSize_t: uint8_t
{
  NVM_HEADER_FIELD_SIZE_IDENTIFIER     = sizeof(uint32_t),
  NVM_HEADER_FIELD_SIZE_LENGTH         = sizeof(uint32_t),
  NVM_HEADER_FIELD_SIZE_FORMAT_VERSION = sizeof(uint8_t),
};

enum NVMHeaderIndex_t: uint8_t
{
  NVM_HEADER_INDEX_IDENTIFIER     = 0U,
  NVM_HEADER_INDEX_LENGTH         = NVM_HEADER_INDEX_IDENTIFIER + NVM_HEADER_FIELD_SIZE_IDENTIFIER,
  NVM_HEADER_INDEX_FORMAT_VERSION = NVM_HEADER_INDEX_LENGTH     + NVM_HEADER_FIELD_SIZE_LENGTH,
};

constexpr uint32_t NVM_HEADER_SIZE {NVM_HEADER_FIELD_SIZE_IDENTIFIER +
                                     NVM_HEADER_FIELD_SIZE_LENGTH     +
                                     NVM_HEADER_FIELD_SIZE_FORMAT_VERSION};
static_assert(NVM_HEADER_SIZE == 9U, "NVM_HEADER_SIZE changed");

/* In-memory convenience only - see NVM_FOOTER_SIZE below. */
struct NVMFooter_t
{
  uint32_t identifier {NVM_HEADER_IDENTIFIER_INVALID};
  uint32_t checksum   {0U};
};

static_assert(std::is_standard_layout_v<NVMFooter_t>);
static_assert(std::is_trivially_copyable_v<NVMFooter_t>);

/* Fixed on-NVM field sizes/offsets for NVMFooter_t - see NVM_HEADER_SIZE above for why this is
 * not sizeof(NVMFooter_t). Never read/write NVMFooter_t via reinterpret_cast<uint8_t*>(&footer)/
 * sizeof(footer). */
enum NVMFooterFieldSize_t: uint8_t
{
  NVM_FOOTER_FIELD_SIZE_IDENTIFIER = sizeof(uint32_t),
  NVM_FOOTER_FIELD_SIZE_CHECKSUM   = sizeof(uint32_t),
};

enum NVMFooterIndex_t: uint8_t
{
  NVM_FOOTER_INDEX_IDENTIFIER = 0U,
  NVM_FOOTER_INDEX_CHECKSUM   = NVM_FOOTER_INDEX_IDENTIFIER + NVM_FOOTER_FIELD_SIZE_IDENTIFIER,
};

constexpr uint32_t NVM_FOOTER_SIZE {NVM_FOOTER_FIELD_SIZE_IDENTIFIER + NVM_FOOTER_FIELD_SIZE_CHECKSUM};
static_assert(NVM_FOOTER_SIZE == 8U, "NVM_FOOTER_SIZE changed");

struct NVMVarEntryHeader_t
{
  uint32_t  nvmHash {0U};
  VarType_t type    {Atams::TYPE_NULL};
};

static_assert(std::is_standard_layout_v<NVMVarEntryHeader_t>);
static_assert(std::is_trivially_copyable_v<NVMVarEntryHeader_t>);

enum NVMVarEntryFieldSize_t: uint8_t
{
  NVM_VAR_ENTRY_FIELD_SIZE_NVM_HASH = sizeof(uint32_t),
  NVM_VAR_ENTRY_FIELD_SIZE_TYPE     = sizeof(uint8_t),
};

enum NVMVarEntryIndex_t: uint8_t
{
  NVM_VAR_ENTRY_INDEX_NVM_HASH = 0U,
  NVM_VAR_ENTRY_INDEX_TYPE     = NVM_VAR_ENTRY_INDEX_NVM_HASH + NVM_VAR_ENTRY_FIELD_SIZE_NVM_HASH,
};

/* Autogen mirrors NVM_HEADER_SIZE/NVM_FOOTER_SIZE/NVM_VAR_ENTRY_HEADER_SIZE
 * (Autogen/Modules/FileAutogen.py) to compute each generated map's REQUIRED_NVM_SIZE - safe
 * since all three are sums of fixed-width primitive sizes, never struct sizeof()s, so they can't
 * vary with a platform/compiler's padding or packing settings. */
constexpr uint32_t NVM_VAR_ENTRY_HEADER_SIZE {NVM_VAR_ENTRY_FIELD_SIZE_NVM_HASH + NVM_VAR_ENTRY_FIELD_SIZE_TYPE};
static_assert(NVM_VAR_ENTRY_HEADER_SIZE == 5U, "NVM_VAR_ENTRY_HEADER_SIZE changed");

using NodeSharedMemoryMap_t = SharedMemoryMap_t<NodeVarInfo_t>;

struct MemoryMap_t
{
  const NodeSharedMemoryMap_t sharedMap;
  InitUniversalDataFn_t       genInfoInitFn;
  InitDefaultsFn_t            userDefaultsInitFn;

  MemoryMap_t(const NodeSharedMemoryMap_t &sharedMemoryMapInput,
              InitUniversalDataFn_t        genInfoInitFnInput,
              InitDefaultsFn_t             initUserDefaultsFnInput) :
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

/* Node-only counterpart to validateMemoryMap() - checks NVMStorage and nvmHash, fields that
 * only matter to a Node's own NVM storage path and are never present on a Hub's slimmer
 * VarInfo variant, so they can't live in the generic Memory Map validation function. */
inline Atams::Error_t validateNodeMemoryMap(const SharedMemoryMap_t<NodeVarInfo_t> &memoryMap)
{
  if (memoryMap.genInfo.noOfVars < BlockUniversal::NUMBER_OF_VARS)
  {
    return (Atams::ERROR_MEMORY_MAP); /* Early Return */
  }

  for (uint16_t varID {0U}; varID < memoryMap.genInfo.noOfVars; varID++)
  {
    if (memoryMap.varInfoList[varID].NVMStorage > Atams::ATAMS_TRUE)
    {
      return (Atams::ERROR_MEMORY_MAP); /* Early Return */
    }
  }

  for (uint16_t varIndex {0U}; varIndex < BlockUniversal::NUMBER_OF_VARS; varIndex++)
  {
    const NodeVarInfo_t &mapVarInfo       {memoryMap.varInfoList[varIndex]};
    const NodeVarInfo_t &universalVarInfo {BlockUniversal::varInfoList[varIndex]};

    if ((mapVarInfo.NVMStorage != universalVarInfo.NVMStorage) ||
        (mapVarInfo.nvmHash    != universalVarInfo.nvmHash   ) )
    {
      return (Atams::ERROR_MEMORY_MAP); /* Early Return */
    }
  }

  return (Atams::ERROR_NONE);
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */


