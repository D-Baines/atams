cmake_minimum_required(VERSION 3.10.0)

project(AtamsNode
        VERSION 1.0.0
        LANGUAGES C CXX)

# Two library targets are available, selected with ATAMS_NODE_CORE below. Link the one
# matching the core being built:
#   Atams::NodeComms - single-core Nodes, and the Comms Core of dual-core Nodes
#   Atams::NodeApp   - the App Core of dual-core Nodes

# Required: folder holding the application's Platform files: SharedPlatform.hpp, plus
# CommsPlatform.hpp/.cpp and/or AppPlatform.hpp/.cpp for the Cores being built. Copy the
# templates in Node/Platform into your project, complete them, and set this to that folder
# as an absolute path, before add_subdirectory() or with -DATAMS_NODE_PLATFORM_DIR=<path>.
# Dual-core projects built as separate trees must point every tree at the same folder.
if(NOT DEFINED ATAMS_NODE_PLATFORM_DIR)
    message(FATAL_ERROR "ATAMS_NODE_PLATFORM_DIR is not set. Copy the templates in "
                        "${CMAKE_CURRENT_SOURCE_DIR}/Platform into your project, complete "
                        "them, and set ATAMS_NODE_PLATFORM_DIR to that folder.")
endif()

# Optional: folder holding the generated Node Memory Maps. All .cpp and .hpp files under it
# are compiled. Defaults to Node/Maps, and is set the same way as ATAMS_NODE_PLATFORM_DIR.
if(NOT DEFINED ATAMS_NODE_MAPS_DIR)
    set(ATAMS_NODE_MAPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Maps")
endif()

# Optional: which Node Core targets to create. Defaults to COMMS.
#   COMMS - Atams::NodeComms only (single-core Nodes, or a Comms Core build)
#   APP   - Atams::NodeApp only (an App Core build)
#   BOTH  - both targets, for one build tree that builds both Cores
if(NOT DEFINED ATAMS_NODE_CORE)
    set(ATAMS_NODE_CORE COMMS)
endif()

if(NOT ATAMS_NODE_CORE MATCHES "^(COMMS|APP|BOTH)$")
    message(FATAL_ERROR "ATAMS_NODE_CORE is '${ATAMS_NODE_CORE}'. Valid values are COMMS, APP or BOTH.")
endif()

if(ATAMS_NODE_CORE MATCHES "^(COMMS|BOTH)$")
    set(ATAMS_NODE_BUILD_COMMS TRUE)
else()
    set(ATAMS_NODE_BUILD_COMMS FALSE)
endif()

if(ATAMS_NODE_CORE MATCHES "^(APP|BOTH)$")
    set(ATAMS_NODE_BUILD_APP TRUE)
else()
    set(ATAMS_NODE_BUILD_APP FALSE)
endif()

# Check the Platform files for the selected Cores exist, rather than failing later with a
# missing source file
set(ATAMS_NODE_REQUIRED_PLATFORM_FILES SharedPlatform.hpp)
if(ATAMS_NODE_BUILD_COMMS)
    list(APPEND ATAMS_NODE_REQUIRED_PLATFORM_FILES CommsPlatform.hpp CommsPlatform.cpp)
endif()
if(ATAMS_NODE_BUILD_APP)
    list(APPEND ATAMS_NODE_REQUIRED_PLATFORM_FILES AppPlatform.hpp AppPlatform.cpp)
endif()

foreach(ATAMS_NODE_PLATFORM_FILE ${ATAMS_NODE_REQUIRED_PLATFORM_FILES})
    if(NOT EXISTS "${ATAMS_NODE_PLATFORM_DIR}/${ATAMS_NODE_PLATFORM_FILE}")
        message(FATAL_ERROR "ATAMS_NODE_CORE is ${ATAMS_NODE_CORE}, but ${ATAMS_NODE_PLATFORM_FILE} "
                            "is missing from ATAMS_NODE_PLATFORM_DIR (${ATAMS_NODE_PLATFORM_DIR}).")
    endif()
endforeach()

# Atams root folder, which library and Memory Map headers include from
get_filename_component(ATAMS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# Shared utilities used by both targets
set(ATAMS_NODE_SHARED_SOURCES
    ../Shared/AtamsTypedefs.hpp
    ../Shared/Maps/BlockUniversal.hpp
    ../Shared/Utilities/CircularBuffer.hpp
    ../Shared/Utilities/AtamsUtilities.hpp
    ../Shared/Utilities/AtamsUtilities.cpp
    ../Shared/Utilities/COBS.hpp
    ../Shared/Utilities/COBS.cpp
    ../Shared/Utilities/CRC32.hpp
    ../Shared/Utilities/CRC32.cpp
    Developer/NodeTypedefs.hpp
    Developer/NodeUtilities.hpp
    Developer/NodeUtilities.cpp
    "${ATAMS_NODE_PLATFORM_DIR}/SharedPlatform.hpp"
)

# Explicitly list all source and header files for the Comms Core
set(ATAMS_NODE_COMMS_SOURCES
    CommsCore/CommsCore.hpp
    CommsCore/CommsCore.cpp
    Developer/CircularBuffer.hpp
    Developer/FramingConstants.hpp
    Developer/NVMUnitHandler.hpp
    Developer/NVMUnitHandler.cpp
    Developer/UniversalBlockManager.hpp
    Developer/UniversalBlockManager.cpp
    Developer/WatchdogHandler.hpp
    Developer/WatchdogHandler.cpp
    "${ATAMS_NODE_PLATFORM_DIR}/CommsPlatform.hpp"
    "${ATAMS_NODE_PLATFORM_DIR}/CommsPlatform.cpp"
)

# Explicitly list all source and header files for the App Core
set(ATAMS_NODE_APP_SOURCES
    AppCore/AppCore.hpp
    AppCore/AppCore.cpp
    "${ATAMS_NODE_PLATFORM_DIR}/AppPlatform.hpp"
    "${ATAMS_NODE_PLATFORM_DIR}/AppPlatform.cpp"
)

# Add all .cpp and .hpp files in the Maps folder and its subdirectories
file(GLOB_RECURSE ATAMS_NODE_MAPS_SOURCES
    "${ATAMS_NODE_MAPS_DIR}/*.cpp"
    "${ATAMS_NODE_MAPS_DIR}/*.hpp"
)

set(ATAMS_NODE_TARGETS "")

if(ATAMS_NODE_BUILD_COMMS)
    add_library(AtamsNodeComms ${ATAMS_NODE_SHARED_SOURCES} ${ATAMS_NODE_COMMS_SOURCES} ${ATAMS_NODE_MAPS_SOURCES})
    list(APPEND ATAMS_NODE_TARGETS AtamsNodeComms)

    # Selects the Comms Core includes in the generated Memory Map files
    target_compile_definitions(AtamsNodeComms PRIVATE COMMS_CORE_DETECT=1)
endif()

if(ATAMS_NODE_BUILD_APP)
    add_library(AtamsNodeApp ${ATAMS_NODE_SHARED_SOURCES} ${ATAMS_NODE_APP_SOURCES} ${ATAMS_NODE_MAPS_SOURCES})
    list(APPEND ATAMS_NODE_TARGETS AtamsNodeApp)
endif()

# Optional extras for the Platform files, applied to every Node target:
#   ATAMS_NODE_USER_INCLUDES - include directories for dependencies without a CMake target
#   ATAMS_NODE_USER_LIBS     - libraries or CMake targets the Platform files depend on
#                              (a target's include directories come with it)
#   ATAMS_NODE_USER_SOURCES  - additional Platform source files
foreach(ATAMS_NODE_TARGET ${ATAMS_NODE_TARGETS})
    target_compile_features(${ATAMS_NODE_TARGET} PUBLIC cxx_std_17)

    # Public because the Core headers include SharedPlatform.hpp, and applications include
    # both those and the Memory Map headers
    target_include_directories(${ATAMS_NODE_TARGET} PUBLIC
        "${ATAMS_NODE_PLATFORM_DIR}"
        "${ATAMS_ROOT_DIR}"
    )

    if(ATAMS_NODE_USER_INCLUDES)
        target_include_directories(${ATAMS_NODE_TARGET} PUBLIC "${ATAMS_NODE_USER_INCLUDES}")
    endif()
    if(ATAMS_NODE_USER_LIBS)
        target_link_libraries(${ATAMS_NODE_TARGET} PUBLIC ${ATAMS_NODE_USER_LIBS})
    endif()
    if(ATAMS_NODE_USER_SOURCES)
        target_sources(${ATAMS_NODE_TARGET} PRIVATE ${ATAMS_NODE_USER_SOURCES})
    endif()
endforeach()

if(ATAMS_NODE_BUILD_COMMS)
    add_library(Atams::NodeComms ALIAS AtamsNodeComms)
endif()
if(ATAMS_NODE_BUILD_APP)
    add_library(Atams::NodeApp ALIAS AtamsNodeApp)
endif()
