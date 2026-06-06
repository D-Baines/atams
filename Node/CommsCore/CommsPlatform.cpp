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

/*************************************************************************************/
/* LIBRARY INCLUDES                                                                  */
/*************************************************************************************/

#include "CommsPlatform.hpp"

/*************************************************************************************/
/* USER INCLUDES                                                                     */
/*************************************************************************************/

#include "main.h"
#include "string.h"
#include "PeripheralLayer/SerialPort.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static SerialPort s_serialPort(SerialPort::PORT_ID_MESH);

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief  Get system time in milliseconds since startup.
 *
 * @return System time in milliseconds since startup.
 *
 * @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
uint32_t getMillis(void)
{
  return (HAL_GetTick());
}

/**
 * @brief   Acquire the lock protecting the Node variable storage from invalid concurrent access.
 *
 * @details This function will be called before Node variable storage access. The variable storage
 *          may be accessed from multiple threads or cores depending on the user's platform. The user 
 *          must ensure that the lock is held until Platform::releaseVarStorageLock() is called.
 *
 *          For multi-threaded platforms, the user should use a mutex or similar mechanism. For dual-core 
 *          platforms, the user should use a hardware semaphore or similar mechanism. A combination of both
 *          may be required for multi-threaded, dual-core platforms.
 *         
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 *          ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
void acquireVarStorageLock(void)
{
  while (HAL_HSEM_FastTake(0U) != HAL_OK)
  {
    /* Wait */
  };
}

/**
 * @brief   Release the lock protecting the Node variable storage from invalid concurrent access.
 *
 * @details This function will be called after Node variable storage access. The user must 
 *          release or unlock the mechanism locked in Platform::acquireVarStorageLock().
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 *          ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
void releaseVarStorageLock(void)
{
  HAL_HSEM_Release(0U, 0);
}

/**
 * @brief   Start reception on all peripherals and register the receive callback function.
 *
 * @return  Receive initiation status.
 *          true:  Peripheral reception started successfully.
 *          false: Peripheral fault occurred when attempting to start reception.
 *
 * @param   receiveCallback Function pointer to the function to be called when new bytes
 *                          are ready to be passed to Atams.
 *
 * @see     CommsPlatform.hpp -> PUBLIC TYPEDEFS -> CommsReceiveCallback_t
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool beginReceive(CommsReceiveCallback_t receiveCallback)
{
  s_serialPort.setReceiveCallback(receiveCallback);


  HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_RESET);

  return (s_serialPort.beginReceive() == SerialPort::ERROR_NONE);
}

/**
 * @brief  Stop reception on all peripherals.
 *
 * @return None
 *
 * @note   ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void stopReceive(void)
{
  s_serialPort.stopReceive();
  HAL_GPIO_WritePin(RS485_RE_GPIO_Port, RS485_RE_Pin, GPIO_PIN_SET);
}

/**
 * @brief   User update function called at regular intervals from Atams::updateCommsPolling().
 *
 * @details Users can use this function to poll communications peripherals, check for
 *          peripheral errors, and/or restart peripheral reception if required.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - POLLING COMMS
 */
void update(void)
{
  if (s_serialPort.getError() != SerialPort::ERROR_NONE)
  {
    while (s_serialPort.beginReceive() != SerialPort::ERROR_NONE)
    {
      s_serialPort.stopReceive();
    }
  }
}

/**
 * @brief  Check if the specified communications peripheral is ready to transmit new bytes.
 *
 * @param  peripheralID The ID of the communications peripheral to check.
 *
 * @return Transmission readiness.
 *         true:  Peripheral ready to accept new bytes for transmission.
 *         false: Peripheral not ready to accept new bytes for transmission.
 *
 * @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
bool transmitReady(const CommsPeripheralID_t peripheralID)
{
  static_cast<void>(peripheralID);

  return (s_serialPort.transmitReady());
}

/**
 * @brief   Transmit a buffer of bytes via the specified communications peripheral.
 *
 * @details Users can choose to implement this function as a blocking or non-blocking.
 *          If implemented as non-blocking for event-driven comms, the function must ensure that
 *          the transmission is started before returning true. If implemented as blocking for polling
 *          comms, the function must ensure that all bytes are transmitted before returning true.
 *
 * @param   peripheralID The ID of the communications peripheral to use for transmission.
 * @param   buffer       Pointer to the buffer holding the bytes to be transmitted.
 * @param   length       Number of bytes to be transmitted.
 *
 * @return  Transmission status.
 *          true:  Transmission successfully started or all bytes successfully transmitted.
 *          false: Transmission error occurred.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool transmitBuffer(const CommsPeripheralID_t peripheralID, uint8_t * buffer, const uint16_t length)
{
  static_cast<void>(peripheralID);

  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);               // @suppress("C-Style cast instead of C++ cast")
  HAL_GPIO_WritePin(LED_RS485_GREEN_GPIO_Port, LED_RS485_GREEN_Pin, GPIO_PIN_SET); // @suppress("C-Style cast instead of C++ cast")

  SerialPort::Error_t transmitResult = s_serialPort.transmitBuffer(buffer, length);

  if (transmitResult != SerialPort::ERROR_NONE) return (false);

  return (true);
}

/**
 * @brief   Acquire the lock protecting the circular buffer from concurrent access.
 *
 * @details This function will be called before Atams circular buffer access. The circular buffers
 *          are accessed from the Atams comms update functions and from the CommsReceiveCallback_t function
 *          passed to Platform::beginReceive. The user must ensure that the lock is held until
 *          Platform::releaseCommsBufferLock() is called. If the CommsReceiveCallback_t function is called
 *          from an interrupt, the user should disable the interrupt associated with the provided
 *          comms peripheral ID in this function.
 *
 * @param   peripheralToLock The ID of the communications peripheral to lock.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void acquireCommsBufferLock(const CommsPeripheralID_t peripheralToLock)
{
  static_cast<void>(peripheralToLock);
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief   Release the lock protecting the circular buffer from concurrent access.
 *
 * @details This function will be called after Atams circular buffer access. The circular buffers
 *          are accessed from the Atams comms update functions and from the CommsReceiveCallback_t function
 *          passed to Platform::beginReceive. If an interrupt was disabled in
 *          Platform::acquireCommsBufferLock(), the user should re-enable the same interrupt here.
 *
 * @param   peripheralToUnlock The ID of the communications peripheral to unlock.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void releaseCommsBufferLock(const CommsPeripheralID_t peripheralToUnlock)
{
  static_cast<void>(peripheralToUnlock);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
}

/**
 * @brief   Block the calling thread using a semaphore or similar mechanism.
 *
 * @details If Atams::updateCommsBlocking() is used, this function will be called to
 *          block the update thread while waiting to receive new bytes. A timeout is required
 *          to ensure that the Atams communications watchdog can safely detect a communications
 *          dropout.
 *
 * @param   timeoutInMilliseconds Maximum time to wait while attempting to acquire.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREAD + EVENT DRIVEN COMMS
 */
void acquireWaitOnReceiveSempahore(const uint32_t timeoutMilliseconds)
{
  static_cast<void>(timeoutMilliseconds);
}

/**
 * @brief   Wake up the thread blocked in Platform::waitOnCommsBufferSemaphore().
 *
 * @details If Atams::updateCommsBlocking() is used, this function will be called to
 *          wake up the thread on reception of new bytes.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREAD + EVENT DRIVEN COMMS
 */
void releaseWaitOnReceiveSemaphore(void)
{

}

/**
 * @brief   Erase the full non-volatile memory space dedicated to Atams.
 *
 * @details Must erase at least the number of bytes equal to Platform::NVM_STORAGE_SIZE.
 *
 * @see     CommsPlatform.hpp -> PUBLIC CONSTANTS -> NVM_STORAGE_SIZE
 *
 * @return  Erase completion status.
 *          true:  Erase completed successfully.
 *          false: Erase failure.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool eraseNVM(void)
{
  FLASH_EraseInitTypeDef eraseInitStruct;
  uint32_t               sectorError {0U};

  /* Fill EraseInit structure*/
  eraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
  eraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
  eraseInitStruct.Banks         = FLASH_BANK_2;
  eraseInitStruct.Sector        = FLASH_SECTOR_7;
  eraseInitStruct.NbSectors     = 1U;

  __HAL_FLASH_CLEAR_FLAG_BANK2(FLASH_FLAG_ALL_ERRORS_BANK2);

  if (HAL_FLASH_Unlock() != HAL_OK) return (false);

  if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK)
  {
    HAL_FLASH_Lock();
    return (false);
  }

  if (HAL_FLASH_Lock() != HAL_OK) return (false);

  return (true);
}

/**
 * @brief  Copy bytes from non-volatile memory into a provided buffer.
 *
 * @param  readIndex  The index into the non-volatile memory from which to start reading.
 * @param  outputPtr  Pointer to the buffer the bytes will be copied into.
 * @param  readLength Number of bytes to copy to the buffer from non-volatile memory.
 *
 * @return Read status.
 *         true:  Read completed successfully.
 *         false: Read failure.
 *
 * @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
bool readFromNVM(const uint32_t readIndex, uint8_t * outputPtr, const uint32_t readLength)
{
  if (outputPtr == nullptr) return (false);

  memcpy(outputPtr, reinterpret_cast<uint8_t*>(0x081E0000 + readIndex), readLength);

  return (HAL_FLASH_GetError() == HAL_OK);
}

/**
 *  @brief   Write a unit of data to non-volatile memory.
 *
 *  @details Some platforms can only write to non-volatile memory in fixed size units.
 *           The size of a unit is defined by Platform::NVM_UNIT_SIZE. The user should set
 *           this to an appropriate value for their platform. The provided data buffer will
 *           always be of size Platform::NVM_UNIT_SIZE.
 *
 *  @param  writeIndex Index into non-volatile memory at which to write the provided data.
 *  @param  nvmUnit    Reference to an array of size Platform::NVM_UNIT_SIZE holding the data to write.
 *
 *  @return Write status.
 *          true:  Write completed successfully.
 *          false: Write failure.
 *
 *  @see    CommsPlatform.hpp -> PUBLIC CONSTANTS -> NVM_UNIT_SIZE
 *
 *  @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
bool writeToNVM(const uint32_t writeIndex, const uint8_t (&nvmUnit)[Platform::NVM_UNIT_SIZE])
{
  if (HAL_FLASH_Unlock() != HAL_OK) return (false);

  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, (0x081E0000 + writeIndex), reinterpret_cast<uint32_t>(nvmUnit)) != HAL_OK)
  {
    HAL_FLASH_Lock();
    return (false);
  }

  if (HAL_FLASH_Lock() != HAL_OK) return (false);

  return (true);
}

/**
 * @brief   Accept or reject entry to the Node configuration state.
 *
 * @details In the configuration state, the Hub can trigger blocking storage processes or trigger resets
 *          on the Node device. If the Node application code is not in a safe state to accept blocking
 *          processes, the function should return false. If the Node application state is in a safe state,
 *          the function should return true. The Node application should remain in a safe state until
 *          exitConfigurationState is called.
 *
 * @return  Configuration entry decision.
 *          true:  Configuration entry accepted - safe to call blocking storage functions or reset system.
 *          false: Configuration entry declined - not safe to call blocking storage functions or reset system.
 *
 * @warning Do not allow configuration entry if it is not safe to call blocking functions or it is not safe
 *          to reset the system. Flash writes in storage functions may block the CPU completely.
 *
 * @warning Ensure the safe state is maintained until exitConfigurationState has been called.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool enterConfigurationState(void)
{
  return (true);
}

/**
 * @brief   Notifies the user that the configuration state has been exited.
 *
 * @details In the configuration state, the Hub can trigger blocking storage processes or trigger resets
 *          on the Node device. Once this function is called, Atams will not trigger any of these processes
 *          again without calling enterConfigurationState().
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void exitConfigurationState(void)
{

}


/**
 * @brief   Set the bitrate for all communications peripherals.
 *
 * @details Function will be called when an Atams Hub device requests a bitrate change
 *          through the configuration state.
 *
 * @param   bitrateOption User defined bitrate option to be asserted.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void setBitrate(Atams::BitrateOption_t bitrateOption)
{
  static_cast<void>(bitrateOption);
}

/**
 * @brief   Reset the system.
 *
 * @details Function will be called when an Atams Hub device requests a node reset.
 *          This function should not return, the system should reset immediately.
 *
 * @return  Reset status - should return Atams::ERROR_PLATFORM if reset fails.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
Atams::Error_t resetNode(void)
{
  HAL_NVIC_SystemReset();

  return (Atams::ERROR_PLATFORM);
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
