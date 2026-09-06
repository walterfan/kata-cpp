# Kata for APR

Apache Portable Runtime (APR) is a C library that gives the same system APIs
on Unix, macOS, and Windows. These examples wrap the C API in small C++
programs.

- Cross-platform abstraction for system-level APIs (`01_platform`)
- Memory allocation and management utilities (`02_memory`)
- File and directory manipulation functions (`03_file`)
- Threading and synchronization primitives (`04_thread`)
- Network socket and protocol support (`05_socket`)
- Time and date handling (`06_time`)
- Atomic operations and shared memory support (`07_atomic_shm`)
- Unicode and character encoding conversion via APR-iconv / apr-util xlate (`08_iconv`)
- Utility functions for data structures like tables, arrays, and hash maps (`09_tables`)

## Prerequisites

APR 1.x and APR-util, discoverable by `pkg-config`:

```bash
pkg-config --modversion apr-1
pkg-config --modversion apr-util-1
```

On macOS these ship with the Command Line Tools SDK. On Linux:

```bash
# Debian/Ubuntu
sudo apt install libapr1-dev libaprutil1-dev pkg-config
```

## Build (CMake)

```bash
cd kata/apr
make          # cmake -S . -B bld && cmake --build bld
```

Binaries land in `kata/apr/bld/bin/`.

From the repo root:

```bash
make apr
```

## Run one by one (Makefile)

```bash
cd kata/apr
make run-01    # platform / initialize
make run-02    # pools and palloc
make run-03    # files and directories
make run-04    # threads and mutex
make run-05    # loopback TCP echo
make run-06    # time and date
make run-07    # atomics and shm
make run-08    # charset conversion
make run-09    # array, table, hash
make run       # all of the above, in order
```

`apr_demo.h` holds a tiny RAII wrapper for `apr_initialize` / `apr_terminate`
and `apr_pool_t`.
