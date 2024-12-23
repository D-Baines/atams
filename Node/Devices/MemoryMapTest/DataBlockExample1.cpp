/**
  ******************************************************************************
  * @file    DataBlockExample1.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing function definitions for an 
  *          Atams Data Block with name: Example1.
  *          The Data Block is part of an Atams Memory Map with name: Test.
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

#include "DataBlockExample1.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample1 {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_DATE,
                                                         BlockExample1::DEFAULT_MEMORY_MAP_GEN_DATE);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_MEMORY_MAP_GEN_TIME,
                                                         BlockExample1::DEFAULT_MEMORY_MAP_GEN_TIME);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_MEMORY_MAP_CHECKSUM,
                                                         BlockExample1::DEFAULT_MEMORY_MAP_CHECKSUM);

  return (initStatus); 
}

Error_t initLimits(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  if (initStatus == ERROR_NONE) initStatus = block.assertLimits(BlockExample1::MEMBER_ID_NODE_ID,
                                                                BlockExample1::MAX_LIMIT_NODE_ID,
                                                                BlockExample1::MIN_LIMIT_NODE_ID);

  return (initStatus); 
}


} } } /* End Namespace - Atams::MapTest::BlockExample1 */

/**
  * @}End of File
  */
