/**
  ******************************************************************************
  * @file    WatchdogHandler.cpp
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

#include "NVMUnitHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

NVMUnitHandler::NVMUnitHandler(Atams::CRC32 &nvmCRC) :
nvmCRC_(nvmCRC)
{
  /* Do Nothing */
}

Atams::Error_t NVMUnitHandler::eraseNVM(void)
{
  unitStartIndex_ = 0U;

  nvmCRC_.beginRollingCRC();

  return (Platform::eraseNVM() ? Atams::ERROR_NONE : Atams::ERROR_PLATFORM);
}

Atams::Error_t NVMUnitHandler::writeToNVM(const uint32_t writeIndex, const uint8_t * const inputPtr, const uint32_t length)
{
  if (writeIndex != (unitStartIndex_ + unitIndex_))
  {
    return (Atams::ERROR_NVM_WRITE_ORDER); /* Early Return */
  }

  for (uint32_t byteIndex = 0U; byteIndex < length; byteIndex++)
  {
    unitBuffer_[unitIndex_] = inputPtr[byteIndex];

    nvmCRC_.updateRollingCRC(inputPtr[byteIndex]);

    unitIndex_++;

    if (unitIndex_ >= Platform::NVM_UNIT_SIZE)
    {

      if (Platform::writeToNVM(unitStartIndex_, unitBuffer_) == false)
      {
        return (Atams::ERROR_PLATFORM); /* Early Return */
      }

      unitStartIndex_ += Platform::NVM_UNIT_SIZE;
      unitIndex_ = 0U;
    }
  }

  return (Atams::ERROR_NONE);
}

Atams::Error_t NVMUnitHandler::readFromNVM(const uint32_t readIndex, uint8_t * const outputPtr, const uint32_t length)
{
  return (Platform::readFromNVM(readIndex, outputPtr, length) ? Atams::ERROR_NONE : Atams::ERROR_PLATFORM);
}

Atams::Error_t NVMUnitHandler::flushPendingUnit(void)
{
  if (unitIndex_ == 0U)
  {
    return (Atams::ERROR_NONE); /* Early Return */
  }

  while (unitIndex_ < Platform::NVM_UNIT_SIZE)
  {
    unitBuffer_[unitIndex_] = 0U;
    unitIndex_++;
  }

  if (Platform::writeToNVM(unitStartIndex_, unitBuffer_) == false)
  {
    return (Atams::ERROR_PLATFORM); /* Early Return */
  }

  unitStartIndex_ += Platform::NVM_UNIT_SIZE;
  unitIndex_ = 0U;

  return (Atams::ERROR_NONE);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
