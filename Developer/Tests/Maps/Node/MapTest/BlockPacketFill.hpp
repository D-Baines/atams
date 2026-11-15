/**
  ******************************************************************************
  * @file    BlockPacketFill.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an Atams Data Block with name: PacketFill.
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

namespace Atams { namespace MapTest { namespace BlockPacketFill {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*------------------------ Data Block Info -------------------------*/

inline constexpr uint16_t NUMBER_OF_VARS {40U};

/*-------------------------- Var ID List ---------------------------*/

inline constexpr VarID_t VAR_WRITE_UINT8_1   {99U};
inline constexpr VarID_t VAR_WRITE_UINT8_2   {100U};
inline constexpr VarID_t VAR_WRITE_UINT8_3   {101U};
inline constexpr VarID_t VAR_WRITE_UINT8_4   {102U};
inline constexpr VarID_t VAR_WRITE_UINT8_5   {103U};
inline constexpr VarID_t VAR_WRITE_UINT8_6   {104U};
inline constexpr VarID_t VAR_WRITE_UINT8_7   {105U};
inline constexpr VarID_t VAR_WRITE_UINT8_8   {106U};
inline constexpr VarID_t VAR_WRITE_UINT8_9   {107U};
inline constexpr VarID_t VAR_WRITE_UINT8_10  {108U};
inline constexpr VarID_t VAR_WRITE_UINT8_11  {109U};
inline constexpr VarID_t VAR_WRITE_UINT8_12  {110U};
inline constexpr VarID_t VAR_WRITE_UINT8_13  {111U};
inline constexpr VarID_t VAR_WRITE_UINT8_14  {112U};
inline constexpr VarID_t VAR_WRITE_UINT8_15  {113U};
inline constexpr VarID_t VAR_WRITE_UINT8_16  {114U};
inline constexpr VarID_t VAR_WRITE_UINT8_17  {115U};
inline constexpr VarID_t VAR_WRITE_UINT8_18  {116U};
inline constexpr VarID_t VAR_WRITE_UINT8_19  {117U};
inline constexpr VarID_t VAR_WRITE_UINT8_20  {118U};
inline constexpr VarID_t VAR_WRITE_UINT32_1  {119U};
inline constexpr VarID_t VAR_WRITE_UINT32_2  {120U};
inline constexpr VarID_t VAR_WRITE_UINT32_3  {121U};
inline constexpr VarID_t VAR_WRITE_UINT32_4  {122U};
inline constexpr VarID_t VAR_WRITE_UINT32_5  {123U};
inline constexpr VarID_t VAR_WRITE_UINT32_6  {124U};
inline constexpr VarID_t VAR_WRITE_UINT32_7  {125U};
inline constexpr VarID_t VAR_WRITE_UINT32_8  {126U};
inline constexpr VarID_t VAR_WRITE_UINT32_9  {127U};
inline constexpr VarID_t VAR_WRITE_UINT32_10 {128U};
inline constexpr VarID_t VAR_WRITE_UINT32_11 {129U};
inline constexpr VarID_t VAR_WRITE_UINT32_12 {130U};
inline constexpr VarID_t VAR_WRITE_UINT32_13 {131U};
inline constexpr VarID_t VAR_WRITE_UINT32_14 {132U};
inline constexpr VarID_t VAR_WRITE_UINT32_15 {133U};
inline constexpr VarID_t VAR_WRITE_UINT32_16 {134U};
inline constexpr VarID_t VAR_WRITE_UINT32_17 {135U};
inline constexpr VarID_t VAR_WRITE_UINT32_18 {136U};
inline constexpr VarID_t VAR_WRITE_UINT32_19 {137U};
inline constexpr VarID_t VAR_WRITE_UINT32_20 {138U};

/*------------------------- Defaults List --------------------------*/

inline constexpr uint8_t  DEFAULT_WRITE_UINT8_1   {0U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_2   {1U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_3   {2U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_4   {3U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_5   {4U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_6   {5U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_7   {6U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_8   {7U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_9   {8U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_10  {9U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_11  {10U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_12  {11U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_13  {12U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_14  {13U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_15  {14U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_16  {15U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_17  {16U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_18  {17U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_19  {18U};
inline constexpr uint8_t  DEFAULT_WRITE_UINT8_20  {19U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_1  {20U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_2  {21U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_3  {22U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_4  {23U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_5  {24U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_6  {25U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_7  {26U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_8  {27U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_9  {28U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_10 {29U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_11 {30U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_12 {31U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_13 {32U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_14 {33U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_15 {34U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_16 {35U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_17 {36U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_18 {37U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_19 {38U};
inline constexpr uint32_t DEFAULT_WRITE_UINT32_20 {39U};


} } } /* End Namespace - Atams::MapTest::BlockPacketFill */

/**
  * @}End of File
  */
