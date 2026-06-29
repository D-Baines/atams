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
  Atams::Error_t expectedError {Atams::ERROR_NONE};
  Atams::Error_t error         {Atams::ERROR_NONE};

  /* Node::setVar Var Access Error Checks - Read Only Var */
  expectedError = Atams::ERROR_ACCESS_INVALID;
  error         = Node::setVar(BlockTest1::VAR_READ_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::setVar Var ID Error Checks - Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::setVar((BlockTest3::VAR_WRITE_UINT32_20 + 1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::setVar Var Type Error Checks - Valid and Invalid Var Types */
  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    expectedError = (varID == BlockTest1::VAR_WRITE_UINT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testUint8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testInt8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testUint16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testInt16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testUint32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testInt32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_FLOAT) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::setVar(varID, testFloat_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);
  }

  /* Node::getVar Var Access Error Checks - Write Access Var */
  expectedError = Atams::ERROR_NONE;
  error         = Node::getVar(BlockTest1::VAR_WRITE_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::getVar Var Access Error Checks - Read Access Var */
  expectedError = Atams::ERROR_NONE;
  error         = Node::getVar(BlockTest1::VAR_READ_UINT8,  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::getVar Var ID Error Checks - Invalid Var ID */
  expectedError = Atams::ERROR_VAR_ID;
  error         = Node::getVar((BlockTest3::VAR_WRITE_UINT32_20+1U),  testUint8_); 
  if (error != expectedError) errorHandler(error, argTestErrorMessage_);

  /* Node::getVar Var Type Error Checks - Valid and Invalid Var Types */
  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    expectedError = (varID == BlockTest1::VAR_WRITE_UINT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testUint8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT8) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testInt8_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testUint16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT16) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testInt16_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_UINT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testUint32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_INT32) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testInt32_);
    if (error != expectedError) errorHandler(error, argTestErrorMessage_);

    expectedError = (varID == BlockTest1::VAR_WRITE_FLOAT) ? Atams::ERROR_NONE : Atams::ERROR_VAR_TYPE;
    error         = Node::getVar(varID, testFloat_);
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
  error         = Node::setRequestPattern((BlockTest3::VAR_WRITE_UINT32_20+1U), Atams::ACCESS_READ, Atams::REQUEST_STREAM);
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
  Node::clearAllRequestPatterns();

  /* Request Packet and Write List Duplicate Tests */
  expectedError = Atams::ERROR_NONE;

  for (uint8_t setCount {0U}; setCount <= Atams::WriteList::LIST_MAX_LENGTH; setCount++)
  {
    error = Node::setRequestPattern(BlockTest1::VAR_WRITE_UINT8, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM);

    if (Node::getRequestPacketLength() != Atams::HEADER_SIZE_HEADER + Atams::DATAGRAM_SIZE_HEADER + sizeof(uint8_t))
    {
      errorHandler(Atams::ERROR_NONE, "Request Packet Duplication Test Failure On Node ");
    }

    if (Node::getWriteListLength() != 1U)
    {
      errorHandler(Atams::ERROR_NONE, "Write List Duplication Test Failure On Node ");
    }

    if (error != expectedError) errorHandler(error, argTestErrorMessage_);
  }

  Node::clearAllRequestPatterns();

  /* Request Packet and Write List Overflow Test */
  uint16_t      expectedWriteListLength     {0U};
  uint16_t      expectedRequestPacketLength {Atams::HEADER_SIZE_HEADER};
  const uint8_t setVarDatagramLength        {Atams::DATAGRAM_SIZE_HEADER + sizeof(uint8_t)};
  
  for (uint16_t varID {BlockTest3::VAR_WRITE_UINT8_1}; varID <= BlockTest3::VAR_WRITE_UINT8_20; varID++)
  {
    if ((expectedRequestPacketLength + setVarDatagramLength) > Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING)
    {
      expectedError = Atams::ERROR_REQUEST_BUFFER_LENGTH;
    }
    else 
    {
      expectedError = Atams::ERROR_NONE;
      expectedWriteListLength++;
      expectedRequestPacketLength += setVarDatagramLength;
    }

    error = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM);

    if ((error                          != expectedError              ) ||
        (Node::getWriteListLength()     != expectedWriteListLength    ) ||
        (Node::getRequestPacketLength() != expectedRequestPacketLength) )
    {
      errorHandler(error, "Request Packet Overflow Test Failure on Node ");
    }
  }

  Node::clearAllRequestPatterns();

  /* Response Buffer Length Overflow Test (Read Requests) */
  uint16_t       expectedResponseLength     {Atams::HEADER_SIZE_HEADER};
  uint16_t       expectedReadPacketLength   {Atams::HEADER_SIZE_HEADER};
  const uint16_t nodeMaxPacketSize          {Node::getNodeMaxPacketSize()};
  const uint8_t  readResponseDatagramLength {Atams::DATAGRAM_SIZE_HEADER + sizeof(uint32_t)};

  for (uint16_t varID {BlockTest3::VAR_WRITE_UINT32_1}; varID <= BlockTest3::VAR_WRITE_UINT32_20; varID++)
  {
    if ((expectedResponseLength + readResponseDatagramLength) > nodeMaxPacketSize)
    {
      expectedError = Atams::ERROR_RESPONSE_BUFFER_LENGTH;
    }
    else
    {
      expectedError             = Atams::ERROR_NONE;
      expectedResponseLength   += readResponseDatagramLength;
      expectedReadPacketLength += Atams::DATAGRAM_SIZE_HEADER;
    }

    error = Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_STREAM);

    if ((error                             != expectedError           ) ||
        (Node::getExpectedResponseLength() != expectedResponseLength  ) ||
        (Node::getRequestPacketLength()    != expectedReadPacketLength) )
    {
      errorHandler(error, "Response Buffer Length Overflow Test Failure on Node ");
    }
  }

  Node::clearAllRequestPatterns();
}

void TestNode::runUpdateCycleTests(void)
{
  Atams::Error_t          error                 {Atams::ERROR_NONE};
  Atams::Access_t         currentAccess         {Atams::ACCESS_NONE};
  Atams::RequestPattern_t currentRequestPattern {Atams::REQUEST_INACTIVE};
  uint8_t                 varLength             {0U};

  updateErrorInjection();

  if (expectedBusError_ != Atams::ERROR_NONE)
  {
    return; /* Early Return - Error has been injected */
  }

  for (uint16_t varID {BlockTest1::VAR_WRITE_UINT8}; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
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
        expectedResponseLength_      -= Atams::DATAGRAM_SIZE_HEADER;
      }
      else if (prevAccess_[varID] == Atams::ACCESS_READ)
      {
        expectedRequestPacketLength_ -= Atams::DATAGRAM_SIZE_HEADER;
        expectedResponseLength_      -= (Atams::DATAGRAM_SIZE_HEADER + varLength);
      }
    }
  }

  for (uint16_t varID {BlockTest1::VAR_WRITE_UINT8}; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    Atams::RequestPattern_t writeRequestPattern {static_cast<Atams::RequestPattern_t>(std::rand() % Atams::NUMBER_OF_REQUEST_PATTERNS)};

    error = Node::getVarLength(varID, varLength);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);

    if ((prevAccess_[varID]  == Atams::ACCESS_NONE     ) &&
        (writeRequestPattern != Atams::REQUEST_INACTIVE) )
    {
      switch (varID)
      {
        case BlockTest1::VAR_WRITE_UINT8:  updateWriteValue(varID, feedbackUint8_,  testUint8_);  break;
        case BlockTest1::VAR_WRITE_INT8:   updateWriteValue(varID, feedbackInt8_,   testInt8_);   break;
        case BlockTest1::VAR_WRITE_UINT16: updateWriteValue(varID, feedbackUint16_, testUint16_); break;
        case BlockTest1::VAR_WRITE_INT16:  updateWriteValue(varID, feedbackInt16_,  testInt16_);  break;
        case BlockTest1::VAR_WRITE_UINT32: updateWriteValue(varID, feedbackUint32_, testUint32_); break;
        case BlockTest1::VAR_WRITE_INT32:  updateWriteValue(varID, feedbackInt32_,  testInt32_);  break;
        case BlockTest1::VAR_WRITE_FLOAT:  updateWriteValue(varID, feedbackFloat_,  testFloat_);  break;

        default: errorHandler(Atams::ERROR_VAR_ID, "Unexpected default case on Node "); break;
      }

      /* Check acknowledgement flag is false before starting setVar */
      bool ackReceived {true};

      static_cast<void>(Node::isWriteAcked(varID, ackReceived));

      if (ackReceived == true) errorHandler(error, "Write Acknowledgement Not Cleared On Node ");

      error = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, writeRequestPattern);

      if (error) errorHandler(error, defaultUpdateErrorMessage_);

      expectedRequestPacketLength_ += Atams::DATAGRAM_SIZE_HEADER + varLength;
      expectedResponseLength_      += Atams::DATAGRAM_SIZE_HEADER;

      if (writeRequestPattern == Atams::REQUEST_STREAM) expectedWriteListLength_++;
    }
    else if (prevAccess_[varID] == Atams::ACCESS_WRITE)
    {
      bool ackReceived {false};

      static_cast<void>(Node::isWriteAcked(varID, ackReceived));

      if (ackReceived == false) errorHandler(Atams::ERROR_NONE, "Write Acknowledgement Not Received On Node ");

      static_cast<void>(Node::clearWriteAck(varID));

      bool newDataReady {true};

      static_cast<void>(Node::isDataReady(varID, newDataReady));

      if (newDataReady == true) errorHandler(Atams::ERROR_NONE, "Data Ready Flag Not Cleared On Node ");

      Atams::RequestPattern_t readRequestPattern = (std::rand() % 2 == 0) ? Atams::REQUEST_STREAM : Atams::REQUEST_UNTIL_ACK;

      Node::setRequestPattern(varID, Atams::ACCESS_READ, readRequestPattern);

      if (prevRequestPatterns_[varID] == Atams::REQUEST_STREAM)
      {
        expectedRequestPacketLength_ -= varLength;
        expectedResponseLength_      += varLength;
        expectedWriteListLength_--;
      }
      else if (prevRequestPatterns_[varID] == Atams::REQUEST_UNTIL_ACK)
      {
        expectedRequestPacketLength_ += Atams::DATAGRAM_SIZE_HEADER;
        expectedResponseLength_      += (Atams::DATAGRAM_SIZE_HEADER + varLength);
      }
      else                                                              
      {
        errorHandler(Atams::ERROR_NONE, defaultUpdateErrorMessage_);
      }
    }
    else if (prevAccess_[varID] == Atams::ACCESS_READ)
    {
      bool newDataReady {false};

      static_cast<void>(Node::isDataReady(varID, newDataReady));

      if (newDataReady == false) errorHandler(Atams::ERROR_NONE, "Data Ready Flag Not Set On Node ");

      static_cast<void>(Node::clearDataReady(varID));

      switch (varID)
      {
        case BlockTest1::VAR_WRITE_UINT8:  checkReadValue(varID, feedbackUint8_,  testUint8_);  break;
        case BlockTest1::VAR_WRITE_INT8:   checkReadValue(varID, feedbackInt8_,   testInt8_);   break;
        case BlockTest1::VAR_WRITE_UINT16: checkReadValue(varID, feedbackUint16_, testUint16_); break;
        case BlockTest1::VAR_WRITE_INT16:  checkReadValue(varID, feedbackInt16_,  testInt16_);  break;
        case BlockTest1::VAR_WRITE_UINT32: checkReadValue(varID, feedbackUint32_, testUint32_); break;
        case BlockTest1::VAR_WRITE_INT32:  checkReadValue(varID, feedbackInt32_,  testInt32_);  break;
        case BlockTest1::VAR_WRITE_FLOAT:  checkReadValue(varID, feedbackFloat_,  testFloat_);  break;
        
        default: errorHandler(Atams::ERROR_VAR_ID, "Unexpected default case on Node "); break;
      }

      error = Node::setRequestPattern(varID, Atams::ACCESS_NONE, Atams::REQUEST_INACTIVE);

      if (error != Atams::ERROR_NONE) errorHandler(error, defaultUpdateErrorMessage_);

      if (prevRequestPatterns_[varID] == Atams::REQUEST_STREAM)
      {
        expectedRequestPacketLength_ -= Atams::DATAGRAM_SIZE_HEADER;
        expectedResponseLength_      -= (Atams::DATAGRAM_SIZE_HEADER + varLength);
      }
    }

    if (expectedRequestPacketLength_ != Node::getRequestPacketLength())
    {
      errorHandler(Atams::ERROR_NONE, "Request Packet Length Mismatch on Node ");
    }

    if (expectedResponseLength_ != Node::getExpectedResponseLength())
    {
      errorHandler(Atams::ERROR_NONE, "Response Length Mismatch on Node ");
    }

    if (expectedWriteListLength_ != Node::getWriteListLength())
    {
      errorHandler(Atams::ERROR_NONE, "Write List Length Mismatch on Node ");
    }
  }

  for (uint16_t varID = BlockTest1::VAR_WRITE_UINT8; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
  {
    error = Node::getRequestPattern(varID, prevAccess_[varID], prevRequestPatterns_[varID]);

    if (error) errorHandler(error, defaultUpdateErrorMessage_);
  }
}

Atams::Error_t TestNode::getExpectedBusError(void)
{
  return (expectedBusError_);
}
 

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

void TestNode::updateErrorInjection(void)
{
  uint16_t varIDUsed {VAR_ID_NULL};

  if ((Node::getBusError()               != expectedBusError_    ) ||
      (Node::getAbortedResponseDetails() != expectedAbortDetails_) )
  {
    errorHandler(Node::getBusError(), "Unexpected Node Bus Error On Node ");
  }

  if (expectedBusError_ != Atams::ERROR_NONE)
  {
    if (expectedBusError_ == Atams::ERROR_ABORTED_RESPONSE) expectedBusError_ = expectedAbortDetails_.error;

    Node::clearInjectedBusError(expectedBusError_, BlockTest1::VAR_READ_UINT8);
    
    expectedBusError_     = Atams::ERROR_NONE;
    expectedAbortDetails_ = {Atams::VAR_ID_NULL, Atams::ERROR_NONE};

    if (Node::getRequestPacketLength() == Atams::HEADER_SIZE_HEADER)
    {
      for (uint16_t varID {BlockTest1::VAR_WRITE_UINT8}; varID <= BlockTest1::VAR_WRITE_FLOAT; varID++)
      {
        static_cast<void>(Node::clearWriteAck(varID));
        static_cast<void>(Node::clearDataReady(varID));
      }

      for (Atams::RequestPattern_t &pattern : prevRequestPatterns_) pattern = Atams::REQUEST_INACTIVE;
      for (Atams::Access_t         &access  : prevAccess_         ) access  = Atams::ACCESS_NONE;
      expectedRequestPacketLength_ = Atams::HEADER_SIZE_HEADER;
      expectedResponseLength_      = Atams::HEADER_SIZE_HEADER;
      expectedWriteListLength_     = 0U;
    }
  }

  if ((std::rand() % ERROR_INJECTION_PROBABILITY) == 0)
  {
    expectedBusError_ = errorsToInject_[errorInjectionIndex_];

    Node::injectBusError(expectedBusError_, BlockTest1::VAR_READ_UINT8, varIDUsed);

    if (expectedBusError_ != Atams::ERROR_CONFIGURATION_STATE_INACTIVE)
    {
      expectedAbortDetails_.varID = varIDUsed;
      expectedAbortDetails_.error = expectedBusError_;
      expectedBusError_           = Atams::ERROR_ABORTED_RESPONSE;
    }

    errorInjectionIndex_++;

    if (errorInjectionIndex_ >= NUMBER_OF_ERRORS_TO_INJECT) errorInjectionIndex_ = 0U;
  }
}

void TestNode::errorHandler(const Atams::Error_t error, const char * errorMessage)
{
  static_cast<void>(snprintf(errorBuffer_, MAX_ERROR_MESSAGE_LENGTH, "%s%d", errorMessage, nodeID_));

  hubErrorHandler_(error, errorBuffer_);
}

template<typename T>
void TestNode::updateWriteValue(const uint16_t varID, T &feedbackVar, T &writeVar)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  error = Node::getVar(varID, feedbackVar);

  if (error) errorHandler(error, "Node::getVar error in TestNode::updateWriteValue On Node ");

  writeVar = static_cast<T>(std::rand());

  if (writeVar == feedbackVar) writeVar++;

  error = Node::setVar(varID, writeVar);

  if (error) errorHandler(error, "Node::setVar error in TestNode::updateWriteValue On Node ");
}

template<typename T>
void TestNode::checkReadValue(const uint16_t varID, T &feedbackVar, T &writtenVar)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  writtenVar++;

  error = Node::getVar(varID, feedbackVar);

  if (error) 
  {
    errorHandler(error, "Node::getVar error in TestNode::checkReadValue On Node ");
  }

  if (feedbackVar != writtenVar) 
  {
    errorHandler(Atams::ERROR_NONE, "Feedback Mismatch on Node ");
  }
}

} /* End Namespace - Atams */


/**
  * @}End of File
  */


