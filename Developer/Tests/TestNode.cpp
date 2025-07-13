/**
  ******************************************************************************
  * @file    TestNode.cpp
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

#include "TestNode.hpp"
#include <cstdio>

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

using namespace MapTest;

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

TestNode::TestNode(const uint8_t nodeID, ErrorHandlerCallback_t errorHandler) : 
Node(nodeID),
hubErrorHandler_(errorHandler),
nodeID_(nodeID)
{

}

Atams::Error_t TestNode::initMemoryMap(const Node::MemoryMap_t &memoryMap)
{
  return (Node::init(memoryMap));
}

void TestNode::runFunctionArgTests(void)
{
  Atams::Error_t expectedError = Atams::ERROR_NONE;
  Atams::Error_t error         = Atams::ERROR_NONE;

  /* Writing To Write Accessible Vars */
  expectedError = Atams::ERROR_NONE;
  error         = Node::write(BlockTest1::VAR_WRITE_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Writing To Read Only Vars  */
  expectedError = Atams::ERROR_ACCESS_INVALID;
  error         = Node::write(BlockTest1::VAR_READ_UINT8,  testInt8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Writing To Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::write((BlockTest1::NUMBER_OF_VARS+1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Writing w/ Incorrect Var Types */
  expectedError = Atams::ERROR_VAR_TYPE;
  /* uint8_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testInt32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT8, testFloat_); 
  /* int8_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testInt32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT8,  testFloat_); 
  /* uint16_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testInt32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT16,  testFloat_); 
  /* int16_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testInt32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT16,  testFloat_); 
  /* uint32_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testInt32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_UINT32,  testFloat_); 
  /* int32_t */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_INT32,  testFloat_); 
  /* float */
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testUint8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testInt8_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testUint16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testInt16_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testUint32_); 
  if (!error) error = Node::write(BlockTest1::VAR_WRITE_FLOAT,  testInt32_); 

  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Reading From Write Accessile Vars */
  expectedError = Atams::ERROR_NONE;
  error         = Node::read(BlockTest1::VAR_WRITE_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Reading From Read Only Vars  */
  expectedError = Atams::ERROR_NONE;
  error         = Node::read(BlockTest1::VAR_READ_UINT8,  testInt8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Reading From Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::read((BlockTest1::NUMBER_OF_VARS+1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Reading w/ Incorrect Var Types */
  expectedError = Atams::ERROR_VAR_TYPE;
  /* uint8_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testInt32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT8, testFloat_); 
  /* int8_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testInt32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT8,  testFloat_); 
  /* uint16_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testInt32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT16,  testFloat_); 
  /* int16_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testInt32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT16,  testFloat_); 
  /* uint32_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testInt32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_UINT32,  testFloat_); 
  /* int32_t */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_INT32,  testFloat_); 
  /* float */
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testUint8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testInt8_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testUint16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testInt16_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testUint32_); 
  if (!error) error = Node::read(BlockTest1::VAR_WRITE_FLOAT,  testInt32_); 

  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Write Request Pattern for Writeable Vars */
  expectedError = Atams::ERROR_NONE;
  error         = Node::setRequestPattern(BlockTest1::VAR_WRITE_UINT8, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Write Request Pattern for Read Only Vars */
  expectedError = Atams::ERROR_ACCESS_INVALID;
  error         = Node::setRequestPattern(BlockTest1::VAR_READ_UINT8, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Read Request Pattern for Writeable Vars */
  expectedError = Atams::ERROR_NONE;
  error         = Node::setRequestPattern(BlockTest1::VAR_WRITE_UINT8, Atams::ACCESS_READ, Atams::REQUEST_STREAM); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Read Request Pattern for Read Only Vars */
  expectedError = Atams::ERROR_NONE;
  error         = Node::setRequestPattern(BlockTest1::VAR_READ_UINT8, Atams::ACCESS_READ, Atams::REQUEST_STREAM); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Request Pattern w/ Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::setRequestPattern((BlockTest1::NUMBER_OF_VARS+1U), Atams::ACCESS_READ, Atams::REQUEST_STREAM);
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Request Pattern w/ Invalid Access */
  expectedError = Atams::ERROR_ACCESS_INVALID;
  error = Node::setRequestPattern(BlockTest1::VAR_READ_UINT8, static_cast<Atams::Access_t>(Atams::ACCESS_WRITE+1U), Atams::REQUEST_STREAM);
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Set Request Pattern w/ Invalid Request Pattern */
  expectedError = Atams::ERROR_REQUEST_PATTERN_INVALID;
  error = Node::setRequestPattern(BlockTest1::VAR_READ_UINT8, Atams::ACCESS_READ, static_cast<Atams::RequestPattern_t>(Atams::REQUEST_UNTIL_ACK+1U));
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Clear Request Pattern All Vars */
  Node::resetRequestPacket();
}

void TestNode::runUpdateCycleTests(void)
{
  Atams::Error_t          error                 = Atams::ERROR_NONE;
  Atams::Access_t         currentAccess         = Atams::ACCESS_NONE;
  Atams::RequestPattern_t currentRequestPattern = Atams::REQUEST_INACTIVE;
  uint8_t                 varLength             = 0U;

  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    error = Node::getVarLength(varID, varLength);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);

    error = Node::getRequestPattern(varID, currentAccess, currentRequestPattern);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);

    if ((prevRequestPatterns_[varID] == Atams::REQUEST_UNTIL_ACK) &&
        (currentRequestPattern       != Atams::REQUEST_INACTIVE ) )
    {
      errorHandler(Atams::ERROR_NONE, "Request Pattern Not Cleared On Node ");
    }
    
    if (currentAccess == Atams::ACCESS_NONE)
    {
      Atams::RequestPattern_t requestPattern = static_cast<Atams::RequestPattern_t>(std::rand() % Atams::NUMBER_OF_REQUEST_PATTERNS);
  
      if (requestPattern != Atams::REQUEST_INACTIVE)
      {
        switch (varID)
        {
          case BlockTest1::VAR_WRITE_UINT8:
            testUint8_     = static_cast<uint8_t>(std::rand());
            error          = Node::write(varID, testUint8_);
            break;
          case BlockTest1::VAR_WRITE_INT8:
            testInt8_      = static_cast<int8_t>(std::rand());
            error          = Node::write(varID, testInt8_);
            break;
          case BlockTest1::VAR_WRITE_UINT16:
            testUint16_ = static_cast<uint16_t>(std::rand());
            error       = Node::write(varID, testUint16_);
            break;
          case BlockTest1::VAR_WRITE_INT16:
            testInt16_ = static_cast<int16_t>(std::rand());
            error      = Node::write(varID, testInt16_);
            break;
          case BlockTest1::VAR_WRITE_UINT32:
            testUint32_ = static_cast<uint32_t>(std::rand());
            error       = Node::write(varID, testUint32_);
            break;
          case BlockTest1::VAR_WRITE_INT32:
            testInt32_ = static_cast<int32_t>(std::rand());
            error      = Node::write(varID, testInt32_);
            break;
          case BlockTest1::VAR_WRITE_FLOAT:
            testFloat_ = static_cast<float>(std::rand());
            error      = Node::write(varID, testFloat_);
            break;
        }

        if (error) errorHandler(error, defaultUpdateErrorMessage_);
  
        error = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, requestPattern);

        if (error) errorHandler(error, defaultUpdateErrorMessage_);

        expectedRequestPacketLength_ += Atams::DATAGRAM_SIZE_HEADER + varLength;
      }
    }
    else if (currentAccess == Atams::ACCESS_WRITE)
    {
      Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_UNTIL_ACK);

      expectedRequestPacketLength_ -= varLength;
    }
    else if (currentAccess == Atams::ACCESS_READ)
    {
      switch (varID)
      {
        case BlockTest1::VAR_WRITE_UINT8:
          error = Node::read(varID, feedbackUint8_);
          if (testUint8_ != feedbackUint8_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_INT8:
          error = Node::read(varID, feedbackInt8_);
          if (testInt8_ != feedbackInt8_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_UINT16:
          error = Node::read(varID, feedbackUint16_);
          if (testUint16_ != feedbackUint16_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_INT16:
          error = Node::read(varID, feedbackInt16_);
          if (testInt16_ != feedbackInt16_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_UINT32:
          error = Node::read(varID, feedbackUint32_);
          if (testUint32_ != feedbackUint32_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_INT32:
          error = Node::read(varID, feedbackInt32_);
          if (testInt32_ != feedbackInt32_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
        case BlockTest1::VAR_WRITE_FLOAT:
          error = Node::read(varID, feedbackFloat_);
          if (testFloat_ != feedbackFloat_) errorHandler(Atams::ERROR_NONE, feedbackErrorMessage_);
          break;
      }
    
      if (error != Atams::ERROR_NONE) errorHandler(error, defaultUpdateErrorMessage_);

      error = Node::setRequestPattern(varID, Atams::ACCESS_NONE, Atams::REQUEST_INACTIVE);

      if (error != Atams::ERROR_NONE) errorHandler(error, defaultUpdateErrorMessage_);

      expectedRequestPacketLength_ -= Atams::DATAGRAM_SIZE_HEADER;
    }

    if (expectedRequestPacketLength_ != Node::getRequestPacketLength())
    {
      errorHandler(Atams::ERROR_NONE, "Request Packet Length Mismatch on Node ");
    }

    prevRequestPatterns_[varID] = currentRequestPattern;
  }
}
 

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

  void TestNode::errorHandler(const Atams::Error_t error, const char * errorMessage)
  {
    static_cast<void>(snprintf(errorBuffer_, MAX_ERROR_MESSAGE_LENGTH, "%s%d", errorMessage, nodeID_));

    hubErrorHandler_(error, errorBuffer_);
  }


} /* End Namespace - Atams */


/**
  * @}End of File
  */


