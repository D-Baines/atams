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

#include "MapTestNew.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* STATIC CONSTANTS                                                                  */
/*************************************************************************************/

static const GenInfo_t genInfo = 
{
  /* .atamsVersionMajor = */ 0U,
  /* .atamsVersionMinor = */ 1U,
  /* .genDay            = */ 21U,
  /* .genMonth          = */ 4U,
  /* .genYear           = */ 2025U,
  /* .genHour           = */ 11U,
  /* .genMinute         = */ 3U,
  /* .genSecond         = */ 46U,
  /* .genChecksum       = */ 2433352831U
};

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initGenInfo(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  //if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
  //                                 BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR,
  //                                 genInfo.atamsVersionMajor);

  return (error);
}

static Atams::Error_t initUserDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  //if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_UINT8,
  //                                BlockExample2::DEFAULT_WRITE_UINT8);

  return (error);
}

static Atams::Error_t initAllDefaults(void)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  //if (!error) error = Atams::write(BlockExample2::VAR_ID_WRITE_UINT8,
  //                                 BlockExample2::DEFAULT_WRITE_UINT8);

  if (!error) error = initUserDefaults();

  return (error); 
}


/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

static const VarInfo_t varInfoList[Platform::NODE_NUMBER_OF_VARS] =
{
  /* [BlockExample2::VAR_ID_WRITE_UINT8] = */
  {
    /* .type           = */ Atams::TYPE_UINT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ false,
  },
  /* [BlockExample2::VAR_ID_WRITE_INT8] = */
  {
    /* .type           = */ Atams::TYPE_INT8,
    /* .externalAccess = */ Atams::ACCESS_WRITE,
    /* .NVMStorage     = */ false,
  },
};

const MemoryMap_t memoryMap =
{
  /* noOfVars         = */ static_cast<uint8_t>(MapTest::NUMBER_OF_NODE_VARS),
  /* genInfo          = */ MapTest::genInfo,
  /* initGenInfo      = */ MapTest::initGenInfo,
  /* initAllDefaults  = */ MapTest::initAllDefaults,
  /* initUserDefaults = */ MapTest::initUserDefaults,
  /* varInfoList      = */ MapTest::varInfoList
};


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
