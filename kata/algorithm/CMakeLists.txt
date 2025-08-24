# CMakeLists.txt for kata/algorithm folder
# Contains various algorithm implementations and examples

# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")

# Exponential Moving Average (EMA) Algorithm
add_executable(ema ema.cpp)
target_compile_features(ema PRIVATE cxx_std_20)

# Linear Regression Algorithm
add_executable(linear_regression linear_regression.cpp)
target_compile_features(linear_regression PRIVATE cxx_std_20)

# Leaky Bucket Algorithm
add_executable(leaky_bucket leaky_bucket.cpp)
target_compile_features(leaky_bucket PRIVATE cxx_std_20)

# Token Bucket Algorithm
add_executable(token_bucket token_bucket.cpp)
target_compile_features(token_bucket PRIVATE cxx_std_20)

# Triple Moving Average (TMA) Algorithm
add_executable(tma tma.cpp)
target_compile_features(tma PRIVATE cxx_std_20)

# Triple Exponential Moving Average (TEMA) Algorithm
add_executable(tema tema.cpp)
target_compile_features(tema PRIVATE cxx_std_20)

# Set properties for all executables
set_target_properties(ema linear_regression leaky_bucket token_bucket tma tema
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Custom target for building all algorithm examples
add_custom_target(algorithm_examples ALL
    DEPENDS ema linear_regression leaky_bucket token_bucket tma tema
    COMMENT "Building all algorithm examples"
)

# Install targets
install(TARGETS ema linear_regression leaky_bucket token_bucket tma tema
    RUNTIME DESTINATION bin
)
