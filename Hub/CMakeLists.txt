cmake_minimum_required(VERSION 3.10.0)

project(AtamsHub 
        VERSION 1.0.0
        LANGUAGES C CXX)

# Required: folder holding the application's HubPlatform.hpp and HubPlatform.cpp. Copy the
# templates in Hub/Platform into your project, complete them, and set this to that folder
# as an absolute path, before add_subdirectory() or with -DATAMS_HUB_PLATFORM_DIR=<path>.
if(NOT DEFINED ATAMS_HUB_PLATFORM_DIR)
    message(FATAL_ERROR "ATAMS_HUB_PLATFORM_DIR is not set. Copy the templates in "
                        "${CMAKE_CURRENT_SOURCE_DIR}/Platform into your project, complete "
                        "them, and set ATAMS_HUB_PLATFORM_DIR to that folder.")
endif()

# Optional: folder holding the generated Hub Memory Maps. All .cpp and .hpp files under it
# are compiled. Defaults to Hub/Maps, and is set the same way as ATAMS_HUB_PLATFORM_DIR.
if(NOT DEFINED ATAMS_HUB_MAPS_DIR)
    set(ATAMS_HUB_MAPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Maps")
endif()

# Atams root folder, which library and Memory Map headers include from
get_filename_component(ATAMS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# Explicitly list all source and header files for the Hub implementation
set(ATAMS_HUB_LIB_SOURCES
    Bus.hpp
    Bus.cpp
    Node.hpp
    Node.cpp
    NodeActions.hpp
    NodeActions.cpp
    Developer/CircularBuffer.hpp
    Developer/FramingConstants.hpp
    Developer/NodeCallbackHandler.hpp
    Developer/WriteList.hpp
    Developer/WriteList.cpp
    ../Shared/AtamsTypedefs.hpp
    ../Shared/Maps/BlockUniversal.hpp
    ../Shared/Utilities/CircularBuffer.hpp
    ../Shared/Utilities/AtamsUtilities.hpp
    ../Shared/Utilities/AtamsUtilities.cpp
    ../Shared/Utilities/COBS.hpp
    ../Shared/Utilities/COBS.cpp
    ../Shared/Utilities/CRC32.hpp
    ../Shared/Utilities/CRC32.cpp
)

set(ATAMS_HUB_PLATFORM_SOURCES
    "${ATAMS_HUB_PLATFORM_DIR}/HubPlatform.hpp"
    "${ATAMS_HUB_PLATFORM_DIR}/HubPlatform.cpp"
)

# Add all .cpp and .hpp files in the Maps folder and its subdirectories
file(GLOB_RECURSE ATAMS_HUB_MAPS_SOURCES
    "${ATAMS_HUB_MAPS_DIR}/*.cpp"
    "${ATAMS_HUB_MAPS_DIR}/*.hpp"
)

set(ATAMS_HUB_SOURCES
    ${ATAMS_HUB_LIB_SOURCES}
    ${ATAMS_HUB_PLATFORM_SOURCES}
    ${ATAMS_HUB_MAPS_SOURCES}
)

add_library(${PROJECT_NAME} ${ATAMS_HUB_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Public because Bus.hpp and Node.hpp include HubPlatform.hpp, and applications include
# both those and the Memory Map headers
target_include_directories(${PROJECT_NAME} PUBLIC
    "${ATAMS_HUB_PLATFORM_DIR}"
    "${ATAMS_ROOT_DIR}"
)

# Optional extras for the Platform files:
#   ATAMS_HUB_USER_INCLUDES - include directories for dependencies without a CMake target
#   ATAMS_HUB_USER_LIBS     - libraries or CMake targets the Platform files depend on
#                             (a target's include directories come with it)
#   ATAMS_HUB_USER_SOURCES  - additional Platform source files
if(ATAMS_HUB_USER_INCLUDES)
    target_include_directories(${PROJECT_NAME} PUBLIC "${ATAMS_HUB_USER_INCLUDES}")
endif()
if(ATAMS_HUB_USER_LIBS)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${ATAMS_HUB_USER_LIBS})
endif()
if(ATAMS_HUB_USER_SOURCES)
    target_sources(${PROJECT_NAME} PRIVATE ${ATAMS_HUB_USER_SOURCES})
endif()

add_library(Atams::Hub ALIAS AtamsHub)
