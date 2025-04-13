import customtkinter
import pandas
import os
from   tkinter          import filedialog
from   CTkToolTip       import *
from   PIL              import Image
from   enum             import Enum
from   enum             import StrEnum
from   AtamsFileAutogen import generateCppFiles

FRAMEWORK_NAME = "Atams"

class SearchType(Enum):
  DIRECTORY = 1
  FILE      = 2

def closeWindow(window):
   window.update_idletasks()
   window.destroy()

def center(window):
    window.update_idletasks()
    screen_width  = window.winfo_screenwidth()
    screen_height = window.winfo_screenheight()
    size = tuple(int(_) for _ in window.geometry().split('+')[0].split('x'))
    x = screen_width/2  - size[0]/2
    y = screen_height/2 - size[1]/2
    window.geometry("+%d+%d" % (x, y))
    window.update()

scriptDir       = os.path.dirname(__file__)
tooltipImageDir = os.path.join(scriptDir, 'Images', 'tooltip.png')
tooltipImage    = customtkinter.CTkImage(light_image=Image.open(tooltipImageDir), 
                                         dark_image=Image.open(tooltipImageDir), 
                                         size=(16,16))

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
  generationStatus = generateCppFiles(memoryMapName, memoryMapXlsxPath, nodeDir, hubDir)
  statusLabel.configure(text=generationStatus)
   
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
  warningImageDir = os.path.join(scriptDir, 'Images', 'warning.png')
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
    return # Early Return
  elif (memoryMapName == ""):
    statusLabel.configure(text="Memory Map Name Unset")
    return # Early Return

  memoryMapNameCamel = memoryMapName.lower().capitalize()
  memoryMapNameCamel = memoryMapNameCamel.replace(" ", "")
  nodeMemoryMapDir   = os.path.join(nodeDir, 'Maps', 'MemoryMap' + memoryMapNameCamel)
  hubMemoryMapDir    = os.path.join(hubDir,  'Maps', 'MemoryMap' + memoryMapNameCamel)

  if ((os.path.isdir(nodeMemoryMapDir)) or
      (os.path.isdir(hubMemoryMapDir)) ):
    overwritePopup(memoryMapNameCamel, memoryMapXlsxPath, nodeDir, hubDir)
  else:
    runFileGeneration(memoryMapNameCamel, memoryMapXlsxPath, nodeDir, hubDir)

# TKINTER SETUP #
customtkinter.set_appearance_mode("dark")      # Modes:  system (default), light, dark
customtkinter.set_default_color_theme("green") # Themes: blue (default), dark-blue, green
app = customtkinter.CTk()
app.title(FRAMEWORK_NAME.upper() + " Memory Map Generator")
app.geometry('850x600')
app.minsize(width=850, height=600)
        
fullFrame = customtkinter.CTkFrame(app, fg_color="transparent")
fullFrame.place(relwidth=0.7, relx=0.5, rely=0.5, anchor=customtkinter.CENTER)

title = customtkinter.CTkLabel(fullFrame, text=(FRAMEWORK_NAME.upper() + "\n Memory Map Generator"))
title.pack(side='top', pady=(0,30))
title.configure(font=("TkDefaultFont", 26), text_color="#666")

entryFrame = customtkinter.CTkFrame(fullFrame, height=123, fg_color="transparent")
entryFrame.pack(side='top', fill='x')

fileIconsImageDir = os.path.join(scriptDir, 'Images', 'fileIcons.png')
fileIcons = customtkinter.CTkImage(light_image=Image.open(fileIconsImageDir), 
                                   dark_image=Image.open(fileIconsImageDir), 
                                   size=(123,245))

fileIconsLabel = customtkinter.CTkLabel(entryFrame, text="", image=fileIcons)
fileIconsLabel.pack(side='left', padx=(0,10))

memMapSearch  = directorySearchBox(entryFrame, 
                                   "Memory Map Excel File Path:",                    
                                   SearchType.FILE,      
                                   8,
                                   "Browse and select the Memory Map .xlsx file used to \n"
                                   "define the memory layout of your Node device. \n\n" 
                                   "An example template can be found here: \n"
                                   +FRAMEWORK_NAME+"/Templates/MemoryMapTemplate.xlsx")
nodeDirSearch = directorySearchBox(entryFrame, 
                                   "Node Device Directory Path ("+FRAMEWORK_NAME+"/Node):", 
                                   SearchType.DIRECTORY, 
                                   23,
                                   "Browse and select the "+FRAMEWORK_NAME+"/Node folder located \n"
                                   "in your Hub device software workspace. \n"
                                   "Memory Map C++ files will be generated in this \n"
                                   "location. Ensure access is permitted to all \n"
                                   "sub-folders.")
hubDirSearch  = directorySearchBox(entryFrame, 
                                   "Hub Device Directory Path ("+FRAMEWORK_NAME+"/Hub):",   
                                   SearchType.DIRECTORY, 
                                   23,
                                   "Browse and select the "+FRAMEWORK_NAME+"/Hub folder located \n"
                                   "in your Hub device software workspace. \n"
                                   "Memory Map C++ files will be generated in this \n"
                                   "location. Ensure access is permitted to all \n"
                                   "sub-folders.")

statusLabel = customtkinter.CTkLabel(fullFrame, text="Fill in Fields and Click Generate to Generate C++ Files")
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
memMapSearch.entry.insert(0, "/Users/dan/Desktop/Atams Workspace/TestKitSoftware/CM7/Core/Src/Libraries/Atams/Autogen/Example_Memory_Map.xlsx")
nodeDirSearch.entry.insert(0, "/Users/dan/Desktop/Atams Workspace/TestKitSoftware/CM7/Core/Src/Libraries/Atams/Node")
hubDirSearch.entry.insert(0, "/Users/dan/Desktop/Atams Workspace/HubSoftware/src/Atams/Hub")

app.mainloop()