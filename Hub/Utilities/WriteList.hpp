/**
  ******************************************************************************
  * @file    WriteList.hpp
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
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

#pragma once

/***************************************************************************/
/* INCLUDES                                                                */
/***************************************************************************/

#include <stdint.h>
#include <stdio.h>

/***************************************************************************/
/* NAMESPACE                                                               */
/***************************************************************************/

namespace Atams
{

/***************************************************************************/
/* CLASS DEFINITIONS                                                       */
/***************************************************************************/

class WriteList
{

  public:

  /*-- PUBLIC TYPEDEFS ----------------*/

  typedef struct WriteConfig_t
  {
    uint8_t  blockID;
    uint16_t memberID;
    uint16_t meshPacketDataIndex;
    uint8_t  dataLength;
  
  } WriteConfig_t;

  typedef enum: uint8_t
  {
    ERROR_NONE      = 0U,
    ERROR_INDEX_OOR = 1U,
    ERROR_FULL      = 2U,
  } Error_t;
  
  typedef struct ListReturn_t
  {
    Error_t       status;
    WriteConfig_t writeConfig;
  } Return_t;

  typedef struct SearchResult_t
  {
    bool     configFound;
    uint16_t configIndex;
  } SearchResult_t;

  /*-- PUBLIC FUNCTION DEFINITIONS ----*/

  WriteList(void);

  SearchResult_t findConfig(WriteConfig_t configToFind);

  Error_t addConfig(WriteConfig_t newWriteConfig);

  void removeConfig(WriteConfig_t configToRemove);

  void removeNode(uint8_t nodeID);

  void updateIndexes(uint16_t referenceIndex, int8_t shiftLength);

  Return_t getConfigAtIndex(uint16_t configIndex);

  uint16_t getConfigCount(void);

  private:

  /*-- PRIVATE CONSTANTS --------------*/

  static const uint16_t LIST_MAX_LENGTH = 1024U;

  /*-- PRIVATE VARIABLES --------------*/

  WriteConfig_t _configList[LIST_MAX_LENGTH];
  uint16_t      _configCount = 0U;


  /*-- PRIVATE FUNCTION DEFINITIONS ---*/

  void removeConfigAtIndex(uint16_t configIndex);
  
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */
