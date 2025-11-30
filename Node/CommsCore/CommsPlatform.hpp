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

/** @brief The maximum packet size compatible with all of the Node communications peripherals */  
constexpr uint16_t MAX_BUS_PACKET_SIZE {64U};

/**
*  @brief The circular buffer size used for receiving Atams packets.
*         A minimum size of 3 * Atams::MAX_BUS_PACKET_SIZE is recommended.
*/
constexpr uint16_t CIRCULAR_BUFFER_SIZE {1024U};

/** @brief The size of non-volatile memory (NVM) reserved for Atams use.
*
*   @details The size required depends on the number of vars selected for non-volatile
*            storage in the Atams Memory Map. The formula for calculating the minimum
*            required size is:
*            (noOfNVMVars * sizeOfEachVar) + headerSize + footerSize + safetyMargin
*/
constexpr uint32_t NVM_STORAGE_SIZE {1024U};

/** 
*   @brief The size of each NVM write unit in bytes.
* 
*   @details This value defines the size of each write operation to non-volatile memory.
*            Some platforms can only write to non-volatile memory in fixed size units.
*            The user should set this to an appropriate value for their platform.
*            The provided data buffer in @ref Platform::writeToNVM will always be of 
*            size Platform::NVM_UNIT_SIZE.
*/
constexpr uint8_t NVM_UNIT_SIZE {32U};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** 
*   @brief Identifiers for the Node devices communication peripherals.
*
*   @details Used to identify which communication peripheral is being referenced
*            on multi-peripheral devices. Users should extend this enum as required.
*/
enum CommsPeripheralID_t : uint8_t
{
  COMMS_DEFAULT = 0,

  NUMBER_OF_COMMS_PERIPHERALS
};

/**
  * @brief   Callback function pointer typedef for received bytes.
  *
  * @param   peripheralID   The ID of the communications peripheral on which data was received.
  *                         Corresponds to the @ref Platform::CommsPeripheralID_t enum.
  *
  * @param   rxBufferPtr    Pointer to the received data buffer.
  *
  * @param   rxBufferLength Length of the received data buffer.
  *
  * @details The user must call this function when new bytes have been received.
  *          Bytes will be copied from rxBufferPtr into an Atams circular buffer.
  *
  */
typedef void (*CommsReceiveCallback_t)(const CommsPeripheralID_t peripheralID,
                                             uint8_t            *rxBufferPtr,
                                       const uint16_t            rxBufferLength);

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

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
