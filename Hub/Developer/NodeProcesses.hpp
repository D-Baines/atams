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

class NodeProcesses
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  NodeProcesses(Node &node);

  /* Default Destructor */
  ~NodeProcesses(void);

  /* Copy Constructor */
  NodeProcesses(const NodeProcesses &other) = delete;

  /* Copy Assignment Operator */
  NodeProcesses & operator=(const NodeProcesses &other) = delete;

  /* Move Constructor */
  NodeProcesses(NodeProcesses &&other) = delete;

  /* Move Assignment Operator */
  NodeProcesses & operator=(NodeProcesses &&other) = delete;

  Atams::ProcessState_t updateValidateGenInfo(Atams::Error_t &error, bool &genInfoIsValid);

  Atams::ProcessState_t updateCollectNodeIdentifiers(Atams::Error_t &error);

  Atams::ProcessState_t updateConfigurationStateEntry(Atams::Error_t &error);

  Atams::ProcessState_t updateConfigurationStateExit(Atams::Error_t &error);

  Atams::ProcessState_t updateSetNodeID(Atams::Error_t &error);

  Atams::ProcessState_t updateSetBitrate(Atams::Error_t &error);

  Atams::ProcessState_t updateSetWatchdogPeriod(Atams::Error_t &error);

  Atams::ProcessState_t updateStoreAll(Atams::Error_t &error);

  Atams::ProcessState_t updateResetNode(Atams::Error_t &error);

  private:

  /*-- Private Static Constants -----------------------------------------------------*/

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  enum class ValidateGenInfoState : uint8_t
  {
    START    = 0U,
    COLLECT  = 1U,
    VALIDATE = 2U,
    COMPLETE = 3U,
  };

  enum class CollectNodeIDsState : uint8_t
  {
    START     = 0U,
    COLLECT   = 1U,
    CHECK_NEW = 2U,
    COMPLETE  = 3U,
  };

  enum class ConfigurationEntryState : uint8_t
  {
    START               = 0U,
    COLLECT_STATUS_PRE  = 1U,
    CHECK_STATUS_PRE    = 2U,
    WRITE_PASSCODE      = 3U,
    COLLECT_STATUS_POST = 4U,
    CHECK_STATUS_POST   = 5U,
    COMPLETE            = 6U,
  };

  /*-- Private Objects --------------------------------------------------------------*/

  Node &m_node;

  /*-- Private Variables ------------------------------------------------------------*/

  ValidateGenInfoState    m_validateGenInfoState    = ValidateGenInfoState::START;
  CollectNodeIDsState     m_collectNodeIDsState     = CollectNodeIDsState::START;
  ConfigurationEntryState m_configurationEntryState = ConfigurationEntryState::START;

  /*-- Private Function Declarations ------------------------------------------------*/

};

} /* End Namespace - Atams */


/**
  * @}End of File
  */


