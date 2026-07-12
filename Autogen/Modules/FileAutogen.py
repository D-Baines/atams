import os
import re
import sys
import pathlib
import pandas
from   dataclasses        import dataclass
from   enum               import Enum
from   enum               import StrEnum
from   typing             import List, Optional, TextIO, Tuple
from   datetime           import datetime
from   Modules.AtamsCRC32 import *
from   pathlib            import Path

FRAMEWORK_NAME           = "Atams"
NUMBER_OF_UNIVERSAL_VARS = 30

# Must match the Atams::BlockUniversal namespace name (Shared/Maps/BlockUniversal.hpp) -
# used to qualify Universal Block variable names for NVM hashing, same as user blocks.
UNIVERSAL_BLOCK_NAME = "UNIVERSAL"

# Must match Atams::BlockUniversal::VarID_t (Shared/Maps/BlockUniversal.hpp) - used to check
# generated variables don't collide with a Universal Block variable's NVM hash.
UNIVERSAL_VAR_NAMES = ["ATAMS_VERSION_MAJOR",
                       "ATAMS_VERSION_MINOR",
                       "ATAMS_VERSION_PATCH",
                       "MAP_GEN_DAY",
                       "MAP_GEN_MONTH",
                       "MAP_GEN_YEAR",
                       "MAP_GEN_HOUR",
                       "MAP_GEN_MINUTE",
                       "MAP_GEN_SECOND",
                       "MAP_CHECKSUM",
                       "MAP_NUMBER_OF_VARS",
                       "MAX_BUS_PACKET_SIZE",
                       "CONFIGURATION_PASSKEY",
                       "CONFIGURATION_STATUS",
                       "NODE_ID",
                       "FIRST_NODE_ID",
                       "LAST_NODE_ID",
                       "PREVIOUS_NODE_ID",
                       "BITRATE",
                       "WATCHDOG_PERIOD",
                       "STORE_ALL",
                       "RESTORE_USER_BLOCKS",
                       "RESTORE_ALL",
                       "RESET_NODE",
                       "STORAGE_STATUS",
                       "STORAGE_PROCESS_COMPLETE",
                       "WATCHDOG_FAULT_ACTIVE",
                       "WATCHDOG_RESET",
                       "CRC_ERROR_COUNT",
                       "COBS_ERROR_COUNT"]

# Mirror Atams::NVM_HEADER_SIZE / Atams::NVM_FOOTER_SIZE / Atams::NVM_VAR_ENTRY_HEADER_SIZE
# (Shared/AtamsTypedefs.hpp) - safe to hardcode since all three are sums of fixed-width primitive
# type sizes, never struct sizeof()s, so none of them can vary with a platform/compiler's padding
# or packing settings (NVMHeader_t/NVMFooter_t/NVMVarEntryHeader_t are all explicitly serialised
# field-by-field for exactly this reason - see the comments on those types). A generated
# static_assert next to each real definition guards against the formula itself changing.
NVM_HEADER_SIZE           = 9
NVM_FOOTER_SIZE           = 8
NVM_VAR_ENTRY_HEADER_SIZE = 5

# Mirror the NVMStorage variables in Shared/Maps/BlockUniversal.cpp - VAR_NODE_ID,
# VAR_FIRST_NODE_ID, VAR_LAST_NODE_ID, VAR_PREVIOUS_NODE_ID, VAR_BITRATE (uint8_t) and
# VAR_WATCHDOG_PERIOD (uint32_t): (5 * (NVM_VAR_ENTRY_HEADER_SIZE + 1)) + (NVM_VAR_ENTRY_HEADER_SIZE + 4).
UNIVERSAL_BLOCK_NVM_VAR_SPACE = 39

# Mirror Atams::TYPE_LENGTHS (Shared/AtamsTypedefs.hpp), keyed the same way typeUpper is already
# derived elsewhere in this file (types[varIndex].replace("_t", "").upper()).
TYPE_LENGTHS_BY_NAME = {
    "UINT8":  1, "INT8":  1,
    "UINT16": 2, "INT16": 2,
    "UINT32": 4, "INT32": 4,
    "FLOAT":  4,
}

class Platforms(Enum):
  NODE = 0
  HUB  = 1

class OpenMethods(StrEnum):
  READ_ONLY   = 'r'
  WRITE_TRY   = 'x'
  WRITE_FORCE = 'w'

class Error(StrEnum):
  NONE                  = "File Generation Successful!"
  MAKE_DIRECTORY_FAILED = "Generation Error: Failed to make directory"
  FILE_OPEN_FAILED      = "Generation Error: Failed to open files"
  EMPTY_VAR_NAME_CELL   = "Generation Error: Empty cells in the 'Var Name' column"
  EMPTY_DATA_TYPE_CELL  = "Generation Error: Empty cells in the 'Data Type' column"
  EMPTY_ACCESS_CELL     = "Generation Error: Empty cells in the 'External Access' column"
  EMPTY_NVM_STORAGE_CELL= "Generation Error: Empty cells in the 'NVM Storage' column"

class Access(Enum):
  ACCESS_NONE  = 0
  ACCESS_READ  = 1
  ACCESS_WRITE = 2

class VarType(Enum):
  TYPE_NULL   = 0
  TYPE_UINT8  = 1
  TYPE_INT8   = 2
  TYPE_UINT16 = 3
  TYPE_INT16  = 4
  TYPE_UINT32 = 5
  TYPE_INT32  = 6
  TYPE_FLOAT  = 7

@dataclass
class DataBlock:
  namePascal: str
  data:       pandas.DataFrame

def resourcePath(relativePath: str) -> Path:
  """Get absolute path to resource, works for dev and PyInstaller"""
  if hasattr(sys, "_MEIPASS"): basePath = os.path.join(sys._MEIPASS, 'Modules')      # PyInstaller temp folder
  else:                        basePath = os.path.dirname(os.path.abspath(__file__)) # Folder of this script
  return os.path.join(basePath, relativePath)

def getLongestString(strings: List[str]) -> int:
  maxStringLength = 0
  for string in strings:
    if (len(string) > maxStringLength):
      maxStringLength = len(string)
  return (maxStringLength)

def writeSpaces(noOfSpaces: int,
                targetFile: TextIO) -> None:
  while (noOfSpaces > 0):
    targetFile.write(" ")
    noOfSpaces -= 1

def sanitiseVarName(rawVarName: str) -> str:
  return rawVarName.replace(" ", "_").upper()

def sanitisePascalName(rawName: str) -> str:
  words = re.findall(r"[A-Za-z0-9]+", rawName)
  return "".join(word.capitalize() for word in words)

def generateEnum(iteratorStartValue: int,
                 minStringLength:    int,
                 prefixString:       str,
                 stringList:         List[str],
                 targetFile:         TextIO) -> None:
  iterator = iteratorStartValue
  requiredSpace = getLongestString(stringList)
  if (minStringLength > requiredSpace):
    requiredSpace = minStringLength
  for string in stringList:
    noOfSpaces = requiredSpace - len(string)
    targetFile.write(prefixString + string)
    writeSpaces(noOfSpaces, targetFile)
    targetFile.write(" = " + str(iterator) + "U,\n")
    iterator += 1
  targetFile.seek(targetFile.tell()-1)

def generateConstList(block:        pandas.DataFrame,
                      varNames:     List[str],
                      columnHeader: str,
                      targetFile:   TextIO) -> None:
  varIterator          = 0
  varNamesWithValue    = []
  types                   = block["Data Type"]
  values                  = block[columnHeader]
  preStringRequiredSpace  = getLongestString(types)
  for varName in varNames:
    value = values[varIterator]
    if ((not pandas.isnull(value)) and (value != "-")):
      varNamesWithValue.append(varName)
    varIterator += 1
  varIterator = 0
  postStringRequiredSpace = getLongestString(varNamesWithValue)
  for varName in varNames:
    type           = types[varIterator]
    value          = values[varIterator]
    preNameSpaces  = preStringRequiredSpace  - len(type)
    postNameSpaces = postStringRequiredSpace - len(varName)
    if ((not pandas.isnull(value)) and (value != "-")):
      valueAsString = str(value)
      targetFile.write("constexpr " + type + " ")
      writeSpaces(preNameSpaces, targetFile)
      targetFile.write((columnHeader.replace(' ', '_').upper()) + "_" + varName)
      writeSpaces(postNameSpaces, targetFile)
      targetFile.write(" {" + valueAsString)
      listItemNoSignNoPoint = valueAsString.replace('.', '')
      listItemNoSignNoPoint = listItemNoSignNoPoint.replace('-', '')
      if listItemNoSignNoPoint.isnumeric():
        if (type == "float"):
          if ("." not in valueAsString):
            targetFile.write(".0")
          targetFile.write("F")
        if (type == "uint8_t" or type == "uint16_t" or type == "uint32_t"):
          targetFile.write("U")
      targetFile.write("};\n")
    varIterator += 1

def computeVarNvmHash(qualifiedNameUpper: str) -> int:
  return CRC32().calculateCrc(qualifiedNameUpper.encode())

def computeQualifiedVarNvmHash(blockNameUpper: str, varNameUpper: str) -> int:
  # Variables are only accessed through their block namespace (Block::VAR_x), so the same
  # variable name is allowed to repeat across different blocks - qualify with the block
  # name so the NVM hash (a single flat namespace) doesn't collide between them.
  return computeVarNvmHash(blockNameUpper + "_" + varNameUpper)

def findDuplicateNvmHash(dataBlocks: List[DataBlock]) -> Optional[Tuple[str, str]]:
  seenNames = {computeQualifiedVarNvmHash(UNIVERSAL_BLOCK_NAME, name): (UNIVERSAL_BLOCK_NAME + "_" + name)
               for name in UNIVERSAL_VAR_NAMES}
  for block in dataBlocks:
    blockNameUpper = block.namePascal.upper()
    for varID in block.data["Var Name"]:
      varNameUpper     = sanitiseVarName(varID)
      qualifiedVarName = blockNameUpper + "_" + varNameUpper
      nvmHash          = computeVarNvmHash(qualifiedVarName)
      if nvmHash in seenNames:
        return (seenNames[nvmHash], qualifiedVarName)
      seenNames[nvmHash] = qualifiedVarName
  return None

def computeRequiredNvmSize(dataBlocks: List[DataBlock]) -> int:
  requiredVarSpace = UNIVERSAL_BLOCK_NVM_VAR_SPACE
  for block in dataBlocks:
    types       = block.data["Data Type"]
    nvmStorages = block.data["NVM Storage"]
    for varIndex in range(len(types)):
      if nvmStorages[varIndex] == "YES":
        typeUpper          = types[varIndex].replace("_t", "").upper()
        requiredVarSpace  += NVM_VAR_ENTRY_HEADER_SIZE + TYPE_LENGTHS_BY_NAME[typeUpper]
  return NVM_HEADER_SIZE + requiredVarSpace + NVM_FOOTER_SIZE

def generateVarInfoList(dataBlocks: List[DataBlock],
                        targetFile: TextIO,
                        isHub:      bool) -> None:
  blockIndex = 0
  for block in dataBlocks:
    blockNamePascal = block.namePascal
    varIDs          = block.data["Var Name"]
    types           = block.data["Data Type"]
    accessLevels    = block.data["External Access"]
    nvmStorages     = block.data["NVM Storage"]
    varIndex    = 0
    for varID in varIDs:
      varNameUpper    = sanitiseVarName(varID)
      typeUpper       = types[varIndex].replace("_t", "").upper()
      access          = accessLevels[varIndex]
      nvmStorage      = nvmStorages[varIndex]
      nvmStorageStr   = "Atams::ATAMS_FALSE"
      accessString    = "READ"
      nvmHash         = computeQualifiedVarNvmHash(blockNamePascal.upper(), varNameUpper)
      if (nvmStorage == "YES"): nvmStorageStr = "Atams::ATAMS_TRUE"
      if (access     == "RW"):  accessString  = "WRITE"
      targetFile.write("  /* [Block"+blockNamePascal+"::VAR_"+varNameUpper+"] = */\n")
      targetFile.write("  {\n")
      targetFile.write("    /* .type           = */ Atams::TYPE_"+typeUpper+",\n")
      targetFile.write("    /* .externalAccess = */ Atams::ACCESS_"+accessString+",\n")
      # Hub only ever needs type/externalAccess - NVMStorage/nvmHash are Node-only (its own NVM
      # storage path), so HubVarInfo_t (Hub/Node.hpp) doesn't carry them at all - see the
      # VarInfo_t Hub/Node Split plan.
      if (not isHub):
        targetFile.write("    /* .NVMStorage     = */ "+nvmStorageStr+",\n")
        targetFile.write("    /* .nvmHash        = */ 0x{:08X}U,\n".format(nvmHash))
      if ((varIndex    == (len(varIDs)     - 1) ) and
          (blockIndex  == (len(dataBlocks) - 1) ) ): targetFile.write("  }")
      else:                                   targetFile.write("  },\n")
      varIndex += 1
    blockIndex += 1

def generateInitUniversalMapInfo(targetFile: TextIO) -> None:

  universalvarsToSet = ["ATAMS_VERSION_MAJOR",
                        "ATAMS_VERSION_MINOR",
                        "MAP_GEN_DAY",
                        "MAP_GEN_MONTH",
                        "MAP_GEN_YEAR",
                        "MAP_GEN_HOUR",
                        "MAP_GEN_MINUTE",
                        "MAP_GEN_SECOND",
                        "MAP_CHECKSUM",
                        "MAP_NUMBER_OF_VARS"]
  variableNames      = ["atamsVersionMajor",
                        "atamsVersionMinor",
                        "genDay",
                        "genMonth",
                        "genYear",
                        "genHour",
                        "genMinute",
                        "genSecond",
                        "genChecksum",
                        "noOfVars"]
  varIterator = 0
  for varID in universalvarsToSet:
    variableName = variableNames[varIterator]
    targetFile.write("  if (!error) error = Atams::setVar(BlockUniversal::VAR_"+varID+", ")
    targetFile.write("s_genInfo."+variableName+");\n")
    varIterator += 1
  targetFile.seek(targetFile.tell()-1)

def generateInitDefaultsDefinition(dataBlocks: List[DataBlock],
                                   targetFile: TextIO) -> None:
  for block in dataBlocks:
    blockNamePascal  = block.namePascal
    varIDs           = block.data["Var Name"]
    defaults         = block.data["Default"]
    varsWithDefaults = []
    varIndex         = 0
    for varID in varIDs:
      default = defaults[varIndex]
      if ((not pandas.isnull(default)) and (default != "-")):
        varsWithDefaults.append(varID)
      varIndex += 1
    for varID in varsWithDefaults:
      varIDCaps = sanitiseVarName(varID)
      targetFile.write("  if (!error) error = Atams::setVar(Block"+blockNamePascal+"::VAR_" +varIDCaps+", ")
      targetFile.write("Block"+blockNamePascal+"::DEFAULT_"+varIDCaps+");\n")
  targetFile.seek(targetFile.tell()-1)

def generateMapChecksum(dataBlocks: List[DataBlock]) -> int:
  crcCalculator = CRC32()
  crcCalculator.beginRollingCrc()
  for block in dataBlocks:
    IDs            = block.data["Var Name"]
    types          = block.data["Data Type"]
    accessLevels   = block.data["External Access"]
    varID = 0
    for ID in IDs:
      match (types[varID]):
        case "uint8_t":  crcCalculator.updateRollingCrc(VarType.TYPE_UINT8.value)
        case "int8_t":   crcCalculator.updateRollingCrc(VarType.TYPE_INT8.value)
        case "uint16_t": crcCalculator.updateRollingCrc(VarType.TYPE_UINT16.value)
        case "int16_t":  crcCalculator.updateRollingCrc(VarType.TYPE_INT16.value)
        case "uint32_t": crcCalculator.updateRollingCrc(VarType.TYPE_UINT32.value)
        case "int32_t":  crcCalculator.updateRollingCrc(VarType.TYPE_INT32.value)
        case "float":    crcCalculator.updateRollingCrc(VarType.TYPE_FLOAT.value)
      if (accessLevels[varID] == "RW"): crcCalculator.updateRollingCrc(Access.ACCESS_WRITE.value)
      else:                             crcCalculator.updateRollingCrc(Access.ACCESS_READ.value)
      varID += 1
  return (crcCalculator.getRollingCrc())

def autogenCallMap(autogenHint:      str,
                   memMapNamePascal: str,
                   dataBlocks:       List[DataBlock],
                   targetFile:       TextIO,
                   mapNumberOfVars:  int,
                   timeStamp:        datetime,
                   isHub:            bool) -> None:
  match (autogenHint):
    case "MAP_NAME_PASCAL":
      targetFile.write(memMapNamePascal)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "VAR_INFO_LIST":
      generateVarInfoList(dataBlocks, targetFile, isHub)
    case "DATA_BLOCK_FILE_INCLUDES":
      for block in dataBlocks:
        targetFile.write('#include "Block' + block.namePascal + '.hpp"\n')
      targetFile.seek(targetFile.tell()-1)
    case "INIT_MAP_GEN_INFO_DEFINITION":
      generateInitUniversalMapInfo(targetFile)
    case "INIT_USER_DEFAULTS_DEFINITION":
      generateInitDefaultsDefinition(dataBlocks, targetFile)
    case "VERSION_MAJOR":
      targetFile.write("0U")
    case "VERSION_MINOR":
      targetFile.write("1U")
    case "GENERATION_DAY":
      targetFile.write(str(timeStamp.day) + "U")
    case "GENERATION_MONTH":
      targetFile.write(str(timeStamp.month) + "U")
    case "GENERATION_YEAR":
      targetFile.write(str(timeStamp.year) + "U")
    case "GENERATION_HOUR":
      targetFile.write(str(timeStamp.hour) + "U")
    case "GENERATION_MINUTE":
      targetFile.write(str(timeStamp.minute) + "U")
    case "GENERATION_SECOND":
      targetFile.write(str(timeStamp.second) + "U")
    case "GENERATION_CHECKSUM":
      targetFile.write(str(generateMapChecksum(dataBlocks)) + "U")
    case "NUMBER_OF_VARS":
      targetFile.write(str(mapNumberOfVars) + "U")
    case "REQUIRED_NVM_SIZE":
      targetFile.write(str(computeRequiredNvmSize(dataBlocks)) + "U")
    case "NVM_HEADER_SIZE_MIRROR":
      targetFile.write(str(NVM_HEADER_SIZE) + "U")
    case "NVM_FOOTER_SIZE_MIRROR":
      targetFile.write(str(NVM_FOOTER_SIZE) + "U")
    case "NVM_VAR_ENTRY_HEADER_SIZE_MIRROR":
      targetFile.write(str(NVM_VAR_ENTRY_HEADER_SIZE) + "U")

def generateMemoryMapFile(memMapNamePascal: str,
                          dataBlocks:       List[DataBlock],
                          templateFile:     TextIO,
                          targetFile:       TextIO,
                          mapNumberOfVars:  int,
                          timeStamp:        datetime,
                          isHub:            bool) -> None:
  inputFileString = templateFile.read()
  splitStrings    = inputFileString.split("$$$")
  nextStringAutogenCall = False
  nextStringAutogenEnd  = False

  for string in splitStrings:
    if (string == "AUTOGEN"):
      if (nextStringAutogenEnd == True):
        nextStringAutogenEnd = False
      else:
        nextStringAutogenCall = True
    elif (nextStringAutogenCall == True):
      splitStrings.remove(string)
      autogenCallMap(string,
                     memMapNamePascal,
                     dataBlocks,
                     targetFile,
                     mapNumberOfVars,
                     timeStamp,
                     isHub)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)

def autogenCallBlock(autogenHint:        str,
                     memMapNamePascal:   str,
                     blockNamePascal:    str,
                     dataBlock:          pandas.DataFrame,
                     cumulativeVarIndex: int,
                     targetFile:         TextIO) -> None:
  varIDsUpper = [sanitiseVarName(varID) for varID in dataBlock["Var Name"]]

  match (autogenHint):
    case "MAP_NAME_PASCAL":
      targetFile.write(memMapNamePascal)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_NAME_PASCAL":
      targetFile.write(blockNamePascal)
    case "VAR_ID_LIST":
      generateEnum(cumulativeVarIndex, 0, "  VAR_", varIDsUpper, targetFile)
    case "NUMBER_OF_VARS":
      targetFile.write(str(len(dataBlock["Var Name"])) + "U")
    case "DEFAULTS":
      generateConstList(dataBlock, varIDsUpper, "Default", targetFile)

def generateDataBlockFile(memMapNamePascal:   str,
                          blockNamePascal:    str,
                          dataBlock:          pandas.DataFrame,
                          cumulativeVarIndex: int,
                          templateFile:       TextIO,
                          targetFile:         TextIO,) -> None:
  inputFileString       = templateFile.read()
  splitStrings          = inputFileString.split("$$$")
  nextStringAutogenCall = False
  nextStringAutogenEnd  = False

  for string in splitStrings:
    if (string == "AUTOGEN"):
      if (nextStringAutogenEnd):
        nextStringAutogenEnd = False
      else:
        nextStringAutogenCall = True
    elif (nextStringAutogenCall):
      splitStrings.remove(string)
      autogenCallBlock(string,
                       memMapNamePascal,
                       blockNamePascal,
                       dataBlock,
                       cumulativeVarIndex,
                       targetFile)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)

def generateCppFiles(memMapNamePascal:  str,
                     memoryMapXlsxPath: str,
                     nodeDirectory:     str,
                     hubDirectory:      str) -> Error | str:

  timeStamp = datetime.now()

  memMapHppName = "Map" + memMapNamePascal + ".hpp"
  memMapCppName = "Map" + memMapNamePascal + ".cpp"
  dataBlocks: List[DataBlock] = []
  blockHppTemplatePath   = resourcePath(os.path.join('Templates', 'BlockTemplateHpp.txt'))
  mapTemplateHppPathHub  = resourcePath(os.path.join('Templates', 'MapTemplateHppHub.txt'))
  mapTemplateHppPathNode = resourcePath(os.path.join('Templates', 'MapTemplateHppNode.txt'))
  mapTemplateCppPathHub  = resourcePath(os.path.join('Templates', 'MapTemplateCppHub.txt'))
  mapTemplateCppPathNode = resourcePath(os.path.join('Templates', 'MapTemplateCppNode.txt'))
  memMapNodeDir = os.path.join(nodeDirectory, 'Maps', ('Map' + memMapNamePascal))
  memMapHubDir  = os.path.join(hubDirectory,  'Maps', ('Map' + memMapNamePascal))

  try:
    dataBlockNames = pandas.ExcelFile(memoryMapXlsxPath).sheet_names
    for dataBlockName in dataBlockNames:
      blockData = pandas.read_excel(memoryMapXlsxPath, sheet_name=dataBlockName)
      dataBlocks.append(DataBlock(sanitisePascalName(dataBlockName), blockData))
  except:
    return (Error.FILE_OPEN_FAILED)

  try:
    pathlib.Path(memMapNodeDir).mkdir(parents=False, exist_ok=True)
    pathlib.Path(memMapHubDir).mkdir(parents=False, exist_ok=True)
  except:
    return (Error.MAKE_DIRECTORY_FAILED)

  for block in dataBlocks:
    varIDs     = block.data["Var Name"]
    types      = block.data["Data Type"]
    access     = block.data["External Access"]
    nvmStorage = block.data["NVM Storage"]
    if varIDs.isnull().any():
       return (Error.EMPTY_VAR_NAME_CELL)
    if types.isnull().any():
       return (Error.EMPTY_DATA_TYPE_CELL)
    if access.isnull().any():
       return (Error.EMPTY_ACCESS_CELL)
    if nvmStorage.isnull().any():
       return (Error.EMPTY_NVM_STORAGE_CELL)

  duplicateNvmHash = findDuplicateNvmHash(dataBlocks)
  if duplicateNvmHash is not None:
     existingName, newName = duplicateNvmHash
     return (f"Generation Error: '{existingName}' and '{newName}' produce the same NVM hash (or share a name) - rename one")

  mapNumberOfVars = NUMBER_OF_UNIVERSAL_VARS

  for block in dataBlocks:
    blockNumberOfVars = len(block.data["Var Name"])
    mapNumberOfVars += blockNumberOfVars

  mapHppPathNode = os.path.join(memMapNodeDir, memMapHppName)
  mapCppPathNode = os.path.join(memMapNodeDir, memMapCppName)
  mapHppPathHub  = os.path.join(memMapHubDir,  memMapHppName)
  mapCppPathHub  = os.path.join(memMapHubDir,  memMapCppName)

  mapTemplateHppNode = open(mapTemplateHppPathNode, OpenMethods.READ_ONLY)
  mapHppNode         = open(mapHppPathNode,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNamePascal,
                        dataBlocks,
                        mapTemplateHppNode,
                        mapHppNode,
                        mapNumberOfVars,
                        timeStamp,
                        False)
  mapHppNode.close()
  mapTemplateHppNode.close()

  mapTemplateHppHub = open(mapTemplateHppPathHub, OpenMethods.READ_ONLY)
  mapHppHub         = open(mapHppPathHub,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNamePascal,
                        dataBlocks,
                        mapTemplateHppHub,
                        mapHppHub,
                        mapNumberOfVars,
                        timeStamp,
                        True)
  mapHppHub.close()
  mapTemplateHppHub.close()

  mapTemplateCppNode = open(mapTemplateCppPathNode, OpenMethods.READ_ONLY)
  mapCppNode         = open(mapCppPathNode,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNamePascal,
                        dataBlocks,
                        mapTemplateCppNode,
                        mapCppNode,
                        mapNumberOfVars,
                        timeStamp,
                        False)
  mapCppNode.close()
  mapTemplateCppNode.close()

  mapTemplateCppHub = open(mapTemplateCppPathHub, OpenMethods.READ_ONLY)
  mapCppHub         = open(mapCppPathHub,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNamePascal,
                        dataBlocks,
                        mapTemplateCppHub,
                        mapCppHub,
                        mapNumberOfVars,
                        timeStamp,
                        True)
  mapCppHub.close()
  mapTemplateCppHub.close()

  cumulativeVarIndex = NUMBER_OF_UNIVERSAL_VARS
  blockHppTemplate   = open(blockHppTemplatePath, OpenMethods.READ_ONLY)
  for block in dataBlocks:

    blockHppTemplate.seek(0)
    blockHppName     = "Block" + block.namePascal + ".hpp"

    blockHppPathNode = os.path.join(memMapNodeDir, blockHppName)
    blockHppNode     = open(blockHppPathNode, OpenMethods.WRITE_FORCE)
    generateDataBlockFile(memMapNamePascal,
                          block.namePascal,
                          block.data,
                          cumulativeVarIndex,
                          blockHppTemplate,
                          blockHppNode)
    blockHppNode.close()

    blockHppTemplate.seek(0)

    blockHppPathHub = os.path.join(memMapHubDir, blockHppName)
    blockHppHub     = open(blockHppPathHub, OpenMethods.WRITE_FORCE)
    generateDataBlockFile(memMapNamePascal,
                          block.namePascal,
                          block.data,
                          cumulativeVarIndex,
                          blockHppTemplate,
                          blockHppHub)
    blockHppHub.close()

    cumulativeVarIndex += len(block.data["Var Name"])

  return (Error.NONE)
