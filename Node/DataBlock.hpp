/**
  ******************************************************************************
  * @file    DataBlock.hpp
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
#include "Platform.hpp"
#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class DataBlock
{

  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct VarInfo_t
  {
    DataType_t type        = TYPE_NULL;
    Access_t   accessLevel = ACCESS_NONE;
    bool       NVMStorage  = false;
  };

  typedef Atams::Error_t (&InitDefaultsFnPtr_t)(DataBlock &blockToInit);

  struct Descriptor_t
  {
    uint16_t            noOfDataMembers = 0U;
    GenInfo_t           genInfo;
    InitDefaultsFnPtr_t initDefaults;
    VarInfo_t           varInfo[Platform::NODE_NUMBER_OF_DATA_MEMBERS];

    Descriptor_t(void) = delete;

    Descriptor_t(const uint16_t      initNoOfDataMembers,
                 const GenInfo_t     initGenInfo,
                 InitDefaultsFnPtr_t defaultsInitFnPtr,
                 const VarInfo_t     (&initVarInfo)[Platform::NODE_NUMBER_OF_DATA_MEMBERS]) :
    initDefaults(defaultsInitFnPtr)
    {
      noOfDataMembers = initNoOfDataMembers;
      genInfo         = initGenInfo;

      for (uint16_t varID = 0U; varID < Platform::NODE_NUMBER_OF_DATA_MEMBERS; varID++)
      {
        varInfo[varID] = initVarInfo[varID];
      }
    };

    Descriptor_t(const Descriptor_t &other) = delete;

    Descriptor_t& operator=(const Descriptor_t &other) = delete;
  };

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Constructor */
  DataBlock(void);

  /* Destructor */
  ~DataBlock(void);

  /* Copy Constructor */
  DataBlock(const DataBlock &other) = delete;

  /* Copy Assignment Operator */
  DataBlock & operator=(const DataBlock &other) = delete;

  Atams::Error_t initDescriptor(const Descriptor_t * const blockDescriptor);

  void deinitDescriptor(void);

  Atams::Error_t initDefaults(void);

  void resetDataMembers(void);

  template <typename T>
  Atams::Error_t write(const uint16_t memberID, const T writeData);

  template <typename T>
  Atams::Error_t read(const uint16_t memberID, T &readData);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  memberID,
                                  uint8_t * const inputPtr,
                                  const uint8_t   length);

  protected:

  /*-- Protected Constants ----------------------------------------------------------*/

  /*-- Protected Typedefs -----------------------------------------------------------*/

  struct DataMember_t
  {
    uint8_t data[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
  };

  /*-- Protected Variables ----------------------------------------------------------*/

  const Descriptor_t *_blockDescriptorPtr = nullptr;
  uint16_t            _validVariableCount = 0U;
  uint32_t            _nvmStorageOffset   = 0U;
  DataMember_t        _varStorage[Platform::NODE_NUMBER_OF_DATA_MEMBERS];

};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


