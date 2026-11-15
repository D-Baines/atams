/**
  ******************************************************************************
  * @file    HubPlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief   Harness Platform layer for the Atams Hub library.
  *
  * @details Connects each Atams::Bus to an AtamsTestHarness::Router device. Bytes pass
  *          through untouched. All state lives in the objects each Atams::Bus owns, so
  *          nothing needs resetting between tests.
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
/* ATAMS INCLUDES                                                                    */
/*************************************************************************************/

#include <stdint.h>

#include "Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* USER INCLUDES                                                                     */
/*************************************************************************************/

#include <mutex>
#include <vector>

#include "Harness/BinarySemaphore.hpp"
#include "Harness/Router.hpp"
#include "Harness/RouterCallbackHandler.hpp"
#include "Harness/TestUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace HubPlatform {

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/** @brief The maximum number of Nodes that can be added to any of the application's Atams::Bus objects */
constexpr uint16_t NUMBER_OF_NODES_PER_BUS {15U};

/** @brief The maximum number of Atams vars used by any of the application's Atams::Node objects */
constexpr uint16_t NODE_NUMBER_OF_VARS {Atams::MAX_NUMBER_OF_VARS};

/** @brief Larger than the harness Node's, so the Node's limit is the one applied */
constexpr uint16_t MAX_BUS_PACKET_SIZE {128U};

/** @brief The recommended minimum of 3 * MAX_BUS_PACKET_SIZE */
constexpr uint16_t CIRCULAR_BUFFER_SIZE {3U * MAX_BUS_PACKET_SIZE};

/** @brief Lowered from typical hardware values; only tests that target timeouts should hit them */
constexpr uint32_t BUS_RESPONSE_TIMEOUT {50U};

/** @brief Lowered from typical hardware values; only tests that target timeouts should hit them */
constexpr uint32_t BUS_TRANSMIT_TIMEOUT {50U};

/** @brief Harness NVM operations are instant, so this only matters for tests that target it */
constexpr uint64_t NVM_STORAGE_TIMEOUT {1000U};

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

/**
 * @brief Sends and receives an Atams::Bus's bytes through one router device.
 *
 * @note  ATAMS PLATFORM REQUIREMENT - ALL
 */
class BusPeripheral :
public AtamsTestHarness::RouterCallbackHandler
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Typedefs -----------------------------------------------------*/

  /**
   * @brief The router and device this Bus uses. Both must outlive the Bus.
   */
  struct UserData_t
  {
    AtamsTestHarness::Router     &router;
    AtamsTestHarness::DeviceID_t  device;
  };

  /*-- Required Public Function Declarations ----------------------------------------*/

  /* Constructor */
  explicit BusPeripheral(UserData_t userData);

  /* Destructor */
  virtual ~BusPeripheral(void) override;

  /* Copy Constructor */
  BusPeripheral(const BusPeripheral &other) = delete;

  /* Copy Assignment Operator */
  BusPeripheral & operator=(const BusPeripheral &other) = delete;

  /* Move Constructor */
  BusPeripheral(BusPeripheral &&other) = delete;

  /* Move Assignment Operator */
  BusPeripheral & operator=(BusPeripheral &&other) = delete;

  bool startReceive(void);

  void stopReceive(void);

  bool transmit(uint8_t *buffer, const uint16_t length);

  void update(void);

  private:

  /*-- Required Private Variables ---------------------------------------------------*/

  /** @brief User data object initialised in the bus constructor */
  const UserData_t userData_;

  /*-- User Private Variables -------------------------------------------------------*/

  std::vector<uint8_t> rxCopy_;                        /* rxCallback() takes a non-const pointer, so router bytes are copied here */
  std::mutex           transmitMutex_;                 /* Guards the transmit state below: transmit() and its completion run on different threads */
  bool                 transmitBusy_      {false};
  const uint8_t       *transmitBufferPtr_ {nullptr};   /* The Bus buffer being sent */
  std::vector<uint8_t> transmitCopy_;                  /* Its bytes when transmit() was called, to catch the Bus changing them early */

  /*-- Required Private Function Declarations ---------------------------------------*/

  /**
   * @brief Callback for received bytes. Overridden by Atams::Bus.
   *
   * @note  ATAMS PLATFORM REQUIREMENT - ALL
   */
  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength) = 0;

  /**
   * @brief Callback for transmit completion. Overridden by Atams::Bus.
   *
   * @note  ATAMS PLATFORM REQUIREMENT - ALL
   */
  virtual void txCallback(void) = 0;

  /*-- User Private Function Declarations -------------------------------------------*/

  virtual void onRouterReceive(const uint8_t *bytesPtr, const uint16_t length) override final;

  virtual void onRouterTransmitComplete(void) override final;
};

/**
 *  @brief Memory lock for multi-threaded applications.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - MULTI-THREADED
 */
class MemoryLock
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Function Declarations ----------------------------------------*/

  /* Default Constructor */
  MemoryLock(void) = default;

  /* Default Destructor */
  ~MemoryLock(void) = default;

  /* Copy Constructor */
  MemoryLock(const MemoryLock &other) = delete;

  /* Copy Assignment Operator */
  MemoryLock & operator=(const MemoryLock &other) = delete;

  /* Move Constructor */
  MemoryLock(MemoryLock &&other) = delete;

  /* Move Assignment Operator */
  MemoryLock & operator=(MemoryLock &&other) = delete;

  void acquireLock(void);

  void releaseLock(void);

  private:

  /*-- User Private Variables -------------------------------------------------------*/

  std::mutex mutex_;
};

/**
 *  @brief Comms lock for event-driven comms.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS
 */
class CommsLock
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Function Declarations ----------------------------------------*/

  /* Default Constructor */
  CommsLock(void) = default;

  /* Default Destructor */
  ~CommsLock(void) = default;

  /* Copy Constructor */
  CommsLock(const CommsLock &other) = delete;

  /* Copy Assignment Operator */
  CommsLock & operator=(const CommsLock &other) = delete;

  /* Move Constructor */
  CommsLock(CommsLock &&other) = delete;

  /* Move Assignment Operator */
  CommsLock & operator=(CommsLock &&other) = delete;

  void acquireLock(void);

  void releaseLock(void);

  private:

  /*-- User Private Variables -------------------------------------------------------*/

  std::mutex mutex_;
};

/**
 *  @brief Binary semaphore for blocking Bus update cycles.
 *
 *  @note  ATAMS PLATFORM REQUIREMENT - BLOCKING BUS UPDATE
 */
class BinarySemaphore
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Required Public Function Declarations ----------------------------------------*/

  /* Default Constructor */
  BinarySemaphore(void) = default;

  /* Default Destructor */
  ~BinarySemaphore(void) = default;

  /* Copy Constructor */
  BinarySemaphore(const BinarySemaphore &other) = delete;

  /* Copy Assignment Operator */
  BinarySemaphore & operator=(const BinarySemaphore &other) = delete;

  /* Move Constructor */
  BinarySemaphore(BinarySemaphore &&other) = delete;

  /* Move Assignment Operator */
  BinarySemaphore & operator=(BinarySemaphore &&other) = delete;

  void waitWithTimeout(uint32_t timeoutMs);

  void release(void);

  private:

  /*-- User Private Variables -------------------------------------------------------*/

  AtamsTestHarness::BinarySemaphore semaphore_;
};

/*************************************************************************************/
/* INLINE PUBLIC FUNCTION DEFINITIONS                                                */
/*************************************************************************************/

/**
 *  @brief  Get system time in milliseconds since startup.
 *
 *  @return The harness clock, shared with the Node Platforms and the router.
 *
 *  @note   ATAMS PLATFORM REQUIREMENT - ALL
 */
inline uint32_t getMillis(void)
{
  return (AtamsTestHarness::getMillis());
}

} } /* End Namespace - Atams::HubPlatform */

/**
  * @}End of File
  */
