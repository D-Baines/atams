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

  Atams::Error_t getExpectedBusError(void);

  private:

  /*-- Private Constants ------------------------------------------------------------*/

  static inline constexpr uint8_t MAX_ERROR_MESSAGE_LENGTH    = 100U;
  static inline constexpr uint8_t ERROR_INJECTION_PROBABILITY = 20U;
  static inline constexpr uint8_t NUMBER_OF_ERRORS_TO_INJECT  = 5U;

  /*-- Private Typedefs -------------------------------------------------------------*/

  /*-- Private Function References --------------------------------------------------*/

  ErrorHandlerCallback_t hubErrorHandler_;

  /*-- Private Variables ------------------------------------------------------------*/

  const uint8_t nodeID_;

  Atams::Error_t                  expectedBusError_    {Atams::ERROR_NONE};
  Atams::AbortedResponseDetails_t expectedAbortDetails_ {Atams::VAR_ID_NULL, Atams::ERROR_NONE};

  uint16_t expectedRequestPacketLength_ {Atams::HEADER_SIZE_HEADER};
  uint16_t expectedWriteListLength_     {0U};

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

  uint8_t errorInjectionIndex_ {0U};

  Atams::Error_t errorsToInject_[NUMBER_OF_ERRORS_TO_INJECT] =
  {
    Atams::ERROR_VAR_ID,
    Atams::ERROR_ACCESS_INVALID,
    Atams::ERROR_REQUEST_BUFFER_LENGTH,
    Atams::ERROR_RESPONSE_BUFFER_LENGTH,
    Atams::ERROR_CONFIGURATION_STATE_INACTIVE,
  };


  Atams::Access_t         prevAccess_[Atams::MapTest::BlockTest1::NUMBER_OF_VARS]          = {Atams::ACCESS_NONE};
  Atams::RequestPattern_t prevRequestPatterns_[Atams::MapTest::BlockTest1::NUMBER_OF_VARS] = {Atams::REQUEST_INACTIVE};

  char errorBuffer_[MAX_ERROR_MESSAGE_LENGTH];

  const char * defaultUpdateErrorMessage_ {"Update Cycle Test Failure on Node "};
  const char * argTestErrorMessage_       {"Node Function Argument Test Failure on Node "};

  /*-- Private Function Declarations ------------------------------------------------*/

  void updateErrorInjection(void);

  void errorHandler(const Atams::Error_t error, const char * errorMessage);

  template<typename T>
  void updateWriteValue(const uint16_t varID, T &feedbackVar, T &writeVar);

  template<typename T>
  void checkReadValue(const uint16_t varID, T &feedbackVar, T &writtenVar);
};


} /* End Namespace - Atams */

/**
  * @}End of File
  */

