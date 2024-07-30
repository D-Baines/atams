/**
  ******************************************************************************
  * @file    Node.hpp
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

/* Pragma to prevent recursive inclusion --------------------------------------------*/
#pragma once

/*************************************************************************************/
/* INCLUDES                                                                          */
/*************************************************************************************/

#include <stdint.h>

#include "../AtamsTypedefs.hpp"
#include "Platform/NodePlatform.hpp"


/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {


/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/


/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

class Bus;


/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class   Node :
private NodePlatform
{
  /*-- Friend Declarations ----------------------------------------------------------*/

  friend class Bus;


  /*-- Public -----------------------------------------------------------------------*/

  public:

  /*-- PUBLIC CONSTANTS ---------------*/

  /*-- PUBLIC TYPEDEFS ----------------*/

  typedef Error_t (&InitDefaultsFunction_t)(Node &nodeToInit);

  typedef Error_t (&InitLimitsFunction_t  )(Node &nodeToInit);

  struct DataMember_t
  {
    volatile uint8_t    data[MAX_TYPE_SIZE];
    const    uint32_t   NVMOffset      = 0U;
    const    uint32_t   OTPOffset      = 0U;
    const    DataType_t type           = TYPE_NULL;
    const    Access_t   externalAccess = ACCESS_NONE_NACK;
    volatile bool       limitsAsserted;
    volatile uint8_t    limitMax[MAX_TYPE_SIZE];
    volatile uint8_t    limitMin[MAX_TYPE_SIZE];
    volatile bool       writeLock;
  };

  struct DataField_t
  {
    uint16_t     noOfDataMembers;
    DataMember_t dataMembers[NODE_NUMBER_OF_DATA_MEMBERS];
  };

  struct MemoryMap_t
  {
    const uint16_t               noOfDataFields;
    const InitDefaultsFunction_t initDefaults;
    const InitLimitsFunction_t   initLimits;
          DataField_t            dataFields[NODE_NUMBER_OF_DATA_FIELDS];

    MemoryMap_t(void) = delete;

    MemoryMap_t(const uint16_t                initNoOfDataFields,
                const DataField_t             (&initDataFields)[NODE_NUMBER_OF_DATA_FIELDS],
                const InitDefaultsFunction_t  initDefaultsRef,
                const InitLimitsFunction_t    initLimitsRef) :
    noOfDataFields(initNoOfDataFields),
    initDefaults  (initDefaultsRef   ),
    initLimits    (initLimitsRef     )

    {
      for (uint16_t index = 0U; index < noOfDataFields; index++)
      {
        dataFields[index] = initDataFields[index];
      }
    };

    MemoryMap_t(const MemoryMap_t &other) :
    noOfDataFields(other.noOfDataFields),
    initDefaults  (other.initDefaults  ),
    initLimits    (other.initLimits    )
    {
      for (uint16_t index = 0U; index < noOfDataFields; index++)
      {
        dataFields[index] = other.dataFields[index];
      }
    }

    MemoryMap_t& operator=(const MemoryMap_t &other) = delete;
  };

  /*-- PUBLIC FUNCTION DECLARATIONS ---*/

  Node(const MemoryMap_t &memoryMap, const Bus &bus, NodePlatform::UserData_t userData);

  Node(const Node &other)             = delete;

  Node & operator=(const Node &other) = delete;
     
  Error_t init(void);

  template <typename T>
  Error_t read(const uint8_t   blockID,
               const uint16_t  memberID,
                     T        &readData);

  template <typename T>
  Error_t write(const uint8_t   blockID,
                const uint16_t  memberID,
                const T        &writeData);

  template <typename T>
  Error_t assertLimits(const uint8_t  blockID,
                       const uint16_t memberID,
                       const T        limitMax,
                       const T        limitMin);

  Error_t setUpdatePattern(const uint8_t          blockID,
                           const uint16_t         memberID,
                           const CommandPattern_t updatePattern);


  /*-- PRIVATE -----------------------------------------------------------------------*/

  private:

  /*-- PRIVATE CONSTANTS --------------*/

  static constexpr uint8_t  ABORT_RESPONSE_SIZE      = MESH_SIZE_HEADER + NODE_SIZE_HEADER + DATAGRAM_SIZE_HEADER + sizeof(Error_t);
  static constexpr uint32_t DEFAULT_WATCHDOG_TIMEOUT = 500U;


  /*-- PRIVATE TYPEDEFS ---------------*/


  /*-- PRIVATE VARIABLES --------------*/

  MemoryMap_t  _memoryMap;
  const Bus   &_bus;
  uint8_t      _nodeID                            = 0x00U;
  Error_t      _latestError                       = ERROR_NONE;
  uint16_t     _errorCounts[NUMBER_OF_Atams_ERRORS] = {0U};


  /*-- PRIVATE FUNCTION DECLARATIONS --*/

  void storeReceiveBuffer(uint8_t* buffer, uint16_t length, Error_t &errorStatus);

  void processNewData(Error_t &errorStatus);

  void getTransmitBuffer(uint8_t* buffer, uint16_t length);

  DataStatusReturn_t<uint8_t> getMemberLength(const uint8_t  blockID,
                                              const uint16_t memberID);

  Error_t externalTransfer(const Access_t  accessRequest,
                           const uint8_t   blockID,
                           const uint16_t  memberID,
                           uint8_t * const dataStoragePtr,
                           const uint8_t   length);

  template <typename T>
  Error_t checkLimitsType(const DataMember_t    &dataMember,
                          const uint8_t * const  dataStoragePtr);

  Error_t checkLimits(const DataMember_t    &dataMember,
                      const uint8_t * const  dataStoragePtr);

  Error_t recordError(const Error_t error);

};

} /* End Namespace - Atams */

/**
  * @}End of File
  */

