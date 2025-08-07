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
  /* .genDay            = */ 7U,
  /* .genMonth          = */ 8U,
  /* .genYear           = */ 2025U,
  /* .genHour           = */ 19U,
  /* .genMinute         = */ 40U,
  /* .genSecond         = */ 33U,
  /* .genChecksum       = */ 495011647U,
  /* .noOfVars          = */ 96U
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
  },
  /* [BlockTest3::VAR_WRITE_UINT8_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
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
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_2, BlockTest3::DEFAULT_WRITE_UINT8_2);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_3, BlockTest3::DEFAULT_WRITE_UINT8_3);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_4, BlockTest3::DEFAULT_WRITE_UINT8_4);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_5, BlockTest3::DEFAULT_WRITE_UINT8_5);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_6, BlockTest3::DEFAULT_WRITE_UINT8_6);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_7, BlockTest3::DEFAULT_WRITE_UINT8_7);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_8, BlockTest3::DEFAULT_WRITE_UINT8_8);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_9, BlockTest3::DEFAULT_WRITE_UINT8_9);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_10, BlockTest3::DEFAULT_WRITE_UINT8_10);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_11, BlockTest3::DEFAULT_WRITE_UINT8_11);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_12, BlockTest3::DEFAULT_WRITE_UINT8_12);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_13, BlockTest3::DEFAULT_WRITE_UINT8_13);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_14, BlockTest3::DEFAULT_WRITE_UINT8_14);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_15, BlockTest3::DEFAULT_WRITE_UINT8_15);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_16, BlockTest3::DEFAULT_WRITE_UINT8_16);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_17, BlockTest3::DEFAULT_WRITE_UINT8_17);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_18, BlockTest3::DEFAULT_WRITE_UINT8_18);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_19, BlockTest3::DEFAULT_WRITE_UINT8_19);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT8_20, BlockTest3::DEFAULT_WRITE_UINT8_20);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_1, BlockTest3::DEFAULT_WRITE_UINT32_1);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_2, BlockTest3::DEFAULT_WRITE_UINT32_2);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_3, BlockTest3::DEFAULT_WRITE_UINT32_3);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_4, BlockTest3::DEFAULT_WRITE_UINT32_4);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_5, BlockTest3::DEFAULT_WRITE_UINT32_5);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_6, BlockTest3::DEFAULT_WRITE_UINT32_6);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_7, BlockTest3::DEFAULT_WRITE_UINT32_7);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_8, BlockTest3::DEFAULT_WRITE_UINT32_8);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_9, BlockTest3::DEFAULT_WRITE_UINT32_9);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_10, BlockTest3::DEFAULT_WRITE_UINT32_10);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_11, BlockTest3::DEFAULT_WRITE_UINT32_11);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_12, BlockTest3::DEFAULT_WRITE_UINT32_12);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_13, BlockTest3::DEFAULT_WRITE_UINT32_13);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_14, BlockTest3::DEFAULT_WRITE_UINT32_14);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_15, BlockTest3::DEFAULT_WRITE_UINT32_15);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_16, BlockTest3::DEFAULT_WRITE_UINT32_16);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_17, BlockTest3::DEFAULT_WRITE_UINT32_17);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_18, BlockTest3::DEFAULT_WRITE_UINT32_18);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_19, BlockTest3::DEFAULT_WRITE_UINT32_19);
  if (!error) error = Atams::write(BlockTest3::VAR_WRITE_UINT32_20, BlockTest3::DEFAULT_WRITE_UINT32_20);  

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
