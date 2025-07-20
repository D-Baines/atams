/**
  ******************************************************************************
  * @file    BlockTest2.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Test2.
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockTest2 {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

enum VarID_t: uint16_t
{
  VAR_READ_UINT8   = 42U,
  VAR_READ_INT8    = 43U,
  VAR_READ_UINT16  = 44U,
  VAR_READ_INT16   = 45U,
  VAR_READ_UINT32  = 46U,
  VAR_READ_INT32   = 47U,
  VAR_READ_FLOAT   = 48U,
  VAR_WRITE_UINT8  = 49U,
  VAR_WRITE_INT8   = 50U,
  VAR_WRITE_UINT16 = 51U,
  VAR_WRITE_INT16  = 52U,
  VAR_WRITE_UINT32 = 53U,
  VAR_WRITE_INT32  = 54U,
  VAR_WRITE_FLOAT  = 55U,
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NUMBER_OF_VARS {14U};

inline constexpr int8_t   DEFAULT_READ_INT8    {1};
inline constexpr uint16_t DEFAULT_READ_UINT16  {2U};
inline constexpr int16_t  DEFAULT_READ_INT16   {3};
inline constexpr uint32_t DEFAULT_READ_UINT32  {4U};
inline constexpr int32_t  DEFAULT_READ_INT32   {5};
inline constexpr float    DEFAULT_READ_FLOAT   {6.0F};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8  {7U};
inline constexpr int8_t   DEFAULT_WRITE_INT8   {8};
inline constexpr uint16_t DEFAULT_WRITE_UINT16 {9U};
inline constexpr int16_t  DEFAULT_WRITE_INT16  {10};
inline constexpr uint32_t DEFAULT_WRITE_UINT32 {11U};
inline constexpr int32_t  DEFAULT_WRITE_INT32  {12};
inline constexpr float    DEFAULT_WRITE_FLOAT  {13.0F};


} } } /* End Namespace - Atams::MapTest::BlockTest2 */

/**
  * @}End of File
  */
