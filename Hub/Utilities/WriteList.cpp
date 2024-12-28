/**
  ******************************************************************************
  * @file    WriteList.cpp
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

#include "WriteList.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams 
{

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

WriteList::WriteList(void)
{

}

WriteList::SearchResult_t WriteList::findConfig(WriteConfig_t configToFind)
{
  SearchResult_t searchResult;

  for (uint16_t listIndex = 0U; listIndex < _configCount; listIndex++)
  {
    if ((configToFind.blockID == _configList[listIndex].blockID) &&
        (configToFind.varID   == _configList[listIndex].varID  ) ) 
    {
      searchResult.configIndex = listIndex;
      searchResult.configFound = true;
      return (searchResult);
    }
  }   

  searchResult.configFound = false;

  return (searchResult);
}

WriteList::Error_t WriteList::addConfig(WriteConfig_t newWriteConfig)
{
  if (_configCount >= LIST_MAX_LENGTH)
  {
    return (ERROR_FULL);
  }

  _configList[_configCount] = newWriteConfig;
  _configCount++;
  
  return (ERROR_NONE);
}

void WriteList::removeConfigIfFound(WriteConfig_t configToRemove)
{
  SearchResult_t searchResult = findConfig(configToRemove);

  if (searchResult.configFound == true)
  {
    removeConfigAtIndex(searchResult.configIndex);
  }
}

void WriteList::updateIndexes(uint16_t referenceIndex, int16_t shiftLength)
{
  for (WriteConfig_t &writeConfig : _configList)
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
