# ==============================================================================
# Grid Module
# ==============================================================================
# This module provides the spatial grid for the simulation domain

add_library(vps_grid
    src/grid.cpp
)

# Create alias for consistent usage
add_library(vps::grid ALIAS vps_grid)

# Include directories
target_include_directories(vps_grid
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(vps_grid
    PRIVATE
        vps_compiler_warnings
        vps_compiler_features
)

# OpenMP support
if(VPS_ENABLE_OPENMP)
    target_link_libraries(vps_grid PUBLIC OpenMP::OpenMP_CXX)
    target_compile_definitions(vps_grid PUBLIC VPS_ENABLE_OPENMP)
endif()

# ==============================================================================
# Tests
# ==============================================================================
if(VPS_BUILD_TESTS)
    add_subdirectory(test)
endif()

# ==============================================================================
# Benchmarks
# ==============================================================================
if(VPS_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()
