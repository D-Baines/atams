/**
  ******************************************************************************
  * @file    BlockTest1.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Test1.
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

#include "../../../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockTest1 {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

constexpr uint16_t NUMBER_OF_VARS {14U};

/*-------------------------- Var ID List ---------------------------*/

constexpr VarID_t VAR_READ_UINT8   {30U};
constexpr VarID_t VAR_READ_INT8    {31U};
constexpr VarID_t VAR_READ_UINT16  {32U};
constexpr VarID_t VAR_READ_INT16   {33U};
constexpr VarID_t VAR_READ_UINT32  {34U};
constexpr VarID_t VAR_READ_INT32   {35U};
constexpr VarID_t VAR_READ_FLOAT   {36U};
constexpr VarID_t VAR_WRITE_UINT8  {37U};
constexpr VarID_t VAR_WRITE_INT8   {38U};
constexpr VarID_t VAR_WRITE_UINT16 {39U};
constexpr VarID_t VAR_WRITE_INT16  {40U};
constexpr VarID_t VAR_WRITE_UINT32 {41U};
constexpr VarID_t VAR_WRITE_INT32  {42U};
constexpr VarID_t VAR_WRITE_FLOAT  {43U};

/*------------------------- Defaults List --------------------------*/

constexpr uint8_t  DEFAULT_READ_UINT8   {0U};
constexpr int8_t   DEFAULT_READ_INT8    {1};
constexpr uint16_t DEFAULT_READ_UINT16  {2U};
constexpr int16_t  DEFAULT_READ_INT16   {3};
constexpr uint32_t DEFAULT_READ_UINT32  {4U};
constexpr int32_t  DEFAULT_READ_INT32   {5};
constexpr float    DEFAULT_READ_FLOAT   {6.0F};
constexpr uint8_t  DEFAULT_WRITE_UINT8  {7U};
constexpr int8_t   DEFAULT_WRITE_INT8   {8};
constexpr uint16_t DEFAULT_WRITE_UINT16 {9U};
constexpr int16_t  DEFAULT_WRITE_INT16  {10};
constexpr uint32_t DEFAULT_WRITE_UINT32 {11U};
constexpr int32_t  DEFAULT_WRITE_INT32  {12};
constexpr float    DEFAULT_WRITE_FLOAT  {13.0F};


} } } /* End Namespace - Atams::MapTest::BlockTest1 */

/**
  * @}End of File
  */
