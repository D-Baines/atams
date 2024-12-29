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

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_UINT8,
                                                         BlockExample1::DEFAULT_WRITE_UINT8);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_INT8,
                                                         BlockExample1::DEFAULT_WRITE_INT8);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_UINT8,
                                                         BlockExample1::DEFAULT_READ_UINT8);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_INT8,
                                                         BlockExample1::DEFAULT_READ_INT8);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_UINT16,
                                                         BlockExample1::DEFAULT_WRITE_UINT16);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_INT16,
                                                         BlockExample1::DEFAULT_WRITE_INT16);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_UINT16,
                                                         BlockExample1::DEFAULT_READ_UINT16);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_INT16,
                                                         BlockExample1::DEFAULT_READ_INT16);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_UINT32,
                                                         BlockExample1::DEFAULT_WRITE_UINT32);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_INT32,
                                                         BlockExample1::DEFAULT_WRITE_INT32);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_UINT32,
                                                         BlockExample1::DEFAULT_READ_UINT32);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_INT32,
                                                         BlockExample1::DEFAULT_READ_INT32);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_WRITE_FLOAT,
                                                         BlockExample1::DEFAULT_WRITE_FLOAT);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockExample1::MEMBER_ID_READ_FLOAT,
                                                         BlockExample1::DEFAULT_READ_FLOAT);

  return (initStatus); 
}

Error_t initLimits(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  return (initStatus); 
}


} } } /* End Namespace - Atams::MapTest::BlockExample1 */

/**
  * @}End of File
  */
