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

class   DataBlock :
private Platform::MemoryLock
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  struct VarInfo_t
  {
    DataType_t type        = TYPE_NULL;
    Access_t   accessLevel = ACCESS_NONE;
    uint8_t    NVMStorage  = false;

    static_assert(sizeof(type)        == 1U, "VarInfo_t member type size invalid");
    static_assert(sizeof(accessLevel) == 1U, "VarInfo_t member type size invalid");
    static_assert(sizeof(NVMStorage)  == 1U, "VarInfo_t member type size invalid");
  };

  typedef Atams::Error_t (&InitDefaultsFnPtr_t)(DataBlock &blockToInit);

  struct Descriptor_t
  {
    uint16_t            noOfDataMembers = 0U;
    InitDefaultsFnPtr_t initDefaults;
    VarInfo_t           varInfo[Platform::NODE_NUMBER_OF_DATA_MEMBERS];

    Descriptor_t(const uint16_t      initNoOfDataMembers,
                 InitDefaultsFnPtr_t defaultsInitFnPtr,
                 const VarInfo_t     (&initVarInfo)[Platform::NODE_NUMBER_OF_DATA_MEMBERS]) :
    initDefaults(defaultsInitFnPtr)
    {
      noOfDataMembers = initNoOfDataMembers;
      for (uint16_t varID = 0U; varID < sizeof(varInfo); varID++)
      {
        varInfo[varID] = initVarInfo[varID];
      }
    };

    Descriptor_t(void) = delete;

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

  Atams::Error_t initDefaults(void);

  void resetDataMembers(void);

  template <typename T>
  Atams::Error_t write(const uint16_t memberID,
                       const T        writeData);

  template <typename T>
  Atams::Error_t writeWithRequestPattern(const uint16_t         memberID,
                                         const T                writeData,          
                                         const RequestPattern_t requestPattern);
  template <typename T>
  Atams::Error_t read(const uint16_t  memberID,
                            T        &readData);

  //TODO:: Change to check new separated function
  template <typename T>
  Atams::Error_t readIfNew(const uint16_t  memberID,
                                 T        &readData);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint16_t memberID);

  Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                  const uint16_t  memberID,
                                  uint8_t * const dataStoragePtr,
                                  const uint8_t   length);
       
  Atams::Error_t setRequestPattern(const uint16_t         varID,
                                   const Access_t         accessRequest,
                                   const RequestPattern_t requestPattern);

  Atams::Error_t getRequestPattern(const uint16_t    varID,
                                   Access_t         &accessRequest,
                                   RequestPattern_t &requestPattern);

  Atams::Error_t setRequestPatternNoChecks(const uint16_t         varID,
                                           const Access_t         accessRequest,
                                           const RequestPattern_t requestPattern);


  protected:

  /*-- Protected Functions ----------------------------------------------------------*/

  Atams::Error_t initDescriptor(const Descriptor_t * const blockDescriptor);

  void deinitDescriptor(void);
                                           
  private:

  /*-- Private Constants ------------------------------------------------------------*/

  /*-- Private Typedefs -------------------------------------------------------------*/

  struct DataMember_t
  {
    uint8_t          data[MAX_TYPE_SIZE] = {0U, 0U, 0U, 0U};
    Access_t         requestAccess       = Atams::ACCESS_NONE;
    RequestPattern_t requestPattern      = Atams::REQUEST_INACTIVE;
    bool             newDataReady        = false;
  };

  /*-- Private Variables ------------------------------------------------------------*/

  const Descriptor_t *_blockDescriptorPtr   = nullptr;
  uint16_t            _validNoOfDataMembers = 0U;
  DataMember_t        _dataMembers[Platform::NODE_NUMBER_OF_DATA_MEMBERS];
};


} /* End Namespace - Atams */


/**
  * @}End of File
  */


