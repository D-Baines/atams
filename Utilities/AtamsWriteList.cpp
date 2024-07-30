/**
  ******************************************************************************
  * @file    AtamsWriteList.cpp
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

#include "../../Atams/Utilities/AtamsWriteList.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams 
{

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/


WriteList::WriteList(uint16_t maxConfigCount)
{
  _maxConfigCount = maxConfigCount;
}


WriteList::SearchResult_t WriteList::findConfig(MemberWriteConfig_t configToFind)
{
  SearchResult_t searchResult;

  for (uint16_t listIndex = 0U; listIndex < _configCount; listIndex++)
  {
    if ((configToFind.nodeID   == _configList[listIndex].nodeID  ) &&
        (configToFind.blockID  == _configList[listIndex].blockID ) &&
        (configToFind.memberID == _configList[listIndex].memberID) ) 
    {
      searchResult.configIndex = listIndex;
      searchResult.configFound = true;
      return (searchResult);
    }
  }   

  searchResult.configFound = false;

  return (searchResult);
}


WriteList::Error_t WriteList::addConfig(MemberWriteConfig_t newWriteConfig)
{
  if ((_configCount >= LIST_MAX_LENGTH) ||
      (_configCount >= _maxConfigCount ) )
  {
    return (ERROR_FULL);
  }

  _configList[_configCount] = newWriteConfig;
  _configCount++;
  
  return (ERROR_NONE);
}


void WriteList::removeConfig(MemberWriteConfig_t configToRemove)
{
  SearchResult_t searchResult = findConfig(configToRemove);

  if (searchResult.configFound == true)
  {
    removeConfigAtIndex(searchResult.configIndex);
  }
}


void WriteList::removeNode(uint8_t nodeID)
{
  for (uint16_t listIndex = 0U; listIndex < _configCount; listIndex++)
  {
    if (_configList[listIndex].nodeID == nodeID)
    {
      removeConfigAtIndex(listIndex);
    }
  }   
}


void WriteList::updateIndexes(uint16_t referenceIndex, int8_t shiftLength)
{
  for (MemberWriteConfig_t &writeConfig : _configList)
  {
    if (writeConfig.meshPacketDataIndex >= referenceIndex)
    {
      writeConfig.meshPacketDataIndex += shiftLength;
    }
  }   
}


WriteList::Return_t WriteList::getConfigAtIndex(uint16_t configIndex)
{ 
  Return_t configReturn;

  if (configIndex >= _configCount)
  {
    configReturn.status = ERROR_INDEX_OOR;
    return (configReturn);
  }

  configReturn.status      = ERROR_NONE;
  configReturn.writeConfig = _configList[configIndex];

  return (configReturn);
}


uint16_t WriteList::getConfigCount(void)
{
  return (_configCount);
}


/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/


void WriteList::removeConfigAtIndex(uint16_t configIndex)
{
  for (uint16_t index = configIndex; index < (_configCount - 1U); index++)
  {
    _configList[index] = _configList[index + 1U];
  }

  _configCount--;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
