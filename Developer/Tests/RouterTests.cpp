/**
  ******************************************************************************
  * @file    RouterTests.cpp
  *
  * @author  D. Baines
  *
  * @brief   Checks of the harness router's delivery rules, run directly against
  *          AtamsTestHarness::Router with no Atams libraries attached. A router bug
  *          would otherwise show up as a library bug in every other test.
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

#include <chrono>
#include <functional>
#include <thread>
#include <vector>

#include "Harness/Assertions.hpp"
#include "Harness/HarnessTypedefs.hpp"
#include "Harness/Router.hpp"
#include "Harness/RouterCallbackHandler.hpp"
#include "Harness/TestUtilities.hpp"
#include "Shared/AtamsTypedefs.hpp"
#include "Shared/Utilities/COBS.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTests {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

/* Upper bound on update() calls needed to drain the router in these tests */
static constexpr uint16_t MAX_DRAIN_UPDATES {1000U};

/* Maximum chunk length used when chunking is enabled in these tests */
static constexpr uint16_t CHUNK_MAX_LENGTH {5U};

/*************************************************************************************/
/* PRIVATE CLASS DEFINITIONS                                                         */
/*************************************************************************************/

/**
 * @brief Router device that records everything the router gives it.
 */
class RecordingDevice :
public AtamsTestHarness::RouterCallbackHandler
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  RecordingDevice(void) = default;

  /* Default Destructor */
  ~RecordingDevice(void) = default;

  /* Copy Constructor */
  RecordingDevice(const RecordingDevice &other) = delete;

  /* Copy Assignment Operator */
  RecordingDevice & operator=(const RecordingDevice &other) = delete;

  /* Move Constructor */
  RecordingDevice(RecordingDevice &&other) = delete;

  /* Move Assignment Operator */
  RecordingDevice & operator=(RecordingDevice &&other) = delete;

  const std::vector<uint8_t> & getReceivedBytes(void) const;

  const std::vector<uint16_t> & getChunkLengths(void) const;

  uint32_t getTransmitCompleteCount(void) const;

  void clear(void);

  private:

  /*-- Private Function Declarations ------------------------------------------------*/

  virtual void onRouterReceive(const uint8_t *bytesPtr, const uint16_t length) override final;

  virtual void onRouterTransmitComplete(void) override final;

  /*-- Private Variables ------------------------------------------------------------*/

  std::vector<uint8_t>  receivedBytes_;              /* All received bytes, in order */
  std::vector<uint16_t> chunkLengths_;               /* Length of each receive callback */
  uint32_t              transmitCompleteCount_ {0U};
};

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

const std::vector<uint8_t> & RecordingDevice::getReceivedBytes(void) const
{
  return (receivedBytes_);
}

const std::vector<uint16_t> & RecordingDevice::getChunkLengths(void) const
{
  return (chunkLengths_);
}

uint32_t RecordingDevice::getTransmitCompleteCount(void) const
{
  return (transmitCompleteCount_);
}

void RecordingDevice::clear(void)
{
  receivedBytes_.clear();
  chunkLengths_.clear();
  transmitCompleteCount_ = 0U;
}

void RecordingDevice::onRouterReceive(const uint8_t *bytesPtr, const uint16_t length)
{
  if (bytesPtr == nullptr) return; /* Early Return */

  receivedBytes_.insert(receivedBytes_.end(), bytesPtr, bytesPtr + length);
  chunkLengths_.push_back(length);
}

void RecordingDevice::onRouterTransmitComplete(void)
{
  transmitCompleteCount_++;
}

/**
 * @brief Add a device to the router with recorder as its callback handler, receiving.
 */
static AtamsTestHarness::DeviceID_t addRecordingDevice(AtamsTestHarness::Router &router, const char *name, RecordingDevice &recorder)
{
  const AtamsTestHarness::DeviceID_t device {router.addDevice(name)};

  HARNESS_REQUIRE(device != AtamsTestHarness::ROUTER_DEVICE_ID_NONE);
  HARNESS_REQUIRE(router.setCallbackHandler(device, &recorder));
  HARNESS_REQUIRE(router.setReceiving(device, true));

  return (device);
}

/**
 * @brief Thread function for testRouterWaitForTraffic: sleep 10 ms, then transmit two
 *        fixed test bytes from sender.
 */
static void transmitTestBytesAfter10Ms(AtamsTestHarness::Router &router, const AtamsTestHarness::DeviceID_t sender)
{
  const uint8_t bytes[] {0x01U, 0x00U};

  std::this_thread::sleep_for(std::chrono::milliseconds(10));
  static_cast<void>(router.transmit(sender, bytes, sizeof(bytes)));
}

/**
 * @brief Call router.update() until nothing is left to deliver.
 */
static void drain(AtamsTestHarness::Router &router)
{
  uint16_t updates {0U};

  while (router.waitForTraffic(0U))
  {
    HARNESS_REQUIRE(updates < MAX_DRAIN_UPDATES);

    router.update();
    updates++;
  }
}

/**
 * @brief Send the transmissions from one device to another with chunking on, and
 *        record what arrives in receiver.
 */
static void runChunkedTransfer(const uint32_t                           seed,
                               const std::vector<std::vector<uint8_t>> &transmissions,
                               RecordingDevice                         &receiver)
{
  AtamsTestHarness::Router router;
  RecordingDevice          sender;

  const AtamsTestHarness::DeviceID_t senderDevice {addRecordingDevice(router, "Sender", sender)};
  static_cast<void>(addRecordingDevice(router, "Receiver", receiver));

  router.setEcho(false);
  HARNESS_REQUIRE(router.setChunking(true, seed, CHUNK_MAX_LENGTH));

  for (const std::vector<uint8_t> &transmission : transmissions)
  {
    HARNESS_REQUIRE(router.transmit(senderDevice, transmission.data(), static_cast<uint16_t>(transmission.size())));
  }

  router.update();

  /* One piece per update, so the first update must not deliver everything */
  HARNESS_CHECK(receiver.getReceivedBytes().size() <= CHUNK_MAX_LENGTH);

  drain(router);

  /* Check every transmission completes */
  HARNESS_CHECK_EQ(sender.getTransmitCompleteCount(), static_cast<uint32_t>(transmissions.size()));
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief Every device hears every transmission, only after update(), echo follows
 *        setEcho(), and transmit completion fires once per transmission.
 */
void testRouterBroadcast(const AtamsTestHarness::TestOptions_t &options)
{
  static_cast<void>(options);

  AtamsTestHarness::Router router;
  RecordingDevice          hubRecorder;
  RecordingDevice          node1Recorder;
  RecordingDevice          node2Recorder;

  const AtamsTestHarness::DeviceID_t hub      {addRecordingDevice(router, "Hub",   hubRecorder)};
  const AtamsTestHarness::DeviceID_t node1    {addRecordingDevice(router, "Node1", node1Recorder)};
  static_cast<void>(addRecordingDevice(router, "Node2", node2Recorder));
  const AtamsTestHarness::DeviceID_t injector {router.addDevice("Injector")}; /* No callback handler */

  HARNESS_REQUIRE(injector != AtamsTestHarness::ROUTER_DEVICE_ID_NONE);

  const std::vector<uint8_t> hubBytes      {0x01U, 0x02U, 0x03U};
  const std::vector<uint8_t> node1Bytes    {0x04U, 0x05U};
  const std::vector<uint8_t> injectorBytes {0x06U, 0x07U};

  /* Nothing moves until update() */
  HARNESS_CHECK(!router.waitForTraffic(0U));
  HARNESS_REQUIRE(router.transmit(hub, hubBytes.data(), static_cast<uint16_t>(hubBytes.size())));
  HARNESS_CHECK(router.waitForTraffic(0U));
  HARNESS_CHECK(node1Recorder.getReceivedBytes().empty());
  HARNESS_CHECK_EQ(hubRecorder.getTransmitCompleteCount(), 0U);

  router.update();

  /* Check update() drained the queue */
  HARNESS_CHECK(!router.waitForTraffic(0U));

  /* Check every device heard the transmission */
  HARNESS_CHECK(hubRecorder.getReceivedBytes()   == hubBytes); /* Echo is on by default */
  HARNESS_CHECK(node1Recorder.getReceivedBytes() == hubBytes);
  HARNESS_CHECK(node2Recorder.getReceivedBytes() == hubBytes);

  /* Check delivery arrived in one piece with chunking off */
  HARNESS_CHECK_EQ(node1Recorder.getChunkLengths().size(), 1U);

  /* Check only the sender's transmission completes */
  HARNESS_CHECK_EQ(hubRecorder.getTransmitCompleteCount(),   1U);
  HARNESS_CHECK_EQ(node1Recorder.getTransmitCompleteCount(), 0U);

  /* Without echo the sender hears nothing */
  router.setEcho(false);
  HARNESS_REQUIRE(router.transmit(node1, node1Bytes.data(), static_cast<uint16_t>(node1Bytes.size())));
  router.update();

  /* Check sender received nothing new, others received it after the earlier bytes */
  HARNESS_CHECK(node1Recorder.getReceivedBytes() == hubBytes);
  HARNESS_CHECK(node2Recorder.getReceivedBytes() == (std::vector<uint8_t>{0x01U, 0x02U, 0x03U, 0x04U, 0x05U}));
  HARNESS_CHECK(hubRecorder.getReceivedBytes()   == (std::vector<uint8_t>{0x01U, 0x02U, 0x03U, 0x04U, 0x05U}));

  /* Only the sender's transmission completes */
  HARNESS_CHECK_EQ(node1Recorder.getTransmitCompleteCount(), 1U);
  HARNESS_CHECK_EQ(hubRecorder.getTransmitCompleteCount(),   1U); /* Unchanged from its own transmission */
  HARNESS_CHECK_EQ(node2Recorder.getTransmitCompleteCount(), 0U);

  /* A test can inject raw bytes from a device with no handler */
  HARNESS_REQUIRE(router.transmit(injector, injectorBytes.data(), static_cast<uint16_t>(injectorBytes.size())));
  router.update();

  /* Check injected bytes are delivered */
  HARNESS_CHECK(node1Recorder.getReceivedBytes() == (std::vector<uint8_t>{0x01U, 0x02U, 0x03U, 0x06U, 0x07U}));

  /* Invalid arguments */
  HARNESS_CHECK(!router.transmit(hub, nullptr, 1U));
  HARNESS_CHECK(!router.transmit(hub, hubBytes.data(), 0U));
  HARNESS_CHECK(!router.transmit(AtamsTestHarness::ROUTER_DEVICE_ID_NONE, hubBytes.data(), 1U));
  HARNESS_CHECK(!router.setCallbackHandler(AtamsTestHarness::ROUTER_DEVICE_ID_NONE, &hubRecorder));
  HARNESS_CHECK(!router.setReceiving(AtamsTestHarness::ROUTER_DEVICE_ID_NONE, true));
  HARNESS_CHECK(!router.waitForTraffic(0U));
}

/**
 * @brief Chunked delivery arrives intact and in order, in pieces no longer than the
 *        maximum, over several updates, and the same seed gives the same pieces.
 */
void testRouterChunking(const AtamsTestHarness::TestOptions_t &options)
{
  const std::vector<std::vector<uint8_t>> transmissions
  {
    {0x10U, 0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U, 0x18U, 0x19U, 0x1AU, 0x00U},
    {0x20U},
    {0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x00U},
  };

  std::vector<uint8_t> expected;

  for (const std::vector<uint8_t> &transmission : transmissions)
  {
    expected.insert(expected.end(), transmission.begin(), transmission.end());
  }

  RecordingDevice first;
  RecordingDevice second;

  runChunkedTransfer(options.seed, transmissions, first);
  runChunkedTransfer(options.seed, transmissions, second);

  /* Check bytes arrive intact and in order */
  HARNESS_CHECK(first.getReceivedBytes() == expected);

  /* Check transmissions were split into multiple pieces */
  HARNESS_CHECK(first.getChunkLengths().size() > transmissions.size());

  /* Check every piece is within the maximum chunk length */
  for (const uint16_t chunkLength : first.getChunkLengths())
  {
    HARNESS_CHECK((chunkLength >= 1U) && (chunkLength <= CHUNK_MAX_LENGTH));
  }

  /* Check same seed gives the same pieces */
  HARNESS_CHECK(second.getChunkLengths() == first.getChunkLengths());
}

/**
 * @brief Disconnected devices neither send nor receive but their transmissions still
 *        complete, and disconnecting or stopping receive drops bytes still waiting.
 */
void testRouterConnection(const AtamsTestHarness::TestOptions_t &options)
{
  AtamsTestHarness::Router router;
  RecordingDevice          hubRecorder;
  RecordingDevice          node1Recorder;
  RecordingDevice          node2Recorder;

  const AtamsTestHarness::DeviceID_t hub   {addRecordingDevice(router, "Hub",   hubRecorder)};
  const AtamsTestHarness::DeviceID_t node1 {addRecordingDevice(router, "Node1", node1Recorder)};
  static_cast<void>(addRecordingDevice(router, "Node2", node2Recorder));

  const std::vector<uint8_t> bytes {0x01U, 0x02U, 0x03U, 0x04U, 0x05U, 0x06U, 0x07U, 0x08U,
                                    0x09U, 0x0AU, 0x0BU, 0x0CU, 0x0DU, 0x0EU, 0x0FU, 0x00U};

  router.setEcho(false);

  /* Disconnected receiver */
  HARNESS_CHECK(router.isConnected(node1));
  HARNESS_REQUIRE(router.setConnected(node1, false));
  HARNESS_CHECK(!router.isConnected(node1));
  HARNESS_REQUIRE(router.transmit(hub, bytes.data(), static_cast<uint16_t>(bytes.size())));
  router.update();
  HARNESS_CHECK(node1Recorder.getReceivedBytes().empty());
  HARNESS_CHECK(node2Recorder.getReceivedBytes() == bytes);

  /* Disconnected sender reaches no one, but its transmission completes */
  HARNESS_REQUIRE(router.transmit(node1, bytes.data(), static_cast<uint16_t>(bytes.size())));
  router.update();
  HARNESS_CHECK_EQ(hubRecorder.getReceivedBytes().size(), 0U);
  HARNESS_CHECK_EQ(node2Recorder.getReceivedBytes().size(), bytes.size());
  HARNESS_CHECK_EQ(node1Recorder.getTransmitCompleteCount(), 1U);
  HARNESS_CHECK(!router.getTrafficLog().back().senderConnected);

  /* Reconnected */
  HARNESS_REQUIRE(router.setConnected(node1, true));
  HARNESS_REQUIRE(router.transmit(hub, bytes.data(), static_cast<uint16_t>(bytes.size())));
  router.update();
  HARNESS_CHECK(node1Recorder.getReceivedBytes() == bytes);

  /* Disconnecting part way through a chunked delivery drops the rest */
  node1Recorder.clear();
  node2Recorder.clear();
  HARNESS_REQUIRE(router.setChunking(true, options.seed, CHUNK_MAX_LENGTH));
  HARNESS_REQUIRE(router.transmit(hub, bytes.data(), static_cast<uint16_t>(bytes.size())));
  router.update();
  HARNESS_REQUIRE(router.setConnected(node1, false));
  HARNESS_REQUIRE(router.setConnected(node1, true));
  drain(router);
  HARNESS_CHECK((node1Recorder.getReceivedBytes().size() >= 1U) && (node1Recorder.getReceivedBytes().size() <= CHUNK_MAX_LENGTH));
  HARNESS_CHECK(node2Recorder.getReceivedBytes() == bytes);

  /* Stopping receive part way through a chunked delivery drops the rest */
  node1Recorder.clear();
  HARNESS_REQUIRE(router.transmit(hub, bytes.data(), static_cast<uint16_t>(bytes.size())));
  router.update();
  HARNESS_REQUIRE(router.setReceiving(node1, false));
  HARNESS_REQUIRE(router.setReceiving(node1, true));
  drain(router);
  HARNESS_CHECK((node1Recorder.getReceivedBytes().size() >= 1U) && (node1Recorder.getReceivedBytes().size() <= CHUNK_MAX_LENGTH));

  /* Invalid arguments */
  HARNESS_CHECK(!router.setChunking(true, 0U, 0U));
  HARNESS_CHECK(!router.setConnected(AtamsTestHarness::ROUTER_DEVICE_ID_NONE, false));
  HARNESS_CHECK(!router.isConnected(AtamsTestHarness::ROUTER_DEVICE_ID_NONE));
}

/**
 * @brief The traffic log records every transmission with its sender and bytes,
 *        decodes packet headers, and keeps only the newest entries.
 */
void testRouterTrafficLog(const AtamsTestHarness::TestOptions_t &options)
{
  static_cast<void>(options);

  AtamsTestHarness::Router router;

  const AtamsTestHarness::DeviceID_t hub  {router.addDevice("Hub")};
  const AtamsTestHarness::DeviceID_t node {router.addDevice("Node")};

  /* A response from Node ID 3, with a zero in the payload so COBS has work to do */
  const uint8_t packet[Atams::PACKET_HEADER_SIZE + 4U]
  {
    Atams::MESSAGE_RESPONSE, 7U, 0xAAU, 0xBBU, 0xCCU, 0xDDU, 3U,
    0x11U, 0x00U, 0x22U, 0x33U
  };
  uint8_t encoded[sizeof(packet) + 8U] {};

  const Atams::COBS::Result_t encodeResult {Atams::COBS::encode(packet, sizeof(packet), encoded, sizeof(encoded))};

  HARNESS_REQUIRE(encodeResult.status == Atams::COBS::ERROR_NONE);

  HARNESS_REQUIRE(router.transmit(node, encoded, encodeResult.outputLength));

  std::vector<AtamsTestHarness::TrafficLogEntry> log {router.getTrafficLog()};

  /* Check transmission is logged immediately */
  HARNESS_REQUIRE_EQ(log.size(), 1U);

  /* Check entry records sequence, sender and raw bytes */
  HARNESS_CHECK_EQ(log[0U].sequence, 0U);
  HARNESS_CHECK_EQ(log[0U].sender, node);
  HARNESS_CHECK(log[0U].senderConnected);
  HARNESS_CHECK(log[0U].bytes == std::vector<uint8_t>(encoded, encoded + encodeResult.outputLength));

  /* Check packet header is decoded */
  HARNESS_REQUIRE_EQ(log[0U].packets.size(), 1U);
  HARNESS_CHECK(log[0U].packets[0U].decoded);
  HARNESS_CHECK_EQ(log[0U].packets[0U].messageType, static_cast<uint8_t>(Atams::MESSAGE_RESPONSE));
  HARNESS_CHECK_EQ(log[0U].packets[0U].syncCount, 7U);
  HARNESS_CHECK_EQ(log[0U].packets[0U].nodeID, 3U);
  HARNESS_CHECK_EQ(log[0U].packets[0U].payloadLength, 4U);

  /* Logging is independent of delivery: nothing has been delivered yet */
  HARNESS_CHECK(router.waitForTraffic(0U));
  router.update();

  /* Only the newest entries are kept */
  const uint32_t extra {5U};

  for (uint32_t index = 0U; index < (AtamsTestHarness::ROUTER_TRAFFIC_LOG_CAPACITY + extra); index++)
  {
    HARNESS_REQUIRE(router.transmit(hub, encoded, encodeResult.outputLength));
  }

  log = router.getTrafficLog();

  /* Check log is capped and the oldest entries were dropped */
  HARNESS_CHECK_EQ(log.size(), static_cast<size_t>(AtamsTestHarness::ROUTER_TRAFFIC_LOG_CAPACITY));
  HARNESS_CHECK_EQ(log.front().sequence, 1U + extra);
  HARNESS_CHECK_EQ(log.back().sequence, extra + AtamsTestHarness::ROUTER_TRAFFIC_LOG_CAPACITY);
  HARNESS_CHECK_EQ(log.back().sender, hub);
}

/**
 * @brief waitForTraffic() times out when nothing is sent, and wakes promptly when
 *        another thread transmits.
 */
void testRouterWaitForTraffic(const AtamsTestHarness::TestOptions_t &options)
{
  static_cast<void>(options);

  AtamsTestHarness::Router router;

  const AtamsTestHarness::DeviceID_t hub {router.addDevice("Hub")};

  uint32_t startTime {AtamsTestHarness::getMillis()};

  /* Check wait times out after the full timeout with no traffic */
  HARNESS_CHECK(!router.waitForTraffic(20U));
  HARNESS_CHECK((AtamsTestHarness::getMillis() - startTime) >= 20U);

  /* Transmit from another thread while waiting */
  std::thread transmitter(&transmitTestBytesAfter10Ms, std::ref(router), hub);

  startTime = AtamsTestHarness::getMillis();

  const bool     woken   {router.waitForTraffic(2000U)};
  const uint32_t elapsed {AtamsTestHarness::getMillis() - startTime};

  transmitter.join();

  /* Check wait woke on the transmission, well before the timeout */
  HARNESS_CHECK(woken);
  HARNESS_CHECK(elapsed < 1000U);
}

} /* End Namespace - AtamsTests */

/**
  * @}End of File
  */
