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
#include "../Shared/AtamsTypedefs.hpp"
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

inline constexpr uint16_t NUMBER_OF_NODES_PER_BUS = 10U;
inline constexpr uint16_t NODE_NUMBER_OF_VARS     = 100U; 
inline constexpr uint16_t MAX_BUS_PACKET_SIZE     = 64U;
inline constexpr uint16_t CIRCULAR_BUFFER_SIZE    = 1024U;
inline constexpr uint64_t BUS_RESPONSE_TIMEOUT    = 2000000U;

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

class BusPeripheral :
private asio::serial_port
{
  public:
  
  struct UserData_t
  {

  };

  /* Constructor */
  BusPeripheral(UserData_t userData);

  /* Copy Constructor */
  BusPeripheral(const BusPeripheral &other) = delete;

  /* Copy Assignment Operator */
  BusPeripheral & operator=(const BusPeripheral &other) = delete;

  bool startPeripheral(void)
  {
    asio::serial_port::open("/dev/cu.usbserial-AQ02Y2T5");
    asio::serial_port::set_option(asio::serial_port_base::baud_rate(230400));
    asio::serial_port::set_option(asio::serial_port::character_size(8));
    asio::serial_port::set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    asio::serial_port::set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));
    asio::serial_port::set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    asio::serial_port::async_read_some(asio::buffer(_rxBuffer), std::bind(&BusPeripheral::handler, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
    return (true);
  }

  void handler(asio::error_code ec, size_t xfr) 
  {
    static_cast<void>(ec);

    if (xfr > 0)
    {
      rxCallback(_rxBuffer, xfr);
    }
    asio::serial_port::async_read_some(asio::buffer(_rxBuffer), std::bind(&BusPeripheral::handler, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
  }

  bool transmitReady(void)
  {
    return (true);
  }

  bool transmit(uint8_t *buffer, uint16_t length)
  {
    /* Transmit Buffer */
    asio::write(static_cast<asio::serial_port&>(*this), asio::buffer(buffer, length));

    return (true);
  }

  void update(void);
  
  private:

  const UserData_t _userData;

  uint8_t _rxBuffer[MAX_BUS_PACKET_SIZE];

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
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint64_t getMillis(void);

} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
