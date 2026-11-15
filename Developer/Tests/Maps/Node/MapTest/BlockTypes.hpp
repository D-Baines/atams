/**
  ******************************************************************************
  * @file    BlockTypes.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Types.
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

namespace Atams { namespace MapTest { namespace BlockTypes {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {14U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_READ_UINT8   {30U};
inline constexpr VarID_t VAR_READ_INT8    {31U};
inline constexpr VarID_t VAR_READ_UINT16  {32U};
inline constexpr VarID_t VAR_READ_INT16   {33U};
inline constexpr VarID_t VAR_READ_UINT32  {34U};
inline constexpr VarID_t VAR_READ_INT32   {35U};
inline constexpr VarID_t VAR_READ_FLOAT   {36U};
inline constexpr VarID_t VAR_WRITE_UINT8  {37U};
inline constexpr VarID_t VAR_WRITE_INT8   {38U};
inline constexpr VarID_t VAR_WRITE_UINT16 {39U};
inline constexpr VarID_t VAR_WRITE_INT16  {40U};
inline constexpr VarID_t VAR_WRITE_UINT32 {41U};
inline constexpr VarID_t VAR_WRITE_INT32  {42U};
inline constexpr VarID_t VAR_WRITE_FLOAT  {43U};

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


} } } /* End Namespace - Atams::MapTest::BlockTypes */

/**
  * @}End of File
  */
