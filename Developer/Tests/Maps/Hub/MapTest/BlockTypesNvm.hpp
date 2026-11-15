/**
  ******************************************************************************
  * @file    BlockTypesNvm.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: TypesNvm.
  *          The Data Block is part of an Atams Memory Map with name: Test.
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockTypesNvm {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {14U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_READ_UINT8   {44U};
inline constexpr VarID_t VAR_READ_INT8    {45U};
inline constexpr VarID_t VAR_READ_UINT16  {46U};
inline constexpr VarID_t VAR_READ_INT16   {47U};
inline constexpr VarID_t VAR_READ_UINT32  {48U};
inline constexpr VarID_t VAR_READ_INT32   {49U};
inline constexpr VarID_t VAR_READ_FLOAT   {50U};
inline constexpr VarID_t VAR_WRITE_UINT8  {51U};
inline constexpr VarID_t VAR_WRITE_INT8   {52U};
inline constexpr VarID_t VAR_WRITE_UINT16 {53U};
inline constexpr VarID_t VAR_WRITE_INT16  {54U};
inline constexpr VarID_t VAR_WRITE_UINT32 {55U};
inline constexpr VarID_t VAR_WRITE_INT32  {56U};
inline constexpr VarID_t VAR_WRITE_FLOAT  {57U};

/*------------------------- Defaults List --------------------------*/

inline constexpr uint8_t  DEFAULT_READ_UINT8   {0U};
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


} } } /* End Namespace - Atams::MapTest::BlockTypesNvm */

/**
  * @}End of File
  */
