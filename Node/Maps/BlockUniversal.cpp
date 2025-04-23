/**
  ******************************************************************************
  * @file    BlockUniversal.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing function definitions for an 
  *          Atams Data Block with name: Universal.
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
#include "../Node.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

const VarInfo_t varInfoList[BlockUniversal::NUMBER_OF_UNIVERSAL_VARS] =
{
  /* [BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_DAY] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_MONTH] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_YEAR] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_HOUR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_MINUTE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_GEN_SECOND] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_MAP_CHECKSUM] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_CONFIGURATION_STATUS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_FIRST_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_LAST_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_PREVIOUS_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_BITRATE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_WATCHDOG_PERIOD] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
  },
  /* [BlockUniversal::VAR_ID_STORE_ALL] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_RESTORE_ALL] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_RESET_NODE] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_STORAGE_STATUS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_WATCHDOG_RESET] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_CRC_ERROR_COUNT] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockUniversal::VAR_ID_COBS_ERROR_COUNT] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Atams::Error_t initDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockUniversal::VAR_ID_NODE_ID,          BlockUniversal::DEFAULT_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_FIRST_NODE_ID,    BlockUniversal::DEFAULT_FIRST_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_LAST_NODE_ID,     BlockUniversal::DEFAULT_LAST_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_PREVIOUS_NODE_ID, BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_BITRATE,          BlockUniversal::DEFAULT_BITRATE);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_WATCHDOG_PERIOD,  BlockUniversal::DEFAULT_WATCHDOG_PERIOD);

  return (error);
}


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
