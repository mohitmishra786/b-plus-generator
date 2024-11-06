cmake_minimum_required(VERSION 3.10)
project(b_plus_generator C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Define core library sources
set(CORE_SOURCES
    src/core/tree.c
    src/core/utils.c
)

# Create core library
add_library(bplus_core ${CORE_SOURCES})

# Create CLI library
set(CLI_SOURCES
    src/cli/interface.c
)
add_library(bplus_cli ${CLI_SOURCES})
target_link_libraries(bplus_cli bplus_core)

# Create main executable
add_executable(b-plus-tree src/cli/main.c)
target_link_libraries(b-plus-tree bplus_cli bplus_core)

# Create test runner executable that runs all tests
add_executable(run_tests
    tests/unit/test_runner.c
    tests/unit/test_tree.c
    tests/unit/test_operations.c
    tests/unit/test_cli.c
)
target_link_libraries(run_tests bplus_core bplus_cli)

# Add the single test target
add_test(NAME all_tests COMMAND run_tests)