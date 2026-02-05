# C++ 语法速查

## 基本类型

```cpp
// 整数类型
int8_t, int16_t, int32_t, int64_t      // 有符号
uint8_t, uint16_t, uint32_t, uint64_t  // 无符号
size_t                                  // 无符号，用于大小
ptrdiff_t                              // 有符号，用于指针差

// 浮点类型
float       // 32位
double      // 64位
long double // 80位或更大

// 布尔和字符
bool        // true/false
char        // 8位字符
wchar_t     // 宽字符
char8_t     // UTF-8 (C++20)
char16_t    // UTF-16
char32_t    // UTF-32
```

## auto 与类型推导

```cpp
auto x = 42;              // int
auto y = 3.14;            // double
auto z = "hello";         // const char*
auto s = "hello"s;        // std::string (需要 using namespace std::literals)

const auto& r = x;        // const int&
auto* p = &x;             // int*

decltype(x) a;            // int
decltype(auto) b = x;     // int
decltype((x)) c = x;      // int& (加括号变引用)
```

## 初始化

```cpp
// 直接初始化
int a(42);
std::string s("hello");

// 拷贝初始化
int b = 42;
std::string s2 = "hello";

// 列表初始化 (C++11)
int c{42};
std::string s3{"hello"};
std::vector<int> v{1, 2, 3};

// 值初始化
int d{};  // 0
int* p{};  // nullptr
```

## 指针与引用

```cpp
int x = 42;

// 指针
int* p = &x;      // 取地址
*p = 100;         // 解引用

// 引用
int& r = x;       // 左值引用
r = 100;

// 右值引用 (C++11)
int&& rr = 42;

// 指针常量
const int* p1 = &x;      // 指向常量的指针
int* const p2 = &x;      // 常量指针
const int* const p3 = &x; // 两者都是

// 空指针
int* null = nullptr;  // C++11
```

## 范围 for 循环

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// 只读
for (const auto& item : vec) { }

// 可修改
for (auto& item : vec) { }

// 拷贝
for (auto item : vec) { }

// C++20: 带初始化
for (auto v = get_vec(); auto& item : v) { }
```

## Lambda 表达式

```cpp
// 基本语法
auto f = [](int x) { return x * 2; };

// 带捕获
int factor = 2;
auto multiply = [factor](int x) { return x * factor; };

// 引用捕获
auto modify = [&factor](int x) { factor = x; };

// 全部捕获
auto all_val = [=]() { return factor; };   // 值
auto all_ref = [&]() { factor++; };        // 引用

// 泛型 Lambda (C++14)
auto generic = [](auto x) { return x * 2; };

// 带返回类型
auto divide = [](int a, int b) -> double { return (double)a / b; };

// mutable Lambda
auto counter = [count = 0]() mutable { return ++count; };
```

## 智能指针

```cpp
#include <memory>

// unique_ptr
auto up = std::make_unique<int>(42);
// up = up2;  // 错误！
auto up2 = std::move(up);

// shared_ptr
auto sp = std::make_shared<int>(42);
auto sp2 = sp;  // OK

// weak_ptr
std::weak_ptr<int> wp = sp;
if (auto locked = wp.lock()) { }
```

## 可变参数模板

```cpp
// 折叠表达式 (C++17)
template<typename... Args>
auto sum(Args... args) {
    return (args + ...);  // 右折叠
}

// 递归展开
template<typename T>
void print(T t) { std::cout << t; }

template<typename T, typename... Args>
void print(T t, Args... args) {
    std::cout << t << " ";
    print(args...);
}
```

## 结构化绑定 (C++17)

```cpp
// 数组
int arr[] = {1, 2, 3};
auto [a, b, c] = arr;

// pair/tuple
auto [x, y] = std::make_pair(1, 2);

// 结构体
struct Point { int x, y; };
Point p{10, 20};
auto [px, py] = p;

// map 遍历
for (const auto& [key, value] : map) { }
```

## 属性

```cpp
[[nodiscard]] int compute();   // 不应忽略返回值
[[deprecated]] void old_func(); // 已废弃
[[maybe_unused]] int unused;   // 可能未使用
[[fallthrough]];               // switch 贯穿
[[likely]] / [[unlikely]]      // 分支预测提示 (C++20)
[[no_unique_address]]          // 空类优化 (C++20)
```

## if constexpr (C++17)

```cpp
template<typename T>
auto process(T value) {
    if constexpr (std::is_integral_v<T>) {
        return value * 2;
    } else {
        return value;
    }
}
```

## 概念 (C++20)

```cpp
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

template<Addable T>
T add(T a, T b) { return a + b; }

// 或简写
void print(std::integral auto value) { }
```

## 三路比较 (C++20)

```cpp
#include <compare>

struct Point {
    int x, y;
    auto operator<=>(const Point&) const = default;
};

// 自动生成 ==, !=, <, <=, >, >=
```
