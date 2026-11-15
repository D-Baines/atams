cmake_minimum_required(VERSION 3.14)

# Atams host tests. A separate top-level project: configure this folder on its own to
# build the tests. Projects that use Atams add Hub/ or Node/ and never build these.
project(AtamsHostTests
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# Atams root folder, which library headers include from
get_filename_component(ATAMS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# Atams libraries, built with the harness Platform files and the tracked test maps.
# The Platform files include harness headers as "Harness/...", so this folder is added
# to the libraries' include path. Harness sources are compiled into the executable.
set(ATAMS_HUB_PLATFORM_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Harness/Platform")
set(ATAMS_HUB_MAPS_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/Maps/Hub")
set(ATAMS_HUB_USER_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory("${ATAMS_ROOT_DIR}/Hub" AtamsHub)

set(ATAMS_NODE_PLATFORM_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Harness/Platform")
set(ATAMS_NODE_MAPS_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/Maps/Node")
set(ATAMS_NODE_USER_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
set(ATAMS_NODE_CORE          COMMS)
add_subdirectory("${ATAMS_ROOT_DIR}/Node" AtamsNode)

add_executable(AtamsHostTests
    Harness/Assertions.cpp
    Harness/BinarySemaphore.cpp
    Harness/Router.cpp
    Harness/TestRunner.cpp
    Harness/TestUtilities.cpp
    TestList.cpp
    NodeTestMap.cpp
    HarnessSelfTests.cpp
    RouterTests.cpp
    # The router decodes logged packets with the Shared COBS utilities
    "${ATAMS_ROOT_DIR}/Shared/Utilities/COBS.cpp"
)

# Test code includes harness headers as "Harness/..." and library headers as "Shared/..."
target_include_directories(AtamsHostTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${ATAMS_ROOT_DIR}"
)

target_link_libraries(AtamsHostTests PRIVATE Atams::Hub Atams::NodeComms Threads::Threads)

foreach(ATAMS_TESTS_TARGET AtamsHostTests AtamsHub AtamsNodeComms)
    if(MSVC)
        target_compile_options(${ATAMS_TESTS_TARGET} PRIVATE /W4)
    else()
        target_compile_options(${ATAMS_TESTS_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

enable_testing()
add_test(NAME AtamsHostTests COMMAND AtamsHostTests)

# Ends a hung run (e.g. a deadlock) when the tests run unattended through ctest
set_tests_properties(AtamsHostTests PROPERTIES TIMEOUT 300)
