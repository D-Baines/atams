/**
  ******************************************************************************
  * @file    BlockConfigWidgets.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: ConfigWidgets.
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

namespace Atams { namespace MapTest { namespace BlockConfigWidgets {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {17U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_CHECKBOX          {58U};
inline constexpr VarID_t VAR_BOOLSTATE_RO      {59U};
inline constexpr VarID_t VAR_BOOLSTATE_RW      {60U};
inline constexpr VarID_t VAR_DROPDOWN_UINT8    {61U};
inline constexpr VarID_t VAR_DROPDOWN_INT8     {62U};
inline constexpr VarID_t VAR_DROPDOWN_UINT16   {63U};
inline constexpr VarID_t VAR_DROPDOWN_INT16    {64U};
inline constexpr VarID_t VAR_DROPDOWN_UINT32   {65U};
inline constexpr VarID_t VAR_DROPDOWN_INT32    {66U};
inline constexpr VarID_t VAR_DROPDOWN_FLOAT    {67U};
inline constexpr VarID_t VAR_READOPTION_UINT8  {68U};
inline constexpr VarID_t VAR_READOPTION_INT8   {69U};
inline constexpr VarID_t VAR_READOPTION_UINT16 {70U};
inline constexpr VarID_t VAR_READOPTION_INT16  {71U};
inline constexpr VarID_t VAR_READOPTION_UINT32 {72U};
inline constexpr VarID_t VAR_READOPTION_INT32  {73U};
inline constexpr VarID_t VAR_READOPTION_FLOAT  {74U};

/*------------------------- Defaults List --------------------------*/

inline constexpr uint8_t  DEFAULT_CHECKBOX          {0U};
inline constexpr uint8_t  DEFAULT_BOOLSTATE_RO      {1U};
inline constexpr uint8_t  DEFAULT_BOOLSTATE_RW      {0U};
inline constexpr uint8_t  DEFAULT_DROPDOWN_UINT8    {8U};
inline constexpr int8_t   DEFAULT_DROPDOWN_INT8     {9};
inline constexpr uint16_t DEFAULT_DROPDOWN_UINT16   {10U};
inline constexpr int16_t  DEFAULT_DROPDOWN_INT16    {11};
inline constexpr uint32_t DEFAULT_DROPDOWN_UINT32   {12U};
inline constexpr int32_t  DEFAULT_DROPDOWN_INT32    {13};
inline constexpr float    DEFAULT_DROPDOWN_FLOAT    {14.0F};
inline constexpr uint8_t  DEFAULT_READOPTION_UINT8  {24U};
inline constexpr int8_t   DEFAULT_READOPTION_INT8   {25};
inline constexpr uint16_t DEFAULT_READOPTION_UINT16 {26U};
inline constexpr int16_t  DEFAULT_READOPTION_INT16  {27};
inline constexpr uint32_t DEFAULT_READOPTION_UINT32 {28U};
inline constexpr int32_t  DEFAULT_READOPTION_INT32  {29};
inline constexpr float    DEFAULT_READOPTION_FLOAT  {30.0F};


} } } /* End Namespace - Atams::MapTest::BlockConfigWidgets */

/**
  * @}End of File
  */
