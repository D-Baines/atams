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
  /* .genDay            = */ 24U,
  /* .genMonth          = */ 4U,
  /* .genYear           = */ 2025U,
  /* .genHour           = */ 10U,
  /* .genMinute         = */ 1U,
  /* .genSecond         = */ 42U,
  /* .genChecksum       = */ 2433352831U,
  /* .numberOfVars      = */ 56U
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
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initGenInfo(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR, s_genInfo.atamsVersionMajor);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR, s_genInfo.atamsVersionMinor);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_DAY, s_genInfo.genDay);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_MONTH, s_genInfo.genMonth);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_YEAR, s_genInfo.genYear);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_HOUR, s_genInfo.genHour);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_MINUTE, s_genInfo.genMinute);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_GEN_SECOND, s_genInfo.genSecond);
  if (!error) error = Atams::write(BlockUniversal::VAR_ID_MAP_CHECKSUM, s_genInfo.genChecksum);  
  return (error); 
}

Atams::Error_t initUserDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_UINT8, BlockExample1::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_INT8, BlockExample1::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_UINT8, BlockExample1::DEFAULT_READ_UINT8);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_INT8, BlockExample1::DEFAULT_READ_INT8);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_UINT16, BlockExample1::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_INT16, BlockExample1::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_UINT16, BlockExample1::DEFAULT_READ_UINT16);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_INT16, BlockExample1::DEFAULT_READ_INT16);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_UINT32, BlockExample1::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_INT32, BlockExample1::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_UINT32, BlockExample1::DEFAULT_READ_UINT32);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_INT32, BlockExample1::DEFAULT_READ_INT32);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_WRITE_FLOAT, BlockExample1::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::write(BlockExample1::VAR_ID_READ_FLOAT, BlockExample1::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_UINT8, BlockExample2::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_INT8, BlockExample2::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_UINT8, BlockExample2::DEFAULT_READ_UINT8);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_INT8, BlockExample2::DEFAULT_READ_INT8);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_UINT16, BlockExample2::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_INT16, BlockExample2::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_UINT16, BlockExample2::DEFAULT_READ_UINT16);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_INT16, BlockExample2::DEFAULT_READ_INT16);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_UINT32, BlockExample2::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_INT32, BlockExample2::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_UINT32, BlockExample2::DEFAULT_READ_UINT32);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_INT32, BlockExample2::DEFAULT_READ_INT32);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_FLOAT, BlockExample2::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::write(BlockExample2::VAR_ID_READ_FLOAT, BlockExample2::DEFAULT_READ_FLOAT);  
  return (error); 
}

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap =
{
  /* noOfVars         = */ s_genInfo.numberOfVars,
  /* genInfo          = */ MapTest::s_genInfo,
  /* initGenInfo      = */ MapTest::initGenInfo,
  /* initUserDefaults = */ MapTest::initUserDefaults,
  /* varInfoList      = */ MapTest::s_varInfoList
};

} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
