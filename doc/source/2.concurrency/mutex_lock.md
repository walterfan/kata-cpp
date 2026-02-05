# 互斥锁与锁管理

## std::mutex 基本用法

```cpp
#include <mutex>
#include <thread>

std::mutex mtx;
int shared_data = 0;

void unsafe_increment() {
    for (int i = 0; i < 100000; ++i) {
        ++shared_data;  // 数据竞争！
    }
}

void safe_increment() {
    for (int i = 0; i < 100000; ++i) {
        mtx.lock();
        ++shared_data;  // 安全
        mtx.unlock();
    }
}
```

## RAII 锁管理

### std::lock_guard

```cpp
void safe_function() {
    std::lock_guard<std::mutex> lock(mtx);  // 构造时加锁
    ++shared_data;
    // 可能抛异常...
}  // 析构时自动解锁，即使发生异常

// C++17: 模板参数推导
void modern_function() {
    std::lock_guard lock(mtx);  // 不需要 <std::mutex>
}
```

### std::unique_lock

```cpp
void flexible_function() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // 可以手动解锁和重新加锁
    lock.unlock();
    // 做一些不需要锁的工作
    lock.lock();
    
    // 可以延迟加锁
    std::unique_lock<std::mutex> lock2(mtx, std::defer_lock);
    // 稍后加锁
    lock2.lock();
    
    // 可以尝试加锁
    std::unique_lock<std::mutex> lock3(mtx, std::try_to_lock);
    if (lock3.owns_lock()) {
        // 成功获取锁
    }
}
```

### std::scoped_lock (C++17)

```cpp
std::mutex m1, m2;

// 避免死锁的多锁获取
void safe_multi_lock() {
    std::scoped_lock lock(m1, m2);  // 原子地获取多个锁
    // 使用 m1 和 m2 保护的数据
}

// 对比：手动获取多个锁（可能死锁）
void dangerous_multi_lock() {
    std::lock_guard<std::mutex> lock1(m1);
    std::lock_guard<std::mutex> lock2(m2);  // 可能死锁！
}
```

## 其他互斥锁类型

### std::recursive_mutex

```cpp
std::recursive_mutex rmtx;

void recursive_function(int n) {
    std::lock_guard<std::recursive_mutex> lock(rmtx);
    if (n > 0) {
        recursive_function(n - 1);  // 同一线程可以再次获取锁
    }
}
```

### std::timed_mutex

```cpp
std::timed_mutex tmtx;

void timed_function() {
    // 尝试在指定时间内获取锁
    if (tmtx.try_lock_for(std::chrono::milliseconds(100))) {
        // 获取锁成功
        tmtx.unlock();
    } else {
        // 超时，未获取到锁
    }
    
    // 或指定截止时间
    auto deadline = std::chrono::steady_clock::now() + 
                    std::chrono::seconds(1);
    if (tmtx.try_lock_until(deadline)) {
        tmtx.unlock();
    }
}
```

### std::shared_mutex (C++17)

```cpp
#include <shared_mutex>

std::shared_mutex rw_mutex;
std::map<int, int> data;

void reader() {
    std::shared_lock lock(rw_mutex);  // 共享锁，允许多个读者
    auto it = data.find(1);
}

void writer() {
    std::unique_lock lock(rw_mutex);  // 独占锁
    data[1] = 42;
}
```

## 死锁避免

### 死锁场景

```cpp
std::mutex m1, m2;

void thread1() {
    std::lock_guard<std::mutex> lock1(m1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock2(m2);  // 等待 m2
}

void thread2() {
    std::lock_guard<std::mutex> lock1(m2);  // 注意顺序相反
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::lock_guard<std::mutex> lock2(m1);  // 等待 m1 -> 死锁！
}
```

### 解决方案

```cpp
// 方案1：使用 std::scoped_lock (C++17)
void safe_thread1() {
    std::scoped_lock lock(m1, m2);
    // 安全
}

// 方案2：使用 std::lock
void safe_thread2() {
    std::unique_lock<std::mutex> lock1(m1, std::defer_lock);
    std::unique_lock<std::mutex> lock2(m2, std::defer_lock);
    std::lock(lock1, lock2);  // 原子地获取两个锁
}

// 方案3：固定加锁顺序
void ordered_thread() {
    std::lock_guard<std::mutex> lock1(m1);  // 总是先锁 m1
    std::lock_guard<std::mutex> lock2(m2);  // 再锁 m2
}

// 方案4：使用 try_lock 避免阻塞
void trylock_thread() {
    while (true) {
        std::unique_lock<std::mutex> lock1(m1);
        if (m2.try_lock()) {
            std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
            // 获取成功
            break;
        }
        // 失败，释放 m1 并重试
    }
}
```

## 线程安全的单例模式

### 双重检查锁定（错误示例）

```cpp
// 经典 DCLP - 在 C++11 之前有问题
class Singleton {
    static Singleton* instance;
    static std::mutex mtx;
public:
    static Singleton* get() {
        if (instance == nullptr) {  // 第一次检查
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {  // 第二次检查
                instance = new Singleton();  // 问题：可能重排序
            }
        }
        return instance;
    }
};
```

### 正确实现

```cpp
// 方案1：使用 std::call_once
class Singleton {
    static std::unique_ptr<Singleton> instance;
    static std::once_flag flag;
public:
    static Singleton& get() {
        std::call_once(flag, [] {
            instance = std::make_unique<Singleton>();
        });
        return *instance;
    }
};

// 方案2：Meyers Singleton（推荐）
class Singleton {
public:
    static Singleton& get() {
        static Singleton instance;  // C++11 保证线程安全
        return instance;
    }
};
```

## 锁的性能考虑

### 减少锁的持有时间

```cpp
void inefficient() {
    std::lock_guard<std::mutex> lock(mtx);
    // 读取数据
    auto data = shared_data;
    // 处理数据（长时间操作）
    auto result = expensive_calculation(data);  // 不需要锁
    // 写回数据
    shared_data = result;
}

void efficient() {
    int data;
    {
        std::lock_guard<std::mutex> lock(mtx);
        data = shared_data;  // 快速读取
    }
    
    auto result = expensive_calculation(data);  // 无锁处理
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        shared_data = result;  // 快速写入
    }
}
```

### 使用细粒度锁

```cpp
// 粗粒度锁
class CoarseGrained {
    std::mutex mtx;
    std::map<int, int> data1;
    std::map<int, int> data2;
    
    void update1(int k, int v) {
        std::lock_guard<std::mutex> lock(mtx);
        data1[k] = v;
    }
};

// 细粒度锁
class FineGrained {
    std::mutex mtx1, mtx2;
    std::map<int, int> data1;
    std::map<int, int> data2;
    
    void update1(int k, int v) {
        std::lock_guard<std::mutex> lock(mtx1);
        data1[k] = v;
    }
    
    void update2(int k, int v) {
        std::lock_guard<std::mutex> lock(mtx2);
        data2[k] = v;
    }
};
```

```{tip}
锁的选择指南：
- 简单场景：`std::lock_guard`
- 需要灵活控制：`std::unique_lock`
- 多个锁：`std::scoped_lock` (C++17)
- 读多写少：`std::shared_mutex`
```
