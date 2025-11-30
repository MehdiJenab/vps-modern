# ==============================================================================
# Main Application
# ==============================================================================

add_executable(vps_solver
    main.cpp
)

# Link all required modules
target_link_libraries(vps_solver
    PRIVATE
        vps::particles
        vps::grid
        vps_compiler_warnings
        vps_compiler_features
)
