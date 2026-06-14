cmake_minimum_required(VERSION 3.10.0)

project(AtamsHub 
        VERSION 1.0.0
        LANGUAGES C CXX)

# Explicitly list all source and header files for the Hub implementation
set(ATAMS_HUB_LIB_SOURCES
    Bus.hpp
    Bus.cpp
    Node.hpp
    Node.cpp
    Platform.hpp
    Platform.cpp
    NodeActions.hpp
    NodeActions.cpp
    Developer/CircularBuffer.hpp
    Developer/FramingConstants.hpp
    Developer/NodeCallbackHandler.hpp
    Developer/WriteList.hpp
    Developer/WriteList.cpp
    ../Shared/AtamsTypedefs.hpp
    ../Shared/Maps/BlockUniversal.hpp
    ../Shared/Maps/BlockUniversal.cpp
    ../Shared/Utilities/CircularBuffer.hpp
    ../Shared/Utilities/AtamsUtilities.hpp
    ../Shared/Utilities/AtamsUtilities.cpp
    ../Shared/Utilities/COBS.hpp
    ../Shared/Utilities/COBS.cpp
    ../Shared/Utilities/CRC32.hpp
    ../Shared/Utilities/CRC32.cpp
)

# Add all .cpp and .hpp files in Atams/Hub/Maps and its subdirectories
file(GLOB_RECURSE ATAMS_HUB_MAPS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Maps/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Maps/*.hpp"
)

set(ATAMS_HUB_SOURCES
    ${ATAMS_HUB_LIB_SOURCES}
    ${ATAMS_HUB_MAPS_SOURCES}
)

add_library(${PROJECT_NAME} ${ATAMS_HUB_SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Add user-supplied include directories, libraries, and sources if set
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
