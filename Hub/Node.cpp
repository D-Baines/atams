/**
  ******************************************************************************
  * @file    Node.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams Hub Node class.
  *
  * @details Implements the Node class for the Atams Hub library. Manages per-Node
  *          variable storage, request packet construction and maintenance, response
  *          packet validation and processing, and all public API functions.
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

#include "Node.hpp"

#include <cstring>

#include "Platform.hpp"
#include "../Shared/Utilities/AtamsUtilities.hpp"
#include "../Shared/Maps/BlockUniversal.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief Construct a Node with the specified node ID.
 *
 * Initialises the Node instance with the given node ID. 
 * The Node Memory Map is left uninitialized until @ref Node::init is called.
 *
 * @param nodeID The unique identifier for this Node.
 */
Node::Node(const uint8_t nodeID) :
nodeID_(nodeID)
{

}

/**
 * @brief Initialises the Node with an Atams Memory Map.
 *
 * Validates and stores a reference to the provided Memory Map for variable access control. Initialises concurrency locks and
 * prepares the Node instance for use. If validation or lock initialisation fails, the Memory Map is invalidated.
 *
 * @param memoryMap Reference to a @c MemoryMap_t structure defining the Node's variable layout and properties.
 *
 * @retval @c ERROR_NONE                   Initialisation successful.
 * @retval @c ERROR_MEMORY_MAP             Memory Map validation failed (invalid structure, length, universal block, or checksum).
 * @retval @c ERROR_ATAMS_VERSION_MISMATCH Memory Map validation failed (Atams version mismatch).
 * @retval @c ERROR_PLATFORM               Failed to initialise concurrency locks.
 *
 * @note This function must be called before accessing variables or changing request patterns on the Node.
 */
Atams::Error_t Node::init(const MemoryMap_t &memoryMap)
{
  Atams::Error_t error {Atams::validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS)};

  if (error == Atams::ERROR_NONE)
  {
    memoryMap_     = &memoryMap;
    validVarCount_ = memoryMap.genInfo.noOfVars;
  }

  return (error);
}

/**
 * @brief Sets the Node ID for this instance.
 *
 * Updates the Node ID stored in this object.
 * This does not configure the Node ID on the physical device; use the Atams::Bus
 * set Node configuration process for that.
 *
 * @param nodeID The new Node ID to assign.
 */
void Node::setNodeID(const uint8_t nodeID)
{
  nodeID_ = nodeID;
}

/**
 * @brief Retrieves the Node ID for this instance.
 *
 * Returns the Node ID currently stored in this object. 
 * This may differ from the physical ID on the Node device if it has not been synchronised.
 *
 * @return The Node ID assigned to this instance.
 */
uint8_t Node::getNodeID(void)
{
  return (nodeID_);
}

/**
 * @brief Sets the value of a variable in the Node's internal variable storage.
 *
 * Writes a value to the specified variable in the Node's local storage. 
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 * This function does not communicate with any external or physical device; it only updates the value
 * currently held in this Node instance.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 * @param varID      The ID of the variable to set.
 * @param writeValue The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE           Variable successfully set.
 * @retval @c ERROR_VAR_ID         The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE       The data type for the specified variable ID does not match @c T.
 * @retval @c ERROR_ACCESS_INVALID Insufficient access permissions to write to the specified variable.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t Node::setVar(const uint16_t varID, const T writeValue)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_WRITE       > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();

  writeToVarStorage(writeValue, var.storage);

  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t Node::setVar<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template Atams::Error_t Node::setVar<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template Atams::Error_t Node::setVar<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template Atams::Error_t Node::setVar<int16_t >(const uint16_t varID, const int16_t  writeValue);
template Atams::Error_t Node::setVar<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template Atams::Error_t Node::setVar<int32_t >(const uint16_t varID, const int32_t  writeValue);
template Atams::Error_t Node::setVar<float   >(const uint16_t varID, const float    writeValue);

/**
 * @brief Retrieves the value of a variable from the Node's internal variable storage.
 *
 * Reads the value of the specified variable from the Node's local storage.
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 * This function does not communicate with any external or physical device; it only accesses the value 
 * currently held in this Node instance.
 *
 * @tparam T         The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 * @param varID      The ID of the variable to retrieve.
 * @param outputRef  Reference to a variable where the retrieved value will be stored.
 *
 * @retval @c ERROR_NONE           Variable successfully retrieved.
 * @retval @c ERROR_VAR_ID         The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE       The data type for the specified variable ID does not match @c T.
 * @retval @c ERROR_ACCESS_INVALID Insufficient access permissions to read the specified variable.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t Node::getVar(const uint16_t varID, T &outputRef)
{
  if (varID >= validVarCount_) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();

  readFromVarStorage(outputRef, var.storage);

  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

template Atams::Error_t Node::getVar<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template Atams::Error_t Node::getVar<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template Atams::Error_t Node::getVar<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template Atams::Error_t Node::getVar<int16_t >(const uint16_t varID, int16_t  &outputRef);
template Atams::Error_t Node::getVar<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template Atams::Error_t Node::getVar<int32_t >(const uint16_t varID, int32_t  &outputRef);
template Atams::Error_t Node::getVar<float   >(const uint16_t varID, float    &outputRef);

/**
 * @brief Sets the request pattern for a variable.
 *
 * Adds, removes, or replaces a read or write datagram for the specified variable in the Node's internal request packet, 
 * according to the requested access type and pattern. A datagram is only present in the request packet if required by the 
 * current configuration; it is not possible to have both a read and write datagram for the same variable at the same time.
 * The request packet is sent to the physical Node device during a Bus update cycle, allowing data to be written to or retrieved 
 * from multiple device variables at once. This function only updates the internal request packet; no communication with the 
 * physical device occurs until the next Bus update cycle.
 *
 * @param varID          Variable ID to configure.
 * @param accessRequest  Access type to request for the variable:
 *                       - @c ACCESS_NONE:  Remove any existing request for this variable.
 *                       - @c ACCESS_READ:  Add a read request for this variable. If a write request exists, it will be replaced.
 *                       - @c ACCESS_WRITE: Add a write request for this variable. If a read request exists, it will be replaced.
 * @param requestPattern Request pattern to set:
 *                       - @c REQUEST_INACTIVE:  Remove any existing request for this variable.
 *                       - @c REQUEST_UNTIL_ACK: Keep the request until a write acknowledgement or new read data is received.
 *                       - @c REQUEST_STREAM:    Keep the request until explicitly cleared or changed.
 *
 * @retval @c ERROR_NONE                    Success.
 * @retval @c ERROR_VAR_ID                  Invalid variable ID.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error - the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The function checks access permissions before updating the request packet.
 */
Atams::Error_t Node::setRequestPattern(const uint16_t                varID,
                                       const Atams::Access_t         accessRequest,
                                       const Atams::RequestPattern_t requestPattern)
{ 
  if (varID          >= validVarCount_)                    return (Atams::ERROR_VAR_ID);                  /* Early Return */
  if (requestPattern >= Atams::NUMBER_OF_REQUEST_PATTERNS) return (Atams::ERROR_REQUEST_PATTERN_INVALID); /* Early Return */
  
  Node::Var_t     &var     {varStorage_[varID]};
  const HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  if (accessRequest > varInfo.externalAccess)
  {
    return (Atams::ERROR_ACCESS_INVALID); /* Early Return */
  }

  requestPacketLock_.acquireLock();

  if ((accessRequest  == var.requestAccess ) &&
      (requestPattern == var.requestPattern) )
  {
    requestPacketLock_.releaseLock();
    return (Atams::ERROR_NONE); /* Early Return */
  }

  if (wouldExceedResponseBuffer(varInfo, accessRequest, requestPattern, var.requestAccess))
  {
    requestPacketLock_.releaseLock();
    return (Atams::ERROR_RESPONSE_BUFFER_LENGTH); /* Early Return */
  }

  Atams::Error_t statusReturn {processRequestPacketChange(varID, accessRequest, requestPattern)};
  
  if (statusReturn == Atams::ERROR_NONE)
  {
    var.requestAccess  = accessRequest;
    var.requestPattern = requestPattern;
  }

  requestPacketLock_.releaseLock();

  return (statusReturn);
}

/**
 * @brief Retrieves the active request pattern and access type for a variable.
 *
 * Returns the current request pattern and access type configured for the specified variable in this Node instance.
 *
 * @param varID          The ID of the variable to query.
 * @param accessRequest  Reference to store the current access type for the variable.
 * @param requestPattern Reference to store the current request pattern for the variable.
 *
 * @retval @c ERROR_NONE   Request pattern and access type successfully retrieved.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::getRequestPattern(const uint16_t           varID,
                                       Atams::Access_t         &accessRequest,
                                       Atams::RequestPattern_t &requestPattern)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  requestPacketLock_.acquireLock();
 
  accessRequest  = var.requestAccess;
  requestPattern = var.requestPattern;

  requestPacketLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Clears all active request patterns for variables in this Node instance.
 *
 * Resets the request pattern and access type for every variable in the Node's internal storage,
 * and resets the internal request packet. This removes all read and write requests from the packet.
 */
void Node::clearAllRequestPatterns(void)
{
  requestPacketLock_.acquireLock();

  resetRequestPacketNoLock();

  requestPacketLock_.releaseLock();
}

/**
 * @brief Retrieves the most recent Bus error detected for this Node instance.
 *
 * During an Atams::Bus update cycle, the physical Node device returns a response packet that is processed to update read values and
 * acknowledge write requests. The Bus update cycle will only report the first Node with an error, but this function allows the user to 
 * check for errors specific to this Node (such as decoding failures, aborted responses, or timeouts) after the cycle completes. 
 * The stored bus error is reset automatically at the start of each new bus update cycle.
 *
 * @return The first @c Atams::Error_t detected for this Node during the last Bus update cycle, or @c Atams::ERROR_NONE if no error
 *         has occurred.
 */
Atams::Error_t Node::getBusError(void)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  busErrorLock_.acquireLock();
  error = busError_;
  busErrorLock_.releaseLock();

  return (error);
}


/**
 * @brief Returns the maximum packet size limit for this Node's response packet.
 *
 * The maximum packet size is collected from the Node device during the Bus initialisation
 * cycle and is used during request packet configuration to ensure that the cumulative expected
 * response length does not exceed the Node's limit.
 *
 * @return The maximum response packet size in bytes.
 */
uint16_t Node::getNodeMaxPacketSize(void) const
{
  return (nodeMaxPacketSize_);
}

/**
 * @brief Sets a variable's value and configures a write-until-acknowledged request pattern.
 *
 * Writes the specified value to the variable in the Node's internal storage, then updates the request packet to repeatedly request a 
 * write operation for this variable until an acknowledgement is received from the physical Node device during a Bus update cycle.
 * This function only updates the internal state and request packet; communication with the physical device occurs during the next 
 * Bus update cycle.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 * @param varID      The ID of the variable to set.
 * @param writeValue The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE                  Operation successful.
 * @retval @c ERROR_VAR_ID                The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE              The data type for the specified variable ID does not match @c T.
 * @retval @c ERROR_ACCESS_INVALID        Insufficient access permissions to write to the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL  Internal packet error - the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL       Write list is full.
 * @retval @c ERROR_NULLPTR               Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t Node::setWriteUntilAck(const uint16_t varID, const T writeValue)
{
  Atams::Error_t error {Node::setVar(varID, writeValue)};

  if (error == Atams::ERROR_NONE) 
  {
    error = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_UNTIL_ACK);
  }

  return (error);
}

template Atams::Error_t Node::setWriteUntilAck<uint8_t >(const uint16_t varID, const uint8_t  writeValue);
template Atams::Error_t Node::setWriteUntilAck<int8_t  >(const uint16_t varID, const int8_t   writeValue);
template Atams::Error_t Node::setWriteUntilAck<uint16_t>(const uint16_t varID, const uint16_t writeValue);
template Atams::Error_t Node::setWriteUntilAck<int16_t >(const uint16_t varID, const int16_t  writeValue);
template Atams::Error_t Node::setWriteUntilAck<uint32_t>(const uint16_t varID, const uint32_t writeValue);
template Atams::Error_t Node::setWriteUntilAck<int32_t >(const uint16_t varID, const int32_t  writeValue);
template Atams::Error_t Node::setWriteUntilAck<float   >(const uint16_t varID, const float    writeValue);

/**
 * @brief Configures a read-until-acknowledged request pattern for a variable.
 *
 * Updates the request packet to repeatedly request a read operation for the specified variable until new data is received from the 
 * physical Node device during a Bus update cycle. This function only updates the internal request packet; communication with the 
 * physical device occurs during the next Bus update cycle.
 *
 * @param varID The ID of the variable to configure.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to read the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::setReadUntilAck(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_UNTIL_ACK));
}

/**
 * @brief Configures a write-stream request pattern for a variable.
 *
 * Updates the request packet to continuously request a write operation for the specified variable during each Bus update cycle, until 
 * the request pattern is explicitly cleared or changed. This function only updates the internal request packet; communication with the 
 * physical device occurs during the next Bus update cycle.
 *
 * @param varID The ID of the variable to configure.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to write to the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::setWriteStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_STREAM));
}

/**
 * @brief Configures a read-stream request pattern for a variable.
 *
 * Updates the request packet to continuously request a read operation for the specified variable during each Bus update cycle, until 
 * the request pattern is explicitly cleared or changed. This function only updates the internal request packet; communication with the 
 * physical device occurs during the next Bus update cycle.
 *
 * @param varID The ID of the variable to configure.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to read the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::setReadStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_READ, Atams::REQUEST_STREAM));
}

/**
 * @brief Stops any active stream or repeated request pattern for a variable.
 *
 * Updates the request packet to remove any existing read or write request for the specified variable, setting its access type to
 * @c ACCESS_NONE and its request pattern to @c REQUEST_INACTIVE. This function only updates the internal request packet; communication 
 * with the physical device occurs during the next Bus update cycle.
 *
 * @param varID The ID of the variable for which to stop streaming or repeated requests.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to read the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::stopStream(const uint16_t varID)
{
  return (Node::setRequestPattern(varID, Atams::ACCESS_NONE, Atams::REQUEST_INACTIVE));
}

/**
 * @brief Checks if new data is available for a variable in the Node's internal storage.
 *
 * Determines whether new data has been received and is ready to be read for the specified variable in this Node instance.
 * This function only checks the internal state and does not communicate with any external or physical device.
 *
 * @param varID        The ID of the variable to check.
 * @param newDataReady Reference to a boolean that will be set to @c true if new data is available, or @c false otherwise.
 *
 * @retval @c ERROR_NONE   Operation successful.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::isDataReady(const uint16_t varID, bool &newDataReady)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();
  newDataReady = var.newDataReady;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Clears the data-ready flag for a variable in the Node's internal storage.
 *
 * Resets the data-ready status for the specified variable in this Node instance, indicating that any new data has been acknowledged or 
 * processed. This function only updates the internal state and does not communicate with any external or physical device.
 *
 * @param varID The ID of the variable for which to clear the data-ready flag.
 *
 * @retval @c ERROR_NONE   Operation successful.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::clearDataReady(const uint16_t varID)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();
  var.newDataReady = false;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Checks if a write acknowledgement has been received for a variable.
 *
 * Determines whether a write operation for the specified variable has been acknowledged by the physical Node device during a Bus 
 * update cycle. This function only checks the internal state of this Node instance and does not communicate with any external or 
 * physical device.
 *
 * @param varID       The ID of the variable to check.
 * @param ackReceived Reference to a boolean that will be set to @c true if a write acknowledgement has been received, or @c false otherwise.
 *
 * @retval @c ERROR_NONE   Operation successful.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::isWriteAcked(const uint16_t varID, bool &ackReceived)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();
  ackReceived = var.ackReceived;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Clears the write acknowledgement flag for a variable in the Node's internal storage.
 *
 * Resets the write acknowledgement status for the specified variable in this Node instance, indicating that any previous write 
 * acknowledgement has been acknowledged or processed. This function only updates the internal state and does not communicate with 
 * any external or physical device.
 *
 * @param varID The ID of the variable for which to clear the write acknowledgement flag.
 *
 * @retval @c ERROR_NONE   Operation successful.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::clearWriteAck(const uint16_t varID)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var {varStorage_[varID]};

  varStorageLock_.acquireLock();
  var.ackReceived = false;
  varStorageLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Clears the data-ready flag and configures a read-stream request pattern for a variable.
 *
 * Resets the data-ready status for the specified variable in this Node instance, then updates the request packet to continuously 
 * request a read operation for this variable during each Bus update cycle, until the request pattern is explicitly cleared or changed.
 * This function only updates the internal state and request packet; communication with the physical device occurs during the next 
 * Bus update cycle.
 *
 * @param varID The ID of the variable to configure.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to read the specified variable.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::clearDataReadySetReadStream(const uint16_t varID)
{
  Atams::Error_t error {Node::clearDataReady(varID)};

  if (!error) error = Node::setReadStream(varID);

  return (error);
}

/**
 * @brief Stops any active request pattern for a variable and checks if new data is available.
 *
 * Removes any existing read or write request for the specified variable by setting its access type to @c ACCESS_NONE and its request 
 * pattern to @c REQUEST_INACTIVE. Then, checks whether new data has been received and is ready to be read for this variable in the 
 * Node's internal storage. This function only updates the internal request packet and checks the internal state; communication with 
 * the physical device occurs during the next Bus update cycle.
 *
 * @param varID        The ID of the variable to process.
 * @param newDataReady Reference to a boolean that will be set to @c true if new data is available, or @c false otherwise.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::stopStreamIsDataReady(const uint16_t varID, bool &newDataReady)
{
  Atams::Error_t error {Node::stopStream(varID)};

  if (!error) error = Node::isDataReady(varID, newDataReady);

  return (error);
}

/**
 * @brief Retrieves the value of a variable if new data is available.
 *
 * Checks whether new data has been received for the specified variable in the Node's internal storage. If new data is available, the 
 * value is read and the data-ready flag is cleared; otherwise, no data is read and an error is returned. This function only accesses 
 * the internal state and does not communicate with any external or physical device.
 *
 * @tparam T        The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 * @param varID     The ID of the variable to retrieve.
 * @param outputRef Reference to a variable where the retrieved value will be stored if new data is available.
 *
 * @retval @c ERROR_NONE               Variable successfully retrieved and data-ready flag cleared.
 * @retval @c ERROR_VAR_ID             The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE           The data type for the specified variable ID does not match @c T.
 * @retval @c ERROR_ACCESS_INVALID     Insufficient access permissions to read the specified variable.
 * @retval @c ERROR_NEW_DATA_NOT_READY No new data is available for the specified variable.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t Node::getVarIfDataReady(const uint16_t varID, T &outputRef)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t   &var   {varStorage_[varID]};
  Atams::Error_t error {Atams::ERROR_NONE};

  varStorageLock_.acquireLock();

  if (var.newDataReady) readFromVarStorage(outputRef, var.storage);
  else                  error = Atams::ERROR_NEW_DATA_NOT_READY;

  var.newDataReady = false;

  varStorageLock_.releaseLock();

  return (error);
}

template Atams::Error_t Node::getVarIfDataReady<uint8_t >(const uint16_t varID, uint8_t  &outputRef);
template Atams::Error_t Node::getVarIfDataReady<int8_t  >(const uint16_t varID, int8_t   &outputRef);
template Atams::Error_t Node::getVarIfDataReady<uint16_t>(const uint16_t varID, uint16_t &outputRef);
template Atams::Error_t Node::getVarIfDataReady<int16_t >(const uint16_t varID, int16_t  &outputRef);
template Atams::Error_t Node::getVarIfDataReady<uint32_t>(const uint16_t varID, uint32_t &outputRef);
template Atams::Error_t Node::getVarIfDataReady<int32_t >(const uint16_t varID, int32_t  &outputRef);
template Atams::Error_t Node::getVarIfDataReady<float   >(const uint16_t varID, float    &outputRef);

/**
 * @brief Stops any active request pattern for a variable and retrieves its value if new data is available.
 *
 * Removes any existing read or write request for the specified variable by setting its access type to @c ACCESS_NONE and its request 
 * pattern to @c REQUEST_INACTIVE. Then, checks whether new data has been received for this variable in the Node's internal storage. 
 * If new data is available, the value is read and the data-ready flag is cleared; otherwise, no data is read and an error is returned.
 * This function only updates the internal request packet and accesses the internal state; communication with the physical device 
 * occurs during the next Bus update cycle.
 *
 * @tparam T         The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 * @param varID      The ID of the variable to process.
 * @param readData   Reference to a variable where the retrieved value will be stored if new data is available.
 *
 * @retval @c ERROR_NONE                    Variable successfully retrieved and data-ready flag cleared.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 * @retval @c ERROR_NEW_DATA_NOT_READY      No new data is available for the specified variable.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template<typename T>
Atams::Error_t Node::stopStreamGetVarIfDataReady(const uint16_t varID, T &readData)
{
  Atams::Error_t error {Node::stopStream(varID)};

  if (!error) error = Node::getVarIfDataReady(varID, readData);

  return (error);
}

template Atams::Error_t Node::stopStreamGetVarIfDataReady<uint8_t >(const uint16_t varID, uint8_t  &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<int8_t  >(const uint16_t varID, int8_t   &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<uint16_t>(const uint16_t varID, uint16_t &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<int16_t >(const uint16_t varID, int16_t  &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<uint32_t>(const uint16_t varID, uint32_t &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<int32_t >(const uint16_t varID, int32_t  &readData);
template Atams::Error_t Node::stopStreamGetVarIfDataReady<float   >(const uint16_t varID, float    &readData);

/**
 * @brief Clears the write acknowledgement flag, sets a new value, and configures a write-stream request pattern for a variable.
 *
 * Resets the write acknowledgement status for the specified variable in this Node instance, writes the provided value to the
 * variable's internal storage, and updates the request packet to continuously request a write operation for this variable during each 
 * Bus update cycle, until the request pattern is explicitly cleared or changed. This function only updates the internal state and 
 * request packet; communication with the physical device occurs during the next Bus update cycle.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 * @param varID      The ID of the variable to configure.
 * @param writeData  The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE                The data type for the specified variable ID does not match @c T.
 * @retval @c ERROR_ACCESS_INVALID          Insufficient access permissions to write to the specified variable.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template<typename T>
Atams::Error_t Node::clearAckSetWriteStream(const uint16_t varID, const T writeData)
{
  Atams::Error_t error {Node::clearWriteAck(varID)};

  if (!error) error = Node::setVar(varID, writeData);
  if (!error) error = Node::setWriteStream(varID);

  return (error);
}

template Atams::Error_t Node::clearAckSetWriteStream<uint8_t >(const uint16_t varID, const uint8_t  writeData);
template Atams::Error_t Node::clearAckSetWriteStream<int8_t  >(const uint16_t varID, const int8_t   writeData);
template Atams::Error_t Node::clearAckSetWriteStream<uint16_t>(const uint16_t varID, const uint16_t writeData);
template Atams::Error_t Node::clearAckSetWriteStream<int16_t >(const uint16_t varID, const int16_t  writeData);
template Atams::Error_t Node::clearAckSetWriteStream<uint32_t>(const uint16_t varID, const uint32_t writeData);
template Atams::Error_t Node::clearAckSetWriteStream<int32_t >(const uint16_t varID, const int32_t  writeData);
template Atams::Error_t Node::clearAckSetWriteStream<float   >(const uint16_t varID, const float    writeData);

/**
 * @brief Stops any active request pattern for a variable and checks if a write acknowledgement has been received.
 *
 * Removes any existing read or write request for the specified variable by setting its access type to @c ACCESS_NONE and its request 
 * pattern to @c REQUEST_INACTIVE. Then, checks whether a write operation for this variable has been acknowledged by the physical Node 
 * device during a Bus update cycle. This function only updates the internal request packet and checks the internal state; communication 
 * with the physical device occurs during the next Bus update cycle.
 *
 * @param varID       The ID of the variable to process.
 * @param ackReceived Reference to a boolean that will be set to @c true if a write acknowledgement has been received, or @c false otherwise.
 *
 * @retval @c ERROR_NONE                    Operation successful.
 * @retval @c ERROR_VAR_ID                  The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_REQUEST_PATTERN_INVALID Invalid request pattern.
 * @retval @c ERROR_REQUEST_PACKET_FATAL    Internal packet error – the request packet is corrupt and all request patterns have been reset.
 * @retval @c ERROR_REQUEST_BUFFER_LENGTH   Request packet buffer is full. Updated size would exceed @c Atams::Platform::MAX_BUS_PACKET_SIZE.
 * @retval @c ERROR_WRITE_LIST_FULL         Write list is full.
 * @retval @c ERROR_VAR_TYPE                Variable type mismatch.
 * @retval @c ERROR_NULLPTR                 Null pointer encountered.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::stopStreamGetWriteAck(const uint16_t varID, bool &ackReceived)
{
  Atams::Error_t error {Node::stopStream(varID)};

  if (!error) error = Node::isWriteAcked(varID, ackReceived);

  return (error);
}

/**
 * @brief Retrieves the storage length (in bytes) for a variable.
 *
 * Returns the number of bytes required to store the specified variable, as defined in the initialised Memory Map.
 * This function only accesses the internal state of the Node instance and does not communicate with any external or physical device.
 *
 * @param varID  The ID of the variable to query.
 * @param length Reference to a variable where the storage length (in bytes) will be stored.
 *
 * @retval @c ERROR_NONE   Operation successful.
 * @retval @c ERROR_VAR_ID The specified variable ID is out of range for the initialised Memory Map.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
Atams::Error_t Node::getVarLength(const uint16_t varID, uint8_t &length)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  length = TYPE_LENGTHS[varInfo.type];

  return (Atams::ERROR_NONE);
}

/**
 * @brief Retrieves the current length of the Node's request packet.
 *
 * Returns the length, in bytes, of the internal request packet that will be sent to the physical Node device during the next Bus 
 * update cycle. This function only accesses the internal state of the Node instance and does not communicate with any external or 
 * physical device.
 *
 * @return The length of the request packet, in bytes.
 */
uint16_t Node::getRequestPacketLength(void)
{
  requestPacketLock_.acquireLock();

  uint16_t requestPacketLength {requestPacket_.length};

  requestPacketLock_.releaseLock();

  return (requestPacketLength);
}

/**
 * @brief Retrieves the expected length of the response packet from the Node device.
 *
 * Returns the length, in bytes, of the response packet that the Node device is expected to send back following a request during the
 * Bus update cycle. This value is determined by all active requests configured for the Node. This function only accesses the internal
 * state of the Node instance and does not communicate with any external or physical device.
 *
 * @return The expected length of the response packet, in bytes.
 */
uint16_t Node::getExpectedResponseLength(void)
{
  requestPacketLock_.acquireLock();

  uint16_t expectedResponseLength {expectedResponseLength_};

  requestPacketLock_.releaseLock();

  return (expectedResponseLength);
}

/**
 * @brief Retrieves the number of active write requests in the Node's request packet.
 *
 * Returns the current length of the internal write list, which tracks all variables with active write stream requests in the Node's 
 * request packet. At the start of each Bus update cycle, the write list is used to efficiently transfer variable values from the 
 * Node's internal storage into the appropriate locations within the request packet. This function only accesses the internal state 
 * of the Node instance and does not communicate with any external or physical device.
 *
 * @return The number of active write requests in the write list.
 */
uint16_t Node::getWriteListLength(void)
{
  requestPacketLock_.acquireLock();

  uint16_t writeListLength {requestPacket_.writeList.getConfigCount()};

  requestPacketLock_.releaseLock();

  return (writeListLength);
}

/**
 * @brief Retrieves details of the most recent aborted response received from the Node device.
 *
 * If a Node device detects a problem with the incoming request packet - such as packet corruption or a scenario suggesting a mismatch 
 * between the Hub and Node Memory Maps - it will send an aborted response packet. This packet contains an @c Atams::Error_t code 
 * indicating the cause of the aborted response, and may also include a @c varID if the error was related to a specific variable 
 * access attempt. If the error was not associated with a particular variable, the @c varID will be set to @c Atams::VAR_ID_NULL.
 *
 * @return An @c AbortedResponseDetails_t structure containing the error code and variable ID associated with the most recent aborted 
 *         response received from the Node device.
 *
 * @note The details are updated each time an aborted response is received and are specific to this Node instance.
 */
Atams::AbortedResponseDetails_t Node::getAbortedResponseDetails(void)
{
  busErrorLock_.acquireLock();

  Atams::AbortedResponseDetails_t abortedResponseDetails {abortedResponseDetails_};

  busErrorLock_.releaseLock();

  return (abortedResponseDetails);
}


/**
 * @brief Validates that the Node device's Memory Map matches the Hub's Memory Map.
 *
 * Reads the universal block variables from the Node instances internal variable storage — including the Atams version,
 * map generation timestamp, checksum, and number of variables — and compares them against the genInfo stored in
 * the initialised Memory Map. This allows the Hub to confirm that the physical Node device is running a Memory Map
 * that is identical to the one used by this Node instance.
 *
 * @retval @c true  The Node's genInfo matches the initialised Memory Map's genInfo, and the values are non-null.
 * @retval @c false The Memory Map is not initialised, the retrieved genInfo is null, or the genInfo does not match
 *                  the initialised Memory Map.
 *
 * @note This function does not complete the full genInfo validation process on its own. It only compares values already
 *       held in internal variable storage against the initialised Memory Map; it does not retrieve those values from the
 *       physical Node device. The full validation process — including collection of the Universal Block variables over the
 *       bus — is performed during the Bus initialisation process.
 */
Atams::Error_t Node::validateGenInfo(void)
{
  Atams::Error_t  error {Atams::ERROR_NONE};
  const GenInfo_t nullGenInfo;
  GenInfo_t       genInfo;

  if (getMemoryMapIsValid() == false)
  {
    error = Atams::ERROR_MEMORY_MAP;
  }
  else if ((getVar(BlockUniversal::VAR_ATAMS_VERSION_MAJOR, genInfo.atamsVersionMajor) != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_ATAMS_VERSION_MINOR, genInfo.atamsVersionMinor) != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_DAY,         genInfo.genDay)            != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_MONTH,       genInfo.genMonth)          != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_YEAR,        genInfo.genYear)           != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_HOUR,        genInfo.genHour)           != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_MINUTE,      genInfo.genMinute)         != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_GEN_SECOND,      genInfo.genSecond)         != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_CHECKSUM,        genInfo.genChecksum)       != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_MAP_NUMBER_OF_VARS,  genInfo.noOfVars)          != Atams::ERROR_NONE) )
  {
    error = Atams::ERROR_MEMORY_MAP;
  }
  else if ((genInfo == nullGenInfo        ) ||
           (genInfo != memoryMap_->genInfo) )
  {
    error = Atams::ERROR_GEN_INFO_MISMATCH;
  }

  return (error);
}

/**
 * @brief Validates that the Atams version reported by the Node device matches the local Memory Map.
 *
 * Called during the Bus initialisation cycle after the Atams version variables have been
 * read from the Node device. Compares the reported major and minor versions. Both values must match exactly.
 *
 * @retval @c ERROR_NONE                   Node device Atams version matches the local Memory Map.
 *
 * @retval @c ERROR_MEMORY_MAP             The Memory Map is not initialised, or the version
 *                                         variables could not be retrieved from the local 
 *                                         variable storage.
 *
 * @retval @c ERROR_ATAMS_VERSION_MISMATCH The major or minor version reported by the Node
 *                                         device does not match the local Memory Map.
 */
Atams::Error_t Node::validateAtamsVersion(void)
{
  Atams::Error_t error {Atams::ERROR_NONE};
  uint8_t        major {0U};
  uint8_t        minor {0U};

  if (getMemoryMapIsValid() == false)
  {
    error = Atams::ERROR_MEMORY_MAP;
  }
  else if ((getVar(BlockUniversal::VAR_ATAMS_VERSION_MAJOR, major) != Atams::ERROR_NONE) ||
           (getVar(BlockUniversal::VAR_ATAMS_VERSION_MINOR, minor) != Atams::ERROR_NONE) )
  {
    error = Atams::ERROR_MEMORY_MAP;
  }
  else if ((major != memoryMap_->genInfo.atamsVersionMajor) ||
           (minor != memoryMap_->genInfo.atamsVersionMinor) )
  {
    error = Atams::ERROR_ATAMS_VERSION_MISMATCH;
  }

  return (error);
}

#if (DEVELOPER_TOOLS == true)

void Node::injectBusError(const Atams::Error_t errorToInject, 
                          const uint16_t       readOnlyVarID,
                          uint16_t            &varIDUsed)
{
  DatagramHeader_t datagramHeader;

  switch (errorToInject)
  {
    case Atams::ERROR_VAR_ID:
      requestPacketLock_.acquireLock();
      if ((requestPacket_.length + Atams::DATAGRAM_SIZE_HEADER) < static_cast<uint16_t>(sizeof(requestPacket_.buffer)))
      {
        datagramHeader.varID   = Atams::VAR_ID_NULL;
        datagramHeader.command = Atams::ACCESS_READ;
        Atams::datagramHeaderToBuffer(datagramHeader, &requestPacket_.buffer[requestPacket_.length]);
        requestPacket_.length += Atams::DATAGRAM_SIZE_HEADER;
      }
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_ACCESS_INVALID:
      requestPacketLock_.acquireLock();
      static_cast<void>(processRequestPacketChange(readOnlyVarID, 
                                                   Atams::ACCESS_WRITE, 
                                                   Atams::REQUEST_STREAM));
      varIDUsed = readOnlyVarID;
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_REQUEST_BUFFER_LENGTH:
      requestPacketLock_.acquireLock();
      requestPacket_.length++;
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_RESPONSE_BUFFER_LENGTH:
    {
      requestPacketLock_.acquireLock();
      
      resetRequestPacketNoLock();

      uint8_t  varLength   {0U};
      uint16_t responseLen {Atams::PACKET_HEADER_SIZE};

      datagramHeader.command   = Atams::ACCESS_READ;
      datagramHeader.varID     = readOnlyVarID;
      varIDUsed                = Atams::VAR_ID_NULL;

      static_cast<void>(getVarLength(readOnlyVarID, varLength));

      while ((requestPacket_.length + Atams::DATAGRAM_SIZE_HEADER) < static_cast<uint16_t>(sizeof(requestPacket_.buffer)))
      {
        Atams::datagramHeaderToBuffer(datagramHeader, &requestPacket_.buffer[requestPacket_.length]);
        requestPacket_.length += Atams::DATAGRAM_SIZE_HEADER;
        responseLen           += static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + varLength);
        if (responseLen > nodeMaxPacketSize_) break;
      }

      requestPacketLock_.releaseLock();
      break;
    }
    case Atams::ERROR_CONFIGURATION_STATE_INACTIVE:
      static_cast<void>(setWriteStream(BlockUniversal::VAR_STORE_ALL));
    default:
      /* Do Nothing */
      break; 
  }
}

void Node::clearInjectedBusError(const Atams::Error_t errorToClear, const uint16_t readOnlyVarID)
{
  switch (errorToClear)
  {
    case Atams::ERROR_VAR_ID:
      requestPacketLock_.acquireLock();
      requestPacket_.length -= Atams::DATAGRAM_SIZE_HEADER;
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_ACCESS_INVALID:
      requestPacketLock_.acquireLock();
      static_cast<void>(processRequestPacketChange(readOnlyVarID, 
                                                   Atams::ACCESS_NONE, 
                                                   Atams::REQUEST_INACTIVE));
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_REQUEST_BUFFER_LENGTH:
      requestPacketLock_.acquireLock();
      requestPacket_.length--;
      requestPacketLock_.releaseLock();
      break;
    case Atams::ERROR_RESPONSE_BUFFER_LENGTH:
      clearAllRequestPatterns();
      break;
    case Atams::ERROR_CONFIGURATION_STATE_INACTIVE:
      static_cast<void>(stopStream(BlockUniversal::VAR_STORE_ALL));
    default:
      /* Do Nothing */
      break; 
  }
}

#endif /* DEVELOPER_TOOLS */

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

Atams::Error_t Node::externalTransfer(const Atams::Access_t accessRequest,
                                      const uint16_t        varID,
                                      uint8_t * const       bytesPtr,
                                      const uint8_t         length)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::HubVarInfo_t &varInfo {memoryMap_->varInfoList[varID]};

  if (TYPE_LENGTHS[varInfo.type] != length ) return (Atams::ERROR_VAR_TYPE); /* Early Return */
  if (bytesPtr                   == nullptr) return (Atams::ERROR_NULLPTR);  /* Early Return */

  Node::Var_t    &var        {varStorage_[varID]};
  Atams::Error_t accessError {Atams::ERROR_NONE};

  varStorageLock_.acquireLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(bytesPtr, var.storage, TYPE_LENGTHS[varInfo.type]);
      break;

    case ACCESS_WRITE:
      memcpy(var.storage, bytesPtr, TYPE_LENGTHS[varInfo.type]);
      var.newDataReady = true;
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  varStorageLock_.releaseLock();

  return (accessError);
}

void Node::resetVars(void)
{
  varStorageLock_.acquireLock();

  for (Var_t &var : varStorage_) memset(var.storage, 0U, sizeof(var.storage));

  varStorageLock_.releaseLock();
}

bool Node::getMemoryMapIsValid(void)
{
  return ((memoryMap_    != nullptr) &&
          (validVarCount_ > 0U     ) );
}

void Node::invalidateMemoryMap(void)
{
  validVarCount_ = 0U;
  memoryMap_     = nullptr;
  resetVars();
}

void Node::processAbortedResponse(void)
{
  uint8_t  bufferVarIDHi {responseBuffer_[Atams::ABORT_INDEX_VAR_ID_HI]};
  uint8_t  bufferVarIDLo {responseBuffer_[Atams::ABORT_INDEX_VAR_ID_LO]};
  uint8_t  errorByte     {responseBuffer_[Atams::ABORT_INDEX_ERROR]};

  uint16_t varID = ((static_cast<uint16_t>(bufferVarIDHi & Atams::ABORT_MASK_VAR_ID_HI) << Atams::ABORT_SHIFT_VAR_ID_HI) |
                    (static_cast<uint16_t>(bufferVarIDLo & Atams::ABORT_MASK_VAR_ID_LO) << Atams::ABORT_SHIFT_VAR_ID_LO) );

  if ((responseLength_ != Atams::ABORT_PACKET_SIZE) ||
      (errorByte       >= NUMBER_OF_ATAMS_ERRORS  ) )
  {
    reportBusError(Atams::ERROR_ABORT_FAILURE);
  }
  else
  {
    reportAbortedResponse(varID, static_cast<Atams::Error_t>(errorByte));
  }
}

void Node::reportAbortedResponse(const uint16_t varID, const Atams::Error_t error)
{
  busErrorLock_.acquireLock();
  abortedResponseDetails_ = {varID, error};
  busErrorLock_.releaseLock();
  reportBusError(Atams::ERROR_ABORTED_RESPONSE);
}

/* Warning - no check of remaining response buffer length before copy, *
 * validateResponseBuffer must be called before using this function    */
bool Node::processDatagramRead(const DatagramHeader_t datagramHeader,
                               uint16_t              &datagramStartIndex,
                               const uint8_t          payloadLength)
{
  Atams::Error_t error {updateRequestPatternOnReceive(datagramHeader.varID)};

  if (error == Atams::ERROR_NONE) error = externalTransfer(Atams::ACCESS_WRITE,
                                                           datagramHeader.varID,
                                                           &responseBuffer_[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                                                           payloadLength); 

  if (error == Atams::ERROR_NONE) datagramStartIndex += (DATAGRAM_SIZE_HEADER + payloadLength);
  else                            reportBusError(error);

  return (error != Atams::ERROR_NONE);
}

bool Node::processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t error {updateRequestPatternOnReceive(datagramHeader.varID)};
  
  if (error == Atams::ERROR_NONE)
  {
    /* VarID validity guaranteed by validateResponseBuffer */
    varStorageLock_.acquireLock();
    varStorage_[datagramHeader.varID].ackReceived = true;
    varStorageLock_.releaseLock();
  }

  if (error == Atams::ERROR_NONE) datagramStartIndex += DATAGRAM_SIZE_HEADER;
  else                            reportBusError(error);
  
  return (error != Atams::ERROR_NONE);
}

bool Node::processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t error            {Atams::ERROR_NONE};
  bool           cancelProcessing {false};

  if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
  {
    error = updateRequestPatternOnReceive(datagramHeader.varID);

    if (error == Atams::ERROR_NONE)
    {
      error               = Atams::ERROR_CONFIGURATION_STATE_INACTIVE;
      datagramStartIndex += DATAGRAM_SIZE_HEADER;
    }
  }
  else 
  {
    cancelProcessing = true;
    error            = Atams::ERROR_INVALID_NACK;
  }

  if (error != Atams::ERROR_NONE) reportBusError(error);

  return (cancelProcessing);
}

Atams::Error_t Node::validateResponseBuffer(uint8_t * const responsePacket,
                                            const uint16_t  responsePacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex {HEADER_INDEX_FIRST_DATAGRAM};
  uint8_t          varLength          {0U};

  while (datagramStartIndex + DATAGRAM_SIZE_HEADER <= responsePacketLength)
  {
    bufferToDatagramHeader(&responsePacket[datagramStartIndex], datagramHeader);

    Atams::Error_t lengthStatus {getVarLength(datagramHeader.varID, varLength)};

    if (lengthStatus != Atams::ERROR_NONE)
    {
      return (lengthStatus); /* Early Return */
    }

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {
      case Atams::RESPONSE_ACK_READ:
        datagramStartIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength);
        break;
      case Atams::RESPONSE_ACK_WRITE:
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;
      case Atams::RESPONSE_NACK:
        datagramStartIndex += DATAGRAM_SIZE_HEADER;
        break;
      default:
        return (Atams::ERROR_ACCESS_INVALID); /* Early Return */
        break;
    }
  }

  if (datagramStartIndex != responsePacketLength)
  {
    return (Atams::ERROR_RESPONSE_BUFFER_LENGTH); /* Early Return */
  }

  return (Atams::ERROR_NONE);
}

DataStatusReturn_t<bool> Node::findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig)
{
  DataStatusReturn_t<bool> statusReturn;
  statusReturn.status = Atams::ERROR_NONE;
  statusReturn.data   = false;

  uint8_t varLength {0U};

  changeConfig.datagramStartIndex = HEADER_INDEX_FIRST_DATAGRAM;

  while (changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacket_.length)
  {
    bufferToDatagramHeader(&requestPacket_.buffer[changeConfig.datagramStartIndex], changeConfig.currentDatagramHeader);

    changeConfig.currentDatagramLength = DATAGRAM_SIZE_HEADER;

    if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE)
    {
      Atams::Error_t lengthStatus {getVarLength(changeConfig.currentDatagramHeader.varID, varLength)};

      if (lengthStatus != Atams::ERROR_NONE)
      {
        statusReturn.status = Atams::ERROR_REQUEST_PACKET_FATAL;
        return (statusReturn); /* Early Return */
      }

      changeConfig.currentDatagramLength += varLength;
    }

    if (changeConfig.currentDatagramHeader.varID == changeConfig.newDatagramHeader.varID)
    {
      statusReturn.data = true;
      return (statusReturn); /* Early Return */
    }
    else
    {
      changeConfig.datagramStartIndex += changeConfig.currentDatagramLength;
    }
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketShift(const uint16_t shiftIndex, const int16_t shiftLength)
{
  if (shiftIndex > requestPacket_.length)
  {
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  }

  const int32_t newLength {static_cast<int32_t>(requestPacket_.length) + shiftLength};

  if (newLength < static_cast<int32_t>(Atams::PACKET_HEADER_SIZE))
  {
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  }

  if (newLength > static_cast<int32_t>(sizeof(requestPacket_.buffer)))
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  memmove(&requestPacket_.buffer[shiftIndex + shiftLength],
          &requestPacket_.buffer[shiftIndex],
          (requestPacket_.length - shiftIndex));

  requestPacket_.length = static_cast<uint16_t>(newLength);

  requestPacket_.writeList.updateIndexes(shiftIndex, shiftLength);

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  uint16_t shiftIndex  {static_cast<uint16_t>(changeConfig.datagramStartIndex + changeConfig.currentDatagramLength)};
  int16_t  shiftLength {static_cast<int16_t>(-changeConfig.currentDatagramLength)};
    
  Atams::Error_t error {Atams::ERROR_NONE};
  
  if (requestPacketShift(shiftIndex, shiftLength) != Atams::ERROR_NONE)
  {
    error = Atams::ERROR_REQUEST_PACKET_FATAL;
  }
  else
  {
    if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE)
    {
      requestPacket_.writeList.removeConfigIfFound(changeConfig.currentDatagramHeader.varID);
      expectedResponseLength_ -= Atams::DATAGRAM_SIZE_HEADER;
    }
    else if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_READ)
    {
      expectedResponseLength_ -= static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + changeConfig.writePayloadLength);
    }
  }

  return (error);
}

Atams::Error_t Node::requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t  error       {Atams::ERROR_NONE};
  uint16_t shiftIndex  {static_cast<uint16_t>(changeConfig.datagramStartIndex + changeConfig.currentDatagramLength)};
  int16_t  shiftLength {static_cast<int16_t> (changeConfig.newDatagramLength  - changeConfig.currentDatagramLength)};

  bool writeListAdditionRequired {((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
                                   (changeConfig.requestPattern == Atams::REQUEST_STREAM) )};

  if (writeListAdditionRequired)
  {
    WriteList::WriteConfig_t writeConfig;

    writeConfig.varID              = changeConfig.newDatagramHeader.varID;
    writeConfig.requestPacketIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER);
    writeConfig.dataLength         = static_cast<uint8_t> (changeConfig.writePayloadLength);

    if (requestPacket_.writeList.addConfig(writeConfig) != WriteList::ERROR_NONE)
    {
      error = Atams::ERROR_WRITE_LIST_FULL;
    }
  }
  else
  {
    /* If requestPacketShift subsequently fails, this removal is not rolled back. This is safe   *
     * because an existing write list entry implies the current datagram is WRITE+STREAM, and   *
     * transitioning away from it always produces a shrinking or zero shift. A shrinking shift  *
     * can only fail with ERROR_REQUEST_PACKET_FATAL, which resets all state upstream.          */
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  if ((error        == Atams::ERROR_NONE) &&
      (shiftLength  != 0                ) )
  { 
    error = requestPacketShift(shiftIndex, shiftLength);
  }

  if (error == Atams::ERROR_NONE)
  {
    /* Copy new datagram into available space */
    memcpy(&requestPacket_.buffer[changeConfig.datagramStartIndex],
           changeConfig.newDatagramBuffer,
           changeConfig.newDatagramLength);

    const uint16_t oldResponseSize {(changeConfig.currentDatagramHeader.command == Atams::ACCESS_READ)                   ? 
                                    static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + changeConfig.writePayloadLength) : 
                                    static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER)                                   };
                                    
    const uint16_t newResponseSize {(changeConfig.accessRequest == Atams::ACCESS_READ)                                   ? 
                                    static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + changeConfig.writePayloadLength) : 
                                    static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER)                                   };

    expectedResponseLength_ += newResponseSize;
    expectedResponseLength_ -= oldResponseSize;
  }
  else if (writeListAdditionRequired)
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  return (error);
}

Atams::Error_t Node::requestPacketAppendDatagram(RequestChangeConfig_t &changeConfig)
{
  if ((requestPacket_.length + changeConfig.newDatagramLength) > sizeof(requestPacket_.buffer))
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  changeConfig.datagramStartIndex = requestPacket_.length;

  if ((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
      (changeConfig.requestPattern == Atams::REQUEST_STREAM) )
  {
    WriteList::WriteConfig_t writeConfigToAdd;

    writeConfigToAdd.varID              = changeConfig.newDatagramHeader.varID;
    writeConfigToAdd.requestPacketIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER);
    writeConfigToAdd.dataLength         = static_cast<uint8_t> (changeConfig.writePayloadLength);

    if (requestPacket_.writeList.addConfig(writeConfigToAdd) != WriteList::ERROR_NONE)
    {
      return (Atams::ERROR_WRITE_LIST_FULL); /* Early Return */
    }
  }

  /* Copy new datagram into available space + update request packet length */
  memcpy(&requestPacket_.buffer[requestPacket_.length], changeConfig.newDatagramBuffer, changeConfig.newDatagramLength);
  requestPacket_.length += changeConfig.newDatagramLength;

  if (changeConfig.accessRequest == Atams::ACCESS_READ)
  {
    expectedResponseLength_ += static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + changeConfig.writePayloadLength);
  }
  else if (changeConfig.accessRequest == Atams::ACCESS_WRITE)
  {
    expectedResponseLength_ += Atams::DATAGRAM_SIZE_HEADER;
  }

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::constructDatagramBuffer(RequestChangeConfig_t &changeConfig)
{
  Error_t statusReturn {Atams::ERROR_NONE};

  datagramHeaderToBuffer(changeConfig.newDatagramHeader, changeConfig.newDatagramBuffer);

  changeConfig.newDatagramLength = Atams::DATAGRAM_SIZE_HEADER;

  if (changeConfig.accessRequest == Atams::ACCESS_WRITE)
  {
    if (externalTransfer(Atams::ACCESS_READ,
                         changeConfig.newDatagramHeader.varID,
                         &changeConfig.newDatagramBuffer[DATAGRAM_INDEX_PAYLOAD],
                         changeConfig.writePayloadLength))
    {
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
    else
    {
      changeConfig.newDatagramLength += changeConfig.writePayloadLength;
    }
  }

  return (statusReturn);
}

void Node::resetRequestPacketNoLock(void)
{
  requestPacket_.length        = Atams::PACKET_HEADER_SIZE;
  expectedResponseLength_      = Atams::PACKET_HEADER_SIZE;
  requestPacket_.writeList.reset();
  
  for (uint16_t varID {0U}; varID < validVarCount_; varID++)
  {
    Node::Var_t &var {varStorage_[varID]};

    var.requestAccess  = Atams::ACCESS_NONE;
    var.requestPattern = Atams::REQUEST_INACTIVE;
  }
}

Atams::Error_t Node::processRequestPacketChange(const uint16_t                varID,
                                                const Atams::Access_t         accessRequest,
                                                const Atams::RequestPattern_t requestPattern)
{
  if (varID >= validVarCount_)
  {
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }

  Atams::Error_t        error {Atams::ERROR_NONE};
  RequestChangeConfig_t packetChangeConfig;
  packetChangeConfig.accessRequest             = accessRequest;
  packetChangeConfig.requestPattern            = requestPattern;
  packetChangeConfig.writePayloadLength        = TYPE_LENGTHS[memoryMap_->varInfoList[varID].type];
  packetChangeConfig.newDatagramHeader.command = accessRequest;
  packetChangeConfig.newDatagramHeader.varID   = varID;

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramFoundInPacket {findDatagramMatchInPacket(packetChangeConfig)};

  if (datagramFoundInPacket.status != Atams::ERROR_NONE)
  {
    resetRequestPacketNoLock();
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  }

  if ((requestPattern == Atams::REQUEST_INACTIVE) ||
      (accessRequest  == Atams::ACCESS_NONE     ) )
  {
    if ((datagramFoundInPacket.data                             == true             ) &&
        (requestPacketRemoveCurrentDatagram(packetChangeConfig) != Atams::ERROR_NONE) )
    {
      error = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
  }
  else
  {
    if (constructDatagramBuffer(packetChangeConfig) != Atams::ERROR_NONE)
    {
      error = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
    else if (datagramFoundInPacket.data == true)
    {
      error = requestPacketAdjustCurrentDatagram(packetChangeConfig);
    }
    else
    {
      error = requestPacketAppendDatagram(packetChangeConfig);
    }
  }

  if (error == Atams::ERROR_REQUEST_PACKET_FATAL) resetRequestPacketNoLock();

  return (error);
}

/* Warning - varID must be pre-validated; all callers reach this via validateResponseBuffer */
Atams::Error_t Node::updateRequestPatternOnReceive(const uint16_t varID)
{
  requestPacketLock_.acquireLock();

  Node::Var_t &var {varStorage_[varID]};

  Atams::Error_t          statusReturn          {Atams::ERROR_NONE};
  Atams::RequestPattern_t currentRequestPattern {var.requestPattern};
  
  if (currentRequestPattern == Atams::REQUEST_UNTIL_ACK)
  {
    statusReturn = processRequestPacketChange(varID, ACCESS_NONE, REQUEST_INACTIVE);

    if (statusReturn == Atams::ERROR_NONE)
    {
      var.requestAccess  = ACCESS_NONE;
      var.requestPattern = REQUEST_INACTIVE;
    }
  }

  requestPacketLock_.releaseLock();

  return (statusReturn);
}

bool Node::wouldExceedResponseBuffer(const Atams::HubVarInfo_t      &varInfo,
                                     const Atams::Access_t          accessRequest,
                                     const Atams::RequestPattern_t  requestPattern,
                                     const Atams::Access_t          currentAccess) const
{
  if (nodeMaxPacketSize_ == 0U) 
  {
    return (false); /* Early Return */
  }

  const uint16_t varLength {TYPE_LENGTHS[varInfo.type]};

  const uint16_t currentContrib {(currentAccess == Atams::ACCESS_READ ) ? static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + varLength) :
                                 (currentAccess == Atams::ACCESS_WRITE) ? static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER)             :
                                                                          static_cast<uint16_t>(0U) };

  const uint16_t newContrib {(requestPattern == Atams::REQUEST_INACTIVE ) ? static_cast<uint16_t>(0U)                                      :
                             (accessRequest  == Atams::ACCESS_READ      ) ? static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER + varLength) :
                             (accessRequest  == Atams::ACCESS_WRITE     ) ? static_cast<uint16_t>(Atams::DATAGRAM_SIZE_HEADER)             :
                                                                            static_cast<uint16_t>(0U) };

  return ((newContrib                                                > currentContrib    ) &&
          ((expectedResponseLength_ + (newContrib - currentContrib)) > nodeMaxPacketSize_) );
}

Atams::Error_t Node::updateRequestPacketWriteData(void)
{
  requestPacketLock_.acquireLock();

  WriteList::ConfigReturn_t listReturn;

  uint16_t writeListLength {requestPacket_.writeList.getConfigCount()};

  for (uint16_t writeListIndex {0U}; writeListIndex < writeListLength; writeListIndex++)
  {
    listReturn = requestPacket_.writeList.getConfigAtIndex(writeListIndex);

    if (listReturn.status != WriteList::ERROR_NONE)
    {
      resetRequestPacketNoLock();
      requestPacketLock_.releaseLock();
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
    else if (externalTransfer(Atams::ACCESS_READ,
                              listReturn.writeConfig.varID,
                              &requestPacket_.buffer[listReturn.writeConfig.requestPacketIndex],
                              listReturn.writeConfig.dataLength) != Atams::ERROR_NONE)
    {
      resetRequestPacketNoLock();
      requestPacketLock_.releaseLock();
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
  }

  requestPacketLock_.releaseLock();

  return (Atams::ERROR_NONE);
}

void Node::setNodeMaxPacketSize(const uint16_t maxPacketSize)
{
  nodeMaxPacketSize_ = maxPacketSize;
}

void Node::reportBusError(Atams::Error_t busError)
{
  busErrorLock_.acquireLock();
  if (busError_ == Atams::ERROR_NONE) busError_ = busError;
  busErrorLock_.releaseLock();
}

void Node::clearBusError(void)
{
  busErrorLock_.acquireLock();
  busError_ = Atams::ERROR_NONE;
  busErrorLock_.releaseLock();
}

void Node::clearAbortDetails(void)
{
  busErrorLock_.acquireLock();
  abortedResponseDetails_ = {Atams::VAR_ID_NULL, Atams::ERROR_NONE};
  busErrorLock_.releaseLock();
}

void Node::responseReceived(uint8_t *inputBuffer, uint16_t inputLength)
{
  if ((inputBuffer != nullptr                ) &&
      (inputLength <= sizeof(responseBuffer_)) ) 
  {
    memcpy(responseBuffer_, inputBuffer, inputLength);
    responseLength_   = inputLength;
    newResponseReady_ = true;
  }
  else
  {
    reportBusError(Atams::ERROR_RESPONSE_BUFFER_LENGTH);
  }
}

void Node::processResponseBuffer(void)
{
  if (getBusError() != Atams::ERROR_NONE)
  {
    return; /* Early Return */
  }

  if (!newResponseReady_)
  {
    reportBusError(Atams::ERROR_NO_RESPONSE);
    return; /* Early Return */
  }

  newResponseReady_ = false;

  if ((responseBuffer_[HEADER_INDEX_MSG_TYPE] == Atams::MESSAGE_ABORT_RESPONSE       ) ||
      (responseBuffer_[HEADER_INDEX_MSG_TYPE] == Atams::MESSAGE_ABORT_RESPONSE_SYNCED) )
  {
    processAbortedResponse();
    return; /* Early Return */
  }

  Atams::Error_t packetStatus {validateResponseBuffer(responseBuffer_, responseLength_)};

  if (packetStatus != Atams::ERROR_NONE)
  {
    reportBusError(packetStatus);
    return; /* Early Return */
  }

  bool     cancelProcessing   {false};
  uint16_t datagramStartIndex {HEADER_INDEX_FIRST_DATAGRAM};

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= responseLength_) &&
         (cancelProcessing                          == false          ) )
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&responseBuffer_[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateResponseBuffer */
    uint8_t varLength {TYPE_LENGTHS[memoryMap_->varInfoList[datagramHeader.varID].type]};

    switch (static_cast<AccessResponse_t>(datagramHeader.command))
    {
      case RESPONSE_ACK_READ:
        cancelProcessing = processDatagramRead(datagramHeader,
                                               datagramStartIndex,
                                               varLength);
        break;
      case RESPONSE_ACK_WRITE:
        cancelProcessing = processDatagramWrite(datagramHeader, datagramStartIndex);
        break;
      case RESPONSE_NACK:
        cancelProcessing = processDatagramNack(datagramHeader, datagramStartIndex);
        break;
      default:
        reportBusError(Atams::ERROR_ACCESS_RESPONSE_INVALID);
        cancelProcessing = true;
        break;
    }
  }
}

/** @note @p outputBuffer must be at least @c Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING bytes — the same
 *        size as the internal @c requestPacket_.buffer. No bounds check is performed at runtime. */
void Node::getRequestPacket(const Atams::MessageType_t requestType,
                             const uint8_t             syncCount,
                             uint8_t * const           outputBuffer,
                             uint16_t                 &outputLength)
{
  requestPacketLock_.acquireLock();

  requestPacket_.buffer[HEADER_INDEX_MSG_TYPE] = requestType;
  requestPacket_.buffer[HEADER_INDEX_NODE_ID ] = nodeID_;
  requestPacket_.buffer[HEADER_INDEX_SYNC]     = syncCount;

  const uint16_t length {requestPacket_.length};

  memcpy(outputBuffer, requestPacket_.buffer, length);

  outputLength = length;

  requestPacketLock_.releaseLock();
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


