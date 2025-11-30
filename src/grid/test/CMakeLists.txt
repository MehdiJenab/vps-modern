# ==============================================================================
# Grid Module Tests
# ==============================================================================

add_executable(test_grid
    test_grid.cpp
)

target_link_libraries(test_grid
    PRIVATE
        vps::grid
        GTest::gtest_main
)

# Register tests with CTest
gtest_discover_tests(test_grid)
