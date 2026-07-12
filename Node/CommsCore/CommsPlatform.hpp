/**
  ******************************************************************************
  * @file    CommsPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform interface declarations for the Atams Node Comms Core.
  *
  * @details Defines the platform abstraction layer (PAL) for the Atams Node 
  *          Comms Core. Contains the constants, types, and function 
  *          declarations that must be implemented by the user to port the Atams 
  *          Node library to a specific hardware platform. The interface covers
  *          communications peripheral management, concurrency primitives, 
  *          non-volatile memory access, and system control.
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

#include "../../Shared/AtamsTypedefs.hpp"
#include "../SharedPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/**
 * @brief The maximum packet size across all of the Node's communications peripherals.
 *
 * @details Used to set the size of static request and response buffers.
 *
 */
constexpr uint16_t MAX_BUS_PACKET_SIZE {64U};

/**
 * @brief The circular buffer size used for receiving Atams packets.
 *        A minimum size of 3 * Atams::MAX_BUS_PACKET_SIZE is recommended.
 */
constexpr uint16_t CIRCULAR_BUFFER_SIZE {1024U};

/**
 * @brief The size of non-volatile memory (NVM) reserved for Atams use.
 *
 * @details The size required depends on the number of vars selected for non-volatile
 *          storage in the Atams Memory Map. Once a Memory Map has been generated, set this to
 *          Atams::Map<YourMapName>::REQUIRED_NVM_SIZE + Platform::NVM_UNIT_SIZE - autogen
 *          computes the exact minimum for the Memory Map but REQUIRED_NVM_SIZE itself does
 *          not include the trailing NVM_UNIT_SIZE write-unit headroom below. There is no need 
 *          to recompute the formula below by hand or keep it in sync as vars are added/removed. 
 *      
 *          The formula (for reference only) is:
 *
 *          (noOfNVMVars * (sizeOfEachVar + entryHeaderSize)) + headerSize + footerSize + NVM_UNIT_SIZE
 *
 *          Where:
 *
 *          entryHeaderSize = Atams::NVM_VAR_ENTRY_HEADER_SIZE (per NVMStorage var)
 *          headerSize      = Atams::NVM_HEADER_SIZE
 *          footerSize      = Atams::NVM_FOOTER_SIZE
 */
constexpr uint32_t NVM_STORAGE_SIZE {1024U};

/** 
 * @brief   The size of each NVM write unit in bytes.
 *
 * @details This value defines the size of each write operation to non-volatile memory.
 *          Some platforms can only write to non-volatile memory in fixed size units.
 *          The user should set this to an appropriate value for their platform.
 *          The provided data buffer in @ref Platform::writeToNVM will always be of
 *          size Platform::NVM_UNIT_SIZE.
 */
constexpr uint32_t NVM_UNIT_SIZE {32U};

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** 
 * @brief   Identifiers for the Node devices communication peripherals.
 *
 * @details Used to identify which communication peripheral is being referenced
 *          on multi-peripheral devices. Users should fill in this enum list
 *          if multiple comms peripherals are used, or leave it as is on Nodes
 *          with only one peripheral.
 */
enum CommsPeripheralID_t : uint8_t
{
  COMMS_DEFAULT = 0U,

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

bool writeToNVM(const uint32_t writeIndex, const uint8_t (&nvmUnit)[Platform::NVM_UNIT_SIZE]);

bool enterConfigurationState(void);

void exitConfigurationState(void);

void setBitrate(const Atams::BitrateOption_t bitrateOption);

Atams::Error_t resetNode(void);


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
