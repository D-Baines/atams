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
  HAL_NVIC_DisableIRQ(UART4_IRQn);
}

void releaseCommsBufferLock(CommsChannel_t channelToLock)
{
  static_cast<void>(channelToLock);
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}

void waitOnCommsBufferSemaphore(uint32_t timeoutMilliseconds)
{
  static_cast<void>(timeoutMilliseconds);
}

void signalCommsBufferSemaphore(void)
{

}

bool readFromNVM(uint32_t startIndex, uint32_t size, uint8_t * const outputPtr)
{
  static_cast<void>(startIndex);
  static_cast<void>(size);
  static_cast<void>(outputPtr);
  return (false);
}

bool writeToNVM(uint32_t startIndex, uint32_t size, const uint8_t * const inputPtr)
{
  static_cast<void>(startIndex);
  static_cast<void>(size);
  static_cast<void>(inputPtr);
  return (false);
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

void resetNode(void)
{
  HAL_NVIC_SystemReset();
}

} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
