/**
  ******************************************************************************
  * @file    CircularBuffer.hpp
  *
  * @author  D. Baines
  *
  * @brief   Hub-specific CircularBuffer instantiation.
  *
  * @details Defines HubCommsLockPolicy, which wraps Platform::CommsLock, and
  *          aliases CircularBufferBase<HubCommsLockPolicy> as CircularBuffer for
  *          use within the Hub library.
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include "../Platform.hpp"
#include "../../Shared/Utilities/CircularBuffer.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* LOCK POLICY                                                                       */
/*************************************************************************************/

class HubCommsLockPolicy
{
  public:

  using ArgumentType_t = uint8_t; /* unused - Hub lock requires no discriminator */

  static constexpr uint16_t BUFFER_SIZE {Platform::CIRCULAR_BUFFER_SIZE};

  void setArgument(ArgumentType_t) {}

  void acquireLock(void) { commsLock_.acquireLock(); }
  void releaseLock(void) { commsLock_.releaseLock(); }

  private:
  
  Platform::CommsLock commsLock_;
};

/*************************************************************************************/
/* TYPE ALIAS                                                                        */
/*************************************************************************************/

using CircularBuffer = CircularBufferBase<HubCommsLockPolicy>;

} /* End Namespace - Atams */


/**
  * @}End of File
  */
