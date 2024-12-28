/**
  ******************************************************************************
  * @file    PlatformOptions.hpp
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
#include <mutex>
#include "../AtamsTypedefs.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams { namespace Platform {

/*************************************************************************************/
/* PUBLIC CLASSES                                                                    */
/*************************************************************************************/

class BusPeripheral 
{
  public:
  
  struct UserData_t
  {

  };

  BusPeripheral(UserData_t userData) :
  _userData(userData)
  {
  
  }

  bool startPeripheral(void)
  {
    return (false);
  }

  bool transmitReady(void)
  {
    return (true);
  }

  void transmit(uint8_t *buffer, uint16_t length)
  {
    /* Transmit Buffer */
  }
  
  private:

  const UserData_t _userData;

};

class MemoryLock
{
  public:

  Error_t init(void)
  {
    /* Init user lock mechanisam - not required when using std::mutex */
    return (ERROR_NONE);
  }

  void acquireLock(void)
  {
    _memoryLock.lock();
  }

  void releaseLock(void)
  {
    _memoryLock.unlock();
  }

  private:

  std::mutex _memoryLock;
};

class CommsLock
{
  public:

  Error_t init(void)
  {
    /* Init user lock mechanisam - not required when using std::mutex */
    return (ERROR_NONE);
  }

  void acquireLock(void)
  {
    _commsLock.lock();
  }

  void releaseLock(void)
  {
    _commsLock.unlock();
  }

  private:

  std::mutex _commsLock;
};

/*************************************************************************************/
/* PUBLIC CONSTANTS                                                                  */
/*************************************************************************************/

inline constexpr uint16_t NUMBER_OF_NODES_PER_BUS     = 10U;
inline constexpr uint16_t NODE_NUMBER_OF_DATA_MEMBERS = 200U; 
inline constexpr uint16_t NODE_NUMBER_OF_DATA_BLOCKS  = 2U; 
inline constexpr uint16_t COMMS_BUFFER_SIZE           = 512U;

/*************************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                                      */
/*************************************************************************************/

uint64_t getMillis(void);

} } /* End Atams Namespace */


/**
  * @}End of File
  */
