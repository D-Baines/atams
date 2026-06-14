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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
constexpr uint16_t COBS_TERMINATOR_SIZE            {1U};
constexpr uint16_t COBS_MAX_CODE_BYTES             {(MAX_BUS_PACKET_SIZE + (COBS_MAX_DATA_PER_CODE - 1U)) / COBS_MAX_DATA_PER_CODE};
constexpr uint16_t COBS_MAX_OVERHEAD               {COBS_MAX_CODE_BYTES + COBS_TERMINATOR_SIZE};
constexpr uint16_t MAX_BUS_PACKET_SIZE_PRE_FRAMING {MAX_BUS_PACKET_SIZE - COBS_MAX_OVERHEAD};

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */


