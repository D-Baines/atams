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

}

BlockOwnerInteractor::~BlockOwnerInteractor(void)
{

}

Atams::Error_t BlockOwnerInteractor::transferFullAccess(const Access_t  accessRequest,
                                                        const uint16_t  memberID,
                                                        uint8_t * const inputPtr,
                                                        const uint8_t   length)
{
  return (DataBlock::transferFullAccess(accessRequest, memberID, inputPtr, length));
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */
