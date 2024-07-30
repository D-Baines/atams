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
#include "Platform/BusPlatform.hpp"


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
private BusPlatform
{
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

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  Bus(BusPlatform::UserData_t userData);

  /* Copy Constructor */
  Bus(const Bus &other) = delete;

  /* Copy Assignment Operator */
  Bus & operator=(const Bus &other) = delete;

  /* Destructor */
  ~Bus(void);

  Atams::Error_t addNodeToBus(Node &node);

  Atams::Error_t removeNodeFromBus(Node &node);

  Bus::InitState_t updateInitProcedure(void);

  void update(void);

  Atams::Error_t startCollectionCycle(void);

  bool isNewDataReady(void);

  Atams::Error_t processCollectedData(void);


  /*-- Private ----------------------------------------------------------------------*/

  private:


  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  /*-- Private Function Declarations ------------------------------------------------*/

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


