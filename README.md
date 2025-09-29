
# Introduction
### Communications
### Variable Storage Management
### Readme Guide
### Prerequisites
### Folder Structure

# Autogen
### .xlsx format
### Generation

# Node Setup & Operation
### Architecture & Operation
Explain variable storage, comms, universal functionality
#### Application API
Include Maps/UserMap.hpp file for access to all memory map variables   
Include Maps/UserDataBlock.hpp file for access to specific data block variables only   

#### Platform Files
The Node side of the Atams library is written to maximise compatibility with different user application setups.
This includes single or multi-threaded applications running on single or dual-core microcontrollers, alongside
polling or interrupt/event driven communications peripheral setups. Atams provides platform files that contain
all the required user constants, function declarations, and empty function definitions. It is the users
responsibility to complete these platform files. The required platform function definitions change depending
upon the users core, threading, and communications peripheral setups. Comments are provided above each empty
function definition to aid with correct implementation. The following tags are present in these comments to 
indicate when a function definition should be filled in by the user:

/* ATAMS PLATFORM REQUIREMENT - ALL */ - 

/* ATAMS PLATFORM REQUIREMENT - MULTI-THREAD */ - 

/* ATAMS PLATFORM REQUIREMENT - DUAL-CORE */ - 

/* ATAMS PLATFORM REQUIREMENT - POLLING COMMS */ - 

/* ATAMS PLATFORM REQUIREMENT - EVENT DRIVEN COMMS */ - 

/* ATAMS PLATFORM REQUIREMENT - MULTI-THREAD + EVENT DRIVEN COMMS */ - 


### Single Core Setup
#### Single Thread
Include CommsCore.hpp for all application API access  
Complete definitions for the following functions in CommsPlatform.cpp:  
getMillis  
eraseNVM  
readFromNVM  
writeToNVM  
enterConfigurationState  
exitConfigurationState  
setBitrate  
resetNode  

#### Multi-thread
• Include CommsCore.hpp on comms core and run x  
• Include AppCore.hpp for variable read/write access
Complete definitions for the following functions in CommsPlatform.cpp:  
getMillis  
eraseNVM  
readFromNVM  
writeToNVM  
enterConfigurationState  
exitConfigurationState  
setBitrate  
resetNode  
acquireVarStorageLock
releaseVarStorageLock

### Dual Core Setup
#### Single Thread
#### Multi-thread

### Application API
#### Initialisation
#### Operation

### Platform File Setup

### Threading Options
#### Single core + single thread 

#### Single core + multi-thread 
#### Dual core + single thread
#### Dual core + multi-thread

### Comms Options
#### Comms Summary
#### Polling
#### Events (Interrrupts, signals)

### Building
#### Eclipse/STM32CubeIDE Setup
#### Cmake (Coming soon)


# Hub Setup and Operation
### Architecture (platform files)

### Application API
#### Initialisation
#### Operation

### Platform Options: Threading
#### Single thread 
#### Multi-thread

### Platform Options: Comms
#### Comms Summary
#### Polling
#### Events (Interrupts, signals)


## Developer
### Universal Data Block
### Coding Standards
### Testing
### Linter Setup


## Contact



# README #

This README would normally document whatever steps are necessary to get your application up and running.

### What is this repository for? ###

* Quick summary
* Version
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### How do I get set up? ###

* Summary of set up
* Configuration
* Dependencies
* Database configuration
* How to run tests
* Deployment instructions

### Contribution guidelines ###

* Writing tests
* Code review
* Other guidelines

### Who do I talk to? ###

* Repo owner or admin
* Other community or team contact