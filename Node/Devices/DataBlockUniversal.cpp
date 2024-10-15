/**
  ******************************************************************************
  * @file    DataBlockUniversal.cpp
  *
  * @author  D. Baines
  *
  * @brief   Auto-generated file containing public function definitions
  *          for an Atams Memory Map with name: Universal.
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
/* MEMORY MAP NAMESPACE                                                              */
/*************************************************************************************/

namespace Atams { namespace BlockUniversal {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(void)
{
  Error_t transferStatus = ERROR_NONE;


  if (transferStatus == ERROR_NONE) transferStatus = write(BLOCK_ID_UNIVERSAL,
                                                           BlockUniversal::MEMBER_ID_NODE_ID,
                                                           BlockUniversal::DEFAULT_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = write(BLOCK_ID_UNIVERSAL,
                                                           BlockUniversal::MEMBER_ID_FIRST_NODE_ID,
                                                           BlockUniversal::DEFAULT_FIRST_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = write(BLOCK_ID_UNIVERSAL,
                                                           BlockUniversal::MEMBER_ID_LAST_NODE_ID,
                                                           BlockUniversal::DEFAULT_LAST_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = write(BLOCK_ID_UNIVERSAL,
                                                           BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID,
                                                           BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = write(BLOCK_ID_UNIVERSAL,
                                                           BlockUniversal::MEMBER_ID_WATCHDOG_TIMEOUT,
                                                           BlockUniversal::DEFAULT_WATCHDOG_TIMEOUT);

  return (transferStatus); 
}

Error_t initLimits(void)
{
  Error_t transferStatus = ERROR_NONE;

  if (transferStatus == ERROR_NONE) transferStatus = assertLimits(BLOCK_ID_UNIVERSAL,
                                                                  BlockUniversal::MEMBER_ID_NODE_ID,
                                                                  BlockUniversal::MAX_LIMIT_NODE_ID,
                                                                  BlockUniversal::MIN_LIMIT_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = assertLimits(BLOCK_ID_UNIVERSAL,
                                                                  BlockUniversal::MEMBER_ID_FIRST_NODE_ID,
                                                                  BlockUniversal::MAX_LIMIT_FIRST_NODE_ID,
                                                                  BlockUniversal::MIN_LIMIT_FIRST_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = assertLimits(BLOCK_ID_UNIVERSAL,
                                                                  BlockUniversal::MEMBER_ID_LAST_NODE_ID,
                                                                  BlockUniversal::MAX_LIMIT_LAST_NODE_ID,
                                                                  BlockUniversal::MIN_LIMIT_LAST_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = assertLimits(BLOCK_ID_UNIVERSAL,
                                                                  BlockUniversal::MEMBER_ID_PREVIOUS_NODE_ID,
                                                                  BlockUniversal::MAX_LIMIT_PREVIOUS_NODE_ID,
                                                                  BlockUniversal::MIN_LIMIT_PREVIOUS_NODE_ID);

  return (transferStatus); 
}

} } /* End Namespace - Atams::DataBlockUniversal */

/**
  * @}End of File
  */
