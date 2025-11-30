/**
  ******************************************************************************
  * @file    CommsPlatform.hpp
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

#include "../../Shared/AtamsTypedefs.hpp"
#include "../SharedPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint16_t MAX_BUS_PACKET_SIZE  {64U};
constexpr uint16_t CIRCULAR_BUFFER_SIZE {1024U};
constexpr uint32_t NVM_STORAGE_SIZE     {1024U};
constexpr uint8_t  NVM_UNIT_SIZE        {32U};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

enum CommsPeripheralID_t : uint8_t
{
  COMMS_DEFAULT = 0,

  NUMBER_OF_COMMS_PERIPHERALS
};

typedef void (*CommsReceiveCallback_t)(const CommsPeripheralID_t peripheralID,
                                             uint8_t            *rxBufferPtr,
                                       const uint16_t            rxBufferLength);

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

/**
 * @brief  Get system time in milliseconds since startup.
 *
 * @return System time in milliseconds since startup.
 *
 * @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
uint32_t getMillis(void);

void acquireVarStorageLock(void);

void releaseVarStorageLock(void);

void beginReceive(CommsReceiveCallback_t receiveCallback);

void stopReceive(void);

void update(void);

bool transmitReady(CommsPeripheralID_t peripheralID);

bool transmitBuffer(CommsPeripheralID_t peripheralID, uint8_t *buffer, uint16_t length);

void acquireCommsBufferLock(CommsPeripheralID_t peripheralToLock);

void releaseCommsBufferLock(CommsPeripheralID_t peripheralToUnlock);

void acquireWaitOnReceiveSempahore(uint32_t timeoutInMilliseconds);

void releaseWaitOnReceiveSemaphore(void);

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
