/**
  ******************************************************************************
  * @file    BlockOwnerInteractor.cpp
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <cstring>
#include "BlockOwnerInteractor.hpp"
#include "../Utilities/AtamsUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE STATIC CONSTANTS                                                          */
/*************************************************************************************/



/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

BlockOwnerInteractor::BlockOwnerInteractor(void)
{
  /* Do Nothing */
}

BlockOwnerInteractor::~BlockOwnerInteractor(void)
{
  /* Do Nothing */
}

Atams::Error_t BlockOwnerInteractor::initDescriptor(const DataBlock::Descriptor_t * const blockDescriptor)
{
  return (DataBlock::initDescriptor(blockDescriptor));
}

void BlockOwnerInteractor::deinitDescriptor(void)
{
  DataBlock::deinitDescriptor();
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
