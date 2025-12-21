import os
import sys
import pathlib
import pandas
from   enum               import Enum
from   enum               import StrEnum
from   typing             import List, TextIO
from   datetime           import datetime
from   Modules.AtamsCRC32 import *
from   pathlib            import Path

FRAMEWORK_NAME           = "Atams"
NUMBER_OF_UNIVERSAL_VARS = 28

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
  EMPTY_VAR_ID_CELL     = "Generation Error: Empty cells in the 'Var ID' column"
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

class NVMStorageFlag(Enum):
  STORAGE_FALSE = 0
  STORAGE_TRUE  = 1

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

def generateVarInfoList(dataBlockNamesCamel: List[str],
                        dataBlocks:          pandas.DataFrame,
                        targetFile:          TextIO) -> None:
  blockIndex = 0
  for block in dataBlocks:
    blockNameCamel = dataBlockNamesCamel[blockIndex]
    varIDs         = block["Var ID"]
    types          = block["Data Type"]
    accessLevels   = block["External Access"]
    nvmStorages    = block["NVM Storage"]
    varIndex    = 0
    for varID in varIDs:
      varNameUpper    = varID.replace(" ", "_").upper()
      typeUpper       = types[varIndex].replace("_t", "").upper()
      access          = accessLevels[varIndex]
      nvmStorage      = nvmStorages[varIndex]
      nvmStorageStr   = "Atams::ATAMS_FALSE"
      accessString    = "READ"
      if (nvmStorage == "YES"): nvmStorageStr = "Atams::ATAMS_TRUE"
      if (access     == "RW"):  accessString  = "WRITE"
      targetFile.write("  /* [Block"+blockNameCamel+"::VAR_"+varNameUpper+"] = */\n")
      targetFile.write("  {\n")
      targetFile.write("    /* .type           = */ Atams::TYPE_"+typeUpper+",\n")
      targetFile.write("    /* .externalAccess = */ Atams::ACCESS_"+accessString+",\n")
      targetFile.write("    /* .NVMStorage     = */ "+nvmStorageStr+",\n")
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

def generateInitDefaultsDefinition(dataBlockNamesCamel: List[str],
                                   dataBlocks:          pandas.DataFrame,
                                   targetFile:          TextIO) -> None:
  blockIndex = 0
  for block in dataBlocks:
    blockNameCamel   = dataBlockNamesCamel[blockIndex]
    varIDs           = block["Var ID"]
    defaults         = block["Default"]
    varsWithDefaults = []
    varIndex         = 0
    for varID in varIDs:
      default = defaults[varIndex]
      if ((not pandas.isnull(default)) and (default != "-")): 
        varsWithDefaults.append(varID)
      varIndex += 1
    for varID in varsWithDefaults:
      varIDCaps = varID.replace(" ", "_").upper()
      targetFile.write("  if (!error) error = Atams::setVar(Block"+blockNameCamel+"::VAR_" +varIDCaps+", ")
      targetFile.write("Block"+blockNameCamel+"::DEFAULT_"+varIDCaps+");\n")
    blockIndex +=1
  targetFile.seek(targetFile.tell()-1)

def generateMapChecksum(dataBlocks: List[pandas.DataFrame]) -> int:
  crcCalculator = CRC32()
  crcCalculator.beginRollingCrc()
  for block in dataBlocks:
    IDs            = block["Var ID"]
    types          = block["Data Type"]
    accessLevels   = block["External Access"]
    NVMStorages    = block["NVM Storage"]
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
      if (NVMStorages[varID] == "YES"): crcCalculator.updateRollingCrc(NVMStorageFlag.STORAGE_TRUE.value)
      else:                             crcCalculator.updateRollingCrc(NVMStorageFlag.STORAGE_FALSE.value)
      varID += 1
  return (crcCalculator.getRollingCrc())

def autogenCallMap(autogenHint:         str,
                   memMapNameCamel:     str,
                   dataBlockNamesCamel: List[str], 
                   dataBlocks:          List[pandas.DataFrame],
                   targetFile:          TextIO,
                   mapNumberOfVars:     int,
                   timeStamp:           datetime) -> None:
  match (autogenHint):
    case "MAP_NAME_CAMEL": 
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME": 
      targetFile.write(FRAMEWORK_NAME)
    case "VAR_INFO_LIST": 
      generateVarInfoList(dataBlockNamesCamel, dataBlocks, targetFile)
    case "DATA_BLOCK_FILE_INCLUDES": 
      for blockName in dataBlockNamesCamel:
        targetFile.write('#include "Block' + blockName + '.hpp"\n')
      targetFile.seek(targetFile.tell()-1)
    case "INIT_MAP_GEN_INFO_DEFINITION": 
      generateInitUniversalMapInfo(targetFile)
    case "INIT_USER_DEFAULTS_DEFINITION":
      generateInitDefaultsDefinition(dataBlockNamesCamel, dataBlocks, targetFile)
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

def generateMemoryMapFile(memMapNameCamel:     str,
                          dataBlockNamesCamel: List[str], 
                          dataBlocks:          List[pandas.DataFrame], 
                          templateFile:        TextIO,
                          targetFile:          TextIO,
                          mapNumberOfVars:     int,
                          timeStamp:           datetime) -> None:
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
                     memMapNameCamel,
                     dataBlockNamesCamel, 
                     dataBlocks,
                     targetFile,
                     mapNumberOfVars,
                     timeStamp)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)
      
def autogenCallBlock(autogenHint:        str,
                     memMapNameCamel:    str,
                     blockNameCamel:     str,
                     dataBlock:          pandas.DataFrame,
                     cumulativeVarIndex: int,
                     targetFile:         TextIO) -> None:
  varIDsUpper = []
  for varID in dataBlock["Var ID"]:
    varIDsUpper.append(varID.replace(" ", "_").upper())

  match (autogenHint):
    case "MAP_NAME_CAMEL":
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_NAME_CAMEL":
      targetFile.write(blockNameCamel)
    case "BLOCK_NAME_UPPER":
      targetFile.write(blockNameCamel.upper())
    case "VAR_ID_LIST":
      generateEnum(cumulativeVarIndex, 0, "  VAR_", varIDsUpper, targetFile)
    case "NUMBER_OF_VARS":
      targetFile.write(str(len(dataBlock["Var ID"])) + "U")
    case "DEFAULTS":
      generateConstList(dataBlock, varIDsUpper, "Default", targetFile)
  
def generateDataBlockFile(memMapNameCamel:    str,
                          blockNameCamel:     str,
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
                       memMapNameCamel,
                       blockNameCamel,
                       dataBlock,
                       cumulativeVarIndex,
                       targetFile)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)

def generateCppFiles(memMapNameCamel:   str, 
                     memoryMapXlsxPath: str, 
                     nodeDirectory:     str, 
                     hubDirectory:      str) -> Error:

  timeStamp = datetime.now()

  memMapHppName = "Map" + memMapNameCamel + ".hpp"
  memMapCppName = "Map" + memMapNameCamel + ".cpp"
  dataBlocks          = []
  dataBlockNamesCamel = []
  blockHppTemplatePath   = resourcePath(os.path.join('Templates', 'BlockTemplateHpp.txt'))
  mapTemplateHppPathHub  = resourcePath(os.path.join('Templates', 'MapTemplateHppHub.txt'))
  mapTemplateHppPathNode = resourcePath(os.path.join('Templates', 'MapTemplateHppNode.txt'))
  mapTemplateCppPathHub  = resourcePath(os.path.join('Templates', 'MapTemplateCppHub.txt'))
  mapTemplateCppPathNode = resourcePath(os.path.join('Templates', 'MapTemplateCppNode.txt'))
  memMapNodeDir = os.path.join(nodeDirectory, 'Maps', ('Map' + memMapNameCamel))
  memMapHubDir  = os.path.join(hubDirectory,  'Maps', ('Map' + memMapNameCamel))

  try:
    dataBlockNames = pandas.ExcelFile(memoryMapXlsxPath).sheet_names 
    for dataBlockName in dataBlockNames:
      dataBlockNameCamel = dataBlockName.replace(" ", "").lower().capitalize()
      dataBlockNamesCamel.append(dataBlockNameCamel)
      newBlock = pandas.read_excel(memoryMapXlsxPath, sheet_name=dataBlockName)
      dataBlocks.append(newBlock)
  except:
    return (Error.FILE_OPEN_FAILED)
  
  try:
    pathlib.Path(memMapNodeDir).mkdir(parents=False, exist_ok=True)
    pathlib.Path(memMapHubDir).mkdir(parents=False, exist_ok=True)
  except:
    return (Error.MAKE_DIRECTORY_FAILED)
  
  for block in dataBlocks:
    varIDs     = block["Var ID"]
    types      = block["Data Type"]
    access     = block["External Access"]
    nvmStorage = block["NVM Storage"]
    if varIDs.isnull().any():
       return (Error.EMPTY_VAR_ID_CELL)
    if types.isnull().any():
       return (Error.EMPTY_DATA_TYPE_CELL)
    if access.isnull().any():
       return (Error.EMPTY_ACCESS_CELL)
    if nvmStorage.isnull().any():
       return (Error.EMPTY_NVM_STORAGE_CELL)

  mapNumberOfVars = NUMBER_OF_UNIVERSAL_VARS

  for block in dataBlocks:
    blockNumberOfVars = len(block["Var ID"])
    mapNumberOfVars += blockNumberOfVars

  mapHppPathNode = os.path.join(memMapNodeDir, memMapHppName)
  mapCppPathNode = os.path.join(memMapNodeDir, memMapCppName)
  mapHppPathHub  = os.path.join(memMapHubDir,  memMapHppName)
  mapCppPathHub  = os.path.join(memMapHubDir,  memMapCppName)

  mapTemplateHppNode = open(mapTemplateHppPathNode, OpenMethods.READ_ONLY)
  mapHppNode         = open(mapHppPathNode,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlocks, 
                        mapTemplateHppNode, 
                        mapHppNode,
                        mapNumberOfVars,
                        timeStamp)
  mapHppNode.close()
  mapTemplateHppNode.close()

  mapTemplateHppHub = open(mapTemplateHppPathHub, OpenMethods.READ_ONLY)
  mapHppHub         = open(mapHppPathHub,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlocks, 
                        mapTemplateHppHub, 
                        mapHppHub,
                        mapNumberOfVars,
                        timeStamp)
  mapHppHub.close()
  mapTemplateHppHub.close()
  
  mapTemplateCppNode = open(mapTemplateCppPathNode, OpenMethods.READ_ONLY)
  mapCppNode         = open(mapCppPathNode,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNameCamel,
                        dataBlockNamesCamel,
                        dataBlocks,
                        mapTemplateCppNode,
                        mapCppNode,
                        mapNumberOfVars,
                        timeStamp)
  mapCppNode.close()
  mapTemplateCppNode.close()

  mapTemplateCppHub = open(mapTemplateCppPathHub, OpenMethods.READ_ONLY)
  mapCppHub         = open(mapCppPathHub,         OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(memMapNameCamel,
                        dataBlockNamesCamel,
                        dataBlocks,
                        mapTemplateCppHub,
                        mapCppHub,
                        mapNumberOfVars,
                        timeStamp)
  mapCppHub.close()
  mapTemplateCppHub.close()

  blockIterator      = 0
  cumulativeVarIndex = NUMBER_OF_UNIVERSAL_VARS
  blockHppTemplate   = open(blockHppTemplatePath, OpenMethods.READ_ONLY)
  for dataBlock in dataBlocks:

    blockHppTemplate.seek(0)
    blockHppName     = "Block" + dataBlockNamesCamel[blockIterator] + ".hpp"

    blockHppPathNode = os.path.join(memMapNodeDir, blockHppName)
    blockHppNode     = open(blockHppPathNode, OpenMethods.WRITE_FORCE)
    generateDataBlockFile(memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlock,
                          cumulativeVarIndex,
                          blockHppTemplate,
                          blockHppNode)
    blockHppNode.close()

    blockHppTemplate.seek(0)

    blockHppPathHub = os.path.join(memMapHubDir, blockHppName)
    blockHppHub     = open(blockHppPathHub, OpenMethods.WRITE_FORCE)
    generateDataBlockFile(memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlock,
                          cumulativeVarIndex,
                          blockHppTemplate,
                          blockHppHub)
    blockHppHub.close()

    blockIterator      += 1
    cumulativeVarIndex += len(dataBlock["Var ID"])

  return (Error.NONE)



