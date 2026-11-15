/**
  ******************************************************************************
  * @file    Router.cpp
  *
  * @author  D. Baines
  *
  * @brief   Simulated shared bus connecting the harness Hub and Node Platforms.
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

#include "Router.hpp"

#include <stdio.h>
#include <chrono>
#include <utility>

#include "TestUtilities.hpp"
#include "Shared/AtamsTypedefs.hpp"
#include "Shared/Utilities/COBS.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static const char * const s_messageTypeNames[Atams::NUMBER_OF_MESSAGE_TYPES] =
{
  /* [Atams::MESSAGE_UNKNOWN              ] = */ "UNKNOWN",
  /* [Atams::MESSAGE_REQUEST              ] = */ "REQUEST",
  /* [Atams::MESSAGE_RESPONSE             ] = */ "RESPONSE",
  /* [Atams::MESSAGE_REQUEST_SYNCED       ] = */ "REQUEST_SYNCED",
  /* [Atams::MESSAGE_RESPONSE_SYNCED      ] = */ "RESPONSE_SYNCED",
  /* [Atams::MESSAGE_SYNC_JOG             ] = */ "SYNC_JOG",
  /* [Atams::MESSAGE_ABORT_RESPONSE       ] = */ "ABORT_RESPONSE",
  /* [Atams::MESSAGE_ABORT_RESPONSE_SYNCED] = */ "ABORT_RESPONSE_SYNCED",
};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief  Add a device to the bus. New devices are connected and not receiving, with
 *         no callback handler.
 *
 * @param  name Name shown in the traffic log. Must outlive the router.
 *
 * @return The new device's ID, or ROUTER_DEVICE_ID_NONE if ROUTER_MAX_DEVICES have
 *         already been added.
 */
DeviceID_t Router::addDevice(const char *name)
{
  std::lock_guard<std::mutex> lock(stateMutex_);

  if (deviceCount_ >= ROUTER_MAX_DEVICES) return (ROUTER_DEVICE_ID_NONE); /* Early Return */

  const DeviceID_t device {deviceCount_};

  devices_[device].name = (name != nullptr) ? name : "?";

  deviceCount_++;

  return (device);
}

/**
 * @brief  Set the object that receives a device's events: received bytes and transmit
 *         completion.
 *
 * @param  device     The device.
 * @param  handlerPtr The handler, or nullptr for none. Must stay valid until replaced or
 *                    cleared. Clearing also drops any bytes still waiting for delivery.
 *
 * @return false if the device ID is invalid.
 *
 * @note   Waits for any update() in progress to finish its callbacks, so the previous
 *         handler is never called after this function returns. Must not be called
 *         from a handler callback.
 */
bool Router::setCallbackHandler(const DeviceID_t device, RouterCallbackHandler *handlerPtr)
{
  std::lock_guard<std::mutex> deliveryLock(deliveryMutex_);
  std::lock_guard<std::mutex> lock(stateMutex_);

  if (!isValidDevice(device)) return (false); /* Early Return */

  devices_[device].handlerPtr = handlerPtr;

  if (handlerPtr == nullptr) devices_[device].stream.clear();

  return (true);
}

/**
 * @brief  Start or stop a device receiving. A device that is not receiving drops the
 *         bytes sent to it, but its transmissions still complete.
 *
 * @param  device    The device.
 * @param  receiving true to receive. Stopping also drops any bytes still waiting for
 *                   delivery to the device.
 *
 * @return false if the device ID is invalid.
 *
 * @note   Waits for any update() in progress to finish its callbacks, so the device
 *         receives nothing after stopping. Must not be called from a handler callback.
 */
bool Router::setReceiving(const DeviceID_t device, const bool receiving)
{
  std::lock_guard<std::mutex> deliveryLock(deliveryMutex_);
  std::lock_guard<std::mutex> lock(stateMutex_);

  if (!isValidDevice(device)) return (false); /* Early Return */

  devices_[device].receiving = receiving;

  if (!receiving) devices_[device].stream.clear();

  return (true);
}

/**
 * @brief  Connect a device to, or disconnect it from, the bus.
 *
 * @details A disconnected device receives nothing, and its transmissions reach no one
 *          but still complete. Disconnecting drops any bytes still waiting for delivery
 *          to the device, as unplugging a cable part way through a packet would.
 *
 * @return false if the device ID is invalid.
 *
 * @note   Waits for any update() in progress to finish its callbacks, so a disconnected
 *         device receives nothing after this function returns. Must not be called from
 *         a handler callback.
 */
bool Router::setConnected(const DeviceID_t device, const bool connected)
{
  std::lock_guard<std::mutex> deliveryLock(deliveryMutex_);
  std::lock_guard<std::mutex> lock(stateMutex_);

  if (!isValidDevice(device)) return (false); /* Early Return */

  devices_[device].connected = connected;

  if (!connected) devices_[device].stream.clear();

  return (true);
}

/**
 * @return true if the device is connected. false if it is disconnected or the device ID
 *         is invalid.
 */
bool Router::isConnected(const DeviceID_t device) const
{
  std::lock_guard<std::mutex> lock(stateMutex_);

  return (isValidDevice(device) && devices_[device].connected);
}

/**
 * @brief Set whether a device receives its own transmissions. On by default.
 */
void Router::setEcho(const bool enabled)
{
  std::lock_guard<std::mutex> lock(stateMutex_);

  echoEnabled_ = enabled;
}

/**
 * @brief  Turn chunked delivery on or off.
 *
 * @details While on, each update() delivers one piece of each receiver's waiting bytes,
 *          with a random length from 1 to maxChunkLength. While off, each update()
 *          delivers every transmission it puts on the bus whole. Intended to be set
 *          before any traffic; turning chunking off drops any bytes still waiting for
 *          chunked delivery.
 *
 * @param  enabled        true to chunk deliveries.
 * @param  seed           Seeds the piece lengths, so a run can be repeated exactly.
 * @param  maxChunkLength Longest piece delivered. Ignored when disabled.
 *
 * @return false, with the setting unchanged, if enabled with a maxChunkLength of 0.
 */
bool Router::setChunking(const bool enabled, const uint32_t seed, const uint16_t maxChunkLength)
{
  if (enabled && (maxChunkLength == 0U)) return (false); /* Early Return */

  std::lock_guard<std::mutex> lock(stateMutex_);

  chunkingEnabled_ = enabled;

  if (enabled)
  {
    maxChunkLength_ = maxChunkLength;
    random_.seed(seed);
  }
  else
  {
    for (uint8_t device = 0U; device < deviceCount_; device++) devices_[device].stream.clear();
  }

  return (true);
}

/**
 * @brief  Queue bytes for delivery by the next update(), and log them.
 *
 * @param  sender   The transmitting device.
 * @param  bytesPtr Bytes to send. Copied before returning.
 * @param  length   Number of bytes to send.
 *
 * @return false, with nothing queued or logged, if the device ID is invalid, bytesPtr
 *         is nullptr or length is 0. A disconnected sender still returns true.
 */
bool Router::transmit(const DeviceID_t sender, const uint8_t *bytesPtr, const uint16_t length)
{
  if ((bytesPtr == nullptr) || (length == 0U)) return (false); /* Early Return */

  TrafficLogEntry entry;

  entry.timeMs = getMillis();
  entry.sender = sender;
  entry.bytes.assign(bytesPtr, bytesPtr + length);
  entry.packets = decodePackets(entry.bytes);

  {
    std::lock_guard<std::mutex> lock(stateMutex_);

    if (!isValidDevice(sender)) return (false); /* Early Return */

    entry.sequence        = nextSequence_++;
    entry.senderConnected = devices_[sender].connected;

    Transmission transmission;

    transmission.sender          = sender;
    transmission.senderConnected = entry.senderConnected;
    transmission.bytes           = entry.bytes;

    transmitQueue_.push_back(std::move(transmission));

    if (trafficLog_.size() >= ROUTER_TRAFFIC_LOG_CAPACITY) trafficLog_.pop_front();

    trafficLog_.push_back(std::move(entry));
  }

  trafficSignal_.notify_all();

  return (true);
}

/**
 * @brief Deliver queued transmissions.
 *
 * @details Puts every queued transmission on the bus in transmit order, delivering it
 *          to every connected, receiving device with a callback handler (and to the
 *          sender only when echo is on), then calls the sender's
 *          onRouterTransmitComplete() once per transmission. Receive callbacks are
 *          called before transmit complete callbacks, as echoed bytes arrive before a
 *          transmission ends on real hardware. With chunking on, receivers get one
 *          piece of their waiting bytes.
 *
 *          Callbacks are called with deliveryMutex_ held but not stateMutex_, so they
 *          may call transmit().
 */
void Router::update(void)
{
  std::lock_guard<std::mutex> deliveryLock(deliveryMutex_);

  std::vector<Delivery>              deliveries;
  std::vector<RouterCallbackHandler *> transmitCompletes;

  {
    std::lock_guard<std::mutex> lock(stateMutex_);

    for (const Transmission &transmission : transmitQueue_)
    {
      RouterCallbackHandler *senderHandlerPtr {devices_[transmission.sender].handlerPtr};

      if (senderHandlerPtr != nullptr) transmitCompletes.push_back(senderHandlerPtr);

      if (!transmission.senderConnected) continue;

      for (uint8_t device = 0U; device < deviceCount_; device++)
      {
        Device &receiver {devices_[device]};

        if (!isReceiver(receiver)) continue;

        if (!echoEnabled_ && (device == transmission.sender)) continue;

        if (chunkingEnabled_)
        {
          receiver.stream.insert(receiver.stream.end(), transmission.bytes.begin(), transmission.bytes.end());
        }
        else
        {
          deliveries.push_back(Delivery{receiver.handlerPtr, transmission.bytes});
        }
      }
    }

    transmitQueue_.clear();

    if (chunkingEnabled_)
    {
      for (uint8_t device = 0U; device < deviceCount_; device++)
      {
        Device &receiver {devices_[device]};

        if (receiver.stream.empty()) continue;

        const uint16_t waitingLength {static_cast<uint16_t>(receiver.stream.size())};
        const uint16_t randomLength  {getRandomChunkLength()};
        const uint16_t chunkLength   {(randomLength < waitingLength) ? randomLength : waitingLength};

        deliveries.push_back(Delivery{receiver.handlerPtr,
                                        std::vector<uint8_t>(receiver.stream.begin(), receiver.stream.begin() + chunkLength)});
        receiver.stream.erase(receiver.stream.begin(), receiver.stream.begin() + chunkLength);
      }
    }
  }

  for (const Delivery &delivery : deliveries)
  {
    if (delivery.handlerPtr == nullptr) continue;

    delivery.handlerPtr->onRouterReceive(delivery.bytes.data(), static_cast<uint16_t>(delivery.bytes.size()));
  }

  for (RouterCallbackHandler *handlerPtr : transmitCompletes)
  {
    if (handlerPtr == nullptr) continue;

    handlerPtr->onRouterTransmitComplete();
  }
}

/**
 * @brief  Wait until there is something for update() to deliver.
 *
 * @param  timeoutMs Longest time to wait. 0 checks without waiting.
 *
 * @return true if a transmission is queued or bytes are waiting for chunked delivery.
 *         Returns as soon as either is true, including straight away.
 */
bool Router::waitForTraffic(const uint32_t timeoutMs)
{
  const std::chrono::steady_clock::time_point deadline {std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs)};

  std::unique_lock<std::mutex> lock(stateMutex_);

  /* Loops because a condition variable can wake without being notified */
  while (!hasPendingTraffic())
  {
    if (trafficSignal_.wait_until(lock, deadline) == std::cv_status::timeout) return (hasPendingTraffic()); /* Early Return */
  }

  return (true);
}

/**
 * @return A copy of the traffic log, oldest first. Holds the last
 *         ROUTER_TRAFFIC_LOG_CAPACITY transmissions.
 */
std::vector<TrafficLogEntry> Router::getTrafficLog(void) const
{
  std::lock_guard<std::mutex> lock(stateMutex_);

  return (std::vector<TrafficLogEntry>(trafficLog_.begin(), trafficLog_.end()));
}

/**
 * @brief Print the most recent traffic log entries to stdout, oldest first, one line
 *        per packet followed by the raw bytes.
 *
 * @param lastEntries Number of entries to print. Clamped to the log size.
 */
void Router::printTrafficLog(const uint16_t lastEntries) const
{
  std::lock_guard<std::mutex> lock(stateMutex_);

  const uint16_t logSize    {static_cast<uint16_t>(trafficLog_.size())};
  const uint16_t firstEntry {(logSize > lastEntries) ? static_cast<uint16_t>(logSize - lastEntries) : static_cast<uint16_t>(0U)};

  for (uint16_t entryIndex = firstEntry; entryIndex < logSize; entryIndex++)
  {
    const TrafficLogEntry &entry {trafficLog_[entryIndex]};

    printf("#%-6u %8u ms  %-10s %3u bytes%s\n",
           static_cast<unsigned>(entry.sequence),
           static_cast<unsigned>(entry.timeMs),
           devices_[entry.sender].name,
           static_cast<unsigned>(entry.bytes.size()),
           entry.senderConnected ? "" : "  (sender disconnected)");

    for (const PacketInfo_t &packet : entry.packets)
    {
      if (!packet.decoded)
      {
        printf("    undecoded frame\n");

        continue;
      }

      const char *typeName {(packet.messageType < Atams::NUMBER_OF_MESSAGE_TYPES) ? s_messageTypeNames[packet.messageType] : "INVALID"};

      printf("    %-21s node %3u  sync %3u  payload %3u\n",
             typeName,
             static_cast<unsigned>(packet.nodeID),
             static_cast<unsigned>(packet.syncCount),
             static_cast<unsigned>(packet.payloadLength));
    }

    printf("   ");

    for (const uint8_t byte : entry.bytes) printf(" %02X", static_cast<unsigned>(byte));

    printf("\n");
  }

  static_cast<void>(fflush(stdout));
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

/**
 * @note Caller must hold stateMutex_.
 */
bool Router::isValidDevice(const DeviceID_t device) const
{
  return (device < deviceCount_);
}

/**
 * @return true if the device currently takes delivery of bytes on the bus.
 *
 * @note   Caller must hold stateMutex_.
 */
bool Router::isReceiver(const Device &device) const
{
  return (device.connected && device.receiving && (device.handlerPtr != nullptr));
}

/**
 * @note Caller must hold stateMutex_.
 */
bool Router::hasPendingTraffic(void) const
{
  if (!transmitQueue_.empty()) return (true); /* Early Return */

  for (uint8_t device = 0U; device < deviceCount_; device++)
  {
    if (!devices_[device].stream.empty()) return (true); /* Early Return */
  }

  return (false);
}

/**
 * @brief  Get a piece length from 1 to maxChunkLength_. Uses the raw std::mt19937
 *         output, which the standard fully specifies, so a seed gives the same pieces on
 *         every standard library (std::uniform_int_distribution does not).
 *
 * @note   Caller must hold stateMutex_.
 */
uint16_t Router::getRandomChunkLength(void)
{
  return (static_cast<uint16_t>(1U + (random_() % maxChunkLength_)));
}

/**
 * @brief  Split a transmission into COBS frames and read each packet header.
 */
std::vector<PacketInfo_t> Router::decodePackets(const std::vector<uint8_t> &bytes)
{
  const uint16_t            length {static_cast<uint16_t>(bytes.size())};
  std::vector<PacketInfo_t> packets;
  std::vector<uint8_t>      decoded(length);
  uint16_t                  frameStart {0U};

  for (uint16_t index = 0U; index < length; index++)
  {
    if (bytes[index] != Atams::EOL_BYTE) continue;

    const uint16_t              frameLength {static_cast<uint16_t>(index + 1U - frameStart)};
    const Atams::COBS::Result_t result      {Atams::COBS::decode(&bytes[frameStart], frameLength, decoded.data(), length)};

    PacketInfo_t packet;

    if ((result.status == Atams::COBS::ERROR_NONE) && (result.outputLength >= Atams::PACKET_HEADER_SIZE))
    {
      packet.decoded       = true;
      packet.messageType   = decoded[Atams::HEADER_INDEX_MSG_TYPE];
      packet.syncCount     = decoded[Atams::HEADER_INDEX_SYNC];
      packet.nodeID        = decoded[Atams::HEADER_INDEX_NODE_ID];
      packet.payloadLength = static_cast<uint16_t>(result.outputLength - Atams::PACKET_HEADER_SIZE);
    }

    packets.push_back(packet);

    frameStart = static_cast<uint16_t>(index + 1U);
  }

  /* Trailing bytes with no frame terminator */
  if (frameStart < length) packets.push_back(PacketInfo_t{});

  return (packets);
}

} /* End Namespace - AtamsTestHarness */

/**
  * @}End of File
  */
