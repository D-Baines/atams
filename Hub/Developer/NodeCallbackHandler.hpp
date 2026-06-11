/**
  ******************************************************************************
  * @file    NodeCallbackHandler.hpp
  *
  * @author  D. Baines
  *
  * @brief   Abstract interface for Hub-side Node event callbacks.
  *
  * @details Declares the NodeCallbackHandler pure abstract base class that
  *          Atams::Node inherits from. The Atams::Bus class uses these virtual
  *          functions to deliver received data, report bus errors, retrieve encoded
  *          request packets, and validate Node general information during each update
  *          cycle. Friend access is granted to Atams::Bus and Atams::NodeActions.
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

class Bus;
class NodeActions;

/*************************************************************************************/
/* PROTOTYPES/CLASS DEFINITIONS                                                      */
/*************************************************************************************/

class NodeCallbackHandler
{
  /*-- Friend Class Declarations ----------------------------------------------------*/

  friend class Atams::Bus;

  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  NodeCallbackHandler(void) = default;

  /* Default Destructor */
  virtual ~NodeCallbackHandler(void) = default;

  /* Copy Constructor */
  NodeCallbackHandler(const NodeCallbackHandler &other) = delete;

  /* Copy Assignment Operator */
  NodeCallbackHandler & operator=(const NodeCallbackHandler &other) = delete;

  /* Move Constructor */
  NodeCallbackHandler(NodeCallbackHandler &&NodeCallbackHandler) = delete;

  /* Move Assignment Operator */
  NodeCallbackHandler & operator=(NodeCallbackHandler &&other) = delete;

  private:

  /*-- Private Pure Virtual Function Declarations -----------------------------------*/

  virtual bool validateGenInfo(void) = 0;

  virtual void setNodeMaxPacketSize(const uint16_t maxPacketSize) = 0;

  virtual void reportBusError(const Atams::Error_t error) = 0;

  virtual void clearBusError(void) = 0;

  virtual void clearAbortDetails(void) = 0;

  virtual void responseReceived(uint8_t *inputBuffer, uint16_t inputLength) = 0;

  virtual void processResponseBuffer(void) = 0;

  virtual void getRequestPacket(const Atams::MessageType_t requestType,
                                const uint8_t              syncCount,
                                uint8_t * const            outputBuffer,
                                uint16_t                  &outputLength) = 0;

  virtual Atams::Error_t updateRequestPacketWriteData(void) = 0;
};

} /* End Namespace - Atams */


  /**
    * @}End of File
  */


