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
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/
                                        
Atams::Error_t initUniversalInfo(void)
{
  Atams::Error_t initStatus = ERROR_NONE;

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_ATAMS_VERSION_NUMBER,
                                                                 AUTOGEN_ATAMS_VERSION_NUMBER);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_DAY,
                                                                 AUTOGEN_MAP_GEN_DAY);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_MONTH,
                                                                 AUTOGEN_MAP_GEN_MONTH);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_YEAR,
                                                                 AUTOGEN_MAP_GEN_YEAR);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_HOUR,
                                                                 AUTOGEN_MAP_GEN_HOUR);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_MINUTE,
                                                                 AUTOGEN_MAP_GEN_MINUTE);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_GEN_SECOND,
                                                                 AUTOGEN_MAP_GEN_SECOND);

  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,
                                                                 BlockUniversal::VAR_ID_MAP_CHECKSUM,
                                                                 AUTOGEN_MAP_CHECKSUM);
  
  return (initStatus); 
}

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static const DataBlock::BlockDescriptor_t* blockDescriptors[Platform::NODE_NUMBER_OF_DATA_BLOCKS] = 
{
  &BlockUniversal::blockDescriptor,
  &BlockExample1::blockDescriptor,
  &BlockExample2::blockDescriptor
};

extern const MemoryMap_t memoryMap(static_cast<uint8_t>(NUMBER_OF_DATA_BLOCKS),
                                   initUniversalInfo,
                                   blockDescriptors);


} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
