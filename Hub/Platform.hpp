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
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** @brief The maximum number of Nodes added to on any of the application's Atams::Bus objects */
constexpr uint16_t NUMBER_OF_NODES_PER_BUS = 3U;

/** @brief The maximum number of Atams vars used by any of the application's Atams::Node objects */
constexpr uint16_t NODE_NUMBER_OF_VARS     = 100U; 

/** @brief The maximum packet size compatible with the applications communications peripheral */
constexpr uint16_t MAX_BUS_PACKET_SIZE     = 64U;

/** 
*   @brief The circular buffer size used to accept Atams packets.
*          A minimum size of 3 * Atams::MAX_BUS_PACKET_SIZE is recommended. 
*/
constexpr uint16_t CIRCULAR_BUFFER_SIZE    = 1024U;

/** @brief The maximum time period for Atams communications updates to wait for reception of a new packet */
constexpr uint64_t BUS_RESPONSE_TIMEOUT    = 500U;

/** 
*   @brief The maximum time period for an Atams::Bus object to wait without response while an Atams Node 
*          completes a non-volatile memory storage task
*/
constexpr uint64_t NVM_STORAGE_TIMEOUT     = 5000U;

/* MOVE THIS STUFF */
constexpr uint16_t COBS_MAX_DATA_PER_CODE          = 254U; 
constexpr uint16_t COBS_MAX_OVERHEAD               = (MAX_BUS_PACKET_SIZE + (COBS_MAX_DATA_PER_CODE - 1U)) / COBS_MAX_DATA_PER_CODE;
constexpr uint16_t MAX_BUS_PACKET_SIZE_PRE_FRAMING = MAX_BUS_PACKET_SIZE - COBS_MAX_OVERHEAD;

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

/** 
* @brief Platform comms peripheral class for transmit and receive of raw byte buffers. 
*        BusPeripheral is inherited and used by the Atams::Bus class. A single 
*        BusPeripheral object is constructed for each Atams::Bus object.
* @note  ATAMS PLATFORM REQUIREMENT - ALL
*/ 
class BusPeripheral
{
  public:

  /**
  * @brief   User data structure for the BusPeripheral class.
  * @details The user must populate this structure with any data required for
  *          the platform comms peripheral to operate. This structure is passed
  *          to the Atams::Bus constructor.
  * @note    ATAMS PLATFORM REQUIREMENT - OPTIONAL
  */
  struct UserData_t
  {
    asio::io_context  &ioContext;
    asio::serial_port &serialPort;
  };

  /** @brief Constructor. */
  BusPeripheral(UserData_t userData);

  /** @brief   Start receiving data on the user comms peripheral. 
   *  @details This function is called from the Atams::Bus class when a bus init or config update process is started.
   *  @return  True if the user comms peripheral was started successfully, false otherwise.
   *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
   */
  bool startReceive(void);

  /**
   * @brief   Check if the user comms peripheral is ready to transmit data.
   * @details This function is called from the Atams::Bus class during a bus update cycle.
   * @return  True if the user comms peripheral is ready to transmit data, false otherwise.
   * @note    ATAMS PLATFORM REQUIREMENT - ALL
   */
  bool transmitReady(void);

  /**
   * @brief   Transmit bytes using the user comms peripheral.
   * @details This function is called from the Atams::Bus class during a bus update cycle.
   * @param   buffer Pointer to the data buffer to transmit.
   * @param   length Length of the data buffer to transmit.
   * @return  Transmission status.
   *          true:  Transmission successfully started or all bytes successfully transmitted.
   *          false: Transmission error occurred.
   *  @note   ATAMS PLATFORM REQUIREMENT - ALL
   */
  bool transmit(uint8_t *buffer, const uint16_t length);

  /**
   * @brief   Update or poll the user comms peripheral if required.
   * @details This function is called from the Atams::Bus class during a bus update cycle.
   * @note    ATAMS PLATFORM REQUIREMENT - POLLING COMMS
   */
  void update(void);

  private: 

  /** @brief User data object initialised in the bus constructor */
  const UserData_t userData_;

  /** @brief Receive buffer for the user comms peripheral to store incoming bytes. */
  uint8_t rxBuffer_[MAX_BUS_PACKET_SIZE];

  std::atomic<bool> transmitReady_ {true};

  /**
   * @brief   Callback function for received bytes.
   * @param   rxBufferPtr    Pointer to the received data buffer (optionally use rxBuffer_).
   * @param   rxBufferLength Length of the received data buffer.
   * @details The user must call this function when new bytes have been received.
   *          This function is overridden by the Atams::Bus class. Bytes will be copied
   *          from rxBufferPtr into an Atams::Bus circular buffer in the overidden function.
   * @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
   */
  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength) = 0;

  void rxHandler(asio::error_code ec, size_t xfr);

  void txHandler(asio::error_code ec, size_t xfr);
};

/** 
 *  @brief   Memory lock class for multi-threaded applications.
 *
 *  @details This class provides a lock mechanism for protecting access to
 *           variable storage resources in a multi-threaded environment.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
class MemoryLock
{
  public:

  /** @brief Initialize the memory lock if required. */
  bool init(void);

  /** @brief Acquire the memory lock. */
  void acquireLock(void);

  /** @brief Release the memory lock. */
  void releaseLock(void);

  private:

  /* User objects can be placed here (mutex, semaphore, etc.) */

  std::mutex _memoryLock;
};

/**
 *  @brief   Comms lock class for event-driven comms.
 *
 *  @details This class provides a lock mechanism for protecting access to
 *           communications resources in an event-driven comms environment.
 *
 *  @note    ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
class CommsLock
{
  public:

  /** @brief Initialize the comms lock if required. */
  bool init(void);

  /** @brief Acquire the comms lock. */
  void acquireLock(void);

  /** @brief Release the comms lock. */
  void releaseLock(void);

  private:

  /* User objects can be placed here (mutex, semaphore, etc.) */

  std::mutex _commsLock;
};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 *  @brief  Get system time in milliseconds since startup.
 *
 *  @return System time in milliseconds since startup.
 *
 *  @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
inline uint32_t getMillis(void)
{
  return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}

} } /* End Namespace - Atams::Platform */

/**
  * @}End of File
  */
