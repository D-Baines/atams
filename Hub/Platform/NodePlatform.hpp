/**
  ******************************************************************************
  * @file    NodePlatform.hpp
  *
  * @author  D. Baines
  *
  * @brief
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/


class NodePlatform
{
  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct UserData_t
  {

  };

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  NodePlatform(NodePlatform::UserData_t userData);

  /* Copy Constructor */
  NodePlatform(const NodePlatform &other) = delete;

  /* Copy Assignment Operator */
  NodePlatform & operator=(const NodePlatform &other) = delete;

  /* Destructor */
  ~NodePlatform(void);

  void acquireMemoryLock(void);

  void releaseMemoryLock(void);


  /*-- Private ----------------------------------------------------------------------*/

  private:


  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  NodePlatform::UserData_t _userData;

  /*-- Private Function Declarations ------------------------------------------------*/

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


