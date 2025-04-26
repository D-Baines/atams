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
#include "../../AtamsTypedefs.hpp"

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
  NodeActions(Node &node);

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

  Atams::ProcessState_t updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid);

  Atams::ProcessState_t updateCollectNodeIdentifiers(Atams::Error_t &error);

  Atams::ProcessState_t updateConfigurationStateEntry(Atams::Error_t &error);

  Atams::ProcessState_t updateConfigurationStateExit(Atams::Error_t &error, bool applyChanges);

  Atams::ProcessState_t updateSetNodeID(Atams::Error_t &error, uint8_t nodeID);

  Atams::ProcessState_t updateSetBitrate(Atams::Error_t &error, Atams::BitrateOption_t bitrateOption);

  Atams::ProcessState_t updateSetWatchdogPeriod(Atams::Error_t &error, uint32_t watchdogPeriod);

  Atams::ProcessState_t updateStoreAll(Atams::Error_t &error);

  Atams::ProcessState_t updateRestoreAll(Atams::Error_t &error);

  Atams::ProcessState_t updateRestoreUserBlocks(Atams::Error_t &error);

  Atams::ProcessState_t updateResetNode(Atams::Error_t &error);

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
    START         = 0U,
    ENTER_CONFIG  = 1U,
    WRITE         = 2U,
    READ          = 3U,
    CHECK         = 4U,
    CANCEL_CONFIG = 5U,
    APPLY_CONFIG  = 6U,
    COMPLETE      = 7U,
    ERROR         = 8U
  };

  enum class StorageProcessState : uint8_t
  {
    START              = 0U,
    ENTER_CONFIG       = 1U,
    PROGRESS_CLEAR     = 2U,
    PROGRESS_READ_PRE  = 3U,
    PROGRESS_CHECK_PRE = 4U,
    WRITE_PASSCODE     = 5U,
    STATUS_STREAM      = 6U,
    STATUS_CHECK_POST  = 7U,
    CANCEL_CONFIG      = 10U,
    APPLY_CONFIG       = 11U,
    COMPLETE           = 12U,
    ERROR              = 13U
  };

  enum class ResetNodeState : uint8_t
  {
    START          = 0U,
    ENTER_CONFIG   = 1U,
    WRITE_PASSCODE = 2U,
    CHECK_ACK      = 3U,
    COMPLETE       = 4U,
    ERROR          = 5U
  };

  /*-- Private Helper Struct Declarations -------------------------------------------*/

  template <typename T>
  struct ProcessHandler
  {
    ProcessHandler(Node &node) :
    node(node){}

    Node                 &node;
    Atams::ProcessState_t processState  = Atams::PROCESS_STATE_COMPLETE;
    T                     minorState    = T::START;
    Atams::Error_t        error         = Atams::ERROR_NONE;
    Atams::Error_t        minorError    = Atams::ERROR_NONE;
    uint32_t              prevEventTime = 0U;

    void terminate(Atams::Error_t error);

    void setProcessComplete(void);

    bool getProcessTerminated(void);

    void resetProcess(void);
  };
  
  /*-- Private Objects --------------------------------------------------------------*/

  Node &node_;
  ProcessHandler<ValidateGenInfoState> validateGenInfoProcess_;
  ProcessHandler<CollectNodeIDsState>  collectNodeIDsProcess_;
  ProcessHandler<ConfigEntryState>     configEntryProcess_;
  ProcessHandler<ConfigExitState>      configExitProcess_;
  ProcessHandler<SetConfigVarState>    setConfigVarProcess_;
  ProcessHandler<StorageProcessState>  storageProcess_;

  /*-- Private Variables ------------------------------------------------------------*/

  /*-- Private Function Declarations ------------------------------------------------*/

  template<typename T>
  void cancelConfigProcess(ProcessHandler<T> &process, Atams::Error_t error);

  template<typename T>
  Atams::ProcessState_t updateSetConfigVar(Atams::Error_t &error, const uint16_t varID, const T value);

  Atams::ProcessState_t updateStorageProcess(Atams::Error_t &error, const uint16_t varID, uint32_t passcode);
};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


