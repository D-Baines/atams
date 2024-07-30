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

def generateEnum(prefixString, 
                 stringList, 
                 targetFile):
  iterator = 0
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
    if ((value != "-") and (value != "")):
      memberNamesWithValue.append(memberName)
    memberIterator += 1
  memberIterator = 0
  postStringRequiredSpace = getLongestString(memberNamesWithValue)
  for memberName in memberNames:
    type           = types[memberIterator]
    value          = values[memberIterator]
    preNameSpaces  = preStringRequiredSpace  - len(type)
    postNameSpaces = postStringRequiredSpace - len(memberName)
    if ((value != "-") and (value != "")):
      targetFile.write("constexpr inline " + type + " ")
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

def generateBlockDescriptor(platform: Platforms, 
                            blockNameCamel,
                            blockNameUpper,
                            block,
                            memberNamesUpper,
                            targetFile):
  memberIterator = 0
  targetFile.write("const Node::DataBlockInfo_t blockDescriptor =\n{\n")
  targetFile.write("  /* .noOfDataMembers = */ BlockTEMPLATE::NUMBER_OF_"+blockNameUpper+"_DATA_MEMBERS,\n")
  targetFile.write("  /* .dataMemberInfo  = */\n  {\n")
  types        = block["Data Type"]
  accessLevels = block["External Access"]
  NVMOffsets   = block["NVM Offset"]
  OTPOffsets   = block["OTP Offset"]
  
  for memberName in memberNamesUpper:
    access    = accessLevels[memberIterator]
    NVMOffset = NVMOffsets[memberIterator]
    OTPOffset = OTPOffsets[memberIterator]
    accessString = ""
    if ((NVMOffset == "-") or (NVMOffset == "")):
      NVMOffset = "NVM_OFFSET_NULL"
    if ((OTPOffset == "-") or (OTPOffset == "")):
      OTPOffset = "NVM_OFFSET_NULL"
    if (access == "RW"): accessString = "WRITE"
    else:                accessString = "READ"
    typeUpper = types[memberIterator].replace("_t", "").upper()
    targetFile.write("    [Block"+blockNameCamel+"::MEMBER_ID_"+memberName+"] =\n    {\n")
    targetFile.write("      /* .type           = */ TYPE_"+typeUpper+",\n")
    targetFile.write("      /* .externalAccess = */ ACCESS_"+accessString+"\n")
    if (platform == Platforms.NODE):
      targetFile.write("      /* .NVMOffset      = */ "+NVMOffset)
      if (NVMOffset != "NVM_OFFSET_NULL"): targetFile.write("U")
      targetFile.write("\n")
      targetFile.write("      /* .OTPOffset      = */ "+OTPOffset)
      if (OTPOffset != "NVM_OFFSET_NULL"): targetFile.write("U")
      targetFile.write("\n")
    targetFile.write("    },\n")
    memberIterator += 1

def generateBlockDefinitions(platform: Platforms, 
                             dataBlockNamesCamel,
                             dataBlockNamesUpper,
                             dataBlocks,
                             targetFile):
  blockIterator = 0
  for blockNameCamel in dataBlockNamesCamel:
    block          = dataBlocks[blockIterator]
    blockNameUpper = dataBlockNamesUpper[blockIterator]
    memberIDsUpper = []
    for memberID in block["Member ID"]:
      memberIDsUpper.append(memberID.replace(" ", "_").upper())
    targetFile.write("/*--- DATA BLOCK " + blockNameUpper + " -----------------------------------------------------------*/\n")
    targetFile.write("namespace Block" + blockNameCamel + " {\n\n")
    targetFile.write("/*--- Member List ---*/\n")
    targetFile.write("typedef enum: uint16_t\n{\n")
    generateEnum("  VAR_ID_", memberIDsUpper, targetFile)
    targetFile.write("  NUMBER_OF_" + blockNameUpper + "_DATA_MEMBERS\n")
    targetFile.write("} DataMemberID_t;\n\n")
    targetFile.write("/*--- Defaults ---*/\n")
    generateConstList(block, memberIDsUpper, "Default", targetFile)
    targetFile.write("\n/*--- Minimum Limits ---*/\n")
    generateConstList(block, memberIDsUpper, "Min Limit", targetFile)
    targetFile.write("\n/*--- Maximum Limits ---*/\n")
    generateConstList(block, memberIDsUpper, "Max Limit", targetFile)
    targetFile.write("\n/*--- Descriptor ---*/\n")
    generateBlockDescriptor(platform, blockNameCamel, blockNameUpper, block, memberIDsUpper, targetFile)
    targetFile.write("  }\n};\n\n")
    targetFile.write("} /* End Namespace - Block"+blockNameCamel+" */\n\n")
    blockIterator += 1 

def autogenCall(platform: Platforms,
                autogenHint: str,
                memMapNameCamel: str,
                dataBlockNamesCamel, 
                dataBlockNamesUpper, 
                dataBlocks,
                targetFile):
  match (autogenHint):
    case "MAP_NAME_CAMEL":
      targetFile.write(memMapNameCamel)
    case "FRAMEWORK_NAME":
      targetFile.write(FRAMEWORK_NAME)
    case "BLOCK_ID_LIST":
      generateEnum("  BLOCK_ID_", dataBlockNamesUpper, targetFile)
    case "INIT_DEFAULTS_DECLARATION":
      match (platform):
        case Platforms.NODE:
          targetFile.write("Error_t initDefaults(void);")
        case Platforms.HUB:
          targetFile.write("Error_t initDefaults(Node &nodeToInit);")
    case "INIT_LIMITS_DECLARATION":
      match (platform):
        case Platforms.NODE:
          targetFile.write("Error_t initLimits(void);")
        case Platforms.HUB:
          targetFile.write("Error_t initLimits(Node &nodeToInit);")
    case "DATA_BLOCK_DEFINITIONS":
      generateBlockDefinitions(platform, 
                               dataBlockNamesCamel,
                               dataBlockNamesUpper,
                               dataBlocks,
                               targetFile)

def generateMemoryMapFile(platform: Platforms,
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
      autogenCall(platform,
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
  universalDataBlockPath    = os.path.join(os.path.dirname(__file__), 'UniversalDataBlock.xlsx')
  memMapTemplateHppPath = os.path.join(os.path.dirname(__file__), 'MemoryMapAutogen.hpp')

  try:
    newBlock = pandas.read_excel(universalDataBlockPath, sheet_name="Universal")
    dataBlocks.append(newBlock)
    dataBlockNamesCamel.append("Universal")
    dataBlockNamesUpper.append("UNIVERSAL")
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
  nodeMemMapHpp     = open(nodeMemMapHppPath, OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(Platforms.NODE,
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        nodeMemMapHpp)

  memMapTemplateHpp = open(memMapTemplateHppPath, OpenMethods.READ_ONLY)
  hubMemMapHpp      = open(hubMemMapHppPath,      OpenMethods.WRITE_FORCE)
  generateMemoryMapFile(Platforms.HUB,
                        memMapNameCamel, 
                        dataBlockNamesCamel, 
                        dataBlockNamesUpper, 
                        dataBlocks, 
                        memMapTemplateHpp, 
                        hubMemMapHpp)
  memMapTemplateHpp.close()
  hubMemMapHpp.close()

  return (Error.NONE)



