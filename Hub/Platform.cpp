/**
  ******************************************************************************
  * @file    Platform.cpp
  *
  * @author  D. Baines
  *
  * @brief   Platform abstraction layer (PAL) implementation for the Atams Hub.
  *
  * @details Provides empty implementations of the PAL functions required by the
  *          Atams Hub. Users must complete these functions with their own 
  *          platform-specific code to provide serial communications and locking 
  *          for their target environment. Refer to the @note tags on each
  *          function for platform requirement classifications.
  * 
  *          The 'BusPeripheral - USER PRIVATE FUNCTION DEFINITIONS' section
  *          can be used to define implementations for any user declared 
  *          BusPeripheral functions.
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* BusPeripheral - REQUIRED PUBLIC FUNCTION DEFINITIONS                              */
/*************************************************************************************/

/** @brief Constructor. */
BusPeripheral::BusPeripheral(UserData_t userData) : 
userData_(userData)
{
  
}

/** @brief   Start receiving data on the user comms peripheral.
 *
 *  @details This function is called from the Atams::Bus class at the start of every Bus process: bus init,
 *           config update, and each update cycle (all-node and single-node). Reception should continue,
 *           delivering received bytes via @c rxCallback(), until @c stopReceive() is called.
 *
 *           Atams will not call this function again until @c stopReceive() has been called.
 *
 *  @return  True if reception was started successfully, false otherwise.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
bool BusPeripheral::startReceive(void)
{
  if (!userData_.serialPort.is_open())
  {
    return (false);
  }

  if (isReceiving_)
  {
    return (true); /* Only one read may be outstanding on the port. */
  }

  isReceiving_ = true;

  userData_.serialPort.async_read_some(asio::buffer(rxBuffer_),
                                       std::bind(&BusPeripheral::rxHandler,
                                                 this,
                                                 asio::placeholders::error,
                                                 asio::placeholders::bytes_transferred));
  return (true);
}

/** @brief   Stop receiving data on the user comms peripheral.
 *
 *  @details This function is called from the Atams::Bus class when a Bus process ends, or fails to begin
 *           after reception was started. Any transmission in progress may also be stopped.
 *
 *           This function may be called while reception is already stopped. The user implementation must
 *           handle this case and do nothing.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void BusPeripheral::stopReceive(void)
{
  if (isReceiving_)
  {
    asio::error_code ec;

    userData_.serialPort.cancel(ec);

    isReceiving_ = false;
  }
}

/** @brief   Transmit bytes using the user comms peripheral.
 *
 *  @details This function is called from the Atams::Bus class during a bus update cycle.
 *           Regardless of whether the implementation is synchronous (blocking) or
 *           asynchronous (interrupt / DMA / async), @c txCallback() must be called exactly
 *           once when the transmission completes. For a blocking implementation, call
 *           @c txCallback() at the end of this function before returning. For an
 *           asynchronous implementation, call @c txCallback() from the transmit-complete
 *           interrupt, DMA callback, or async handler.
 *
 *  @param   buffer Pointer to the data buffer to transmit.
 *
 *  @param   length Length of the data buffer to transmit.
 *
 *  @return  Transmission status.
 *           true:  Transmission successfully started or all bytes successfully transmitted.
 *           false: Transmission error occurred. @c txCallback() must NOT be called on failure.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - ALL
 */
bool BusPeripheral::transmit(uint8_t *buffer, const uint16_t length)
{
  if (!userData_.serialPort.is_open())
  {
    return (false);
  }

  asio::async_write(userData_.serialPort,
                    asio::buffer(buffer, length),
                    std::bind(&BusPeripheral::txHandler,
                              this,
                              asio::placeholders::error,
                              asio::placeholders::bytes_transferred));

  return (true);
}

/** 
 * @brief   User update function called at regular intervals during Bus Update Cycles
 *
 * @details Users can use this function to poll communications peripherals, check for
 *          peripheral errors, and/or restart peripheral reception if required.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - POLLING COMMS
 */
void BusPeripheral::update(void)
{

}

/*************************************************************************************/
/* BusPeripheral - USER PRIVATE FUNCTION DEFINITIONS                                 */
/*************************************************************************************/

void BusPeripheral::rxHandler(asio::error_code ec, size_t xfr)
{
  if (ec)
  {
    return; /* Cancelled by stopReceive, port closed, or device removed - don't re-arm. */
  }

  if (xfr > 0)
  {
    rxCallback(rxBuffer_, xfr);
  }

  userData_.serialPort.async_read_some(asio::buffer(rxBuffer_), 
                                       std::bind(&BusPeripheral::rxHandler, 
                                                 this, 
                                                 asio::placeholders::error, 
                                                 asio::placeholders::bytes_transferred));
}

void BusPeripheral::txHandler(asio::error_code ec, size_t xfr)
{
  static_cast<void>(xfr);

  if (!ec)
  {
    txCallback();
  }
}

/*************************************************************************************/
/* MemoryLock - REQUIRED PUBLIC FUNCTION DEFINITIONS                                 */
/*************************************************************************************/

/** 
 *  @brief Acquire the memory lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
void MemoryLock::acquireLock(void)
{
  _memoryLock.lock();
}

/** 
 *  @brief Release the memory lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
void MemoryLock::releaseLock(void)
{
  _memoryLock.unlock();
}

/*************************************************************************************/
/* CommsLock - REQUIRED PUBLIC FUNCTION DEFINITIONS                                  */
/*************************************************************************************/

/** 
 *  @brief Acquire the comms lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void CommsLock::acquireLock(void)
{
  _commsLock.lock();
}

/** 
 *  @brief Release the comms lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void CommsLock::releaseLock(void)
{
  _commsLock.unlock();
}


/*************************************************************************************/
/* BinarySemaphore - REQUIRED PUBLIC FUNCTION DEFINITIONS                            */
/*************************************************************************************/

/**
 * @brief Block the calling thread until the semaphore is released or the timeout expires.
 *
 * @note  ATAMS PLATFORM REQUIREMENT - BLOCKING COMMS
 */
void BinarySemaphore::waitWithTimeout(uint32_t timeoutMilliseconds)
{
  std::unique_lock<std::mutex> lock(mutex_);

  cv_.wait_for(lock,
               std::chrono::milliseconds(timeoutMilliseconds),
               [this]{ return released_; });

  released_ = false;
}

/**
 * @brief Release the semaphore, unblocking any thread waiting in wait or waitWithTimeout.
 *
 * @note  ATAMS PLATFORM REQUIREMENT - BLOCKING COMMS
 */
void BinarySemaphore::release(void)
{
  std::unique_lock<std::mutex> lock(mutex_);
  released_ = true;
  lock.unlock();

  cv_.notify_one();
}

} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
