/**
  ******************************************************************************
  * @file    CircularBuffer.hpp
  *
  * @author  D. Baines
  *
  * @brief   Node-specific CircularBuffer instantiation.
  *
  * @details Defines NodeCommsLockPolicy, which delegates to the Platform 
  *          functions acquireCommsBufferLock / releaseCommsBufferLock, and aliases
  *          CircularBufferBase<NodeCommsLockPolicy> as CircularBuffer for use
  *          within the Node library.
  *
  *          The lock argument identifies which comms peripheral to lock, enabling
  *          multi-peripheral Node devices to use independent buffers safely.
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

#include "../CommsCore/CommsPlatform.hpp"
#include "../../Shared/Utilities/CircularBuffer.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* LOCK POLICY                                                                       */
/*************************************************************************************/

class NodeCommsLockPolicy
{
  public:
  
  using ArgumentType_t = Platform::CommsPeripheralID_t;

  static constexpr uint16_t BUFFER_SIZE {Platform::CIRCULAR_BUFFER_SIZE};

  void setArgument(ArgumentType_t id) { id_ = id; }

  void acquireLock(void) { Platform::acquireCommsBufferLock(id_); }
  void releaseLock(void) { Platform::releaseCommsBufferLock(id_); }

  private:

  Platform::CommsPeripheralID_t id_ {static_cast<Platform::CommsPeripheralID_t>(0U)};
};

/*************************************************************************************/
/* TYPE ALIAS                                                                        */
/*************************************************************************************/

using CircularBuffer = CircularBufferBase<NodeCommsLockPolicy>;

} /* End Namespace - Atams */


/**
  * @}End of File
  */
