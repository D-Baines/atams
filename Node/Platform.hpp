/**
  ******************************************************************************
  * @file    Platform.hpp
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../AtamsTypedefs.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef enum: uint8_t
{
  COMMS_CHANNEL_DEFAULT = 0,

  NUMBER_OF_COMMS_CHANNELS
} CommsChannel_t;

typedef void (*CommsReceiveCallback_t)(const Platform::CommsChannel_t commsChannel,
                                             uint8_t                 *rxBufferPtr,
                                       const uint16_t                 rxBufferLength);

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NODE_NUMBER_OF_DATA_MEMBERS = 50U;  /* Must be <= MAX_NUMBER_OF_DATA_MEMBERS */
inline constexpr uint16_t NODE_NUMBER_OF_DATA_BLOCKS  = 3U;   /* Must be <= MAX_NUMBER_OF_DATA_BLOCKS  */
inline constexpr uint16_t COMMS_BUFFER_SIZE           = 512U;



/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

void     setReceiveCallback(CommsReceiveCallback_t receiveCallback);

void     update(void);

Error_t  transmitBuffer(uint8_t *buffer, uint16_t length);

uint32_t getMillis(void);

void acquireMemoryLock(void);

void releaseMemoryLock(void);

void acquireCommsBufferLock(CommsChannel_t channelToLock);

void releaseCommsBufferLock(CommsChannel_t channelToLock);


} } /* End Atams::Platform Namespace */


/**
  * @}End of File
  */
