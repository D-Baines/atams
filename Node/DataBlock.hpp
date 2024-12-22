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
    DataType_t type           = TYPE_NULL;
    Access_t   externalAccess = ACCESS_NONE;
    bool       NVMStorage     = false;
  };

  typedef Error_t (*InitDefaultsFunction_t)(DataBlock &blockToInit);
  typedef Error_t (*InitLimitsFunction_t  )(DataBlock &blockToInit);

  struct BlockDescriptor_t
  {
    uint16_t               noOfDataMembers = 0U;
    InitDefaultsFunction_t initDefaults    = nullptr;
    InitLimitsFunction_t   initLimits      = nullptr;
    MemberInfo_t           dataMemberInfo[Platform::NODE_NUMBER_OF_DATA_MEMBERS];
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

  Error_t initDescriptor(const BlockDescriptor_t &blockDescriptor);

  void resetDataMembers(void);

  void deinit(void);

  template <typename T>
  Error_t write(const uint16_t memberID,
                const T        writeData);

  template <typename T>
  Error_t read(const uint16_t  memberID,
                     T        &readData);

  template <typename T>
  Error_t assertLimits(const uint16_t memberID,
                       const T        limitMax,
                       const T        limitMin);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Error_t setWriteLock(const uint16_t memberID,
                       const bool     writeLock);

  Error_t externalTransfer(const Access_t  accessRequest,
                           const uint16_t  memberID,
                           uint8_t * const dataStoragePtr,
                           const uint8_t   length);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct DataMember_t
  {
    uint8_t data[MAX_TYPE_SIZE]     = {0U, 0U, 0U, 0U};
    uint8_t limitMax[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
    uint8_t limitMin[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
    bool    limitsAsserted          = false;
    bool    writeLock               = false;
  };

  /*-- Private Variables ------------------------------------------------------------*/

  BlockDescriptor_t _blockDescriptor;
  DataMember_t      _dataMembers[Platform::NODE_NUMBER_OF_DATA_MEMBERS];


  /*-- Private Function Declarations ------------------------------------------------*/

  template <typename T>
  Error_t checkLimitsType(const DataMember_t    &dataMember,
                          const uint8_t * const  dataStoragePtr);

  Error_t checkLimits(const MemberInfo_t    &memberInfo,
                      const DataMember_t    &dataMember,
                      const uint8_t * const dataStoragePtr);
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


