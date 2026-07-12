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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

const VarInfo_t varInfoList[BlockUniversal::NUMBER_OF_VARS] =
{
  /* [BlockUniversal::VAR_ATAMS_VERSION_MAJOR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x99ABFB4EU,
  },
  /* [BlockUniversal::VAR_ATAMS_VERSION_MINOR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xF1313CB2U,
  },
  /* [BlockUniversal::VAR_ATAMS_VERSION_PATCH] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3ADAB87BU,
  },
  /* [BlockUniversal::VAR_MAP_GEN_DAY] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7BCDD1FCU,
  },
  /* [BlockUniversal::VAR_MAP_GEN_MONTH] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC902FDCDU,
  },
  /* [BlockUniversal::VAR_MAP_GEN_YEAR] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xD97D4BB2U,
  },
  /* [BlockUniversal::VAR_MAP_GEN_HOUR] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xEB2DACA2U,
  },
  /* [BlockUniversal::VAR_MAP_GEN_MINUTE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x83E69268U,
  },
  /* [BlockUniversal::VAR_MAP_GEN_SECOND] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x4FF7F0E7U,
  },
  /* [BlockUniversal::VAR_MAP_CHECKSUM] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x4D06A895U,
  },
  /* [BlockUniversal::VAR_MAP_NUMBER_OF_VARS] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x574D5AF0U,
  },
  /* [BlockUniversal::VAR_MAX_BUS_PACKET_SIZE] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xD989C9C9U,
  },
  /* [BlockUniversal::VAR_CONFIGURATION_PASSKEY] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x46C872ABU,
  },
  /* [BlockUniversal::VAR_CONFIGURATION_STATUS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x408F80C1U,
  },
  /* [BlockUniversal::VAR_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x1120767CU,
  },
  /* [BlockUniversal::VAR_FIRST_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xF0424734U,
  },
  /* [BlockUniversal::VAR_LAST_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x0490BEBDU,
  },
  /* [BlockUniversal::VAR_PREVIOUS_NODE_ID] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xD472C4E8U,
  },
  /* [BlockUniversal::VAR_BITRATE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x78024CA7U,
  },
  /* [BlockUniversal::VAR_WATCHDOG_PERIOD] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xD668D913U,
  },
  /* [BlockUniversal::VAR_STORE_ALL] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8C1A8E65U,
  },
  /* [BlockUniversal::VAR_RESTORE_USER_BLOCKS] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE20EF819U,
  },
  /* [BlockUniversal::VAR_RESTORE_ALL] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x988FFAB1U,
  },
  /* [BlockUniversal::VAR_RESET_NODE] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE983BFDFU,
  },
  /* [BlockUniversal::VAR_STORAGE_STATUS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xEBE9BAA6U,
  },
  /* [BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xDE6C2CF1U,
  },
  /* [BlockUniversal::VAR_WATCHDOG_FAULT_ACTIVE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x011D6E16U,
  },
  /* [BlockUniversal::VAR_WATCHDOG_RESET] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x70F28FF4U,
  },
  /* [BlockUniversal::VAR_CRC_ERROR_COUNT] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x28B7C37AU,
  },
  /* [BlockUniversal::VAR_COBS_ERROR_COUNT] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x589F311DU,
  },
};


} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
