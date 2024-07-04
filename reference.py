#######################################################################################
## FILE WRITE                                                                        ##
#######################################################################################
      
header.write("""/**
  ******************************************************************************
  * @file    """ + title + """MemoryMap.hpp
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>
#include "../../Node.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace SMI { namespace """ + title + """MemoryMap
{


/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

/*-- Defaults -----------------------------------------------------------------------*/ \n\n""")

writeConstLists(defaults, "DEFAULT")

header.write("""\n/*-- Minimum Limits -----------------------------------------------------------------*/ \n\n""")

writeConstLists(minLimits, "MIN_LIMIT")

header.write("""\n/*-- Maximum Limits -----------------------------------------------------------------*/ \n\n""")

writeConstLists(maxLimits, "MAX_LIMIT")

header.write("""\n\n/*************************************************************************************/
/* PUBLIC TYPEDEFS                                                                   */
/*************************************************************************************/

/*-- Memory Map Data Field List -----------------------------------------------------*/
             
typedef enum: uint8_t
{
  FIELD_ID_UNIVERSAL = 0U,
  FIELD_ID_""" + title.upper() + """      = 1U,

  NUMBER_OF_DATAFIELDS

} DataFieldID_t;


/*-- Data Field Member Lists --------------------------------------------------------*/

typedef enum: uint16_t
{ \n""")

writeFieldEnum()

header.write("""\n  NUMBER_OF_""" + title.upper() + """_DATA_MEMBERS
} """ + title + """DataMemberID_t;


/*-- Custom Member Types ------------------------------------------------------------*/\n\n""")

iterator = 0

for memberID in memberIDs:
  customType = customTypes[iterator]
  if (customType != "-"):
    header.write(customType + "\n\n")
  iterator += 1
             
header.write("""
/*************************************************************************************/
/* CONSTANT GLOBALS                                                                  */
/*************************************************************************************/

extern const MemoryMap_t memoryMap;


/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

Error_t initDefaults(void);

Error_t assertLimits(void);


} } /* End Namespace - SMI::"""+ title + """MemoryMap */

/**
  * @}End of File
  */
""")

header.close()

source = open(fileNameSource,'w')

source.write("""/**
  ******************************************************************************
  * @file    """ + title + '''MemoryMap.cpp
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

#include "''' + title + """MemoryMap.hpp"
#include "UniversalMemoryMap.hpp"
#include "../SMINode.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace SMI { namespace """ + title + """MemoryMap
{


/*************************************************************************************/
/* PRIVATE CONSTANTS                                                                 */
/*************************************************************************************/

/* WARNING - DESIGNATED INITIALISERS ARE NOT PROPERLY SUPPORTED PRE C++20 
 * (Some compilers may provide support e.g. G++)            
 * Designated initialiser member names have been commented to avoid errors when 
 * compiling with earlier C++ versions. All array elements are initialised in order. 
 */

static const DataField_t _dataField""" + title + """ =
{
  .noOfDataMembers = NUMBER_OF_""" + title.upper() + """_DATA_MEMBERS,
  .dataMembers     =
  { \n""")

iterator = 0

for memberID in memberIDs:
  NVMOffset   = NVMOffsets[iterator]
  OTPOffset   = OTPOffsets[iterator]
  type        = types[iterator]
  accessLevel = accessLevels[iterator]

  NVMOffsetString = ""
  if (NVMOffset == "-"):
    NVMoffsetString = "NULL_NVM_OFFSET"
  else:
    NVMoffsetString = NVMOffset + "U"

  OTPOffsetString = ""
  if (OTPOffset == "-"):
    OTPOffsetString = "NULL_NVM_OFFSET"
  else:
    OTPOffsetString = OTPOffset + "U"

  typeString = "!!!INPUT ERROR!!!"
  match type:
    case "uint8_t":
      typeString = "TYPE_UINT8"
    case "int8_t":
      typeString = "TYPE_INT8"
    case "uint16_t":
      typeString = "TYPE_UINT16"
    case "int16_t":
      typeString = "TYPE_INT16"
    case "uint32_t":
      typeString = "TYPE_UINT32"
    case "int32_t":
      typeString = "TYPE_INT32"
    case "float":
      typeString = "TYPE_FLOAT"

  accessString = "!!!INPUT ERROR!!!"
  match accessLevel:
    case "RO":
      accessString = "ACCESS_READ_ACK"
    case "RW":
      accessString = "ACCESS_WRITE_ACK"

  source.write("""    [""" + memberID + """] =
    {
      /*.data           = */ {0U, 0U, 0U, 0U},
      /*.NVMOffset      = */ """ + NVMoffsetString + """,
      /*.OTPOffset      = */ """ + OTPOffsetString + """,
      /*.type           = */ """ + typeString + """,
      /*.externalAccess = */ """ + accessString + """,
      /*.limitsAsserted = */ false,
      /*.limitMax       = */ {0U, 0U, 0U, 0U},
      /*.limitMin       = */ {0U, 0U, 0U, 0U},
      /*.writeLock      = */ false,
    },\n""")
  
  iterator += 1

source.write("""  }
};
             
/*************************************************************************************/
/* CONSTANT GLOBALS                                                                  */
/*************************************************************************************/

const MemoryMap_t memoryMap =
{
  .nodeType       = NODE_TYPE_""" + title.upper() + """,
  .noOfDataFields = NUMBER_OF_DATAFIELDS,
  .dataFields     = 
  {
    [FIELD_ID_UNIVERSAL] = UniversalMemoryMap::dataFieldUniversal,
    [FIELD_ID_""" + title.upper() + """]      = _dataField""" + title + """,
  }
};


/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

Error_t initDefaults(void)
{
  Error_t transferStatus = ERROR_NONE;\n\n""")

iterator = 0

for memberID in memberIDs:
  default = defaults[iterator]
  if default != "-":
    defaultString = memberID.replace('MEMBER_ID', (" " + "DEFAULT"))
    source.write("""  if (transferStatus == ERROR_NONE) transferStatus = Node::internalWrite(FIELD_ID_""" + title.upper() + """,
                                                                         """ + memberID + """,
                                                                        """  + defaultString + """);\n\n""")
  iterator += 1

source.write("""  return (transferStatus);
}

Error_t assertLimits(void)
{
  Error_t transferStatus = ERROR_NONE;\n\n""")

iterator = 0

for memberID in memberIDs:
  minLimit = minLimits[iterator]
  maxLimit = maxLimits[iterator]
  minLimitString = memberID.replace('MEMBER_ID', (" " + "MIN_LIMIT"))
  maxLimitString = memberID.replace('MEMBER_ID', (" " + "MAX_LIMIT"))
  if ((minLimit != "-") and (maxLimit != "-")):
    source.write("""  if (transferStatus == ERROR_NONE) transferStatus = Node::assertLimits(FIELD_ID_""" + title.upper() + """,
                                                                        """ + memberID + """,
                                                                       """  + maxLimitString + """,
                                                                       """ + minLimitString + """);\n\n""")
  iterator += 1

source.write("""  return (transferStatus);
}

} } /* End Namespace - SMI::"""+ title + """MemoryMap */

/**
  * @}End of File
  */""")

source.close()