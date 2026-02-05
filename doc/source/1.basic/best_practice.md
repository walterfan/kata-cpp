# C++ 最佳实践

本节总结现代 C++ 开发的最佳实践。

## 命名与代码风格

### 命名约定

```cpp
// 类名：PascalCase
class MyClass {};

// 函数名：snake_case 或 camelCase（保持一致）
void do_something();
void doSomething();

// 变量名：snake_case
int counter_value;

// 常量：k 前缀或全大写
constexpr int kMaxSize = 100;
constexpr int MAX_SIZE = 100;

// 成员变量：后缀下划线
class Widget {
    int value_;
};
```

### 头文件保护

```cpp
// 传统方式
#ifndef MY_HEADER_H
#define MY_HEADER_H
// ...
#endif

// 现代方式（非标准但广泛支持）
#pragma once
```

## RAII 原则

### 资源管理

```cpp
// 差：手动管理资源
void bad_example() {
    int* arr = new int[100];
    if (error) return;  // 内存泄漏！
    delete[] arr;
}

// 好：使用 RAII
void good_example() {
    auto arr = std::make_unique<int[]>(100);
    if (error) return;  // 自动释放
}

// 文件操作
void process_file(const std::string& path) {
    std::ifstream file(path);  // RAII 管理文件
    // 即使抛异常，文件也会正确关闭
}
```

### 自定义 RAII 类

```cpp
// 使用 scope_guard 模式
template<typename F>
class scope_guard {
    F func_;
    bool active_;
public:
    explicit scope_guard(F f) : func_(std::move(f)), active_(true) {}
    ~scope_guard() { if (active_) func_(); }
    void dismiss() { active_ = false; }
};

// 使用
void example() {
    auto* resource = acquire_resource();
    auto guard = scope_guard([&]{ release_resource(resource); });
    
    // 如果成功，取消清理
    if (success) {
        guard.dismiss();
    }
}
```

## 类设计原则

### Rule of Zero/Five

```cpp
// Rule of Zero：优先使用标准库类型，不写特殊成员函数
class Widget {
    std::string name_;
    std::vector<int> data_;
    // 编译器生成的特殊成员函数就够用了
};

// Rule of Five：如果需要自定义一个，就要考虑所有五个
class Resource {
public:
    Resource();
    ~Resource();
    Resource(const Resource&);
    Resource& operator=(const Resource&);
    Resource(Resource&&) noexcept;
    Resource& operator=(Resource&&) noexcept;
};

// 或明确删除
class NonCopyable {
public:
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
```

### 接口设计

```cpp
// 使用 const 正确性
class Container {
public:
    // const 成员函数
    size_t size() const { return size_; }
    
    // 重载提供 const 和非 const 版本
    int& operator[](size_t i) { return data_[i]; }
    const int& operator[](size_t i) const { return data_[i]; }
};

// 参数传递指南
void process(const std::string& s);   // 只读大对象
void process(std::string_view s);     // C++17，更高效
void process(int x);                   // 小对象直接传值
void process(std::unique_ptr<T> p);   // 转移所有权
void process(T* p);                    // 可选参数，可为 null
void process(T& ref);                  // 必须有效，会被修改
```

## 错误处理

### 异常 vs 返回值

```cpp
// 使用异常：构造函数、运算符、程序无法继续的情况
class File {
public:
    File(const std::string& path) {
        if (!open(path)) {
            throw std::runtime_error("Cannot open file");
        }
    }
};

// 使用返回值：可预期的失败
std::optional<int> find_value(int key) {
    if (/* found */) return value;
    return std::nullopt;
}

// C++23: std::expected
std::expected<int, Error> parse(std::string_view s) {
    if (/* error */) return std::unexpected(Error::PARSE_ERROR);
    return value;
}
```

### 错误处理策略

```cpp
// 使用 nodiscard 防止忽略返回值
[[nodiscard]] int calculate();  // 忽略返回值会警告

// 使用 assert 检查程序逻辑
void process(int* ptr) {
    assert(ptr != nullptr);  // 仅在 Debug 模式生效
    // ...
}

// 使用 static_assert 编译期检查
template<typename T>
class Container {
    static_assert(std::is_nothrow_move_constructible_v<T>,
                  "T must be nothrow move constructible");
};
```

## 性能最佳实践

### 避免不必要的拷贝

```cpp
// 使用移动语义
std::vector<std::string> get_names() {
    std::vector<std::string> names;
    // ...
    return names;  // RVO 或移动，不会拷贝
}

// 使用 emplace 代替 push
std::vector<std::pair<int, std::string>> vec;
vec.push_back(std::make_pair(1, "hello"));  // 可能拷贝
vec.emplace_back(1, "hello");  // 原地构造

// 预分配容器空间
std::vector<int> vec;
vec.reserve(1000);  // 避免多次重新分配
```

### 使用 string_view

```cpp
// 差：不必要的字符串拷贝
void process(const std::string& s);
process("literal");  // 创建临时 string

// 好：使用 string_view
void process(std::string_view s);
process("literal");  // 无拷贝

// 注意生命周期！
std::string_view dangerous(std::string&& s) {
    return s;  // 危险！s 销毁后 view 悬空
}
```

## 现代 C++ 习惯用法

### 使用 auto

```cpp
// 好的使用场景
auto it = container.begin();  // 迭代器类型太长
auto ptr = std::make_unique<Widget>();  // 避免重复类型
auto result = complex_calculation();  // 复杂表达式

// 避免的场景
auto x = 1;  // 不清楚类型
auto y = func();  // 如果返回类型不明显
```

### 使用范围 for

```cpp
// 优先使用范围 for
for (const auto& item : container) {
    // 只读访问
}

for (auto& item : container) {
    // 需要修改
}

for (auto item : container) {
    // 拷贝每个元素（小对象时可以）
}

// 需要索引时
for (size_t i = 0; i < vec.size(); ++i) {
    // 使用 i 和 vec[i]
}
```

### 使用 constexpr

```cpp
// 编译期常量
constexpr int MAX_SIZE = 100;

// 编译期函数
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 编译期类
class Point {
public:
    constexpr Point(int x, int y) : x_(x), y_(y) {}
    constexpr int x() const { return x_; }
    constexpr int y() const { return y_; }
private:
    int x_, y_;
};

constexpr Point p(1, 2);
static_assert(p.x() == 1);
```

## 代码组织

### 头文件 vs 实现文件

```cpp
// widget.h
#pragma once
#include <string>

class Widget {
public:
    Widget(std::string name);
    void process();
private:
    std::string name_;
};

// widget.cpp
#include "widget.h"

Widget::Widget(std::string name) : name_(std::move(name)) {}

void Widget::process() {
    // 实现
}
```

### 使用 PIMPL 减少编译依赖

```cpp
// widget.h
#pragma once
#include <memory>

class Widget {
public:
    Widget();
    ~Widget();
    void process();
private:
    struct Impl;
    std::unique_ptr<Impl> pimpl_;
};

// widget.cpp
#include "widget.h"
#include <complex_dependency.h>

struct Widget::Impl {
    ComplexType data;
    // 实现细节
};

Widget::Widget() : pimpl_(std::make_unique<Impl>()) {}
Widget::~Widget() = default;  // 需要在 cpp 中定义
```

```{tip}
遵循 C++ Core Guidelines：
https://isocpp.github.io/CppCoreGuidelines/
```
