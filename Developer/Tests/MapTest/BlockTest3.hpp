/**
  ******************************************************************************
  * @file    BlockTest3.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: Test3.
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace MapTest { namespace BlockTest3 {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

enum VarID_t: uint16_t
{
  VAR_WRITE_UINT8_1   = 56U,
  VAR_WRITE_UINT8_2   = 57U,
  VAR_WRITE_UINT8_3   = 58U,
  VAR_WRITE_UINT8_4   = 59U,
  VAR_WRITE_UINT8_5   = 60U,
  VAR_WRITE_UINT8_6   = 61U,
  VAR_WRITE_UINT8_7   = 62U,
  VAR_WRITE_UINT8_8   = 63U,
  VAR_WRITE_UINT8_9   = 64U,
  VAR_WRITE_UINT8_10  = 65U,
  VAR_WRITE_UINT8_11  = 66U,
  VAR_WRITE_UINT8_12  = 67U,
  VAR_WRITE_UINT8_13  = 68U,
  VAR_WRITE_UINT8_14  = 69U,
  VAR_WRITE_UINT8_15  = 70U,
  VAR_WRITE_UINT8_16  = 71U,
  VAR_WRITE_UINT8_17  = 72U,
  VAR_WRITE_UINT8_18  = 73U,
  VAR_WRITE_UINT8_19  = 74U,
  VAR_WRITE_UINT8_20  = 75U,
  VAR_WRITE_UINT32_1  = 76U,
  VAR_WRITE_UINT32_2  = 77U,
  VAR_WRITE_UINT32_3  = 78U,
  VAR_WRITE_UINT32_4  = 79U,
  VAR_WRITE_UINT32_5  = 80U,
  VAR_WRITE_UINT32_6  = 81U,
  VAR_WRITE_UINT32_7  = 82U,
  VAR_WRITE_UINT32_8  = 83U,
  VAR_WRITE_UINT32_9  = 84U,
  VAR_WRITE_UINT32_10 = 85U,
  VAR_WRITE_UINT32_11 = 86U,
  VAR_WRITE_UINT32_12 = 87U,
  VAR_WRITE_UINT32_13 = 88U,
  VAR_WRITE_UINT32_14 = 89U,
  VAR_WRITE_UINT32_15 = 90U,
  VAR_WRITE_UINT32_16 = 91U,
  VAR_WRITE_UINT32_17 = 92U,
  VAR_WRITE_UINT32_18 = 93U,
  VAR_WRITE_UINT32_19 = 94U,
  VAR_WRITE_UINT32_20 = 95U,
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint16_t NUMBER_OF_VARS {40U};

constexpr uint8_t  DEFAULT_WRITE_UINT8_1   {0U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_2   {1U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_3   {2U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_4   {3U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_5   {4U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_6   {5U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_7   {6U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_8   {7U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_9   {8U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_10  {9U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_11  {10U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_12  {11U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_13  {12U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_14  {13U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_15  {14U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_16  {15U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_17  {16U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_18  {17U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_19  {18U};
constexpr uint8_t  DEFAULT_WRITE_UINT8_20  {19U};
constexpr uint32_t DEFAULT_WRITE_UINT32_1  {20U};
constexpr uint32_t DEFAULT_WRITE_UINT32_2  {21U};
constexpr uint32_t DEFAULT_WRITE_UINT32_3  {22U};
constexpr uint32_t DEFAULT_WRITE_UINT32_4  {23U};
constexpr uint32_t DEFAULT_WRITE_UINT32_5  {24U};
constexpr uint32_t DEFAULT_WRITE_UINT32_6  {25U};
constexpr uint32_t DEFAULT_WRITE_UINT32_7  {26U};
constexpr uint32_t DEFAULT_WRITE_UINT32_8  {27U};
constexpr uint32_t DEFAULT_WRITE_UINT32_9  {28U};
constexpr uint32_t DEFAULT_WRITE_UINT32_10 {29U};
constexpr uint32_t DEFAULT_WRITE_UINT32_11 {30U};
constexpr uint32_t DEFAULT_WRITE_UINT32_12 {31U};
constexpr uint32_t DEFAULT_WRITE_UINT32_13 {32U};
constexpr uint32_t DEFAULT_WRITE_UINT32_14 {33U};
constexpr uint32_t DEFAULT_WRITE_UINT32_15 {34U};
constexpr uint32_t DEFAULT_WRITE_UINT32_16 {35U};
constexpr uint32_t DEFAULT_WRITE_UINT32_17 {36U};
constexpr uint32_t DEFAULT_WRITE_UINT32_18 {37U};
constexpr uint32_t DEFAULT_WRITE_UINT32_19 {38U};
constexpr uint32_t DEFAULT_WRITE_UINT32_20 {39U};


} } } /* End Namespace - Atams::MapTest::BlockTest3 */

/**
  * @}End of File
  */
