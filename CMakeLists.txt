# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(b_plus_generator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Define core library sources
set(CORE_SOURCES
    src/core/tree.c
    src/core/utils.c
)

# Create core library
add_library(bplus_core ${CORE_SOURCES})

# Create CLI executable
add_executable(b-plus-tree 
    src/cli/main.c
    src/cli/interface.c
)
target_link_libraries(b-plus-tree bplus_core)

# Create test executable
add_executable(run_tests 
    tests/unit/test_tree.c
)
target_link_libraries(run_tests bplus_core)

# Add test
add_test(NAME unit_tests COMMAND run_tests)