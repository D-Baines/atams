/**
  ******************************************************************************
  * @file    Node.cpp
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

#include "Platform.hpp"
#include <type_traits>
#include "Node.hpp"
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
  Atams::Error_t statusReturn = Atams::validateMemoryMap(memoryMap, Platform::NODE_NUMBER_OF_VARS);

  if (statusReturn == Atams::ERROR_NONE)
  {
    if ((varStorageLock_.init()   ) &&
        (requestPacketLock_.init()) &&
        (busErrorLock_.init()     ) )
    {
      memoryMap_     = &memoryMap;
      validVarCount_ = memoryMap.noOfVars;
    }
    else 
    {
      invalidateMemoryMap();
      statusReturn = ERROR_PLATFORM;
    }
  }

  return (statusReturn);
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
  if (varID >= validVarCount_) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_WRITE       > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();

  writeToVarStorage(writeValue, var);

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

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

  varStorageLock_.acquireLock();

  readFromVarStorage(outputRef, var);

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
Atams::Error_t Node::setRequestPattern(const uint16_t         varID,
                                       const Access_t         accessRequest,
                                       const RequestPattern_t requestPattern)
{ 
  if (varID          >= validVarCount_)                    return (Atams::ERROR_VAR_ID);                  /* Early Return */
  if (requestPattern >= Atams::NUMBER_OF_REQUEST_PATTERNS) return (Atams::ERROR_REQUEST_PATTERN_INVALID); /* Early Return */
  
  Node::Var_t     &var     = varStorage_[varID];
  const VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

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

  Atams::Error_t statusReturn = processRequestPacketChange(varID, accessRequest, requestPattern);
  
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
Atams::Error_t Node::getRequestPattern(const uint16_t   varID,
                                       Access_t         &accessRequest,
                                       RequestPattern_t &requestPattern)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  Node::Var_t &var = varStorage_[varID];

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
  Atams::Error_t errorReturn = Atams::ERROR_NONE;

  busErrorLock_.acquireLock();
  errorReturn = busError_;
  busErrorLock_.releaseLock();

  return (errorReturn);
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
  Atams::Error_t statusReturn = Node::setVar(varID, writeValue);

  if (statusReturn == Atams::ERROR_NONE) 
  {
    statusReturn = Node::setRequestPattern(varID, Atams::ACCESS_WRITE, Atams::REQUEST_UNTIL_ACK);
  }

  return (statusReturn);
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

  Node::Var_t &var = varStorage_[varID];

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

  Node::Var_t &var = varStorage_[varID];

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

  Node::Var_t &var = varStorage_[varID];

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

  Node::Var_t &var = varStorage_[varID];

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
  Atams::Error_t error = Node::clearDataReady(varID);
  if (!error)    error = Node::setReadStream(varID);

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
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::isDataReady(varID, newDataReady);

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

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (getAtamsType<T>() != varInfo.type)           return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (ACCESS_READ        > varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Node::Var_t   &var          = varStorage_[varID];
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  varStorageLock_.acquireLock();

  if (var.newDataReady) readFromVarStorage(outputRef, var);
  else                  statusReturn = Atams::ERROR_NEW_DATA_NOT_READY;

  var.newDataReady = false;

  varStorageLock_.releaseLock();

  return (statusReturn);
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
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::getVarIfDataReady(varID, readData);

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
  Atams::Error_t error = Node::clearWriteAck(varID);
  if (!error)    error = Node::setVar(varID, writeData);
  if (!error)    error = Node::setWriteStream(varID);

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
  Atams::Error_t error = Node::stopStream(varID);
  if (!error)    error = Node::isWriteAcked(varID, ackReceived);

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

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

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

  uint16_t requestPacketLength = requestPacket_.length;

  requestPacketLock_.releaseLock();

  return (requestPacketLength);
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

  uint16_t writeListLength = requestPacket_.writeList.getConfigCount();

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

  Atams::AbortedResponseDetails_t abortedResponseDetails = abortedResponseDetails_;

  busErrorLock_.releaseLock();

  return (abortedResponseDetails);
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
      if ((requestPacket_.length + Atams::DATAGRAM_SIZE_HEADER) < sizeof(requestPacket_.buffer))
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
      requestPacketLock_.acquireLock();
      resetRequestPacketNoLock();
      datagramHeader.command = Atams::ACCESS_READ;
      datagramHeader.varID   = readOnlyVarID;
      varIDUsed              = readOnlyVarID;
      if ((requestPacket_.length + Atams::DATAGRAM_SIZE_HEADER) < sizeof(requestPacket_.buffer))
      {
        Atams::datagramHeaderToBuffer(datagramHeader, &requestPacket_.buffer[requestPacket_.length]);
        requestPacket_.length += Atams::DATAGRAM_SIZE_HEADER;
      }
      requestPacketLock_.releaseLock();
      break;
    case ERROR_DECODE_FRAMING:
      /* TODO - Cannot be handled here */
      break;
    case ERROR_DECODE_CHECKSUM:
      /* TODO - Cannot be handled here */
      break;
    case ERROR_MESSAGE_TYPE:
      /* TODO - Cannot be handled here */
      break;
    case ERROR_SYNC_COUNT:
      /* TODO - Cannot be handled here */
      break;
    case ERROR_SYNC_NODE:
      /* TODO - Cannot be handled here */
      break;
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

template <typename T>
constexpr Atams::VarType_t Node::getAtamsType(void)
{
  if      constexpr (std::is_same<T, uint8_t>::value)  return (Atams::TYPE_UINT8);
  else if constexpr (std::is_same<T, int8_t>::value)   return (Atams::TYPE_INT8);
  else if constexpr (std::is_same<T, uint16_t>::value) return (Atams::TYPE_UINT16);
  else if constexpr (std::is_same<T, int16_t>::value)  return (Atams::TYPE_INT16);
  else if constexpr (std::is_same<T, uint32_t>::value) return (Atams::TYPE_UINT32);
  else if constexpr (std::is_same<T, int32_t>::value)  return (Atams::TYPE_INT32);
  else if constexpr (std::is_same<T, float>::value)    return (Atams::TYPE_FLOAT);
  else    static_assert(!std::is_same<T, T>::value,   "Invalid type passed to Node::getAtamsType(void)");
  return (Atams::TYPE_NULL);
}

template<typename T>
inline void Node::writeToVarStorage(const T inputVar, Node::Var_t &nodeVar)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in writeToVarStorage");

  uint32_t tempVar;

  if constexpr (std::is_same<T, float>::value) memcpy(&tempVar, &inputVar, sizeof(tempVar));
  else                                         tempVar = static_cast<uint32_t>(inputVar);

  /* Little endian: LSB first */
  nodeVar.storage[0U] = static_cast<uint8_t>((tempVar                     ) & SINGLE_BYTE_MASK);
  nodeVar.storage[1U] = static_cast<uint8_t>((tempVar >> SINGLE_BYTE_SHIFT) & SINGLE_BYTE_MASK);
  nodeVar.storage[2U] = static_cast<uint8_t>((tempVar >> TWO_BYTE_SHIFT   ) & SINGLE_BYTE_MASK);
  nodeVar.storage[3U] = static_cast<uint8_t>((tempVar >> THREE_BYTE_SHIFT ) & SINGLE_BYTE_MASK);
}

template<typename T>
inline void Node::readFromVarStorage(T &outputVar, const Node::Var_t &nodeVar)
{
  static_assert(sizeof(T) <= Atams::MAX_TYPE_SIZE, "Incompatible type size used in readFromVarStorage");

  uint32_t tempVar;

  /* Little endian: LSB first */
  tempVar = ((static_cast<uint32_t>(nodeVar.storage[0U])                     ) |
             (static_cast<uint32_t>(nodeVar.storage[1U]) << SINGLE_BYTE_SHIFT) |
             (static_cast<uint32_t>(nodeVar.storage[2U]) << TWO_BYTE_SHIFT   ) |
             (static_cast<uint32_t>(nodeVar.storage[3U]) << THREE_BYTE_SHIFT ) );

  if constexpr (std::is_same<T, float>::value) memcpy(&outputVar, &tempVar, sizeof(outputVar));
  else                                         outputVar = static_cast<T>(tempVar);
}

Atams::Error_t Node::externalTransfer(const Access_t  accessRequest,
                                      const uint16_t  varID,
                                      uint8_t * const bytesPtr,
                                      const uint8_t   length)
{
  if (varID >= validVarCount_) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::VarInfo_t &varInfo = memoryMap_->varInfoList[varID];

  if (TYPE_LENGTHS[varInfo.type] != length ) return (Atams::ERROR_VAR_TYPE); /* Early Return */
  if (bytesPtr                   == nullptr) return (Atams::ERROR_NULLPTR);  /* Early Return */

  Node::Var_t    &var        = varStorage_[varID];
  Atams::Error_t accessError = Atams::ERROR_NONE;

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

  varStorageLock_.acquireLock();
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

/* Warning - no check of remaining response buffer length before copy, *
 * validateResponseBuffer must be called before using this function    */
bool Node::processDatagramRead(const DatagramHeader_t datagramHeader,
                               uint16_t              &datagramStartIndex,
                               const uint8_t          payloadLength)
{
  Atams::Error_t status = updateRequestPatternOnReceive(datagramHeader.varID);

  if (status == Atams::ERROR_NONE) status = externalTransfer(Atams::ACCESS_WRITE,
                                                             datagramHeader.varID,
                                                             &responseBuffer_[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                                                             payloadLength); 

  if (status == Atams::ERROR_NONE) datagramStartIndex += (DATAGRAM_SIZE_HEADER + payloadLength);
  else                             reportBusError(status);

  return (status != Atams::ERROR_NONE);
}

bool Node::processDatagramWrite(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t status = updateRequestPatternOnReceive(datagramHeader.varID);
  
  if (status == Atams::ERROR_NONE)
  {
    /* VarID validity confirmed in updateRequestPatternOnReceive*/
    varStorageLock_.acquireLock();
    varStorage_[datagramHeader.varID].ackReceived = true;
    varStorageLock_.releaseLock();
  }

  if (status == Atams::ERROR_NONE) datagramStartIndex += DATAGRAM_SIZE_HEADER;
  else                             reportBusError(status);
  
  return (status != Atams::ERROR_NONE);
}

bool Node::processDatagramNack(const DatagramHeader_t datagramHeader, uint16_t &datagramStartIndex)
{
  Atams::Error_t statusReturn     = Atams::ERROR_NONE;
  bool           cancelProcessing = false;

  if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
  {
    statusReturn = updateRequestPatternOnReceive(datagramHeader.varID);

    if (statusReturn == Atams::ERROR_NONE)
    {
      statusReturn        = Atams::ERROR_CONFIGURATION_STATE_INACTIVE;
      datagramStartIndex += DATAGRAM_SIZE_HEADER;
    }
  }
  else 
  {
    cancelProcessing = true;
    statusReturn     = Atams::ERROR_INVALID_NACK;
  }

  if (statusReturn != Atams::ERROR_NONE) reportBusError(statusReturn);

  return (cancelProcessing);
}

Atams::Error_t Node::validateResponseBuffer(uint8_t * const responsePacket,
                                            const uint16_t  responsePacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex = HEADER_INDEX_FIRST_DATAGRAM;
  uint8_t          varLength          = 0U;

  while (datagramStartIndex + DATAGRAM_SIZE_HEADER <= responsePacketLength)
  {
    bufferToDatagramHeader(&responsePacket[datagramStartIndex], datagramHeader);

    Atams::Error_t lengthStatus = getVarLength(datagramHeader.varID, varLength);

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

  Atams::Error_t packetStatus = validateResponseBuffer(responseBuffer_, responseLength_);

  if (packetStatus != Atams::ERROR_NONE)
  {
    reportBusError(packetStatus);
    return; /* Early Return */
  }

  bool     cancelProcessing   = false;
  uint16_t datagramStartIndex = HEADER_INDEX_FIRST_DATAGRAM;

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= responseLength_) &&
         (cancelProcessing                          == false          ) )
  {
    DatagramHeader_t datagramHeader;

    bufferToDatagramHeader(&responseBuffer_[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateResponseBuffer */
    uint8_t varLength = TYPE_LENGTHS[memoryMap_->varInfoList[datagramHeader.varID].type];

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

DataStatusReturn_t<bool> Node::findDatagramMatchInPacket(RequestChangeConfig_t &changeConfig)
{
  DataStatusReturn_t<bool> statusReturn;
  statusReturn.status = Atams::ERROR_NONE;
  statusReturn.data   = false;
  uint8_t varLength   = 0U;

  changeConfig.datagramStartIndex = HEADER_INDEX_FIRST_DATAGRAM;

  while (changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacket_.length)    
  {
    bufferToDatagramHeader(&requestPacket_.buffer[changeConfig.datagramStartIndex], changeConfig.currentDatagramHeader);

    changeConfig.currentDatagramLength = DATAGRAM_SIZE_HEADER;

    if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE)
    {
      Atams::Error_t lengthStatus = getVarLength(changeConfig.currentDatagramHeader.varID, varLength);

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
  if (static_cast<uint16_t>(requestPacket_.length + shiftLength) < Atams::HEADER_SIZE_HEADER)
  {
    return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
  }

  if (static_cast<uint16_t>(requestPacket_.length + shiftLength) > sizeof(requestPacket_.buffer))
  {
    return (Atams::ERROR_REQUEST_BUFFER_LENGTH); /* Early Return */
  }

  memmove(&requestPacket_.buffer[shiftIndex + shiftLength], 
          &requestPacket_.buffer[shiftIndex], 
          (requestPacket_.length - shiftIndex));

  requestPacket_.length += shiftLength;

  requestPacket_.writeList.updateIndexes(shiftIndex, shiftLength);

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::requestPacketRemoveCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  uint16_t shiftIndex  =  changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t  shiftLength = -changeConfig.currentDatagramLength;
    
  Atams::Error_t statusReturn = Atams::ERROR_NONE;
  
  if (requestPacketShift(shiftIndex, shiftLength) != Atams::ERROR_NONE)
  {
    statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
  }
  else if (changeConfig.currentDatagramHeader.command == Atams::ACCESS_WRITE) 
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.currentDatagramHeader.varID);
  }

  return (statusReturn);
}

Atams::Error_t Node::requestPacketAdjustCurrentDatagram(RequestChangeConfig_t &changeConfig)
{
  Error_t     statusReturn = Atams::ERROR_NONE;
  uint16_t    shiftIndex   = changeConfig.datagramStartIndex + changeConfig.currentDatagramLength;
  int16_t     shiftLength  = changeConfig.newDatagramLength  - changeConfig.currentDatagramLength;

  bool writeListAdditionRequired = ((changeConfig.accessRequest  == Atams::ACCESS_WRITE  ) &&
                                    (changeConfig.requestPattern == Atams::REQUEST_STREAM) );

  if (writeListAdditionRequired)
  {
    WriteList::WriteConfig_t writeConfig;

    writeConfig.varID              = changeConfig.newDatagramHeader.varID;
    writeConfig.requestPacketIndex = static_cast<uint16_t>(changeConfig.datagramStartIndex + DATAGRAM_SIZE_HEADER);
    writeConfig.dataLength         = static_cast<uint8_t> (changeConfig.writePayloadLength);

    if (requestPacket_.writeList.addConfig(writeConfig) != WriteList::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_WRITE_LIST_FULL;
    }
  }
  else
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  if ((statusReturn == Atams::ERROR_NONE) &&
      (shiftLength  != 0                ) )
  { 
    statusReturn = requestPacketShift(shiftIndex, shiftLength);
  }

  if (statusReturn == Atams::ERROR_NONE)
  {
    /* Copy new datagram into available space */
    memcpy(&requestPacket_.buffer[changeConfig.datagramStartIndex], 
           changeConfig.newDatagramBuffer, 
           changeConfig.newDatagramLength);
  }
  else if (writeListAdditionRequired)
  {
    requestPacket_.writeList.removeConfigIfFound(changeConfig.newDatagramHeader.varID);
  }

  return (statusReturn);
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

  return (Atams::ERROR_NONE);
}

Atams::Error_t Node::constructDatagramBuffer(RequestChangeConfig_t &changeConfig)
{
  Error_t statusReturn = Atams::ERROR_NONE;

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
   
    changeConfig.newDatagramLength += changeConfig.writePayloadLength;
  }

  return (statusReturn);
}

void Node::resetRequestPacketNoLock(void)
{
  requestPacket_.length = Atams::HEADER_SIZE_HEADER;
  requestPacket_.writeList.reset();
  for (uint16_t varID = 0U; varID < validVarCount_; varID++)
  {
    Node::Var_t &var = varStorage_[varID];
    var.requestAccess  = Atams::ACCESS_NONE;
    var.requestPattern = Atams::REQUEST_INACTIVE;
  }
}

/* Warning - No OOR checks, should be completed by calling function */
Atams::Error_t Node::processRequestPacketChange(const uint16_t         varID,
                                                const Access_t         accessRequest,
                                                const RequestPattern_t requestPattern)
{
  if (varID >= validVarCount_) 
  {
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }

  RequestChangeConfig_t packetChangeConfig;
  Atams::Error_t        statusReturn           = Atams::ERROR_NONE;
  packetChangeConfig.accessRequest             = accessRequest; 
  packetChangeConfig.requestPattern            = requestPattern;
  packetChangeConfig.writePayloadLength        = TYPE_LENGTHS[memoryMap_->varInfoList[varID].type];
  packetChangeConfig.newDatagramHeader.command = accessRequest;
  packetChangeConfig.newDatagramHeader.varID   = varID;

  /* Search Node packet for a datagram matching the new datagram */
  DataStatusReturn_t<bool> datagramFoundInPacket = findDatagramMatchInPacket(packetChangeConfig);

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
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
  }
  else
  {
    if (constructDatagramBuffer(packetChangeConfig) != Atams::ERROR_NONE)
    {
      statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
    }
    else if (datagramFoundInPacket.data == true) 
    {
      statusReturn = requestPacketAdjustCurrentDatagram(packetChangeConfig);
    }
    else                                         
    {
      statusReturn = requestPacketAppendDatagram(packetChangeConfig);
    }
  }

  if (statusReturn == Atams::ERROR_REQUEST_PACKET_FATAL) resetRequestPacketNoLock();

  return (statusReturn);
}

Atams::Error_t Node::updateRequestPatternOnReceive(const uint16_t varID)
{
  if (varID >= validVarCount_)
  {
    reportBusError(Atams::ERROR_VAR_ID);
    return (Atams::ERROR_VAR_ID); /* Early Return */
  }
  
  requestPacketLock_.acquireLock();

  Node::Var_t &var = varStorage_[varID];

  Atams::Error_t   statusReturn          = Atams::ERROR_NONE;
  RequestPattern_t currentRequestPattern = var.requestPattern;
  
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

/* requestPacketLock_ must be acquired before using this function */
Atams::Error_t Node::updateRequestPacketWriteData(void)
{
  WriteList::Return_t listReturn;

  uint16_t writeListLength = requestPacket_.writeList.getConfigCount();
  
  for (uint16_t writeListIndex = 0U; writeListIndex < writeListLength; writeListIndex++)
  {
    listReturn = requestPacket_.writeList.getConfigAtIndex(writeListIndex);
    
    if (listReturn.status != WriteList::ERROR_NONE)
    {
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
    else if (externalTransfer(Atams::ACCESS_READ,
                              listReturn.writeConfig.varID, 
                              &requestPacket_.buffer[listReturn.writeConfig.requestPacketIndex], 
                              listReturn.writeConfig.dataLength)!= Atams::ERROR_NONE)
    {
      return (Atams::ERROR_REQUEST_PACKET_FATAL); /* Early Return */
    }
  }

  return (Atams::ERROR_NONE);
}

bool Node::validateGenInfo(void)
{
  if (getMemoryMapIsValid() == false) return (false); /* Early Return */
  
  const GenInfo_t nullGenInfo;
  bool            genInfoMatch = false;
  GenInfo_t       genInfo;
  
  static_cast<void>(getVar(BlockUniversal::VAR_ATAMS_VERSION_MAJOR, genInfo.atamsVersionMajor));
  static_cast<void>(getVar(BlockUniversal::VAR_ATAMS_VERSION_MINOR, genInfo.atamsVersionMinor));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_DAY,         genInfo.genDay));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_MONTH,       genInfo.genMonth));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_YEAR,        genInfo.genYear));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_HOUR,        genInfo.genHour));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_MINUTE,      genInfo.genMinute));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_GEN_SECOND,      genInfo.genSecond));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_CHECKSUM,        genInfo.genChecksum));
  static_cast<void>(getVar(BlockUniversal::VAR_MAP_NUMBER_OF_VARS,  genInfo.noOfVars));

  if ((genInfo != nullGenInfo         ) &&
      (genInfo == memoryMap_->genInfo) )
  {
    genInfoMatch = true;
  }

  return (genInfoMatch);
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

void Node::processAbortedResponse(void)
{
  uint8_t  bufferVarIDHi = responseBuffer_[Atams::ABORT_INDEX_VAR_ID_HI];
  uint8_t  bufferVarIDLo = responseBuffer_[Atams::ABORT_INDEX_VAR_ID_LO];
  uint8_t  errorByte     = responseBuffer_[Atams::ABORT_INDEX_ERROR];
  uint16_t varID         = ((static_cast<uint16_t>(bufferVarIDHi & Atams::ABORT_MASK_VAR_ID_HI) << Atams::ABORT_SHIFT_VAR_ID_HI) |
                            (static_cast<uint16_t>(bufferVarIDLo & Atams::ABORT_MASK_VAR_ID_LO) << Atams::ABORT_SHIFT_VAR_ID_LO) );

  if ((responseLength_ != Atams::ABORT_SIZE_PACKET) ||
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

Atams::Error_t Node::getEncodedRequestPacket(const Atams::MessageType_t requestType,
                                             const uint8_t              syncCount,
                                             uint8_t * const            outputBuffer,
                                             const uint16_t             outputBufferMaxLength, 
                                             uint16_t                  &outputLength)
{
  Atams::Error_t statusReturn = Atams::ERROR_NONE;

  requestPacketLock_.acquireLock();

  if (updateRequestPacketWriteData() != Atams::ERROR_NONE)
  {
    clearAllRequestPatterns();
    statusReturn = Atams::ERROR_REQUEST_PACKET_FATAL;
  }
  else 
  {
    requestPacket_.buffer[HEADER_INDEX_MSG_TYPE] = requestType;
    requestPacket_.buffer[HEADER_INDEX_NODE_ID ] = nodeID_;
    requestPacket_.buffer[HEADER_INDEX_SYNC]     = syncCount;
  
    statusReturn = encodeBusPacket(requestPacket_.buffer, 
                                   requestPacket_.length, 
                                   outputBuffer, 
                                   outputBufferMaxLength, 
                                   outputLength);
  }

  requestPacketLock_.releaseLock();

  if (statusReturn != Atams::ERROR_NONE) reportBusError(statusReturn);

  return (statusReturn);
}


} /* End Namespace - Atams */


/**
  * @}End of File
  */


