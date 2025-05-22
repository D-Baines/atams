/**
  ******************************************************************************
  * @file    MemoryMapTest.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing public function definitions
  *          for an Atams Memory Map with name: Test.
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

#include "MapTest.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* STATIC CONSTANTS                                                                  */
/*************************************************************************************/

static const GenInfo_t s_genInfo = 
{
  /* .atamsVersionMajor = */ 0U,
  /* .atamsVersionMinor = */ 1U,
  /* .genDay            = */ 19U,
  /* .genMonth          = */ 5U,
  /* .genYear           = */ 2025U,
  /* .genHour           = */ 19U,
  /* .genMinute         = */ 43U,
  /* .genSecond         = */ 43U,
  /* .genChecksum       = */ 2433352831U,
  /* .noOfVars          = */ 56U
};

static const VarInfo_t s_varInfoList[Platform::NODE_NUMBER_OF_VARS] =
{
  /*------------------------ Universal Var Info -------------------------*/
  
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_DAY],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_MONTH],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_YEAR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_HOUR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_MINUTE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_GEN_SECOND],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_CHECKSUM],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_MAP_NUMBER_OF_VARS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_CONFIGURATION_PASSKEY],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_CONFIGURATION_STATUS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_FIRST_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_LAST_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_PREVIOUS_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_BITRATE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_WATCHDOG_PERIOD],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_STORE_ALL],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_RESTORE_USER_BLOCKS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_RESTORE_ALL],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_RESET_NODE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_STORAGE_STATUS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_STORAGE_PROCESS_COMPLETE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_WATCHDOG_FAULT_ACTIVE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_WATCHDOG_RESET],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_CRC_ERROR_COUNT],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ID_COBS_ERROR_COUNT],

  /*--------------------------- User Var Info ---------------------------*/

  /* [BlockExample1::VAR_ID_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample1::VAR_ID_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockExample2::VAR_ID_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  }
};

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

const Node::MemoryMap_t memoryMap =
{
  /* .sharedMemoryMap = */
  {
    /* noOfVars         = */ MapTest::s_genInfo.noOfVars,
    /* genInfo          = */ MapTest::s_genInfo,
    /* varInfoList      = */ MapTest::s_varInfoList
  }
};


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
