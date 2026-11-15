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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* STATIC CONSTANTS                                                                  */
/*************************************************************************************/

static constexpr GenInfo_t s_genInfo
{
  /* .atamsVersionMajor = */ 0U,
  /* .atamsVersionMinor = */ 1U,
  /* .genDay            = */ 14U,
  /* .genMonth          = */ 11U,
  /* .genYear           = */ 2026U,
  /* .genHour           = */ 18U,
  /* .genMinute         = */ 49U,
  /* .genSecond         = */ 7U,
  /* .genChecksum       = */ 2728499568U,
  /* .noOfVars          = */ 139U
};

static_assert(0U == Atams::ATAMS_VERSION_MAJOR, "Autogen VERSION_MAJOR out of sync with Atams::ATAMS_VERSION_MAJOR");
static_assert(1U == Atams::ATAMS_VERSION_MINOR, "Autogen VERSION_MINOR out of sync with Atams::ATAMS_VERSION_MINOR");

static constexpr HubVarInfo_t s_varInfoList[]
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

  /* [BlockTypes::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypes::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypes::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockTypesNvm::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_CHECKBOX] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_BOOLSTATE_RO] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_BOOLSTATE_RW] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_VALUE_UNITS] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_BAR_INT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_BAR_FLOAT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_BAR_MIN_ONLY] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_BAR_NO_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_STATUS_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_STATUS_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_FLAG] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_INPUT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_DROPDOWN_MODE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_SLIDER_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_SLIDER_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_SLIDER_NO_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_COMMAND_OPTIONS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_COMMAND_PLAIN] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockControls::VAR_DEVICE_TICK] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_TEST_SAWTOOTH] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_TEST_SINE] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockControls::VAR_TEST_COSINE] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_CONSTANT] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockNvmTestSameName::VAR_NVM_VAR_CONSTANT] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
  }
};

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

constexpr Node::MemoryMap_t hubMemoryMap
{
  /* .sharedMap */
  {
    /* .genInfo     = */ MapTest::s_genInfo,
    /* .varInfoList = */ MapTest::s_varInfoList
  }
};

static_assert(hubMemoryMap.sharedMap.varInfoListLength == hubMemoryMap.sharedMap.genInfo.noOfVars,
              "Var info list length does not match genInfo.noOfVars - regenerate this map");
static_assert(hubMemoryMap.sharedMap.varInfoListLength <= HubPlatform::NODE_NUMBER_OF_VARS,
              "Memory Map exceeds HubPlatform::NODE_NUMBER_OF_VARS var storage");


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
