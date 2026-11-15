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

#include "SharedPlatform.hpp"

#ifdef COMMS_CORE_DETECT
#include "Node/CommsCore/CommsCore.hpp"
#else
#include "Node/AppCore/AppCore.hpp"
#endif

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

static constexpr uint32_t GEN_NVM_CHECKSUM {1017591324U};

static_assert(0U == Atams::ATAMS_VERSION_MAJOR, "Autogen VERSION_MAJOR out of sync with Atams::ATAMS_VERSION_MAJOR");
static_assert(1U == Atams::ATAMS_VERSION_MINOR, "Autogen VERSION_MINOR out of sync with Atams::ATAMS_VERSION_MINOR");

static constexpr NodeVarInfo_t s_varInfoList[]
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
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x4C28480FU,
  },
  /* [BlockTypes::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x813569A2U,
  },
  /* [BlockTypes::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x94D6B2ABU,
  },
  /* [BlockTypes::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x897856FFU,
  },
  /* [BlockTypes::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7409155AU,
  },
  /* [BlockTypes::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x69A7F10EU,
  },
  /* [BlockTypes::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC677C485U,
  },
  /* [BlockTypes::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE3D92D63U,
  },
  /* [BlockTypes::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x06DBB9BFU,
  },
  /* [BlockTypes::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB8FC1F7CU,
  },
  /* [BlockTypes::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x26893393U,
  },
  /* [BlockTypes::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x5823B88DU,
  },
  /* [BlockTypes::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC6569462U,
  },
  /* [BlockTypes::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x6986A1E9U,
  },
  /* [BlockTypesNvm::VAR_READ_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x5DA5AE25U,
  },
  /* [BlockTypesNvm::VAR_READ_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x57F85A43U,
  },
  /* [BlockTypesNvm::VAR_READ_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xDF9899ABU,
  },
  /* [BlockTypesNvm::VAR_READ_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x98F5B0D5U,
  },
  /* [BlockTypesNvm::VAR_READ_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x3F473E5AU,
  },
  /* [BlockTypesNvm::VAR_READ_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x782A1724U,
  },
  /* [BlockTypesNvm::VAR_READ_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xD7FA22AFU,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xA8970663U,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x17565F95U,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xB8B75157U,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x6DC71893U,
  },
  /* [BlockTypesNvm::VAR_WRITE_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x5868F6A6U,
  },
  /* [BlockTypesNvm::VAR_WRITE_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x8D18BF62U,
  },
  /* [BlockTypesNvm::VAR_WRITE_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x22C88AE9U,
  },
  /* [BlockConfigWidgets::VAR_CHECKBOX] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x074D57B6U,
  },
  /* [BlockConfigWidgets::VAR_BOOLSTATE_RO] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xB479FE05U,
  },
  /* [BlockConfigWidgets::VAR_BOOLSTATE_RW] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x2EFE61A5U,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xEF793199U,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x9E152FF3U,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x207CF82DU,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x2A292F69U,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xC0A35FDCU,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xCAF68898U,
  },
  /* [BlockConfigWidgets::VAR_DROPDOWN_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x6526BD13U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x9F575941U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x2DC58C72U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT16] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x79066F21U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT16] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x5A0747B1U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_UINT32] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x99D9C8D0U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_INT32] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xBAD8E040U,
  },
  /* [BlockConfigWidgets::VAR_READOPTION_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x1508D5CBU,
  },
  /* [BlockControls::VAR_VALUE_UNITS] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3321F3E4U,
  },
  /* [BlockControls::VAR_BAR_INT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x9A9F55F4U,
  },
  /* [BlockControls::VAR_BAR_FLOAT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x08D6A66BU,
  },
  /* [BlockControls::VAR_BAR_MIN_ONLY] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3E5B5246U,
  },
  /* [BlockControls::VAR_BAR_NO_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xD4286656U,
  },
  /* [BlockControls::VAR_STATUS_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x55FF1858U,
  },
  /* [BlockControls::VAR_STATUS_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x222D90CBU,
  },
  /* [BlockControls::VAR_FLAG] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8E2D03D8U,
  },
  /* [BlockControls::VAR_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8B6A52D2U,
  },
  /* [BlockControls::VAR_INPUT_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC3F7820DU,
  },
  /* [BlockControls::VAR_DROPDOWN_MODE] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x36937812U,
  },
  /* [BlockControls::VAR_SLIDER_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7BF59D50U,
  },
  /* [BlockControls::VAR_SLIDER_FLOAT] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xF1AA11DAU,
  },
  /* [BlockControls::VAR_SLIDER_NO_LIMITS] = */
  {
    /* .type           = */ Atams::TYPE_INT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xC6BA414BU,
  },
  /* [BlockControls::VAR_COMMAND_OPTIONS] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7D11D21EU,
  },
  /* [BlockControls::VAR_COMMAND_PLAIN] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA7A5BBEAU,
  },
  /* [BlockControls::VAR_DEVICE_TICK] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x65270007U,
  },
  /* [BlockControls::VAR_TEST_SAWTOOTH] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x19FAEC1DU,
  },
  /* [BlockControls::VAR_TEST_SINE] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x22B5C1ECU,
  },
  /* [BlockControls::VAR_TEST_COSINE] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_READ,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x58A3528CU,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_CONSTANT] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xA7D2C943U,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xAF5FA64DU,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x9B4CF335U,
  },
  /* [BlockNvmTestSameName::VAR_NVM_VAR_CONSTANT] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x8B58DDA7U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x9E068C18U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x8D567FECU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7F3DFCEFU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xABF79804U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x599C1B07U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x4ACCE8F3U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB8A76BF0U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE6B457D4U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x14DFD4D7U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xF88783CCU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x0AEC00CFU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x19BCF33BU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xEBD77038U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x3F1D14D3U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xCD7697D0U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xDE266424U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x2C4DE727U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x725EDB03U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x80355800U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT8_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xCC602B55U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_1] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xCF222EF0U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xDC72DD04U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_3] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x2E195E07U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_4] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xFAD33AECU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_5] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x08B8B9EFU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_6] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x1BE84A1BU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_7] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE983C918U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_8] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB790F53CU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_9] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x45FB763FU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_10] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x913F57BBU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_11] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x6354D4B8U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_12] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x7004274CU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_13] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x826FA44FU,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_14] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x56A5C0A4U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_15] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA4CE43A7U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_16] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xB79EB053U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_17] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x45F53350U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_18] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0x1BE60F74U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_19] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xE98D8C77U,
  },
  /* [BlockPacketFill::VAR_WRITE_UINT32_20] = */
  {
    /* .type           = */ Atams::TYPE_UINT32,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_FALSE,
    /* .nvmHash        = */ 0xA5D8FF22U,
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

  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_UINT8, BlockTypes::DEFAULT_READ_UINT8);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_INT8, BlockTypes::DEFAULT_READ_INT8);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_UINT16, BlockTypes::DEFAULT_READ_UINT16);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_INT16, BlockTypes::DEFAULT_READ_INT16);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_UINT32, BlockTypes::DEFAULT_READ_UINT32);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_INT32, BlockTypes::DEFAULT_READ_INT32);
  if (!error) error = Atams::setVar(BlockTypes::VAR_READ_FLOAT, BlockTypes::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_UINT8, BlockTypes::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_INT8, BlockTypes::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_UINT16, BlockTypes::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_INT16, BlockTypes::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_UINT32, BlockTypes::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_INT32, BlockTypes::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::setVar(BlockTypes::VAR_WRITE_FLOAT, BlockTypes::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_UINT8, BlockTypesNvm::DEFAULT_READ_UINT8);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_INT8, BlockTypesNvm::DEFAULT_READ_INT8);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_UINT16, BlockTypesNvm::DEFAULT_READ_UINT16);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_INT16, BlockTypesNvm::DEFAULT_READ_INT16);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_UINT32, BlockTypesNvm::DEFAULT_READ_UINT32);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_INT32, BlockTypesNvm::DEFAULT_READ_INT32);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_READ_FLOAT, BlockTypesNvm::DEFAULT_READ_FLOAT);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_UINT8, BlockTypesNvm::DEFAULT_WRITE_UINT8);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_INT8, BlockTypesNvm::DEFAULT_WRITE_INT8);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_UINT16, BlockTypesNvm::DEFAULT_WRITE_UINT16);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_INT16, BlockTypesNvm::DEFAULT_WRITE_INT16);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_UINT32, BlockTypesNvm::DEFAULT_WRITE_UINT32);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_INT32, BlockTypesNvm::DEFAULT_WRITE_INT32);
  if (!error) error = Atams::setVar(BlockTypesNvm::VAR_WRITE_FLOAT, BlockTypesNvm::DEFAULT_WRITE_FLOAT);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_CHECKBOX, BlockConfigWidgets::DEFAULT_CHECKBOX);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_BOOLSTATE_RO, BlockConfigWidgets::DEFAULT_BOOLSTATE_RO);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_BOOLSTATE_RW, BlockConfigWidgets::DEFAULT_BOOLSTATE_RW);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_UINT8, BlockConfigWidgets::DEFAULT_DROPDOWN_UINT8);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_INT8, BlockConfigWidgets::DEFAULT_DROPDOWN_INT8);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_UINT16, BlockConfigWidgets::DEFAULT_DROPDOWN_UINT16);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_INT16, BlockConfigWidgets::DEFAULT_DROPDOWN_INT16);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_UINT32, BlockConfigWidgets::DEFAULT_DROPDOWN_UINT32);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_INT32, BlockConfigWidgets::DEFAULT_DROPDOWN_INT32);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_DROPDOWN_FLOAT, BlockConfigWidgets::DEFAULT_DROPDOWN_FLOAT);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_UINT8, BlockConfigWidgets::DEFAULT_READOPTION_UINT8);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_INT8, BlockConfigWidgets::DEFAULT_READOPTION_INT8);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_UINT16, BlockConfigWidgets::DEFAULT_READOPTION_UINT16);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_INT16, BlockConfigWidgets::DEFAULT_READOPTION_INT16);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_UINT32, BlockConfigWidgets::DEFAULT_READOPTION_UINT32);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_INT32, BlockConfigWidgets::DEFAULT_READOPTION_INT32);
  if (!error) error = Atams::setVar(BlockConfigWidgets::VAR_READOPTION_FLOAT, BlockConfigWidgets::DEFAULT_READOPTION_FLOAT);
  if (!error) error = Atams::setVar(BlockControls::VAR_VALUE_UNITS, BlockControls::DEFAULT_VALUE_UNITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_BAR_INT_LIMITS, BlockControls::DEFAULT_BAR_INT_LIMITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_BAR_FLOAT_LIMITS, BlockControls::DEFAULT_BAR_FLOAT_LIMITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_BAR_MIN_ONLY, BlockControls::DEFAULT_BAR_MIN_ONLY);
  if (!error) error = Atams::setVar(BlockControls::VAR_BAR_NO_LIMITS, BlockControls::DEFAULT_BAR_NO_LIMITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_STATUS_UINT8, BlockControls::DEFAULT_STATUS_UINT8);
  if (!error) error = Atams::setVar(BlockControls::VAR_STATUS_INT8, BlockControls::DEFAULT_STATUS_INT8);
  if (!error) error = Atams::setVar(BlockControls::VAR_FLAG, BlockControls::DEFAULT_FLAG);
  if (!error) error = Atams::setVar(BlockControls::VAR_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING, BlockControls::DEFAULT_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING);
  if (!error) error = Atams::setVar(BlockControls::VAR_INPUT_LIMITS, BlockControls::DEFAULT_INPUT_LIMITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_DROPDOWN_MODE, BlockControls::DEFAULT_DROPDOWN_MODE);
  if (!error) error = Atams::setVar(BlockControls::VAR_SLIDER_UINT8, BlockControls::DEFAULT_SLIDER_UINT8);
  if (!error) error = Atams::setVar(BlockControls::VAR_SLIDER_FLOAT, BlockControls::DEFAULT_SLIDER_FLOAT);
  if (!error) error = Atams::setVar(BlockControls::VAR_SLIDER_NO_LIMITS, BlockControls::DEFAULT_SLIDER_NO_LIMITS);
  if (!error) error = Atams::setVar(BlockControls::VAR_COMMAND_OPTIONS, BlockControls::DEFAULT_COMMAND_OPTIONS);
  if (!error) error = Atams::setVar(BlockControls::VAR_COMMAND_PLAIN, BlockControls::DEFAULT_COMMAND_PLAIN);
  if (!error) error = Atams::setVar(BlockControls::VAR_DEVICE_TICK, BlockControls::DEFAULT_DEVICE_TICK);
  if (!error) error = Atams::setVar(BlockControls::VAR_TEST_SAWTOOTH, BlockControls::DEFAULT_TEST_SAWTOOTH);
  if (!error) error = Atams::setVar(BlockControls::VAR_TEST_SINE, BlockControls::DEFAULT_TEST_SINE);
  if (!error) error = Atams::setVar(BlockControls::VAR_TEST_COSINE, BlockControls::DEFAULT_TEST_COSINE);
  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_CONSTANT, BlockNvmTest::DEFAULT_NVM_VAR_CONSTANT);
  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_1, BlockNvmTest::DEFAULT_NVM_VAR_NAME_CHANGE_1);
  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE, BlockNvmTest::DEFAULT_NVM_VAR_TYPE_CHANGE);
  if (!error) error = Atams::setVar(BlockNvmTestSameName::VAR_NVM_VAR_CONSTANT, BlockNvmTestSameName::DEFAULT_NVM_VAR_CONSTANT);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_1, BlockPacketFill::DEFAULT_WRITE_UINT8_1);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_2, BlockPacketFill::DEFAULT_WRITE_UINT8_2);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_3, BlockPacketFill::DEFAULT_WRITE_UINT8_3);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_4, BlockPacketFill::DEFAULT_WRITE_UINT8_4);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_5, BlockPacketFill::DEFAULT_WRITE_UINT8_5);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_6, BlockPacketFill::DEFAULT_WRITE_UINT8_6);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_7, BlockPacketFill::DEFAULT_WRITE_UINT8_7);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_8, BlockPacketFill::DEFAULT_WRITE_UINT8_8);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_9, BlockPacketFill::DEFAULT_WRITE_UINT8_9);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_10, BlockPacketFill::DEFAULT_WRITE_UINT8_10);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_11, BlockPacketFill::DEFAULT_WRITE_UINT8_11);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_12, BlockPacketFill::DEFAULT_WRITE_UINT8_12);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_13, BlockPacketFill::DEFAULT_WRITE_UINT8_13);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_14, BlockPacketFill::DEFAULT_WRITE_UINT8_14);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_15, BlockPacketFill::DEFAULT_WRITE_UINT8_15);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_16, BlockPacketFill::DEFAULT_WRITE_UINT8_16);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_17, BlockPacketFill::DEFAULT_WRITE_UINT8_17);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_18, BlockPacketFill::DEFAULT_WRITE_UINT8_18);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_19, BlockPacketFill::DEFAULT_WRITE_UINT8_19);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT8_20, BlockPacketFill::DEFAULT_WRITE_UINT8_20);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_1, BlockPacketFill::DEFAULT_WRITE_UINT32_1);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_2, BlockPacketFill::DEFAULT_WRITE_UINT32_2);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_3, BlockPacketFill::DEFAULT_WRITE_UINT32_3);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_4, BlockPacketFill::DEFAULT_WRITE_UINT32_4);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_5, BlockPacketFill::DEFAULT_WRITE_UINT32_5);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_6, BlockPacketFill::DEFAULT_WRITE_UINT32_6);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_7, BlockPacketFill::DEFAULT_WRITE_UINT32_7);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_8, BlockPacketFill::DEFAULT_WRITE_UINT32_8);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_9, BlockPacketFill::DEFAULT_WRITE_UINT32_9);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_10, BlockPacketFill::DEFAULT_WRITE_UINT32_10);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_11, BlockPacketFill::DEFAULT_WRITE_UINT32_11);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_12, BlockPacketFill::DEFAULT_WRITE_UINT32_12);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_13, BlockPacketFill::DEFAULT_WRITE_UINT32_13);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_14, BlockPacketFill::DEFAULT_WRITE_UINT32_14);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_15, BlockPacketFill::DEFAULT_WRITE_UINT32_15);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_16, BlockPacketFill::DEFAULT_WRITE_UINT32_16);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_17, BlockPacketFill::DEFAULT_WRITE_UINT32_17);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_18, BlockPacketFill::DEFAULT_WRITE_UINT32_18);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_19, BlockPacketFill::DEFAULT_WRITE_UINT32_19);
  if (!error) error = Atams::setVar(BlockPacketFill::VAR_WRITE_UINT32_20, BlockPacketFill::DEFAULT_WRITE_UINT32_20);  

  return (error == Atams::ERROR_NONE ?
          Atams::ERROR_NONE          :
          Atams::ERROR_MEMORY_MAP    );
}

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

constexpr MemoryMap_t nodeMemoryMap
{
  /* .sharedMap */
  {
    /* .genInfo     = */ MapTest::s_genInfo,
    /* .varInfoList = */ MapTest::s_varInfoList
  },
  /* genNvmChecksum   = */ MapTest::GEN_NVM_CHECKSUM,
  /* initGenInfo      = */ MapTest::initGenInfo,
  /* initUserDefaults = */ MapTest::initUserDefaults,
};

static_assert(nodeMemoryMap.sharedMap.varInfoListLength == nodeMemoryMap.sharedMap.genInfo.noOfVars,
              "Var info list length does not match genInfo.noOfVars - regenerate this map");
static_assert(nodeMemoryMap.sharedMap.varInfoListLength <= NodePlatform::NODE_NUMBER_OF_VARS,
              "Memory Map exceeds NodePlatform::NODE_NUMBER_OF_VARS var storage");

} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
