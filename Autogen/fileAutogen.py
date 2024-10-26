import csv
import sys
import os
import pandas
from   enum   import Enum
from   enum   import StrEnum
from   typing import List

FRAMEWORK_NAME = "Atams"

class Platforms(Enum):
  NODE = 0
  HUB  = 1

class OpenMethods(StrEnum):
  READ_ONLY   = 'r'
  WRITE_TRY   = 'x'
  WRITE_FORCE = 'w' 

class Error(StrEnum):
  NONE                = "File Generation Successful!"
  FILE_OPEN_FAILED    = "Generation Error: Failed to Open Files"
  INAVLID_FILE_FORMAT = "Generation Error: Invalid Memory Map Xlsx Format"

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
                 prefixString, 
                 stringList, 
                 targetFile):
  iterator = iteratorStartValue
  requiredSpace = getLongestString(stringList)
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
      print(value)
      targetFile.write("inline constexpr " + type + " ")
      writeSpaces(preNameSpaces, targetFile)
      targetFile.write((columnHeader.replace(' ', '_').upper()) + "_" + memberName)
      writeSpaces(postNameSpaces, targetFile)
      suffix = getSuffixString(type)
      targetFile.write(" = " + value)
      listItemNoSignNoPoint = value.replace('.', '')
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
  targetFile.write("constexpr DataBlock::BlockDescriptor_t blockDescriptor =\n{\n")
  targetFile.write("  /* .noOfDataMembers = */ Block"+blockNameCamel+"::NUMBER_OF_"+blockNameUpper+"_DATA_MEMBERS,\n")
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
    targetFile.write("    [Block"+blockNameCamel+"::MEMBER_ID_"+memberName+"] =\n    {\n")
    targetFile.write("      /* .type           = */ TYPE_"+typeUpper+",\n")
    targetFile.write("      /* .externalAccess = */ ACCESS_"+accessString+",\n")
    if (platformNameCamel == "Node"):
      targetFile.write("      /* .NVMStorage     = */ "+NVMStorage)
      targetFile.write("\n")
    targetFile.write("    },\n")
    memberIterator += 1

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
    generateEnum(0, "  MEMBER_ID_", memberIDsUpper, targetFile)
    targetFile.write("  NUMBER_OF_" + blockNameUpper + "_DATA_MEMBERS\n")
    targetFile.write("} DataMemberID_t;\n\n")
    targetFile.write("/*--- Defaults ---*/\n")
    generateConstList(block, memberIDsUpper, "Default", targetFile)
    targetFile.write("\n/*--- Minimum Limits ---*/\n")
    generateConstList(block, memberIDsUpper, "Min Limit", targetFile)
    targetFile.write("\n/*--- Maximum Limits ---*/\n")
    generateConstList(block, memberIDsUpper, "Max Limit", targetFile)
    targetFile.write("\n/*--- Descriptor ---*/\n")
    generateBlockDescriptor(platformNameCamel, blockNameCamel, blockNameUpper, block, memberIDsUpper, targetFile)
    targetFile.write("  }\n};\n\n")
    targetFile.write("} /* End Namespace - Block"+blockNameCamel+" */\n\n")

def generateBlockArray(dataBlockNamesCamel, targetFile):
  targetFile.write("{")
  noOfBlocks = len(dataBlockNamesCamel)
  iterator   = 0
  if (noOfBlocks > 0):
    targetFile.write("Block" + dataBlockNamesCamel[0] + "::blockDescriptor,\n")
    iterator += 1
  while (iterator < noOfBlocks):
    targetFile.write("                             Block" + dataBlockNamesCamel[iterator] + "::blockDescriptor")
    if (iterator == (noOfBlocks - 1)):
      targetFile.write(" }")
    else:
      targetFile.write(",\n")
    iterator += 1

def generateInitDefaultsDefinition(platformNameCamel,
                                   dataBlockNamesCamel,
                                   dataBlocks,
                                   targetFile):
  if (platformNameCamel != "Node"):
    return
  blockIterator = 0
  targetFile.write("Error_t initDefaults(void)\n{\n")
  targetFile.write("  Error_t initStatus = BlockUniversal::initDefaults();\n\n")

  universalMembersToSet    = ["ATAMS_VERSION_NUMBER",
                              "MAP_GEN_DAY",    
                              "MAP_GEN_MONTH",
                              "MAP_GEN_YEAR",  
                              "MAP_GEN_HOUR", 
                              "MAP_GEN_MINUTE", 
                              "MAP_GEN_SECOND",
                              "MAP_NUMBER_OF_BLOCKS",
                              "MAP_CHECKSUM"]  

  for memberString in universalMembersToSet:
    targetFile.write("  if (initStatus == ERROR_NONE) initStatus = write(BLOCK_ID_UNIVERSAL,\n")
    targetFile.write("                                                   BlockUniversal::MEMBER_ID_"+memberString+",\n")
    targetFile.write("                                                   AUTOGEN_"+memberString+");\n\n")

  for block in dataBlocks:
    memberIDsUpper = []
    for memberID in block["Member ID"]:
      memberIDsUpper.append(memberID.replace(" ", "_").upper())
    blockNameCamel = dataBlockNamesCamel[blockIterator]
    blockNameUpper = blockNameCamel.upper()
    defaults = block["Default"]
    memberIterator = 0
    for memberIDUpper in memberIDsUpper:
      default = defaults[memberIterator]
      if (default and (default != "-")):
        targetFile.write("  if (initStatus == ERROR_NONE) initStatus = write(")
        targetFile.write("BLOCK_ID_" +blockNameUpper+",\n")
        targetFile.write("                                                   Block"+blockNameCamel+"::MEMBER_ID_" + memberIDUpper+",\n")
        targetFile.write("                                                   Block"+blockNameCamel+"::DEFAULT_"   + memberIDUpper+");\n\n")
      memberIterator += 1
    blockIterator += 1

  targetFile.write("""  return (initStatus); \n}""")
  
def generateInitLimitsDefinition(platformNameCamel,
                                 dataBlockNamesCamel,
                                 dataBlocks,
                                 targetFile):
  if (platformNameCamel != "Node"):
    return
  blockIterator = 0
  targetFile.write("Error_t initLimits(void)\n{\n")
  targetFile.write("  Error_t initStatus = BlockUniversal::initLimits();\n\n")

  for block in dataBlocks:
    memberIDsUpper = []
    for memberID in block["Member ID"]:
      memberIDsUpper.append(memberID.replace(" ", "_").upper())
    blockNameCamel = dataBlockNamesCamel[blockIterator]
    blockNameUpper = blockNameCamel.upper()
    minLimits = block["Min Limit"]
    maxLimits = block["Max Limit"]
    memberIterator = 0
    for memberIDUpper in memberIDsUpper:
      minLimit = minLimits[memberIterator]
      maxLimit = maxLimits[memberIterator]
      if (minLimit and (minLimit != "-") and 
          maxLimit and (maxLimit != "-") ):
        targetFile.write("  if (initStatus == ERROR_NONE) initStatus = assertLimits(")
        targetFile.write("BLOCK_ID_" +blockNameUpper+",\n")
        targetFile.write("                                                          Block"+blockNameCamel+"::MEMBER_ID_" + memberIDUpper+",\n")
        targetFile.write("                                                          Block"+blockNameCamel+"::MAX_LIMIT_" + memberIDUpper+",\n")
        targetFile.write("                                                          Block"+blockNameCamel+"::MIN_LIMIT_" + memberIDUpper+");\n\n")
      memberIterator += 1
    blockIterator += 1

  targetFile.write("""  return (initStatus); \n}""")

def autogenCall(platformNameCamel: str,
                autogenHint: str,
                memMapNameCamel: str,
                dataBlockNamesCamel, 
                dataBlockNamesUpper, 
                dataBlocks,
                targetFile):
  match (autogenHint):
    case "PLAT_NAME_CAMEL":
      targetFile.write(platformNameCamel)
    case "MAP_NAME_CAMEL":
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_ID_LIST":
      generateEnum(1, "  BLOCK_ID_", dataBlockNamesUpper, targetFile)
    case "INIT_DEFAULTS_DECLARATION":
      match (platformNameCamel):
        case "Node":
          targetFile.write("Error_t initDefaults(void);")
        case "Hub":
          targetFile.write("Error_t initDefaults(Node &nodeToInit);")
    case "INIT_LIMITS_DECLARATION":
      match (platformNameCamel):
        case "Node":
          targetFile.write("Error_t initLimits(void);")
        case "Hub":
          targetFile.write("Error_t initLimits(Node &nodeToInit);")
    #case "DATA_BLOCK_DEFINITIONS":
    #  generateBlockDefinitions(platformNameCamel, 
    #                           dataBlockNamesCamel,
    #                           dataBlocks,
    #                           targetFile)
    case "DATA_BLOCK_ARRAY":
      generateBlockArray(dataBlockNamesCamel, targetFile)
    case "INIT_DEFAULTS_DEFINITION":
      generateInitDefaultsDefinition(platformNameCamel,
                                     dataBlockNamesCamel,
                                     dataBlocks,
                                     targetFile)
    case "INIT_LIMITS_DEFINITION":
      generateInitLimitsDefinition(platformNameCamel,
                                   dataBlockNamesCamel,
                                   dataBlocks,
                                   targetFile)
    case "VERSION_NUMBER":
      targetFile.write("0.1" + "F")
    case "GENERATION_DAY":
      targetFile.write("15"+"U")
    case "GENERATION_MONTH":
      targetFile.write("10"+"U")
    case "GENERATION_YEAR":
      targetFile.write("2024"+"U")
    case "GENERATION_HOUR":
      targetFile.write("11"+"U")
    case "GENERATION_MINUTE":
      targetFile.write("33"+"U")
    case "GENERATION_SECOND":
      targetFile.write("20"+"U")
    case "GENERATION_CHECKSUM":
      targetFile.write("32457"+"U")


def generateMemoryMapFile(platformNameCamel: str,
                          memMapNameCamel: str,
                          dataBlockNamesCamel, 
                          dataBlockNamesUpper, 
                          dataBlocks, 
                          templateFile,
                          targetFile):
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
      autogenCall(platformNameCamel,
                  string,
                  memMapNameCamel,
                  dataBlockNamesCamel, 
                  dataBlockNamesUpper, 
                  dataBlocks,
                  targetFile)
      nextStringAutogenCall = False
    else:
      targetFile.write(string)

def generateCppFiles(memMapNameCamel: str, memoryMapXlsxPath: str, nodeDirectory: str, hubDirectory: str):

  memMapHppName = "MemoryMap" + memMapNameCamel + ".hpp"
  memMapCppName = "MemoryMap" + memMapNameCamel + ".cpp"
  dataBlocks          = []
  dataBlockNamesCamel = []
  dataBlockNamesUpper = []
  memMapTemplateHppPath = os.path.join(os.path.dirname(__file__), 'MemoryMapAutogen.hpp')
  memMapTemplateCppPath = os.path.join(os.path.dirname(__file__), 'MemoryMapAutogen.cpp')

  try:
    dataBlockNames = pandas.ExcelFile(memoryMapXlsxPath).sheet_names 
    for dataBlockName in dataBlockNames:
      dataBlockNamesCamel.append(dataBlockName.replace(" ", "").lower().capitalize())
      dataBlockNamesUpper.append(dataBlockName.replace(" ", "_").upper())
      newBlock = pandas.read_excel(memoryMapXlsxPath, sheet_name=dataBlockName)
      dataBlocks.append(newBlock)
  except:
    return (Error.FILE_OPEN_FAILED)

  nodeMemMapHppPath = os.path.join(nodeDirectory, "Devices", memMapHppName)
  nodeMemMapCppPath = os.path.join(nodeDirectory, "Devices", memMapCppName)
  hubMemMapHppPath  = os.path.join(hubDirectory,  "Devices", memMapHppName)
  hubMemMapCppPath  = os.path.join(hubDirectory,  "Devices", memMapCppName)

  memMapTemplateHpp = open(memMapTemplateHppPath, OpenMethods.READ_ONLY)
  nodeMemMapHpp     = open(nodeMemMapHppPath,     OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Node",
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        nodeMemMapHpp)
  nodeMemMapHpp.close()
  hubMemMapHpp = open(hubMemMapHppPath, OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Hub",
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        hubMemMapHpp)
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
                        nodeMemMapCpp)
  nodeMemMapCpp.close()
  hubMemMapCpp = open(hubMemMapCppPath, OpenMethods.WRITE_FORCE)
  generateMemoryMapFile("Hub",
                        memMapNameCamel,
                        dataBlockNamesCamel,
                        dataBlockNamesUpper,
                        dataBlocks,
                        memMapTemplateCpp,
                        hubMemMapCpp)
  hubMemMapCpp.close()
  memMapTemplateCpp.close()

  return (Error.NONE)



