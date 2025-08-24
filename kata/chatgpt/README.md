# Kata HTTP Client

This project contains HTTP client implementations using Conan for dependency management.

## Dependencies

- **OpenSSL 1.1.1** - SSL/TLS support
- **cURL 7.87.0** - HTTP client library
- **jsoncpp 1.9.5** - JSON parsing library

## Prerequisites

1. **Install Conan 2.x**:
   ```bash
   pip install conan
   ```

2. **Add Conan remote** (if not already added):
   ```bash
   conan remote add conancenter https://center.conan.io
   ```

## Building

### Option 1: Use the build script (Recommended)
```bash
cd kata/http
./build.sh
```

### Option 2: Manual build
```bash
cd kata/http
mkdir -p build
cd build

# Install dependencies
conan install .. --build=missing

# Configure and build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## What Gets Built

- **`chatgpt_demo`** - Full-featured OpenAI/DeepSeek API client using jsoncpp

## Executables Location

After building, the executable will be in the `bin/` directory (relative to the source).

## Configuration

The project uses Conan's CMake integration:
- `CMakeDeps` - Generates CMake find modules for dependencies
- `CMakeToolchain` - Generates CMake toolchain file

## Troubleshooting

### If Conan can't find packages:
```bash
conan remote list
conan search openssl/1.1.1
conan search curl/7.87.0
```

### If build fails:
```bash
# Clean and retry
rm -rf build
./build.sh
```

### Check Conan version:
```bash
conan --version
# Should be 2.x.x
```
