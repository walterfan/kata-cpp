# 并发编程陷阱

## 数据竞争

### 什么是数据竞争

当两个线程同时访问同一内存位置，且至少一个是写操作时，就发生了数据竞争。

```cpp
int counter = 0;

void bad_increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;  // 数据竞争！
    }
}

// counter++ 实际上是三个操作：
// 1. 读取 counter
// 2. 增加 1
// 3. 写回 counter
// 多线程可能交错执行这些操作
```

### 解决方案

```cpp
// 方案1：互斥锁
std::mutex mtx;
void safe_increment_mutex() {
    std::lock_guard<std::mutex> lock(mtx);
    ++counter;
}

// 方案2：原子操作
std::atomic<int> atomic_counter{0};
void safe_increment_atomic() {
    ++atomic_counter;
}

// 方案3：线程局部存储
thread_local int local_counter = 0;
void thread_local_increment() {
    ++local_counter;  // 每个线程独立
}
```

## 死锁

### 四个必要条件

1. 互斥：资源不能共享
2. 持有并等待：持有资源的同时等待其他资源
3. 不可抢占：资源只能自愿释放
4. 循环等待：存在等待环

### 常见死锁场景

```cpp
std::mutex m1, m2;

void thread_a() {
    std::lock_guard<std::mutex> lock1(m1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock2(m2);  // 等待 m2
}

void thread_b() {
    std::lock_guard<std::mutex> lock1(m2);  // 已持有 m2
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock2(m1);  // 等待 m1 -> 死锁！
}
```

### 避免死锁

```cpp
// 方案1：使用 std::scoped_lock (C++17)
void safe_a() {
    std::scoped_lock lock(m1, m2);  // 原子获取多个锁
}

// 方案2：使用 std::lock
void safe_b() {
    std::unique_lock<std::mutex> lock1(m1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(m2, std::defer_lock);
    std::lock(lock1, lock2);
}

// 方案3：固定加锁顺序
void ordered_a() {
    std::lock_guard<std::mutex> lock1(m1);  // 总是先 m1
    std::lock_guard<std::mutex> lock2(m2);  // 再 m2
}

// 方案4：使用 try_lock 并重试
void trylock_a() {
    while (true) {
        std::unique_lock<std::mutex> lock1(m1);
        if (m2.try_lock()) {
            std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
            break;  // 成功获取两个锁
        }
        // 失败，释放 m1，稍后重试
        std::this_thread::yield();
    }
}
```

## 竞态条件

### 检查后使用 (Time-of-Check to Time-of-Use, TOCTOU)

```cpp
// 危险：检查和使用之间可能发生变化
void dangerous_check_then_use(std::map<int, int>& map, int key) {
    if (map.find(key) != map.end()) {  // 检查
        // 另一个线程可能删除了 key
        std::cout << map[key];  // 使用：可能失败！
    }
}

// 安全：原子操作
void safe_access(std::map<int, int>& map, std::mutex& mtx, int key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = map.find(key);
    if (it != map.end()) {
        std::cout << it->second;  // 在锁内使用
    }
}
```

### 复合操作

```cpp
// 危险：非原子的复合操作
std::atomic<int> counter{0};

void dangerous_update() {
    if (counter < 100) {  // 检查
        ++counter;         // 更新：可能导致 counter > 100
    }
}

// 安全：使用 CAS
void safe_update() {
    int expected;
    do {
        expected = counter.load();
        if (expected >= 100) return;
    } while (!counter.compare_exchange_weak(expected, expected + 1));
}
```

## 内存可见性问题

### 缓存一致性

```cpp
// 可能的问题：编译器或 CPU 重排序
int data = 0;
bool ready = false;

void producer() {
    data = 42;
    ready = true;  // 可能被重排到 data = 42 之前
}

void consumer() {
    while (!ready);  // 可能看到 ready = true
    assert(data == 42);  // 但 data 可能还是 0
}

// 解决方案：使用原子变量和适当的内存序
std::atomic<int> data{0};
std::atomic<bool> ready{false};

void safe_producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_release);
}

void safe_consumer() {
    while (!ready.load(std::memory_order_acquire));
    assert(data.load(std::memory_order_relaxed) == 42);  // 保证成立
}
```

## 优先级反转

当高优先级线程等待低优先级线程持有的锁时发生。

```cpp
// 场景：
// - 低优先级线程 L 持有锁
// - 高优先级线程 H 等待锁
// - 中优先级线程 M 抢占 L
// 结果：H 间接被 M 阻塞

// 解决方案1：优先级继承（操作系统支持）
// 解决方案2：优先级天花板协议
// 解决方案3：避免在高优先级线程中使用共享锁
```

## 常见错误模式

### 1. 在持有锁时调用回调

```cpp
class BadDesign {
    std::mutex mutex_;
    std::function<void()> callback_;
    
    void process() {
        std::lock_guard<std::mutex> lock(mutex_);
        // callback_ 可能尝试获取同一个锁 -> 死锁
        callback_();
    }
};

class BetterDesign {
    std::mutex mutex_;
    std::function<void()> callback_;
    
    void process() {
        std::function<void()> cb;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            cb = callback_;  // 复制回调
        }
        cb();  // 在锁外调用
    }
};
```

### 2. 条件变量的虚假唤醒

```cpp
// 错误：不处理虚假唤醒
void bad_wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv.wait(lock);  // 可能在条件不满足时被唤醒
    process_data();  // 数据可能无效！
}

// 正确：使用带谓词的 wait
void good_wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv.wait(lock, []{ return data_ready; });
    process_data();
}
```

### 3. 忘记 join 或 detach

```cpp
// 错误：线程对象销毁时未 join 或 detach
void bad_thread_usage() {
    std::thread t([]{ /* work */ });
    // 如果忘记 join 或 detach
}  // 调用 std::terminate()！

// 使用 C++20 std::jthread 自动管理
void good_thread_usage() {
    std::jthread t([]{ /* work */ });
}  // 自动 join
```

### 4. 引用已销毁的数据

```cpp
// 错误
void dangerous() {
    int local = 42;
    std::thread t([&local]{ 
        std::cout << local;  // local 可能已销毁
    });
    t.detach();
}  // local 销毁

// 正确：值捕获或确保生命周期
void safe() {
    int local = 42;
    std::thread t([local]{  // 值捕获
        std::cout << local;
    });
    t.join();  // 或确保 local 存活
}
```

## 调试并发问题

### 工具

```bash
# Thread Sanitizer (TSan)
g++ -fsanitize=thread -g program.cpp

# Helgrind (Valgrind 工具)
valgrind --tool=helgrind ./program

# Intel Inspector
# Dr. Memory
```

### 技巧

1. **可重现测试**：使用确定性调度器
2. **日志记录**：添加线程 ID 和时间戳
3. **断言**：检查不变量
4. **模型检查**：如 CHESS

```cpp
// 调试日志
#define THREAD_LOG(msg) \
    std::cerr << "[" << std::this_thread::get_id() << "] " \
              << __FILE__ << ":" << __LINE__ << " " << msg << "\n"
```

```{warning}
并发 bug 通常难以重现和调试。预防胜于治疗：
1. 尽量使用高层抽象（如线程池、并发容器）
2. 最小化共享状态
3. 优先使用不可变数据
4. 使用静态分析和运行时检测工具
```
