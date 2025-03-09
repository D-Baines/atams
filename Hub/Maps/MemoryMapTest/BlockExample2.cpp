/**
  ******************************************************************************
  * @file    BlockExample2.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Example1.
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

#include "BlockExample2.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample2 {

/*************************************************************************************/
/* INIT FUNCTION DEFINITIONS                                                         */
/*************************************************************************************/

static Error_t initDefaults(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  return (initStatus); 
}

/*************************************************************************************/
/* BLOCK DESCRIPTOR                                                                  */
/*************************************************************************************/

const DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockExample2::NUMBER_OF_DATA_MEMBERS,
  /* .initDefaults    = */ initDefaults,
  /* .dataMemberInfo  = */
  {
    /* [BlockExample1::MEMBER_ID_WRITE_UINT8] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_INT8] = */
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_READ_UINT8] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_READ_INT8] = */
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_UINT16] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_INT16] = */
    {
      /* .type           = */ TYPE_INT16,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_READ_UINT16] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_READ_INT16] = */
    {
      /* .type           = */ TYPE_INT16,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_UINT32] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_INT32] = */
    {
      /* .type           = */ TYPE_INT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_READ_UINT32] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_READ_INT32] = */
    {
      /* .type           = */ TYPE_INT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample1::MEMBER_ID_WRITE_FLOAT] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample1::MEMBER_ID_READ_FLOAT] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
    },
  }
};


} } } /* End Namespace - Atams::MapTest::DataBlockExample2 */

/**
  * @}End of File
  */
