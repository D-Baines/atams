/**
  ******************************************************************************
  * @file    BlockOwner.hpp
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

/* Define to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "DataBlock.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class BlockOwnerInteractor :
public DataBlock
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  BlockOwnerInteractor(void);

  /* Destructor */
  ~BlockOwnerInteractor(void);

  /* Copy Constructor */
  BlockOwnerInteractor(const BlockOwnerInteractor &other) = delete;

  /* Copy Assignment Operator */
  BlockOwnerInteractor & operator=(const BlockOwnerInteractor &other) = delete;

  Atams::Error_t initDescriptor(const DataBlock::Descriptor_t * const blockDescriptor);

  void deinitDescriptor(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Variables ------------------------------------------------------------*/

  /*-- Private Function Declarations -------------------------------------------------*/

};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


