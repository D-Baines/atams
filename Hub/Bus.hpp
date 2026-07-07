/**
  ******************************************************************************
  * @file    Bus.hpp
  *
  * @author  D. Baines
  *
  * @brief   Public interface for an Atams Hub Bus instance.
  *
  * @details The Bus class coordinates communication and synchronisation between 
  *          the Hub and multiple Node instances. All Bus member functions must 
  *          be called from the same thread or context to ensure correct operation.
  *          Node functions (such as @c Node::setVar, @c Node::getVar, and 
  *          @c Node::setRequestPattern) may be called from other threads or
  *          contexts, provided the user has correctly filled the 
  *          multi-threading/concurrency function definitions in the Platform files.
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include <atomic>

#include "../Shared/AtamsTypedefs.hpp"
#include "Developer/CircularBuffer.hpp"
#include "Platform.hpp"
#include "Developer/FramingConstants.hpp"
#include "NodeActions.hpp"
#include "Node.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

/**
 * @class Bus
 *
 * @brief Manages communication with multiple Node instances on an Atams Bus.
 *
 * The Bus class coordinates communication and synchronisation between the Hub and multiple Node instances.
 * All Bus member functions must be called from the same thread or context to ensure correct operation.
 * Node functions (such as @c Node::setVar, @c Node::getVar, and @c Node::setRequestPattern) may be called
 * from other threads or contexts, provided the user has correctly filled the multi-threading/concurrency
 * function definitions in the Platform files.
 *
 */
class   Bus :
private Platform::BusPeripheral
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  Bus(Platform::BusPeripheral::UserData_t userData);

  /* Default Constructor */
  Bus(void) = delete;

  /* Default Destructor */
  ~Bus(void) = default;

  /* Copy Constructor */
  Bus(const Bus &other) = delete;

  /* Copy Assignment Operator */
  Bus & operator=(const Bus &other) = delete;

  /* Move Constructor */
  Bus(Bus &&other) = delete;

  /* Move Assignment Operator */
  Bus & operator=(Bus &&other) = delete;

  Atams::Error_t addNodeToBus(Node &node);

  Atams::Error_t removeNodeFromBus(Node &node);

  Atams::Error_t beginBusInitProcess(void);

  Atams::ProcessState_t updateBusInitProcess(Atams::Error_t &error);

  Atams::Error_t beginUpdateCycle(void);

  Atams::Error_t beginSingleNodeUpdateCycle(Atams::Node &node);

  Atams::ProcessState_t runUpdateCycleSync(Atams::Error_t &error);

  Atams::ProcessState_t runUpdateCycleSyncBlocking(Atams::Error_t &error);

  Atams::ProcessState_t runUpdateCycleAsync(Atams::Error_t &error);

  Atams::ProcessState_t runUpdateCycleAsyncBlocking(Atams::Error_t &error);

  Atams::ProcessState_t runSingleNodeUpdateCycle(Atams::Error_t &error, Atams::Node &node);

  Atams::ProcessState_t runSingleNodeUpdateCycleBlocking(Atams::Error_t &error, Atams::Node &node);

  Atams::Error_t processResponseBuffers(void);

  Atams::Error_t beginSetNodeConfigProcess(const Atams::NodeConfig_t &userConfig);

  Atams::ProcessState_t updateSetNodeConfigProcess(Atams::Error_t &error); 

  /*-- Private ----------------------------------------------------------------------*/

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static inline constexpr uint32_t SET_CONFIG_BUS_TIMEOUT = 100U;
  static inline constexpr uint32_t MAX_CONFIG_RETRIES     = 10U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class InitState: uint8_t
  {
    START                    = 0U,
    VALIDATE_ATAMS_VERSION   = 1U,
    VALIDATE_UNIVERSAL_BLOCK = 2U,
    SET_BUS_IDS              = 3U,
    STORE_BUS_IDS            = 4U,
    VALIDATE_IDS_POST        = 5U,
    COMPLETE                 = 6U,
    ERROR                    = 7U,
  };

  enum class ConfigUpdateState: uint8_t
  {
    START        = 0U,
    WRITE_CONFIG = 2U,
    STORE_CONFIG = 3U,
    SEND_REQUEST = 4U,
    GET_RESPONSE = 5U,
    COMPLETE     = 6U,
    ERROR        = 7U
  };

  enum class UpdateState: uint8_t
  {
    SEND_REQUESTS     = 0U,
    COLLECT_RESPONSES = 1U,
    JOG_NODE          = 2U,
    COMPLETE          = 3U,
    ERROR             = 4U,
  };

  enum class ValidateState: uint8_t
  {
    START        = 0U,
    UPDATE       = 1U,
    VALIDATE     = 2U,
    COMPLETE     = 3U,
    ERROR        = 4U
  }; 

  enum class PollResult : uint8_t
  {
    WAITING = 0U,
    READY   = 1U,
    TIMEOUT = 2U,
  };

  /*-- Private Helper Struct Declarations -----------------------------------------*/

  struct ProcessHandlerBase
  {
    Atams::ProcessState_t processState     {Atams::PROCESS_ERROR};
    Atams::ProcessState_t subProcessState  {Atams::PROCESS_ERROR};
    Atams::Error_t        error            {Atams::ERROR_INIT_ORDER};
    uint32_t              prevEventTime    {0U};
    bool                  allNodesComplete {false};
    Atams::Node          *activeNodePtr    {nullptr};

    void terminate(Atams::Error_t exitError);
  };
  
  template <typename T>
  struct ProcessHandler :
  public ProcessHandlerBase
  {
    ProcessHandler(void) = default;

    T specificState     {T::COMPLETE};
    T nextSpecificState {T::COMPLETE};

    void terminate(Atams::Error_t error);
    void setProcessComplete(void);
    void readyProcess(void);
  };

  /*-- Static Private Variables -----------------------------------------------------*/

  static const GenInfo_t         s_dummyGenInfo;
  static const Node::MemoryMap_t s_dummyMemoryMap;

  /*-- Private Class Objects --------------------------------------------------------*/

  Platform::BinarySemaphore rxSemaphore_;
  Platform::BinarySemaphore txSemaphore_;

  std::atomic<bool> txReady_ {true};

  Atams::CircularBuffer circularBuffer_;
  Node                 *nodePtrs_[Platform::NUMBER_OF_NODES_PER_BUS];
  NodeActions           nodeProcessHandler_;
  Node                  dummyNode_ {0U};
  NodeCallbackHandler  &dummyNodeCallbackHandler_ {dummyNode_};

  Bus::ProcessHandler<Bus::InitState>         initProcessHandler_;
  Bus::ProcessHandler<Bus::ConfigUpdateState> configUpdateProcessHandler_;
  Bus::ProcessHandler<Bus::ValidateState>     validateGenInfoProcessHandler_;
  Bus::ProcessHandler<Bus::UpdateState>       updateProcessHandler_;
  Bus::ProcessHandler<Bus::UpdateState>       singleNodeUpdateProcessHandler_;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t  rxBuffer_     [Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  decodedBuffer_[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint8_t  rawTxBuffer_  [Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint8_t  encodedBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  jogBuffer_    [HEADER_SIZE_HEADER];

  uint16_t initNodeIndex_   {0U};
  uint16_t updateNodeIndex_ {0U};
  uint16_t noOfNodesOnBus_  {0U};
  uint16_t rxLength_        {0U};
  uint16_t decodedLength_   {0U};
  uint16_t encodedLength_   {0U};
  uint8_t  activeSyncCount_ {0U};

  Atams::MessageType_t lastSentMessageType_;

  NodeConfig_t    userConfigToSet_;
  Atams::BusIDs_t busIDsToSet_;

  /*-- Private Function Declarations ------------------------------------------------*/

  Atams::ProcessState_t runUpdateCycleSyncCore(Atams::Error_t &error, bool blocking);

  Atams::ProcessState_t runUpdateCycleAsyncCore(Atams::Error_t &error, bool blocking);

  Atams::ProcessState_t runSingleNodeUpdateCycleCore(Atams::Error_t &error, Atams::Node &node, bool blocking);

  void validateNodeUniversalBlock(Node &node);

  bool safeToRemoveNode(void);

  bool findNodeOnBus(Node &node);

  Atams::Error_t beginUpdateCyclePrivate(void);

  Atams::Error_t beginSingleNodeUpdateCyclePrivate(Atams::Node &node);

  bool nodeHasQueuedRequestData(Atams::Node &node);

  void clearAllBusErrors(void);

  bool pollForRequestTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t requestType, const bool blocking);

  bool pollForJogTransmit(Atams::Node &node, Bus::ProcessHandlerBase &process, const bool blocking);

  bool awaitTxReady(Bus::ProcessHandlerBase &process, const bool blocking);

  void doTransmit(Bus::ProcessHandlerBase &process, const Atams::MessageType_t messageType);

  Bus::PollResult pollForResponse(Atams::Node               &node, 
                                  Bus::ProcessHandlerBase   &process, 
                                  const Atams::MessageType_t expectedResponse, 
                                  const bool                 blocking);

  virtual void rxCallback(uint8_t       *rxBufferPtr,
                          const uint16_t rxBufferLength) override final;

  virtual void txCallback(void) override final;

  bool validateAndStoreResponsePacket(Atams::Node &node, const MessageType_t responseType);

  void startWriteConfigVars(void);

  bool allConfigVarsAcknowledged(void);

  void startReadConfigVars(void);

  Atams::Error_t validateConfigVars(void);

  Atams::Node * getUpdateNodePtr(void);

  bool tryNodeIncrementUpdate(void);

  void startResponseCollectionSync(void);

  void triggerJogSync(void);

  void triggerNextRequestAsync(void);

  void beginInitValidateAtamsVersion(Atams::Node &node);

  void beginInitValidateUniversalBlock(Atams::Node &node);

  void beginInitValidateIDsPost(Atams::Node &node, const Atams::BusIDs_t busIDs);
  
  void beginInitSetBusIDs(Atams::Node &node, Atams::BusIDs_t busIDs);

  void beginInitStore(Atams::Node &node);

  void startNextNodeInit(void);

  void beginSetConfigWrite(void);

  void beginSetConfigStore(void);

  void updateSetConfigSendRequest(void);

  void updateSetConfigGetResponse(void);

  void startNextConfigCycle(const Bus::ConfigUpdateState nextState);
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


