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
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

/* Dual core shared memory space requirement: 2 + (4 * Platform::NODE_NUMBER_OF_VARS) */
#define ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NODE_NUMBER_OF_VARS = 240U;  /* Must be <= MAX_NUMBER_OF_DATA_MEMBERS */
inline constexpr uint16_t COMMS_BUFFER_SIZE   = 512U;
inline constexpr uint32_t NVM_STORAGE_SIZE    = 1024U;

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

typedef enum: uint8_t
{
  COMMS_CHANNEL_DEFAULT = 0,

  NUMBER_OF_COMMS_CHANNELS
} CommsChannel_t;

/* DO NOT CHANGE THIS TYPEDEF */
typedef void (*CommsReceiveCallback_t)(const Platform::CommsChannel_t commsChannel,
                                             uint8_t                 *rxBufferPtr,
                                       const uint16_t                 rxBufferLength);

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

void setReceiveCallback(CommsReceiveCallback_t receiveCallback);

void update(void);

bool transmitBuffer(CommsChannel_t commsChannel, uint8_t *buffer, uint16_t length);

uint32_t getMillis(void);

void acquireVarStorageLock(void);

void releaseVarStorageLock(void);

void acquireCommsBufferLock(CommsChannel_t channelToLock);

void releaseCommsBufferLock(CommsChannel_t channelToLock);

void waitOnCommsBufferSemaphore(uint32_t timeoutMilliseconds);

void signalCommsBufferSemaphore(void);

bool readFromNVM(uint32_t startIndex, uint32_t size, uint8_t * const outputPtr);

bool writeToNVM(uint32_t startIndex, uint32_t size, const uint8_t * const inputPtr);

bool enterConfigurationState(void);

void exitConfigurationState(void);

void setBitrate(Atams::BitrateOption_t bitrateOption);

Atams::Error_t resetNode(void);


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
