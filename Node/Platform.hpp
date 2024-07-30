/**
  ******************************************************************************
  * @file    AtamsPlatform.hpp
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

#include <stdint.h>
#include "../AtamsTypedefs.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {


/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

constexpr inline uint16_t NODE_NUMBER_OF_DATA_MEMBERS = 200U; /* Must be <= MAX_NUMBER_OF_DATA_MEMBERS */
constexpr inline uint16_t NODE_NUMBER_OF_DATA_BLOCKS  = 2U;   /* Must be <= MAX_NUMBER_OF_DATA_BLOCKS  */


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

void     setReceiveCallback(CommsReceiveCallback_t receiveCallback);

Error_t  transmitBuffer(uint8_t *buffer, uint16_t length);

uint32_t getMillis(void);

void acquireMemoryLock(void);

void releaseMemoryLock(void);

void acquireCommsBufferLock(void);

void releaseCommsBufferLock(void);


} } /* End Atams::Platform Namespace */


/**
  * @}End of File
  */
