/**
  ******************************************************************************
  * @file    FramingConstants.hpp
  *
  * @author  D. Baines
  *
  * @brief   COBS framing size constants derived from the Hub platform packet size.
  *
  * @details Exposes compile-time constants defining the COBS overhead and the maximum
  *          unframed payload size for Hub bus packets. Values are derived from
  *          Platform::MAX_BUS_PACKET_SIZE defined in Hub/Platform.hpp. 
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint16_t COBS_MAX_DATA_PER_CODE          {254U};
constexpr uint16_t COBS_MAX_OVERHEAD               {(MAX_BUS_PACKET_SIZE + (COBS_MAX_DATA_PER_CODE - 1U)) / COBS_MAX_DATA_PER_CODE};
constexpr uint16_t MAX_BUS_PACKET_SIZE_PRE_FRAMING {MAX_BUS_PACKET_SIZE - COBS_MAX_OVERHEAD};

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */


