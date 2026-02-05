# 线程基础

## std::thread 基本用法

```cpp
#include <thread>
#include <iostream>

// 方式1：函数指针
void worker(int id) {
    std::cout << "Worker " << id << " running\n";
}

// 方式2：Lambda
auto lambda_worker = [](int id) {
    std::cout << "Lambda worker " << id << "\n";
};

// 方式3：函数对象
struct Functor {
    void operator()(int id) const {
        std::cout << "Functor worker " << id << "\n";
    }
};

int main() {
    std::thread t1(worker, 1);
    std::thread t2(lambda_worker, 2);
    std::thread t3(Functor{}, 3);
    
    t1.join();  // 等待线程结束
    t2.join();
    t3.join();
}
```

## 线程的生命周期

### join vs detach

```cpp
void example() {
    std::thread t([]{ /* work */ });
    
    // 必须选择一种方式处理线程
    // 方式1：等待线程结束
    t.join();
    
    // 方式2：分离线程（后台运行）
    // t.detach();
    
    // 如果都不做，析构时会调用 std::terminate()！
}
```

### RAII 线程管理

```cpp
// 确保线程被正确 join
class jthread_guard {
    std::thread& t_;
public:
    explicit jthread_guard(std::thread& t) : t_(t) {}
    ~jthread_guard() {
        if (t_.joinable()) {
            t_.join();
        }
    }
    // 禁止拷贝
    jthread_guard(const jthread_guard&) = delete;
    jthread_guard& operator=(const jthread_guard&) = delete;
};

// C++20: std::jthread 自动 join
#include <stop_token>

void worker(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        // 工作
    }
}

void example() {
    std::jthread t(worker);  // 析构时自动 join
    // t.request_stop();     // 请求停止
}  // 自动调用 request_stop() 和 join()
```

## 参数传递

### 值传递 vs 引用传递

```cpp
void work_by_value(int x) { x++; }
void work_by_ref(int& x) { x++; }

int main() {
    int value = 0;
    
    // 默认值传递（拷贝）
    std::thread t1(work_by_value, value);
    t1.join();
    std::cout << value;  // 0，未改变
    
    // 引用传递需要 std::ref
    std::thread t2(work_by_ref, std::ref(value));
    t2.join();
    std::cout << value;  // 1，已改变
    
    // 危险：引用已销毁的对象
    // std::thread t([&value]{ value++; });
    // t.detach();  // 危险！value 可能已销毁
}
```

### 移动语义

```cpp
void process(std::unique_ptr<int> ptr) {
    std::cout << *ptr << "\n";
}

int main() {
    auto ptr = std::make_unique<int>(42);
    
    // std::thread t(process, ptr);  // 错误！unique_ptr 不能拷贝
    std::thread t(process, std::move(ptr));  // OK
    t.join();
}
```

## 线程局部存储

```cpp
// 每个线程有独立的副本
thread_local int tls_counter = 0;

void worker() {
    ++tls_counter;
    std::cout << "TLS counter: " << tls_counter << "\n";
}

int main() {
    std::thread t1(worker);  // 输出 1
    std::thread t2(worker);  // 输出 1（独立的副本）
    t1.join();
    t2.join();
}
```

## 获取线程信息

```cpp
#include <thread>
#include <iostream>

int main() {
    // 当前线程 ID
    std::cout << "Main thread ID: " << std::this_thread::get_id() << "\n";
    
    // 硬件并发数
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << "Hardware concurrency: " << n << "\n";
    
    std::thread t([] {
        std::cout << "Worker thread ID: " << std::this_thread::get_id() << "\n";
    });
    
    // 获取线程句柄
    auto handle = t.native_handle();
    
    t.join();
}
```

## 线程休眠

```cpp
#include <thread>
#include <chrono>

void worker() {
    using namespace std::chrono_literals;  // C++14
    
    std::this_thread::sleep_for(100ms);     // 休眠 100 毫秒
    std::this_thread::sleep_for(1s);        // 休眠 1 秒
    
    auto wake_time = std::chrono::steady_clock::now() + 500ms;
    std::this_thread::sleep_until(wake_time);  // 休眠到指定时间点
    
    std::this_thread::yield();  // 让出 CPU 时间片
}
```

## 最佳实践

### 1. 避免在构造函数中启动线程

```cpp
class BadExample {
    std::thread t_;
public:
    BadExample() : t_(&BadExample::worker, this) {
        // 危险！对象尚未完全构造
    }
    void worker() { /* 访问成员 */ }
};

class GoodExample {
    std::thread t_;
public:
    void start() {
        t_ = std::thread(&GoodExample::worker, this);
    }
    void worker() { /* 安全 */ }
    ~GoodExample() {
        if (t_.joinable()) t_.join();
    }
};
```

### 2. 线程安全地传递数据

```cpp
// 使用 promise/future 传递结果
#include <future>

int compute() { return 42; }

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    
    std::thread t([&promise] {
        promise.set_value(compute());
    });
    
    int result = future.get();  // 阻塞等待结果
    t.join();
}
```

### 3. 合理的线程数量

```cpp
unsigned int num_threads = std::thread::hardware_concurrency();
if (num_threads == 0) num_threads = 2;  // 默认值

// 对于 CPU 密集型任务
// 线程数 ≈ CPU 核心数

// 对于 IO 密集型任务
// 线程数可以更多，取决于 IO 等待时间比例
```

```{warning}
不要创建过多线程！线程创建和上下文切换都有开销。
对于大量短任务，考虑使用线程池。
```
