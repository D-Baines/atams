/**
  ******************************************************************************
  * @file    TestNode.hpp
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
#include "../../Hub/Node.hpp"
#include "MapTest/MapTest.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* TYPEDEFS                                                                          */
/*************************************************************************************/

/*************************************************************************************/
/* CLASS DEFINITIONS                                                                 */
/*************************************************************************************/

class TestNode :
public Node
{
  public:

  /*-- Public Constants -------------------------------------------------------------*/

  /*-- Public Typedefs --------------------------------------------------------------*/

  typedef void (&ErrorHandlerCallback_t)(Atams::Error_t error, const char * errorMessage);

  /*-- Public Function Declarations -------------------------------------------------*/

  /* Default Constructor */
  TestNode(void) = delete;

  /* Constructor */
  TestNode(const uint8_t nodeID, ErrorHandlerCallback_t errorHandler);

  /* Default Constructor */
  ~TestNode(void) = default;

  /* Copy Constructor */
  TestNode(const TestNode &other) = delete;

  /* Copy Assignment Operator */
  TestNode & operator=(TestNode &other) = delete;

  /* Move Constructor */
  TestNode(TestNode &&TestNode) = delete;

  /* Move Assignment Operator */
  TestNode & operator=(TestNode &&other) = delete;

  Atams::Error_t initMemoryMap(const Node::MemoryMap_t &memoryMap);

  void runFunctionArgTests(void);

  void runUpdateCycleTests(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static inline constexpr uint8_t MAX_ERROR_MESSAGE_LENGTH = 100U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Function References --------------------------------------------------*/

  ErrorHandlerCallback_t hubErrorHandler_;

  /*-- Private Variables ------------------------------------------------------------*/

  const uint8_t nodeID_;

  uint16_t expectedRequestPacketLength_ {0U};

  uint8_t  testUint8_  {0U};
  int8_t   testInt8_   {0};  
  uint16_t testUint16_ {0U};
  int16_t  testInt16_  {0};
  uint32_t testUint32_ {0U};
  int32_t  testInt32_  {0}; 
  float    testFloat_  {0.0F}; 

  uint8_t  feedbackUint8_  {0U};
  int8_t   feedbackInt8_   {0};  
  uint16_t feedbackUint16_ {0U};
  int16_t  feedbackInt16_  {0};
  uint32_t feedbackUint32_ {0U};
  int32_t  feedbackInt32_  {0}; 
  float    feedbackFloat_  {0.0F}; 

  Atams::RequestPattern_t prevRequestPatterns_[Atams::MapTest::BlockTest1::NUMBER_OF_VARS] = {Atams::REQUEST_INACTIVE};

  char errorBuffer_[MAX_ERROR_MESSAGE_LENGTH];

  const char * defaultUpdateErrorMessage_ {"Update Cycle Test Failure on Node "};
  const char * feedbackErrorMessage_      {"Feedback Mismatch on Node "};
  const char * argTestErrorMessage_       {"Node Function Argument Test Failure on Node "};

  /*-- Private Function Declarations ------------------------------------------------*/

  void errorHandler(const Atams::Error_t error, const char * errorMessage);
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */

