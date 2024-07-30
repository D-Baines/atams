/**
  ******************************************************************************
  * @file    Node.cpp
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
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */


/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <cstring>

#include "Node.hpp"
#include "Bus.hpp"
#include "../Options.hpp"
#include "../Utilities/AtamsUtilities.hpp"
#include "Devices/UniversalMemoryMap.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

static const char * PLATFORM_TYPE_NAMES[NUMBER_OF_TYPES] =
{
  [TYPE_NULL  ] = "NULL",
  [TYPE_UINT8 ] = typeid(uint8_t ).name(),
  [TYPE_INT8  ] = typeid(int8_t  ).name(),
  [TYPE_UINT16] = typeid(uint16_t).name(),
  [TYPE_INT16 ] = typeid(int16_t ).name(),
  [TYPE_UINT32] = typeid(uint32_t).name(),
  [TYPE_INT32 ] = typeid(int32_t ).name(),
  [TYPE_FLOAT ] = typeid(float   ).name(),
};

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Node::Node(const MemoryMap_t &memoryMap, const Bus &bus, NodePlatform::UserData_t userData) :
NodePlatform(userData),
_memoryMap(memoryMap),
_bus(bus)
{
  /* Do Nothing */
}

Error_t Node::init(void)
{
  Error_t initStatus = ERROR_NONE;

  initStatus = _memoryMap.initDefaults(*this);

  if (initStatus == ERROR_NONE) initStatus = _memoryMap.initLimits(*this);

  return (initStatus);
}

template <typename T>
Error_t Node::write(const uint8_t   blockID,
                    const uint16_t  memberID,
                    const T        &writeData)
{
  if (blockID  >= _memoryMap.noOfDataFields)                      return (ERROR_BLOCK_ID_OOR);
  if (memberID >= _memoryMap.dataFields[blockID].noOfDataMembers) return (ERROR_MEMBER_ID_OOR);

  DataMember_t &dataMember = _memoryMap.dataFields[blockID].dataMembers[memberID];

  if (PLATFORM_TYPE_NAMES[dataMember.type] != typeid(T).name())   return (ERROR_MEMBER_TYPE);

  Error_t accessError = ERROR_NONE;

  NodePlatform::acquireMemoryLock();

  if (dataMember.writeLock) accessError = ERROR_WRITE_LOCK;
  else                      std::memcpy(&dataMember.data, &writeData, sizeof(T));

  NodePlatform::releaseMemoryLock();

  return (accessError);
}

template Error_t Node::write<uint8_t >(const uint8_t blockID, const uint16_t memberID, const uint8_t  &writeData);
template Error_t Node::write<int8_t  >(const uint8_t blockID, const uint16_t memberID, const int8_t   &writeData);
template Error_t Node::write<uint16_t>(const uint8_t blockID, const uint16_t memberID, const uint16_t &writeData);
template Error_t Node::write<int16_t >(const uint8_t blockID, const uint16_t memberID, const int16_t  &writeData);
template Error_t Node::write<uint32_t>(const uint8_t blockID, const uint16_t memberID, const uint32_t &writeData);
template Error_t Node::write<int32_t >(const uint8_t blockID, const uint16_t memberID, const int32_t  &writeData);
template Error_t Node::write<float   >(const uint8_t blockID, const uint16_t memberID, const float    &writeData);


template <typename T>
Error_t Node::read(const uint8_t   blockID,
                   const uint16_t  memberID,
                         T        &readData)
{
  if (blockID  >= _memoryMap.noOfDataFields)                      return (ERROR_BLOCK_ID_OOR);
  if (memberID >= _memoryMap.dataFields[blockID].noOfDataMembers) return (ERROR_MEMBER_ID_OOR);

  DataMember_t &dataMember = _memoryMap.dataFields[blockID].dataMembers[memberID];

  if (PLATFORM_TYPE_NAMES[dataMember.type] != typeid(T).name())   return (ERROR_MEMBER_TYPE);

  NodePlatform::acquireMemoryLock();

  readData = *(reinterpret_cast<T*>(dataMember.data));

  NodePlatform::releaseMemoryLock();

  return (ERROR_NONE);
}

template Error_t Node::read<uint8_t >(const uint8_t blockID, const uint16_t memberID, uint8_t  &readData);
template Error_t Node::read<int8_t  >(const uint8_t blockID, const uint16_t memberID, int8_t   &readData);
template Error_t Node::read<uint16_t>(const uint8_t blockID, const uint16_t memberID, uint16_t &readData);
template Error_t Node::read<int16_t >(const uint8_t blockID, const uint16_t memberID, int16_t  &readData);
template Error_t Node::read<uint32_t>(const uint8_t blockID, const uint16_t memberID, uint32_t &readData);
template Error_t Node::read<int32_t >(const uint8_t blockID, const uint16_t memberID, int32_t  &readData);
template Error_t Node::read<float   >(const uint8_t blockID, const uint16_t memberID, float    &readData);


template <typename T>
Error_t Node::assertLimits(const uint8_t   blockID,
                           const uint16_t  memberID,
                           const T         limitMax,
                           const T         limitMin)
{
  if (blockID  >= _memoryMap.noOfDataFields)                      return (ERROR_BLOCK_ID_OOR);
  if (memberID >= _memoryMap.dataFields[blockID].noOfDataMembers) return (ERROR_MEMBER_ID_OOR);

  DataMember_t &dataMember = _memoryMap.dataFields[blockID].dataMembers[memberID];

  if (TYPE_LENGTHS[dataMember.type] != sizeof(T))                 return (ERROR_MEMBER_LENGTH);

  NodePlatform::acquireMemoryLock();

  *(reinterpret_cast<T*>(dataMember.limitMax)) = limitMax;
  *(reinterpret_cast<T*>(dataMember.limitMin)) = limitMin;
  dataMember.limitsAsserted                    = true;

  NodePlatform::releaseMemoryLock();

  return (ERROR_NONE);
}

template Error_t Node::assertLimits<uint8_t >(const uint8_t blockID, const uint16_t memberID, const uint8_t  limitMax, const uint8_t  limitMin);
template Error_t Node::assertLimits<int8_t  >(const uint8_t blockID, const uint16_t memberID, const int8_t   limitMax, const int8_t   limitMin);
template Error_t Node::assertLimits<uint16_t>(const uint8_t blockID, const uint16_t memberID, const uint16_t limitMax, const uint16_t limitMin);
template Error_t Node::assertLimits<int16_t >(const uint8_t blockID, const uint16_t memberID, const int16_t  limitMax, const int16_t  limitMin);
template Error_t Node::assertLimits<uint32_t>(const uint8_t blockID, const uint16_t memberID, const uint32_t limitMax, const uint32_t limitMin);
template Error_t Node::assertLimits<int32_t >(const uint8_t blockID, const uint16_t memberID, const int32_t  limitMax, const int32_t  limitMin);
template Error_t Node::assertLimits<float   >(const uint8_t blockID, const uint16_t memberID, const float    limitMax, const float    limitMin);


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

DataStatusReturn_t<uint8_t> Node::getMemberLength(const uint8_t blockID, const uint16_t memberID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (blockID >= _memoryMap.noOfDataFields)
  {
    lengthReturn.status = ERROR_BLOCK_ID_OOR;
    return (lengthReturn);
  }

  if (memberID >= _memoryMap.dataFields[blockID].noOfDataMembers)
  {
    lengthReturn.status = ERROR_MEMBER_ID_OOR;
    return (lengthReturn);
  }

  DataMember_t &dataMember = _memoryMap.dataFields[blockID].dataMembers[memberID];

  lengthReturn.data   = TYPE_LENGTHS[dataMember.type];
  lengthReturn.status = ERROR_NONE;

  return (lengthReturn);
}

Error_t Node::externalTransfer(const Access_t  accessRequest,
                               const uint8_t   blockID,
                               const uint16_t  memberID,
                               uint8_t * const dataStoragePtr,
                               const uint8_t   length)
{
  if (blockID  >= _memoryMap.noOfDataFields)                      return (recordError(ERROR_BLOCK_ID_OOR));
  if (memberID >= _memoryMap.dataFields[blockID].noOfDataMembers) return (recordError(ERROR_MEMBER_ID_OOR));

  DataMember_t &dataMember = _memoryMap.dataFields[blockID].dataMembers[memberID];

  if (TYPE_LENGTHS[dataMember.type] != length)                    return (recordError(ERROR_MEMBER_LENGTH));
  if (dataStoragePtr                == nullptr)                   return (recordError(ERROR_NULL_PTR));
  if (accessRequest                 >  dataMember.externalAccess) return (recordError(ERROR_ACCESS_INVALID));

  Error_t accessError = ERROR_NONE;

  NodePlatform::acquireMemoryLock();

  switch (accessRequest)
  {
    case ACCESS_READ_ACK:
      memcpy(dataStoragePtr, dataMember.data, TYPE_LENGTHS[dataMember.type]);
      if (systemIsBigEndian()) swapEndiannessRaw(dataStoragePtr, TYPE_LENGTHS[dataMember.type]);
      break;

    case ACCESS_WRITE_ACK:
      if (systemIsBigEndian()) swapEndiannessRaw(dataStoragePtr, TYPE_LENGTHS[dataMember.type]);

      if (dataMember.writeLock)
      {
        accessError = ERROR_WRITE_LOCK;
      }
      else if ((dataMember.limitsAsserted                            ) &&
               (checkLimits(dataMember, dataStoragePtr) != ERROR_NONE) )
      {
        accessError = ERROR_LIMITS;
      }
      else
      {
        memcpy(dataMember.data, dataStoragePtr, TYPE_LENGTHS[dataMember.type]);
      }
      break;

    default:
      accessError = ERROR_ACCESS_INVALID;
      recordError(accessError);
      break;
  }

  NodePlatform::releaseMemoryLock();

  return (accessError);
}

template <typename T>
Error_t Node::checkLimitsType(const DataMember_t    &dataMember,
                              const uint8_t * const  dataStoragePtr)
{
  volatile const T inputAsType    = *(reinterpret_cast<const T* const>(dataStoragePtr));
  volatile const T limitMaxAsType = *(reinterpret_cast<const T* const>(dataMember.limitMax));
  volatile const T limitMinAsType = *(reinterpret_cast<const T* const>(dataMember.limitMin));

  if ((inputAsType > limitMaxAsType) ||
      (inputAsType < limitMinAsType) )
  {
    return (ERROR_LIMITS);
  }

  return (ERROR_NONE);
}


/* Warning - Length checks are omitted and should be handled by calling function */
Error_t Node::checkLimits(const DataMember_t    &dataMember,
                          const uint8_t * const  dataStoragePtr)
{
  Error_t limitStatus = ERROR_NONE;

  switch (dataMember.type)
  {
    case TYPE_UINT8:
      if (checkLimitsType<uint8_t> (dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT8:
      if (checkLimitsType<int8_t>  (dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_UINT16:
      if (checkLimitsType<uint16_t>(dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT16:
      if (checkLimitsType<int16_t> (dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_UINT32:
      if (checkLimitsType<uint32_t>(dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_INT32:
      if (checkLimitsType<int32_t> (dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    case TYPE_FLOAT:
      if (checkLimitsType<float>   (dataMember, dataStoragePtr)) limitStatus = ERROR_LIMITS;
      break;
    default:
      limitStatus = ERROR_LIMITS;
      break;
  }

  return (limitStatus);
}


Error_t Node::recordError( Error_t error)
{
  if (error > NUMBER_OF_Atams_ERRORS) error = ERROR_ERROR_MANAGEMENT;

  _latestError = error;
  _errorCounts[error]++;

  return (error);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


