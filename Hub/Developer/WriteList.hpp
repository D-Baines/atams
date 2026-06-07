/**
  ******************************************************************************
  * @file    WriteList.hpp
  *
  * @author  D. Baines
  *
  * @brief   Compact list tracking active write-stream datagrams in a Node request packet.
  *
  * @details Defines the WriteList class, which maintains a fixed-size array of
  *          WriteConfig_t entries. Each entry records a variable ID, its byte offset
  *          within the request packet buffer, and its data length. This allows the Hub
  *          to update write payload bytes in place each Bus update cycle without
  *          rescanning the full request packet. The maximum list size is bounded by
  *          the number of write datagrams that fit within one packet.
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

#include "FramingConstants.hpp"
#include "../../Shared/AtamsTypedefs.hpp"

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

  /*-- PUBLIC CONSTANTS ---------------*/

  static constexpr uint16_t LIST_MAX_LENGTH {Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING / Atams::MINIMUM_SIZE_WRITE_DATAGRAM};

  /*-- PUBLIC TYPEDEFS ----------------*/

  struct WriteConfig_t
  {
    uint16_t varID;
    uint16_t requestPacketIndex;
    uint8_t  dataLength;
  };

  enum Error_t: uint8_t
  {
    ERROR_NONE      = 0U,
    ERROR_INDEX_OOR = 1U,
    ERROR_FULL      = 2U,
  };
  
  struct ConfigReturn_t
  {
    Error_t       status;
    WriteConfig_t writeConfig;
  };

  struct SearchResult_t
  {
    bool     configFound;
    uint16_t configIndex;
  };

  /*-- PUBLIC FUNCTION DEFINITIONS ----*/

  WriteList(void);

  void reset(void);

  SearchResult_t findConfig(const uint16_t varID);

  Error_t addConfig(const WriteConfig_t newWriteConfig);

  void removeConfigIfFound(const uint16_t varID);

  void updateIndexes(const uint16_t referenceIndex, const int16_t shiftLength);

  ConfigReturn_t getConfigAtIndex(const uint16_t configIndex);

  uint16_t getConfigCount(void);

  private:

  /*-- PRIVATE CONSTANTS --------------*/

  /*-- PRIVATE VARIABLES --------------*/

  WriteConfig_t _configList[LIST_MAX_LENGTH];
  uint16_t      _configCount = 0U;

  /*-- PRIVATE FUNCTION DEFINITIONS ---*/

  void removeConfigAtIndex(const uint16_t configIndex);
  
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */
