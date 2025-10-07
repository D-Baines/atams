
# Introduction

Atams is a C++ framework designed for use in embedded systems where a single central device ("Hub") communicates with and manages multiple distributed devices ("Nodes"). It simplifies variable sharing, synchronisation, and non-volatile storage, making it ideal for robotics, automation, and control applications. 

Given the example of a self-balancing robot, a Hub device might read from an IMU Node to estimate body angle, write to motor controller Nodes to maintain balance, and read from wheel encoder Nodes for odometry. Depending on the application, it might be important to synchronize data exchanges across all Nodes, and/or save configuration settings to each Node.

Atams is split into three sections: Autogen tooling, a Hub library, and a Node library.

To accelerate development, Atams provides tools for auto-generating device-specific "Memory Maps" for the Hub and Node libraries. These Memory Maps hold information for each Node variable, including type, access permission, storage option, and factory default. 

The Atams Hub and Node libraries provide the following commonly required features for distributed embedded systems:
- Type-safe variable exchange with multiple Nodes from a single Hub.
- Support for multiple groups of Nodes on separate buses from a single Hub.
- Optional synchronous processing to ensure all Node variable interactions on the same bus occur simultaneously.
- Dynamic packet generation to fascilitate adaptive variable exchange.
- Packet checksumming and framing to ensure safe variable transport.
- Endianness agnostic variable transport to maximise platform compatibility.
- Support for connecting each Node to multiple buses with different hardware peripherals.
- Hub-triggered saving of selected variables to non-volatile memory on Nodes.
- Hub-triggered restoration of factory defaults to non-volatile memory on Nodes.
- Checksumming and version tracking of non-volatile memory storage.
- Coordination of Memory Map versioning between Node and Hub implementations to ensure safe variable sharing.
- Node watchdog timers to trigger safe state entry should Hub communications fail.
- Hub-triggered communication bitrate changes on Nodes.
- Hub-initiated Node resets.


# Communications Hardware Requirements
Atams primarily targets multi-drop buses (such as RS485, CAN FD, CAN XL, 10Base-T1S Ethernet) to simplify wiring in multi-device systems.

- **Broadcast capability:** All Hub and Node devices on an Atams Bus must be able to receive all packets transmitted by any other device. If using non-multi-drop hardware, multi-casting or broadcasting must be available.
- **Self-reception:** Atams safely handles cases where a device receives its own messages.
- **Bus arbitration:** Hardware-level bus arbitration is not required, but can be used alongside Atams arbitration to improve robustness.
- **Packet size:** Atams is not compatible with hardware that has a small maximum packet size (such as classic CAN). A minimum *maximum* packet size of 64 bytes or greater is recommended.


# Platform Requirements
Atams is compatible with any platform that meets the following requirements:
- Single read and write operations to variables of type `uint8_t` must be inherently atomic.
- 32 bit and above.
- The size of a float must be 4 bytes and meet the IEC559 standard for bit representation.


# Language Standard
Atams is compatible with C++17 and above, does not use any non-ISO C++ features, and is tested with the following compiler flags:  
`-Wall -Wextra -Wpedantic -Wswitch-default -Wunreachable-code -Wformat`


# Memory Maps
Memory Map C++ files are required for operation of the Node library and Node objects in the Hub library. They provide Atams with the information it needs to initialise and regulate interactions with the Atams user variable storage.

- **Data Blocks:** Memory Maps are divided into sections, or "Data Blocks", to enable users to loosely limit access to certain blocks of variables to specific user files.
- **Variable Information:** Each Data Block contains an enum list of variable IDs (`VarID_t`) and an associated array of structs holding variable information (`VarInfo_t`). The information held for each variable includes its type, access permission, and non-volatile storage option. 
- **Initialisation Functions:** For the Node library, the Memory Map also holds function references used to initialise the Universal Data Block, and restore factory defaults when requested. 
- **The Universal Data Block:** The Universal Data Block is included in all Memory Maps, and is used by the Atams libraries to implement Atams functionality.


# Auto-generation
GUI tooling is provided for auto-generating device specific Memory Map C++ files for use in the Hub and Node libraries. 

### Memory Map Tables
To use the auto-generation application, a user must fill out an Atams Memory Map table in the provided `.xlsx` format to define the required information for each device variable. The table is used by the autogen tool to create the Memory Map C++ files, and doubles as documentation for the device Memory Map. The template is available at the following directory path:  
`Atams/Autogen/TemplateMap.xlsx`.

- **Data Blocks:** Each Data Block should be defined in a separate sheet of the `.xlsx` file. Copy the original template sheet to maintain the correct table formatting. Data Block names will be taken from the name of each sheet and converted to `PascalCase` namespaces with a `Block` prefix in the generated C++ files.

- **Columns:**
The following columns of information are provided. Only the required columns are necessary for Atams operation - these columns must be completed for successful auto-generation:

| Column Name     | Requirement  | Description    |
| :-------------: | :----------: | :------------: |
| Var ID          | Required     | Name of the device variable. This can be provided with any formatting, but will be converted to `SCREAMING_SNAKE_CASE` with a `VAR_` prefix in the Data Block C++ file `VarID_t` enum list. This ID will be used as an input argument for Atams functions so it is worth keeping naming concise where possible. |
| Data Type       | Required     | Pick from the dropdown list of variable types to define the variable type. Interacting with this variable with any other type will return errors from Atams functions. Atams is compatible with the provided types only. |
| External Access | Required     | Pick from RO (Read Only), or RW (Read/Write). RO means a Hub device can only read from the selected Node variable. RW means the Hub device can read from and write to the selected Node variable. |
| Units           | Optional     | Provided only for user documentation purposes. |
| Min Limit       | Optional     | Provided only for user documentation purposes. |
| Max Limit       | Optional     | Provided only for user documentation purposes. |
| Default         | Optional     | Defines the value for the variable to be set to on startup and during a factory restore operation. Values stored in non-volatile memory will overwrite default values on startup. If this column is left blank, the variable will be zero-initialised. |
| NVM Storage     | Optional     | Defines whether the variable should be stored in non-volatile memory or not during a STORE ALL operation. If this column is left blank, the variable will not be stored in non-volatile memory. |
| Description     | Optional     | Provided only for user documentation purposes. |
| Widget          | Future Scope | Provided for future Atams application compatibility. |


### GUI Application
Follow these steps to start auto-generating Memory Maps with the Atams Memory Map Generator GUI.
- **Step 1:** Install the library requirements. Python library requirements for the autogen application can be installed by running the following command in a terminal:  
    `pip install -r PATH_TO_USER_PROJECT_FOLDER/Atams/Autogen/requirements.txt`.   
- **Step 2:** Open the application. The tool can opened by running `AtamsAutogen.py` from the `Atams/Autogen` folder.
- **Step 3:** Browse and select the Memory Map Table `.xlsx` file specific to the Node device in development.
- **Step 4:** Browse and select the Atams Node library directory path used by the Node device C++ project. The auto-generation tool will place a Memory Map folder in the `Atams/Node/Maps` folder, containing all the generated Data Block and Memory Map files specific to the Node library.
- **Step 5:** Browse and select the Atams Hub library directory path used by the Hub device C++ project. The auto-generation tool will place a Memory Map folder in the `Atams/Hub/Maps` folder, containing all generated Data Block and Memory Map files specific to the Hub library.
- **Step 6:** Enter a name for the Memory Map and click the *Generate* button. A popup may appear with a warning if there is a risk of over-writing previously generated Memory Map files. Generation status information will be provided towards the bottom of the application. Memory Map names will be converted to `PascalCase` namespaces with a `Map` prefix in the generated C++ files.

> [!NOTE]  
> The application will remember any selected paths when restarted.

> [!TIP]  
> If the Hub and Node are not being developed on the same device, consider generating to local repositories and using version control to efficiently synchronise Memory Map files between development environments. Atams Bus initialisation functions will effectively catch and return errors if the Hub and Node Memory Maps are not synchronised.


# Using the Generated Memory Map Files
Once the Memory Map C++ have been generated, they are ready to be used in the Node and Hub libraries.

- **Variable IDs:** Each of the generated Data Block `.hpp` files includes an enum list of the variable IDs associated with that Block. These variable IDs are used as input arguments to Atams functions to specify variable access. An example list might look like this:
    ```cpp
    enum VarID_t: uint16_t
    {
      VAR_EXAMPLE_1 = 28U,
      VAR_EXAMPLE_2 = 29U,
      VAR_EXAMPLE_3 = 30U,
    };
    ```
- **Full Map Access:** If a user file needs access to all variables from an Atams Memory Map, it should include the associated Map file from the associated Map folder found in either `Atams/Node/Maps` or `Atams/Hub/Maps`. The following include example would be appropriate for user files using the Node library: 
    ```cpp
    #include "Atams/Node/Maps/MapExample/MapExample.hpp"
    ```
- **Limited Data Block Access:** If a user file only requires access to a specific Data Block, it should only include the associated Block file found in the associated Map folder. The following example would be appropriate for user files using the Node Library:
    ```cpp
    #include "Atams/Node/Maps/MapExample/BlockExample1.hpp"
    ```
- **Variable ID Access:** With the appropriate Map or Block files have been included, an example variable ID could look like this:  
    ```cpp
    Atams::MapExample::BlockExample::VAR_EXAMPLE_1
    ```
    This nested namespace access is verbose and results in long variable names. It is recommended to use `using namespace` to access the required Block, but limit the use of `using namespace` to the smallest sensible scope. It is not recommended to use `using namespace` down to the Data Block namespace level to avoid accidental use of variables from other blocks.  

    ```cpp
    uint8_t variableToWrite {0U};

    // Recommended 
    using namespace Atams::MapExample;

    Atams::write(BlockExample1::VAR_EXAMPLE_1, userVariableToWrite);
    
    // Not Recommended 
    using namespace Atams::MapExample::BlockExample1;
    using namespace Atams::MapExample::BlockExample2;

    Atams::write(VAR_EXAMPLE_1, userVariableToWrite); // Variable could belong to BlockExample1 or 2
    ```
> [!CAUTION]   
> Block-only header inclusion only provides a loose limit on variable access. The compiler will provide warnings should the user try to use a variable ID that is not provided by the included file, or if a variable ID does not exist in the explicitly specified Block namespace. However, users should be cautious of hidden includes of other Data Block or Memory Map files, or using raw `uint16_t` variables instead of the provided enum named constants as input arguments to Atams functions. Atams functions will always return an error if the variable ID is outside the bounds of the entire Memory Map.


# Node Library

### Includes & Core Setup
The Node library is compatible with single and dual-core micro-controllers. The user can decide which setup is more appropriate for their use case, but should aim to minimise Atams communications response times for the best Atams Bus performance.

- **Minimising Response Times:** Care should be taken when using Atams alongside user code containing lengthy, high-priority, device functions. The definition of "lengthy" depends on the use case. If user device functions run at the same priority or higher than an Atams comms update function, the response time of the Node could be at least up to the length of time it takes these user functions to run. One solution for RTOS based systems can be to run the `Atams::updateCommsBlocking(void)` function in a higher priority thread than any user device functions. If this is not possible, the second core of a dual-core MCU can be used to run Atams communications for the quickest Node response times.

- **Single Core Includes:** If the Atams Node library is used on a single core platform, all the necessary Node device functions become accessible with the following header include:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    ```
- **Dual-Core Includes:** If a dual-core setup is used, files with code running on the user application core should include the following header file:
    ```cpp
    #include "Atams/Node/AppCore/AppCore.hpp"
    ```
    Files with code running on the core used for Atams communications should then include the following header file:
    ```cpp
    #include "Atams/Node/CommsCore/CommsCore.hpp"
    ```

### Comms Core (Single & Dual-Core)
- **Responsibility:**
- **Initialisation:**
- **Comms Update:**
- **Read and Write:**

### App Core (Dual-Core Only)
- **Responsibility:**
- **Initialisation:**
- **Read and Write:**

### Platform Setup
The Node library is written to maximise compatibility with different user application setups. This includes single or multi-threaded applications running on single or dual-core microcontrollers, alongside polling or event-driven communications peripheral setups. Atams provides platform files that contain all the required user constants, function declarations, and empty function definitions. It is the users responsibility to complete these platform files. The required platform function definitions change depending upon the users core, threading, and communications peripheral setups. Platform file comments are provided above each constant and function definition to aid with correct implementation. 

- **Function Comments:** The following tags are present in the function definition comments to indicate when the definition is required. Non-applicable function definitions can be left empty; it is recommended to cast any unused function arguments to void to avoid compiler warnings.

| Comment Tag                        | Requirement Condition |  
| :--------------------------------- | :---------- |  
| ALL                                | Required for all platform setups. |  
| MULTI-THREAD                       | Required where Atams variable storage could be accessed concurrently from multiple threads, and/or interrupt contexts, on the given core. It is best to assume that all Atams functions result in variable storage access. | 
| DUAL-CORE                          | Required where Atams variable storage could be accessed concurrently from multiple MCU cores. |
| POLLING COMMS                      | Required where the platform hardware needs to be polled in order to receive incoming bytes. |
| EVENT DRIVEN COMMS                 | Required where bytes are received into an interrupt or event context separate from the context running an Atams comms update function. |
| THREAD BLOCK + EVENT DRIVEN COMMS  | Required where Atams::updateCommsBlocking() is used in combination with an event driven receive setup. |

- **Platform Byte Pre-Processing:** No pre-processing of incoming bytes is required at the platform level; all incoming bytes can be passed directly to Atams using the provided receive callback functions. The incoming bytes must be passed to Atams in order, and the same bytes should not be passed to Atams more than once.

- **Platform Reception Methods:** Atams packets are compatible with polling, character delimited, or idle line reception methods. If a user wants to use character delimited reception methods - the delimiter should be set to hex `0x00`. 

### Building (CMake Example) - Coming Soon

### Building (STM32CubeIDE Dual-Core Example)


# Hub Library

### Includes

### Node Initialisation

### Bus Setup

### Bus Update Cycle

### Platform Implementation






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