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
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static SerialPort _meshPort(SerialPort::PORT_ID_MESH);

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/*
*  @brief Solves non-linear equation with Newton method.
*
*  @details
*   Solves a non-linear equation using the Newton method which uses the
*   function and its derivate function for finding a suitable approximation
*   to the equation root.
*
*  @warning  Throws NonCoverge exception when the root is not found.
*
*  @param fun  Non-linear function f(x)
*  @param dfun Derivative of non-linear function df(x) = d/dx f(x)
*  @param x0   Initial guess
*  @param eps  Tolerance for stopping criteria.
*
*  @return     Equation result object containing result and error code.
*
*  @todo  Implement unit test with lots of test cases.
*
*  @note     The function f(x) must be continues and differentiable.
*/

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
 * @brief
 *
 * @details
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 *          ATAMS PLATFORM REQUIREMENT - DUAL-CORE
 */
void acquireVarStorageLock(void)
{
  HAL_HSEM_FastTake(0U);
}

/**
 * @brief
 *
 * @details
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
 * @brief
 *
 * @details Users can optionally choose to start peripheral reception in this function
 *
 * @return  None
 *
 * @param   receiveCallback Function pointer to the function to be called when new bytes
 *                          are ready to be passed to Atams
 *
 * @see     CommsPlatform.hpp -> PUBLIC TYPEDEFS -> CommsReceiveCallback_t
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void setReceiveCallback(CommsReceiveCallback_t receiveCallback)
{
  _meshPort.setReceiveCallback(receiveCallback);
  while (_meshPort.beginReceive() != SerialPort::ERROR_NONE);
}

/**
 * @brief
 *
 * @details Users can use this function to poll communications peripherals, check for
 *          peripheral errors, and/or restart peripheral reception if required.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - POLLING
 */
void update(void)
{

}

/**
 * @brief
 *
 * @details
 *
 * @param   commsChannel The ID of the communications peripheral to use for transmission
 * @param   buffer       Pointer to the buffer holding the bytes to be transmitted
 * @param   length       Number of bytes to be transmitted
 *
 * @return  Transmission status.
 *          true:  Transmission successfully started or all bytes successfully transmitted.
 *          false: Transmission error occurred.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool transmitBuffer(CommsChannel_t commsChannel, uint8_t *buffer, uint16_t length)
{
  static_cast<void>(commsChannel);

  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);               // @suppress("C-Style cast instead of C++ cast")
  HAL_GPIO_WritePin(LED_RS485_GREEN_GPIO_Port, LED_RS485_GREEN_Pin, GPIO_PIN_SET); // @suppress("C-Style cast instead of C++ cast")

  SerialPort::Error_t transmitResult = _meshPort.transmitBuffer(buffer, length);

  if (transmitResult != SerialPort::ERROR_NONE) return (false);

  return (true);
}

/**
 * @brief
 *
 * @details
 *
 * @param   channelToLock The ID of the communications peripheral to lock
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void acquireCommsBufferLock(CommsChannel_t channelToLock)
{
  static_cast<void>(channelToLock);
  //HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief
 *
 * @details
 *
 * @param   channelToUnlock The ID of the communications peripheral to unlock
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void releaseCommsBufferLock(CommsChannel_t channelToUnlock)
{
  static_cast<void>(channelToLock);
  //HAL_NVIC_EnableIRQ(USART2_IRQn);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
}

/**
 * @brief
 *
 * @details If Atams::updateCommsBlocking() is used, this function will be called to
 *          block the update thread while waiting to receive new bytes. A timeout is required to ensure that
 *          the Atams communications watchdog continues to be updated.
 *
 * @param   timeoutInMilliseconds Maximum time to wait while attempting to acquire
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREAD + EVENT DRIVEN COMMS
 */
void waitOnCommsBufferSemaphore(uint32_t timeoutMilliseconds)
{
  static_cast<void>(timeoutMilliseconds);
}

/**
 * @brief
 *
 * @details If Atams::updateCommsBlocking() is used, this function will be called to
 *          wake up the thread on reception of new bytes.
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREAD + EVENT DRIVEN COMMS
 */
void signalCommsBufferSemaphore(void)
{

}

/**
 * @brief
 *
 * @details Erases the non-volatile memory space dedicated to Atams.
 *          Must erase at least the number of bytes equal to Platform::NVM_STORAGE_SIZE
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
  uint32_t               sectorError = 0U;

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
 * @brief
 *
 * @details
 *
 * @param  readIndex  Index into the non-volatile memory space dedicated to Atams to be read from.
 * @param  outputPtr  Pointer to the buffer to transfer the read bytes to from non-volatile memory.
 * @param  readLength Number of bytes to transfer to the buffer from non-volatile memory.
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
 *  @brief
 *
 *  @details
 *
 *  @param  writeIndex Index into the non-volatile memory space dedicated to Atams.
 *  @param  nvmUnit    Reference to an array of size Platform::NVM_UNIT_SIZE to be written to
 *                     non-volatile memory at the writeIndex.
 *
 *  @return Write status.
 *          true:  Write completed successfully.
 *          false: Write failure.
 *
 *  @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
bool writeToNVM(const uint32_t writeIndex, uint8_t (&nvmUnit)[Platform::NVM_UNIT_SIZE])
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
 * @brief
 *
 * @details
 *
 * @return  Configuration entry decision.
 *          true:  Configuration entry accepted - safe to call blocking storage functions or reset system
 *          false: Configuration entry declined - not safe to call blocking storage functions or reset system
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
 * @brief   Notifies the user that the configuration state has been exited
 *
 * @details
 *
 * @return  None
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void exitConfigurationState(void)
{

}


/**
 * @brief
 *
 * @details
 *
 * @return  bitrateOption User defined bitrate option to be asserted.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void setBitrate(Atams::BitrateOption_t bitrateOption)
{
  static_cast<void>(bitrateOption);
}

/**
 * @brief   Reset the system
 *
 * @details Function should never exit if system resets successfully.
 *          This function will only be called if Atams has first entered the configuration state.
 *
 * @return  None.
 *
 * @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
void resetNode(void)
{
  HAL_NVIC_SystemReset();

  return (false);
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
