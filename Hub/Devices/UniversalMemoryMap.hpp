/**
  ******************************************************************************
  * @file    UniversalMemoryMap.hpp
  *
  * @author  D. Baines
  *
  * @brief
  *
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
#include "../Node.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace UniversalMemoryMap {


/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*-- Defaults -----------------------------------------------------------------------*/ 

constexpr uint32_t DEFAULT_WATCHDOG_TIMEOUT = 0UL;
constexpr uint32_t DEFAULT_WATCHDOG_COUNT   = 0UL;

/*-- Minimum Limits -----------------------------------------------------------------*/ 

constexpr uint8_t  MIN_LIMIT_NODE_ID          = 0U;
constexpr uint32_t MIN_LIMIT_WATCHDOG_TIMEOUT = 0UL;
constexpr uint32_t MIN_LIMIT_WATCHDOG_COUNT   = 0UL;

/*-- Maximum Limits -----------------------------------------------------------------*/ 

constexpr uint8_t  MAX_LIMIT_NODE_ID          = NODE_ID_MAX;
constexpr uint32_t MAX_LIMIT_WATCHDOG_TIMEOUT = MAX_UINT32;
constexpr uint32_t MAX_LIMIT_WATCHDOG_COUNT   = MAX_UINT32;


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/*-- Memory Map Data Field List -----------------------------------------------------*/
             
typedef enum: uint8_t
{
  BLOCK_ID_UNIVERSAL = 0U,

  NUMBER_OF_DATAFIELDS

} DataFieldID_t;


/*-- Data Field Member Lists --------------------------------------------------------*/

typedef enum: uint16_t
{ 
  MEMBER_ID_NODE_ID          = 0U,
  MEMBER_ID_NODE_TYPE        = 1U,
  MEMBER_ID_FIRMWARE_MAJOR   = 2U,
  MEMBER_ID_FIRMWARE_MINOR   = 3U,
  MEMBER_ID_HARDWARE_MAJOR   = 4U,
  MEMBER_ID_HARDWARE_MINOR   = 5U,
  MEMBER_ID_SERIAL_NUMBER    = 6U,
  MEMBER_ID_BOOT_MODE        = 7U,
  MEMBER_ID_STORE_ALL        = 8U,
  MEMBER_ID_RESTORE_FACTORY  = 9U,
  MEMBER_ID_WATCHDOG_TIMEOUT = 10U,
  MEMBER_ID_WATCHDOG_COUNT   = 11U,

  NUMBER_OF_UNIVERSAL_DATA_MEMBERS
} UniversalDataMemberID_t;


/*-- Custom Member Types ------------------------------------------------------------*/


/*************************************************************************************/
/* CONSTANT GLOBALS                                                                  */
/*************************************************************************************/

extern const Node::MemoryMap_t memoryMap;
extern const Node::DataField_t dataFieldUniversal;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(Node &nodeToInit);

Error_t initLimits(Node &nodeToInit);


} } /* End Namespace - Atams::UniversalMemoryMap */

/**
  * @}End of File
  */
