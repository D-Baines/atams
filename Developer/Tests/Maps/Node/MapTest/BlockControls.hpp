/**
  ******************************************************************************
  * @file    BlockControls.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Controls.
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

namespace Atams { namespace MapTest { namespace BlockControls {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {20U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_VALUE_UNITS                                          {75U};
inline constexpr VarID_t VAR_BAR_INT_LIMITS                                       {76U};
inline constexpr VarID_t VAR_BAR_FLOAT_LIMITS                                     {77U};
inline constexpr VarID_t VAR_BAR_MIN_ONLY                                         {78U};
inline constexpr VarID_t VAR_BAR_NO_LIMITS                                        {79U};
inline constexpr VarID_t VAR_STATUS_UINT8                                         {80U};
inline constexpr VarID_t VAR_STATUS_INT8                                          {81U};
inline constexpr VarID_t VAR_FLAG                                                 {82U};
inline constexpr VarID_t VAR_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING {83U};
inline constexpr VarID_t VAR_INPUT_LIMITS                                         {84U};
inline constexpr VarID_t VAR_DROPDOWN_MODE                                        {85U};
inline constexpr VarID_t VAR_SLIDER_UINT8                                         {86U};
inline constexpr VarID_t VAR_SLIDER_FLOAT                                         {87U};
inline constexpr VarID_t VAR_SLIDER_NO_LIMITS                                     {88U};
inline constexpr VarID_t VAR_COMMAND_OPTIONS                                      {89U};
inline constexpr VarID_t VAR_COMMAND_PLAIN                                        {90U};
inline constexpr VarID_t VAR_DEVICE_TICK                                          {91U};
inline constexpr VarID_t VAR_TEST_SAWTOOTH                                        {92U};
inline constexpr VarID_t VAR_TEST_SINE                                            {93U};
inline constexpr VarID_t VAR_TEST_COSINE                                          {94U};

/*------------------------- Defaults List --------------------------*/

inline constexpr float    DEFAULT_VALUE_UNITS                                          {1234.5F};
inline constexpr int16_t  DEFAULT_BAR_INT_LIMITS                                       {25};
inline constexpr float    DEFAULT_BAR_FLOAT_LIMITS                                     {42.5F};
inline constexpr uint16_t DEFAULT_BAR_MIN_ONLY                                         {20U};
inline constexpr uint32_t DEFAULT_BAR_NO_LIMITS                                        {1000U};
inline constexpr uint8_t  DEFAULT_STATUS_UINT8                                         {1U};
inline constexpr int8_t   DEFAULT_STATUS_INT8                                          {0};
inline constexpr uint8_t  DEFAULT_FLAG                                                 {1U};
inline constexpr uint8_t  DEFAULT_A_VERY_LONG_VARIABLE_NAME_FOR_TESTING_TITLE_CLIPPING {0U};
inline constexpr int32_t  DEFAULT_INPUT_LIMITS                                         {0};
inline constexpr uint8_t  DEFAULT_DROPDOWN_MODE                                        {0U};
inline constexpr uint8_t  DEFAULT_SLIDER_UINT8                                         {10U};
inline constexpr float    DEFAULT_SLIDER_FLOAT                                         {0.0F};
inline constexpr int16_t  DEFAULT_SLIDER_NO_LIMITS                                     {0};
inline constexpr uint8_t  DEFAULT_COMMAND_OPTIONS                                      {0U};
inline constexpr uint16_t DEFAULT_COMMAND_PLAIN                                        {0U};
inline constexpr uint32_t DEFAULT_DEVICE_TICK                                          {0U};
inline constexpr uint8_t  DEFAULT_TEST_SAWTOOTH                                        {0U};
inline constexpr float    DEFAULT_TEST_SINE                                            {0.0F};
inline constexpr float    DEFAULT_TEST_COSINE                                          {1.0F};


} } } /* End Namespace - Atams::MapTest::BlockControls */

/**
  * @}End of File
  */
