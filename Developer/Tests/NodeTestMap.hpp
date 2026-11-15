/**
  ******************************************************************************
  * @file    NodeTestMap.hpp
  *
  * @author  D. Baines
  *
  * @brief   Access to the Node MapTest Memory Map for test files.
  *
  * @details The Hub and Node MapTest headers define the same names, so one source file
  *          cannot include both. Test files include the Hub map for its var IDs (the
  *          Node map's are identical) and get the Node map through getNodeTestMap().
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

#include "Node/Developer/NodeTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTests {

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

const Atams::MemoryMap_t & getNodeTestMap(void);

} /* End Namespace - AtamsTests */


/**
  * @}End of File
  */
