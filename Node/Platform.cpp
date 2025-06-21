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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "Platform.hpp"
#include "../../../PeripheralLayer/SerialPort.hpp"
#include "main.h"
#include "string.h"

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

void setReceiveCallback(CommsReceiveCallback_t receiveCallback)
{
  _meshPort.setReceiveCallback(receiveCallback);
  while (_meshPort.beginReceive() != SerialPort::ERROR_NONE);
}

void update(void)
{

}

bool transmitBuffer(CommsChannel_t commsChannel, uint8_t *buffer, uint16_t length)
{
  HAL_GPIO_WritePin(RS485_DE_GPIO_Port, RS485_DE_Pin, GPIO_PIN_SET);               // @suppress("C-Style cast instead of C++ cast")
  HAL_GPIO_WritePin(LED_RS485_GREEN_GPIO_Port, LED_RS485_GREEN_Pin, GPIO_PIN_SET); // @suppress("C-Style cast instead of C++ cast")

  SerialPort::Error_t transmitResult = _meshPort.transmitBuffer(buffer, length);

  if (transmitResult != SerialPort::ERROR_NONE) return (false);

  return (true);
}

uint32_t getMillis(void)
{
  return (HAL_GetTick());
}

void acquireVarStorageLock(void)
{
  HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void releaseVarStorageLock(void)
{
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void acquireCommsBufferLock(CommsChannel_t channelToLock)
{
  static_cast<void>(channelToLock);
  HAL_NVIC_DisableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
}

void releaseCommsBufferLock(CommsChannel_t channelToLock)
{
  static_cast<void>(channelToLock);
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  HAL_NVIC_DisableIRQ(DMA1_Stream0_IRQn);
}

void waitOnCommsBufferSemaphore(uint32_t timeoutMilliseconds)
{
  static_cast<void>(timeoutMilliseconds);
}

void signalCommsBufferSemaphore(void)
{

}

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

bool readFromNVM(const uint32_t readIndex, uint8_t * outputPtr, const uint32_t readLength)
{
  if (outputPtr == nullptr) return (false);

  memcpy(outputPtr, reinterpret_cast<uint8_t*>(0x081E0000 + readIndex), readLength);

  return (HAL_FLASH_GetError() == HAL_OK);
}

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

bool enterConfigurationState(void)
{
  return (true);
}

void exitConfigurationState(void)
{

}

void setBitrate(Atams::BitrateOption_t bitrateOption)
{
  static_cast<void>(bitrateOption);
}

Atams::Error_t resetNode(void)
{
  HAL_NVIC_SystemReset();

  return (Atams::ERROR_PLATFORM);
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
