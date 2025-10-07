import customtkinter
import os
import sys
import appdirs
from   pathlib             import Path
from   tkinter             import filedialog
from   CTkToolTip          import *
from   PIL                 import Image
from   enum                import Enum
from   Modules.FileAutogen import Error, OpenMethods, generateCppFiles
from   tkinter             import PhotoImage

FRAMEWORK_NAME         = "Atams"
AUTOGEN_PATH_SAVE_FILE = "AutogenFilePaths.txt"
APP_NAME               = "AtamsAutogen"
APP_AUTHOR             = "Atams"

saveFileDir  = Path(appdirs.user_data_dir(APP_NAME, APP_AUTHOR))
saveFilePath = os.path.join(saveFileDir, AUTOGEN_PATH_SAVE_FILE)
app          = None 
statusLabel  = None 

class SearchType(Enum):
  DIRECTORY = 1
  FILE      = 2

def resourcePath(relativePath: str) -> Path:
  """Get absolute path to resource, works for dev and PyInstaller"""
  if hasattr(sys, "_MEIPASS"): basePath = sys._MEIPASS                                # PyInstaller temp folder
  else:                        basePath = os.path.dirname(os.path.abspath(__file__))  # Folder of this script
  return os.path.join(basePath, relativePath)

def closeWindow(window):
   window.update_idletasks()
   window.destroy()

tooltipImageDir = resourcePath(os.path.join('Images', 'tooltip.png'))
tooltipImage    = customtkinter.CTkImage(light_image=Image.open(tooltipImageDir), 
                                         dark_image=Image.open(tooltipImageDir), 
                                         size=(16,16))
logoImageDir = resourcePath(os.path.join('Images', 'logo.png'))
logoImage    = customtkinter.CTkImage(light_image=Image.open(logoImageDir), 
                                      dark_image=Image.open(logoImageDir), 
                                      size=(245,56))
headerImageDir = resourcePath(os.path.join('Images', 'header.png'))
headerImage    = customtkinter.CTkImage(light_image=Image.open(headerImageDir), 
                                        dark_image=Image.open(headerImageDir), 
                                        size=(220,13))

class directorySearchBox:
  def __init__(self, parentFrame: customtkinter.CTkFrame, labelText: str, search: SearchType, topPad: int, tipMessage: str):
    if (search == SearchType.DIRECTORY): self.searchFunction = filedialog.askdirectory
    else:                                self.searchFunction = filedialog.askopenfilename
    self.titleFrame = customtkinter.CTkFrame(parentFrame, height=20, fg_color="transparent")
    self.label = customtkinter.CTkLabel(self.titleFrame, text=labelText)
    self.tooltipButton = customtkinter.CTkButton(self.titleFrame, text="", image=tooltipImage, width=16, height=16, fg_color="transparent", hover_color="#333")
    self.browseFrame = customtkinter.CTkFrame(parentFrame, height=30, fg_color="transparent")
    self.entry = customtkinter.CTkEntry(self.browseFrame)
    self.browseButton = customtkinter.CTkButton(self.browseFrame, text="Browse", command=self.buttonPress, width=30, height=30)
    self.titleFrame.pack(side='top', anchor='nw', pady=(topPad, 2))
    self.label.pack(side='left')
    self.tooltipButton.pack(side='left', padx=(4,0))
    self.browseFrame.pack(side='top', fill='x')
    self.entry.pack(side="left", expand=True, fill='both', padx=(0, 4))
    self.browseButton.pack(side="right")
    self.path = ""
    self.tooltip = CTkToolTip(self.tooltipButton, delay=0.5, message=tipMessage)

  def buttonPress(self):
      app.update_idletasks()
      self.path = self.searchFunction()
      self.entry.delete(0,customtkinter.END)
      self.entry.insert(0, self.path)

def runFileGeneration(memoryMapName, memoryMapXlsxPath, nodeDir, hubDir):
  #try:
  generationStatus = generateCppFiles(memoryMapName, memoryMapXlsxPath, nodeDir, hubDir)
  #except:
    #generationStatus = "Error: File Generation Failed - Invalid Memory Map"
  if statusLabel is not None:
    statusLabel.configure(text=generationStatus)
  
  if (generationStatus == Error.NONE):
    saveFileDir.mkdir(parents=True, exist_ok=True)
    saveFile = open(saveFilePath, OpenMethods.WRITE_FORCE)
    saveFile.write(f"{memoryMapXlsxPath}\n{nodeDir}\n{hubDir}")
    saveFile.close()

def overwriteAccepted(window, memoryMapName: str, memoryMapXlsxPath:str, nodeDir: str, hubDir:str):
   closeWindow(window)
   runFileGeneration(memoryMapName, memoryMapXlsxPath, nodeDir, hubDir)

def overwritePopup(memoryMapName: str, memoryMapXlsxPath:str, nodeDir: str, hubDir:str):
  popup = customtkinter.CTkToplevel(app)
  popup.focus_force()
  popup.update()
  popup.geometry('350x250')
  popup.minsize(width=450, height=320)
  popup.title("Overwrite Check")
  popupFrame = customtkinter.CTkFrame(popup, fg_color="transparent")
  popupFrame.place(relwidth=0.7, relx=0.5, rely=0.5, anchor=customtkinter.CENTER)
  warningImageDir = resourcePath(os.path.join('Images', 'warning.png'))
  warningIcon = customtkinter.CTkImage(light_image=Image.open(warningImageDir), 
                                       dark_image=Image.open(warningImageDir), 
                                       size=(100,100))
  warningIconLabel = customtkinter.CTkLabel(popupFrame, text="", image=warningIcon)
  warningIconLabel.pack(side='top', pady=(0, 10))
  warningText  = customtkinter.CTkLabel(popupFrame, 
                                        font=("TkDefaultFont", 14),
                                        text="Warning: This Memory Map name has previously been used to generate files " \
                                             "in at least one of the target folders \n Click OK to overwrite files"      )
  warningText.configure(wraplength=250)
  overwriteAcceptedLambda = lambda:(overwriteAccepted(popup, memoryMapName, memoryMapXlsxPath, nodeDir, hubDir))
  okButton = customtkinter.CTkButton(popupFrame, width=80, height=30, text="OK", command=overwriteAcceptedLambda)
  cancelButton = customtkinter.CTkButton(popupFrame, width=80, height=30, text="Cancel", command=lambda:(closeWindow(popup)))
  warningText.pack(side='top', pady=(0,35))
  okButton.pack(side='left', padx=(40,0), pady=(0,15))
  cancelButton.pack(side = 'right', padx=(0,40), pady=(0,15))

def find(name, path) -> bool:
  for root, dirs, files in os.walk(path):
      if name in files:
          return True
  return False

def generateButtonPressed(memoryMapName: str, memoryMapXlsxPath:str, nodeDir: str, hubDir:str): 
  if (not memoryMapXlsxPath.endswith('.xlsx')):
    statusLabel.configure(text="Invalid Memory Map File Type")
    return # Early Return
  elif (not nodeDir.endswith(os.path.join(FRAMEWORK_NAME, 'Node'))):
    statusLabel.configure(text="Invalid Node Directory")
    return # Early Return
  elif (not hubDir.endswith(os.path.join(FRAMEWORK_NAME, 'Hub'))):
    statusLabel.configure(text="Invalid Hub Directory")
    return # Early Return1
  elif (memoryMapName == ""):
    statusLabel.configure(text="Memory Map Name Unset")
    return # Early Return

  memoryMapNameCamel = memoryMapName.lower().title()
  memoryMapNameCamel = memoryMapNameCamel.replace(" ", "")
  nodeMemoryMapDir   = os.path.join(nodeDir, 'Maps', 'Map' + memoryMapNameCamel)
  hubMemoryMapDir    = os.path.join(hubDir,  'Maps', 'Map' + memoryMapNameCamel)

  if ((os.path.isdir(nodeMemoryMapDir)) or
      (os.path.isdir(hubMemoryMapDir)) ):
    overwritePopup(memoryMapNameCamel, memoryMapXlsxPath, nodeDir, hubDir)
  else:
    runFileGeneration(memoryMapNameCamel, memoryMapXlsxPath, nodeDir, hubDir)

def main():
  global app, statusLabel
  # TKINTER SETUP #
  customtkinter.set_appearance_mode("dark")      # Modes:  system (default), light, dark
  customtkinter.set_default_color_theme("green") # Themes: blue (default), dark-blue, green
  app = customtkinter.CTk()
  app.title(FRAMEWORK_NAME.upper() + " Memory Map Generator")
  app.geometry('700x600')
  app.minsize(width=620, height=550)

  # Load the image: png/gif tested
  icon = PhotoImage(file=resourcePath(os.path.join('Images', 'icon.png')))

  # Set the window icon
  app.iconphoto(True, icon)
          
  fullFrame = customtkinter.CTkFrame(app, fg_color="transparent")
  fullFrame.place(relwidth=0.7, relx=0.5, rely=0.5, anchor=customtkinter.CENTER)

  logoLabel   = customtkinter.CTkLabel(fullFrame, text="", image=logoImage)
  headerLabel = customtkinter.CTkLabel(fullFrame, text="", image=headerImage)
  logoLabel.pack(side='top', pady=(0,0))
  headerLabel.pack(side='top', pady=(0,30))

  entryFrame = customtkinter.CTkFrame(fullFrame, height=123, fg_color="transparent")
  entryFrame.pack(side='top', fill='x')

  fileIconsImageDir = resourcePath(os.path.join('Images', 'fileIcons.png'))
  fileIcons = customtkinter.CTkImage(light_image=Image.open(fileIconsImageDir), 
                                      dark_image=Image.open(fileIconsImageDir), 
                                      size=(123,245))

  fileIconsLabel = customtkinter.CTkLabel(entryFrame, text="", image=fileIcons)
  fileIconsLabel.pack(side='left', padx=(0,10))

  memMapSearch  = directorySearchBox(entryFrame, 
                                      "Memory Map Excel File Path:",                    
                                      SearchType.FILE,      
                                      8,
                                      """\nBrowse and select the Memory Map table ".xlsx" file used to \n"""
                                      "define the variable configuration of your Node device. \n\n" 
                                      "An example template can be found here: \n"
                                      +os.path.join(FRAMEWORK_NAME, "Autogen", "MapTemplate.xlsx")+"\n")
  nodeDirSearch = directorySearchBox(entryFrame, 
                                      "Node Device Directory Path ("+FRAMEWORK_NAME+"/Node):", 
                                      SearchType.DIRECTORY, 
                                      23,
                                      "\nBrowse and select the "+FRAMEWORK_NAME+"/Node folder located \n"
                                      "in your Node device software workspace. \n\n"
                                      "Memory Map C++ files will be generated in\n"
                                      "the 'Maps' folder found at this directory.\n\n"
                                      "Ensure access is permitted to all \n"
                                      "sub-directories.\n")
  hubDirSearch  = directorySearchBox(entryFrame, 
                                      "\nHub Device Directory Path ("+FRAMEWORK_NAME+"/Hub):",   
                                      SearchType.DIRECTORY, 
                                      23,
                                      "\nBrowse and select the "+FRAMEWORK_NAME+"/Hub folder located \n"
                                      "in your Hub device software workspace. \n\n"
                                      "Memory Map C++ files will be generated in\n"
                                      "the 'Maps' folder found at this directory.\n\n"
                                      "Ensure access is permitted to all \n"
                                      "sub-directories.\n")

  statusLabel = customtkinter.CTkLabel(fullFrame, text="Confirm file paths, input map name, then click 'Generate' to generate C++ Files")
  statusLabel.pack(side='bottom')

  generateFrame = customtkinter.CTkFrame(fullFrame, width=800, height=100, fg_color="transparent")
  nameFrame     = customtkinter.CTkFrame(generateFrame, fg_color="transparent")
  generateFrame.pack(side='bottom', pady=(30,5))
  nameFrame.pack(side='left', padx=(0,20))

  nameLabel       = customtkinter.CTkLabel(nameFrame, text="Memory Map Name:")
  memMapNameEntry = customtkinter.CTkEntry(nameFrame, width=200)
  nameLabel.pack(side='top', pady=(0,6))
  memMapNameEntry.pack(side='bottom')

  generateButton = customtkinter.CTkButton(generateFrame, 
                                            text="Generate", 
                                            command=lambda:(generateButtonPressed(memMapNameEntry.get(),
                                                                                  memMapSearch.entry.get(),
                                                                                  nodeDirSearch.entry.get(),
                                                                                  hubDirSearch.entry.get())), 
                                                                                  width=100)
    
  generateButton.pack(side='right', fill='y', pady=(10,0))
  saveFileDir.mkdir(parents=True, exist_ok=True)
  if (os.path.isfile(saveFilePath)):
    saveFile     = open(saveFilePath, OpenMethods.READ_ONLY)
    savedStrings = saveFile.read().splitlines()
    if (len(savedStrings) == 3):
      memMapSearch.entry.insert(0,  savedStrings[0])
      nodeDirSearch.entry.insert(0, savedStrings[1])
      hubDirSearch.entry.insert(0,  savedStrings[2])
    saveFile.close()

  app.mainloop()

if __name__ == "__main__":
    main()