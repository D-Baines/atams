/**
  ******************************************************************************
  * @file    ClassTemplate.hpp
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class ClassTemplate 
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  ClassTemplate(void) = default;

  /* Default Constructor */
  ~ClassTemplate(void) = default;

  /* Copy Constructor */
  ClassTemplate(const ClassTemplate &other) = delete;

  /* Copy Assignment Operator */
  ClassTemplate & operator=(ClassTemplate &other) = delete;

  /* Move Constructor */
  ClassTemplate(ClassTemplate &&ClassTemplate) = delete;

  /* Move Assignment Operator */
  ClassTemplate & operator=(ClassTemplate &&other) = delete;

  private:

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  /*-- Private Function Declarations ------------------------------------------------*/

};


} /* End Namespace - Atams */

/**
  * @}End of File
  */

