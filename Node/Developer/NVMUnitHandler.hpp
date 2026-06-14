/**
  ******************************************************************************
  * @file    NVMHandler.hpp
  *
  * @author  D. Baines
  *
  * @brief   Handler for fixed-unit non-volatile memory access on Atams Node devices.
  *
  * @details Defines the NVMUnitHandler class, which manages the buffering and writing
  *          of data to non-volatile memory in fixed-size units, as required by platforms
  *          that cannot write arbitrary byte lengths. Provides sequential write buffering,
  *          NVM erase, read, and flush operations. A rolling CRC is maintained over all
  *          written data to enable write integrity verification.
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
#include "../../Shared/AtamsTypedefs.hpp"
#include "../../Shared/Utilities/CRC32.hpp"
#include "../CommsCore/CommsPlatform.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class NVMUnitHandler
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  NVMUnitHandler(Atams::CRC32 &nvmCRC);

  /* Default Constructor */
  NVMUnitHandler(void) = delete;

  /* Default Destructor */
  ~NVMUnitHandler(void) = default;

  /* Copy Constructor */
  NVMUnitHandler(const NVMUnitHandler &other) = delete;

  /* Copy Assignment Operator */
  NVMUnitHandler & operator=(const NVMUnitHandler &other) = delete;

  /* Move Constructor */
  NVMUnitHandler(NVMUnitHandler &&other) = delete;

  /* Move Assignment Operator */
  NVMUnitHandler & operator=(NVMUnitHandler &&other) = delete;

  Atams::Error_t eraseNVM(void);

  Atams::Error_t writeToNVM(const uint32_t writeIndex, const uint8_t * const inputPtr, const uint32_t length);

  Atams::Error_t readFromNVM(const uint32_t readIndex, uint8_t * const outputPtr, const uint32_t length);

  Atams::Error_t flushPendingUnit(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Objects --------------------------------------------------------------*/

  Atams::CRC32 &nvmCRC_;

  /*-- Private Variables ------------------------------------------------------------*/

  uint8_t  unitIndex_ = 0U;
  uint8_t  unitBuffer_[Platform::NVM_UNIT_SIZE];
  uint32_t unitStartIndex_ = 0U;

  /*-- Private Function Declarations -------------------------------------------------*/

};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


