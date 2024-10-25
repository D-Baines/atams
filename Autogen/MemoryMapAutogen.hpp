/**
  ******************************************************************************
  * @file    MemoryMap$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$.hpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing typedefs, constants, and function
  *          declarations for an $$$AUTOGEN$$$FRAMEWORK_NAME$$$AUTOGEN$$$ Memory Map with name: $$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$.
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
#include "../Node.hpp"
$$$AUTOGEN$$$BLOCK_FILE_INCLUDES$$$AUTOGEN$$$

/*************************************************************************************/
/* MEMORY MAP NAMESPACE                                                              */
/*************************************************************************************/

namespace $$$AUTOGEN$$$FRAMEWORK_NAME$$$AUTOGEN$$$ { namespace Map$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$ {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/
             
typedef enum: uint8_t
{
$$$AUTOGEN$$$BLOCK_ID_LIST$$$AUTOGEN$$$
  NUMBER_OF_DATA_BLOCKS
} DataBlockID_t;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

$$$AUTOGEN$$$INIT_DEFAULTS_DECLARATION$$$AUTOGEN$$$

$$$AUTOGEN$$$INIT_LIMITS_DECLARATION$$$AUTOGEN$$$

/*************************************************************************************/
/* PUBLIC EXTERN CONSTANTS                                                           */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr float    AUTOGEN_ATAMS_VERSION_NUMBER = $$$AUTOGEN$$$VERSION_NUMBER$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_DAY          = $$$AUTOGEN$$$GENERATION_DAY$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MONTH        = $$$AUTOGEN$$$GENERATION_MONTH$$$AUTOGEN$$$;
inline constexpr uint16_t AUTOGEN_MAP_GEN_YEAR         = $$$AUTOGEN$$$GENERATION_YEAR$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_HOUR         = $$$AUTOGEN$$$GENERATION_HOUR$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_MINUTE       = $$$AUTOGEN$$$GENERATION_MINUTE$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_GEN_SECOND       = $$$AUTOGEN$$$GENERATION_SECOND$$$AUTOGEN$$$;
inline constexpr uint8_t  AUTOGEN_MAP_NUMBER_OF_BLOCKS = $$$AUTOGEN$$$NUMBER_OF_BLOCKS$$$AUTOGEN$$$;
inline constexpr uint32_t AUTOGEN_MAP_CHECKSUM         = $$$AUTOGEN$$$GENERATION_CHECKSUM$$$AUTOGEN$$$;

/*************************************************************************************/
/* DATA BLOCK DEFINITIONS                                                            */
/*************************************************************************************/

$$$AUTOGEN$$$DATA_BLOCK_DEFINITIONS$$$AUTOGEN$$$

} } /* End Namespace - $$$AUTOGEN$$$FRAMEWORK_NAME$$$AUTOGEN$$$::Map$$$AUTOGEN$$$MAP_NAME_CAMEL$$$AUTOGEN$$$ */

/**
  * @}End of File
  */
