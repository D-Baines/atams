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
#include "../Shared/AtamsTypedefs.hpp"

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

inline constexpr uint16_t NODE_NUMBER_OF_VARS  = 100U;  /* Must be <= Atams::MAX_NUMBER_OF_VARS */
inline constexpr uint16_t MAX_BUS_PACKET_SIZE  = 64U;
inline constexpr uint16_t CIRCULAR_BUFFER_SIZE = 1024U;
inline constexpr uint32_t NVM_STORAGE_SIZE     = 1024U;
inline constexpr uint8_t  NVM_UNIT_SIZE        = 32U;

inline constexpr uint16_t COBS_MAX_DATA_PER_CODE          = 254U;
inline constexpr uint16_t COBS_MAX_OVERHEAD               = (MAX_BUS_PACKET_SIZE + (COBS_MAX_DATA_PER_CODE - 1U)) / COBS_MAX_DATA_PER_CODE;
inline constexpr uint16_t MAX_BUS_PACKET_SIZE_PRE_FRAMING = MAX_BUS_PACKET_SIZE - COBS_MAX_OVERHEAD;

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

bool eraseNVM(void);

bool readFromNVM(const uint32_t readIndex, uint8_t * outputPtr, const uint32_t readLength);

bool writeToNVM(uint32_t writeIndex, uint8_t (&nvmUnit)[Platform::NVM_UNIT_SIZE]);

bool enterConfigurationState(void);

void exitConfigurationState(void);

void setBitrate(Atams::BitrateOption_t bitrateOption);

Atams::Error_t resetNode(void);


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
