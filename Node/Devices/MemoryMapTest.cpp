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

#include "MemoryMapTest.hpp"

/*************************************************************************************/
/* MEMORY MAP NAMESPACE                                                              */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* GLOBAL CONSTANTS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap(NUMBER_OF_DATA_BLOCKS,                          
                            initDefaults,
                            initLimits,
                            {BlockExample1::blockDescriptor,
                             BlockExample2::blockDescriptor });

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(void)
{
  Error_t initStatus = BlockUniversal::initDefaults();

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_ATAMS_VERSION_NUMBER,
                                                   AUTOGEN_ATAMS_VERSION_NUMBER);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_DAY,
                                                   AUTOGEN_MAP_GEN_DAY);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_MONTH,
                                                   AUTOGEN_MAP_GEN_MONTH);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_YEAR,
                                                   AUTOGEN_MAP_GEN_YEAR);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_HOUR,
                                                   AUTOGEN_MAP_GEN_HOUR);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_MINUTE,
                                                   AUTOGEN_MAP_GEN_MINUTE);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_SECOND,
                                                   AUTOGEN_MAP_GEN_SECOND);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_NUMBER_OF_BLOCKS,
                                                   AUTOGEN_MAP_NUMBER_OF_BLOCKS);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_CHECKSUM,
                                                   AUTOGEN_MAP_CHECKSUM);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE1,
                                                   BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_DATE,
                                                   BlockExample1::DEFAULT_MEMORY_MAP_GEN_DATE);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE1,
                                                   BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_TIME,
                                                   BlockExample1::DEFAULT_MEMORY_MAP_GEN_TIME);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE1,
                                                   BlockExample1::MEMBER_ID_MEMORY_MAP_CHECKSUM,
                                                   BlockExample1::DEFAULT_MEMORY_MAP_CHECKSUM);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE2,
                                                   BlockExample2::MEMBER_ID_MEMORY_MAP_GEN_DATE,
                                                   BlockExample2::DEFAULT_MEMORY_MAP_GEN_DATE);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE2,
                                                   BlockExample2::MEMBER_ID_MEMORY_MAP_GEN_TIME,
                                                   BlockExample2::DEFAULT_MEMORY_MAP_GEN_TIME);

  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_EXAMPLE2,
                                                   BlockExample2::MEMBER_ID_MEMORY_MAP_CHECKSUM,
                                                   BlockExample2::DEFAULT_MEMORY_MAP_CHECKSUM);

  return (initStatus); 
}

Error_t initLimits(void)
{
  Error_t initStatus = BlockUniversal::initLimits();

  if (initStatus == ERROR_NONE) initStatus = assertLimits(BLOCK_ID_EXAMPLE1,
                                                          BlockExample1::MEMBER_ID_NODE_ID,
                                                          BlockExample1::MAX_LIMIT_NODE_ID,
                                                          BlockExample1::MIN_LIMIT_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = assertLimits(BLOCK_ID_EXAMPLE2,
                                                          BlockExample2::MEMBER_ID_NODE_ID,
                                                          BlockExample2::MAX_LIMIT_NODE_ID,
                                                          BlockExample2::MIN_LIMIT_NODE_ID);

  return (initStatus); 
}

} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
