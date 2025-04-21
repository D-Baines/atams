/**
  ******************************************************************************
  * @file    BlockExample2.hpp
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

enum VarID_t: uint16_t
{
  VAR_ID_WRITE_UINT8  = 0U,
  VAR_ID_WRITE_INT8   = 1U,
  VAR_ID_READ_UINT8   = 2U,
  VAR_ID_READ_INT8    = 3U,
  VAR_ID_WRITE_UINT16 = 4U,
  VAR_ID_WRITE_INT16  = 5U,
  VAR_ID_READ_UINT16  = 6U,
  VAR_ID_READ_INT16   = 7U,
  VAR_ID_WRITE_UINT32 = 8U,
  VAR_ID_WRITE_INT32  = 9U,
  VAR_ID_READ_UINT32  = 10U,
  VAR_ID_READ_INT32   = 11U,
  VAR_ID_WRITE_FLOAT  = 12U,
  VAR_ID_READ_FLOAT   = 13U,

  NUMBER_OF_EXAMPLE2_VARS
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint8_t  DEFAULT_WRITE_UINT8  {1};
inline constexpr int8_t   DEFAULT_WRITE_INT8   {2};
inline constexpr uint8_t  DEFAULT_READ_UINT8   {3};
inline constexpr int8_t   DEFAULT_READ_INT8    {4};
inline constexpr uint16_t DEFAULT_WRITE_UINT16 {5};
inline constexpr int16_t  DEFAULT_WRITE_INT16  {6};
inline constexpr uint16_t DEFAULT_READ_UINT16  {7};
inline constexpr int16_t  DEFAULT_READ_INT16   {8};
inline constexpr uint32_t DEFAULT_WRITE_UINT32 {9};
inline constexpr int32_t  DEFAULT_WRITE_INT32  {10};
inline constexpr uint32_t DEFAULT_READ_UINT32  {11};
inline constexpr int32_t  DEFAULT_READ_INT32   {12};
inline constexpr float    DEFAULT_WRITE_FLOAT  {13.0F};
inline constexpr float    DEFAULT_READ_FLOAT   {14.0F};

/*************************************************************************************/
/* CONST EXTERNS                                                                     */
/*************************************************************************************/

extern const DataBlock::Descriptor_t blockDescriptor;


} } } /* End Namespace - Atams::MapTest::BlockExample2 */

/**
  * @}End of File
  */
