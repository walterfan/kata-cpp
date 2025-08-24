#!/bin/bash

# Build script for kata-http using Conan
set -e

echo "Building kata-http with Conan..."

# Create build directory
mkdir -p build
cd build

# Install dependencies with Conan
echo "Installing dependencies with Conan..."
conan install .. --build=missing --output-folder=.

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# Build
echo "Building..."
cmake --build . --config Release

echo "Build completed successfully!"
echo "Executable is in: ../bin/chatgpt_demo"
