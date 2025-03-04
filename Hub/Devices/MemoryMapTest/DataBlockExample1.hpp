/**
  ******************************************************************************
  * @file    DataBlockExample1.hpp
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

#include <stdint.h>
#include "../../DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockExample1 {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/*--- Member List ---*/
enum DataMemberID_t: uint16_t
{
  MEMBER_ID_WRITE_UINT8  = 0U,
  MEMBER_ID_WRITE_INT8   = 1U,
  MEMBER_ID_READ_UINT8   = 2U,
  MEMBER_ID_READ_INT8    = 3U,
  MEMBER_ID_WRITE_UINT16 = 4U,
  MEMBER_ID_WRITE_INT16  = 5U,
  MEMBER_ID_READ_UINT16  = 6U,
  MEMBER_ID_READ_INT16   = 7U,
  MEMBER_ID_WRITE_UINT32 = 8U,
  MEMBER_ID_WRITE_INT32  = 9U,
  MEMBER_ID_READ_UINT32  = 10U,
  MEMBER_ID_READ_INT32   = 11U,
  MEMBER_ID_WRITE_FLOAT  = 12U,
  MEMBER_ID_READ_FLOAT   = 13U,

  NUMBER_OF_DATA_MEMBERS
};

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

/*************************************************************************************/
/* CONSTANT EXTERNS                                                                  */
/*************************************************************************************/

/*--- Descriptor ---*/
extern const DataBlock::BlockDescriptor_t blockDescriptor;


} } } /* End Namespace - Atams::MapTest::BlockExample1 */

/**
  * @}End of File
  */
