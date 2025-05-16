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
#include "../Shared/Maps/BlockUniversal.hpp"
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

  Atams::Error_t startPeripheral(void);

  Atams::ProcessState updateBusInitProcess(Atams::Error_t &error);

  Atams::Error_t startUpdateCycle(void);

  Atams::ProcessState runUpdateCycleSync(Atams::Error_t &error);

  Atams::ProcessState runUpdateCycleAsync(Atams::Error_t &error);

  Atams::Error_t processBuffers(void);

  void beginSetNodeConfigProcess(const uint8_t         nodeIDToSet, 
                                 const BitrateOption_t bitrateOption,
                                 const uint32_t        watchdogPeriod);

  Atams::ProcessState updateSetNodeConfigProcess(Atams::Error_t &error); 

  /*-- Private ----------------------------------------------------------------------*/

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class InitState: uint8_t
  {
    START                 = 0U,
    VALIDATE_GEN_INFO     = 2U,
    VALIDATE_IDS          = 4U,
    ENTER_CONFIG          = 5U,
    ASSIGN_NODE_IDS       = 6U,
    SAVE_AND_EXIT         = 7U,
    VALIDATE_IDS_POST     = 8U,
    START_UPDATE_CYCLE    = 9U,
    UPDATE_CYCLE          = 10U,
    COMPLETE              = 11U,
    ERROR                 = 12U,
  };

  enum class ConfigUpdateState: uint8_t
  {
    START               = 0U,
    INIT_NODE           = 1U,
    BEGIN_CONFIG_ENTRY  = 2U,
    UPDATE_CONFIG_ENTRY = 3U,
    WRITE_CONFIG        = 4U,
    CHECK_ACK           = 5U,
    BEGIN_CONFIG_EXIT   = 6U,
    UPDATE_CONFIG_EXIT  = 7U,
    READ_CONFIG         = 8U,
    CHECK_CONFIG        = 9U,
    BEGIN_SAVE_ALL      = 10U,
    UPDATE_SAVE_ALL     = 11U,
    SEND_REQUEST        = 12U,
    GET_RESPONSE        = 13U,
    COMPLETE            = 14U,
    ERROR               = 15U
  };

  enum class UpdateState: uint8_t
  {
    START             = 0U,
    SEND_REQUESTS     = 1U,
    COLLECT_RESPONSES = 2U,
    JOG_NODE          = 3U,
    COMPLETE          = 4U,
    ERROR             = 5U,
  };

  enum class ValidateState: uint8_t
  {
    START        = 0U,
    UPDATE       = 1U,
    VALIDATE     = 2U,
    COMPLETE     = 3U,
    ERROR        = 4U
  }; 

  struct UniversalConfig_t
  {
    uint8_t  nodeID;
    uint8_t  bitrateOption;
    uint32_t watchdogPeriod;

    bool operator!=(const UniversalConfig_t &other)
    {
      return ((nodeID         != other.nodeID        ) ||
              (bitrateOption  != other.bitrateOption ) ||
              (watchdogPeriod != other.watchdogPeriod) );
    }
  };

  /*-- Private Helper Struct Declarations -----------------------------------------*/

  struct ProcessHandlerBase
  {
    ProcessHandlerBase(void) = default;

    Atams::ProcessState processState     = Atams::ProcessState::COMPLETE;
    Atams::ProcessState subProcessState  = Atams::ProcessState::COMPLETE;
    Atams::Error_t      error            = Atams::ERROR_NONE;
    uint32_t            prevEventTime    = 0U;
    bool                allNodesComplete = false;
  };
  
  template <typename T>
  struct ProcessHandler :
  public ProcessHandlerBase
  {
    ProcessHandler(void) = default;
    T specificState      = T::START;
    T nextSpecificState  = T::START;

    void terminate(Atams::Error_t error);
    void setProcessComplete(void);
    bool getProcessTerminated(void);
    void resetProcess(void);
  };

  /*-- Static Private Variables -----------------------------------------------------*/

  static const GenInfo_t         blankGenInfo_;
  static const Node::MemoryMap_t dummyMemoryMap_;

  /*-- Private Objects --------------------------------------------------------------*/
                                      
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

  uint16_t activeNodeIndex_     = 0U;
  uint16_t noOfNodesOnBus_      = 0U;
  uint8_t  rxBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  decodedBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  encodedBuffer_[Platform::MAX_BUS_PACKET_SIZE];
  uint8_t  jogBuffer_[MESH_SIZE_HEADER];
  uint16_t rxLength_            = 0U;
  uint16_t decodedLength_       = 0U;
  uint16_t encodedLength_       = 0U;
  uint8_t  activeSyncCount_     = 0U;

  UniversalConfig_t setupConfig_;

  /*-- Private Function Declarations ------------------------------------------------*/

  bool pollForRequestTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t requestType);
  
  bool pollForJogTransmit(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process);

  bool pollForResponse(Atams::NodeCallbackHandler &node, Bus::ProcessHandlerBase &process, const Atams::MessageType_t expectedResponse);

  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength) final;

  Atams::Error_t validateAndStoreResponsePacket(Atams::NodeCallbackHandler &node, const MessageType_t responseType);

  Atams::Error_t assignNodeIDs(void);

  void beginValidateGenInfoAllNodes(void);

  void beginValidateIDsAllNodes(void);

  void beginConfigurationEntryAllNodes(void);

  void beginStoreAllNodes(void);

  Atams::ProcessState updateValidateGenInfoAllNodes(Atams::Error_t &error);

  Atams::ProcessState updateValidateIDsAllNodes(Atams::Error_t &error);

  void updateConfigurationEntryAllNode(Atams::Error_t &error);

  void updateStoreAllNodes(Atams::Error_t &error);

  void startWriteConfigVars(void);

  bool allConfigVarsAcknowledged(void);

  void startReadConfigVars(void);

  Atams::Error_t validateConfigVars(void);

  /* Update State */

  Atams::Node * getActiveNodePtr(void);

  bool tryNodeIncrement(void);

  void startResponseCollectionSync(void);

  void triggerJogSync(void);

  void triggerNextRequestAsync(void);

  void triggerNextSetNodeConfigCycle(const Bus::ConfigUpdateState nextState);

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


