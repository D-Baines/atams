/**
  ******************************************************************************
  * @file    MemoryMapNvmTest2.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing public function definitions
  *          for an Atams Memory Map with name: NvmTest2.
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

#include "MapNvmTest2.hpp"

#include "../../../SharedPlatform.hpp"

#ifdef COMMS_CORE_DETECT
#include "../../../CommsCore/CommsCore.hpp"
#include "../../../CommsCore/CommsPlatform.hpp"
#else
#include "../../../AppCore/AppCore.hpp"
#include "../../../AppCore/AppPlatform.hpp"
#endif

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapNvmTest2 {

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
  /* .genMinute         = */ 11U,
  /* .genSecond         = */ 2U,
  /* .genChecksum       = */ 2116266573U,
  /* .noOfVars          = */ 33U
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

  /* [BlockNvmTest::VAR_NVM_VAR_CONSTANT] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xA7D2C943U,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_2] = */
  {
    /* .type           = */ Atams::TYPE_UINT16,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0xBC0F55B9U,
  },
  /* [BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE] = */
  {
    /* .type           = */ Atams::TYPE_FLOAT,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ Atams::ATAMS_TRUE,
    /* .nvmHash        = */ 0x9B4CF335U,
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

  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_CONSTANT, BlockNvmTest::DEFAULT_NVM_VAR_CONSTANT);
  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_NAME_CHANGE_2, BlockNvmTest::DEFAULT_NVM_VAR_NAME_CHANGE_2);
  if (!error) error = Atams::setVar(BlockNvmTest::VAR_NVM_VAR_TYPE_CHANGE, BlockNvmTest::DEFAULT_NVM_VAR_TYPE_CHANGE);  

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
    /* .genInfo     = */ MapNvmTest2::s_genInfo,
    /* .varInfoList = */ MapNvmTest2::s_varInfoList
  },
  /* initGenInfo      = */ MapNvmTest2::initGenInfo,
  /* initUserDefaults = */ MapNvmTest2::initUserDefaults,
};

} } /* End Namespace - Atams::MapNvmTest2 */

/**
  * @}End of File
  */
