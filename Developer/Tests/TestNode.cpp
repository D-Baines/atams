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

  /* Node::write Var Access Error Checks - Read Only Var */
  expectedError = Atams::ERROR_ACCESS_INVALID;
  error         = Node::write(BlockTest1::VAR_READ_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::write Var ID Error Checks - Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::write((BlockTest2::VAR_WRITE_FLOAT+1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::write Var Type Error Checks - Valid and Invalid Var Types */
  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    expectedError = (varID == BlockTest1::VAR_WRITE_UINT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testUint8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testInt8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testUint16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testInt16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testUint32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testInt32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_FLOAT) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::write(varID, testFloat_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);
  }

  /* Node::read Var Access Error Checks - Write Access Var */
  expectedError = Atams::ERROR_NONE;
  error         = Node::read(BlockTest1::VAR_WRITE_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::read Var Access Error Checks - Read Access Var */
  expectedError = Atams::ERROR_NONE;
  error         = Node::read(BlockTest1::VAR_READ_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::read Var ID Error Checks - Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::read((BlockTest2::VAR_WRITE_FLOAT+1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::read Var Type Error Checks - Valid and Invalid Var Types */
  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    expectedError = (varID == BlockTest1::VAR_WRITE_UINT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testUint8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testInt8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testUint16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testInt16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testUint32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testInt32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_FLOAT) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::read(varID, testFloat_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);
  }

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
  error         = Node::setRequestPattern((BlockTest2::VAR_WRITE_FLOAT+1U), Atams::ACCESS_READ, Atams::REQUEST_STREAM);
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
    if (prevRequestPatterns_[varID] == Atams::REQUEST_UNTIL_ACK)
    {
      error = Node::getVarLength(varID, varLength);

      if (error) errorHandler(error, defaultUpdateErrorMessage_);

      error = Node::getRequestPattern(varID, currentAccess, currentRequestPattern);

      if (error) errorHandler(error, defaultUpdateErrorMessage_);

      if ((currentRequestPattern != Atams::REQUEST_INACTIVE) ||
          (currentAccess         != Atams::ACCESS_NONE     ) )
      {
        errorHandler(Atams::ERROR_NONE, "Request Pattern Not Cleared On Node ");
      }
      else if (prevAccess_[varID] == Atams::ACCESS_WRITE) 
      {
        expectedRequestPacketLength_ -= (Atams::DATAGRAM_SIZE_HEADER + varLength);
      }
      else if (prevAccess_[varID] == Atams::ACCESS_READ) 
      {
        expectedRequestPacketLength_ -= Atams::DATAGRAM_SIZE_HEADER;
      }
    }
  }

  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    Atams::RequestPattern_t writeRequestPattern = static_cast<Atams::RequestPattern_t>(std::rand() % Atams::NUMBER_OF_REQUEST_PATTERNS);

    error = Node::getVarLength(varID, varLength);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);

    if (prevAccess_[varID] == Atams::ACCESS_NONE)
    {
      if (writeRequestPattern != Atams::REQUEST_INACTIVE)
      {
        switch (varID)
        {
          case BlockTest1::VAR_WRITE_UINT8:
            testUint8_ = static_cast<uint8_t>(std::rand());
            error      = Node::write(varID, testUint8_);
            break;
          case BlockTest1::VAR_WRITE_INT8:
            testInt8_ = static_cast<int8_t>(std::rand());
            error     = Node::write(varID, testInt8_);
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
  
        error = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, writeRequestPattern);

        if (error) errorHandler(error, defaultUpdateErrorMessage_);

        expectedRequestPacketLength_ += Atams::DATAGRAM_SIZE_HEADER + varLength;
      }
    }
    else if (prevAccess_[varID] == Atams::ACCESS_WRITE)
    {
      Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_UNTIL_ACK);

      if      (prevRequestPatterns_[varID] == Atams::REQUEST_STREAM)    expectedRequestPacketLength_ -= varLength;
      else if (prevRequestPatterns_[varID] == Atams::REQUEST_UNTIL_ACK) expectedRequestPacketLength_ += Atams::DATAGRAM_SIZE_HEADER;
      else                                                              errorHandler(Atams::ERROR_NONE, defaultUpdateErrorMessage_);
    }
    else if (prevAccess_[varID] == Atams::ACCESS_READ)
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
    }

    if (expectedRequestPacketLength_ != Node::getRequestPacketLength())
    {
      errorHandler(Atams::ERROR_NONE, "Request Packet Length Mismatch on Node ");
    }
  }

  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    error = Node::getRequestPattern(varID, prevAccess_[varID], prevRequestPatterns_[varID]);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);
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


