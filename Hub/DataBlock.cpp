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

Atams::Error_t DataBlock::initDescriptor(const BlockDescriptor_t &blockDescriptor)
{
  Error_t initStatus = ERROR_NONE;

  if ((blockDescriptor.noOfDataMembers > Platform::NODE_NUMBER_OF_DATA_MEMBERS) ||
      (blockDescriptor.noOfDataMembers >           MAX_NUMBER_OF_DATA_BLOCKS  ) )
  {
    initStatus = ERROR_MEMORY;
  }
  else
  {
    _blockDescriptor = blockDescriptor;
  }

  return (initStatus);
}

void DataBlock::resetDataMembers(void)
{
  Platform::MemoryLock::acquireLock();

  for (DataMember_t &dataMember : _dataMembers)
  {
    //TODO:: Volatile memset may be required
    memset(dataMember.data, 0U, sizeof(dataMember.data));
  }

  Platform::MemoryLock::releaseLock();
}

void DataBlock::deinit(void)
{
  _blockDescriptor.noOfDataMembers = 0U;

  for (MemberInfo_t &varInfo : _blockDescriptor.dataMemberInfo)
  {
    varInfo.type        = TYPE_NULL;
    varInfo.accessLevel = ACCESS_READ;
  }
}

template <typename T>
Atams::Error_t DataBlock::write(const uint16_t  memberID,
                                const T         writeData)
{
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_MEMBER_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()      ) return (ERROR_MEMBER_TYPE);
  if (ACCESS_WRITE                          > memberInfo.accessLevel) return (ERROR_ACCESS_INVALID);

  DataMember_t &dataMember  = _dataMembers[memberID];
  Error_t       accessError = ERROR_NONE;

  Platform::Lock::acquireLock();

  memcpy(&dataMember.data, &writeData, sizeof(dataMember.data));

  Platform::Lock::releaseLock();

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
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_MEMBER_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()      ) return (ERROR_MEMBER_TYPE);
  if (ACCESS_READ                          >  memberInfo.accessLevel) return (ERROR_ACCESS_INVALID);

  DataMember_t &dataMember = _dataMembers[memberID];

  Platform::Lock::acquireLock();

  memcpy(&readData, &dataMember.data, sizeof(readData));

  Platform::Lock::releaseLock();

  return (ERROR_NONE);
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

  if (memberID >= _blockDescriptor.noOfDataMembers)
  {
    lengthReturn.status = ERROR_MEMBER_ID;
    return (lengthReturn);
  }

  const MemberInfo_t &dataMemberInfo = _blockDescriptor.dataMemberInfo[memberID];

  lengthReturn.data   = TYPE_LENGTHS[dataMemberInfo.type];
  lengthReturn.status = ERROR_NONE;

  return (lengthReturn);
}

Error_t DataBlock::externalTransfer(const Access_t  accessRequest,
                                    const uint16_t  memberID,
                                    uint8_t * const inputPtr,
                                    const uint8_t   length)
{
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_MEMBER_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (TYPE_LENGTHS[memberInfo.type] != length ) return (ERROR_MEMBER_LENGTH);
  if (inputPtr                      == nullptr) return (ERROR_NULL_PTR);

  DataMember_t &dataMember  = _dataMembers[memberID];
  Error_t       accessError = ERROR_NONE;

  Platform::MemoryLock::acquireLock();

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
      accessError = ERROR_ACCESS_INVALID;
      break;
  }

  Platform::MemoryLock::acquireLock();

  return (accessError);
}

DataStatusReturn_t<bool> DataBlock::setRequestPattern(const uint16_t         varID,
                                                      const Access_t         accessRequest,
                                                      const RequestPattern_t requestPattern)
{
  DataStatusReturn_t<bool> requestReturn;
  bool &requestChanged = requestReturn.data;
  requestChanged       = false;
  requestReturn.status = ERROR_NONE;

  if (varID >= _blockDescriptor.noOfDataMembers)
  {
    requestReturn.status = ERROR_MEMBER_ID;
    return (requestReturn);
  }

  if (accessRequest > NUMBER_OF_ACCESS_LEVELS)
  {
    requestReturn.status = Atams::ERROR_ACCESS_INVALID;
    return (requestReturn);
  }

  if (requestPattern >= NUMBER_OF_REQUEST_PATTERNS)
  {
    requestReturn.status = ERROR_REQUEST_PATTERN_INVALID;
    return (requestReturn);
  }
 
  DataMember_t &dataMember = _dataMembers[varID];
  MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[varID];

  if (accessRequest > memberInfo.accessLevel)
  {
    requestReturn.status = ERROR_ACCESS_INVALID;
    return (requestReturn);
  }

  Platform::MemoryLock::acquireLock();

  dataMember.requestPattern[accessRequest] = requestPattern;
  requestChanged                           = true;

  Platform::MemoryLock::releaseLock();

  return (requestReturn);
}

DataStatusReturn_t<bool> DataBlock::updateRequestPattern(const uint16_t varID, const Access_t accessRequest)
{
  DataStatusReturn_t<bool> updateStatus;
  updateStatus.status = Atams::ERROR_NONE;
  updateStatus.data   = false;

  if (varID > _blockDescriptor.noOfDataMembers) 
  {
    updateStatus.status = Atams::ERROR_MEMBER_ID;
    return (updateStatus);
  }

  if (accessRequest > NUMBER_OF_ACCESS_LEVELS)
  {
    updateStatus.status = Atams::ERROR_ACCESS_INVALID;
    return (updateStatus);
  }

  Platform::MemoryLock::acquireLock();

  DataMember_t &dataMember = _dataMembers[varID];

  if (dataMember.requestPattern[accessRequest] == REQUEST_UNTIL_ACK)
  {
    dataMember.requestPattern[accessRequest] = REQUEST_INACTIVE;
    updateStatus.data                        = true;
  }

  Platform::MemoryLock::releaseLock();

  return (updateStatus);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
