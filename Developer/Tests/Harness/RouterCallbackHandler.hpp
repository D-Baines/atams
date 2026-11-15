/**
  ******************************************************************************
  * @file    RouterCallbackHandler.hpp
  *
  * @author  D. Baines
  *
  * @brief   Abstract interface for objects that receive events from the router.
  *
  * @details Declares the RouterCallbackHandler pure abstract base class. The harness
  *          Platform layers (and the router tests) inherit from it, and register
  *          themselves with Router::setCallbackHandler(). Router::update() then calls
  *          these virtual functions on the registered object to deliver received bytes
  *          and report transmit completion. Friend access is granted to Router.
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

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace AtamsTestHarness {

/*************************************************************************************/
/* FORWARD DECLARATIONS                                                              */
/*************************************************************************************/

class Router;

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class RouterCallbackHandler
{
  /*-- Friend Class Declarations ----------------------------------------------------*/

  friend class AtamsTestHarness::Router;

  public:

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  RouterCallbackHandler(void) = default;

  /* Default Destructor */
  virtual ~RouterCallbackHandler(void) = default;

  /* Copy Constructor */
  RouterCallbackHandler(const RouterCallbackHandler &other) = delete;

  /* Copy Assignment Operator */
  RouterCallbackHandler & operator=(const RouterCallbackHandler &other) = delete;

  /* Move Constructor */
  RouterCallbackHandler(RouterCallbackHandler &&other) = delete;

  /* Move Assignment Operator */
  RouterCallbackHandler & operator=(RouterCallbackHandler &&other) = delete;

  private:

  /*-- Private Pure Virtual Function Declarations -----------------------------------*/

  /* Called by Router::update() with bytes the device received. bytesPtr is only valid during the call; length is never 0 */
  virtual void onRouterReceive(const uint8_t *bytesPtr, const uint16_t length) = 0;

  /* Called by Router::update() once per transmission from the device, after it has been put on the bus */
  virtual void onRouterTransmitComplete(void) = 0;
};

} /* End Namespace - AtamsTestHarness */


/**
  * @}End of File
  */
