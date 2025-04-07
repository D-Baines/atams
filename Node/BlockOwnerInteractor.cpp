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
  uint32_t requiredSpace = 0U;

  if (_blockDescriptorPtr != nullptr)
  {
    for (const VarInfo_t &varInfo : _blockDescriptorPtr->varInfo)
    {
      if (varInfo.NVMStorage) requiredSpace += Atams::TYPE_LENGTHS[varInfo.type];
    }
  }

  return (requiredSpace);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::Error_t BlockOwnerInteractor::NVMTransfer(const uint32_t maxIndex, uint32_t &nvmIndex, const NVMTransfer_t transferType)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  uint16_t       varID        = 0U;

  if (_blockDescriptorPtr == nullptr)
  {
    return (statusReturn); /* Early Return */
  }

  for (const VarInfo_t &varInfo : _blockDescriptorPtr->varInfo)
  {
    if (varInfo.NVMStorage)
    {
      uint8_t varLength = Atams::TYPE_LENGTHS[varInfo.type];

      if ((nvmIndex + varLength) > maxIndex)
      {
        return (Atams::ERROR_NVM_HEADER_LENGTH); /* Early Return */
      }

      switch (transferType)
      {
        case TRANSFER_LOAD:
          if (!Platform::readFromNVM(nvmIndex, varLength, _vars[varID].data))
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        case TRANSFER_SAVE:
          if (!Platform::writeToNVM(nvmIndex, varLength, _vars[varID].data))
          {
            return (Atams::ERROR_PLATFORM);      /* Early Return */
          }
          break;
        default:
          return (Atams::ERROR_FATAL);           /* Early Return */
      }

      nvmIndex += varLength;
    }

    varID++;
  }

  return (statusReturn);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
