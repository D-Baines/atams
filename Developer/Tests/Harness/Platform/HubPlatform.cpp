/**
  ******************************************************************************
  * @file    HubPlatform.cpp
  *
  * @author  D. Baines
  *
  * @brief   Harness Platform layer for the Atams Hub library.
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "HubPlatform.hpp"

#include <cstring>

#include "Harness/Assertions.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace HubPlatform {

/*************************************************************************************/
/* BusPeripheral - REQUIRED PUBLIC FUNCTION DEFINITIONS                              */
/*************************************************************************************/

/**
 * @brief Registers this object as the device's router handler. The router only calls it
 *        once startReceive() or transmit() has been called, after Atams::Bus is constructed.
 */
BusPeripheral::BusPeripheral(UserData_t userData) :
userData_(userData)
{
  if (userData_.router.setCallbackHandler(userData_.device, this) == false)
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "BusPeripheral: invalid router device");
  }
}

BusPeripheral::~BusPeripheral(void)
{
  static_cast<void>(userData_.router.setCallbackHandler(userData_.device, nullptr));
}

bool BusPeripheral::startReceive(void)
{
  return (userData_.router.setReceiving(userData_.device, true));
}

void BusPeripheral::stopReceive(void)
{
  static_cast<void>(userData_.router.setReceiving(userData_.device, false));
}

/**
 * @brief Queues the bytes on the router. txCallback() is called from Router::update()
 *        once they have been delivered.
 */
bool BusPeripheral::transmit(uint8_t *buffer, const uint16_t length)
{
  if (buffer == nullptr) return (false); /* Early Return */

  transmitMutex_.lock();

  /* Busy, as a real peripheral would be, if Atams::Bus starts a new cycle after a transmit
     timeout while the previous transmission is still waiting for Router::update() */
  if (transmitBusy_)
  {
    transmitMutex_.unlock();
    return (false); /* Early Return */
  }

  transmitBusy_      = true;
  transmitBufferPtr_ = buffer;
  transmitCopy_.assign(buffer, buffer + length);

  transmitMutex_.unlock();

  /* Not holding transmitMutex_, as the router takes its own locks */
  if (userData_.router.transmit(userData_.device, buffer, length) == false)
  {
    std::lock_guard<std::mutex> lock(transmitMutex_);

    transmitBusy_      = false;
    transmitBufferPtr_ = nullptr;

    return (false); /* Early Return */
  }

  return (true);
}

/**
 * @brief Delivery only happens in Router::update(), so there is nothing to poll.
 */
void BusPeripheral::update(void)
{
  /* Do Nothing */
}

/*************************************************************************************/
/* BusPeripheral - USER PRIVATE FUNCTION DEFINITIONS                                 */
/*************************************************************************************/

void BusPeripheral::onRouterReceive(const uint8_t *bytesPtr, const uint16_t length)
{
  if (bytesPtr == nullptr) return; /* Early Return */

  rxCopy_.assign(bytesPtr, bytesPtr + length);

  rxCallback(rxCopy_.data(), length);
}

/**
 * @brief Checks the Bus left its buffer alone while it was being sent, then reports completion.
 */
void BusPeripheral::onRouterTransmitComplete(void)
{
  transmitMutex_.lock();

  if ((transmitBufferPtr_ != nullptr) &&
      (std::memcmp(transmitBufferPtr_, transmitCopy_.data(), transmitCopy_.size()) != 0))
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "BusPeripheral: transmit buffer changed before txCallback()");
  }

  transmitBusy_      = false;
  transmitBufferPtr_ = nullptr;

  transmitMutex_.unlock();

  /* Not holding transmitMutex_, as Atams::Bus may transmit again from txCallback() */
  txCallback();
}

/*************************************************************************************/
/* MemoryLock - REQUIRED PUBLIC FUNCTION DEFINITIONS                                 */
/*************************************************************************************/

void MemoryLock::acquireLock(void)
{
  mutex_.lock();
}

void MemoryLock::releaseLock(void)
{
  mutex_.unlock();
}

/*************************************************************************************/
/* CommsLock - REQUIRED PUBLIC FUNCTION DEFINITIONS                                  */
/*************************************************************************************/

void CommsLock::acquireLock(void)
{
  mutex_.lock();
}

void CommsLock::releaseLock(void)
{
  mutex_.unlock();
}

/*************************************************************************************/
/* BinarySemaphore - REQUIRED PUBLIC FUNCTION DEFINITIONS                            */
/*************************************************************************************/

void BinarySemaphore::waitWithTimeout(uint32_t timeoutMs)
{
  semaphore_.waitWithTimeout(timeoutMs);
}

void BinarySemaphore::release(void)
{
  semaphore_.release();
}

} } /* End Namespace - Atams::HubPlatform */

/**
  * @}End of File
  */
