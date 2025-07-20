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
  /* .genDay            = */ 18U,
  /* .genMonth          = */ 7U,
  /* .genYear           = */ 2025U,
  /* .genHour           = */ 17U,
  /* .genMinute         = */ 34U,
  /* .genSecond         = */ 59U,
  /* .genChecksum       = */ 240391391U,
  /* .noOfVars          = */ 56U
};

static const VarInfo_t s_varInfoList[Platform::NODE_NUMBER_OF_VARS] =
{
  /*------------------------ Universal Var Info -------------------------*/
  
  BlockUniversal::varInfoList[BlockUniversal::VAR_ATAMS_VERSION_MAJOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ATAMS_VERSION_MINOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_DAY],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_MONTH],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_YEAR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_HOUR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_MINUTE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_SECOND],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_CHECKSUM],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_NUMBER_OF_VARS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_CONFIGURATION_PASSKEY],
  BlockUniversal::varInfoList[BlockUniversal::VAR_CONFIGURATION_STATUS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_FIRST_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_LAST_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_PREVIOUS_NODE_ID],
  BlockUniversal::varInfoList[BlockUniversal::VAR_BITRATE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_WATCHDOG_PERIOD],
  BlockUniversal::varInfoList[BlockUniversal::VAR_STORE_ALL],
  BlockUniversal::varInfoList[BlockUniversal::VAR_RESTORE_USER_BLOCKS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_RESTORE_ALL],
  BlockUniversal::varInfoList[BlockUniversal::VAR_RESET_NODE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_STORAGE_STATUS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_STORAGE_PROCESS_COMPLETE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_WATCHDOG_FAULT_ACTIVE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_WATCHDOG_RESET],
  BlockUniversal::varInfoList[BlockUniversal::VAR_CRC_ERROR_COUNT],
  BlockUniversal::varInfoList[BlockUniversal::VAR_COBS_ERROR_COUNT],

  /*--------------------------- User Var Info ---------------------------*/

  /* [BlockTest1::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest1::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest2::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  }
};

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initGenInfo(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockUniversal::VAR_ATAMS_VERSION_MAJOR, s_genInfo.atamsVersionMajor);
  if (!error) error = Atams::write(BlockUniversal::VAR_ATAMS_VERSION_MINOR, s_genInfo.atamsVersionMinor);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_DAY, s_genInfo.genDay);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_MONTH, s_genInfo.genMonth);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_YEAR, s_genInfo.genYear);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_HOUR, s_genInfo.genHour);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_MINUTE, s_genInfo.genMinute);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_GEN_SECOND, s_genInfo.genSecond);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_CHECKSUM, s_genInfo.genChecksum);
  if (!error) error = Atams::write(BlockUniversal::VAR_MAP_NUMBER_OF_VARS, s_genInfo.noOfVars);  

  return (error); 
}

Atams::Error_t initUserDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BlockTest1::VAR_READ_INT8, BlockTest1::DEFAULT_READ_INT8);
  if (!error) error = Atams::write(BlockTest1::VAR_READ_UINT16, BlockTest1::DEFAULT_READ_UINT16);
  if (!error) error = Atams::write(BlockTest1::VAR_READ_INT16, BlockTest1::DEFAULT_READ_INT16);
  if (!error) error = Atams::write(BlockTest1::VAR_READ_UINT32, BlockTest1::DEFAULT_READ_UINT32);
  if (!error) error = Atams::write(BlockTest1::VAR_READ_INT32, BlockTest1::DEFAULT_READ_INT32);
  if (!error) error = Atams::write(BlockTest1::VAR_READ_FLOAT, BlockTest1::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_UINT8, BlockTest1::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_INT8, BlockTest1::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_UINT16, BlockTest1::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_INT16, BlockTest1::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_UINT32, BlockTest1::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_INT32, BlockTest1::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::write(BlockTest1::VAR_WRITE_FLOAT, BlockTest1::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_INT8, BlockTest2::DEFAULT_READ_INT8);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_UINT16, BlockTest2::DEFAULT_READ_UINT16);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_INT16, BlockTest2::DEFAULT_READ_INT16);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_UINT32, BlockTest2::DEFAULT_READ_UINT32);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_INT32, BlockTest2::DEFAULT_READ_INT32);
  if (!error) error = Atams::write(BlockTest2::VAR_READ_FLOAT, BlockTest2::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_UINT8, BlockTest2::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_INT8, BlockTest2::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_UINT16, BlockTest2::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_INT16, BlockTest2::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_UINT32, BlockTest2::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_INT32, BlockTest2::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::write(BlockTest2::VAR_WRITE_FLOAT, BlockTest2::DEFAULT_WRITE_FLOAT);  

  return (error); 
}

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap =
{
  /* SharedMemoryMap_t */
  {
    /* .noOfVars    = */ MapTest::s_genInfo.noOfVars,
    /* .genInfo     = */ MapTest::s_genInfo,
    /* .varInfoList = */ MapTest::s_varInfoList
  },
  /* initGenInfo      = */ MapTest::initGenInfo,
  /* initUserDefaults = */ MapTest::initUserDefaults,
};

} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
