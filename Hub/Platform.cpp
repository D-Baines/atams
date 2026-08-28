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
 *  @details This function is called from the Atams::Bus class when a bus init or config update process is started.
 *
 *  @return  True if the user comms peripheral was started successfully, false otherwise.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
bool BusPeripheral::startReceive(void)
{

}

/** @brief   Transmit bytes using the user comms peripheral.
 *
 *  @details This function is called from the Atams::Bus class during a bus update cycle.
 *           Regardless of whether the implementation is synchronous (blocking) or
 *           asynchronous (interrupt / DMA / async), txCallback() must be called exactly
 *           once when the transmission completes. For a blocking implementation, call
 *           txCallback() at the end of this function before returning. For an
 *           asynchronous implementation, call txCallback() from the transmit-complete
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

}

/*
 * @brief   User update function called at regular intervals during Bus Update Cycles
 *
 * @details Users can use this function to poll communications peripherals, check for
 *          peripheral errors, and/or restart peripheral reception if required.
 *
 *  @note   ATAMS PLATFORM REQUIREMENT - POLLING COMMS
 */
void BusPeripheral::update(void)
{

}

/*************************************************************************************/
/* BusPeripheral - USER PRIVATE FUNCTION DEFINITIONS                                 */
/*************************************************************************************/



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

}

/** 
 *  @brief Release the memory lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
void MemoryLock::releaseLock(void)
{

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

}

/** 
 *  @brief Release the comms lock.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
void CommsLock::releaseLock(void)
{

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

}

/**
 * @brief Release the semaphore, unblocking any thread waiting in wait or waitWithTimeout.
 *
 * @note  ATAMS PLATFORM REQUIREMENT - BLOCKING COMMS
 */
void BinarySemaphore::release(void)
{

}


} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
