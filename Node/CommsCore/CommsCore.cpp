/**
  ******************************************************************************
  * @file    CommsCore.cpp
  *
  * @author  D. Baines
  *
  * @brief   Implementation of the Atams Node Communications Core for single 
  *          and dual-core platforms.
  *
  * @details The Atams Node Comms Core is responsible for updating the Node's
  *          communications interface - allowing external Hub devices to access
  *          the Node's variable storage, and trigger Node processes.
  *          On single-core platforms, user application code can use the Comms
  *          Core setter and getter functions to access the Node variable storage.
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

#include "CommsCore.hpp"

#include <string.h>
#include <atomic>

#include "CommsPlatform.hpp"
#include "../../Shared/Utilities/AtamsUtilities.hpp"
#include "../../Shared/Maps/BlockUniversal.hpp"
#include "../../Shared/Utilities/CRC32.hpp"
#include "../Developer/UniversalBlockManager.hpp"
#include "../Developer/FramingConstants.hpp"
#include "../Developer/NodeTypedefs.hpp"
#include "../Developer/CircularBuffer.hpp"
#include "../Developer/WatchdogHandler.hpp"
#include "../Developer/NVMUnitHandler.hpp"

/*************************************************************************************/
/* NAMESPACE                                                                         */
/*************************************************************************************/

namespace Atams {

/*************************************************************************************/

/*************************************************************************************/
/* PRIVATE TYPEDEFS                                                                  */
/*************************************************************************************/

enum NodeCommsState_t: uint8_t
{
  NODE_STATE_UNINITIALISED   = 0U,
  NODE_STATE_INITIALISED     = 1U,
  NODE_STATE_ACTIVE          = 2U,
  NODE_STATE_PROCESS_PENDING = 3U,
  NODE_STATE_RESTART_COMMS   = 4U
};

struct ChannelResponse_t
{
  uint8_t              buffer[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint16_t             index            {0U};
  bool                 aborted          {false};
  Atams::MessageType_t abortMessageType {Atams::MESSAGE_ABORT_RESPONSE};
  uint8_t              activeSyncCount  {0U};
  uint8_t              encodedBuffer[Platform::MAX_BUS_PACKET_SIZE];
  uint16_t             encodedLength    {0U};
};

struct ChannelSyncPacket_t
{
  uint8_t  buffer[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  uint16_t length {0U};
};

/*************************************************************************************/
/* PRIVATE CLASS OBJECTS                                                             */
/*************************************************************************************/

static const MemoryMap_t    *s_memoryMapPtr;
static CRC32                 s_nodeCRC(Atams::CRC32_POLYNOMIAL);
static CircularBuffer        s_circularBuffers[Platform::NUMBER_OF_COMMS_PERIPHERALS];

static WatchdogHandler       s_watchdogHandler;
static UniversalBlockManager s_uniBlockManager(s_watchdogHandler);
static NVMUnitHandler        s_nvmUnitHandler(s_nodeCRC);

/*************************************************************************************/
/* PRIVATE VARIABLES                                                                 */
/*************************************************************************************/

static uint16_t         s_validVarCount       {0U};
static NodeCommsState_t s_nodeCommsState      {NODE_STATE_UNINITIALISED};
static bool             s_appCoreInitRequired {true};

ATAMS_DUAL_CORE_SHARED_MEMORY_ATTRIBUTE
static SharedData_t s_sharedData;

/*************************************************************************************/
/* PRIVATE FUNCTION DEFINITIONS                                                      */
/*************************************************************************************/

static void resetVars(void)
{
  Platform::acquireVarStorageLock();

  for (VarStorage_t &varStorage : s_sharedData.varStorage)
  {
    memset(varStorage, 0U, sizeof(varStorage));
  }

  Platform::releaseVarStorageLock();
}

static void invalidateMemoryMap(void)
{
  s_validVarCount = 0U;
  s_memoryMapPtr  = nullptr;
  resetVars();
}

static Atams::Error_t externalTransfer(const Access_t  accessRequest,
                                       const uint16_t  varID,
                                       uint8_t * const bytesPtr,
                                       const uint8_t   length)
{
  if (varID >= s_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

  if (TYPE_LENGTHS[varInfo.type] != length)                 return (Atams::ERROR_VAR_TYPE);       /* Early Return */
  if (bytesPtr                   == nullptr)                return (Atams::ERROR_NULLPTR);        /* Early Return */
  if (accessRequest              >  varInfo.externalAccess) return (Atams::ERROR_ACCESS_INVALID); /* Early Return */

  Atams::VarStorage_t &varStorage {s_sharedData.varStorage[varID]};
  Atams::Error_t      accessError {Atams::ERROR_NONE};

  Platform::acquireVarStorageLock();

  switch (accessRequest)
  {
    case ACCESS_READ:
      memcpy(bytesPtr, varStorage, TYPE_LENGTHS[varInfo.type]);
      break;

    case ACCESS_WRITE:
      memcpy(varStorage, bytesPtr, TYPE_LENGTHS[varInfo.type]);
      break;

    default:
      accessError = Atams::ERROR_ACCESS_INVALID;
      break;
  }

  Platform::releaseVarStorageLock();

  return (accessError);
}

static DataStatusReturn_t<uint8_t> getVarLength(const uint16_t varID)
{
  DataStatusReturn_t<uint8_t> lengthReturn;

  if (varID >= s_validVarCount)
  {
    lengthReturn.status = Atams::ERROR_VAR_ID;
    return (lengthReturn); /* Early Return */
  }

  const NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

  lengthReturn.data   = TYPE_LENGTHS[varInfo.type];
  lengthReturn.status = Atams::ERROR_NONE;

  return (lengthReturn);
}

static void receiveCallback(const Platform::CommsPeripheralID_t commsChannel,
                            const uint8_t * const               rxBufferPtr,
                            const uint16_t                      rxBufferLength)
{
  if ((commsChannel   < Platform::NUMBER_OF_COMMS_PERIPHERALS) &&
      (rxBufferLength > 0U                                   ) )
  {
    if (s_circularBuffers[commsChannel].pushHead(rxBufferPtr, rxBufferLength) == CircularBuffer::ERROR_FULL)
    {
      s_circularBuffers[commsChannel].reset();
    }
    Platform::releaseWaitOnReceiveSemaphore();
  }
}

static void resetResponse(ChannelResponse_t &response, const uint8_t syncCount)
{
  response.activeSyncCount               = syncCount;
  response.aborted                       = false;
  response.buffer[HEADER_INDEX_MSG_TYPE] = MESSAGE_UNKNOWN;
  response.index                         = HEADER_INDEX_FIRST_DATAGRAM;
}

static void abortResponse(ChannelResponse_t &response, const Atams::Error_t error, const uint16_t varID)
{
  response.buffer[HEADER_INDEX_NODE_ID]  = s_uniBlockManager.getLocalNodeID();
  response.buffer[HEADER_INDEX_MSG_TYPE] = response.abortMessageType;
  response.buffer[ABORT_INDEX_ERROR]     = error;
  response.buffer[ABORT_INDEX_VAR_ID_HI] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_HI) & ABORT_MASK_VAR_ID_HI);
  response.buffer[ABORT_INDEX_VAR_ID_LO] = static_cast<uint8_t>((varID >> ABORT_SHIFT_VAR_ID_LO) & ABORT_MASK_VAR_ID_LO);
  response.index                         = ABORT_PACKET_SIZE;
  response.aborted                       = true;
}

static void sendResponsePacket(const Platform::CommsPeripheralID_t commsChannel,
                               ChannelResponse_t                  &response,
                               const Atams::MessageType_t          messageType)
{
  if (response.aborted == false)
  {
    response.buffer[HEADER_INDEX_MSG_TYPE] = messageType;
  }

  response.buffer[HEADER_INDEX_NODE_ID] = s_uniBlockManager.getLocalNodeID();
  response.buffer[HEADER_INDEX_SYNC]    = response.activeSyncCount;

  if (encodeBusPacket(response.buffer,
                      response.index,
                      response.encodedBuffer,
                      sizeof(response.encodedBuffer),
                      response.encodedLength        ) == Atams::ERROR_NONE)
  {
    Platform::transmitBuffer(commsChannel, response.encodedBuffer, response.encodedLength);
  }
}

static void processDatagramRead(ChannelResponse_t &response,
                                DatagramHeader_t   datagramHeader,
                                uint16_t          &requestPacketDatagramIndex,
                                const uint8_t      payloadLength)
{
  Atams::Error_t transferStatus {externalTransfer(ACCESS_READ,
                                                  datagramHeader.varID,
                                                  &response.buffer[response.index + DATAGRAM_SIZE_HEADER],
                                                  payloadLength)};

  if (transferStatus != Atams::ERROR_NONE)
  {
    abortResponse(response, transferStatus, datagramHeader.varID);
  }
  else
  {
    if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS) s_uniBlockManager.processRead(datagramHeader.varID);
    datagramHeader.command = RESPONSE_ACK_READ;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    requestPacketDatagramIndex += DATAGRAM_SIZE_HEADER;
  }
}

static void processDatagramWrite(ChannelResponse_t &response,
                                 DatagramHeader_t   datagramHeader,
                                 uint16_t          &requestPacketDatagramIndex,
                                 uint8_t * const    datagramPayload,
                                 const uint8_t      payloadLength)
{
  if ((datagramHeader.varID >= BlockUniversal::NUMBER_OF_VARS          ) ||
      (s_uniBlockManager.checkWriteAccess(datagramHeader.varID) == true) )
  {
    Atams::Error_t transferStatus {externalTransfer(ACCESS_WRITE,
                                                    datagramHeader.varID,
                                                    datagramPayload,
                                                    payloadLength)};

    if (transferStatus == Atams::ERROR_NONE)
    {
      datagramHeader.command      = RESPONSE_ACK_WRITE;
      datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
      response.index             += DATAGRAM_SIZE_HEADER;
      requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
      if (datagramHeader.varID < BlockUniversal::NUMBER_OF_VARS)
      {
        s_uniBlockManager.setUpdateRequired();
      }
    }
    else
    {
      abortResponse(response, transferStatus, datagramHeader.varID);
    }
  }
  else
  {
    datagramHeader.command      = Atams::RESPONSE_NACK;
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
    response.index             += Atams::DATAGRAM_SIZE_HEADER;
    requestPacketDatagramIndex += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + payloadLength);
    datagramHeaderToBuffer(datagramHeader, &response.buffer[response.index]);
  }
}

static void validateRequestPacket(ChannelResponse_t &response,
                                  uint8_t * const    requestPacket,
                                  const uint16_t     requestPacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex     {HEADER_INDEX_FIRST_DATAGRAM};
  uint16_t         requiredResponseLength {PACKET_HEADER_SIZE};

  while ((datagramStartIndex + DATAGRAM_SIZE_HEADER <= requestPacketLength) &&
         (response.aborted                          == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    DataStatusReturn_t<uint8_t> varLength = getVarLength(datagramHeader.varID);

    if (varLength.status != Atams::ERROR_NONE)
    {
      abortResponse(response, varLength.status, datagramHeader.varID);
    }
    else
    {
      switch (static_cast<Access_t>(datagramHeader.command))
      {
        case Atams::ACCESS_READ:
          datagramStartIndex     += DATAGRAM_SIZE_HEADER;
          requiredResponseLength += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength.data);
          break;

        case Atams::ACCESS_WRITE:
          datagramStartIndex     += static_cast<uint16_t>(DATAGRAM_SIZE_HEADER + varLength.data);
          requiredResponseLength += DATAGRAM_SIZE_HEADER;
          break;

        default:
          abortResponse(response, Atams::ERROR_ACCESS_INVALID, datagramHeader.varID);
          break;
      }
    }
  }

  if (response.aborted == false)
  {
    if (datagramStartIndex     != requestPacketLength    ) abortResponse(response, Atams::ERROR_REQUEST_BUFFER_LENGTH,  Atams::VAR_ID_NULL);
    if (requiredResponseLength  > sizeof(response.buffer)) abortResponse(response, Atams::ERROR_RESPONSE_BUFFER_LENGTH, Atams::VAR_ID_NULL);
  }
}

static void processRequestPacket(ChannelResponse_t &response,
                                 uint8_t * const    requestPacket,
                                 const uint16_t     requestPacketLength)
{
  DatagramHeader_t datagramHeader;
  uint16_t         datagramStartIndex {HEADER_INDEX_FIRST_DATAGRAM};

  resetResponse(response, requestPacket[HEADER_INDEX_SYNC]);

  validateRequestPacket(response, requestPacket, requestPacketLength);

  while (((datagramStartIndex + DATAGRAM_SIZE_HEADER) <= requestPacketLength) &&
         (response.aborted                            == false              ) )
  {
    bufferToDatagramHeader(&requestPacket[datagramStartIndex], datagramHeader);

    /* Var ID and Memory Map validity confirmed in validateRequestPacket */
    uint8_t varLength {TYPE_LENGTHS[s_memoryMapPtr->sharedMap.varInfoList[datagramHeader.varID].type]};

    switch (static_cast<Access_t>(datagramHeader.command))
    {
      case ACCESS_READ:
        processDatagramRead(response, datagramHeader, datagramStartIndex, varLength);
        break;

      case ACCESS_WRITE:
        processDatagramWrite(response,
                             datagramHeader,
                             datagramStartIndex,
                             &requestPacket[datagramStartIndex + DATAGRAM_INDEX_PAYLOAD],
                             varLength);
        break;

      default:
        abortResponse(response, Atams::ERROR_ACCESS_INVALID, datagramHeader.varID);
        return;
    }
  }

  if (response.aborted == false)
  {
    s_watchdogHandler.toggleWatchdog();
    s_nodeCommsState = NODE_STATE_ACTIVE;
  }
}

static inline void copyRequestToSyncPacket(ChannelSyncPacket_t  &syncPacket,
                                           const uint8_t * const requestBuffer,
                                           const uint16_t        requestLength)
{
  memcpy(syncPacket.buffer, requestBuffer, requestLength);
  syncPacket.length = requestLength;
}

static inline void setAbortMessageType(ChannelResponse_t &response, const MessageType_t messageType)
{
  switch (messageType)
  {
    case MESSAGE_REQUEST_SYNCED:
    case MESSAGE_RESPONSE_SYNCED:
    case MESSAGE_ABORT_RESPONSE_SYNCED:
    case MESSAGE_SYNC_JOG:
      response.abortMessageType = MESSAGE_ABORT_RESPONSE_SYNCED;
      break;
    default:
      response.abortMessageType = MESSAGE_ABORT_RESPONSE;
      break;
  }
}

static void processEncodedMeshPacket(const Platform::CommsPeripheralID_t commsChannel,
                                     const uint8_t * const               packetBuffer,
                                     const uint16_t                      packetLength)
{
  static uint8_t             s_decodedPacket[Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING];
  static uint16_t            s_decodedLength {0U};
  static ChannelSyncPacket_t s_commsChannelSyncPackets[Platform::NUMBER_OF_COMMS_PERIPHERALS];
  static ChannelResponse_t   s_commsChannelResponses[Platform::NUMBER_OF_COMMS_PERIPHERALS];

  if (decodeBusPacket(packetBuffer,
                      packetLength,
                      &s_decodedPacket[0U],
                      sizeof(s_decodedPacket),
                      s_decodedLength         ) == Atams::ERROR_NONE)
  {
    const MessageType_t messageType     {static_cast<MessageType_t>(s_decodedPacket[HEADER_INDEX_MSG_TYPE])};
    const uint8_t       packetNodeID    {s_decodedPacket[HEADER_INDEX_NODE_ID]};
    const uint8_t       localNodeID     {s_uniBlockManager.getLocalNodeID()};
    const uint8_t       firstSyncNodeID {s_uniBlockManager.getFirstSyncNodeID()};
    const uint8_t       finalSyncNodeID {s_uniBlockManager.getFinalSyncNodeID()};
    const uint8_t       prevSyncNodeID  {s_uniBlockManager.getPrevSyncNodeID()};
    const uint8_t       packetSyncCount {s_decodedPacket[HEADER_INDEX_SYNC]};

    ChannelSyncPacket_t &syncPacket {s_commsChannelSyncPackets[commsChannel]};
    ChannelResponse_t   &response   {s_commsChannelResponses[commsChannel]};

    setAbortMessageType(response, messageType);

    switch (messageType)
    {
      case MESSAGE_REQUEST:
        if (packetNodeID == localNodeID)
        {
          processRequestPacket(response, s_decodedPacket, s_decodedLength);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE);
        }
        break;
      case MESSAGE_REQUEST_SYNCED:
        if (packetNodeID == localNodeID)
        {
          if (localNodeID == finalSyncNodeID)
          {
            processRequestPacket(response, s_decodedPacket, s_decodedLength);
            if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
          }
          else
          {
            copyRequestToSyncPacket(syncPacket, s_decodedPacket, s_decodedLength);
          }
        }
        else if (packetNodeID == finalSyncNodeID)
        {
          processRequestPacket(response, syncPacket.buffer, syncPacket.length);
          if (localNodeID == firstSyncNodeID) sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_RESPONSE_SYNCED:
      case MESSAGE_ABORT_RESPONSE_SYNCED:
        if ((packetNodeID == prevSyncNodeID) &&
            (packetNodeID != localNodeID   ) )
        {
          if (packetSyncCount != response.activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      case MESSAGE_SYNC_JOG:
        if (packetNodeID == localNodeID)
        {
          if (packetSyncCount != response.activeSyncCount) abortResponse(response, Atams::ERROR_SYNC_COUNT, Atams::VAR_ID_NULL);
          sendResponsePacket(commsChannel, response, Atams::MESSAGE_RESPONSE_SYNCED);
        }
        break;
      default:
        /* Do Nothing */
        break;
    }
  }
  else
  {
    static uint32_t s_errorCount {0U};
    s_errorCount++;
    Atams::setVar(BlockUniversal::VAR_CRC_ERROR_COUNT, s_errorCount);
  }
}

static void processRawMeshData(void)
{
  static uint8_t  s_requestPacketBuffer[Platform::MAX_BUS_PACKET_SIZE];
  static uint16_t s_requestPacketLength {0U};

  for (uint8_t commsChannel {0U}; commsChannel < Platform::NUMBER_OF_COMMS_PERIPHERALS; commsChannel++)
  {
    const CircularBuffer::Error_t bufferStatus {s_circularBuffers[commsChannel].getPacket(s_requestPacketBuffer,
                                                                                          sizeof(s_requestPacketBuffer),
                                                                                          s_requestPacketLength)};

    if (bufferStatus == CircularBuffer::ERROR_NONE)
    {
      /* Process the packet that has been copied into the request packet buffer */
      processEncodedMeshPacket(static_cast<Platform::CommsPeripheralID_t>(commsChannel),
                               s_requestPacketBuffer,
                               s_requestPacketLength);
    }
    else if (bufferStatus == CircularBuffer::ERROR_NO_EOL_BUFFER_FULL)
    {
      s_circularBuffers[commsChannel].reset();
    }
  }
}

static Atams::Error_t getMemoryMapIsValid(void)
{
  if ((s_memoryMapPtr != nullptr) &&
      (s_validVarCount > 0U     ) )
  {
    return (Atams::ERROR_NONE);
  }

  return (Atams::ERROR_MEMORY_MAP);
}

static void waitForAppCoreInit(void)
{
  uint32_t                  previousCoreCheckTime {0U};
  volatile CoreInitStatus_t coreInitStatus        {CORE_INIT_IN_PROGRESS};

  Platform::acquireVarStorageLock();
  s_sharedData.coreInitComplete[CORE_APP] = CORE_INIT_IN_PROGRESS;
  Platform::releaseVarStorageLock();

  while (coreInitStatus == CORE_INIT_IN_PROGRESS)
  {
    uint32_t currentTime {Platform::getMillis()};

    if ((currentTime - previousCoreCheckTime) >= CORE_STATUS_CHECK_PERIOD)
    {
      Platform::acquireVarStorageLock();

      coreInitStatus = s_sharedData.coreInitComplete[CORE_APP];

      Platform::releaseVarStorageLock();

      previousCoreCheckTime = currentTime;
    }
  }
}

static void signalCommsCoreInitComplete(void)
{
  Platform::acquireVarStorageLock();
  s_sharedData.coreInitComplete[CORE_COMMS] = CORE_INIT_COMPLETE;
  Platform::releaseVarStorageLock();
}

/**
 * @brief   Encodes/decodes NVMHeader_t/NVMFooter_t to/from their fixed on-NVM layout.
 *
 * @details Built on Atams::uint32ToBuffer()/bufferToUint32() (Shared/Utilities/AtamsUtilities.hpp) -
 *          see the comment on NVMVarEntryHeader_t for why these types are never read/written via
 *          reinterpret_cast<uint8_t*>/sizeof().
 */
static void encodeNVMHeader(uint8_t * const bytesPtr, const NVMHeader_t &header)
{
  Atams::uint32ToBuffer(header.identifier, &bytesPtr[Atams::NVM_HEADER_INDEX_IDENTIFIER]);
  Atams::uint32ToBuffer(header.length,     &bytesPtr[Atams::NVM_HEADER_INDEX_LENGTH]);
  bytesPtr[Atams::NVM_HEADER_INDEX_FORMAT_VERSION] = header.nvmFormatVersion;
}

static NVMHeader_t decodeNVMHeader(const uint8_t * const bytesPtr)
{
  NVMHeader_t header;

  header.identifier       = Atams::bufferToUint32(&bytesPtr[Atams::NVM_HEADER_INDEX_IDENTIFIER]);
  header.length           = Atams::bufferToUint32(&bytesPtr[Atams::NVM_HEADER_INDEX_LENGTH]);
  header.nvmFormatVersion = bytesPtr[Atams::NVM_HEADER_INDEX_FORMAT_VERSION];

  return (header);
}

static void encodeNVMFooter(uint8_t * const bytesPtr, const NVMFooter_t &footer)
{
  Atams::uint32ToBuffer(footer.identifier, &bytesPtr[Atams::NVM_FOOTER_INDEX_IDENTIFIER]);
  Atams::uint32ToBuffer(footer.checksum,   &bytesPtr[Atams::NVM_FOOTER_INDEX_CHECKSUM]);
}

static NVMFooter_t decodeNVMFooter(const uint8_t * const bytesPtr)
{
  NVMFooter_t footer;

  footer.identifier = Atams::bufferToUint32(&bytesPtr[Atams::NVM_FOOTER_INDEX_IDENTIFIER]);
  footer.checksum   = Atams::bufferToUint32(&bytesPtr[Atams::NVM_FOOTER_INDEX_CHECKSUM]);

  return (footer);
}

static Atams::Error_t validateNVMChecksum(const NVMHeader_t &nvmHeader, const NVMFooter_t &nvmFooter)
{
  const uint32_t endOfVarStorage {nvmHeader.length - Atams::NVM_FOOTER_SIZE};

  Atams::Error_t error {Atams::ERROR_NONE};

  s_nodeCRC.beginRollingCRC();

  for (uint32_t nvmIndex {0U}; nvmIndex < endOfVarStorage; nvmIndex++)
  {
    uint8_t nvmByte;

    if (s_nvmUnitHandler.readFromNVM(nvmIndex, &nvmByte, 1U) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM); /* Early Return */
    }

    s_nodeCRC.updateRollingCRC(nvmByte);
  }

  const uint32_t nvmChecksum {s_nodeCRC.getRollingCRC()};

  if (nvmFooter.checksum != nvmChecksum)
  {
    error = Atams::ERROR_NVM_CHECKSUM;
  }

  return (error);
}

static uint32_t getNVMVarSpaceRequirement(void)
{
  uint32_t requiredVarSpace {0U};

  for (uint16_t varID {0U}; varID < s_memoryMapPtr->sharedMap.genInfo.noOfVars; varID++)
  {
    const NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

    if (varInfo.NVMStorage) requiredVarSpace += Atams::NVM_VAR_ENTRY_HEADER_SIZE + Atams::TYPE_LENGTHS[varInfo.type];
  }

  return (requiredVarSpace);
}

static Atams::Error_t constructAndStoreFooter(const uint32_t nvmSpaceUsed)
{
  NVMFooter_t nvmFooter;

  nvmFooter.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmFooter.checksum   = s_nodeCRC.getRollingCRC();

  uint8_t footerBuffer[Atams::NVM_FOOTER_SIZE];

  encodeNVMFooter(footerBuffer, nvmFooter);

  return (s_nvmUnitHandler.writeToNVM(nvmSpaceUsed, footerBuffer, Atams::NVM_FOOTER_SIZE));
}

/**
 * @brief   Encodes an NVMVarEntryHeader_t's fields into a fixed 5-byte little-endian layout.
 *
 * @details Deliberately does not use reinterpret_cast<uint8_t*>(&header)/sizeof(header) -
 *          see the comment on NVMVarEntryHeader_t for why this data must stay parseable
 *          across firmware rebuilds regardless of struct padding.
 */
static void encodeNVMVarEntryHeader(uint8_t * const bytesPtr, const uint32_t nvmHash, const Atams::VarType_t type)
{
  Atams::uint32ToBuffer(nvmHash, &bytesPtr[Atams::NVM_VAR_ENTRY_INDEX_NVM_HASH]);
  bytesPtr[Atams::NVM_VAR_ENTRY_INDEX_TYPE] = static_cast<uint8_t>(type);
}

static Atams::Error_t writeVarsToNVM(const uint32_t maxIndex)
{
  uint32_t nvmIndex {Atams::NVM_HEADER_SIZE};

  for (uint16_t varID {0U}; varID < s_memoryMapPtr->sharedMap.genInfo.noOfVars; varID++)
  {
    const NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

    if (varInfo.NVMStorage)
    {
      if (varInfo.type >= Atams::NUMBER_OF_VAR_TYPES)
      {
        return (Atams::ERROR_MEMORY_MAP);        /* Early Return */
      }

      const uint8_t  varLength {Atams::TYPE_LENGTHS[varInfo.type]};
      const uint32_t entrySize {Atams::NVM_VAR_ENTRY_HEADER_SIZE + varLength};

      if ((nvmIndex + entrySize) > maxIndex)
      {
        return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return */
      }

      uint8_t entryBuffer[Atams::NVM_VAR_ENTRY_HEADER_SIZE + Atams::MAX_TYPE_SIZE];

      encodeNVMVarEntryHeader(entryBuffer, varInfo.nvmHash, varInfo.type);

      Platform::acquireVarStorageLock();
      memcpy(&entryBuffer[Atams::NVM_VAR_ENTRY_HEADER_SIZE], s_sharedData.varStorage[varID], varLength);
      Platform::releaseVarStorageLock();

      if (s_nvmUnitHandler.writeToNVM(nvmIndex, entryBuffer, entrySize) != Atams::ERROR_NONE)
      {
        return (Atams::ERROR_PLATFORM);          /* Early Return */
      }

      nvmIndex += entrySize;
    }
  }

  return (Atams::ERROR_NONE);
}

/**
 * @brief   Decodes an NVMVarEntryHeader_t's fields from their fixed 5-byte little-endian layout.
 *
 * @details Mirrors encodeNVMVarEntryHeader() - see the comment on NVMVarEntryHeader_t for why
 *          this must stay a fixed-width decode rather than reinterpret_cast<NVMVarEntryHeader_t*>.
 */
static Atams::NVMVarEntryHeader_t decodeNVMVarEntryHeader(const uint8_t * const bytesPtr)
{
  Atams::NVMVarEntryHeader_t entryHeader;

  entryHeader.nvmHash = Atams::bufferToUint32(&bytesPtr[Atams::NVM_VAR_ENTRY_INDEX_NVM_HASH]);
  entryHeader.type    = static_cast<Atams::VarType_t>(bytesPtr[Atams::NVM_VAR_ENTRY_INDEX_TYPE]);

  return (entryHeader);
}

/**
 * @brief   Finds the varID whose NodeVarInfo_t::nvmHash matches nvmHash.
 *
 * @details Linear scan - this runs once per stored entry at boot, and the autogen tool already
 *          guarantees nvmHash is unique per variable (Autogen/Modules/FileAutogen.py), so at
 *          most one match is possible.
 */
static bool findVarIDByNvmHash(const uint32_t nvmHash, uint16_t &varID)
{
  for (uint16_t candidateID {0U}; candidateID < s_memoryMapPtr->sharedMap.genInfo.noOfVars; candidateID++)
  {
    const NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[candidateID]};

    if ((varInfo.NVMStorage) && (varInfo.nvmHash == nvmHash))
    {
      varID = candidateID;
      return (true);
    }
  }

  return (false);
}

static uint16_t countNVMStorageVars(void)
{
  uint16_t count {0U};

  for (uint16_t varID {0U}; varID < s_memoryMapPtr->sharedMap.genInfo.noOfVars; varID++)
  {
    if (s_memoryMapPtr->sharedMap.varInfoList[varID].NVMStorage) count++;
  }

  return (count);
}

/**
 * @brief   Walks the hash-tagged var entry stream, migrating whatever it can into varStorage.
 *
 * @param   maxIndex          NVM offset immediately after the last var entry (i.e. where the
 *                              footer starts) - NOT nvmHeader.length, which also includes the
 *                              footer itself.
 * @param[out] migrationOccurred Set true if anything was left at default that the stream didn't
 *                              already exactly account for (dropped entry, retyped variable, or
 *                              a current variable never encountered in the stream) - i.e. NVM no
 *                              longer exactly matches what a fresh storeAll() would produce. Only
 *                              ever set true, never reset - callers should initialise it false.
 *
 * @details For each stored entry: a matching, same-type variable is loaded; a matching but
 *          different-type variable (retyped since this was stored) is left at whatever
 *          initAllDefaults() already set, since its old bytes cannot be safely reinterpreted
 *          under the new type; an entry matching no current variable (removed/renamed since this
 *          was stored) is simply skipped over using its stored type's length. A variable that is
 *          never encountered in the stream (newly added, or NVMStorage newly enabled) is likewise
 *          left at its default - initAllDefaults() already ran before this function is called.
 */
static Atams::Error_t loadNVMVarsFromStream(const uint32_t maxIndex, bool &migrationOccurred)
{
  uint32_t nvmIndex   {Atams::NVM_HEADER_SIZE};
  uint16_t matchCount {0U};

  while (nvmIndex < maxIndex)
  {
    if ((nvmIndex + Atams::NVM_VAR_ENTRY_HEADER_SIZE) > maxIndex)
    {
      return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return - truncated entry stream */
    }

    uint8_t headerBuffer[Atams::NVM_VAR_ENTRY_HEADER_SIZE];

    if (s_nvmUnitHandler.readFromNVM(nvmIndex, headerBuffer, Atams::NVM_VAR_ENTRY_HEADER_SIZE) != Atams::ERROR_NONE)
    {
      return (Atams::ERROR_PLATFORM);          /* Early Return */
    }

    const Atams::NVMVarEntryHeader_t entryHeader {decodeNVMVarEntryHeader(headerBuffer)};

    nvmIndex += Atams::NVM_VAR_ENTRY_HEADER_SIZE;

    if (entryHeader.type >= Atams::NUMBER_OF_VAR_TYPES)
    {
      return (Atams::ERROR_NVM_ENTRY_CORRUPT); /* Early Return - corrupt entry stream */
    }

    const uint8_t valueLength {Atams::TYPE_LENGTHS[entryHeader.type]};

    if ((nvmIndex + valueLength) > maxIndex)
    {
      return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return - truncated entry stream */
    }

    uint16_t varID {0U};

    if (findVarIDByNvmHash(entryHeader.nvmHash, varID))
    {
      const NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

      matchCount++;

      if (varInfo.type == entryHeader.type)
      {
        uint8_t valueBuffer[Atams::MAX_TYPE_SIZE];

        if (s_nvmUnitHandler.readFromNVM(nvmIndex, valueBuffer, valueLength) != Atams::ERROR_NONE)
        {
          return (Atams::ERROR_PLATFORM);      /* Early Return */
        }

        Platform::acquireVarStorageLock();
        memcpy(s_sharedData.varStorage[varID], valueBuffer, valueLength);
        Platform::releaseVarStorageLock();
      }
      else
      {
        migrationOccurred = true; /* Type changed since this was stored - can't safely reinterpret. */
      }
    }
    else
    {
      migrationOccurred = true;  /* Removed/renamed since this was stored. */
    }

    nvmIndex += valueLength;
  }

  if (matchCount < countNVMStorageVars())
  {
    migrationOccurred = true;    /* At least one current variable was never seen in the stream. */
  }

  return (Atams::ERROR_NONE);
}

static Atams::Error_t saveVarsToNVM(const uint32_t availableNVMSpace)
{
  return (writeVarsToNVM(availableNVMSpace));
}

static void initCommsBuffers(void)
{
  for (uint8_t commsChannel {0U}; commsChannel < Platform::NUMBER_OF_COMMS_PERIPHERALS; commsChannel++)
  {
    s_circularBuffers[commsChannel].setEOLChar(Atams::EOL_BYTE);
    s_circularBuffers[commsChannel].setLockArgument(static_cast<Platform::CommsPeripheralID_t>(commsChannel));
  }
}

static Atams::Error_t initUniversalDefaults(void)
{
  Atams::Error_t error {Atams::ERROR_NONE};

  if (!error) error = Atams::setVar(BlockUniversal::VAR_ATAMS_VERSION_PATCH, Atams::ATAMS_VERSION_PATCH);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_MAX_BUS_PACKET_SIZE, static_cast<uint16_t>(Platform::MAX_BUS_PACKET_SIZE_PRE_FRAMING));
  if (!error) error = Atams::setVar(BlockUniversal::VAR_NODE_ID,             BlockUniversal::DEFAULT_NODE_ID);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_FIRST_NODE_ID,       BlockUniversal::DEFAULT_FIRST_NODE_ID);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_LAST_NODE_ID,        BlockUniversal::DEFAULT_LAST_NODE_ID);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_PREVIOUS_NODE_ID,    BlockUniversal::DEFAULT_PREVIOUS_NODE_ID);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_BITRATE,             BlockUniversal::DEFAULT_BITRATE);
  if (!error) error = Atams::setVar(BlockUniversal::VAR_WATCHDOG_PERIOD,     BlockUniversal::DEFAULT_WATCHDOG_PERIOD);

  return (error == Atams::ERROR_NONE ?
          Atams::ERROR_NONE          :
          Atams::ERROR_MEMORY_MAP    );
}

static Atams::Error_t initAllDefaults(void)
{
  Atams::Error_t error {getMemoryMapIsValid()};

  if (!error) error = initUniversalDefaults();

  if (!error) error = s_memoryMapPtr->userDefaultsInitFn();

  return (error);
}

static Atams::Error_t storeNVMHeader(const NVMHeader_t &nvmHeader)
{
  uint8_t headerBuffer[Atams::NVM_HEADER_SIZE];

  encodeNVMHeader(headerBuffer, nvmHeader);

  return (s_nvmUnitHandler.writeToNVM(0U, headerBuffer, Atams::NVM_HEADER_SIZE));
}

static Atams::Error_t extractNVMHeader(NVMHeader_t &nvmHeader)
{
  uint8_t headerBuffer[Atams::NVM_HEADER_SIZE];

  Atams::Error_t error {s_nvmUnitHandler.readFromNVM(0U, headerBuffer, Atams::NVM_HEADER_SIZE)};

  if (!error) nvmHeader = decodeNVMHeader(headerBuffer);

  return (error);
}

static Atams::Error_t extractNVMFooter(const NVMHeader_t &nvmHeader, NVMFooter_t &nvmFooter)
{
  const uint32_t nvmFooterIndex {nvmHeader.length - Atams::NVM_FOOTER_SIZE};

  uint8_t footerBuffer[Atams::NVM_FOOTER_SIZE];

  Atams::Error_t error {s_nvmUnitHandler.readFromNVM(nvmFooterIndex, footerBuffer, Atams::NVM_FOOTER_SIZE)};

  if (!error) nvmFooter = decodeNVMFooter(footerBuffer);

  return (error);
}

static Atams::Error_t validateNVMHeaderFooter(NVMHeader_t &nvmHeader, NVMFooter_t &nvmFooter)
{
  if (Platform::NVM_STORAGE_SIZE < Atams::NVM_HEADER_SIZE)            return (Atams::ERROR_NVM_PLATFORM_SIZE);          /* Early Return */

  if (extractNVMHeader(nvmHeader) != Atams::ERROR_NONE)              return (Atams::ERROR_PLATFORM);                   /* Early Return */

  if (nvmHeader.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)    return (Atams::ERROR_NVM_HEADER_VALIDITY);        /* Early Return */

  if (nvmHeader.nvmFormatVersion != Atams::NVM_FORMAT_VERSION)       return (Atams::ERROR_NVM_FORMAT_VERSION_MISMATCH);/* Early Return */

  if (Platform::NVM_STORAGE_SIZE < nvmHeader.length)                 return (Atams::ERROR_NVM_HEADER_LENGTH);          /* Early Return */

  if (extractNVMFooter(nvmHeader, nvmFooter) != Atams::ERROR_NONE)   return (Atams::ERROR_PLATFORM);                   /* Early Return */

  if (nvmFooter.identifier != Atams::NVM_HEADER_IDENTIFIER_VALID)    return (Atams::ERROR_NVM_HEADER_VALIDITY);        /* Early Return */

  return (validateNVMChecksum(nvmHeader, nvmFooter));
}

static Atams::Error_t initNVM(void)
{
  NVMHeader_t nvmHeader;
  NVMFooter_t nvmFooter;
  bool        migrationOccurred {false};

  Atams::Error_t error {getMemoryMapIsValid()};

  if (!error) error = validateNVMHeaderFooter(nvmHeader, nvmFooter);

  if (!error) error = loadNVMVarsFromStream(nvmHeader.length - Atams::NVM_FOOTER_SIZE, migrationOccurred);

  /* Best-effort compaction: rewrite NVM in the current map's entry form so dropped/retyped entries
   * are reclaimed and newly-added variables get an entry, but only once, right here, rather than
   * on every boot. A failed rewrite does not affect the value returned below - the variables
   * above were already loaded successfully into RAM; NVM simply keeps its previous (still valid,
   * still migratable) contents and the same rewrite will be attempted again next time this runs
   * with something to migrate. */
  if ((!error) && (migrationOccurred)) Atams::storeAll();

  s_uniBlockManager.notifyStorageProcessComplete(error);

  return (error);
}

static bool allCommsTransmissionsComplete(void)
{
  bool allComplete {true};

  for (uint8_t peripheralID {0U}; peripheralID < Platform::NUMBER_OF_COMMS_PERIPHERALS; peripheralID++)
  {
    if (Platform::transmitReady(static_cast<Platform::CommsPeripheralID_t>(peripheralID)) == false)
    {
      allComplete = false;
    }
  }

  return (allComplete);
}

/**
 * @brief Sets the value of a variable in the Node's variable storage.
 *
 * Writes a value to the specified variable in the Node's variable storage in a type-safe manner.
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 *
 * @param varID      The ID of the variable to set.
 * @param writeValue The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE     Variable successfully set.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
static Atams::Error_t setVarImpl(const uint16_t varID, const T writeValue)
{
  if (varID >= s_validVarCount) return (Atams::ERROR_VAR_ID); /* Early Return */

  const Atams::NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &varStorage = s_sharedData.varStorage[varID];

  Platform::acquireVarStorageLock();

  writeToVarStorage(writeValue, varStorage);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

/**
 * @brief Retrieves the value of a variable from the Node's variable storage.
 *
 * Copies the value of the specified variable from the Node's variable storage to the provided
 * output reference in a type-safe manner.
 *
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T        The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 *
 * @param varID     The ID of the variable to retrieve.
 * @param outputRef Reference to a variable where the retrieved value will be stored.
 *
 * @retval @c ERROR_NONE     Variable successfully retrieved.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <typename T>
Atams::Error_t getVarImpl(const uint16_t varID, T &outputRef)
{
  if (varID >= s_validVarCount) return (Atams:: ERROR_VAR_ID); /* Early Return */

  const Atams::NodeVarInfo_t &varInfo {s_memoryMapPtr->sharedMap.varInfoList[varID]};

  if (getAtamsType<T>() != varInfo.type) return (Atams::ERROR_VAR_TYPE); /* Early Return */

  Atams::VarStorage_t &var {s_sharedData.varStorage[varID]};

  Platform::acquireVarStorageLock();

  readFromVarStorage(outputRef, var);

  Platform::releaseVarStorageLock();

  return (Atams::ERROR_NONE);
}

/*************************************************************************************/
/* PUBLIC FUNCTION DEFINITIONS                                                       */
/*************************************************************************************/

/**
 * @brief   Initialise the Node for single core platforms.
 *
 * @details Validates and stores a reference to the provided Memory Map, initialises default variable values,
 *          validates and loads non-volatile memory (NVM) data, and starts the communication peripherals.
 *          The result of the NVM validation and load process is reported via the @p nvmError output parameter.
 *
 * @param      memoryMap Reference to a @c MemoryMap_t structure defining the Node's variable layout and properties.
 * @param[out] nvmError  Reference to an @c Atams::Error_t variable. On return, this will contain the result of the NVM
 *                       validation and load process. If @c ERROR_NONE, NVM data was present, valid, and loaded -
 *                       possibly with some variables migrated to their defaults if they were added, removed, renamed,
 *                       or retyped since the data was last stored (see NVMVarEntryHeader_t). Any other error means no
 *                       NVM data was loaded and all variables are at their defaults - either the stored data was
 *                       corrupt/absent, or @c ERROR_NVM_FORMAT_VERSION_MISMATCH if it was written by an incompatible
 *                       version of the Atams library.
 *
 * @retval @c ERROR_NONE                   Initialisation successful; Node is ready for operation.
 * @retval @c ERROR_MEMORY_MAP             Memory Map validation failed (Invalid structure, length, universal block, or checksum).
 * @retval @c ERROR_ATAMS_VERSION_MISMATCH Memory Map validation failed (Atams version mismatch).
 * @retval @c ERROR_PLATFORM               Platform fault occurred when attempting to start peripheral reception.
 *
 * @warning It is recommended to check the value of the @p nvmError output parameter after calling this function. 
 *          See param details above.
 *
 * @note    This function must be called before accessing any Node variables.
 */
Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError)
{
  s_appCoreInitRequired = false;

  return (initCommsCore(memoryMap, nvmError));
}

/**
 * @brief   Initialise the Node Communications Core for dual-core platforms.
 *
 * @details Validates and stores a reference to the provided Memory Map, initialises default variable values,
 *          validates and loads non-volatile memory (NVM) data, and starts the communication peripherals.
 *          The result of the NVM validation and load process is reported via the @p nvmError output parameter.
 *          This function also synchronises with the Application Core and will block until the Application Core
 *          has successfully completed its initialisation.
 *
 * @param      memoryMap Reference to a @c MemoryMap_t structure defining the Node's variable layout and properties.
 * @param[out] nvmError  Reference to an @c Atams::Error_t variable. On return, this will contain the result of the NVM
 *                       validation and load process. If @c ERROR_NONE, NVM data was present, valid, and loaded -
 *                       possibly with some variables migrated to their defaults if they were added, removed, renamed,
 *                       or retyped since the data was last stored (see NVMVarEntryHeader_t). Any other error means no
 *                       NVM data was loaded and all variables are at their defaults - either the stored data was
 *                       corrupt/absent, or @c ERROR_NVM_FORMAT_VERSION_MISMATCH if it was written by an incompatible
 *                       version of the Atams library.
 *
 * @retval @c ERROR_NONE                   Initialisation successful; Node is ready for operation.
 * @retval @c ERROR_MEMORY_MAP             Memory Map validation failed (Invalid structure, length, universal block, or checksum).
 * @retval @c ERROR_ATAMS_VERSION_MISMATCH Memory Map validation failed (Atams version mismatch).
 * @retval @c ERROR_PLATFORM               Platform fault occurred when attempting to start peripheral reception.
 *
 * @warning It is recommended to check the value of the @p nvmError output parameter after calling this function. 
 *          See param details above.
 *
 * @note    This function must be called before accessing any Node variables.
 */
Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError)
{
  if (s_appCoreInitRequired) waitForAppCoreInit();

  Atams::Error_t error {Atams::validateMemoryMap(memoryMap.sharedMap, Platform::NODE_NUMBER_OF_VARS)};

  if (!error) error = Atams::validateNodeMemoryMap(memoryMap.sharedMap);

  if (!error)
  {
    s_memoryMapPtr  = &memoryMap;
    s_validVarCount =  memoryMap.sharedMap.genInfo.noOfVars;

    resetVars();
  }

  if (!error) error    = initAllDefaults();
  if (!error) nvmError = initNVM();

  if (nvmError != Atams::ERROR_NONE)
  {
    resetVars();
    s_uniBlockManager.notifyStorageProcessComplete(nvmError);
    error = initAllDefaults();
  }

  if (!error) error = memoryMap.genInfoInitFn();
  if (!error) error = s_uniBlockManager.applyUniversalConfiguration();

  if (!error)
  {
    initCommsBuffers();

    error = (Platform::beginReceive(receiveCallback) == false) ?
            Atams::ERROR_PLATFORM                              :
            Atams::ERROR_NONE                                  ;
  }

  if (!error)
  {
    signalCommsCoreInitComplete();
    s_nodeCommsState = Atams::NODE_STATE_INITIALISED;
  }
  else
  {
    invalidateMemoryMap();
  }

  return (error);
}

/**
 * @brief Sets the value of a variable in the Node's variable storage.
 *
 * Writes a value to the specified variable in the Node's variable storage in a type-safe manner.
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T         The data type of the variable to set. Must match the type stored for the specified variable ID.
 *
 * @param varID      The ID of the variable to set.
 * @param writeValue The value to write to the variable storage.
 *
 * @retval @c ERROR_NONE     Variable successfully set.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <> Atams::Error_t setVar<uint8_t >(const uint16_t varID, const uint8_t  writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<int8_t  >(const uint16_t varID, const int8_t   writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<uint16_t>(const uint16_t varID, const uint16_t writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<int16_t >(const uint16_t varID, const int16_t  writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<uint32_t>(const uint16_t varID, const uint32_t writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<int32_t >(const uint16_t varID, const int32_t  writeValue) { return (setVarImpl(varID, writeValue)); }
template <> Atams::Error_t setVar<float   >(const uint16_t varID, const float    writeValue) { return (setVarImpl(varID, writeValue)); }

/**
 * @brief Retrieves the value of a variable from the Node's variable storage.
 *
 * Copies the value of the specified variable from the Node's variable storage to the provided
 * output reference in a type-safe manner.
 *
 * The data type @c T must match the type defined for the variable ID in the initialised Memory Map.
 *
 * @tparam T        The data type of the variable to retrieve. Must match the type stored for the specified variable ID.
 *
 * @param varID     The ID of the variable to retrieve.
 * @param outputRef Reference to a variable where the retrieved value will be stored.
 *
 * @retval @c ERROR_NONE     Variable successfully retrieved.
 * @retval @c ERROR_VAR_ID   The specified variable ID is out of range for the initialised Memory Map.
 * @retval @c ERROR_VAR_TYPE The data type for the specified variable ID does not match @c T.
 *
 * @note The Memory Map must be initialised before calling this function.
 */
template <> Atams::Error_t getVar<uint8_t >(const uint16_t varID, uint8_t  &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<int8_t  >(const uint16_t varID, int8_t   &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<uint16_t>(const uint16_t varID, uint16_t &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<int16_t >(const uint16_t varID, int16_t  &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<uint32_t>(const uint16_t varID, uint32_t &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<int32_t >(const uint16_t varID, int32_t  &outputRef) { return (getVarImpl(varID, outputRef)); };
template <> Atams::Error_t getVar<float   >(const uint16_t varID, float    &outputRef) { return (getVarImpl(varID, outputRef)); };

/**
 * @brief   Performs non-blocking polling and update operations for the Node Communications Core.
 *
 * @details Processes incoming and outgoing communication packets, updates the Node's state machine,
 *          and manages watchdog and pending storage processes. This function should be called regularly
 *          from the main application loop to maintain Node communications.
 *
 * @note    The Node should be initialised before calling this function.
 *
 * @note    This function operates in non-blocking mode and returns immediately after processing.
 *          For blocking operation, use @ref updateCommsBlocking. The recommended call period is
 *          equal to or less than @c WatchdogHandler::WATCHDOG_UPDATE_PERIOD (1ms) to maintain correct
 *          operation. More frequent calls improve Node responsiveness and Atams Bus update cycle speed.
 *
 * @warning If a storage process is initiated via Bus communications, this function will block while
 *          the storage process completes. Before this occurs, the application is queried whether it is 
 *          safe to enter configuration state using @ref Platform::enterConfigurationState. Blocking 
 *          processes will only occur if the Node is in the configuration state.
 */
void updateCommsPolling(void)
{
  uint32_t currentTime {Platform::getMillis()};

  switch (s_nodeCommsState)
  {
    case Atams::NODE_STATE_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_STATE_INITIALISED:
      Platform::update();
      processRawMeshData();
      break;

    case Atams::NODE_STATE_ACTIVE:
      Platform::update();
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      s_sharedData.watchdogFault.store(s_watchdogHandler.getWatchdogFault(), std::memory_order_relaxed);
      break;

    case Atams::NODE_STATE_PROCESS_PENDING:
      if (allCommsTransmissionsComplete() == true)
      {
        Platform::stopReceive();
        s_uniBlockManager.runPendingProcess();
        s_nodeCommsState = Atams::NODE_STATE_RESTART_COMMS;
      }
      break;

    case Atams::NODE_STATE_RESTART_COMMS:
      if (Platform::beginReceive(receiveCallback)) s_nodeCommsState = Atams::NODE_STATE_ACTIVE;
      break;

    default:
      /* Do Nothing */
      break;
  }

  if (s_uniBlockManager.update() < UniversalBlockManager::NUMBER_OF_PROCESSES)
  {
    s_nodeCommsState = Atams::NODE_STATE_PROCESS_PENDING;
  }
}

/**
 * @brief   Performs blocking update operations for the Node Communications Core.
 *
 * @details Processes incoming and outgoing communication packets, updates the Node's state machine,
 *          and manages watchdog and pending storage processes. This function operates in blocking mode, 
            blocking the calling thread while waiting for a communication event or for the watchdog period
 *          (@c WatchdogHandler::WATCHDOG_UPDATE_PERIOD, 1ms) to elapse before returning. 
 *          For non-blocking operation, use @ref updateCommsPolling.
 *
 * @note    The Node should be initialised before calling this function.
 *
 * @warning If a storage process is initiated via Bus communications, this function will block while
 *          the storage process completes. Before this occurs, the application is queried whether it is
 *          safe to enter configuration state using @ref Platform::enterConfigurationState.
 */
void updateCommsBlocking(void)
{
  uint32_t currentTime {Platform::getMillis()};

  switch (s_nodeCommsState)
  {
    case Atams::NODE_STATE_UNINITIALISED:
      /* Do Nothing - Wait for successful initialization */
      break;

    case Atams::NODE_STATE_INITIALISED:
      Platform::acquireWaitOnReceiveSempahore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      break;

    case Atams::NODE_STATE_ACTIVE:
      Platform::acquireWaitOnReceiveSempahore(WatchdogHandler::WATCHDOG_UPDATE_PERIOD);
      processRawMeshData();
      s_watchdogHandler.update(currentTime);
      s_sharedData.watchdogFault.store(s_watchdogHandler.getWatchdogFault(), std::memory_order_relaxed);
      break;

    case Atams::NODE_STATE_PROCESS_PENDING:
      if (allCommsTransmissionsComplete() == true)
      {
        Platform::stopReceive();
        s_uniBlockManager.runPendingProcess();
        s_nodeCommsState = Atams::NODE_STATE_RESTART_COMMS;
      }
      break;

    case Atams::NODE_STATE_RESTART_COMMS:
      if (Platform::beginReceive(receiveCallback)) s_nodeCommsState = Atams::NODE_STATE_ACTIVE;
      break;


    default:
      /* Do Nothing */
      break;
  }

  if (s_uniBlockManager.update() < UniversalBlockManager::NUMBER_OF_PROCESSES)
  {
    s_nodeCommsState = Atams::NODE_STATE_PROCESS_PENDING;
  }
}

/**
 * @brief   Restores all Node variables to their default values and saves them to non-volatile memory (NVM).
 *
 * @details This function resets all variables defined in the Node's Memory Map to their default values,
 *          then writes the updated values to NVM.
 *
 * @retval @c ERROR_NONE              All variables successfully restored and saved to NVM.
 * @retval @c ERROR_MEMORY_MAP        Memory Map validation failed or is not initialised.
 * @retval @c ERROR_PLATFORM          Platform-level NVM access failure (erase, write).
 * @retval @c ERROR_NVM_PLATFORM_SIZE Platform NVM size insufficient for header or data.
 *
 * @note    The Node must be initialised and a valid Memory Map must be present before calling this function.
 *          All user and Universal Block variables will be reset to their default values as defined in the Memory Map.
 *
 * @note    This function will be called automatically by @ref Atams::UniversalBlockManager if the Node is in the configuration
 *          state and a restore-all process has been triggered via bus communications.
 */
Atams::Error_t restoreAll(void)
{
  Atams::Error_t error {initAllDefaults()};

  if (!error) error = storeAll();

  return (error);
}

/**
 * @brief   Restores all User Data Block variables to their default values and saves them to non-volatile memory (NVM).
 *
 * @details This function resets all User Data Block variables defined in the Node's Memory Map to their default values,
 *          then writes the updated values to NVM. Universal Block variables are not affected.
 *
 * @retval @c ERROR_NONE              All user variables successfully restored and saved to NVM.
 * @retval @c ERROR_MEMORY_MAP        Memory Map validation failed or is not initialised.
 * @retval @c ERROR_PLATFORM          Platform-level NVM access failure (erase, write).
 * @retval @c ERROR_NVM_PLATFORM_SIZE Platform NVM size insufficient for header or data.
 *
 * @note    The Node must be initialised and a valid Memory Map must be present before calling this function.
 *          Only user Data Block variables will be reset to their default values as defined in the Memory Map.
 *          Universal Block variables remain unchanged.
 *
 * @note    This function may be called automatically by @ref Atams::UniversalBlockManager if the Node is in the configuration
 *          state and a restore-user process has been triggered via bus communications.
 */
Atams::Error_t restoreUser(void)
{
  Atams::Error_t error {getMemoryMapIsValid()};

  if (!error) error = s_memoryMapPtr->userDefaultsInitFn();

  if (!error) error = storeAll();

  return (error);
}

/**
 * @brief   Stores all Node variables to non-volatile memory (NVM).
 *
 * @details This function writes the current values of all variables defined in the Node's Memory Map
 *          to NVM, including both Universal and User Data Block variables. The NVM header and footer
 *          are constructed and written to ensure data integrity and version tracking.
 *
 * @retval @c ERROR_NONE              All variables successfully saved to NVM.
 * @retval @c ERROR_MEMORY_MAP        Memory Map validation failed or is not initialised.
 * @retval @c ERROR_PLATFORM          Platform-level NVM access failure (erase, write, flush).
 * @retval @c ERROR_NVM_PLATFORM_SIZE Platform NVM size insufficient for header or data.
 *
 * @note    The Node must be initialised and a valid Memory Map must be present before calling this function.
 *          All user and universal block variables will be saved to NVM as defined in the Memory Map.
 *
 * @warning This function stops communications while the storage process is in progress.
 */
Atams::Error_t storeAll(void)
{
  NVMHeader_t nvmHeader {};

  if (getMemoryMapIsValid() != Atams::ERROR_NONE) return (Atams::ERROR_MEMORY_MAP); /* Early Return */

  const uint32_t requiredNVMVarSpace {getNVMVarSpaceRequirement()};
  const uint32_t requiredNVMSpace    {Atams::NVM_HEADER_SIZE + requiredNVMVarSpace + Atams::NVM_FOOTER_SIZE};
  const uint32_t nvmFooterIndex      {Atams::NVM_HEADER_SIZE + requiredNVMVarSpace};

  if (requiredNVMSpace > Platform::NVM_STORAGE_SIZE) return (Atams::ERROR_NVM_PLATFORM_SIZE); /* Early Return */

  nvmHeader.identifier = Atams::NVM_HEADER_IDENTIFIER_VALID;
  nvmHeader.length     = requiredNVMSpace;

  /* Erase NVM to invalidate */
  Atams::Error_t error {s_nvmUnitHandler.eraseNVM()};

  if (!error) error = storeNVMHeader(nvmHeader);

  if (!error) error = saveVarsToNVM(nvmFooterIndex);

  if (!error) error = constructAndStoreFooter(nvmFooterIndex);

  if (!error) error = s_nvmUnitHandler.flushPendingUnit();

  for (CircularBuffer &circularBuffer : s_circularBuffers)
  {
    circularBuffer.reset();
  }

  return (error);
}

/**
 * @brief Get the watchdog fault status.
 *
 * Returns @c true if the time since the last valid message received
 * from an Atams Hub exceeds the configured watchdog period.
 *
 * @retval @c true  Watchdog fault active.
 * @retval @c false Watchdog fault inactive.
 *
 */
bool getWatchdogFault(void)
{
  return (static_cast<bool>(s_sharedData.watchdogFault.load(std::memory_order_relaxed)));
}


} /* End Namespace - Atams */

/**
  * @}End of File
  */
