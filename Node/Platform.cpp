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

Error_t transmitBuffer(uint8_t *buffer, uint16_t length)
{
  SerialPort::Error_t transmitResult = _meshPort.transmitBuffer(buffer, length);

  if (transmitResult != SerialPort::ERROR_NONE) return (ERROR_PLATFORM);

  return (ERROR_NONE);
}

uint32_t getMillis(void)
{
  return (HAL_GetTick());
}

void acquireMemoryLock(void)
{
  HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
  HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
}

void releaseMemoryLock(void)
{
  HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void acquireCommsBufferLock(CommsChannel_t channelToLock)
{
  HAL_NVIC_DisableIRQ(UART4_IRQn);
}

void releaseCommsBufferLock(CommsChannel_t channelToLock)
{
  HAL_NVIC_EnableIRQ(UART4_IRQn);
}


} } /* End Namespace - Atams::Platform */


/**
  * @}End of File
  */
