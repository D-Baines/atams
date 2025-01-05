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
#include "../../AtamsTypedefs.hpp"
#include "asio.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

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

  BusPeripheral(UserData_t userData);

  bool startPeripheral(void)
  {
    asio::serial_port::open("/dev/cu.usbserial-AQ02Y2T5");
    asio::serial_port::set_option(asio::serial_port_base::baud_rate(115200));
    asio::serial_port::set_option(asio::serial_port::character_size(8));
    asio::serial_port::set_option(asio::serial_port::stop_bits(asio::serial_port::stop_bits::one));
    asio::serial_port::set_option(asio::serial_port::flow_control(asio::serial_port::flow_control::none));
    asio::serial_port::set_option(asio::serial_port::parity(asio::serial_port::parity::none));
    asio::serial_port::async_read_some(asio::buffer(_rxBuffer), std::bind(&BusPeripheral::handler, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
    return (true);
  }

  void handler(asio::error_code ec, size_t xfr) 
  {
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

  void transmit(uint8_t *buffer, uint16_t length)
  {
    /* Transmit Buffer */
    asio::write(static_cast<asio::serial_port&>(*this), asio::buffer(buffer, length));
  }

  void update(void);
  
  private:

  const UserData_t _userData;

  uint8_t  _rxBuffer[MAX_MESH_PACKET_SIZE];

  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength)
  {
    /* Do nothing - overidden by Bus */ 
  }
                          

};

class MemoryLock
{
  public:

  Error_t init(void)
  {
    /* Init user lock mechanisam - not required when using std::mutex */
    return (ERROR_NONE);
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

  Error_t init(void)
  {
    /* Init user lock mechanisam - not required when using std::mutex */
    return (ERROR_NONE);
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
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NUMBER_OF_NODES_PER_BUS     = 10U;
inline constexpr uint16_t NODE_NUMBER_OF_DATA_MEMBERS = 200U; 
inline constexpr uint16_t NODE_NUMBER_OF_DATA_BLOCKS  = 3U; 
inline constexpr uint16_t COMMS_BUFFER_SIZE           = 512U;
inline constexpr uint64_t BUS_RESPONSE_TIMEOUT        = 50U;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint64_t getMillis(void);

} } /* End Atams Namespace */


/**
  * @}End of File
  */
