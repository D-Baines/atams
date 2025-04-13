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

Atams::Error_t BlockOwnerInteractor::loadFromNVM(const uint32_t maxIndex, uint32_t &nvmIndex)
{
  return (NVMTransfer(maxIndex, nvmIndex, TRANSFER_LOAD));
}

Atams::Error_t BlockOwnerInteractor::saveToNVM(const uint32_t maxIndex, uint32_t &nvmIndex)
{
  return (NVMTransfer(maxIndex, nvmIndex, TRANSFER_SAVE));
}

uint32_t BlockOwnerInteractor::getNVMSpaceRequirement(void)
{
  return (DataBlock::getNVMSpaceRequirement());
}

Atams::Error_t BlockOwnerInteractor::initDescriptor(const DataBlock::Descriptor_t * const blockDescriptor)
{
  return (DataBlock::initDescriptor(blockDescriptor));
}

void BlockOwnerInteractor::deinitDescriptor(void)
{
  DataBlock::deinitDescriptor();
}

void BlockOwnerInteractor::resetStorageBlockIndex(void)
{
  DataBlock::resetStorageBlockIndex();
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
