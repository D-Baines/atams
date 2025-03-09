/**
  ******************************************************************************
  * @file    BlockUniversal.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Example1.
  *          The Data Block is part of an Atams Memory Map with name: Test.
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

#include "BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/

static Error_t initDefaults(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_NODE_ID,
                                                         BlockUniversal::DEFAULT_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_FIRST_NODE_ID,
                                                         BlockUniversal::DEFAULT_FIRST_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_LAST_NODE_ID,
                                                         BlockUniversal::DEFAULT_LAST_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID,
                                                         BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT,
                                                         BlockUniversal::DEFAULT_WATCHDOG_TIMEOUT);

  return (initStatus); 
}

/*************************************************************************************/
/* BLOCK DESCRIPTOR                                                                  */
/*************************************************************************************/

extern const DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockUniversal::NUMBER_OF_UNIVERSAL_DATA_MEMBERS,
  /* .initDefaults    = */ initDefaults,
  /* .dataMemberInfo  = */
  {
    /* [BlockUniversal::MEMBER_ID_ATAMS_VERSION_NUMBER] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_UNIVERSAL_UNLOCK] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_DAY] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_MONTH] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_YEAR] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_HOUR] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_MINUTE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_GEN_SECOND] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_MAP_CHECKSUM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_FIRST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_LAST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_COUNTER] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_FAULT_ACTIVE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_WATCHDOG_RESET] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_CRC_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_COBS_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockUniversal::MEMBER_ID_STORE_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_RESTORE_FACTORY_NVM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockUniversal::MEMBER_ID_NVM_STATUS] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
  }
};


} } /* End Namespace - Atams::DataBlockUniversal */

/**
  * @}End of File
  */
