/**
  ******************************************************************************
  * @file    DataBlockUniversal.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing function definitions for an 
  *          Atams Data Block with name: Universal.
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

#include "DataBlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_NODE_ID,
                                                         BlockUniversal::DEFAULT_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_FIRST_NODE_ID,
                                                         BlockUniversal::DEFAULT_FIRST_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_LAST_NODE_ID,
                                                         BlockUniversal::DEFAULT_LAST_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID,
                                                         BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);

  if (initStatus == ERROR_NONE) initStatus = block.write(BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT,
                                                         BlockUniversal::DEFAULT_WATCHDOG_TIMEOUT);

  return (initStatus); 
}

Error_t initLimits(DataBlock &block)
{
  Error_t initStatus = ERROR_NONE;

  return (initStatus); 
}

} } /* End Namespace - Atams::BlockUniversal */

/**
  * @}End of File
  */
