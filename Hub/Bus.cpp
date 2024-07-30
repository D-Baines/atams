/**
  ******************************************************************************
  * @file    Bus.cpp
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "Bus.hpp"
#include "Node.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Bus::Bus(BusPlatform::UserData_t userData) :
BusPlatform(userData)
{

}


Bus::~Bus(void)
{

}

Atams::Error_t Bus::addNodeToBus(Node &node)
{

}

Atams::Error_t Bus::removeNodeFromBus(Node &node)
{

}

Bus::InitState_t Bus::updateInitProcedure(void)
{

}

void Bus::update(void)
{

}

Atams::Error_t Bus::startCollectionCycle(void)
{

}

bool Bus::isNewDataReady(void)
{

}

Atams::Error_t Bus::processCollectedData(void)
{

}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/



} /* End Namespace - Atams */


/**
  * @}End of File
  */
