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

  struct MemberInfo_t
  {
    DataType_t type        = TYPE_NULL;
    Access_t   accessLevel = ACCESS_NONE;
    bool       NVMStorage  = false;
  };

  typedef Atams::Error_t (&InitDefaultsFn_t)(DataBlock &blockToInit);

  struct BlockDescriptor_t
  {
    uint16_t         noOfDataMembers = 0U;
    InitDefaultsFn_t initDefaults;
    MemberInfo_t     dataMemberInfo[Platform::NODE_NUMBER_OF_DATA_MEMBERS];

    BlockDescriptor_t(const uint16_t     initNoOfDataMembers,
                      InitDefaultsFn_t   defaultsInitFnPtr,
                      const MemberInfo_t (&initVarInfo)[Platform::NODE_NUMBER_OF_DATA_MEMBERS]) :
    initDefaults(defaultsInitFnPtr)
    {
      noOfDataMembers = initNoOfDataMembers;
      for (uint16_t varID = 0U; varID < Platform::NODE_NUMBER_OF_DATA_MEMBERS; varID++)
      {
        dataMemberInfo[varID] = initVarInfo[varID];
      }
    };

    BlockDescriptor_t(const BlockDescriptor_t &other) = delete;

    BlockDescriptor_t& operator=(const BlockDescriptor_t &other) = delete;
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

  Atams::Error_t initDescriptor(const BlockDescriptor_t * const blockDescriptor);

  void deinitDescriptor(void);

  Atams::Error_t initDefaults(void);

  void resetDataMembers(void);

  template <typename T>
  Atams::Error_t write(const uint16_t memberID,
                       const T        writeData);

  template <typename T>
  Atams::Error_t read(const uint16_t  memberID,
                            T        &readData);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  memberID,
                                  uint8_t * const inputPtr,
                                  const uint8_t   length);

  uint16_t getVariableCount(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct DataMember_t
  {
    uint8_t data[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
  };

  /*-- Private Variables ------------------------------------------------------------*/

  const BlockDescriptor_t *_blockDescriptorPtr = nullptr;
  uint16_t                 _validVariableCount = 0U;
  DataMember_t             _dataMembers[Platform::NODE_NUMBER_OF_DATA_MEMBERS];
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


