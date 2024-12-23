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
private Platform::CommsLock
{
  /*-- Friend Declarations ----------------------------------------------------------*/

  friend class Node;

  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef enum: uint8_t
  {
    INIT_STATE_INCOMPLETE  = 0U,
    INIT_STATE_IN_PROGRESS = 1U,
    INIT_STATE_SUCCESSFUL  = 2U,
    INIT_STATE_FAILURE     = 3U
  } InitState_t;

  struct NodeHandle_t
  {
    Node   *nodeContext;
    uint8_t nodeID;
    uint8_t inactiveBuffer[Platform::COMMS_BUFFER_SIZE];
    uint8_t activeBuffer[Platform::COMMS_BUFFER_SIZE];
  };
  

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  Bus(void);

  /* Copy Constructor */
  Bus(const Bus &other) = delete;

  /* Copy Assignment Operator */
  Bus & operator=(const Bus &other) = delete;

  /* Destructor */
  ~Bus(void) = delete;

  Bus::InitState_t updateInitProcedure(void);

  Atams::Error_t startUpdateCycle(void);

  Atams::Error_t update(void);

  bool updateCycleComplete(void);

  Atams::Error_t processBuffers(void);

  /*-- Private ----------------------------------------------------------------------*/

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/\
  
  NodeHandle_t _nodeHandles[Platform::NUMBER_OF_NODES_PER_BUS];

  /*-- Private Function Declarations ------------------------------------------------*/

  Atams::Error_t addNodeToBus(Node &node);

  Atams::Error_t removeNodeFromBus(Node &node);

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


