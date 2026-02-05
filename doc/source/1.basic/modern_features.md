# 现代 C++ 特性速览

本节快速介绍 C++11 到 C++23 的重要新特性。

## C++11 核心特性

### auto 类型推导

```cpp
// 基础用法
auto x = 42;            // int
auto y = 3.14;          // double
auto s = "hello"s;      // std::string (需要 using namespace std::literals)

// 迭代器简化
std::map<std::string, std::vector<int>> data;
auto it = data.begin();  // 代替冗长的类型声明

// 注意：auto 会去掉引用和 const
const int& ref = x;
auto a = ref;           // int，不是 const int&
auto& b = ref;          // const int&
decltype(auto) c = ref; // const int& (C++14)
```

### Lambda 表达式

```cpp
// 基础语法
auto add = [](int a, int b) { return a + b; };

// 捕获列表
int factor = 2;
auto multiply = [factor](int x) { return x * factor; };   // 值捕获
auto modify = [&factor](int x) { factor = x; };           // 引用捕获
auto all_by_value = [=](int x) { return x * factor; };    // 全部值捕获
auto all_by_ref = [&](int x) { factor = x; };             // 全部引用捕获

// 陷阱：引用捕获悬空
std::function<int()> create_counter() {
    int count = 0;
    return [&count]() { return ++count; };  // 危险！count 已销毁
}
```

### 移动语义与右值引用

```cpp
class Buffer {
    int* data;
    size_t size;
public:
    // 移动构造函数
    Buffer(Buffer&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // 移动赋值运算符
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
};

// 使用 std::move
Buffer b1(1024);
Buffer b2 = std::move(b1);  // b1 被"掏空"
```

### 智能指针

```cpp
#include <memory>

// unique_ptr: 独占所有权
auto up = std::make_unique<int>(42);  // C++14
// auto up2 = up;  // 编译错误！
auto up2 = std::move(up);  // OK

// shared_ptr: 共享所有权
auto sp = std::make_shared<int>(42);
auto sp2 = sp;  // OK，引用计数 +1
std::cout << sp.use_count();  // 2

// weak_ptr: 弱引用，不增加引用计数
std::weak_ptr<int> wp = sp;
if (auto locked = wp.lock()) {  // 尝试获取 shared_ptr
    std::cout << *locked;
}
```

## C++14 新特性

### 泛型 Lambda

```cpp
// 参数使用 auto
auto generic_add = [](auto a, auto b) { return a + b; };
generic_add(1, 2);      // int
generic_add(1.0, 2.0);  // double
generic_add("a"s, "b"s);// string
```

### 返回类型推导

```cpp
auto factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);  // 编译器推导返回类型
}
```

### 变量模板

```cpp
template<typename T>
constexpr T pi = T(3.14159265358979323846);

double area = pi<double> * r * r;
float areaf = pi<float> * r * r;
```

## C++17 新特性

### 结构化绑定

```cpp
// 数组
int arr[] = {1, 2, 3};
auto [a, b, c] = arr;

// pair/tuple
auto [key, value] = std::make_pair(1, "hello");

// 结构体
struct Point { int x, y; };
Point p{10, 20};
auto [x, y] = p;

// map 遍历
std::map<std::string, int> scores;
for (const auto& [name, score] : scores) {
    std::cout << name << ": " << score << "\n";
}
```

### if/switch 初始化

```cpp
// if with initializer
if (auto it = map.find(key); it != map.end()) {
    // 使用 it
}  // it 超出作用域

// switch with initializer
switch (auto val = get_value(); val) {
    case 1: /* ... */ break;
    case 2: /* ... */ break;
}
```

### std::optional

```cpp
#include <optional>

std::optional<int> find_value(int key) {
    if (/* found */) return value;
    return std::nullopt;  // 或 {}
}

if (auto result = find_value(42)) {
    std::cout << *result;
}

// 带默认值
int val = find_value(42).value_or(-1);
```

### std::variant

```cpp
#include <variant>

std::variant<int, double, std::string> v;
v = 42;
v = 3.14;
v = "hello";

// 访问
std::cout << std::get<std::string>(v);

// 安全访问
if (auto* p = std::get_if<int>(&v)) {
    std::cout << *p;
}

// 访问者模式
std::visit([](auto&& arg) {
    std::cout << arg;
}, v);
```

## C++20 新特性

### 概念 (Concepts)

```cpp
#include <concepts>

// 定义概念
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};

// 使用概念
template<Addable T>
T add(T a, T b) { return a + b; }

// 或使用 requires 子句
template<typename T>
    requires std::integral<T>
T square(T x) { return x * x; }

// 简写语法
void print(std::integral auto value) {
    std::cout << value;
}
```

### Ranges

```cpp
#include <ranges>
#include <vector>

std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 链式操作
auto result = vec 
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; })
    | std::views::take(3);

for (int n : result) {
    std::cout << n << " ";  // 4 16 36
}
```

### 三路比较运算符 (Spaceship Operator)

```cpp
#include <compare>

struct Point {
    int x, y;
    
    // 自动生成所有比较运算符
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2}, p2{1, 3};
if (p1 < p2) { /* ... */ }   // 自动可用
if (p1 == p2) { /* ... */ }  // 自动可用
```

### 协程

```cpp
#include <coroutine>

// 简单的生成器
generator<int> iota(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

for (int n : iota(1, 10)) {
    std::cout << n << " ";
}
```

## C++23 新特性预览

### std::expected

```cpp
#include <expected>

std::expected<int, std::string> parse(std::string_view s) {
    if (/* valid */) return value;
    return std::unexpected("parse error");
}

auto result = parse("42");
if (result) {
    std::cout << *result;
} else {
    std::cerr << result.error();
}
```

### std::print

```cpp
#include <print>

// 类似 Python 的 print
std::print("Hello, {}!\n", "World");
std::println("Value: {}", 42);  // 自动换行
```

### Deducing this

```cpp
struct Widget {
    // 显式 this 参数
    template<typename Self>
    auto& get_value(this Self&& self) {
        return std::forward<Self>(self).value;
    }
};
```

```{note}
建议逐步采用新特性，确保团队和编译器都支持。
使用特性检测宏 `__cpp_xxx` 进行条件编译。
```
