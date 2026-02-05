# 内存布局

## 对象内存布局

### 基本类型

```cpp
// 查看大小和对齐
std::cout << sizeof(char) << ", " << alignof(char);    // 1, 1
std::cout << sizeof(short) << ", " << alignof(short);  // 2, 2
std::cout << sizeof(int) << ", " << alignof(int);      // 4, 4
std::cout << sizeof(long) << ", " << alignof(long);    // 8, 8 (64位)
std::cout << sizeof(double) << ", " << alignof(double);// 8, 8
std::cout << sizeof(void*) << ", " << alignof(void*);  // 8, 8 (64位)
```

### 结构体对齐与填充

```cpp
// 差：大量填充
struct Bad {
    char a;     // 1 byte
    // 7 bytes padding
    double b;   // 8 bytes
    char c;     // 1 byte
    // 3 bytes padding
    int d;      // 4 bytes
    char e;     // 1 byte
    // 7 bytes padding
};  // 总计 32 bytes

// 好：紧凑布局
struct Good {
    double b;   // 8 bytes
    int d;      // 4 bytes
    char a;     // 1 byte
    char c;     // 1 byte
    char e;     // 1 byte
    // 1 byte padding
};  // 总计 16 bytes

// 使用 offsetof 查看偏移
#include <cstddef>
std::cout << offsetof(Good, b);  // 0
std::cout << offsetof(Good, d);  // 8
std::cout << offsetof(Good, a);  // 12
```

### 控制对齐

```cpp
// 指定对齐要求
struct alignas(16) Aligned16 {
    int x, y, z, w;
};  // 16 字节对齐

// 打包（禁用填充，不推荐）
#pragma pack(push, 1)
struct Packed {
    char a;
    double b;
    char c;
};  // 10 bytes，无填充
#pragma pack(pop)

// C++11 alignas
struct alignas(64) CacheLineAligned {
    int data[16];
};
```

## 类的内存布局

### 无虚函数的类

```cpp
class Simple {
    int a;      // offset 0
    double b;   // offset 8 (对齐到 8)
    char c;     // offset 16
    // padding 7 bytes
};  // sizeof = 24

// 子类布局
class Derived : public Simple {
    int d;      // offset 24
    // padding 4 bytes
};  // sizeof = 32
```

### 带虚函数的类

```cpp
class Virtual {
    virtual void foo() {}
    int a;
};

// 布局：
// [vptr]   8 bytes  - 虚函数表指针
// [a]      4 bytes
// [pad]    4 bytes
// sizeof = 16

class Derived : public Virtual {
    int b;
    virtual void bar() {}  // 不增加 vptr
};

// 布局：
// [vptr]   8 bytes  - 继承的 vptr
// [a]      4 bytes
// [b]      4 bytes
// sizeof = 16
```

### 多重继承

```cpp
class A { int a; };
class B { int b; };
class C : public A, public B { int c; };

// 布局：
// [A::a]   4 bytes
// [pad]    4 bytes (假设 8 字节对齐)
// [B::b]   4 bytes
// [C::c]   4 bytes
```

### 虚继承

```cpp
class Base { int b; };
class D1 : virtual public Base { int d1; };
class D2 : virtual public Base { int d2; };
class Diamond : public D1, public D2 { int d; };

// Diamond 只有一份 Base 的成员
// 但有额外的虚基类指针
```

## 查看实际布局

### 编译器命令

```bash
# Clang
clang++ -Xclang -fdump-record-layouts program.cpp

# GCC
g++ -fdump-lang-class program.cpp

# MSVC
cl /d1reportSingleClassLayoutClassName program.cpp
```

### 运行时检查

```cpp
#include <iostream>
#include <cstddef>

class Example {
public:
    char a;
    int b;
    double c;
    
    static void print_layout() {
        std::cout << "sizeof: " << sizeof(Example) << "\n";
        std::cout << "offsetof(a): " << offsetof(Example, a) << "\n";
        std::cout << "offsetof(b): " << offsetof(Example, b) << "\n";
        std::cout << "offsetof(c): " << offsetof(Example, c) << "\n";
    }
};
```

## 空基类优化 (EBO)

```cpp
struct Empty {};  // sizeof = 1 (标准要求)

struct WithEmpty {
    Empty e;      // 1 byte
    int data;     // 4 bytes (对齐后)
};  // sizeof = 8

// 空基类优化
struct Optimized : Empty {
    int data;
};  // sizeof = 4 (Empty 不占空间)

// 利用 EBO 的模式
template<typename Allocator, typename T>
class Container : private Allocator {  // 空分配器不占空间
    T* data_;
    size_t size_;
};
```

## std::is_standard_layout

```cpp
#include <type_traits>

// 标准布局类型可以与 C 互操作
struct StandardLayout {
    int a;
    double b;
    char c;
};
static_assert(std::is_standard_layout_v<StandardLayout>);

// 非标准布局
struct NonStandard {
    virtual void foo() {}  // 有虚函数
    int a;
};
static_assert(!std::is_standard_layout_v<NonStandard>);
```

## POD (Plain Old Data)

```cpp
// C++20 使用 is_trivial 和 is_standard_layout
template<typename T>
constexpr bool is_pod_v = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

struct POD {
    int a;
    double b;
};
static_assert(is_pod_v<POD>);

// 非 POD
struct NonPOD {
    NonPOD() {}  // 用户定义的构造函数
    int a;
};
static_assert(!is_pod_v<NonPOD>);
```

## 位字段

```cpp
struct Flags {
    unsigned int a : 1;  // 1 bit
    unsigned int b : 3;  // 3 bits
    unsigned int c : 4;  // 4 bits
};  // 可能只占 1 byte

// 注意：位字段的布局是实现定义的
// 不同编译器可能有不同布局
```

## union

```cpp
union Variant {
    int i;
    float f;
    char c[4];
};  // sizeof = 4 (最大成员的大小)

// 类型双关（type punning）- 严格来说是 UB
Variant v;
v.f = 3.14f;
// 读取 v.i 是 UB（除了 char[]）

// C++20 推荐使用 std::bit_cast
float f = 3.14f;
auto i = std::bit_cast<int>(f);  // 安全的类型双关
```

```{tip}
内存布局优化建议：
1. 按成员大小降序排列以减少填充
2. 考虑缓存行对齐（64 字节）
3. 使用 `alignas` 明确指定对齐
4. 利用空基类优化
5. 使用编译器工具查看实际布局
```
