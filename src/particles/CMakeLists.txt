# ==============================================================================
# Particles Module
# ==============================================================================
# This module provides the SoA (Struct of Arrays) container for phase-space points

add_library(vps_particles
    src/particles.cpp
)

# Create alias for consistent usage
add_library(vps::particles ALIAS vps_particles)

# Include directories
target_include_directories(vps_particles
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(vps_particles
    PRIVATE
        vps_compiler_warnings
        vps_compiler_features
)

# OpenMP support
if(VPS_ENABLE_OPENMP)
    target_link_libraries(vps_particles PUBLIC OpenMP::OpenMP_CXX)
    target_compile_definitions(vps_particles PUBLIC VPS_ENABLE_OPENMP)
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
