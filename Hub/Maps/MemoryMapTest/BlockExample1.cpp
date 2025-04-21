/**
  ******************************************************************************
  * @file    BlockExample1.cpp
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

#include "BlockExample1.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample1 {

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/

static Atams::Error_t initDefaults(DataBlock &blockToInit)
{
  Atams::Error_t error = Atams::ERROR_NONE;

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_UINT8,
                                        BlockExample1::DEFAULT_WRITE_UINT8);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_INT8,
                                        BlockExample1::DEFAULT_WRITE_INT8);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_UINT8,
                                        BlockExample1::DEFAULT_READ_UINT8);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_INT8,
                                        BlockExample1::DEFAULT_READ_INT8);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_UINT16,
                                        BlockExample1::DEFAULT_WRITE_UINT16);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_INT16,
                                        BlockExample1::DEFAULT_WRITE_INT16);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_UINT16,
                                        BlockExample1::DEFAULT_READ_UINT16);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_INT16,
                                        BlockExample1::DEFAULT_READ_INT16);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_UINT32,
                                        BlockExample1::DEFAULT_WRITE_UINT32);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_INT32,
                                        BlockExample1::DEFAULT_WRITE_INT32);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_UINT32,
                                        BlockExample1::DEFAULT_READ_UINT32);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_INT32,
                                        BlockExample1::DEFAULT_READ_INT32);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_WRITE_FLOAT,
                                        BlockExample1::DEFAULT_WRITE_FLOAT);

  if (!error) error = blockToInit.write(BlockExample1::VAR_ID_READ_FLOAT,
                                        BlockExample1::DEFAULT_READ_FLOAT);

  return (error); 
}

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

const DataBlock::Descriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockExample1::NUMBER_OF_EXAMPLE1_VARS,
  /* .initDefaults    = */ initDefaults, 
  /* .dataMemberInfo  = */
  {
    /* [BlockExample1::VAR_ID_WRITE_UINT8] = */
    {
      /* .type           = */ Atams::TYPE_UINT8,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_INT8] = */
    {
      /* .type           = */ Atams::TYPE_INT8,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_UINT8] = */
    {
      /* .type           = */ Atams::TYPE_UINT8,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_INT8] = */
    {
      /* .type           = */ Atams::TYPE_INT8,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_UINT16] = */
    {
      /* .type           = */ Atams::TYPE_UINT16,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_INT16] = */
    {
      /* .type           = */ Atams::TYPE_INT16,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_UINT16] = */
    {
      /* .type           = */ Atams::TYPE_UINT16,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_INT16] = */
    {
      /* .type           = */ Atams::TYPE_INT16,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_UINT32] = */
    {
      /* .type           = */ Atams::TYPE_UINT32,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_INT32] = */
    {
      /* .type           = */ Atams::TYPE_INT32,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_UINT32] = */
    {
      /* .type           = */ Atams::TYPE_UINT32,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_INT32] = */
    {
      /* .type           = */ Atams::TYPE_INT32,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_WRITE_FLOAT] = */
    {
      /* .type           = */ Atams::TYPE_FLOAT,
      /* .externalAccess = */ Atams::ACCESS_WRITE,
      /* .NVMStorage     = */ false,
    },
    /* [BlockExample1::VAR_ID_READ_FLOAT] = */
    {
      /* .type           = */ Atams::TYPE_FLOAT,
      /* .externalAccess = */ Atams::ACCESS_READ,
      /* .NVMStorage     = */ false,
    },
  }
};


} } } /* End Namespace - Atams::MapTest::BlockExample1 */

/**
  * @}End of File
  */
