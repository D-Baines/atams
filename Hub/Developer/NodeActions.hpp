/**
  ******************************************************************************
  * @file    NodeActions.hpp
  *
  * @author  D. Baines
  *
  * @brief   Multi-step process handlers for Hub-side Node configuration and management.
  *
  * @details Defines the NodeActions class, which encapsulates state machines for
  *          performing multi-step operations on Atams Node devices from a Hub.
  *          Supported processes include entering and exiting the configuration state,
  *          setting the Node ID, comms bitrate, watchdog period, and bus IDs,
  *          as well as triggering NVM storage and node reset operations. Each process
  *          is driven incrementally by calling the corresponding update function once
  *          per Hub update cycle until completion or an error is reported.
  *
  * @version v1.0
  ******************************************************************************
  * @attention
  *
  * Copyright (c) D. Baines
  * All rights reserved.
  *
  * This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at https://mozilla.org/MPL/2.0/.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include <optional>

#include "../../Shared/AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

class Node;

/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class NodeActions
{
  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  NodeActions(void) = default;

  /* Default Destructor */
  ~NodeActions(void) = default;

  /* Copy Constructor */
  NodeActions(const NodeActions &other) = delete;

  /* Copy Assignment Operator */
  NodeActions & operator=(const NodeActions &other) = delete;

  /* Move Constructor */
  NodeActions(NodeActions &&other) = delete;

  /* Move Assignment Operator */
  NodeActions & operator=(NodeActions &&other) = delete;

  void beginConfigEntryProcess(Node &node);

  void beginConfigExitProcess(Node &node, bool applyChangesOnExit, std::optional<uint8_t> newNodeID);

  void beginSetNodeID(Node &node, const uint8_t nodeID);

  void beginSetBitrate(Node &node, const Atams::BitrateOption_t bitrateOption);

  void beginSetWatchdogPeriod(Node &node, const uint32_t watchdogPeriod);

  void beginSetNodeConfig(Node &node, const Atams::NodeConfig_t &userConfig);

  void beginStorageProcess(Node &node);

  void beginResetNodeProcess(Node &node);

  void beginValidateGenInfoProcess(Node &node);

  void beginValidateBusIDsProcess(Node &node, const Atams::BusIDs_t busIDs);

  void beginSetBusIDs(Node &node, const Atams::BusIDs_t busIDs);

  Atams::ProcessState_t updateConfigurationStateEntry(Atams::Error_t &error);

  Atams::ProcessState_t updateConfigurationStateExit(Atams::Error_t &error);

  Atams::ProcessState_t updateSetNodeID(Atams::Error_t &error);

  Atams::ProcessState_t updateSetBitrate(Atams::Error_t &error);

  Atams::ProcessState_t updateSetWatchdogPeriod(Atams::Error_t &error);

  Atams::ProcessState_t updateSetNodeConfig(Atams::Error_t &error);

  Atams::ProcessState_t updateStoreAll(Atams::Error_t &error);

  Atams::ProcessState_t updateRestoreAll(Atams::Error_t &error);

  Atams::ProcessState_t updateRestoreUserBlocks(Atams::Error_t &error);

  Atams::ProcessState_t updateResetNode(Atams::Error_t &error);

  Atams::ProcessState_t updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid);

  Atams::ProcessState_t updateValidateBusIDs(Atams::Error_t &error, bool &allIDsValid);

  Atams::ProcessState_t updateSetBusIDs(Atams::Error_t &error);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static constexpr uint32_t STORAGE_TIMEOUT {5000U};

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class ConfigEntryState : uint8_t
  {
    START               = 0U,
    COLLECT_STATUS_PRE  = 1U,
    CHECK_STATUS_PRE    = 2U,
    WRITE_PASSCODE      = 3U,
    COLLECT_STATUS_POST = 4U,
    CHECK_STATUS_POST   = 5U,
    COMPLETE            = 6U,
    ERROR               = 7U
  };

  enum class ConfigExitState : uint8_t
  {
    START               = 0U,
    COLLECT_STATUS_PRE  = 1U,
    CHECK_STATUS_PRE    = 2U,
    WRITE_PASSCODE      = 3U,
    COLLECT_STATUS_POST = 4U,
    CHECK_STATUS_POST   = 5U,
    COMPLETE            = 6U,
    ERROR               = 7U
  };

  enum class SetConfigVarState : uint8_t
  {
    START              = 0U,
    ENTER_CONFIG       = 1U,
    WRITE              = 2U,
    CHECK_ACK          = 3U,
    CANCEL_CONFIG      = 4U,
    BEGIN_APPLY_CONFIG = 5U,
    APPLY_CONFIG       = 6U,
    READ               = 7U,
    CHECK_VALUE        = 8U,
    COMPLETE           = 9U,
    ERROR              = 10U
  };

  enum class StorageProcessState : uint8_t
  {
    START              = 0U,
    ENTER_CONFIG       = 1U,
    PROGRESS_CLEAR     = 2U,
    PROGRESS_READ_PRE  = 3U,
    PROGRESS_CHECK_PRE = 4U,
    CLEAR_PASSCODE     = 5U,
    WRITE_PASSCODE     = 6U,
    STATUS_STREAM      = 7U,
    STATUS_CHECK_POST  = 8U,
    CANCEL_CONFIG      = 9U,
    BEGIN_EXIT_CONFIG  = 10U,
    EXIT_CONFIG        = 11U,
    COMPLETE           = 12U,
    ERROR              = 13U
  };

  enum class ResetNodeState : uint8_t
  {
    START          = 0U,
    ENTER_CONFIG   = 1U,
    WRITE_PASSCODE = 2U,
    CHECK_ACK      = 3U,
    CANCEL_CONFIG  = 4U,
    COMPLETE       = 5U,
    ERROR          = 6U
  };

  enum class ValidateMultiConfigState : uint8_t
  {
    START    = 0U,
    COLLECT  = 1U,
    VALIDATE = 2U,
    COMPLETE = 3U,
    ERROR    = 4U
  };

  enum class SetMultiConfigState : uint8_t
  {
    START              = 0U,
    ENTER_CONFIG       = 1U,
    WRITE              = 2U,
    CHECK_ACK          = 3U,
    CANCEL_CONFIG      = 4U,
    BEGIN_APPLY_CONFIG = 5U,
    APPLY_CONFIG       = 6U,
    READ               = 7U,
    CHECK_VALUE        = 8U,
    COMPLETE           = 9U,
    ERROR              = 10U
  };

  typedef void           (&MultiConfigWriteFn_t)(NodeActions &actionsHandler, Atams::Node &node);
  typedef bool           (&MultiConfigAckFn_t)  (Atams::Node &node);
  typedef void           (&MultiConfigReadFn_t) (Atams::Node &node);
  typedef Atams::Error_t (&MultiConfigCheckFn_t)(NodeActions &actionsHandler, Atams::Node &node, bool &dataMatch);

  struct SetMultiConfigFtns_t
  {
    MultiConfigWriteFn_t writeFunction;
    MultiConfigAckFn_t   ackFunction;
    MultiConfigReadFn_t  readFunction;
    MultiConfigCheckFn_t checkFunction;
  };

  struct ValidateMultiConfigFtns_t
  {
    MultiConfigReadFn_t  readFunction;
    MultiConfigCheckFn_t checkFunction;
  };

  /*-- Private Helper Struct Declarations -------------------------------------------*/

  template <typename T>
  struct ProcessHandler
  {    
    ProcessHandler(void)                                    = default;
    ~ProcessHandler(void)                                   = default;
    ProcessHandler(const ProcessHandler &other)             = delete;
    ProcessHandler & operator=(const ProcessHandler &other) = delete;
    ProcessHandler(ProcessHandler &&other)                  = delete;
    ProcessHandler & operator=(ProcessHandler &&other)      = delete;;

    Node                 *nodePtr         {nullptr};
    Atams::ProcessState_t processState    {Atams::PROCESS_COMPLETE};
    Atams::ProcessState_t subProcessState {Atams::PROCESS_COMPLETE};
    T                     specificState   {T::START}; /* All specific state enums must include START, ERROR, COMPLETE */
    Atams::Error_t        error           {Atams::ERROR_NONE};
    Atams::Error_t        cancelError     {Atams::ERROR_NONE};
    uint32_t              prevEventTime   {0U};
    uint32_t              startTime       {0U};

    void resetAndAssignNode(Node &node);
    void terminate(Atams::Error_t error);
    void setProcessComplete(void);
    bool getProcessTerminated(void);
    void updateTimeout(void);
    bool nullptrCheck(Atams::Error_t &errorReturn);
  };
  
  /*-- Private Objects --------------------------------------------------------------*/

  ProcessHandler<ConfigEntryState>         configEntryProcess_;
  ProcessHandler<ConfigExitState>          configExitProcess_;
  ProcessHandler<SetConfigVarState>        setConfigVarProcess_;
  ProcessHandler<StorageProcessState>      storageProcess_;
  ProcessHandler<ResetNodeState>           resetNodeProcess_;
  ProcessHandler<ValidateMultiConfigState> validateMultiConfigProcess_;
  ProcessHandler<SetMultiConfigState>      setMultiConfigProcess_;

  /*-- Private Variables ------------------------------------------------------------*/

  bool                   applyChanges_        {false};
  uint8_t                nodeIDToSet_         {0U};
  std::optional<uint8_t> configExitNodeID_    {std::nullopt};
  Atams::BitrateOption_t bitrateOptionToSet_  {Atams::BitrateOption_t::BITRATE_OPTION_0};
  uint32_t               watchdogPeriodToSet_ {0U};
  Atams::NodeConfig_t    nodeConfigToSet_;
  BusIDs_t               busIDsToSet_;

  ValidateMultiConfigFtns_t validateGenInfoFunctions_ {validateGenInfoRead, validateGenInfoCheck};
  ValidateMultiConfigFtns_t validateBusIDsFunctions_  {validateBusIDsRead,  validateBusIDsCheck};

  SetMultiConfigFtns_t setUserConfigFunctions_ {setUserConfigWrite, setUserConfigGetAck, setUserConfigRead, setUserConfigCheck};
  SetMultiConfigFtns_t setBusIDsFunctions_     {setBusIDsWrite,     setBusIDsGetAck,     setBusIDsRead,     setBusIDsCheck};

  /*-- Private Function Declarations ------------------------------------------------*/

  template<typename T>
  void cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error);

  template<typename T>
  void updateCancelConfigState(ProcessHandler<T> &process);

  template<typename T>
  Atams::ProcessState_t updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value);

  Atams::ProcessState_t updateStorageProcess(Atams::Error_t &error, const uint16_t varID, uint32_t passcode);

  Atams::ProcessState_t updateValidateMultipleConfig(Atams::Error_t            &error, 
                                                     bool                      &allVarsValid, 
                                                     ValidateMultiConfigFtns_t &specificFunctions);

  Atams::ProcessState_t updateSetMultipleConfig(Atams::Error_t &error, SetMultiConfigFtns_t &specificFunctions);
  
  static void validateGenInfoRead(Atams::Node &node);

  static void validateBusIDsRead(Atams::Node &node);

  static Atams::Error_t validateGenInfoCheck(NodeActions &processHandler, Atams::Node &node, bool &genInfoIsValid);

  static Atams::Error_t validateBusIDsCheck(NodeActions &processHandler, Atams::Node &node, bool &allIDsValid);

  static void setUserConfigWrite(NodeActions &processHandler, Atams::Node &node);

  static bool setUserConfigGetAck(Atams::Node &node);

  static void setUserConfigRead(Atams::Node &node);

  static Atams::Error_t setUserConfigCheck(NodeActions &processHandler, Atams::Node &node, bool &userConfigIsValid);

  static void setBusIDsWrite(NodeActions &processHandler, Atams::Node &node);

  static bool setBusIDsGetAck(Atams::Node &node);

  static void setBusIDsRead(Atams::Node &node);

  static Atams::Error_t setBusIDsCheck(NodeActions &processHandler, Atams::Node &node, bool &allIDsValid);
};

} /* End Namespace - Atams */


  /**
    * @}End of File
  */


