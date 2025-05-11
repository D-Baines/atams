/**
  ******************************************************************************
  * @file    NodeProcesses.hpp
  *
  * @author  D. Baines
  *
  * @brief
  *
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
  NodeActions(void);

  /* Default Destructor */
  ~NodeActions(void);

  /* Copy Constructor */
  NodeActions(const NodeActions &other) = delete;

  /* Copy Assignment Operator */
  NodeActions & operator=(const NodeActions &other) = delete;

  /* Move Constructor */
  NodeActions(NodeActions &&other) = delete;

  /* Move Assignment Operator */
  NodeActions & operator=(NodeActions &&other) = delete;

  void beginValidateGenInfoProcess(Node *node);

  void beginCollectNodeIDsProcess(Node *node);

  void beginConfigEntryProcess(Node *node);

  void beginConfigExitProcess(Node *node, const bool applyChangesOnExit);

  void beginSetNodeID(Node *node, const uint8_t nodeID);

  void beginSetBitrate(Node *node, const Atams::BitrateOption_t bitrateOption);

  void beginSetWatchdogPeriod(Node *node, const uint32_t watchdogPeriod);

  void beginStorageProcess(Node *node);

  void beginResetNodeProcess(Node *node);
  
  Atams::ProcessState updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid);

  Atams::ProcessState updateCollectNodeIDs(Atams::Error_t &error);

  Atams::ProcessState updateConfigurationStateEntry(Atams::Error_t &error);

  Atams::ProcessState updateConfigurationStateExit(Atams::Error_t &error);

  Atams::ProcessState updateSetNodeID(Atams::Error_t &error);

  Atams::ProcessState updateSetBitrate(Atams::Error_t &error);

  Atams::ProcessState updateSetWatchdogPeriod(Atams::Error_t &error);

  Atams::ProcessState updateStoreAll(Atams::Error_t &error);

  Atams::ProcessState updateRestoreAll(Atams::Error_t &error);

  Atams::ProcessState updateRestoreUserBlocks(Atams::Error_t &error);

  Atams::ProcessState updateResetNode(Atams::Error_t &error);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  static inline constexpr uint32_t STORAGE_TIMEOUT = 5000U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class ValidateGenInfoState : uint8_t
  {
    START    = 0U,
    COLLECT  = 1U,
    VALIDATE = 2U,
    COMPLETE = 3U,
    ERROR    = 4U
  };

  enum class CollectNodeIDsState : uint8_t
  {
    START     = 0U,
    COLLECT   = 1U,
    CHECK_NEW = 2U,
    COMPLETE  = 3U,
    ERROR     = 4U
  };

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

  /*-- Private Helper Struct Declarations -------------------------------------------*/

  template <typename T>
  struct ProcessHandler
  {
    ProcessHandler(void)  = default;
    ~ProcessHandler(void) = default;
    ProcessHandler(const ProcessHandler &other)             = delete;
    ProcessHandler & operator=(const ProcessHandler &other) = delete;
    ProcessHandler(ProcessHandler &&other)                  = delete;
    ProcessHandler & operator=(ProcessHandler &&other)      = delete;

    Node               *nodePtr         = nullptr;
    Atams::ProcessState processState    = Atams::ProcessState::COMPLETE;
    Atams::ProcessState subProcessState = Atams::ProcessState::COMPLETE;
    T                   specificState   = T::START;
    Atams::Error_t      error           = Atams::ERROR_NONE;
    Atams::Error_t      cancelError     = Atams::ERROR_NONE;
    uint32_t            prevEventTime   = 0U;

    Atams::Error_t assignNode(Node *node);
    void           terminate(Atams::Error_t error);
    void           setProcessComplete(void);
    bool           getProcessTerminated(void);
    void           resetProcess(void);
  };
  
  /*-- Private Objects --------------------------------------------------------------*/

  ProcessHandler<ValidateGenInfoState> validateGenInfoProcess_;
  ProcessHandler<CollectNodeIDsState>  collectNodeIDsProcess_;
  ProcessHandler<ConfigEntryState>     configEntryProcess_;
  ProcessHandler<ConfigExitState>      configExitProcess_;
  ProcessHandler<SetConfigVarState>    setConfigVarProcess_;
  ProcessHandler<StorageProcessState>  storageProcess_;
  ProcessHandler<ResetNodeState>       resetNodeProcess_;

  /*-- Private Variables ------------------------------------------------------------*/

  bool                   applyChanges_        {false};
  uint8_t                nodeIDToSet_         {0U};
  Atams::BitrateOption_t bitrateOptionToSet_  {Atams::BitrateOption_t::BITRATE_OPTION_0};
  uint32_t               watchdogPeriodToSet_ {0U};

  /*-- Private Function Declarations ------------------------------------------------*/

  template<typename T>
  void cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error);

  template<typename T>
  void updateCancelConfigState(ProcessHandler<T> &process);

  template<typename T>
  Atams::ProcessState updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value);

  Atams::ProcessState updateStorageProcess(Atams::Error_t &error, const uint16_t varID, uint32_t passcode);
  
  void startReadGenInfo(Node &node);

  Atams::Error_t validateCollectedGenInfo(Node &node, bool &genInfoIsValid);

  void startReadAllNodeIDs(Node &node);

  Atams::Error_t validateReadAllNodeIDs(Node &node);
};

} /* End Namespace - Atams */


  /**
    * @}End of File
  */


