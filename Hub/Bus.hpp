/**
  ******************************************************************************
  * @file    Bus.hpp
  *
  * @author  D. Baines
  *
  * @brief
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
#include "../Shared/AtamsTypedefs.hpp"
#include "Developer/CircularBuffer.hpp"
#include "Platform.hpp"
#include "Developer/NodeActions.hpp"
#include "Node.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class   Bus :
private Platform::BusPeripheral
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Helper Class Definitions ----------------------------------------------*/

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

  void removeNodeFromBus(Node &node);

  Atams::Error_t beginBusInitProcess(void);

  Atams::ProcessState updateBusInitProcess(Atams::Error_t &error);

  Atams::Error_t beginUpdateCycle(void);

  Atams::ProcessState runUpdateCycleSync(Atams::Error_t &error);

  Atams::ProcessState runUpdateCycleAsync(Atams::Error_t &error);

  Atams::Error_t processBuffers(void);

  void beginSetNodeConfigProcess(const NodeUserConfig_t &userConfig);

  Atams::ProcessState updateSetNodeConfigProcess(Atams::Error_t &error); 

  /*-- Private ----------------------------------------------------------------------*/

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static inline constexpr uint32_t SET_CONFIG_BUS_TIMEOUT = 100U;
  static inline constexpr uint32_t MAX_CONFIG_RETRIES     = 10U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class InitState: uint8_t
  {
    START               = 0U,
    VALIDATE_GEN_INFO   = 1U,
    VALIDATE_IDS_PRE    = 2U,
    SET_BUS_IDS         = 3U,
    STORE_BUS_IDS       = 4U,
    VALIDATE_IDS_POST   = 5U,
    COMPLETE            = 6U,
    ERROR               = 7U,
  };

  enum class ConfigUpdateState: uint8_t
  {
    START        = 0U,
    INIT_NODE    = 1U,
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
    ProcessHandlerBase(void) = default;

    Atams::ProcessState processState     = Atams::ProcessState::ERROR;
    Atams::ProcessState subProcessState  = Atams::ProcessState::ERROR;
    Atams::Error_t      error            = Atams::ERROR_INIT_ORDER;
    uint32_t            prevEventTime    = 0U;
    bool                allNodesComplete = false;
  };
  
  template <typename T>
  struct ProcessHandler :
  public ProcessHandlerBase
  {
    ProcessHandler(void) = default;
    
    T specificState      = T::COMPLETE;
    T nextSpecificState  = T::COMPLETE;

    void terminate(Atams::Error_t error);
    void setProcessComplete(void);
    bool getProcessTerminated(void);
    void resetProcess(void);
  };

  /*-- Static Private Variables -----------------------------------------------------*/

  static const GenInfo_t         blankGenInfo_;
  static const Node::MemoryMap_t dummyMemoryMap_;

  /*-- Private Class Objects --------------------------------------------------------*/
                                      
  Atams::CircularBuffer circularBuffer_;
  Node                 *nodePtrs_[Platform::NUMBER_OF_NODES_PER_BUS];
  NodeActions           nodeProcessHandler_;
  Node                  dummyNode_ = {0U};
  NodeCallbackHandler  &dummyNodeCallbackHandler_ = dummyNode_;

  Bus::ProcessHandler<Bus::InitState>         initProcessHandler_;
  Bus::ProcessHandler<Bus::ConfigUpdateState> configUpdateProcessHandler_;
  Bus::ProcessHandler<Bus::ValidateState>     validateGenInfoProcessHandler_;
  Bus::ProcessHandler<Bus::UpdateState>       updateProcessHandler_;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t  rxBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  decodedBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  encodedBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  jogBuffer_[MESH_SIZE_HEADER];

  uint16_t initNodeIndex_   {0U};
  uint16_t updateNodeIndex_ {0U};
  uint16_t noOfNodesOnBus_  {0U};
  uint16_t rxLength_        {0U};
  uint16_t decodedLength_   {0U};
  uint16_t encodedLength_   {0U};
  uint8_t  activeSyncCount_ {0U};

  Atams::MessageType_t lastSentMessageType_;

  NodeUserConfig_t userConfigToSet_;
  Atams::BusIDs_t  busIDsToSet_;

  /*-- Private Function Declarations ------------------------------------------------*/

  bool findNodeOnBus(Node &node);

  Atams::Error_t beginUpdateCyclePrivate(void);

  void clearAllBusErrors(void);

  bool pollForRequestTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t requestType);
  
  bool pollForJogTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process);

  Bus::PollResult pollForResponse(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t expectedResponse);

  virtual void rxCallback(uint8_t       *rxBufferPtr,
                          const uint16_t rxBufferLength) final;

  bool validateAndStoreResponsePacket(Atams::NodeCallbackHandler &node, const MessageType_t responseType);

  void startWriteConfigVars(void);

  bool allConfigVarsAcknowledged(void);

  void startReadConfigVars(void);

  Atams::Error_t validateConfigVars(void);

  Atams::Node * getUpdateNodePtr(void);

  bool tryNodeIncrementUpdate(void);

  void startResponseCollectionSync(void);

  void triggerJogSync(void);

  void triggerNextRequestAsync(void);

  void beginInitValidateGenInfo(Atams::Node &node);
  
  void beginInitValidateIDs(Atams::Node &node, const bool preAssignment, const Atams::BusIDs_t busIDs);
  
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


