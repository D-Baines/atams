/**
  ******************************************************************************
  * @file    NVMTests.hpp
  *
  * @author  D. Baines
  *
  * @brief   Manual test entry point for exercising the Node's NVM store/load
  *          behaviour on real hardware.
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace NVMTests {

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

/**
 * @brief Runs the NVM store/load test scenarios.
 *
 * @retval 0     All tests passed.
 * @retval other Number of failed checks - set a breakpoint in checkCondition()'s
 *               failure branch (NVMTests.cpp) to catch the exact failing check.
 */
uint16_t runTests(void);

} } /* End Namespace - Atams::NVMTests */

/**
  * @}End of File
  */
