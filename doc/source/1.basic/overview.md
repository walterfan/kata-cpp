# C++ 语言概述

## C++ 的独特之处

C++ 是一门多范式编程语言，相比其他语言有以下独特之处：

### 1. 零成本抽象 (Zero-Cost Abstraction)

C++ 的核心设计哲学是"你不用的东西，不需要付出代价"。

```cpp
// 使用 std::vector 与手动管理数组的性能几乎相同
std::vector<int> vec = {1, 2, 3, 4, 5};

// 编译器会优化掉虚函数调用（当类型在编译期已知时）
template<typename T>
void process(T& obj) {
    obj.do_something();  // 静态多态，无虚函数开销
}
```

### 2. 值语义 vs 引用语义

C++ 默认使用值语义，这与 Java/Python 等语言截然不同：

```cpp
struct Point { int x, y; };

Point p1 = {1, 2};
Point p2 = p1;      // 深拷贝！p2 是 p1 的副本
p2.x = 10;          // 不影响 p1

// 对比 Python/Java
// p2 = p1 会创建引用，修改 p2 会影响 p1
```

### 3. 确定性资源管理 (RAII)

C++ 通过 RAII 实现确定性资源管理，这是 GC 语言做不到的：

```cpp
void process_file() {
    std::ifstream file("data.txt");  // 构造时打开文件
    // ... 处理文件 ...
}   // 析构时自动关闭文件，即使发生异常

// 对比 Java/Python 需要 try-finally 或 with 语句
```

### 4. 编译期计算

现代 C++ 可以在编译期进行大量计算：

```cpp
// C++11: constexpr 函数
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// C++17: if constexpr 编译期分支
template<typename T>
auto process(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;
    } else {
        return value;
    }
}

// C++20: consteval 强制编译期求值
consteval int compile_time_only(int n) {
    return n * n;
}
```

## C++ 标准演进

| 标准 | 年份 | 主要特性 |
|------|------|----------|
| C++11 | 2011 | auto, lambda, 智能指针, 移动语义, 右值引用, 范围 for |
| C++14 | 2014 | 泛型 lambda, 返回类型推导, 变量模板 |
| C++17 | 2017 | 结构化绑定, if/switch 初始化, std::optional/variant |
| C++20 | 2020 | 概念(Concepts), 协程, Ranges, Modules, 三路比较 |
| C++23 | 2023 | std::expected, std::print, 更多 constexpr |

## 编译器支持

检查编译器对 C++ 标准的支持：

```cpp
#if __cplusplus >= 202302L
    // C++23
#elif __cplusplus >= 202002L
    // C++20
#elif __cplusplus >= 201703L
    // C++17
#elif __cplusplus >= 201402L
    // C++14
#elif __cplusplus >= 201103L
    // C++11
#endif

// 或使用特性检测宏 (C++20)
#ifdef __cpp_concepts
    // 支持概念
#endif
```

## 与其他语言的对比

### 内存管理

| 语言 | 内存管理方式 | 特点 |
|------|-------------|------|
| C++ | 手动 / RAII / 智能指针 | 精确控制，零开销 |
| Java/C# | 垃圾回收 (GC) | 自动管理，有 GC 停顿 |
| Rust | 所有权系统 | 编译期检查，零 GC |
| Python | 引用计数 + GC | 简单但有循环引用问题 |

### 并发模型

| 语言 | 主要并发模型 |
|------|-------------|
| C++ | std::thread, std::async, 协程 |
| Go | Goroutine + Channel |
| Java | Thread, CompletableFuture |
| Rust | async/await, 线程安全保证 |

## 学习建议

1. **先学好 C++11**：这是现代 C++ 的基础
2. **理解移动语义**：这是 C++ 独有的特性
3. **掌握 RAII**：资源管理的核心思想
4. **多看编译器警告**：开启 `-Wall -Wextra`
5. **使用静态分析工具**：clang-tidy, cppcheck

```{tip}
推荐编译选项：
`g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror`
```
