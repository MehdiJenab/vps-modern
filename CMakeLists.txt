cmake_minimum_required(VERSION 3.20)

project(vlasov-poisson-solver
    VERSION 0.1.0
    DESCRIPTION "A modern C++ Vlasov-Poisson kinetic plasma solver"
    LANGUAGES CXX
)

# ==============================================================================
# Project Options
# ==============================================================================
option(VPS_BUILD_TESTS "Build unit tests" ON)
option(VPS_BUILD_BENCHMARKS "Build benchmarks" OFF)
option(VPS_BUILD_DOCS "Build documentation" OFF)
option(VPS_ENABLE_OPENMP "Enable OpenMP parallelization" ON)
option(VPS_ENABLE_MPI "Enable MPI parallelization" OFF)

# ==============================================================================
# C++ Standard and Compiler Settings
# ==============================================================================
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# Compiler Warnings (modern approach)
# ==============================================================================
add_library(vps_compiler_warnings INTERFACE)
target_compile_options(vps_compiler_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion
    >
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /permissive-
    >
)

# ==============================================================================
# Compiler Features (optimization, debug, etc.)
# ==============================================================================
add_library(vps_compiler_features INTERFACE)
target_compile_options(vps_compiler_features INTERFACE
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:
        -O3 -march=native
    >
    $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:
        -g -O0
    >
)

# ==============================================================================
# Find Dependencies
# ==============================================================================
if(VPS_ENABLE_OPENMP)
    find_package(OpenMP REQUIRED)
endif()

if(VPS_ENABLE_MPI)
    find_package(MPI REQUIRED)
endif()

if(VPS_BUILD_TESTS)
    # We'll use Google Test - fetched automatically
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()
    include(GoogleTest)
endif()

# ==============================================================================
# Add Source Directories
# ==============================================================================
add_subdirectory(src)

# ==============================================================================
# Installation (optional, for later)
# ==============================================================================
# include(GNUInstallDirs)
# TODO: Add install rules when ready

# ==============================================================================
# Print Configuration Summary
# ==============================================================================
message(STATUS "")
message(STATUS "=== Vlasov-Poisson Solver Configuration ===")
message(STATUS "Version:        ${PROJECT_VERSION}")
message(STATUS "C++ Standard:   ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "OpenMP:         ${VPS_ENABLE_OPENMP}")
message(STATUS "MPI:            ${VPS_ENABLE_MPI}")
message(STATUS "Tests:          ${VPS_BUILD_TESTS}")
message(STATUS "Benchmarks:     ${VPS_BUILD_BENCHMARKS}")
message(STATUS "============================================")
message(STATUS "")
