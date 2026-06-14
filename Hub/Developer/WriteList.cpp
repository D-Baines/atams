/**
  ******************************************************************************
  * @file    WriteList.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the WriteList write-stream tracking container.
  *
  * @details Implements the WriteList class. Provides ordered insertion, indexed
  *          lookup, configurable-index removal, and packet-index shift adjustment
  *          for tracking active write-stream datagrams in the Hub Node request packet.
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

void WriteList::reset(void)
{
  _configCount = 0U;
}

WriteList::SearchResult_t WriteList::findConfig(const uint16_t varID)
{
  SearchResult_t searchResult = 
  {
    /* .configFound = */ false,
    /* .configIndex = */ 0U
  };

  for (uint16_t listIndex {0U}; listIndex < _configCount; listIndex++)
  {
    if (varID == _configList[listIndex].varID) 
    {
      searchResult.configIndex = listIndex;
      searchResult.configFound = true;
      break;
    }
  }   

  return (searchResult);
}

WriteList::Error_t WriteList::addConfig(const WriteConfig_t newWriteConfig)
{
  if (_configCount >= LIST_MAX_LENGTH)
  {
    return (WriteList::ERROR_FULL);
  }

  _configList[_configCount] = newWriteConfig;
  _configCount++;
  
  return (WriteList::ERROR_NONE);
}

void WriteList::removeConfigIfFound(const uint16_t varID)
{
  SearchResult_t searchResult {findConfig(varID)};

  if (searchResult.configFound == true) removeConfigAtIndex(searchResult.configIndex);
}

void WriteList::updateIndexes(const uint16_t referenceIndex, const int16_t shiftLength)
{
  for (uint16_t index {0U}; index < _configCount; index++)
  {
    if (_configList[index].requestPacketIndex > referenceIndex)
    {
      _configList[index].requestPacketIndex += shiftLength;
    }
  }
}

WriteList::ConfigReturn_t WriteList::getConfigAtIndex(const uint16_t configIndex)
{ 
  ConfigReturn_t configReturn;

  if (configIndex >= _configCount)
  {
    configReturn.status = WriteList::ERROR_INDEX_OOR;
    return (configReturn);
  }

  configReturn.status      = WriteList::ERROR_NONE;
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

void WriteList::removeConfigAtIndex(const uint16_t configIndex)
{
  for (uint16_t index {configIndex}; index < (_configCount - 1U); index++)
  {
    _configList[index] = _configList[index + 1U];
  }

  _configCount--;
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */
