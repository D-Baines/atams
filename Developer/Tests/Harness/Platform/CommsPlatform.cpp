/**
  ******************************************************************************
  * @file    CommsPlatform.cpp
  *
  * @author  D. Baines
  *
  * @brief   Harness Platform layer for the Atams Node Comms Core.
  *
  * @details Library misuse that real hardware would not report (an out of range NVM
  *          access, a buffer changed while being sent, unpaired configuration calls) is
  *          reported with AtamsTestHarness::reportBackgroundFailure(), never by throwing,
  *          as these functions are called from inside the library.
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

#include "CommsPlatform.hpp"

#include <atomic>
#include <cstring>
#include <mutex>
#include <vector>

#include "Harness/Assertions.hpp"
#include "Harness/BinarySemaphore.hpp"
#include "Harness/RouterCallbackHandler.hpp"
#include "Harness/TestUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NodePlatform {

/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static_assert((NVM_STORAGE_SIZE % NVM_UNIT_SIZE) == 0U, "NVM_STORAGE_SIZE must be a whole number of NVM units");

static constexpr uint32_t NVM_NUMBER_OF_UNITS {NVM_STORAGE_SIZE / NVM_UNIT_SIZE};

static constexpr uint8_t  NVM_ERASED_VALUE    {0xFFU};

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

/**
 * @brief The Node's connection to its router device. Receives the router's callbacks and
 *        passes them to the library's receive callback; tracks the single transmit slot.
 */
class CommsPeripheral :
public AtamsTestHarness::RouterCallbackHandler
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  CommsPeripheral(void) = default;

  /* Default Destructor */
  virtual ~CommsPeripheral(void) override = default;

  /* Copy Constructor */
  CommsPeripheral(const CommsPeripheral &other) = delete;

  /* Copy Assignment Operator */
  CommsPeripheral & operator=(const CommsPeripheral &other) = delete;

  /* Move Constructor */
  CommsPeripheral(CommsPeripheral &&other) = delete;

  /* Move Assignment Operator */
  CommsPeripheral & operator=(CommsPeripheral &&other) = delete;

  void attach(AtamsTestHarness::Router &router, const AtamsTestHarness::DeviceID_t device);

  bool beginReceive(const CommsReceiveCallback_t receiveCallback);

  void stopReceive(void);

  bool transmit(const uint8_t * const bytesPtr, const uint16_t length);

  bool isTransmitBusy(void);

  void reset(void);

  private:

  /*-- Private Function Declarations ------------------------------------------------*/

  virtual void onRouterReceive(const uint8_t *bytesPtr, const uint16_t length) override final;

  virtual void onRouterTransmitComplete(void) override final;

  /*-- Private Variables ------------------------------------------------------------*/

  /* Set on the test thread before the Node starts. receiveCallback_ is only written while the
     router is not delivering to this device: Router::setReceiving() waits for deliveries to end */
  AtamsTestHarness::Router     *routerPtr_       {nullptr};
  AtamsTestHarness::DeviceID_t  device_          {AtamsTestHarness::ROUTER_DEVICE_ID_NONE};
  CommsReceiveCallback_t        receiveCallback_ {nullptr};

  /* transmit() and its completion run on different threads */
  std::mutex                    transmitMutex_;
  bool                          transmitBusy_      {false};
  const uint8_t                *transmitBufferPtr_ {nullptr}; /* The library buffer being sent */
  std::vector<uint8_t>          transmitCopy_;                /* Its bytes when sent, to catch the library changing them early */
};

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static CommsPeripheral                   s_commsPeripheral;

static std::mutex                        s_varStorageMutex;
static std::mutex                        s_commsBufferMutexes[NUMBER_OF_COMMS_PERIPHERALS];
static AtamsTestHarness::BinarySemaphore s_receiveSemaphore;

/* An unwritten unit reads as erased, so the zero-initialised state is fully erased NVM */
static std::mutex                        s_nvmMutex;
static uint8_t                           s_nvm[NVM_STORAGE_SIZE];
static bool                              s_nvmUnitWritten[NVM_NUMBER_OF_UNITS];

/* Mirrors the library's configuration state, so it is not cleared between tests */
static std::atomic<bool>                 s_inConfigurationState {false};

/*************************************************************************************/
/* CommsPeripheral - PUBLIC FUNCTION DEFINITIONS                                     */
/*************************************************************************************/

void CommsPeripheral::attach(AtamsTestHarness::Router &router, const AtamsTestHarness::DeviceID_t device)
{
  routerPtr_ = &router;
  device_    = device;

  if (router.setCallbackHandler(device, this) == false)
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "attachToRouter: invalid router device");
  }
}

/**
 * @brief The callback is stored before reception starts, as the router may deliver straight away.
 */
bool CommsPeripheral::beginReceive(const CommsReceiveCallback_t receiveCallback)
{
  if ((routerPtr_ == nullptr) || (receiveCallback == nullptr)) return (false); /* Early Return */

  receiveCallback_ = receiveCallback;

  return (routerPtr_->setReceiving(device_, true));
}

/**
 * @brief Safe when not attached or not receiving.
 */
void CommsPeripheral::stopReceive(void)
{
  if (routerPtr_ == nullptr) return; /* Early Return */

  static_cast<void>(routerPtr_->setReceiving(device_, false));

  receiveCallback_ = nullptr;
}

/**
 * @brief Single transmit slot: fails while the previous transmission is still being sent.
 */
bool CommsPeripheral::transmit(const uint8_t * const bytesPtr, const uint16_t length)
{
  if ((routerPtr_ == nullptr) || (bytesPtr == nullptr)) return (false); /* Early Return */

  transmitMutex_.lock();

  if (transmitBusy_)
  {
    transmitMutex_.unlock();
    return (false); /* Early Return */
  }

  transmitBusy_      = true;
  transmitBufferPtr_ = bytesPtr;
  transmitCopy_.assign(bytesPtr, bytesPtr + length);

  transmitMutex_.unlock();

  /* Not holding transmitMutex_, as the router takes its own locks */
  if (routerPtr_->transmit(device_, bytesPtr, length) == false)
  {
    std::lock_guard<std::mutex> lock(transmitMutex_);

    transmitBusy_      = false;
    transmitBufferPtr_ = nullptr;

    return (false); /* Early Return */
  }

  return (true);
}

bool CommsPeripheral::isTransmitBusy(void)
{
  std::lock_guard<std::mutex> lock(transmitMutex_);

  return (transmitBusy_);
}

/**
 * @brief Forgets the previous test's router without calling it, as it has been destroyed.
 */
void CommsPeripheral::reset(void)
{
  routerPtr_       = nullptr;
  device_          = AtamsTestHarness::ROUTER_DEVICE_ID_NONE;
  receiveCallback_ = nullptr;

  std::lock_guard<std::mutex> lock(transmitMutex_);

  transmitBusy_      = false;
  transmitBufferPtr_ = nullptr;
  transmitCopy_.clear();
}

/*************************************************************************************/
/* CommsPeripheral - PRIVATE FUNCTION DEFINITIONS                                    */
/*************************************************************************************/

void CommsPeripheral::onRouterReceive(const uint8_t *bytesPtr, const uint16_t length)
{
  if ((receiveCallback_ == nullptr) || (bytesPtr == nullptr)) return; /* Early Return */

  receiveCallback_(COMMS_DEFAULT, bytesPtr, length);
}

/**
 * @brief Checks the library left its buffer alone while it was being sent, then frees the slot.
 */
void CommsPeripheral::onRouterTransmitComplete(void)
{
  std::lock_guard<std::mutex> lock(transmitMutex_);

  if ((transmitBufferPtr_ != nullptr) &&
      (std::memcmp(transmitBufferPtr_, transmitCopy_.data(), transmitCopy_.size()) != 0))
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "transmitBuffer: buffer changed before the transmission completed");
  }

  transmitBusy_      = false;
  transmitBufferPtr_ = nullptr;
}

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

/* Caller must hold s_nvmMutex */
static void eraseNVMNoLock(void)
{
  for (bool &unitWritten : s_nvmUnitWritten)
  {
    unitWritten = false;
  }
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

uint32_t getMillis(void)
{
  return (AtamsTestHarness::getMillis());
}

void acquireVarStorageLock(void)
{
  s_varStorageMutex.lock();
}

void releaseVarStorageLock(void)
{
  s_varStorageMutex.unlock();
}

bool beginReceive(CommsReceiveCallback_t receiveCallback)
{
  return (s_commsPeripheral.beginReceive(receiveCallback));
}

void stopReceive(void)
{
  s_commsPeripheral.stopReceive();
}

/**
 * @brief Delivery only happens in Router::update(), so there is nothing to poll.
 */
void update(void)
{
  /* Do Nothing */
}

bool transmitReady(const CommsPeripheralID_t peripheralID)
{
  if (peripheralID >= NUMBER_OF_COMMS_PERIPHERALS) return (false); /* Early Return */

  return (s_commsPeripheral.isTransmitBusy() == false);
}

/**
 * @brief Non-blocking: queues the bytes on the router, which completes the transmission
 *        in Router::update().
 */
bool transmitBuffer(const CommsPeripheralID_t peripheralID, uint8_t * bufferPtr, const uint16_t length)
{
  if (peripheralID >= NUMBER_OF_COMMS_PERIPHERALS) return (false); /* Early Return */

  return (s_commsPeripheral.transmit(bufferPtr, length));
}

void acquireCommsBufferLock(const CommsPeripheralID_t peripheralToLock)
{
  if (peripheralToLock >= NUMBER_OF_COMMS_PERIPHERALS)
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "acquireCommsBufferLock: invalid peripheral ID");
    return; /* Early Return */
  }

  s_commsBufferMutexes[peripheralToLock].lock();
}

void releaseCommsBufferLock(const CommsPeripheralID_t peripheralToUnlock)
{
  if (peripheralToUnlock >= NUMBER_OF_COMMS_PERIPHERALS)
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "releaseCommsBufferLock: invalid peripheral ID");
    return; /* Early Return */
  }

  s_commsBufferMutexes[peripheralToUnlock].unlock();
}

void acquireWaitOnReceiveSempahore(const uint32_t timeoutInMilliseconds)
{
  s_receiveSemaphore.waitWithTimeout(timeoutInMilliseconds);
}

void releaseWaitOnReceiveSemaphore(void)
{
  s_receiveSemaphore.release();
}

/**
 * @brief Instant erase of the whole NVM.
 */
bool eraseNVM(void)
{
  std::lock_guard<std::mutex> lock(s_nvmMutex);

  eraseNVMNoLock();

  return (true);
}

/**
 * @brief Byte-granular read. Bytes in unwritten units read as erased.
 */
bool readFromNVM(const uint32_t readIndex, uint8_t * outputPtr, const uint32_t readLength)
{
  if ((outputPtr  == nullptr                      ) ||
      (readIndex  >  NVM_STORAGE_SIZE             ) ||
      (readLength > (NVM_STORAGE_SIZE - readIndex)) )
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "readFromNVM: null output or read outside NVM_STORAGE_SIZE");
    return (false); /* Early Return */
  }

  std::lock_guard<std::mutex> lock(s_nvmMutex);

  for (uint32_t byteIndex {0U}; byteIndex < readLength; byteIndex++)
  {
    const uint32_t nvmIndex {readIndex + byteIndex};

    outputPtr[byteIndex] = s_nvmUnitWritten[nvmIndex / NVM_UNIT_SIZE] ? s_nvm[nvmIndex] : NVM_ERASED_VALUE;
  }

  return (true);
}

/**
 * @brief Flash-like write: the unit must be aligned, inside NVM, and erased.
 */
bool writeToNVM(const uint32_t writeIndex, const uint8_t (&nvmUnit)[NodePlatform::NVM_UNIT_SIZE])
{
  if (((writeIndex % NVM_UNIT_SIZE) != 0U                  ) ||
      ( writeIndex > (NVM_STORAGE_SIZE - NVM_UNIT_SIZE)    ) )
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "writeToNVM: unaligned write or write outside NVM_STORAGE_SIZE");
    return (false); /* Early Return */
  }

  const uint32_t unitIndex {writeIndex / NVM_UNIT_SIZE};

  std::lock_guard<std::mutex> lock(s_nvmMutex);

  if (s_nvmUnitWritten[unitIndex])
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "writeToNVM: write to a unit that has not been erased");
    return (false); /* Early Return */
  }

  std::memcpy(&s_nvm[writeIndex], nvmUnit, NVM_UNIT_SIZE);

  s_nvmUnitWritten[unitIndex] = true;

  return (true);
}

/**
 * @brief Always accepts. Checks the library pairs entry and exit.
 */
bool enterConfigurationState(void)
{
  if (s_inConfigurationState.exchange(true))
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "enterConfigurationState: already in the configuration state");
  }

  return (true);
}

void exitConfigurationState(void)
{
  if (s_inConfigurationState.exchange(false) == false)
  {
    AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "exitConfigurationState: not in the configuration state");
  }
}

/**
 * @brief The router has no bitrates yet, so there is nothing to change.
 */
void setBitrate(const Atams::BitrateOption_t bitrateOption)
{
  static_cast<void>(bitrateOption);
}

/**
 * @brief No harness test resets a Node yet, so an unexpected reset fails the test.
 */
Atams::Error_t resetNode(void)
{
  AtamsTestHarness::reportBackgroundFailure(__FILE__, __LINE__, "resetNode: not supported by the harness");

  return (Atams::ERROR_PLATFORM);
}

/*************************************************************************************/
/* HARNESS FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

/**
 * @brief Connects the Node to a router device. Call once per test, before initSingleCore().
 */
void attachToRouter(AtamsTestHarness::Router &router, const AtamsTestHarness::DeviceID_t device)
{
  s_commsPeripheral.attach(router, device);
}

/**
 * @brief Returns the Platform to its power-on state. Called by the test runner before each
 *        test, when no Node or router threads are running.
 */
void resetForNextTest(void)
{
  s_commsPeripheral.reset();

  std::lock_guard<std::mutex> lock(s_nvmMutex);

  eraseNVMNoLock();
}

} } /* End Namespace - Atams::NodePlatform */


/**
  * @}End of File
  */
