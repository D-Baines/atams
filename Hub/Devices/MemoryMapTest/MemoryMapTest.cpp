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
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest {

/*************************************************************************************/
/* PRIVATE FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/

static Error_t initUniversalInfo(Node &nodeToInit);

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static const Node::MemoryMap_t memoryMap(NUMBER_OF_DATA_BLOCKS,    
                                         initUniversalInfo,          
                                         {BlockUniversal::blockDescriptor,
                                          BlockExample1::blockDescriptor,
                                          BlockExample2::blockDescriptor });

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/
                                        
const Node::MemoryMap_t &getMapReference(void)
{
  return (memoryMap);
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static Error_t initUniversalInfo(Node &nodeToInit)
{
  Error_t initStatus = ERROR_NONE;

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_ATAMS_VERSION_NUMBER,
                                                   AUTOGEN_ATAMS_VERSION_NUMBER);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_DAY,
                                                   AUTOGEN_MAP_GEN_DAY);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_MONTH,
                                                   AUTOGEN_MAP_GEN_MONTH);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_YEAR,
                                                   AUTOGEN_MAP_GEN_YEAR);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_HOUR,
                                                   AUTOGEN_MAP_GEN_HOUR);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_MINUTE,
                                                   AUTOGEN_MAP_GEN_MINUTE);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                   BlockUniversal::MEMBER_ID_MAP_GEN_SECOND,
                                                   AUTOGEN_MAP_GEN_SECOND);

  if (initStatus == ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                              BlockUniversal::MEMBER_ID_MAP_CHECKSUM,
                                                              AUTOGEN_MAP_CHECKSUM);

  return (initStatus); 
}

} } /* End Namespace - Atams::MapTest */

/**
  * @}End of File
  */
