cmake_minimum_required(VERSION 3.10)
project(kata_apr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(PkgConfig REQUIRED)
pkg_check_modules(APR REQUIRED apr-1)
pkg_check_modules(APRUTIL REQUIRED apr-util-1)

# pkg-config on macOS reports /usr/include/apr-1, which lives in the SDK, not on disk.
set(APR_RESOLVED_INCLUDES "")
foreach(dir IN LISTS APR_INCLUDE_DIRS APRUTIL_INCLUDE_DIRS)
    if(dir AND EXISTS "${dir}")
        list(APPEND APR_RESOLVED_INCLUDES "${dir}")
    endif()
endforeach()
list(REMOVE_DUPLICATES APR_RESOLVED_INCLUDES)

if(NOT APR_RESOLVED_INCLUDES)
    if(NOT CMAKE_OSX_SYSROOT)
        execute_process(
            COMMAND xcrun --show-sdk-path
            OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
    endif()
    if(CMAKE_OSX_SYSROOT AND EXISTS "${CMAKE_OSX_SYSROOT}/usr/include/apr-1")
        set(APR_RESOLVED_INCLUDES "${CMAKE_OSX_SYSROOT}/usr/include/apr-1")
    endif()
endif()

if(NOT APR_RESOLVED_INCLUDES)
    message(FATAL_ERROR "Could not find apr.h (pkg-config dirs: ${APR_INCLUDE_DIRS})")
endif()

set(APR_EXAMPLES
    01_platform
    02_memory
    03_file
    04_thread
    05_socket
    06_time
    07_atomic_shm
    08_iconv
    09_tables
)

foreach(name IN LISTS APR_EXAMPLES)
    add_executable(${name} ${name}.cpp)
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${APR_RESOLVED_INCLUDES}
    )
    target_compile_options(${name} PRIVATE ${APR_CFLAGS_OTHER} ${APRUTIL_CFLAGS_OTHER})
    target_link_directories(${name} PRIVATE ${APR_LIBRARY_DIRS} ${APRUTIL_LIBRARY_DIRS})
    target_link_libraries(${name} PRIVATE ${APRUTIL_LIBRARIES})
    target_compile_features(${name} PRIVATE cxx_std_20)
endforeach()
