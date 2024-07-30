/**
  ******************************************************************************
  * @file    DataBlock.hpp
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

#include "../../Atams/Node/Platform.hpp"
#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Node {


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
    Access_t   externalAccess = ACCESS_NONE_NACK;
    uint32_t   NVMOffset      = 0U;
    uint32_t   OTPOffset      = 0U;
  };

  struct BlockDescriptor_t
  {
    uint16_t     noOfDataMembers;
    MemberInfo_t dataMemberInfo[Platform::NODE_NUMBER_OF_DATA_MEMBERS];
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

  Error_t init(const BlockDescriptor_t &blockDescriptor);

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

  Error_t checkLimits(const MemberInfo_t &memberInfo,
                      const DataMember_t       &dataMember,
                      const uint8_t * const    dataStoragePtr);
};


} } /* End Namespace - Atams::Node */


/**
  * @}End of File
  */


