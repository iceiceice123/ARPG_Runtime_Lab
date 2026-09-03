cmake_minimum_required(VERSION 3.25)

project(ARPG_Runtime_Lab
    VERSION 0.1.0
    DESCRIPTION "ARPG Runtime Lab - an Unreal-independent C++ game runtime training project"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Global language settings
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Consistency requirement: every target in this tree (including third party
# libraries pulled by FetchContent) must use the same CRT flavour on MSVC,
# otherwise linking fails with LNK2038 runtime mismatch.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type" FORCE)
endif()

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(ARPG_BUILD_TESTS       "Build unit tests"            ON)
option(ARPG_BUILD_BENCHMARKS  "Build benchmarks"            ON)
option(ARPG_BUILD_APPS        "Build runtime executables"   ON)

# ---------------------------------------------------------------------------
# Modules / sub projects
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CompileOptions)
include(Dependencies)

add_subdirectory(Runtime)

if(ARPG_BUILD_APPS)
    add_subdirectory(Apps)
endif()

if(ARPG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

if(ARPG_BUILD_BENCHMARKS)
    add_subdirectory(Benchmarks)
endif()

message(STATUS "----------------------------------------------------------")
message(STATUS "ARPG Runtime Lab ${PROJECT_VERSION}")
message(STATUS "  Build type : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Tests      : ${ARPG_BUILD_TESTS}")
message(STATUS "  Benchmarks : ${ARPG_BUILD_BENCHMARKS}")
message(STATUS "  Apps       : ${ARPG_BUILD_APPS}")
message(STATUS "----------------------------------------------------------")
