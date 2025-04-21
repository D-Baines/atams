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

static const DataBlock::Descriptor_t* blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS] = 
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
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_ATAMS_VERSION_MAJOR,
                                   genInfo.atamsVersionMajor);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_ATAMS_VERSION_MINOR,
                                   genInfo.atamsVersionMinor);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_DAY,
                                   genInfo.genDay);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_MONTH,
                                   genInfo.genMonth);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_YEAR,
                                   genInfo.genYear);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_HOUR,
                                   genInfo.genHour);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_MINUTE,
                                   genInfo.genMinute);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_GEN_SECOND,
                                   genInfo.genSecond);

  if (!error) error = Atams::write(BLOCK_ID_UNIVERSAL,
                                   BlockUniversal::VAR_ID_MAP_CHECKSUM,
                                   genInfo.genChecksum);
  
  return (error); 
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
