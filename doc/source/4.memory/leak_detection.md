# 内存泄漏检测

## Valgrind

### 基本使用

```bash
# 编译时加入调试信息
g++ -g program.cpp -o program

# 运行 Valgrind memcheck
valgrind --leak-check=full ./program
```

### 输出解读

```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 72 bytes in 3 blocks
==12345==   total heap usage: 10 allocs, 7 frees, 73,728 bytes allocated
==12345== 
==12345== 72 (24 direct, 48 indirect) bytes in 1 blocks are definitely lost
==12345==    at 0x4C2E0EF: operator new(unsigned long)
==12345==    by 0x4011B5: create_leak() (program.cpp:10)
==12345==    by 0x40126C: main (program.cpp:25)
```

### 常用选项

```bash
# 完整泄漏检查
valgrind --leak-check=full --show-leak-kinds=all ./program

# 检查未初始化内存
valgrind --track-origins=yes ./program

# 输出到文件
valgrind --log-file=valgrind.log ./program

# 生成可视化数据
valgrind --tool=massif ./program
ms_print massif.out.<pid>
```

## AddressSanitizer (ASan)

### 编译

```bash
# GCC
g++ -fsanitize=address -g program.cpp

# Clang
clang++ -fsanitize=address -g program.cpp
```

### 检测能力

- 堆缓冲区溢出
- 栈缓冲区溢出
- 全局缓冲区溢出
- 释放后使用 (use-after-free)
- 返回后使用 (use-after-return)
- 作用域后使用 (use-after-scope)
- 初始化顺序问题
- 内存泄漏 (需要 LeakSanitizer)

### 示例输出

```
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x6020000001d4
READ of size 4 at 0x6020000001d4 thread T0
    #0 0x4011d8 in main /path/program.cpp:12
    #1 0x7f2b8f0eb0b2 in __libc_start_main

0x6020000001d4 is located 4 bytes inside of 8-byte region
freed by thread T0 here:
    #0 0x7f2b8f7c1537 in operator delete(void*)
    #1 0x4011a8 in main /path/program.cpp:10

previously allocated by thread T0 here:
    #0 0x7f2b8f7c0458 in operator new(unsigned long)
    #1 0x40117f in main /path/program.cpp:8
```

### 环境变量

```bash
# 检测泄漏
ASAN_OPTIONS=detect_leaks=1 ./program

# 详细输出
ASAN_OPTIONS=verbosity=1 ./program

# 遇到错误继续运行
ASAN_OPTIONS=halt_on_error=0 ./program
```

## LeakSanitizer (LSan)

```bash
# 通常与 ASan 一起使用
g++ -fsanitize=address -g program.cpp

# 或单独使用
g++ -fsanitize=leak -g program.cpp

# 设置选项
LSAN_OPTIONS=suppressions=suppr.txt ./program
```

## MemorySanitizer (MSan)

检测使用未初始化内存。

```bash
# 仅 Clang 支持
clang++ -fsanitize=memory -g program.cpp

# 输出
==12345==WARNING: MemorySanitizer: use-of-uninitialized-value
    #0 0x4011d8 in main /path/program.cpp:12
```

## ThreadSanitizer (TSan)

检测数据竞争。

```bash
g++ -fsanitize=thread -g program.cpp

# 输出
WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x7f8d... by thread T1:
    #0 increment() program.cpp:10

  Previous read of size 4 at 0x7f8d... by thread T2:
    #0 read() program.cpp:15
```

## 代码中的内存检查

### 重载 new/delete 跟踪

```cpp
#include <iostream>
#include <cstdlib>
#include <new>

// 跟踪分配
void* operator new(size_t size) {
    void* p = std::malloc(size);
    std::cerr << "new(" << size << ") = " << p << "\n";
    if (!p) throw std::bad_alloc();
    return p;
}

void operator delete(void* p) noexcept {
    std::cerr << "delete(" << p << ")\n";
    std::free(p);
}

void operator delete(void* p, size_t size) noexcept {
    std::cerr << "delete(" << p << ", " << size << ")\n";
    std::free(p);
}
```

### 简单泄漏检测器

```cpp
#include <unordered_map>
#include <mutex>

class LeakDetector {
    struct AllocationInfo {
        size_t size;
        const char* file;
        int line;
    };
    
    std::unordered_map<void*, AllocationInfo> allocations_;
    std::mutex mutex_;

public:
    void record_alloc(void* ptr, size_t size, 
                      const char* file, int line) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_[ptr] = {size, file, line};
    }
    
    void record_free(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocations_.erase(ptr);
    }
    
    void report_leaks() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [ptr, info] : allocations_) {
            std::cerr << "Leak: " << info.size << " bytes at "
                      << info.file << ":" << info.line << "\n";
        }
    }
    
    ~LeakDetector() {
        report_leaks();
    }
};

// 全局实例
LeakDetector g_leak_detector;

// 宏
#define DEBUG_NEW new(__FILE__, __LINE__)
```

## 智能指针调试

```cpp
// 检查 shared_ptr 引用计数
auto sp = std::make_shared<int>(42);
std::cout << "Use count: " << sp.use_count() << "\n";

// 检查 weak_ptr 是否过期
std::weak_ptr<int> wp = sp;
std::cout << "Expired: " << wp.expired() << "\n";

// 自定义删除器用于调试
auto debug_deleter = [](int* p) {
    std::cerr << "Deleting " << *p << " at " << p << "\n";
    delete p;
};
std::shared_ptr<int> sp2(new int(42), debug_deleter);
```

## 工具对比

| 工具 | 优点 | 缺点 |
|------|------|------|
| Valgrind | 功能全面，无需重新编译 | 性能开销大（10-50x） |
| ASan | 快速（2x），检测准确 | 需要重新编译，内存开销 |
| LSan | 专门检测泄漏 | 需要重新编译 |
| MSan | 检测未初始化内存 | 仅 Clang，需要重新编译 |
| TSan | 检测数据竞争 | 需要重新编译，内存开销 |

```{tip}
内存调试建议：
1. 开发时启用 ASan
2. CI 中运行 Valgrind
3. 使用智能指针减少泄漏
4. 代码审查关注资源管理
5. 定期进行内存分析
```
