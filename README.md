# Beautiful C++ - Kata and Examples

Pattern and Kata are the best methods for computer language learning.

## Overview

This repository contains C++ learning materials, examples, and a comprehensive documentation guide for modern C++ development.

## Documentation

The documentation is built with Sphinx and available at: [Beautiful C++ Doc](https://walterfan.github.io/beautiful_cpp/)

### 现代 C++ 实战开发指南

Located in `doc/source/`, organized by topic:

#### 1. 基础知识与陷阱 (Basic)
- [overview.md](./doc/source/1.basic/overview.md) - C++ 语言特性概述
- [modern_features.md](./doc/source/1.basic/modern_features.md) - 现代 C++ 特性 (C++11/14/17/20/23)
- [trap.md](./doc/source/1.basic/trap.md) - 常见陷阱与易错点
- [best_practice.md](./doc/source/1.basic/best_practice.md) - 最佳实践
- [migration.md](./doc/source/1.basic/migration.md) - 从其他语言迁移指南

#### 2. 并发编程 (Concurrency)
- [thread_basics.md](./doc/source/2.concurrency/thread_basics.md) - 线程基础
- [mutex_lock.md](./doc/source/2.concurrency/mutex_lock.md) - 互斥锁与锁管理
- [atomic.md](./doc/source/2.concurrency/atomic.md) - 原子操作
- [condition_variable.md](./doc/source/2.concurrency/condition_variable.md) - 条件变量
- [async_future.md](./doc/source/2.concurrency/async_future.md) - 异步与 Future
- [coroutine.md](./doc/source/2.concurrency/coroutine.md) - 协程 (C++20)
- [patterns.md](./doc/source/2.concurrency/patterns.md) - 并发模式
- [pitfalls.md](./doc/source/2.concurrency/pitfalls.md) - 并发陷阱

#### 3. 性能调优 (Performance)
- [profiling.md](./doc/source/3.performance/profiling.md) - 性能分析工具
- [optimization.md](./doc/source/3.performance/optimization.md) - 优化技巧
- [benchmark.md](./doc/source/3.performance/benchmark.md) - 基准测试
- [cache.md](./doc/source/3.performance/cache.md) - 缓存优化
- [compiler.md](./doc/source/3.performance/compiler.md) - 编译器优化

#### 4. 内存管理 (Memory)
- [smart_pointer.md](./doc/source/4.memory/smart_pointer.md) - 智能指针
- [raii.md](./doc/source/4.memory/raii.md) - RAII 原则
- [allocation.md](./doc/source/4.memory/allocation.md) - 内存分配
- [layout.md](./doc/source/4.memory/layout.md) - 内存布局
- [leak_detection.md](./doc/source/4.memory/leak_detection.md) - 内存泄漏检测

#### 5. 网络编程 (Network)
- [socket_basics.md](./doc/source/5.network/socket_basics.md) - Socket 基础
- [tcp_udp.md](./doc/source/5.network/tcp_udp.md) - TCP/UDP 编程
- [http_client.md](./doc/source/5.network/http_client.md) - HTTP 客户端
- [async_io.md](./doc/source/5.network/async_io.md) - 异步 I/O

#### 6. 常用库 (Libraries)
- [googletest.md](./doc/source/6.library/googletest.md) - GoogleTest 测试框架
- [jsoncpp.md](./doc/source/6.library/jsoncpp.md) - JsonCpp JSON 解析
- [libcurl.md](./doc/source/6.library/libcurl.md) - libcurl HTTP 库
- [boost.md](./doc/source/6.library/boost.md) - Boost 库概览
- [boost_asio.md](./doc/source/6.library/boost_asio.md) - Boost.Asio 异步 I/O
- [boost_beast.md](./doc/source/6.library/boost_beast.md) - Boost.Beast HTTP/WebSocket
- [openssl.md](./doc/source/6.library/openssl.md) - OpenSSL 加密库

#### 7. 速查表 (Cheatsheet)
- [syntax.md](./doc/source/7.cheatsheet/syntax.md) - 语法速查
- [stl.md](./doc/source/7.cheatsheet/stl.md) - STL 速查
- [commands.md](./doc/source/7.cheatsheet/commands.md) - 命令速查

### Build Documentation

```bash
cd doc
pip install -r requirements.txt
make html
```

## Kata Examples

- [example](./kata/example) - Basic examples
- [chatgpt](./kata/chatgpt) - ChatGPT related examples
- [algorithm](./kata/algorithm) - Algorithm implementations
- [pattern](./kata/pattern) - Design patterns

## Build

```bash
mkdir bld
cd bld
cmake ..
make
```

## Learning Notes

- [Beautiful C++ Wiki](https://github.com/walterfan/beautiful_cpp/wiki)
- [Modern C++ Notes](./note/modern_cpp.md)
- [Word Bank Example](./note/word_bank_note.md)

## Reference

- [C++ 11 Standard](./refer/cpp-11.pdf)
- [C++ 14 Standard](./refer/cpp-14.pdf)
- [C++ 17 Standard](./refer/cpp-17.pdf)
- [C++ 20 Standard](./refer/cpp-20.pdf)

## License

CC BY-NC-ND 4.0
