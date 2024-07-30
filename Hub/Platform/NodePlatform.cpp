/**
  ******************************************************************************
  * @file    NodePlatform.cpp
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

#include "NodePlatform.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

NodePlatform::NodePlatform(NodePlatform::UserData_t userData)
{
  _userData = userData;
}


NodePlatform::~NodePlatform(void)
{

}

void NodePlatform::acquireMemoryLock(void)
{

}

void NodePlatform::releaseMemoryLock(void)
{

}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/



} /* End Namespace - Atams */


/**
  * @}End of File
  */
