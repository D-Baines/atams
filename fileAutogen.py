import csv
import sys
import os
import pandas
from   enum   import Enum
from   enum   import StrEnum
from   typing import List

FRAMEWORK_NAME = "Atams"

class Platforms(Enum):
  NODE                = 0
  HUB                 = 1

class OpenMethods(StrEnum):
  READ_ONLY   = 'r'
  WRITE_TRY   = 'x'
  WRITE_FORCE = 'w' 

class Error(StrEnum):
  NONE                = "File Generation Successful!"
  FILE_OPEN_FAILED    = "Generation Error: Failed to Open Files"
  INAVLID_FILE_FORMAT = "Generation Error: Invalid Memory Map Xlsx Format"

fileSuffix = [".cpp", ".hpp"]

def getLongestString(strings: List[str]) -> int:
  longestString = 0
  for string in strings:
    if (len(string) > longestString):
      longestString = len(string)
  return (longestString)

def writeSpaces(noOfSpaces: int, targetFile):
  while (noOfSpaces > 0):
    targetFile.write(" ")
    noOfSpaces -= 1

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

def generateBlockDefinitions(platform: Platforms, 
                             dataBlockNamesCamel,
                             dataBlocks,
                             targetFile):
  blockIterator = 0
  for blockName in dataBlockNamesCamel:
    block = dataBlocks[blockIterator]
    memberIDs = []
    for memberID in block["Member ID"]:
      memberIDs.append(memberID.replace(" ", "_").upper())
    targetFile.write("/*--- DATA BLOCK TEMPLATE -----------------------------------------------------------*/\n\n")
    targetFile.write("namespace Block" + blockName + " {\n\n")
    targetFile.write("/*--- Member List ---*/\n\n")
    generateEnum("  MEMBER_ID_", memberIDs, targetFile)
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



