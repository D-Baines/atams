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
#include "Platform/Platform.hpp"
#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class   DataBlock :
private Platform::MemoryLock
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct MemberInfo_t
  {
    DataType_t type        = TYPE_NULL;
    Access_t   accessLevel = ACCESS_READ;
  };

  struct BlockDescriptor_t
  {
    uint16_t     noOfDataMembers = 0U;
    MemberInfo_t dataMemberInfo[MAX_NUMBER_OF_DATA_MEMBERS];
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

  Atams::Error_t initDescriptor(const BlockDescriptor_t &blockDescriptor);

  void resetDataMembers(void);

  void deinit(void);

  template <typename T>
  Atams::Error_t write(const uint16_t memberID,
                       const T        writeData);

  template <typename T>
  Atams::Error_t read(const uint16_t  memberID,
                            T        &readData);

  template <typename T>
  Atams::Error_t readIfNew(const uint16_t  memberID,
                                 T        &readData);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  memberID,
                                  uint8_t * const dataStoragePtr,
                                  const uint8_t   length);
       
  DataStatusReturn_t<bool> setRequestPattern(const uint16_t         varID,
                                             const Access_t         accessRequest,
                                             const RequestPattern_t requestPattern);

  Atams::Error_t getRequestPattern(const uint16_t    varID,
                                   Access_t         &accessRequest,
                                   RequestPattern_t &requestPattern);

  DataStatusReturn_t<bool> setRequestPatternNoChecks(const uint16_t         varID,
                                                     const Access_t         accessRequest,
                                                     const RequestPattern_t requestPattern);

  DataStatusReturn_t<bool> updateRequestPattern(const uint16_t varID);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct DataMember_t
  {
    uint8_t          data[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
    Access_t         requestAccess  = Atams::ACCESS_NONE;
    RequestPattern_t requestPattern = Atams::REQUEST_INACTIVE;
    bool             newDataReady   = false;
  };

  /*-- Private Variables ------------------------------------------------------------*/

  BlockDescriptor_t _blockDescriptor;
  DataMember_t      _dataMembers[Platform::NODE_NUMBER_OF_DATA_MEMBERS];
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


