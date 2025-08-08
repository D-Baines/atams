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
#include <mutex>
#include "Platform/asio-1.30.2/include/asio.hpp"

/*************************************************************************************/
/* PUBLIC MACROS                                                                     */
/*************************************************************************************/

#define ATAMS_DEBUG 0

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NUMBER_OF_NODES_PER_BUS = 2U;
inline constexpr uint16_t NODE_NUMBER_OF_VARS     = 100U; 
inline constexpr uint16_t MAX_BUS_PACKET_SIZE     = 64U;
inline constexpr uint16_t CIRCULAR_BUFFER_SIZE    = 1024U;
inline constexpr uint64_t BUS_RESPONSE_TIMEOUT    = 500U;
inline constexpr uint64_t NVM_STORAGE_TIMEOUT     = 5000U;

inline constexpr uint16_t COBS_MAX_DATA_PER_CODE          = 254U; 
inline constexpr uint16_t COBS_MAX_OVERHEAD               = (MAX_BUS_PACKET_SIZE + (COBS_MAX_DATA_PER_CODE - 1U)) / COBS_MAX_DATA_PER_CODE;
inline constexpr uint16_t MAX_BUS_PACKET_SIZE_PRE_FRAMING = MAX_BUS_PACKET_SIZE - COBS_MAX_OVERHEAD;

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

class BusPeripheral
{
  public:
  
  struct UserData_t
  {
    asio::io_context  &ioContext;
    asio::serial_port &serialPort;
  };

  /* Default Constructor */
  BusPeripheral(void) = delete;

  /* Constructor */
  BusPeripheral(UserData_t userData) :
  userData_(userData) {};

  /* Default Destructor */
  ~BusPeripheral(void) = default;

  /* Copy Constructor */
  BusPeripheral(const BusPeripheral &other) = delete;

  /* Copy Assignment Operator */
  BusPeripheral & operator=(const BusPeripheral &other) = delete;

  /* Move Constructor */
  BusPeripheral(BusPeripheral &&other) = delete;

  /* Move Assignment Operator */
  BusPeripheral & operator=(BusPeripheral &&other) = delete;

  bool startReceive(void)
  {
    userData_.serialPort.async_read_some(asio::buffer(rxBuffer_), std::bind(&BusPeripheral::rxHandler, 
                                                                            this, 
                                                                            asio::placeholders::error, 
                                                                            asio::placeholders::bytes_transferred));

    return (true);
  }

  void rxHandler(asio::error_code ec, size_t xfr) 
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

  void txHandler(asio::error_code ec, size_t xfr) 
  {
    static_cast<void>(ec);
    static_cast<void>(xfr);

    transmitReady_.store(true);
  }

  bool transmitReady(void)
  {
    return (transmitReady_.load());
  }

  bool transmit(uint8_t *buffer, const uint16_t length)
  {
    /* Transmit Buffer */
    asio::async_write(userData_.serialPort, asio::buffer(buffer, length), std::bind(&BusPeripheral::txHandler, 
                                                                                    this, 
                                                                                    asio::placeholders::error, 
                                                                                    asio::placeholders::bytes_transferred));

    return (true);
  }

  void update(void)
  {
    userData_.ioContext.poll();
  }
  
  private:

  const UserData_t userData_;

  uint8_t rxBuffer_[MAX_BUS_PACKET_SIZE];

  std::atomic<bool> transmitReady_ {true};

  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength)
  {
    static_cast<void>(rxBufferPtr);
    static_cast<void>(rxBufferLength);
    /* Do nothing - overidden by Bus */ 
  }
  
};

class MemoryLock
{
  public:

  bool init(void)
  {
    /* Init user lock mechanisam if required */
    return (true);
  }

  void acquireLock(void)
  {
    _memoryLock.lock();
  }

  void releaseLock(void)
  {
    _memoryLock.unlock();
  }

  private:

  std::mutex _memoryLock;
};

class CommsLock
{
  public:

  bool init(void)
  {
    /* Init user lock mechanism if required */
    return (true);
  }

  void acquireLock(void)
  {
    _commsLock.lock();
  }

  void releaseLock(void)
  {
    _commsLock.unlock();
  }

  private:

  std::mutex _commsLock;
};

/*************************************************************************************/
/* PUBLIC INLINE FUNCTION DEFINITIONS                                                */
/*************************************************************************************/

inline uint32_t getMillis(void)
{
  return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
