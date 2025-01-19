/**
  ******************************************************************************
  * @file    DataBlockExample2.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Example2.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../../DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample2 {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/*--- Member List ---*/
typedef enum: uint16_t
{
  MEMBER_ID_WRITE_UINT8  = 0U,
  MEMBER_ID_WRITE_INT8   = 1U,
  MEMBER_ID_WRITE_UINT16 = 2U,
  MEMBER_ID_WRITE_INT16  = 3U,
  MEMBER_ID_WRITE_UINT32 = 4U,
  MEMBER_ID_WRITE_INT32  = 5U,
  MEMBER_ID_WRITE_FLOAT  = 6U,
  MEMBER_ID_READ_UINT8   = 7U,
  MEMBER_ID_READ_INT8    = 8U,
  MEMBER_ID_READ_UINT16  = 9U,
  MEMBER_ID_READ_INT16   = 10U,
  MEMBER_ID_READ_UINT32  = 11U,
  MEMBER_ID_READ_INT32   = 12U,
  MEMBER_ID_READ_FLOAT   = 13U,

  NUMBER_OF_DATA_MEMBERS
} DataMemberID_t;

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*--- Defaults ---*/
inline constexpr uint8_t  DEFAULT_WRITE_UINT8  = 1U;
inline constexpr int8_t   DEFAULT_WRITE_INT8   = 2;
inline constexpr uint8_t  DEFAULT_READ_UINT8   = 3U;
inline constexpr int8_t   DEFAULT_READ_INT8    = 4;
inline constexpr uint16_t DEFAULT_WRITE_UINT16 = 5U;
inline constexpr int16_t  DEFAULT_WRITE_INT16  = 6;
inline constexpr uint16_t DEFAULT_READ_UINT16  = 7U;
inline constexpr int16_t  DEFAULT_READ_INT16   = 8;
inline constexpr uint32_t DEFAULT_WRITE_UINT32 = 9UL;
inline constexpr int32_t  DEFAULT_WRITE_INT32  = 10L;
inline constexpr uint32_t DEFAULT_READ_UINT32  = 11UL;
inline constexpr int32_t  DEFAULT_READ_INT32   = 12L;
inline constexpr float    DEFAULT_WRITE_FLOAT  = 13.0F;
inline constexpr float    DEFAULT_READ_FLOAT   = 14.0F;

/*--- Minimum Limits ---*/

/*--- Maximum Limits ---*/

/*--- Descriptor ---*/
inline constexpr DataBlock::BlockDescriptor_t blockDescriptor =
{
  /* .noOfDataMembers = */ BlockExample2::NUMBER_OF_DATA_MEMBERS,
  /* .dataMemberInfo  = */
  {
    /* [BlockExample2::MEMBER_ID_WRITE_UINT8] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_INT8] = */
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_UINT16] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_INT16] = */
    {
      /* .type           = */ TYPE_INT16,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_UINT32] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_INT32] = */
    {
      /* .type           = */ TYPE_INT32,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_WRITE_FLOAT] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_WRITE,
    },
    /* [BlockExample2::MEMBER_ID_READ_UINT8] = */
    {
      /* .type           = */ TYPE_UINT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_INT8] = */
    {
      /* .type           = */ TYPE_INT8,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_UINT16] = */
    {
      /* .type           = */ TYPE_UINT16,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_INT16] = */
    {
      /* .type           = */ TYPE_INT16,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_UINT32] = */
    {
      /* .type           = */ TYPE_UINT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_INT32] = */
    {
      /* .type           = */ TYPE_INT32,
      /* .externalAccess = */ ACCESS_READ,
    },
    /* [BlockExample2::MEMBER_ID_READ_FLOAT] = */
    {
      /* .type           = */ TYPE_FLOAT,
      /* .externalAccess = */ ACCESS_READ,
    },
  }
};




} } } /* End Namespace - Atams::MapTest::BlockExample2 */

/**
  * @}End of File
  */
