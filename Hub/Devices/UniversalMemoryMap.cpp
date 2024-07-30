/**
  ******************************************************************************
  * @file    UniversalMemoryMap.cpp
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

#include "UniversalMemoryMap.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace UniversalMemoryMap
{


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

/* WARNING - DESIGNATED INITIALISERS ARE NOT PROPERLY SUPPORTED PRE C++20
 * (Some compilers may provide support e.g. G++)
 * Designated initialiser member names have been commented to avoid errors when
 * compiling with earlier C++ versions. All array elements are initialised in order.
 */

constexpr Node::DataField_t dataFieldUniversal =
{
  .noOfDataMembers = NUMBER_OF_UNIVERSAL_DATA_MEMBERS,
  .dataMembers     =
  {
    [MEMBER_ID_NODE_ID] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ 0U,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT8,
      /*.externalAccess = */ ACCESS_WRITE_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_NODE_TYPE] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT8,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_FIRMWARE_MAJOR] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT16,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_FIRMWARE_MINOR] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT16,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_HARDWARE_MAJOR] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT16,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_HARDWARE_MINOR] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT16,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_SERIAL_NUMBER] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ 0U,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_BOOT_MODE] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_WRITE_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_STORE_ALL] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_WRITE_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_RESTORE_FACTORY] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ NULL_NVM_OFFSET,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_WRITE_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_WATCHDOG_TIMEOUT] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ 0U,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_WRITE_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
    [MEMBER_ID_WATCHDOG_COUNT] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ 0U,
      /*.OTPOffset      = */ NULL_NVM_OFFSET,
      /*.type           = */ TYPE_UINT32,
      /*.externalAccess = */ ACCESS_READ_ACK,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },
  }
};

constexpr Node::DataField_t _dataFields[NODE_NUMBER_OF_DATA_FIELDS] =
{
  [BLOCK_ID_UNIVERSAL] = UniversalMemoryMap::dataFieldUniversal,
};


/*************************************************************************************/
/* CONSTANT GLOBALS                                                                  */
/*************************************************************************************/

const Node::MemoryMap_t memoryMap(NUMBER_OF_DATAFIELDS,
                                       _dataFields,
                                       initDefaults,
                                       initLimits);


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(Node &nodeToInit)
{
  Error_t transferStatus = ERROR_NONE;

  if (transferStatus == ERROR_NONE) transferStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                                      MEMBER_ID_WATCHDOG_TIMEOUT,
                                                                      DEFAULT_WATCHDOG_TIMEOUT);

  if (transferStatus == ERROR_NONE) transferStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,
                                                                      MEMBER_ID_WATCHDOG_COUNT,
                                                                      DEFAULT_WATCHDOG_COUNT);

  return (transferStatus);
}

Error_t initLimits(Node &nodeToInit)
{
  Error_t transferStatus = ERROR_NONE;

  if (transferStatus == ERROR_NONE) transferStatus = nodeToInit.assertLimits(BLOCK_ID_UNIVERSAL,
                                                                             MEMBER_ID_NODE_ID,
                                                                             MAX_LIMIT_NODE_ID,
                                                                             MIN_LIMIT_NODE_ID);

  if (transferStatus == ERROR_NONE) transferStatus = nodeToInit.assertLimits(BLOCK_ID_UNIVERSAL,
                                                                             MEMBER_ID_WATCHDOG_TIMEOUT,
                                                                             MAX_LIMIT_WATCHDOG_TIMEOUT,
                                                                             MIN_LIMIT_WATCHDOG_TIMEOUT);

  if (transferStatus == ERROR_NONE) transferStatus = nodeToInit.assertLimits(BLOCK_ID_UNIVERSAL,
                                                                             MEMBER_ID_WATCHDOG_COUNT,
                                                                             MAX_LIMIT_WATCHDOG_COUNT,
                                                                             MIN_LIMIT_WATCHDOG_COUNT);

  return (transferStatus);
}

} } /* End Namespace - Atams::UniversalMemoryMap */

/**
  * @}End of File
  */
