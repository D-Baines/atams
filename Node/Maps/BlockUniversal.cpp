/**
  ******************************************************************************
  * @file    BlockUniversal.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing function definitions for an 
  *          Atams Data Block with name: Universal.
  *          The Data Block is part of an Atams Memory Map with name: Uni.
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

static Atams::Error_t initDefaults(DataBlock &blockToInit)
{
  Atams::Error_t initStatus = Atams::ERROR_NONE;

  static_cast<void>(blockToInit);

  return (initStatus); 
}

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

const DataBlock::Descriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockUniversal::NUMBER_OF_VARS,
  /* .initDefaults    = */ initDefaults, 
  /* .dataMemberInfo  = */
  {
    /* [BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_DAY] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_MONTH] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_YEAR] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_HOUR] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_MINUTE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_GEN_SECOND] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_MAP_CHECKSUM] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_UNIVERSAL_UNLOCK] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_FIRST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_LAST_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_PREVIOUS_NODE_ID] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_STORE_ALL] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_RESTORE_ALL] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_NVM_STATUS] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_NVM_PROCESS_COMPLETE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_WATCHDOG_TIMEOUT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_WATCHDOG_RESET] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_CRC_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockUniversal::VAR_ID_COBS_ERROR_COUNT] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
  }
};


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
