# CMakeLists.txt for kata/pattern folder
# Contains design pattern examples and implementations

# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")

# State Pattern Example
add_executable(state_pattern state_pattern.cpp state_pattern.h)
target_compile_features(state_pattern PRIVATE cxx_std_20)

# Overload Pattern Example
add_executable(overload_pattern overload_pattern.cpp)
target_compile_features(overload_pattern PRIVATE cxx_std_20)

# Time Range Queue Example
add_executable(time_range_queue time_range_queue.cpp)
target_compile_features(time_range_queue PRIVATE cxx_std_20)

# Set properties for all executables
set_target_properties(state_pattern overload_pattern time_range_queue
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Optional: Add custom target for building all pattern examples
add_custom_target(pattern_examples ALL
    DEPENDS state_pattern overload_pattern time_range_queue
    COMMENT "Building all pattern examples"
)

# Optional: Add install targets
install(TARGETS state_pattern overload_pattern time_range_queue
    RUNTIME DESTINATION bin
)