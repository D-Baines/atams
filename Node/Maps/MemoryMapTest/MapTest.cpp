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

static const MapGenInfo_t genInfo = 
{
  /* .atamsVersionNumber = */ 0.1F,
  /* .genDay             = */ 13U,
  /* .genMonth           = */ 3U,
  /* .genYear            = */ 2025U,
  /* .genHour            = */ 21U,
  /* .genMinute          = */ 2U,
  /* .genSecond          = */ 10U,
  /* .genChecksum        = */ 1061080793U
};

static const DataBlock::BlockDescriptor_t* blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS] = 
{
  &BlockUniversal::blockDescriptor,
  &BlockExample1::blockDescriptor,
  &BlockExample2::blockDescriptor
};

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initUniversalInfo(void)
{
  Atams::Error_t initStatus = Atams::ERROR_NONE;

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_ATAMS_VERSION_NUMBER,
                                                                 genInfo.atamsVersionNumber);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_DAY,
                                                                 genInfo.genDay);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_MONTH,
                                                                 genInfo.genMonth);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_YEAR,
                                                                 genInfo.genYear);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_HOUR,
                                                                 genInfo.genHour);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_MINUTE,
                                                                 genInfo.genMinute);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_SECOND,
                                                                 genInfo.genSecond);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_CHECKSUM,
                                                                 genInfo.genChecksum);
  
  return (initStatus); 
}

/*************************************************************************************/
/* EXTERN CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap(static_cast<uint8_t>(NUMBER_OF_DATA_BLOCKS),
                            genInfo,
                            initUniversalInfo,
                            blockDescriptors);


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
