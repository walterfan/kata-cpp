# CMakeLists.txt for kata/http folder
# Contains HTTP client implementations using Conan dependencies

# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")

# Include Conan-generated files
include(${CMAKE_CURRENT_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)
include(${CMAKE_CURRENT_BINARY_DIR}/conandeps_legacy.cmake)

# Find required packages using Conan
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp REQUIRED)

# ChatGPT Demo Client (with jsoncpp)
add_executable(chatgpt_demo chatgpt_demo.cpp)
target_compile_features(chatgpt_demo PRIVATE cxx_std_20)

# Link libraries
target_link_libraries(chatgpt_demo PRIVATE 
    CURL::libcurl
    openssl::openssl
    JsonCpp::JsonCpp
)

# Set properties for executables
set_target_properties(chatgpt_demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

# Custom target for building all HTTP examples
add_custom_target(http_examples ALL
    DEPENDS chatgpt_demo
    COMMENT "Building all HTTP examples"
)

# Install targets
install(TARGETS chatgpt_demo
    RUNTIME DESTINATION bin
)
