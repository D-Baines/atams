/**
  ******************************************************************************
  * @file    Router.hpp
  *
  * @author  D. Baines
  *
  * @brief   Simulated shared bus connecting the harness Hub and Node Platforms.
  *
  * @details Every device on the router hears every transmission from every other
  *          device, and by default its own transmissions too (echo). Delivery is
  *          queued: transmit() only records the bytes, and update() delivers them to
  *          the receivers and then reports transmit completion to the sender, as an
  *          interrupt or DMA driven peripheral would.
  *
  *          Each device's events go to the RouterCallbackHandler object registered
  *          for it with setCallbackHandler(), normally the harness Platform object
  *          for that device.
  *
  *          Optional seeded chunking cuts each receiver's byte stream at random
  *          points, delivering one piece per receiver per update(). Pieces can end
  *          part way through a packet or span the end of one packet and the start of
  *          the next. The router never reassembles packets.
  *
  *          Every transmission is kept in a capped traffic log. Packet headers are
  *          decoded where possible for display only; decoding never affects delivery.
  *
  *          Uses only harness types in its interface, so it can be shared by Node
  *          library copies built under a different namespace.
  *
  *          Thread safe. Once setCallbackHandler(), setReceiving() or setConnected()
  *          returns, no delivery collected before the call is still running or
  *          waiting to run, as a peripheral's interrupt would be disabled on real
  *          hardware. Handlers may call transmit() from their callbacks, but must not
  *          call those three functions, which wait for the callbacks to finish.
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
#include <array>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <random>
#include <vector>

#include "RouterCallbackHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/** @brief Identifies a device added to a Router */
typedef uint8_t DeviceID_t;

/** @brief Header fields of one packet found in a logged transmission */
struct PacketInfo_t
{
  bool     decoded       {false}; /* false if the frame failed COBS decode or is shorter than a packet header */
  uint8_t  messageType   {0U};
  uint8_t  syncCount     {0U};
  uint8_t  nodeID        {0U};
  uint16_t payloadLength {0U};
};

/** @brief One transmission in the traffic log */
struct TrafficLogEntry
{
  uint32_t                  sequence        {0U};    /* Counts every transmission since the router was constructed */
  uint32_t                  timeMs          {0U};    /* AtamsTestHarness::getMillis() when transmit() was called */
  DeviceID_t                sender          {0U};
  bool                      senderConnected {false}; /* false if the transmission was sent while disconnected and reached no one */
  std::vector<uint8_t>      bytes;
  std::vector<PacketInfo_t> packets;                 /* One entry per frame; bytes after the last terminator form a final undecoded entry */
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr uint8_t  ROUTER_MAX_DEVICES           {16U};
constexpr uint8_t  ROUTER_DEVICE_ID_NONE        {0xFFU};
constexpr uint16_t ROUTER_TRAFFIC_LOG_CAPACITY  {1000U};

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

/**
 * @brief Simulated shared bus. See the file description for the delivery rules.
 */
class Router
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor - no devices, echo on, chunking off */
  Router(void) = default;

  /* Default Destructor */
  ~Router(void) = default;

  /* Copy Constructor */
  Router(const Router &other) = delete;

  /* Copy Assignment Operator */
  Router & operator=(const Router &other) = delete;

  /* Move Constructor */
  Router(Router &&other) = delete;

  /* Move Assignment Operator */
  Router & operator=(Router &&other) = delete;

  DeviceID_t addDevice(const char *name);

  bool setCallbackHandler(const DeviceID_t device, RouterCallbackHandler *handlerPtr);

  bool setReceiving(const DeviceID_t device, const bool receiving);

  bool setConnected(const DeviceID_t device, const bool connected);

  bool isConnected(const DeviceID_t device) const;

  void setEcho(const bool enabled);

  bool setChunking(const bool enabled, const uint32_t seed, const uint16_t maxChunkLength);

  bool transmit(const DeviceID_t sender, const uint8_t *bytesPtr, const uint16_t length);

  void update(void);

  bool waitForTraffic(const uint32_t timeoutMs);

  std::vector<TrafficLogEntry> getTrafficLog(void) const;

  void printTrafficLog(const uint16_t lastEntries) const;

  private:

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct Device
  {
    const char            *name       {nullptr};
    bool                   connected  {true};
    bool                   receiving  {false};
    RouterCallbackHandler *handlerPtr {nullptr};
    std::deque<uint8_t>    stream;                /* Bytes waiting for delivery while chunking */
  };

  struct Transmission
  {
    DeviceID_t           sender          {0U};
    bool                 senderConnected {false};
    std::vector<uint8_t> bytes;
  };

  struct Delivery
  {
    RouterCallbackHandler *handlerPtr {nullptr};
    std::vector<uint8_t>   bytes;
  };

  /*-- Private Function Declarations ------------------------------------------------*/

  bool isValidDevice(const DeviceID_t device) const;

  bool isReceiver(const Device &device) const;

  bool hasPendingTraffic(void) const;

  uint16_t getRandomChunkLength(void);

  static std::vector<PacketInfo_t> decodePackets(const std::vector<uint8_t> &bytes);

  /*-- Private Variables ------------------------------------------------------------*/

  std::mutex                                  deliveryMutex_;            /* Held by update() while collecting and calling callbacks; always taken before stateMutex_ */
  mutable std::mutex                          stateMutex_;               /* Guards all other state; never held while calling callbacks */
  std::condition_variable                     trafficSignal_;
  std::array<Device, ROUTER_MAX_DEVICES>    devices_;
  uint8_t                                     deviceCount_     {0U};
  std::deque<Transmission>                  transmitQueue_;
  bool                                        echoEnabled_     {true};
  bool                                        chunkingEnabled_ {false};
  uint16_t                                    maxChunkLength_  {1U};
  std::mt19937                                random_;                   /* Seeded by setChunking() */
  std::deque<TrafficLogEntry>               trafficLog_;
  uint32_t                                    nextSequence_    {0U};
};

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
