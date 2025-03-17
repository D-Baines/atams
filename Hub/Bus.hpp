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
#include "../AtamsTypedefs.hpp"
#include "Utilities/CircularBuffer.hpp"
#include "Platform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

class Node;

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class   Bus :
private CircularBuffer,
private Platform::BusPeripheral
{
  /*-- Friend Declarations ----------------------------------------------------------*/

  friend class Node;

  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    INIT_STATE_START              = 0U,
    INIT_STATE_START_UPDATE_CYCLE = 1U,
    INIT_STATE_BUS_UPDATE         = 2U,
    INIT_STATE_VALIDITY_CHECKS    = 3U,
    INIT_STATE_ID_ASSIGNMENT      = 4U,
    INIT_STATE_SUCCESS            = 5U,
    INIT_STATE_FAILURE            = 6U
  } InitState_t;

  typedef enum: uint8_t
  {
    UPDATE_STATE_INIT_REQUIRED     = 0U,
    UPDATE_STATE_READY             = 1U,
    UPDATE_STATE_SEND_REQUESTS     = 2U,
    UPDATE_STATE_COLLECT_RESPONSES = 3U,
    UPDATE_STATE_JOG_NODE          = 4U,
    UPDATE_STATE_CYCLE_COMPLETE    = 5U,
  } UpdateState_t;

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  Bus(Platform::BusPeripheral::UserData_t userData);

  /* Copy Constructor */
  Bus(const Bus &other) = delete;

  /* Copy Assignment Operator */
  Bus & operator=(const Bus &other) = delete;

  /* Destructor */
  ~Bus(void);

  Bus::InitState_t updateInitProcedure(Atams::Error_t &statusReturn);

  Atams::Error_t startUpdateCycle(void);

  void update(void);

  bool updateCycleComplete(void);

  bool processBuffers(void);

  /*-- Private ----------------------------------------------------------------------*/

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/
  
  Node          *_nodePtrs[Platform::NUMBER_OF_NODES_PER_BUS];
  uint16_t       _activeNodeIndex      = 0U;
  uint16_t       _noOfNodesOnBus       = 0U;
  InitState_t    _busInitState         = Bus::INIT_STATE_START;
  InitState_t    _nextInitState        = Bus::INIT_STATE_START;
  UpdateState_t  _updateState          = UPDATE_STATE_READY;
  uint8_t        _rxBuffer[MAX_MESH_PACKET_SIZE];
  uint8_t        _decodedBuffer[MAX_MESH_PACKET_SIZE];
  uint8_t        _encodedBuffer[MAX_MESH_PACKET_SIZE];
  uint8_t        _jogBuffer[MESH_SIZE_HEADER];
  uint16_t       _rxLength             = 0U;
  uint16_t       _decodedLength        = 0U;
  uint16_t       _encodedLength        = 0U;
  uint8_t        _activeSyncCount      = 0U;
  uint64_t       _prevResponseTime     = 0U;
  uint64_t       _prevRequestTime      = 0U;

  /*-- Private Function Declarations ------------------------------------------------*/

  Atams::Error_t addNodeToBus(Node &node);

  void removeNodeFromBus(Node &node);

  virtual void rxCallback(      uint8_t  *rxBufferPtr,
                          const uint16_t  rxBufferLength) final;

  Atams::Error_t validateAndStoreResponsePacket(Node &node, const MessageType_t responseType);

  Bus::UpdateState_t updateNoSync(void);

  Atams::Error_t triggerGenInfoCollectionAllNodes(void);

  Atams::Error_t checkGenInfoAllNodes(void);

  Atams::Error_t assignNodeIDs(void);
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


