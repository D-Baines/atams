/**
  ******************************************************************************
  * @file    Platform.cpp
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

#include "Platform.hpp"

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* BusPeripheral - PUBLIC FUNCTION DEFINITIONS                                       */
/*************************************************************************************/

/** @brief Constructor. */
BusPeripheral::BusPeripheral(UserData_t userData) : 
userData_(userData)
{
  
}

/** @brief   Start receiving data on the user comms peripheral. 
 *  @details This function is called from the Atams::Bus class when a bus init or config update process is started.
 *  @return  True if the user comms peripheral was started successfully, false otherwise.
 *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
bool BusPeripheral::startReceive(void)
{
  userData_.serialPort.async_read_some(asio::buffer(rxBuffer_), 
                                       std::bind(&BusPeripheral::rxHandler, 
                                                 this, 
                                                 asio::placeholders::bytes_transferred));
  return (true);
}

/** @brief   Check if the user comms peripheral is ready to transmit data.
 *  @details This function is called from the Atams::Bus class during a bus update cycle.
 *  @return  True if the user comms peripheral is ready to transmit data, false otherwise.
 *  @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool BusPeripheral::transmitReady(void)
{
  return (transmitReady_.load());
}

/** @brief   Transmit bytes using the user comms peripheral.
 *  @details This function is called from the Atams::Bus class during a bus update cycle.
 *  @param   buffer Pointer to the data buffer to transmit.
 *  @param   length Length of the data buffer to transmit.
 *  @return  Transmission status.
 *           true:  Transmission successfully started or all bytes successfully transmitted.
 *           false: Transmission error occurred. 
 *  @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool BusPeripheral::transmit(uint8_t *buffer, const uint16_t length)
{
  asio::async_write(userData_.serialPort, asio::buffer(buffer, length), std::bind(&BusPeripheral::txHandler, 
                                                                                  this, 
                                                                                  asio::placeholders::error, 
                                                                                  asio::placeholders::bytes_transferred));
  return (true);
}

/** @brief   Update or poll the user comms peripheral if required.
 *  @details This function is called from the Atams::Bus class during a bus update cycle.
 *  @note    ATAMS PLATFORM REQUIREMENT - POLLING COMMS
 */
void BusPeripheral::update(void)
{
  userData_.ioContext.poll();
}

/*************************************************************************************/
/* BusPeripheral - PRIVATE FUNCTION DEFINITIONS                                      */
/*************************************************************************************/

void BusPeripheral::rxHandler(asio::error_code ec, size_t xfr)
{
  static_cast<void>(ec);

  if (xfr > 0)
  {
    rxCallback(rxBuffer_, xfr);
  }

  userData_.serialPort.async_read_some(asio::buffer(rxBuffer_), std::bind(&BusPeripheral::rxHandler, 
                                                                          this, 
                                                                          asio::placeholders::error, 
                                                                          asio::placeholders::bytes_transferred));
}

void BusPeripheral::txHandler(asio::error_code ec, size_t xfr)
{
  static_cast<void>(ec);
  static_cast<void>(xfr);

  transmitReady_.store(true);
}

/*************************************************************************************/
/* MemoryLock  - PUBLIC FUNCTION DEFINITIONS                                         */
/*************************************************************************************/

/** 
 * @brief Initialize the memory lock if required.
 * @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
bool MemoryLock::init(void)
{
  return (true);
}

/** 
 *  @brief Acquire the memory lock.
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
void MemoryLock::acquireLock(void)
{
  _memoryLock.lock();
}

/** 
 *  @brief Release the memory lock.
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
void MemoryLock::releaseLock(void)
{
  _memoryLock.unlock();
}

/*************************************************************************************/
/* CommsLock - PUBLIC FUNCTION DEFINITIONS                                           */
/*************************************************************************************/

/** 
 * @brief Initialize the comms lock if required.
 * @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
bool CommsLock::init(void)
{
  return (true);
}

/** 
 *  @brief Acquire the comms lock.
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void CommsLock::acquireLock(void)
{
  _commsLock.lock();
}

/** 
 *  @brief Release the comms lock.
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void CommsLock::releaseLock(void)
{
  _commsLock.unlock();
}


} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
