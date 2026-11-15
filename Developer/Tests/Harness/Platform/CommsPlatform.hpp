/**
  ******************************************************************************
  * @file    CommsPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Harness Platform layer for the Atams Node Comms Core.
  *
  * @details Connects the Node to an AtamsTestHarness::Router device. Bytes pass through
  *          untouched. NVM is a RAM array that behaves like MCU flash. All state is file
  *          scope, so the test runner calls resetForNextTest() before every test.
  *
  *          Built into each Node library copy, so it ends up in that copy's namespace.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "Shared/AtamsTypedefs.hpp"
#include "SharedPlatform.hpp"

#include "Harness/Router.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NodePlatform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** @brief The minimum, so responses split as often as possible */
constexpr uint16_t MAX_BUS_PACKET_SIZE {Atams::MINIMUM_MAXIMUM_BUS_PACKET_SIZE};

/** @brief The recommended minimum of 3 * MAX_BUS_PACKET_SIZE */
constexpr uint16_t CIRCULAR_BUFFER_SIZE {3U * MAX_BUS_PACKET_SIZE};

/** @brief The size of each NVM write unit in bytes */
constexpr uint32_t NVM_UNIT_SIZE {32U};

/** @brief A platform-maximum size, independent of the test Memory Map (checked in NodeTestMap.cpp) */
constexpr uint32_t NVM_STORAGE_SIZE {1024U};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** @brief The harness Node has one communications peripheral */
enum CommsPeripheralID_t : uint8_t
{
  COMMS_DEFAULT = 0U,

  NUMBER_OF_COMMS_PERIPHERALS
};

/** @brief Callback function pointer typedef for received bytes */
typedef void (*CommsReceiveCallback_t)(const CommsPeripheralID_t peripheralID,
                                       const uint8_t * const     rxBufferPtr,
                                       const uint16_t            rxBufferLength);

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint32_t getMillis(void);

void acquireVarStorageLock(void);

void releaseVarStorageLock(void);

bool beginReceive(CommsReceiveCallback_t receiveCallback);

void stopReceive(void);

void update(void);

bool transmitReady(const CommsPeripheralID_t peripheralID);

bool transmitBuffer(const CommsPeripheralID_t peripheralID, uint8_t * bufferPtr, const uint16_t length);

void acquireCommsBufferLock(const CommsPeripheralID_t peripheralToLock);

void releaseCommsBufferLock(const CommsPeripheralID_t peripheralToUnlock);

void acquireWaitOnReceiveSempahore(const uint32_t timeoutInMilliseconds);

void releaseWaitOnReceiveSemaphore(void);

bool eraseNVM(void);

bool readFromNVM(const uint32_t readIndex, uint8_t * outputPtr, const uint32_t readLength);

bool writeToNVM(const uint32_t writeIndex, const uint8_t (&nvmUnit)[NodePlatform::NVM_UNIT_SIZE]);

bool enterConfigurationState(void);

void exitConfigurationState(void);

void setBitrate(const Atams::BitrateOption_t bitrateOption);

Atams::Error_t resetNode(void);

/*************************************************************************************/
/* HARNESS FUNCTION DECLARATIONS                                                     */
/*************************************************************************************/

void attachToRouter(AtamsTestHarness::Router &router, const AtamsTestHarness::DeviceID_t device);

void resetForNextTest(void);


} } /* End Namespace - Atams::NodePlatform */


/**
  * @}End of File
  */
