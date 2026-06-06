/**
  ******************************************************************************
  * @file    Platform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Platform interface declarations for the Atams Hub.
  *
  * @details Defines the platform abstraction layer (PAL) for the Atams Hub. Provides
  *          the BusPeripheral, MemoryLock, and CommsLock classes that the user must
  *          implement to port the library to a specific hardware platform. Also exposes
  *          platform-specific constants and the inline getMillis() time function.
  *          This file should be modified to match the target platform.
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
#include "asio.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** @brief The maximum number of Nodes that can be added to any of the application's Atams::Bus objects */
constexpr uint16_t NUMBER_OF_NODES_PER_BUS {2U};

/** @brief The maximum number of Atams vars used by any of the application's Atams::Node objects */
constexpr uint16_t NODE_NUMBER_OF_VARS {100U};

/** @brief The maximum packet size compatible with the applications communications peripheral */
constexpr uint16_t MAX_BUS_PACKET_SIZE {64U};

/**
*  @brief The circular buffer size used for receiving Atams packets.
*         A minimum size of 3 * Atams::MAX_BUS_PACKET_SIZE is recommended.
*/
constexpr uint16_t CIRCULAR_BUFFER_SIZE {1024U};

/** 
*   @brief The maximum time period for an Atams::Bus object to wait wihout a response 
*          from an Atams::Node device during a Bus update cycle in milliseconds.
*/
constexpr uint64_t BUS_RESPONSE_TIMEOUT {500U};

/** 
*   @brief The maximum time period for an Atams::Bus object to wait without response while an 
*          Atams::Node device completes a non-volatile memory storage task in milliseconds.
*/
constexpr uint64_t NVM_STORAGE_TIMEOUT {5000U};

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

/** 
 * @brief Platform comms peripheral class for the transmit and receive of raw byte buffers. 
 *        BusPeripheral is inherited and used by the Atams::Bus class. A single 
 *        BusPeripheral object is constructed for each Atams::Bus object.
 *
 * @note  ATAMS PLATFORM REQUIREMENT - ALL
 */ 
class BusPeripheral
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Typedefs -----------------------------------------------------*/

  /**
   * @brief   User data structure for the BusPeripheral class.
   *
   * @details The user should populate this structure with any data required for
   *          the platform comms peripheral to operate. This structure is passed
   *          to the Atams::Bus constructor.
   *
   * @note    ATAMS PLATFORM REQUIREMENT - OPTIONAL
   */
  struct UserData_t
  {
    asio::io_context  &ioContext;
    asio::serial_port &serialPort;
  };

  /*-- Required Public Function Declarations ----------------------------------------*/

  BusPeripheral(UserData_t userData);

  bool startReceive(void);

  bool transmitReady(void);

  bool transmit(uint8_t *buffer, const uint16_t length);

  void update(void);

  private: 

  /*-- Required Private Variables ---------------------------------------------------*/

  /** @brief User data object initialised in the bus constructor */
  const UserData_t userData_;

  /*-- User Private Variables -------------------------------------------------------*/
  
  uint8_t rxBuffer_[MAX_BUS_PACKET_SIZE];

  std::atomic<bool> transmitReady_ {true};

  /*-- Required Private Function Declarations ---------------------------------------*/

   /**
   * @brief   Callback function for received bytes.
   *
   * @param   rxBufferPtr    Pointer to the received data buffer (optionally use rxBuffer_).
   *
   * @param   rxBufferLength Length of the received data buffer.
   *
   * @details The user must call this function when new bytes have been received.
   *          This function is overridden by the Atams::Bus class. Bytes will be copied
   *          from rxBufferPtr into an Atams::Bus circular buffer in the overidden function.
   *
   * @note    ATAMS PLATFORM REQUIREMENT - ALL
   */
  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength) = 0;

  /*-- User Private Function Declarations -------------------------------------------*/

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
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Function Declarations ----------------------------------------*/

  bool init(void);

  void acquireLock(void);

  void releaseLock(void);

  private:

  /*-- User Private Variables -------------------------------------------------------*/

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
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Function Declarations ----------------------------------------*/

  bool init(void);

  void acquireLock(void);

  void releaseLock(void);

  private:

  /*-- User Private Variables -------------------------------------------------------*/

  std::mutex _commsLock;
};

/*************************************************************************************/
/* INLINE PUBLIC FUNCTION DEFINITIONS                                                */
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
