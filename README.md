
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

Atams is a C++ framework designed for use in embedded systems where a single central device ("Hub") communicates with and manages multiple distributed devices ("Nodes"). It simplifies variable sharing, synchronisation, and non-volatile storage, making it ideal for robotics, automation, and control applications.

Atams is split into three sections: Autogen tooling, a Hub library, and a Node library.

To accelerate development, Atams provides tools for auto-generating device-specific "Memory Maps" for the Hub and Node libraries. These Memory Maps hold information for each Node variable, including type, access permission, storage option, and factory default. 

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
- Variable value maximum and minimum limits on request packet parsing.
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

- **Data Blocks:** Memory Maps are divided into variable groups, or "Data Blocks", to enable users to loosely limit access to certain blocks of variables to specific user files.
- **Variable IDs:** Each of the auto-generated Data Block `.hpp` files includes an enum list of the variable IDs contained within that Block. These variable IDs are used as input arguments to Atams functions to specify variable access:
    ```cpp
    enum VarID_t: uint16_t
    {
      VAR_EXAMPLE_1 = 28U,
      VAR_EXAMPLE_2 = 29U,
      VAR_EXAMPLE_3 = 30U,
    };
    ```
- **Variable Information:** Each Data Block file also contains an array of `Atams::VarInfo_t` structs holding information related to each variable. The information includes the variables type, access permission, and non-volatile storage option. Atams is compatible with the following variable types:
    ```cpp
    uint8_t
    int8_t
    uint16_t
    int16_t
    uint32_t
    int32_t
    float
    ```
- **Initialisation Functions:** The Node library Memory Map also holds function references used to initialise the Universal Data Block, and restore factory defaults when required.
- **The Universal Data Block:** The Universal Data Block is included in all Memory Maps, and is used by the Atams libraries to implement Atams functionality.


# Auto-generation
GUI tooling is provided for auto-generating device specific Memory Map C++ files for use in the Hub and Node libraries. 

### Memory Map Tables

![alt text](Developer/Documentation/Images/MemoryMapExampleTable.png)

To use the auto-generation application, a user needs to fill out an Atams Memory Map table in the provided `.xlsx` format to define use information for each device variable. The table is used by the autogen tool to create the Memory Map C++ files, and doubles as documentation for the device Memory Map. The template is available at the following directory path:  
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
- **Step 4:** Browse and select the Atams Node library directory path (`Atams/Node`) used by the Node device C++ project. The auto-generation tool will place a Memory Map folder in the `Atams/Node/Maps` folder, containing all the generated Data Block and Memory Map files specific to the Node library.
- **Step 5:** Browse and select the Atams Hub library directory path (`Atams/Hub`) used by the Hub device C++ project. The auto-generation tool will place a Memory Map folder in the `Atams/Hub/Maps` folder, containing all generated Data Block and Memory Map files specific to the Hub library.
- **Step 6:** Enter a name for the Memory Map and click the *Generate* button. A popup may appear with a warning if there is a risk of over-writing previously generated Memory Map files. Generation status information will be provided towards the bottom of the application. Memory Map names will be converted to `PascalCase` namespaces with a `Map` prefix in the generated C++ files.

> [!NOTE]  
> The application will remember any previously selected paths when restarted.

> [!TIP]  
> If the Hub and Node are not being developed on the same device, consider generating to local repositories and using version control to efficiently synchronise Memory Map files between development environments. The Atams Bus initialisation process will effectively catch and return errors if the Hub and Node Memory Maps are not synchronised.


# Memory Map Access
Once the Memory Map C++ files have been generated, they are ready to be used in the Node and Hub libraries.

- **Full Map Access:** If a user file needs access to all variables from an Atams Memory Map, it should include the associated Map file from the associated Map folder found in either `Atams/Node/Maps` for Node projects, or `Atams/Hub/Maps` for Hub projects. 

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
> Block-only header inclusion provides a loose limit on variable access. The compiler will provide warnings should the user try to use a variable ID that is not provided by the included file, or if a variable ID does not exist in the specified Block namespace. However, users should be cautious of hidden includes of other Block or Map files, or using raw `uint16_t` variables instead of the provided enum named constants as input arguments to Atams functions. Atams functions will always return an error if the variable ID is outside the bounds of the entire Memory Map.


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

    The communications core should be initialised with the Memory Map generated for the device-in-development. During initialiation, the Memory Map is validated, before the default values are loaded into the Atams variable storage. If compatible values exist in non-volatile memory (NVM), they are restored - replacing the default values where applicable. If all checks pass, and the init function returns `Atams::ERROR_NONE`, the Comms Core is ready for operation. 

    There are two versions of the Comms Core init function found in `Atams/Node/CommsCore.hpp`:

    Single-core function:
    ```cpp
    Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);
    ```

    Dual-core function (Dual-core only):
    ```cpp
    Atams::Error_t initCommsCore(const MemoryMap_t &memoryMap);
    ```

    > [!NOTE]  
    > The dual-core init function handles synchronisation with the application core. The function will hang if the application core init is never run or does not complete successfully.

    Single-core initialisation example:

    ```cpp
    #include <stdio.h>

    #include "Atams/Node/CommsCore/CommsCore.hpp"
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"

    Atams::Error_t error;

    error = initSingleCore(Atams::MapExample::memoryMap);

    if (error != Atams::ERROR_NONE)
    {
      // Handle error case
      printf("Atams Node initialisation error: ");
      printf(Atams::getErrorString(error));
      printf("\n");
    }
    ```
    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeInit.gif">
    </p>

- **Setting and Getting Variables:** 

    The Node library `Atams::getVar` and `Atams::setVar` functions can be used to read from and write to the Atams variable storage from the user application code. The functions will return an error if variable ID is outside of the Memory Map range, if the Memory Map has not be initialised successfully, or if the template argument variable type does not match the type specified for the variable in the Memory Map. A return of `Atams::ERROR_NONE` indicates a successful transfer.

    Getter Example:
    ```cpp
    using namespace Atams::MapExample;

    Atams::Error_t error;

    uint32_t exampleVar {0U};

    error = getVar(BlockExample::VAR_ID_EXAMPLE_GET, varToGet);
    ```

    <p align="center">
      <img height="500" src="Developer/Documentation/Images/NodeGetVar.gif">
    </p>

    Setter example:
    ```cpp
    using namespace Atams::MapExample;

    Atams::Error_t error;

    int8_t exampleVar {-1};

    error = setVar(BlockExample::VAR_ID_EXAMPLE_SET, exampleVar);
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

    The App Core must be initialised using the same Memory Map as the communications core. The following function from `Atams/Node/AppCore/AppCore.hpp` is used to initialise the application core. The function will validate the provided Memory Map, and then perform a blocking wait until the communication core init is complete. The function will return `Atams::ERROR_NONE` if initialisation is successful.

    ```cpp
    Atams::Error_t initSingleCore(const MemoryMap_t &memoryMap);
    ```

- **Setting and Getting Variables:**

    The application core `getVar` and `setVar` functions operate identically to the communications core versions. Atams handles variable storage locks to prevent race conditions from occurring if both cores attempt to access variables simultaneously.

### Platform Setup

The Node library is written to maximise compatibility with different user application setups. This includes single or multi-threaded applications running on single or dual-core microcontrollers, alongside polling or event-driven communications peripheral setups. Atams provides platform files that contain all the required user constants, function declarations, and empty function definitions. The required platform function definitions change depending upon the users application setup. Comments are provided above each constant and function definition to help with implementation. The following Node library files need to be completed by the user:

Single and dual-core:  
`Atams/Node/CommsCore/CommsPlatform.hpp`  
`Atams/Node/CommsCore/CommsPlatform.cpp`  

Dual-core only:  
`Atams/Node/AppCore/AppPlatform.hpp`  
`Atams/Node/AppCore/AppPlatform.cpp`  

- **Function Comments:** The following tags are present in the `.cpp` file function comments to indicate when completion of the definition is required. Non-applicable function definitions can be left empty; it is recommended to cast any unused function arguments to void to avoid compiler warnings.

| Comment Tag                        | Requirement Condition |  
| :--------------------------------- | :---------- |  
| ALL                                | Required for all platform setups. |  
| MULTI-THREAD                       | Required when Atams functions are used from multiple threads, and/or interrupt contexts, on the given core. | 
| DUAL-CORE                          | Required when Atams functions are used on multiple MCU cores. |
| POLLING COMMS                      | Required when the platform hardware needs to be polled in order to receive incoming bytes. |
| EVENT DRIVEN COMMS                 | Required when incoming bytes are received into an interrupt or event context separate from the context running the Atams comms update function. |
| MULTI-THREAD + EVENT DRIVEN COMMS  | Required when Atams::updateCommsBlocking() is used in combination with an event driven receive setup. |

- **Platform Byte Pre-Processing:** No pre-processing of incoming bytes is required at the platform level; all incoming bytes can be passed directly to Atams using the provided receive callback functions. The incoming bytes must be passed to Atams in order, and the same bytes should not be passed to Atams more than once.

- **Platform Reception Methods:** Atams packets are compatible with polling, character delimited, or idle line reception methods. If a user wants to use character delimited reception methods - the delimiter should be set to hex `0x00`. 

### Maximising Node Performance
Care should be taken when using Atams alongside user code containing high-priority device functions. If user device functions run at the same priority or higher than an Atams comms update function, the response time of the Node could be at least up to the length of time it takes these user functions to run. One solution for RTOS based systems can be to run the `Atams::updateCommsBlocking()` function in a higher priority thread than any user device functions. If this is not possible, a secondary core of a multi-core MCU can be dedicated to run Atams communications for the quickest Node response times. This becomes especially important when using the synchronous update cycle with the Atams Hub library.

### Building (CMake Example) - Coming Soon

### Building (STM32CubeIDE Dual-Core Example) - Coming soon


# Hub Library

The Hub library includes two key classes: a Node class, and a Bus class. 

**Node Class:** The Hub device user code must construct a Node class instance for each physical Node connected to the Hub. Each Node object must be initialised with a Memory Map from `Atams/Hub/Maps` that was generated with the Node Memory Map used to initialise the Node device. 

**Bus Class:** A Bus class instance is required to enable communication and variable exchange with the connected Nodes. Each Node object must be linked to the Bus instance that mirrors the physical connection.

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

References to Node objects can be passed to functions or objects in files that are not responsible for updating the Bus. The platform files must be filled out appropriately to use Node public functions (setVar, getVar, and updateRequestPattern etc.) safely across multiple threads. See [Hub Platform Implementation](#platform-implementation).

### Node Initialisation

### Bus Configuration Process

### Bus Initialisation Process

### Bus Update Cycle

### Platform Implementation


