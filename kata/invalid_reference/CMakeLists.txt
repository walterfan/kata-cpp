cmake_minimum_required(VERSION 3.10)

# Project name
project(InvalidReferenceDemo)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(demo demo.cpp)

# Optional: Add compiler flags for better debugging and warnings
target_compile_options(demo PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -g>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -g>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Optional: Set output directory
set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
