/**
  ******************************************************************************
  * @file    BusPlatform.hpp
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


class BusPlatform
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
  BusPlatform(BusPlatform::UserData_t userData);

  /* Copy Constructor */
  BusPlatform(const BusPlatform &other) = delete;

  /* Copy Assignment Operator */
  BusPlatform & operator=(const BusPlatform &other) = delete;

  /* Destructor */
  ~BusPlatform(void);


  /*-- Private ----------------------------------------------------------------------*/

  private:


  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  BusPlatform::UserData_t _userData;

  /*-- Private Function Declarations ------------------------------------------------*/

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */


