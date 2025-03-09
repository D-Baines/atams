/**
  ******************************************************************************
  * @file    DataBlock.cpp
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

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <cstring>
#include <typeinfo>
#include "DataBlock.hpp"
#include "../Utilities/AtamsUtilities.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PRIVATE STATIC CONSTANTS                                                          */
/*************************************************************************************/

static const char * PLATFORM_TYPE_NAMES[NUMBER_OF_TYPES] =
{
  /* [TYPE_NULL  ] = */ "NULL",
  /* [TYPE_UINT8 ] = */ typeid(uint8_t ).name(),
  /* [TYPE_INT8  ] = */ typeid(int8_t  ).name(),
  /* [TYPE_UINT16] = */ typeid(uint16_t).name(),
  /* [TYPE_INT16 ] = */ typeid(int16_t ).name(),
  /* [TYPE_UINT32] = */ typeid(uint32_t).name(),
  /* [TYPE_INT32 ] = */ typeid(int32_t ).name(),
  /* [TYPE_FLOAT ] = */ typeid(float   ).name(),
};

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

DataBlock::DataBlock(void)
{

}

DataBlock::~DataBlock(void)
{

}

Atams::Error_t DataBlock::initDescriptor(const BlockDescriptor_t * const blockDescriptorPtr)
{
  Atams::Error_t initStatus = ERROR_NONE;

  if (blockDescriptorPtr == nullptr)
  {
    initStatus = Atams::ERROR_NULL_PTR;
  }
  else if ((blockDescriptorPtr->noOfDataMembers > sizeof(_blockDescriptorPtr->dataMemberInfo)) ||
           (blockDescriptorPtr->noOfDataMembers > MAX_NUMBER_OF_DATA_MEMBERS                 ) )
  {
    initStatus = Atams::ERROR_NUMBER_OF_DATA_MEMBERS;
  }
  else
  {
    _blockDescriptorPtr   = blockDescriptorPtr;
    _validNoOfDataMembers = blockDescriptorPtr->noOfDataMembers;
  }

  resetDataMembers();

  return (initStatus);
}

void DataBlock::deinitDescriptor(void)
{
  _validNoOfDataMembers = 0U;
  _blockDescriptorPtr   = nullptr;
  resetDataMembers();
}

Atams::Error_t DataBlock::initDefaults(void)
{
  if (_blockDescriptorPtr == nullptr) return (Atams::ERROR_NULL_PTR);
  else                                return (_blockDescriptorPtr->initDefaults(*this));
}

void DataBlock::resetDataMembers(void)
{
  Platform::acquireMemoryLock();

  for (DataMember_t &dataMember : _dataMembers)
  {
    //TODO:: Volatile memset may be required
    memset(dataMember.data, 0U, sizeof(dataMember.data));
  }

  Platform::releaseMemoryLock();
}

template <typename T>
Atams::Error_t DataBlock::write(const uint16_t  memberID,
                                const T         writeData)
{
  if (memberID >= _validNoOfDataMembers) return (ERROR_VAR_ID); /* Early Return */

  const MemberInfo_t &memberInfo = _blockDescriptorPtr->dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()) return (ERROR_VAR_TYPE); /* Early Return */

  DataMember_t   &dataMember  = _dataMembers[memberID];
  Atams::Error_t  accessError = ERROR_NONE;

  Platform::acquireMemoryLock();

  memcpy(&dataMember.data, &writeData, sizeof(dataMember.data));

  Platform::releaseMemoryLock();

  return (accessError);
}

template Atams::Error_t DataBlock::write<uint8_t >(const uint16_t memberID, const uint8_t  writeData);
template Atams::Error_t DataBlock::write<int8_t  >(const uint16_t memberID, const int8_t   writeData);
template Atams::Error_t DataBlock::write<uint16_t>(const uint16_t memberID, const uint16_t writeData);
template Atams::Error_t DataBlock::write<int16_t >(const uint16_t memberID, const int16_t  writeData);
template Atams::Error_t DataBlock::write<uint32_t>(const uint16_t memberID, const uint32_t writeData);
template Atams::Error_t DataBlock::write<int32_t >(const uint16_t memberID, const int32_t  writeData);
template Atams::Error_t DataBlock::write<float   >(const uint16_t memberID, const float    writeData);


template <typename T>
Atams::Error_t DataBlock::read(const uint16_t  memberID,
                                     T        &readData)
{
  if (memberID >= _validNoOfDataMembers) return (ERROR_VAR_ID); /* Early Return */

  const MemberInfo_t &memberInfo = _blockDescriptorPtr->dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()) return (ERROR_VAR_TYPE); /* Early Return */

  DataMember_t &dataMember = _dataMembers[memberID];

  Platform::acquireMemoryLock();

  memcpy(&readData, &dataMember.data, sizeof(readData));

  Platform::releaseMemoryLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t DataBlock::read<uint8_t >(const uint16_t memberID, uint8_t  &readData);
template Atams::Error_t DataBlock::read<int8_t  >(const uint16_t memberID, int8_t   &readData);
template Atams::Error_t DataBlock::read<uint16_t>(const uint16_t memberID, uint16_t &readData);
template Atams::Error_t DataBlock::read<int16_t >(const uint16_t memberID, int16_t  &readData);
template Atams::Error_t DataBlock::read<uint32_t>(const uint16_t memberID, uint32_t &readData);
template Atams::Error_t DataBlock::read<int32_t >(const uint16_t memberID, int32_t  &readData);
template Atams::Error_t DataBlock::read<float   >(const uint16_t memberID, float    &readData);

DataStatusReturn_t<uint8_t> DataBlock::getMemberLength(const uint16_t memberID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (memberID >= _validNoOfDataMembers)
  {
    lengthReturn.status = Atams::ERROR_VAR_ID;
    return (lengthReturn); /* Early Return */
  }

  const MemberInfo_t &dataMemberInfo = _blockDescriptorPtr->dataMemberInfo[memberID];

  lengthReturn.data   = TYPE_LENGTHS[dataMemberInfo.type];
  lengthReturn.status = ERROR_NONE;

  return (lengthReturn);
}

Error_t DataBlock::externalTransfer(const Access_t  accessRequest,
                                    const uint16_t  memberID,
                                    uint8_t * const inputPtr,
                                    const uint8_t   length)
{
  if (memberID >= _validNoOfDataMembers) return (ERROR_VAR_ID); /* Early Return */

  const MemberInfo_t &memberInfo = _blockDescriptorPtr->dataMemberInfo[memberID];

  if (TYPE_LENGTHS[memberInfo.type] != length)                 return (Atams::ERROR_VAR_LENGTH);     /* Early Return */
  if (inputPtr                      == nullptr)                return (Atams::ERROR_NULL_PTR);       /* Early Return */
  if (accessRequest                 >  memberInfo.accessLevel) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  DataMember_t   &dataMember = _dataMembers[memberID];
  Atams::Error_t accessError = Atams::ERROR_NONE;

  Platform::acquireMemoryLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(inputPtr, dataMember.data, TYPE_LENGTHS[memberInfo.type]);
      if (systemIsBigEndian()) swapEndiannessRaw(inputPtr, TYPE_LENGTHS[memberInfo.type]);
      break;

    case ACCESS_WRITE:
      if (systemIsBigEndian()) swapEndiannessRaw(inputPtr, TYPE_LENGTHS[memberInfo.type]);
      memcpy(dataMember.data, inputPtr, TYPE_LENGTHS[memberInfo.type]);
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  Platform::releaseMemoryLock();

  return (accessError);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
