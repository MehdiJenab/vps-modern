# ==============================================================================
# Particles Module Tests
# ==============================================================================

add_executable(test_particles
    test_particles.cpp
)

target_link_libraries(test_particles
    PRIVATE
        vps::particles
        GTest::gtest_main
)

# Register tests with CTest
gtest_discover_tests(test_particles)
