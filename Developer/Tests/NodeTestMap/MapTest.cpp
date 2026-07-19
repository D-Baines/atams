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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "MapTest.hpp"

#include "../../../Node/SharedPlatform.hpp"

#ifdef COMMS_CORE_DETECT
#include "../../../Node/CommsCore/CommsCore.hpp"
#include "../../../Node/CommsCore/CommsPlatform.hpp"
#else
#include "../../../Node/AppCore/AppCore.hpp"
#include "../../../Node/AppCore/AppPlatform.hpp"
#endif

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* STATIC CONSTANTS                                                                  */
/*************************************************************************************/

static const GenInfo_t s_genInfo 
{
  /* .atamsVersionMajor = */ 0U,
  /* .atamsVersionMinor = */ 1U,
  /* .genDay            = */ 19U,
  /* .genMonth          = */ 7U,
  /* .genYear           = */ 2026U,
  /* .genHour           = */ 13U,
  /* .genMinute         = */ 30U,
  /* .genSecond         = */ 55U,
  /* .genChecksum       = */ 3815048630U,
  /* .noOfVars          = */ 98U
};

/* Autogen's own version stamp above must match the Atams library this map is being compiled
 * against - if these fail, regenerate this map against the current Atams library. */
static_assert(0U == Atams::ATAMS_VERSION_MAJOR, "Autogen VERSION_MAJOR out of sync with Atams::ATAMS_VERSION_MAJOR");
static_assert(1U == Atams::ATAMS_VERSION_MINOR, "Autogen VERSION_MINOR out of sync with Atams::ATAMS_VERSION_MINOR");

static const NodeVarInfo_t s_varInfoList[Platform::NODE_NUMBER_OF_VARS]
{
  /*------------------------ Universal Var Info -------------------------*/
  
  BlockUniversal::varInfoList[BlockUniversal::VAR_ATAMS_VERSION_MAJOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ATAMS_VERSION_MINOR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_ATAMS_VERSION_PATCH],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_DAY],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_MONTH],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_YEAR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_HOUR],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_MINUTE],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_GEN_SECOND],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_CHECKSUM],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAP_NUMBER_OF_VARS],
  BlockUniversal::varInfoList[BlockUniversal::VAR_MAX_BUS_PACKET_SIZE],
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
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x7AE1FD31U,
  },
  /* [BlockTest1::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB7A0D0B4U,
  },
  /* [BlockTest1::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x087B8D88U,
  },
  /* [BlockTest1::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xBFB1E3C1U,
  },
  /* [BlockTest1::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xE8A42A79U,
  },
  /* [BlockTest1::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x5F6E4430U,
  },
  /* [BlockTest1::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xF0BE71BBU,
  },
  /* [BlockTest1::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7F741240U,
  },
  /* [BlockTest1::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x30120C81U,
  },
  /* [BlockTest1::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8B8DCF69U,
  },
  /* [BlockTest1::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xBA240CB0U,
  },
  /* [BlockTest1::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x6B526898U,
  },
  /* [BlockTest1::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x5AFBAB41U,
  },
  /* [BlockTest1::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xF52B9ECAU,
  },
  /* [BlockTest2::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x29738161U,
  },
  /* [BlockTest2::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x0BDEBE06U,
  },
  /* [BlockTest2::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x590DC527U,
  },
  /* [BlockTest2::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xEC239F91U,
  },
  /* [BlockTest2::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB9D262D6U,
  },
  /* [BlockTest2::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x0CFC3860U,
  },
  /* [BlockTest2::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA32C0DEBU,
  },
  /* [BlockTest2::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x2E025AEFU,
  },
  /* [BlockTest2::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x638070D1U,
  },
  /* [BlockTest2::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x778430A3U,
  },
  /* [BlockTest2::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xEB52441FU,
  },
  /* [BlockTest2::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x975B9752U,
  },
  /* [BlockTest2::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x0B8DE3EEU,
  },
  /* [BlockTest2::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xA45DD665U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xD52D74F2U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC67D8706U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x34160405U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE0DC60EEU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x12B7E3EDU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x01E71019U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xF38C931AU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xAD9FAF3EU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x5FF42C3DU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x701E2816U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8275AB15U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x912558E1U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x634EDBE2U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB784BF09U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x45EF3C0AU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x56BFCFFEU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA4D44CFDU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xFAC770D9U,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x08ACF3DAU,
  },
  /* [BlockTest3::VAR_WRITE_UINT8_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x44F9808FU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x47BB852AU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x54EB76DEU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA680F5DDU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x724A9136U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x80211235U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x9371E1C1U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x611A62C2U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3F095EE6U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xCD62DDE5U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x29860783U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xDBED8480U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC8BD7774U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3AD6F477U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xEE1C909CU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x1C77139FU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x0F27E06BU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xFD4C6368U,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA35F5F4CU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x5134DC4FU,
  },
  /* [BlockTest3::VAR_WRITE_UINT32_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x1D61AF1AU,
  }
};

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initGenInfo(void)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (!error) error = Atams::setVar(BlockUniversal::VAR_ATAMS_VERSION_MAJOR, s_genInfo.atamsVersionMajor);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_ATAMS_VERSION_MINOR, s_genInfo.atamsVersionMinor);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_DAY, s_genInfo.genDay);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_MONTH, s_genInfo.genMonth);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_YEAR, s_genInfo.genYear);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_HOUR, s_genInfo.genHour);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_MINUTE, s_genInfo.genMinute);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_GEN_SECOND, s_genInfo.genSecond);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_CHECKSUM, s_genInfo.genChecksum);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAP_NUMBER_OF_VARS, s_genInfo.noOfVars);  

  return (error == Atams::ERROR_NONE ?
          Atams::ERROR_NONE          :
          Atams::ERROR_MEMORY_MAP    );
}

Atams::Error_t initUserDefaults(void)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_UINT8, BlockTest1::DEFAULT_READ_UINT8);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_INT8, BlockTest1::DEFAULT_READ_INT8);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_UINT16, BlockTest1::DEFAULT_READ_UINT16);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_INT16, BlockTest1::DEFAULT_READ_INT16);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_UINT32, BlockTest1::DEFAULT_READ_UINT32);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_INT32, BlockTest1::DEFAULT_READ_INT32);
  if (!error) error = Atams::setVar(BlockTest1::VAR_READ_FLOAT, BlockTest1::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_UINT8, BlockTest1::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_INT8, BlockTest1::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_UINT16, BlockTest1::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_INT16, BlockTest1::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_UINT32, BlockTest1::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_INT32, BlockTest1::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::setVar(BlockTest1::VAR_WRITE_FLOAT, BlockTest1::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_UINT8, BlockTest2::DEFAULT_READ_UINT8);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_INT8, BlockTest2::DEFAULT_READ_INT8);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_UINT16, BlockTest2::DEFAULT_READ_UINT16);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_INT16, BlockTest2::DEFAULT_READ_INT16);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_UINT32, BlockTest2::DEFAULT_READ_UINT32);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_INT32, BlockTest2::DEFAULT_READ_INT32);
  if (!error) error = Atams::setVar(BlockTest2::VAR_READ_FLOAT, BlockTest2::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_UINT8, BlockTest2::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_INT8, BlockTest2::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_UINT16, BlockTest2::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_INT16, BlockTest2::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_UINT32, BlockTest2::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_INT32, BlockTest2::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::setVar(BlockTest2::VAR_WRITE_FLOAT, BlockTest2::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_1, BlockTest3::DEFAULT_WRITE_UINT8_1);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_2, BlockTest3::DEFAULT_WRITE_UINT8_2);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_3, BlockTest3::DEFAULT_WRITE_UINT8_3);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_4, BlockTest3::DEFAULT_WRITE_UINT8_4);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_5, BlockTest3::DEFAULT_WRITE_UINT8_5);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_6, BlockTest3::DEFAULT_WRITE_UINT8_6);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_7, BlockTest3::DEFAULT_WRITE_UINT8_7);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_8, BlockTest3::DEFAULT_WRITE_UINT8_8);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_9, BlockTest3::DEFAULT_WRITE_UINT8_9);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_10, BlockTest3::DEFAULT_WRITE_UINT8_10);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_11, BlockTest3::DEFAULT_WRITE_UINT8_11);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_12, BlockTest3::DEFAULT_WRITE_UINT8_12);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_13, BlockTest3::DEFAULT_WRITE_UINT8_13);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_14, BlockTest3::DEFAULT_WRITE_UINT8_14);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_15, BlockTest3::DEFAULT_WRITE_UINT8_15);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_16, BlockTest3::DEFAULT_WRITE_UINT8_16);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_17, BlockTest3::DEFAULT_WRITE_UINT8_17);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_18, BlockTest3::DEFAULT_WRITE_UINT8_18);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_19, BlockTest3::DEFAULT_WRITE_UINT8_19);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT8_20, BlockTest3::DEFAULT_WRITE_UINT8_20);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_1, BlockTest3::DEFAULT_WRITE_UINT32_1);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_2, BlockTest3::DEFAULT_WRITE_UINT32_2);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_3, BlockTest3::DEFAULT_WRITE_UINT32_3);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_4, BlockTest3::DEFAULT_WRITE_UINT32_4);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_5, BlockTest3::DEFAULT_WRITE_UINT32_5);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_6, BlockTest3::DEFAULT_WRITE_UINT32_6);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_7, BlockTest3::DEFAULT_WRITE_UINT32_7);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_8, BlockTest3::DEFAULT_WRITE_UINT32_8);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_9, BlockTest3::DEFAULT_WRITE_UINT32_9);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_10, BlockTest3::DEFAULT_WRITE_UINT32_10);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_11, BlockTest3::DEFAULT_WRITE_UINT32_11);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_12, BlockTest3::DEFAULT_WRITE_UINT32_12);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_13, BlockTest3::DEFAULT_WRITE_UINT32_13);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_14, BlockTest3::DEFAULT_WRITE_UINT32_14);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_15, BlockTest3::DEFAULT_WRITE_UINT32_15);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_16, BlockTest3::DEFAULT_WRITE_UINT32_16);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_17, BlockTest3::DEFAULT_WRITE_UINT32_17);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_18, BlockTest3::DEFAULT_WRITE_UINT32_18);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_19, BlockTest3::DEFAULT_WRITE_UINT32_19);
  if (!error) error = Atams::setVar(BlockTest3::VAR_WRITE_UINT32_20, BlockTest3::DEFAULT_WRITE_UINT32_20);  

  return (error == Atams::ERROR_NONE ?
          Atams::ERROR_NONE          :
          Atams::ERROR_MEMORY_MAP    );
}

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap 
{
  /* .sharedMap */
  {
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
