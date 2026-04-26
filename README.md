
<p align="center">
  <img src="Developer/Documentation/Images/LogoBannerAlt.png">
</p>

# Table of Contents
- [Introduction](#introduction)
- [Communications Hardware Requirements](#communications-hardware-requirements)
- [Platform Requirements](#platform-requirements)
- [Language Standard](#language-standard)
- [Memory Maps](#memory-maps)
- [Auto-generation](#auto-generation)
- [Memory Map Access](#memory-map-access)
- [Node Library](#node-library)
- [Hub Library](#hub-library)

# Introduction

Atams is a C++ framework designed for use in embedded systems where a single central device (Hub) communicates with and manages multiple distributed devices (Nodes). It simplifies variable sharing, synchronisation, and non-volatile storage, making it ideal for robotics, automation, and control applications.

Atams is split into three sections: Autogen tooling, a Node library, and a Hub library. 

The Atams Hub and Node libraries support the following features:

### Supported Features:

- Type-safe variable exchange with multiple Nodes from a single Hub.
- Support for multiple groups of Nodes on separate buses from a single Hub.
- Optional synchronous processing to ensure all Node variable interactions on the same bus occur simultaneously.
- Support for connecting each Node to multiple buses with different hardware peripherals.
- Dynamic packet generation to fascilitate adaptive variable exchange.
- Packet checksumming and framing to ensure safe variable transport.
- Endianness agnostic variable transport to maximise platform compatibility.
- Hub-triggered saving of selected variables to non-volatile memory on Nodes.
- Hub-triggered restoration of factory defaults to non-volatile memory on Nodes.
- Checksumming and version tracking of non-volatile memory storage.
- Coordination of Memory Map versioning between Node and Hub implementations to ensure safe variable sharing.
- Node watchdog timers to trigger safe state entry should Hub communications fail.
- Hub-triggered communications bitrate changes on Nodes.
- Hub-initiated Node resets.
- Software bus arbitration to avoid messaging conflicts when using hardware without arbitration functionality.
- Zero dynamic memory allocation.

### Not Yet Supported:

- Automatic migration of Node non-volatile storage between compatible Memory Map versions.
- Variable value maximum and minimum limits at the point of Request Packet parsing.
- Large raw buffer transport.

# Communications Hardware Requirements
Atams primarily targets multi-drop buses (such as RS485, CAN FD, CAN XL, 10Base-T1S Ethernet) to simplify wiring in multi-device systems, but can be used on any bus with multi-cast or broadcast capability.

- **Broadcast capability:** All Hub and Node devices must be able to receive all packets transmitted by all other devices on the same Atams Bus. Multi-casting or broadcasting must be available when using non-multi-drop hardware.
- **Self-reception:** Atams safely handles cases where a device receives its own messages.
- **Bus arbitration:** Hardware-level bus arbitration is not required, but can be used alongside Atams software arbitration to improve robustness.
- **Packet size:** Atams is not compatible with hardware that has a small maximum packet size (such as classic CAN). A minimum *maximum* packet size of 64 bytes or greater is recommended.


# Platform Requirements
Atams is compatible with any platform that meets the following requirements:
- Single read and write operations to variables of type `uint8_t` must be inherently atomic.
- The size of a float must be 4 bytes and meet the IEC559 standard for binary representation.


# Language Standard
Atams is compatible with C++17 and above, does not use any non-ISO C++ features, and is tested with the following compiler flags:  
`-Wall -Wextra -Wpedantic -Wswitch-default -Wunreachable-code -Wformat`


# Memory Maps

Memory Map C++ files are required for operation of the Node and Hub libraries. They provide Atams with the information it needs to initialise and regulate interactions with the Atams variable storage.

### Data Blocks 
Memory Maps are divided into variable groups, or "Data Blocks", to enable users to loosely limit access to certain blocks of variables to specific user files.

- **Variable IDs:** Each of the auto-generated Data Block `.hpp` files includes an enum list of the variable IDs contained within that Block. These variable IDs are used as input arguments to Atams functions to specify variable access.
    ```cpp
    enum VarID_t: uint16_t
    {
        VAR_EXAMPLE_1 = 28U,
        VAR_EXAMPLE_2 = 29U,
        VAR_EXAMPLE_3 = 30U,
    };
    ```

- **Variable Information:** Each Data Block file also contains an array of structs holding information related to each variable. The information includes the variables type, access permission, and non-volatile storage option. Atams is compatible with the following variable types:
    ```cpp
    uint8_t
    int8_t
    uint16_t
    int16_t
    uint32_t
    int32_t
    float
    ```

### Initialisation Functions 
The Node library Memory Map holds function references used to initialise the Universal Data Block, and restore variable values to factory defaults when required.

### The Universal Data Block
The Universal Data Block is included in all Memory Maps, and is used by the Atams libraries to implement Atams functionality.

### Gen Info 
An Atams::GenInfo_t struct is generated with every Memory Map. This is used to track when the Memory Map files were generated, what Atams version they were generated for, and includes a checksum. This data is used by the Hub library during Bus initialisation to ensure the Hub device and Node device versions of the Memory Map are fully compatible.
```cpp
struct GenInfo_t
{
  uint8_t  atamsVersionMajor  {ATAMS_VERSION_MAJOR};
  uint8_t  atamsVersionMinor  {ATAMS_VERSION_MINOR};
  uint8_t  genDay             {0U};
  uint8_t  genMonth           {0U};
  uint16_t genYear            {0U};
  uint8_t  genHour            {0U};
  uint8_t  genMinute          {0U};
  uint8_t  genSecond          {0U};
  uint32_t genChecksum        {0U};
  uint16_t noOfVars           {0U};
}
```

# Memory Map Auto-generation
GUI tooling is provided for auto-generating device specific Memory Map C++ files for use with the Hub and Node libraries. 

### Memory Map Tables

![alt text](Developer/Documentation/Images/MemoryMapExampleTable.png)

To use the auto-generation application, a user needs to fill out an Atams Memory Map table in the provided `.xlsx` format to define the required information for each device variable. The table is used by the autogen tool to create the Memory Map C++ files, and doubles as documentation for the device Memory Map. The template is available at the following directory path:  
`Atams/Autogen/TemplateMap.xlsx`.

- **Data Blocks:** Each Data Block should be defined in a separate sheet of the `.xlsx` file. Copy the original template sheet to maintain the correct table formatting. Data Block names will be taken from the name of each sheet and converted to `PascalCase` namespaces with a `Block` prefix in the generated C++ files.

- **Columns:**
The following columns of information are provided. Only the `Required` columns are necessary for Atams operation, and must be completed for successful auto-generation:

| Column Name     | Requirement  | Description    |
| :-------------: | :----------: | :------------: |
| Var ID          | Required     | Name of the device variable. This can be provided in any format, but will be converted to `SCREAMING_SNAKE_CASE` with a `VAR_` prefix in the Data Block C++ file `VarID_t` enum list. This ID will be used as an input argument for Atams functions so it is worth keeping naming concise where possible. |
| Data Type       | Required     | Pick from the dropdown list of variable types to define the variable type. Interacting with this variable with any other type will return errors from Atams functions. Atams is compatible with the provided types only. |
| External Access | Required     | Pick from RO (Read Only), or RW (Read/Write). RO means a Hub device can only read from the selected Node variable. RW means the Hub device can read from and write to the selected Node variable. |
| Units           | Optional     | Provided only for user documentation purposes. |
| Min Limit       | Optional     | Provided only for user documentation purposes. |
| Max Limit       | Optional     | Provided only for user documentation purposes. |
| Default         | Optional     | Defines the value for the variable to be set to on startup and during a factory restore operation. Values previously stored in non-volatile memory will overwrite default values on startup. If this column is left blank, the variable will be zero-initialised. |
| NVM Storage     | Optional     | Defines whether the variable should be stored in non-volatile memory or not during a STORE ALL operation. If this column is left blank, the variable will not be stored in non-volatile memory. |
| Description     | Optional     | Provided only for user documentation purposes. |
| Widget          | Future Scope | Provided for future Atams application compatibility. |


### Autogen GUI Application

<p align="center">
  <img src="Developer/Documentation/Images/AtamsAutogenTool.png">
</p>

Follow these steps to start auto-generating Memory Maps with the Atams Memory Map Generator GUI.
- **Step 1:** Install the Python library requirements for the autogen application by running the following command in terminal:

    `pip install -r PATH_TO_USER_PROJECT_FOLDER/Atams/Autogen/requirements.txt`.   
- **Step 2:** Open the application by running `AtamsAutogen.py` from the `Atams/Autogen` folder.
- **Step 3:** Browse and select the Memory Map Table `.xlsx` file specific to the Node device in development.
- **Step 4:** Browse and select the Atams Node library directory path (`Atams/Node`) used by the Node device C++ project. The auto-generation tool will create a Memory Map folder in the `Atams/Node/Maps` folder, containing the Memory Map and Data Block files specific to the device-in-development.
- **Step 5:** Browse and select the Atams Hub library directory path (`Atams/Hub`) used by the Hub device C++ project. The auto-generation tool will place a Memory Map folder in the `Atams/Hub/Maps` folder, containing the Memory Map and Data Block files specific to the device-in-development.
- **Step 6:** Enter a name for the Memory Map. Memory Map names will be converted to `PascalCase` namespaces with a `Map` prefix in the generated C++ files.
- **Step 7:** Click the *Generate* button. A popup may appear with a warning if there is a risk of over-writing previously generated Memory Map files. Generation status information will be provided towards the bottom of the application.

> [!NOTE]  
> The application will remember any previously selected paths when restarted.

> [!TIP]  
> If the Hub and Node are not being developed on the same device, consider generating to local repositories and using version control to efficiently synchronise Memory Map files between development environments. The Hub library Bus initialisation process will effectively catch and return errors if the Hub and Node Memory Maps are not synchronised.


# Memory Map Access
Once the Memory Map C++ files have been generated, they are ready to be used in the Node and Hub libraries.

- **Full Map Access:** If a user file needs access to all variables from an Atams Memory Map, it should include the Map file from the associated Map folder found in either `Atams/Node/Maps` for Node projects, or `Atams/Hub/Maps` for Hub projects. 

    **Hub example:**
    ```cpp
    #include "Atams/Hub/Maps/MapExample/MapExample.hpp"
    ```
    **Node example:**
    ```cpp
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"
    ```
- **Limited Data Block Access:** If a user file only requires access to a specific Data Block, it should only include the associated Block file found in the associated Map folder.

   **Hub example:**
    ```cpp
    #include "Atams/Hub/Maps/MapExample/BlockExample1.hpp"
    ```
    **Node example:**
    ```cpp
    #include "Atams/Node/Maps/MapExample/BlockExample1.hpp"
    ```

- **Variable ID Access:** With the appropriate Map or Block files included, an example variable ID could look like this:  
    ```cpp
    Atams::MapExample::BlockExample::VAR_EXAMPLE_1
    ```
    This nested namespace access can result in long variable names. It is recommended to use `using namespace` to access the required Memory Map or Data Block where sensible. Extra care must be taken when using `using namespace` in files that directly, or indirectly, includes multiple Memory Map or Data Block headers. 

    ```cpp
    uint8_t variableToWrite {0U};

    // Recommended 
    using namespace Atams::MapExample;

    Atams::write(BlockExample1::VAR_EXAMPLE_1, variableToWrite);
    
    // Not Recommended 
    using namespace Atams::MapExample::BlockExample1;
    using namespace Atams::MapExample::BlockExample2;

    // Variable could belong to BlockExample1 or 2
    Atams::write(VAR_EXAMPLE_1, variableToWrite);
    ```
> [!CAUTION]   
> Block-only header inclusion provides a loose limit on variable access. The compiler will provide warnings should the user try to use a variable ID that is not provided by the included file, or if a variable ID does not exist in the enum of the included Block namespace. However, users should be cautious of hidden includes of other Block or Map files when using `using namespace`. Unless confident, users should avoid using raw `uint16_t` variables instead of the provided enum IDs as input arguments to Atams functions. Atams functions will always return an error if the variable ID is outside the bounds of the entire Memory Map.


# Node Library

### Includes & Core Setup
The Node library is compatible with single and dual-core micro-controllers. The user can decide which setup is more appropriate for their use case, but should aim to minimise Atams communications response times for the best Atams Bus performance. See [Maximising Node Performance](#maximising-node-performance).

- **Memory Map Includes:**

    Node library Memory Map files should be included from `Atams/Node/Maps`. See [Memory Map Access](#memory-map-access) for details.

- **Single Core Includes:** 

    If the Atams Node library is used on a single core platform, all Node device functions become accessible with the following header include:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    ```

    <p align="center">
      <img height="500" src="Developer/Documentation/Images/CommsCoreInclude.gif">
    </p>

- **Dual-Core Includes:** 
    
    If a dual-core setup is used, user application-core files should include the following header:
    ```cpp
    #include "Atams/Node/AppCore/AppCore.hpp"
    ```
    Files running communications-core functions should include the following header:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    ```

### Comms Core (Single & Dual-Core)

- **Overview:**

    The Comms Core is responsible for updating the Node's communications interface - allowing external Hub devices to access the Node's variable storage, and trigger Node processes. On single-core platforms, user application code can access the variable storage using the Comms Core setter and getter functions.

- **Initialisation:** 

    The Comms Core should be initialised with the Memory Map generated for the device-in-development. During initialisation, the Memory Map is validated, before the default values are loaded into the Atams variable storage. If compatible values exist in non-volatile memory (NVM), they are restored - replacing the default values where applicable. If all checks pass, and the init function returns `Atams::ERROR_NONE`, the Comms Core is ready for operation. 
    
    The NVM restoration status can be checked by passing a second Atams::Error_t variable by reference to the init functions. This error is separated from the main error return, so that the Node can continue to operate if non-volatile storage cannot be restored. It is currently not possible to restore data from non-volatile memory if the version of the Node's Memory Map has changed since the values were stored.

    There are two versions of the Comms Core init function found in `Atams/Node/CommsCore.hpp`:

    Single-core function:
    ```cpp
    Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError)
    ```

    Dual-core function (Dual-core only):
    ```cpp
    Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap, Atams::Error_t &nvmError)
    ```

    > [!NOTE]  
    > The dual-core init function handles synchronisation with the App Core. The function will hang if the App Core init is never run or does not complete successfully.

    Single-core initialisation example:

    ```cpp

    #include "Atams/Node/CommsCore/CommsCore.hpp"
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"

    void foo(void)
    {
      Atams::Error_t nvmStatus  {Atams::ERROR_NONE};
      Atams::Error_t initStatus {Atams::ERROR_NONE};
      
      initStatus = Atams::initSingleCore(Atams::MapTest::memoryMap, nvmStatus);
  
      if (initStatus != Atams::ERROR_NONE)
      {
        // An error occured during Node initialisation
        printf("Atams Node initialisation error: ");
        printf(Atams::getErrorString(initStatus));
        printf("\n");
      }
      else if (nvmStatus != Atams::ERROR_NONE)
      {
        // An error occured during restoration from non-volatile storage
        printf("Atams non-volatile: ");
        printf(Atams::getErrorString(initStatus));
        printf("\n");
      }
    }
    ```
    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeInit.gif">
    </p>

- **Setting and Getting Variables:** 

    The Node library `Atams::getVar` and `Atams::setVar` functions can be used to read from, and write to, the Atams variable storage from the user application code. The functions will return an error if the Memory Map has not be initialised successfully, if the variable ID is outside of the Memory Map range, or if the template argument variable type does not match the type specified for the variable in the Memory Map. A return of `Atams::ERROR_NONE` indicates a successful transfer.

    Getter example:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"

    using namespace Atams::MapExample;

    void foo(void)
    {
      uint32_t exampleVar {0U};
  
      Atams::Error_t error = getVar(BlockExample::VAR_ID_EXAMPLE_GET, exampleVar);
    }
    ```

    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeGetVar.gif">
    </p>

    Setter example:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"

    using namespace Atams::MapExample;

    void foo(void)
    {
      int8_t exampleVar = -1;
  
      Atams::Error_t error = setVar(BlockExample::VAR_ID_EXAMPLE_SET, exampleVar);
    }
    ```

    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeSetVar.gif">
    </p>

- **Communications Update:** 

    The Atams communications update functions wait for and process all incoming Bus messages. A single Request Packet from an Atams Hub can include read and write requests for multiple Atams variables. When a Request Packet is received, it is stored for processing at the appropriate time. During processing, the Comms Core validates the request packet, writes received data to the Atams variable storage, transfers read data from the variable storage into a Response packet, and transmits the Response packet back to the Hub at the appropriate time.

    The polling communications update function can be used in polling application setups. It should be called as often as possible. The longer the delay between calls, the slower the potential response time of the Node device:
    ```cpp
    void updateCommsPolling(void);
    ```

    The alternative blocking update function can be called from a RTOS thread. The function will block the calling thread while waiting to receive new packets. The calling thread priority should be set as high as possible for fast Node response times.
    ```cpp
    void updateCommsBlocking(void);
    ```

    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeCommsUpdate.gif">
    </p>

### App Core (Dual-Core Only)

- **Overview:**

    For dual-core applications, the App Core hosts the user application code. Setter and getter functions allow the user application code to access the same variable storage as the Comms Core.

- **Initialisation:** 

    The App Core must be initialised using the same Memory Map as the Comms Core. The following function from `Atams/Node/AppCore/AppCore.hpp` is used to initialise the application core. The function will validate the provided Memory Map, and then perform a blocking wait until the Comms Core initialisation completes. The function will return `Atams::ERROR_NONE` if initialisation is successful.

    ```cpp
    Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);
    ```

- **Setting and Getting Variables:**

    The App Core `getVar` and `setVar` functions operate identically to the communications core versions. Atams handles variable storage locks to prevent race conditions from occurring if both cores attempt to access variables simultaneously.

### Platform Setup

The Node library is written to maximise compatibility with different user application setups. This includes single or multi-threaded applications running on single or dual-core microcontrollers, alongside polling or event-driven communications peripheral setups. Atams provides platform files that contain all the required user constants, function declarations, and empty function definitions. The required platform function definitions change depending upon the users application setup. Comments are provided above each constant and function definition to help with implementation. 

- **Platform Files:** The following Node library files need to be completed by the user:

    Single and dual-core:  
    `Atams/Node/SharedPlatform.cpp`\
    `Atams/Node/CommsCore/CommsPlatform.hpp`\
    `Atams/Node/CommsCore/CommsPlatform.cpp`  
    
    Dual-core only:  
    `Atams/Node/AppCore/AppPlatform.hpp`\
    `Atams/Node/AppCore/AppPlatform.cpp`  

- **Function Comments:** The following tags are present in the `.cpp` file function comments to indicate when completion of the definition is required. Non-applicable function definitions can be left empty. It is recommended to cast any unused function arguments to `void` to avoid compiler warnings.

| Comment Tag                        | Requirement Condition |  
| :--------------------------------- | :---------- |  
| ALL                                | Required for all platform setups. |  
| MULTI-THREAD                       | Required when Atams functions are used from multiple threads, and/or interrupt contexts, on the given core. | 
| DUAL-CORE                          | Required when Atams functions are used on multiple MCU cores. |
| POLLING COMMS                      | Required when the platform hardware needs to be polled in order to receive incoming bytes. |
| EVENT DRIVEN COMMS                 | Required when incoming bytes are received into an interrupt or event context separate from the context running the Atams comms update function. |
| MULTI-THREAD + EVENT DRIVEN COMMS  | Required when Atams::updateCommsBlocking() is used in combination with an event driven receive setup. |

- **Platform Byte Pre-Processing:** No pre-processing of incoming bytes is required at the platform level. All incoming bytes can be passed directly to Atams using the provided receive callback functions. The incoming bytes must be passed to Atams in order, and the same bytes should not be passed to Atams more than once.

- **Platform Reception Methods:** Atams packets are compatible with polling, character delimited, or idle line reception methods. If a user wants to use character delimited reception methods - the delimiter should be set to hex `0x00`. 

### Maximising Node Performance
Care should be taken when using Atams alongside user code containing high-priority device functions. If user device functions run at the same priority or higher than an Atams comms update function, the response time of the Node could be at least up to the length of time it takes these user functions to run. One solution for RTOS based systems can be to run the `Atams::updateCommsBlocking()` function in a higher priority thread than any user device functions. If this is not possible, a secondary core of a multi-core MCU can be dedicated to run Atams communications for the quickest Node response times. This becomes especially important when using the synchronous update cycle with the Atams Hub library.

### Building (CMake Example) - Coming Soon

### Building (STM32CubeIDE Dual-Core Example) - Coming soon


# Hub Library

<p align="center">
    <img height="500" src="Developer/Documentation/Images/BusConstruction.gif">
</p>

The Hub library includes two key classes: a Node class, and a Bus class. 

**Node Class:** The Hub device user code needs to construct a Node class instance for each physical Node connected to the Hub. The Node class public interfaces allow the user to set and get variables stored in the Node instances variable storage. It also allows users to control the method and frequency by which the variables are transferred to and from the variable storage on the physical Node device.

**Bus Class:** A Bus class instance is required to enable communication and variable exchange with connected Node devices. Each Node class instance must be linked to the Bus instance that mirrors the physical device connections. The Bus class public interfaces include easy to update processes - abstracting away packet handling, compatibility checks, Node storage processes, Bus arbitration, Node configuration, Node sychronisation, and error handling. This lets the user focus almost entirely on their application specific functionality.

### Common Return Types
The majority of Hub library functions return one of the two following types:

- **Atams::Error_t**\
This enum type is used to notify the user of any function or process errors.\
The following function included from `Atams/Shared/AtamsTypedefs.hpp` converts the error value into a `const char *` string description:
  ```cpp
  const char *getErrorString(const Atams::Error_t error);
  ```

- **Atams::ProcessState_t**\
This enum type is returned from any Atams function that includes a polled update process. The enum options are as follows:
    ```cpp
    enum ProcessState_t: uint8_t
    {
      PROCESS_ERROR       = 0U, /* - The process reached an error condition and has stopped.
                                 * - Check the Atams::Error_t passed to the process
                                 *   update function for error details. 
                                 * - Restart the process using the appropriate 
                                 *   `begin` function if required.
                                 */

      PROCESS_IN_PROGRESS = 1U, /* - The process is in progress, keep polling the process update 
                                 *   function until either PROCESS_ERROR or PROCESS_COMPLETE is
                                 *   returned.
                                 */

      PROCESS_COMPLETE    = 2U  /* - The process has completed successfully.
                                 * - Restart the process using the appropriate 
                                 *   `begin` function if required.
                                 */
    };
    ```

### Includes

- **Memory Map Includes:**

    Hub library Memory Map files should be included from `Atams/Hub/Maps`. See [Memory Map Access](#memory-map-access) for details.

- **Node Access Includes:**

    Files responsible for initialising or accessing a Node instance should include the following header:\
    `Atams/Hub/Node.hpp`

- **Bus Control Includes:**

    Files responsible for initialising and updating the Bus must include the following headers:\
    `Atams/Hub/Node.hpp`\
    `Atams/Hub/Bus.hpp`

### Bus Construction
!!!TODO:: Bus construction description and example

### The Node Configuration Process
Before a Node device can be used with the Hub library, the user must set up the Node configuration.
```cpp
struct NodeConfig_t
{
  uint8_t                currentNodeID;
  uint8_t                newNodeID;
  Atams::BitrateOption_t bitrateOption;
  uint32_t               watchdogPeriod;
};
```

- **Node ID:**\
Most importantly, the Node configuration includes the unique Node ID used for Node addressing during the Bus update cycle. Each physical Node on an Atams Bus must be configured with it's own unique ID ranging from 1-254. All unconfigured Nodes will start with a Node ID of 0.

- **Bitrate Option:**\
How the Bitrate Option affects the Node's hardware peripheral bitrate is defined by the developer of the Node device in the Node library platform setup. All Node's on the same bus must have the same hardware peripheral bitrate as a result of setting each Bitrate Option.

- **Watchdog Period:**\
The Node library measures the time between messages received from the Hub. If the time spent waiting for a new message exceeds the watchdog period, the Node library will alert the Node device user code so the device can enter a safe state. The period is measured in milliseconds. Set the value to 0 to disable the watchdog functionality.

- **Hardware Connections:**\
If multiple unconfigured Node's need to be added to a physical bus, they must be physically connected and configured one by one. If a Node already has a unique ID, it can be re-configured without having to remove any other Nodes from the bus, so long as the new Node ID does not conflict with an existing Node ID on the bus.

- **Starting the Configuration Process:**\
The following example shows how to start the configuration process for an un-configured Node on the bus.

  ```cpp
  #include "Atams/Hub/Bus.hpp"

  void foo(void)
  {
    Atams::Error_t error = bus.beginSetNodeConfigProcess(nodeConfig);

    if (error != Atams::ERROR_NONE)
    {
      // An error occured while attempting to start the Node configuration process
      printf("Atams Node configuration process error: ");
      printf(Atams::getErrorString(error));
      printf("\n");
    }
  }
  ```

- **Updating the Configuration Process:**\
During the Node configuration process, the new configuration values will be written to the Universal Data Block of the Node device. Once the Hub confirms the values are written successfully, it will trigger an NVM storage operation on the Node. If the Hub confirms the NVM storage operation completes successfully, the update function will return `Atams::PROCESS_COMPLETE`. If an error occurs during the configuration process, the update function will return `Atams::PROCESS_ERROR`. In this case, the value of the `Atams::Error_t` passed to the update function can be checked for further details.

  Example:

  ```cpp
  #include "Atams/Hub/Bus.hpp"

  void foo(void)
  {
    Atams::Error_t        error {Atams::ERROR_NONE};
    Atams::ProcessState_t processState;

    do 
    {
      processState = bus.updateSetNodeConfigProcess(error);
    }
    while (processState == Atams::PROCESS_IN_PROGRESS);

    if (processState == Atams::ProcessState::COMPLETE) 
    {
      printf ("Node Configuration Successful\n");
    }
    else if (processState == Atams::ProcessState::ERROR)    
    {
      // An error occured during the Node configuration process
      printf("Atams Node configuration process error: ");
      printf(Atams::getErrorString(error));
      printf("\n");
    }
  }
  ```
  
### Node Construction

When all Node's on the Bus are appropriately configured (See [Node Configuration](#node-configuration)), Node class instances can be constructed using their unique Node ID. The ID can be changed at runtime using the `setNodeID` function, but keep in mind that the Node device must be re-configured before changing the Node ID of the class instance.

<p align="center">
  <img height="500" src="Developer/Documentation/Images/HubNodeConstruction.gif">
</p>

### Node Initialisation
Each Node instance needs to be initialised with a Memory Map from `Atams/Hub/Maps` that was generated with the Memory Map used to initialise the associated Node device. If a valid Memory Map is provided to the initialisation function, the function will return `Atams::ERROR_NONE`.

**Initialisation example:**
```cpp
#include "Atams/Hub/Node.hpp"
#include "Atams/Hub/Maps/MapExample/MapExample.hpp"

static Atams::Node exampleNode(1);

error = driveNode.init(Atams::MapDrive::memoryMap);
```
<p align="center">
  <img height="500" src="Developer/Documentation/Images/HubNodeInit.gif">
</p>

### Linking Nodes to a Bus
<p align="center">
  <img height="500" src="Developer/Documentation/Images/HubAddNodeToBus.gif">
</p>

### The Bus Initialisation Process

<p align="center">
  <img height="500" src="Developer/Documentation/Images/BusInitProcess.gif">
</p>

### Node Request Pattern Control
!!!TODO:: setRequestPattern description and example
!!!TODO:: Helper function examples

### Node Variable Access
The Node class `getVar` and `setVar` functions can be used to read from, and write to, a Node instances variable storage from the user application code. The functions will return an error if the Node instance has not be initialised successfully, if the variable ID is outside of the Node instance's Memory Map range, or if the template argument variable type does not match the type specified in the Memory Map. A return of `Atams::ERROR_NONE` indicates a successful transfer.

Getter example:
```cpp
using namespace Atams::MapExample;

uint32_t exampleVar {0U};

Atams::Error_t error = getVar(BlockExample::VAR_ID_EXAMPLE_GET, exampleVar);
```

### Node Acknowledgement and New Data Ready Flags
!!!TODO:: Description and examples of checking acknowledgement and new data ready flags

### Node Helper Functions
!!!TODO:: Description of combined helper functions

### Synchronous Bus Update Cycle
- **Overview**\
!!!TODO: Description of the synchronous update cycle

- **Starting the Synchronous Update Cycle:**\
!!!TODO: Example of starting the synchronous update cycle

- **Updating the Synchronous Update Cycle:**\
!!!TODO: Example of updating the synchronous update cycle


### Asynchronous Bus Update Cycle
- **Overview**\
!!!TODO: Description of the asynchronous update cycle

- **Starting the Asynchronous Update Cycle:**\
!!!TODO: Example of starting the asynchronous update cycle

- **Updating the Asynchronous Update Cycle:**\
!!!TODO: Example of updating the asynchronous update cycle

### Platform Implementation
!!!TODO: Explanation on how to set up the Hub library platform files


