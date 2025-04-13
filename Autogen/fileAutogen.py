import csv
import sys
import os
import pathlib
import pandas
from   enum     import Enum
from   enum     import StrEnum
from   typing   import List
from   datetime import datetime
from   crc      import Calculator, Crc32

FRAMEWORK_NAME = "Atams"

class Platforms(Enum):
  NODE = 0
  HUB  = 1

class OpenMethods(StrEnum):
  READ_ONLY   = 'r'
  WRITE_TRY   = 'x'
  WRITE_FORCE = 'w' 

class Error(StrEnum):
  NONE                  = "File Generation Successful!"
  MAKE_DIRECTORY_FAILED = "Generation Error: Failed to Make Directory"
  FILE_OPEN_FAILED      = "Generation Error: Failed to Open Files"
  INAVLID_FILE_FORMAT   = "Generation Error: Invalid Memory Map Xlsx Format"

def getLongestString(strings: List[str]) -> int:
  maxStringLength = 0
  for string in strings:
    if (len(string) > maxStringLength):
      maxStringLength = len(string)
  return (maxStringLength)

def writeSpaces(noOfSpaces: int, targetFile):
  while (noOfSpaces > 0):
    targetFile.write(" ")
    noOfSpaces -= 1

def getSuffixString(typeString: str):
  typeStrings   = ["uint8_t", "int8_t", "uint16_t", "int16_t", "uint32_t", "int32_t", "float"]
  suffixStrings = ["U",       "",       "U",        "",        "UL",       "L",       "F"    ]
  try:    suffixIndex = typeStrings.index(typeString)
  except: return("")
  return (suffixStrings[suffixIndex])

def generateEnum(iteratorStartValue,
                 minStringLength,
                 prefixString, 
                 stringList, 
                 targetFile):
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

def generateConstList(block, memberNames:str, columnHeader: str, targetFile):
  memberIterator          = 0
  memberNamesWithValue    = []
  types                   = block["Data Type"]
  values                  = block[columnHeader]
  preStringRequiredSpace  = getLongestString(types)
  for memberName in memberNames:
    value = values[memberIterator]
    if (value and (value != "-")):
      memberNamesWithValue.append(memberName)
    memberIterator += 1
  memberIterator = 0
  postStringRequiredSpace = getLongestString(memberNamesWithValue)
  for memberName in memberNames:
    type           = types[memberIterator]
    value          = values[memberIterator]
    preNameSpaces  = preStringRequiredSpace  - len(type)
    postNameSpaces = postStringRequiredSpace - len(memberName)
    if (value and (value != "-")):
      valueAsString = str(value)
      targetFile.write("inline constexpr " + type + " ")
      writeSpaces(preNameSpaces, targetFile)
      targetFile.write((columnHeader.replace(' ', '_').upper()) + "_" + memberName)
      writeSpaces(postNameSpaces, targetFile)
      suffix = getSuffixString(type)
      targetFile.write(" = " + valueAsString)
      if (("." not in valueAsString) and 
          (type == "float"         ) ):
        targetFile.write(".0")
      listItemNoSignNoPoint = valueAsString.replace('.', '')
      listItemNoSignNoPoint = listItemNoSignNoPoint.replace('-', '')
      if (listItemNoSignNoPoint.isnumeric()):
        targetFile.write(suffix)
      targetFile.write(";\n")
    memberIterator += 1

def generateBlockDescriptor(platformNameCamel: str, 
                            blockNameCamel,
                            blockNameUpper,
                            block,
                            memberNamesUpper,
                            targetFile):
  memberIterator = 0
  targetFile.write("const DataBlock::BlockDescriptor_t blockDescriptor =\n{\n")
  targetFile.write("  /* .noOfDataMembers = */ Block"+blockNameCamel+"::NUMBER_OF_"+blockNameUpper+"_VARS,\n")
  targetFile.write("  /* .genInfo         = */ genInfo, \n")
  targetFile.write("  /* .initDefaults    = */ initDefaults, \n")
  targetFile.write("  /* .dataMemberInfo  = */\n  {\n")
  types        = block["Data Type"]
  accessLevels = block["External Access"]
  NVMStorages  = block["NVM Storage"]
  
  for memberName in memberNamesUpper:
    access     = accessLevels[memberIterator]
    NVMStorage = NVMStorages[memberIterator]
    accessString = ""
    if (NVMStorage.upper() == "YES"):
      NVMStorage = "true"
    else:
      NVMStorage = "false"
    if (access == "RW"): accessString = "WRITE"
    else:                accessString = "READ"
    typeUpper = types[memberIterator].replace("_t", "").upper()
    targetFile.write("    /* [Block"+blockNameCamel+"::VAR_ID_"+memberName+"] = */\n    {\n")
    targetFile.write("      /* .type           = */ TYPE_"+typeUpper+",\n")
    targetFile.write("      /* .externalAccess = */ ACCESS_"+accessString+",\n")
    if (platformNameCamel == "Node"):
      targetFile.write("      /* .NVMStorage     = */ "+NVMStorage+",\n")
    targetFile.write("    },\n")
    memberIterator += 1
  targetFile.write("  }\n};")

def generateBlockDefinitions(platformNameCamel: str, 
                             dataBlockNamesCamel,
                             dataBlocks,
                             targetFile):
  blockIterator = 0
  for blockNameCamel in dataBlockNamesCamel:
    block          = dataBlocks[blockIterator]
    blockNameUpper = blockNameCamel.upper()
    memberIDsUpper = []
    for memberID in block["Member ID"]:
      memberIDsUpper.append(memberID.replace(" ", "_").upper())
    targetFile.write("/*--- DATA BLOCK " + blockNameUpper + " -----------------------------------------------------------*/\n")
    targetFile.write("namespace Block" + blockNameCamel + " {\n\n")
    targetFile.write("/*--- Member List ---*/\n")
    targetFile.write("typedef enum: uint16_t\n{\n")
    generateEnum(0, 0, "  VAR_ID_", memberIDsUpper, targetFile)
    targetFile.write("  NUMBER_OF_" + blockNameUpper + "_VARS\n")
    targetFile.write("} DataMemberID_t;\n\n")
    targetFile.write("/*--- Defaults ---*/\n")
    generateConstList(block, memberIDsUpper, "Default", targetFile)
    targetFile.write("/*--- Descriptor ---*/\n")
    generateBlockDescriptor(platformNameCamel, blockNameCamel, blockNameUpper, block, memberIDsUpper, targetFile)

def generateBlockRefArray(dataBlockNamesCamel, targetFile):
  noOfBlocks = len(dataBlockNamesCamel)
  iterator   = 0
  while (iterator < noOfBlocks):
    targetFile.write("  &Block"+dataBlockNamesCamel[iterator]+"::blockDescriptor")
    if (iterator != (noOfBlocks - 1)):
      targetFile.write(",\n")
    iterator += 1

def generateInitUniversalMapInfo(platformNameCamel,
                                 targetFile):

  universalMembersToSet = ["ATAMS_VERSION_MAJOR",
                           "ATAMS_VERSION_MINOR",
                           "MAP_GEN_DAY",    
                           "MAP_GEN_MONTH",
                           "MAP_GEN_YEAR",  
                           "MAP_GEN_HOUR", 
                           "MAP_GEN_MINUTE", 
                           "MAP_GEN_SECOND",
                           "MAP_CHECKSUM"]  
  
  variableNames = ["atamsVersionMajor",
                   "atamsVersionMinor",
                   "genDay",    
                   "genMonth",
                   "genYear",  
                   "genHour", 
                   "genMinute", 
                   "genSecond",
                   "genChecksum"]  

  varIterator = 0

  if (platformNameCamel == "Hub"):
    for memberString in universalMembersToSet:
      variableName = variableNames[varIterator]
      targetFile.write("  if (initStatus == Atams::ERROR_NONE) initStatus = nodeToInit.write(BLOCK_ID_UNIVERSAL,\n")
      targetFile.write("                                                                     BlockUniversal::VAR_ID_"+memberString+",\n")
      targetFile.write("                                                                     genInfo."+variableName+");\n\n")
      varIterator += 1
  else:
    for memberString in universalMembersToSet:
      variableName = variableNames[varIterator]
      targetFile.write("  if (initStatus == Atams::ERROR_NONE) initStatus = Atams::write(BLOCK_ID_UNIVERSAL,\n")
      targetFile.write("                                                                 BlockUniversal::VAR_ID_"+memberString+",\n")
      targetFile.write("                                                                 genInfo."+variableName+");\n\n")
      varIterator += 1
  targetFile.seek(targetFile.tell()-1)

def generateMapDefinition(platformNameCamel,
                          targetFile):
  if (platformNameCamel == "Hub"):
    targetFile.write("Node::MemoryMap_t memoryMap(static_cast<uint8_t>(NUMBER_OF_DATA_BLOCKS),\n")     
    targetFile.write("                                  genInfo,\n")
    targetFile.write("                                  initUniversalInfo,\n")
    targetFile.write("                                  blockDescriptors);\n")
  else:
    targetFile.write("MemoryMap_t memoryMap(static_cast<uint8_t>(NUMBER_OF_DATA_BLOCKS),\n")     
    targetFile.write("                            genInfo,\n")
    targetFile.write("                            initUniversalInfo,\n")
    targetFile.write("                            blockDescriptors);\n")

def generateInitDefaultsDefinition(blockNameCamel, block, memberIDsUpper, targetFile):
  memberIterator      = 0
  membersWithDefaults = []
  defaults            = block["Default"]

  for memberID in memberIDsUpper:
    default = defaults[memberIterator]
    if (default and (default != "-")):
      membersWithDefaults.append(memberID)
    memberIterator += 1

  for memberID in membersWithDefaults:
    targetFile.write("  if (initStatus == Atams::ERROR_NONE) initStatus = blockToInit.write(Block"+blockNameCamel+"::VAR_ID_"+memberID+",\n")
    targetFile.write("                                                                      Block"+blockNameCamel+"::DEFAULT_"+memberID+");\n\n")
  targetFile.seek(targetFile.tell()-1)

def generateBlockChecksum(block):
  blockChecksumString = ""
  calculator = Calculator(Crc32.CRC32)
  IDs            = block["Member ID"]
  types          = block["Data Type"]
  accessLevels   = block["External Access"]
  defaults       = block["Default"]
  NVMStorages    = block["NVM Storage"]
  varID = 0
  for ID in IDs:
    blockChecksumString += str(types[varID])
    blockChecksumString += str(accessLevels[varID])
    blockChecksumString += str(defaults[varID])
    blockChecksumString += str(NVMStorages[varID])
    varID     += 1
  bytes = blockChecksumString.encode('utf-8')
  return (calculator.checksum(bytes))

def generateMapChecksum(dataBlocks):
  mapChecksumString = ""
  calculator = Calculator(Crc32.CRC32)
  for block in dataBlocks:
    IDs            = block["Member ID"]
    types          = block["Data Type"]
    accessLevels   = block["External Access"]
    defaults       = block["Default"]
    NVMStorages    = block["NVM Storage"]
    varID = 0
    for ID in IDs:
      mapChecksumString += str(types[varID])
      mapChecksumString += str(accessLevels[varID])
      mapChecksumString += str(defaults[varID])
      mapChecksumString += str(NVMStorages[varID])
      varID     += 1
  bytes = mapChecksumString.encode('utf-8')
  return (calculator.checksum(bytes))

def autogenCallMap(platformNameCamel: str,
                   autogenHint: str,
                   memMapNameCamel: str,
                   dataBlockNamesCamel, 
                   dataBlockNamesUpper, 
                   dataBlocks,
                   targetFile,
                   timeStamp):
  match (autogenHint):
    case "PLAT_NAME_CAMEL":
      targetFile.write(platformNameCamel)
    case "MAP_NAME_CAMEL":
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_ID_LIST":
      generateEnum(1, len("UNIVERSAL"), "  BLOCK_ID_", dataBlockNamesUpper, targetFile)
    case "NODE_NAMESPACE":
      if (platformNameCamel == "Hub"): targetFile.write("Node::")
    case "INIT_MAP_UNIVERSAL_INFO_DEFINITION":
      generateInitUniversalMapInfo(platformNameCamel, targetFile)
    case "DATA_BLOCK_DEFINITIONS":
      generateBlockDefinitions(platformNameCamel, 
                               dataBlockNamesCamel,
                               dataBlocks,
                               targetFile)
    case "DATA_BLOCK_FILE_INCLUDES":
      for blockName in dataBlockNamesCamel:
        targetFile.write('#include "Block' + blockName + '.hpp"\n')
    case "BLOCK_DESC_REFERENCES":
      generateBlockRefArray(dataBlockNamesCamel, targetFile)
    case "NODE_INIT_REFERENCE":
      if (platformNameCamel == "Hub"): targetFile.write("Node &nodeToInit")
      else:                            targetFile.write("void")
    case "MEMORY_MAP_DEFINITION":
      generateMapDefinition(platformNameCamel, targetFile)
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

def generateMemoryMapFile(platformNameCamel: str,
                          memMapNameCamel: str,
                          dataBlockNamesCamel, 
                          dataBlockNamesUpper, 
                          dataBlocks, 
                          templateFile,
                          targetFile,
                          timeStamp):
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
      autogenCallMap(platformNameCamel,
                     string,
                     memMapNameCamel,
                     dataBlockNamesCamel, 
                     dataBlockNamesUpper, 
                     dataBlocks,
                     targetFile,
                     timeStamp)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)
      
def autogenCallBlock(platformNameCamel: str,
                     autogenHint: str,
                     memMapNameCamel: str,
                     blockNameCamel: str,
                     blockNameUpper: str,
                     dataBlock,
                     targetFile,
                     timeStamp):
  memberIDsUpper = []
  for memberID in dataBlock["Member ID"]:
    memberIDsUpper.append(memberID.replace(" ", "_").upper())

  match (autogenHint):
    case "PLAT_NAME_CAMEL":
      targetFile.write(platformNameCamel)
    case "MAP_NAME_CAMEL":
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_NAME_CAMEL":
      targetFile.write(blockNameCamel)
    case "BLOCK_NAME_UPPER":
      targetFile.write(blockNameCamel.upper())
    case "MEMBER_ID_LIST":
      generateEnum(0, 0, "  VAR_ID_", memberIDsUpper, targetFile)
    case "DEFAULTS":
      generateConstList(dataBlock, memberIDsUpper, "Default", targetFile)
    case "INIT_DEFAULTS_DEFINITION":
      generateInitDefaultsDefinition(blockNameCamel, dataBlock, memberIDsUpper, targetFile)
    case "BLOCK_DESCRIPTOR":
      generateBlockDescriptor(platformNameCamel, blockNameCamel, blockNameUpper, dataBlock, memberIDsUpper, targetFile)
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
      targetFile.write(str(generateBlockChecksum(dataBlock)) + "U")

def generateDataBlockFile(platformNameCamel: str,
                          memMapNameCamel: str,
                          blockNameCamel: str,
                          blockNameUpper: str,
                          dataBlock, 
                          templateFile,
                          targetFile,
                          timestamp):
  inputFileString = templateFile.read()
  splitStrings    = inputFileString.split("$$$")
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
      autogenCallBlock(platformNameCamel,
                       string,
                       memMapNameCamel,
                       blockNameCamel,
                       blockNameUpper,
                       dataBlock,
                       targetFile,
                       timestamp)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)

def generateCppFiles(memMapNameCamel: str, memoryMapXlsxPath: str, nodeDirectory: str, hubDirectory: str):

  timeStamp = datetime.now()

  memMapHppName = "Map" + memMapNameCamel + ".hpp"
  memMapCppName = "Map" + memMapNameCamel + ".cpp"
  dataBlocks          = []
  dataBlockNamesCamel = []
  dataBlockNamesUpper = []
  dataBlockHppTemplatePath = os.path.join(os.path.dirname(__file__), 'DataBlockAutogenTemplateHpp.txt')
  dataBlockCppTemplatePath = os.path.join(os.path.dirname(__file__), 'DataBlockAutogenTemplateCpp.txt')
  memMapTemplateHppPath = os.path.join(os.path.dirname(__file__), 'MemoryMapAutogenTemplateHpp.txt')
  memMapTemplateCppPath = os.path.join(os.path.dirname(__file__), 'MemoryMapAutogenTemplateCpp.txt')
  memMapNodeDir = os.path.join(nodeDirectory, 'Maps', ('MemoryMap' + memMapNameCamel))
  memMapHubDir  = os.path.join(hubDirectory,  'Maps', ('MemoryMap' + memMapNameCamel))

  try:
    dataBlockNames = pandas.ExcelFile(memoryMapXlsxPath).sheet_names 
    for dataBlockName in dataBlockNames:
      dataBlockNameCamel = dataBlockName.replace(" ", "").lower().capitalize()
      dataBlockNameUpper = dataBlockName.replace(" ", "_").upper()
      dataBlockNamesCamel.append(dataBlockNameCamel)
      dataBlockNamesUpper.append(dataBlockNameUpper)
      newBlock = pandas.read_excel(memoryMapXlsxPath, sheet_name=dataBlockName)
      dataBlocks.append(newBlock)
  except:
    return (Error.FILE_OPEN_FAILED)
  
  try:
    pathlib.Path(memMapNodeDir).mkdir(parents=False, exist_ok=True)
    pathlib.Path(memMapHubDir).mkdir(parents=False, exist_ok=True)
  except:
    return (Error.MAKE_DIRECTORY_FAILED)

  nodeMemMapHppPath = os.path.join(memMapNodeDir, memMapHppName)
  nodeMemMapCppPath = os.path.join(memMapNodeDir, memMapCppName)
  hubMemMapHppPath  = os.path.join(memMapHubDir,  memMapHppName)
  hubMemMapCppPath  = os.path.join(memMapHubDir,  memMapCppName)

  memMapTemplateHpp = open(memMapTemplateHppPath, OpenMethods.READ_ONLY)
  nodeMemMapHpp     = open(nodeMemMapHppPath,     OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Node",
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        nodeMemMapHpp,
                        timeStamp)
  nodeMemMapHpp.close()
  memMapTemplateHpp.seek(0)
  hubMemMapHpp = open(hubMemMapHppPath, OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Hub",
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        hubMemMapHpp,
                        timeStamp)
  hubMemMapHpp.close()
  memMapTemplateHpp.close()
  
  memMapTemplateCpp = open(memMapTemplateCppPath, OpenMethods.READ_ONLY)
  nodeMemMapCpp     = open(nodeMemMapCppPath,     OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Node",
                        memMapNameCamel,
                        dataBlockNamesCamel,
                        dataBlockNamesUpper,
                        dataBlocks,
                        memMapTemplateCpp,
                        nodeMemMapCpp,
                        timeStamp)
  nodeMemMapCpp.close()
  memMapTemplateCpp.seek(0)
  hubMemMapCpp = open(hubMemMapCppPath, OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Hub",
                        memMapNameCamel,
                        dataBlockNamesCamel,
                        dataBlockNamesUpper,
                        dataBlocks,
                        memMapTemplateCpp,
                        hubMemMapCpp,
                        timeStamp)
  hubMemMapCpp.close()
  memMapTemplateCpp.close()

  blockIterator = 0
  dataBlockHppTemplate = open(dataBlockHppTemplatePath, OpenMethods.READ_ONLY)
  dataBlockCppTemplate = open(dataBlockCppTemplatePath, OpenMethods.READ_ONLY)
  for dataBlock in dataBlocks:
    dataBlockHppTemplate.seek(0)
    dataBlockCppTemplate.seek(0)
    
    dataBlockHppName = "Block" + dataBlockNamesCamel[blockIterator] + ".hpp"
    dataBlockHppNodePath = os.path.join(memMapNodeDir, dataBlockHppName)
    dataBlockNodeHpp = open(dataBlockHppNodePath, OpenMethods.WRITE_FORCE)
    generateDataBlockFile("Node",
                          memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlockNamesUpper[blockIterator],
                          dataBlock,
                          dataBlockHppTemplate,
                          dataBlockNodeHpp,
                          timeStamp)
    dataBlockNodeHpp.close()

    dataBlockCppName = "Block" + dataBlockNamesCamel[blockIterator] + ".cpp"
    dataBlockCppNodePath = os.path.join(memMapNodeDir, dataBlockCppName)
    dataBlockNodeCpp = open(dataBlockCppNodePath, OpenMethods.WRITE_FORCE)
    generateDataBlockFile("Node",
                          memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlockNamesUpper[blockIterator],
                          dataBlock,
                          dataBlockCppTemplate,
                          dataBlockNodeCpp,
                          timeStamp)
    dataBlockNodeCpp.close()

    dataBlockHppTemplate.seek(0)
    dataBlockCppTemplate.seek(0)

    dataBlockHppName = "Block" + dataBlockNamesCamel[blockIterator] + ".hpp"
    dataBlockHppHubPath = os.path.join(memMapHubDir, dataBlockHppName)
    dataBlockHubHpp = open(dataBlockHppHubPath, OpenMethods.WRITE_FORCE)
    generateDataBlockFile("Hub",
                          memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlockNamesUpper[blockIterator],
                          dataBlock,
                          dataBlockHppTemplate,
                          dataBlockHubHpp,
                          timeStamp)
    dataBlockHubHpp.close()

    dataBlockCppName = "Block" + dataBlockNamesCamel[blockIterator] + ".cpp"
    dataBlockCppHubPath = os.path.join(memMapHubDir, dataBlockCppName)
    dataBlockHubCpp = open(dataBlockCppHubPath, OpenMethods.WRITE_FORCE)
    generateDataBlockFile("Hub",
                          memMapNameCamel,
                          dataBlockNamesCamel[blockIterator],
                          dataBlockNamesUpper[blockIterator],
                          dataBlock,
                          dataBlockCppTemplate,
                          dataBlockHubCpp,
                          timeStamp)
    dataBlockHubCpp.close()
    blockIterator += 1

  return (Error.NONE)



