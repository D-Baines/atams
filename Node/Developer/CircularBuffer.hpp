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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
