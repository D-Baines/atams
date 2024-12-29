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
      (blockDescriptor.noOfDataMembers >           MAX_NUMBER_OF_DATA_MEMBERS ) )
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
  Platform::acquireMemoryLock();

  for (DataMember_t &dataMember : _dataMembers)
  {
    //TODO:: Volatile memset may be required
    memset(dataMember.data,     0U, sizeof(dataMember.data));
    memset(dataMember.limitMax, 0U, sizeof(dataMember.limitMax));
    memset(dataMember.limitMin, 0U, sizeof(dataMember.limitMin));
    dataMember.limitsAsserted = false;
    dataMember.writeLock      = false;
  }

  Platform::releaseMemoryLock();
}

void DataBlock::deinit(void)
{
  _blockDescriptor.noOfDataMembers = 0U;

  for (MemberInfo_t &varInfo : _blockDescriptor.dataMemberInfo)
  {
    varInfo.type           = TYPE_NULL;
    varInfo.accessLevel = ACCESS_NONE;
    varInfo.NVMStorage     = false;
  }
}

template <typename T>
Atams::Error_t DataBlock::write(const uint16_t  memberID,
                                const T         writeData)
{
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_VAR_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()) return (ERROR_VAR_TYPE);

  DataMember_t &dataMember  = _dataMembers[memberID];
  Error_t       accessError = ERROR_NONE;

  Platform::acquireMemoryLock();

  if (dataMember.writeLock) accessError = ERROR_WRITE_LOCK;
  else                      memcpy(&dataMember.data, &writeData, sizeof(dataMember.data));

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
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_VAR_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name()) return (ERROR_VAR_TYPE);

  DataMember_t &dataMember = _dataMembers[memberID];

  Platform::acquireMemoryLock();

  memcpy(&readData, &dataMember.data, sizeof(readData));

  Platform::releaseMemoryLock();

  return (ERROR_NONE);
}

template Atams::Error_t DataBlock::read<uint8_t >(const uint16_t memberID, uint8_t  &readData);
template Atams::Error_t DataBlock::read<int8_t  >(const uint16_t memberID, int8_t   &readData);
template Atams::Error_t DataBlock::read<uint16_t>(const uint16_t memberID, uint16_t &readData);
template Atams::Error_t DataBlock::read<int16_t >(const uint16_t memberID, int16_t  &readData);
template Atams::Error_t DataBlock::read<uint32_t>(const uint16_t memberID, uint32_t &readData);
template Atams::Error_t DataBlock::read<int32_t >(const uint16_t memberID, int32_t  &readData);
template Atams::Error_t DataBlock::read<float   >(const uint16_t memberID, float    &readData);


template <typename T>
Atams::Error_t DataBlock::assertLimits(const uint16_t memberID,
                                       const T        limitMax,
                                       const T        limitMin)
{
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_VAR_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (PLATFORM_TYPE_NAMES[memberInfo.type] != typeid(T).name())  return (ERROR_VAR_TYPE);

  DataMember_t &dataMember = _dataMembers[memberID];

  Platform::acquireMemoryLock();

  memcpy(&dataMember.limitMax, &limitMax, sizeof(dataMember.limitMax));
  memcpy(&dataMember.limitMin, &limitMin, sizeof(dataMember.limitMin));
  dataMember.limitsAsserted = true;

  Platform::releaseMemoryLock();

  return (ERROR_NONE);
}

template Atams::Error_t DataBlock::assertLimits<uint8_t >(const uint16_t memberID, const uint8_t  limitMax, const uint8_t  limitMin);
template Atams::Error_t DataBlock::assertLimits<int8_t  >(const uint16_t memberID, const int8_t   limitMax, const int8_t   limitMin);
template Atams::Error_t DataBlock::assertLimits<uint16_t>(const uint16_t memberID, const uint16_t limitMax, const uint16_t limitMin);
template Atams::Error_t DataBlock::assertLimits<int16_t >(const uint16_t memberID, const int16_t  limitMax, const int16_t  limitMin);
template Atams::Error_t DataBlock::assertLimits<uint32_t>(const uint16_t memberID, const uint32_t limitMax, const uint32_t limitMin);
template Atams::Error_t DataBlock::assertLimits<int32_t >(const uint16_t memberID, const int32_t  limitMax, const int32_t  limitMin);
template Atams::Error_t DataBlock::assertLimits<float   >(const uint16_t memberID, const float    limitMax, const float    limitMin);


Atams::Error_t DataBlock::setWriteLock(const uint16_t memberID,
                                       const bool     writeLock)
{
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_VAR_ID);

  DataMember_t &dataMember = _dataMembers[memberID];

  Platform::acquireMemoryLock();

  dataMember.writeLock = writeLock;

  Platform::releaseMemoryLock();

  return (ERROR_NONE);
}

DataStatusReturn_t<uint8_t> DataBlock::getMemberLength(const uint16_t memberID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (memberID >= _blockDescriptor.noOfDataMembers)
  {
    lengthReturn.status = ERROR_VAR_ID;
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
  if (memberID >= _blockDescriptor.noOfDataMembers) return (ERROR_VAR_ID);

  const MemberInfo_t &memberInfo = _blockDescriptor.dataMemberInfo[memberID];

  if (TYPE_LENGTHS[memberInfo.type] != length)                    return (ERROR_VAR_LENGTH);
  if (inputPtr                == nullptr)                   return (ERROR_NULL_PTR);
  if (accessRequest                 >  memberInfo.accessLevel) return (ERROR_ACCESS_INVALID);

  DataMember_t &dataMember  = _dataMembers[memberID];
  Error_t       accessError = ERROR_NONE;

  Platform::acquireMemoryLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(inputPtr, dataMember.data, TYPE_LENGTHS[memberInfo.type]);
      if (systemIsBigEndian()) swapEndiannessRaw(inputPtr, TYPE_LENGTHS[memberInfo.type]);
      break;

    case ACCESS_WRITE:
      if (systemIsBigEndian()) swapEndiannessRaw(inputPtr, TYPE_LENGTHS[memberInfo.type]);

      if (dataMember.writeLock)
      {
        accessError = ERROR_WRITE_LOCK;
      }
      else if ((dataMember.limitsAsserted                                            ) &&
               (checkLimits(memberInfo, dataMember, inputPtr) != ERROR_NONE) )
      {
        accessError = ERROR_LIMITS;
      }
      else
      {
        memcpy(dataMember.data, inputPtr, TYPE_LENGTHS[memberInfo.type]);
      }
      break;

    default:
      accessError = ERROR_ACCESS_INVALID;
      break;
  }

  Platform::releaseMemoryLock();

  return (accessError);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

// TODO:: Make memory map data volatile - volatile memcpy function will be required
template <typename T>
Atams::Error_t DataBlock::checkLimitsType(const DataMember_t    &dataMember,
                                          const uint8_t * const  inputPtr)
{
  T inputAsType;
  T limitMaxAsType;
  T limitMinAsType;

  memcpy(&inputAsType,    inputPtr,            sizeof(inputAsType));
  memcpy(&limitMaxAsType, dataMember.limitMax, sizeof(limitMaxAsType));
  memcpy(&limitMinAsType, dataMember.limitMin, sizeof(limitMinAsType));

  if ((inputAsType > limitMaxAsType) ||
      (inputAsType < limitMinAsType) )
  {
    return (ERROR_LIMITS);
  }

  return (ERROR_NONE);
}

/* Warning - Length checks are omitted and should be handled by calling function */
Atams::Error_t DataBlock::checkLimits(const MemberInfo_t    &memberInfo,
                                      const DataMember_t    &dataMember,
                                      const uint8_t * const  inputPtr)
{
  Atams::Error_t limitStatus = ERROR_NONE;

  switch (memberInfo.type)
  {
    case TYPE_UINT8:
      if (checkLimitsType<uint8_t> (dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT8:
      if (checkLimitsType<int8_t>  (dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_UINT16:
      if (checkLimitsType<uint16_t>(dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT16:
      if (checkLimitsType<int16_t> (dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_UINT32:
      if (checkLimitsType<uint32_t>(dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT32:
      if (checkLimitsType<int32_t> (dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_FLOAT:
      if (checkLimitsType<float>   (dataMember, inputPtr)) limitStatus = ERROR_LIMITS;
      break;
    default:
      limitStatus = ERROR_LIMITS;
      break;
  }

  return (limitStatus);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
