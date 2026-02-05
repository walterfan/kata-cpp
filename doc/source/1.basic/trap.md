# C++ 常见陷阱

本节总结 C++ 开发中最常见的陷阱，帮助你避免这些坑。

## 初始化陷阱

### Most Vexing Parse

```cpp
// 你以为在创建对象...
Widget w();  // 实际上是函数声明！

// 正确写法
Widget w1;           // 默认构造
Widget w2{};         // C++11 统一初始化（推荐）
Widget w3 = Widget();
auto w4 = Widget();
```

### 花括号初始化的窄化问题

```cpp
int x{3.14};           // 错误！花括号不允许窄化转换
int y = 3.14;          // OK，但会丢失精度
int z(3.14);           // OK，但会丢失精度

// 这是个特性，不是 bug！
std::vector<int> v1{10, 20};  // 包含 10 和 20
std::vector<int> v2(10, 20);  // 包含 10 个 20
```

### 静态初始化顺序问题

```cpp
// file1.cpp
int a = 10;

// file2.cpp
extern int a;
int b = a * 2;  // 危险！a 可能还未初始化

// 解决方案：使用函数返回静态局部变量
int& get_a() {
    static int a = 10;
    return a;
}
```

## 生命周期陷阱

### 悬空引用

```cpp
// 返回局部变量引用
int& dangerous() {
    int x = 42;
    return x;  // 警告：返回局部变量引用
}

// Lambda 捕获悬空
auto create_lambda() {
    int value = 42;
    return [&value]() { return value; };  // 危险！
}

// 迭代器失效
std::vector<int> vec = {1, 2, 3};
auto it = vec.begin();
vec.push_back(4);  // 可能导致重新分配
*it = 10;          // 未定义行为！
```

### 临时对象生命周期

```cpp
// 危险：string_view 引用临时对象
std::string_view get_view() {
    return std::string("hello");  // 临时对象立即销毁！
}

// 危险：范围 for 循环中的临时对象
for (auto x : get_vector().get_data()) {  // 危险！
    // get_vector() 返回的临时对象已销毁
}

// 安全写法
auto vec = get_vector();
for (auto x : vec.get_data()) { /* OK */ }
```

## 移动语义陷阱

### 移动后使用

```cpp
std::vector<int> v1 = {1, 2, 3};
std::vector<int> v2 = std::move(v1);
v1.push_back(4);  // 未定义行为？不是！
// v1 处于有效但未指定状态，可以安全操作
// 但不能假设其内容

// 最佳实践：移动后立即赋值或不再使用
v1 = {5, 6, 7};  // OK
```

### 完美转发陷阱

```cpp
template<typename T>
void wrapper(T&& arg) {
    func(arg);  // 错误！arg 是左值
    func(std::forward<T>(arg));  // 正确
}

// 多次转发
template<typename T>
void multi_use(T&& arg) {
    func1(std::forward<T>(arg));  // 第一次可能移动
    func2(std::forward<T>(arg));  // 危险！可能已被移动
}
```

## 智能指针陷阱

### 循环引用

```cpp
struct Node {
    std::shared_ptr<Node> next;  // 可能导致循环引用
};

// 解决方案：使用 weak_ptr 打破循环
struct SafeNode {
    std::shared_ptr<SafeNode> next;
    std::weak_ptr<SafeNode> prev;  // 弱引用不增加引用计数
};
```

### shared_ptr 的陷阱

```cpp
// 陷阱1：多个 shared_ptr 管理同一原始指针
int* raw = new int(42);
std::shared_ptr<int> sp1(raw);
std::shared_ptr<int> sp2(raw);  // 灾难！双重释放

// 陷阱2：this 的 shared_ptr
class Widget {
    void process() {
        // 危险！不要这样做
        auto sp = std::shared_ptr<Widget>(this);
    }
};

// 正确做法：继承 enable_shared_from_this
class SafeWidget : public std::enable_shared_from_this<SafeWidget> {
    void process() {
        auto sp = shared_from_this();  // 安全
    }
};
```

## 多线程陷阱

### 数据竞争

```cpp
int counter = 0;

void increment() {
    for (int i = 0; i < 1000000; ++i) {
        ++counter;  // 数据竞争！
    }
}

// 解决方案1：互斥锁
std::mutex mtx;
void safe_increment1() {
    std::lock_guard<std::mutex> lock(mtx);
    ++counter;
}

// 解决方案2：原子操作
std::atomic<int> atomic_counter{0};
void safe_increment2() {
    ++atomic_counter;  // 原子操作，线程安全
}
```

### 死锁

```cpp
std::mutex m1, m2;

void thread1() {
    std::lock_guard<std::mutex> lock1(m1);
    std::lock_guard<std::mutex> lock2(m2);  // 可能死锁
}

void thread2() {
    std::lock_guard<std::mutex> lock1(m2);  // 顺序相反
    std::lock_guard<std::mutex> lock2(m1);  // 死锁！
}

// 解决方案：使用 std::scoped_lock (C++17)
void safe_thread() {
    std::scoped_lock lock(m1, m2);  // 自动避免死锁
}
```

## 类型转换陷阱

### 隐式转换

```cpp
// 意外的隐式转换
class String {
public:
    String(const char* s);  // 可能导致意外转换
    String(int size);       // 更危险！
};

String s = 10;  // 创建长度为 10 的字符串？

// 解决方案：使用 explicit
class SafeString {
public:
    explicit SafeString(int size);
};
// SafeString s = 10;  // 编译错误
SafeString s(10);      // OK
```

### 切片问题

```cpp
class Base {
public:
    virtual void print() { std::cout << "Base"; }
};

class Derived : public Base {
    int data;
public:
    void print() override { std::cout << "Derived"; }
};

void process(Base b) {  // 值传递导致切片
    b.print();  // 输出 "Base"！
}

Derived d;
process(d);  // 切片！d 被"切"成 Base

// 解决方案：使用引用或指针
void safe_process(const Base& b) {
    b.print();  // 输出 "Derived"
}
```

## 异常陷阱

### 析构函数中的异常

```cpp
class Dangerous {
public:
    ~Dangerous() {
        throw std::runtime_error("error");  // 非常危险！
        // 如果在栈展开过程中调用，程序会终止
    }
};

// 正确做法：析构函数 noexcept
class Safe {
public:
    ~Safe() noexcept {
        try {
            cleanup();
        } catch (...) {
            // 记录日志，但不传播
        }
    }
};
```

### 异常安全

```cpp
class Container {
    int* data;
    size_t size;
public:
    Container& operator=(const Container& other) {
        delete[] data;  // 危险！如果 new 抛异常...
        data = new int[other.size];  // 抛异常后 data 是悬空指针
        // ...
    }
    
    // 异常安全版本（强保证）
    Container& operator=(const Container& other) {
        Container tmp(other);  // 可能抛异常，但状态未改变
        swap(*this, tmp);      // noexcept
        return *this;
    }
};
```

## 宏陷阱

### 宏的多次求值

```cpp
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 1, y = 2;
int z = MAX(x++, y++);  // x 或 y 可能被求值两次！

// 解决方案：使用内联函数或模板
template<typename T>
constexpr T max(T a, T b) { return a > b ? a : b; }
```

### 宏与作用域

```cpp
#define SWAP(a, b) { int tmp = a; a = b; b = tmp; }

if (condition)
    SWAP(x, y);  // 展开后的分号问题
else
    // ...

// 解决方案：do-while(0) 技巧
#define SAFE_SWAP(a, b) do { int tmp = a; a = b; b = tmp; } while(0)
```

```{warning}
这些陷阱在代码审查中经常出现。建议开启所有编译器警告，
并使用静态分析工具如 clang-tidy 来检测这些问题。
```
