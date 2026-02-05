# 从其他语言迁移到 C++

本节帮助有其他语言背景的开发者快速适应 C++。

## 从 Python 迁移

### 类型系统差异

```cpp
// Python: 动态类型
// x = 1
// x = "hello"  # OK

// C++: 静态类型
int x = 1;
// x = "hello";  // 编译错误！

// 使用 auto 可以简化类型声明
auto x = 1;       // int
auto y = "hello"; // const char*
auto z = std::string("hello");  // std::string
```

### 内存管理差异

```python
# Python: 引用计数 + GC
class Node:
    def __init__(self):
        self.next = None

a = Node()
b = Node()
a.next = b  # 引用计数管理
```

```cpp
// C++: 需要显式管理
// 方法1：智能指针（推荐）
struct Node {
    std::shared_ptr<Node> next;
};

auto a = std::make_shared<Node>();
auto b = std::make_shared<Node>();
a->next = b;

// 方法2：值语义
struct Node {
    std::unique_ptr<Node> next;
};
```

### 字符串处理

```python
# Python
s = "hello"
s += " world"
parts = s.split(" ")
```

```cpp
// C++
std::string s = "hello";
s += " world";

// 分割字符串需要手动实现或使用库
std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        result.push_back(token);
    }
    return result;
}

// C++20 ranges 版本
auto parts = s | std::views::split(' ');
```

### 容器操作

```python
# Python
numbers = [1, 2, 3, 4, 5]
doubled = [x * 2 for x in numbers]
filtered = [x for x in numbers if x > 2]
```

```cpp
// C++ 传统写法
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::vector<int> doubled;
std::transform(numbers.begin(), numbers.end(), 
               std::back_inserter(doubled),
               [](int x) { return x * 2; });

// C++20 Ranges
auto doubled = numbers | std::views::transform([](int x) { return x * 2; });
auto filtered = numbers | std::views::filter([](int x) { return x > 2; });
```

## 从 Java 迁移

### 对象创建和销毁

```java
// Java
Widget widget = new Widget();  // 总是在堆上
// 自动 GC

Widget w1 = new Widget();
Widget w2 = w1;  // w1 和 w2 指向同一对象
```

```cpp
// C++: 多种方式
Widget w1;  // 栈上创建，自动销毁（推荐）
auto w2 = std::make_unique<Widget>();  // 堆上，unique_ptr 管理
auto w3 = std::make_shared<Widget>();  // 堆上，shared_ptr 管理

// 值语义 vs 引用语义
Widget a;
Widget b = a;  // 拷贝！b 是 a 的副本
Widget& ref = a;  // ref 是 a 的引用
```

### 继承与多态

```java
// Java
class Base {
    void method() { }  // 默认可重写
}

class Derived extends Base {
    @Override
    void method() { }
}
```

```cpp
// C++
class Base {
public:
    virtual void method() { }  // 必须显式声明 virtual
    virtual ~Base() = default; // 虚析构函数很重要！
};

class Derived : public Base {
public:
    void method() override { }  // override 是可选的但推荐
};

// 使用多态
void process(Base& b) {  // 必须用引用或指针
    b.method();  // 动态绑定
}
```

### 接口与抽象类

```java
// Java
interface Drawable {
    void draw();
}

class Circle implements Drawable {
    public void draw() { }
}
```

```cpp
// C++: 使用纯虚函数
class Drawable {
public:
    virtual void draw() = 0;  // 纯虚函数
    virtual ~Drawable() = default;
};

class Circle : public Drawable {
public:
    void draw() override { }
};

// C++20: 使用 Concepts 作为"接口"
template<typename T>
concept Drawable = requires(T t) {
    { t.draw() } -> std::same_as<void>;
};

template<Drawable T>
void render(T& obj) {
    obj.draw();
}
```

### 异常处理

```java
// Java: 检查型异常
public void readFile() throws IOException {
    // 必须声明或处理
}
```

```cpp
// C++: 非检查型异常
void readFile() {  // 可能抛任何异常
    throw std::runtime_error("error");
}

// C++11: noexcept 说明符
void safeFunction() noexcept {  // 承诺不抛异常
    // 如果抛异常，程序终止
}

// 异常规范已废弃，不要使用
void oldStyle() throw(std::exception);  // 已废弃！
```

## 从 Go 迁移

### 并发模型差异

```go
// Go: Goroutine + Channel
func worker(ch chan int) {
    for n := range ch {
        fmt.Println(n)
    }
}

func main() {
    ch := make(chan int)
    go worker(ch)
    ch <- 42
    close(ch)
}
```

```cpp
// C++: std::thread + std::mutex/atomic/condition_variable
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

class Channel {
    std::queue<int> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool closed_ = false;

public:
    void send(int value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        cv_.notify_one();
    }

    std::optional<int> receive() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt;
        int value = queue_.front();
        queue_.pop();
        return value;
    }

    void close() {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
        cv_.notify_all();
    }
};
```

### 错误处理差异

```go
// Go: 多返回值
func divide(a, b int) (int, error) {
    if b == 0 {
        return 0, errors.New("division by zero")
    }
    return a / b, nil
}

result, err := divide(10, 0)
if err != nil {
    // 处理错误
}
```

```cpp
// C++17: std::optional 或 std::variant
std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

if (auto result = divide(10, 2)) {
    std::cout << *result;
}

// C++23: std::expected
std::expected<int, std::string> divide(int a, int b) {
    if (b == 0) return std::unexpected("division by zero");
    return a / b;
}

auto result = divide(10, 0);
if (result) {
    std::cout << *result;
} else {
    std::cerr << result.error();
}
```

### 结构体与方法

```go
// Go
type Point struct {
    X, Y int
}

func (p Point) Distance() float64 {
    return math.Sqrt(float64(p.X*p.X + p.Y*p.Y))
}
```

```cpp
// C++
struct Point {
    int x, y;
    
    double distance() const {
        return std::sqrt(x * x + y * y);
    }
};

// 或使用自由函数
double distance(const Point& p) {
    return std::sqrt(p.x * p.x + p.y * p.y);
}
```

## 常见迁移问题

### 1. 忘记初始化

```cpp
int x;  // 未初始化！值不确定
int y = 0;  // OK
int z{};    // OK，值初始化为 0

// 类成员也需要初始化
class Widget {
    int value = 0;  // C++11 默认成员初始化
    std::string name;  // OK，string 默认为空
};
```

### 2. 混淆指针和引用

```cpp
void modify_ptr(int* p) {
    *p = 42;  // 需要解引用
}

void modify_ref(int& r) {
    r = 42;  // 直接使用
}

int x = 0;
modify_ptr(&x);  // 传地址
modify_ref(x);   // 传引用

int* ptr = nullptr;  // 指针可以为空
// int& ref = nullptr;  // 引用不能为空
```

### 3. 值传递 vs 引用传递

```cpp
// 小对象：值传递
void process(int x) { }

// 大对象只读：const 引用
void process(const std::string& s) { }

// 需要修改：非 const 引用
void process(std::string& s) { }

// 转移所有权：移动
void process(std::unique_ptr<Widget> w) { }
```

### 4. 理解作用域和生命周期

```cpp
std::string* dangerous() {
    std::string s = "hello";
    return &s;  // 危险！s 将被销毁
}

std::string safe() {
    std::string s = "hello";
    return s;  // OK，返回拷贝或移动
}

// RAII 作用域
{
    std::lock_guard<std::mutex> lock(mtx);
    // 临界区
}  // lock 析构，自动释放锁
```

```{note}
从其他语言迁移到 C++ 最重要的是理解：
1. 值语义 vs 引用语义
2. 手动内存管理（通过 RAII 和智能指针）
3. 编译期 vs 运行期
4. 模板与泛型的区别
```
