# 原子操作

## std::atomic 基础

原子操作是不可分割的操作，不会被其他线程中断。

```cpp
#include <atomic>
#include <thread>

std::atomic<int> counter{0};

void increment() {
    for (int i = 0; i < 100000; ++i) {
        ++counter;  // 原子操作，线程安全
    }
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);
    t1.join();
    t2.join();
    std::cout << counter << "\n";  // 200000（保证正确）
}
```

## 原子操作类型

### 基本原子操作

```cpp
std::atomic<int> a{0};

// 原子读取
int value = a.load();
int value2 = a;  // 隐式 load

// 原子写入
a.store(42);
a = 42;  // 隐式 store

// 原子交换
int old = a.exchange(100);  // 返回旧值

// 原子比较交换 (CAS)
int expected = 100;
bool success = a.compare_exchange_strong(expected, 200);
// 如果 a == expected，则 a = 200，返回 true
// 否则 expected = a，返回 false
```

### 原子算术操作

```cpp
std::atomic<int> a{0};

a.fetch_add(5);   // a += 5，返回旧值
a.fetch_sub(3);   // a -= 3，返回旧值
a += 10;          // 等价于 fetch_add(10)
a -= 5;           // 等价于 fetch_sub(5)
++a;              // 原子递增
a++;              // 原子递增（后缀）

// 位操作
std::atomic<unsigned> flags{0};
flags.fetch_or(0x01);   // 设置位
flags.fetch_and(~0x01); // 清除位
flags.fetch_xor(0x01);  // 翻转位
```

## 内存序 (Memory Order)

### 六种内存序

```cpp
// 最宽松：无同步保证
std::memory_order_relaxed

// 获取语义：阻止读操作被重排到此之前
std::memory_order_acquire

// 释放语义：阻止写操作被重排到此之后
std::memory_order_release

// 获取-释放语义
std::memory_order_acq_rel

// 顺序一致性（默认）：最严格
std::memory_order_seq_cst

// 消费语义（通常不推荐使用）
std::memory_order_consume
```

### 使用示例

```cpp
std::atomic<int> data{0};
std::atomic<bool> ready{false};

void producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_release);  // 发布
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)) {  // 获取
        // 等待
    }
    assert(data.load(std::memory_order_relaxed) == 42);  // 保证成立
}
```

### 顺序一致性 vs Relaxed

```cpp
// 顺序一致性（默认）- 最安全，可能较慢
std::atomic<int> x{0};
x.store(1);  // 等价于 x.store(1, std::memory_order_seq_cst);

// Relaxed - 最快，但需要小心
std::atomic<int> counter{0};
// 只用于计数，不用于同步
counter.fetch_add(1, std::memory_order_relaxed);
```

## 无锁数据结构示例

### 无锁栈

```cpp
template<typename T>
class LockFreeStack {
    struct Node {
        T data;
        Node* next;
        Node(T value) : data(std::move(value)), next(nullptr) {}
    };
    
    std::atomic<Node*> head{nullptr};
    
public:
    void push(T value) {
        Node* new_node = new Node(std::move(value));
        new_node->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node->next, new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed)) {
            // CAS 失败，重试
        }
    }
    
    std::optional<T> pop() {
        Node* old_head = head.load(std::memory_order_acquire);
        while (old_head && 
               !head.compare_exchange_weak(old_head, old_head->next,
                                           std::memory_order_release,
                                           std::memory_order_acquire)) {
            // CAS 失败，重试
        }
        if (old_head) {
            T value = std::move(old_head->data);
            delete old_head;  // 注意：实际需要延迟删除（ABA问题）
            return value;
        }
        return std::nullopt;
    }
};
```

### 自旋锁

```cpp
class SpinLock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    
public:
    void lock() {
        while (flag.test_and_set(std::memory_order_acquire)) {
            // 自旋等待
            // 可以加入 pause 指令优化
        }
    }
    
    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

// C++20: 更好的 test 方法
class SpinLock20 {
    std::atomic<bool> flag{false};
    
public:
    void lock() {
        while (true) {
            // 先检查，避免频繁的 CAS
            if (!flag.exchange(true, std::memory_order_acquire)) {
                break;
            }
            // 等待标志变为 false
            while (flag.load(std::memory_order_relaxed)) {
                // 可以使用 flag.wait(true) (C++20)
            }
        }
    }
    
    void unlock() {
        flag.store(false, std::memory_order_release);
        // flag.notify_one();  // C++20
    }
};
```

## std::atomic_flag

```cpp
// 最简单的原子类型，保证无锁
std::atomic_flag flag = ATOMIC_FLAG_INIT;

void thread1() {
    // 设置标志并获取旧值
    bool was_set = flag.test_and_set(std::memory_order_acquire);
}

void thread2() {
    // 清除标志
    flag.clear(std::memory_order_release);
}

// C++20 新增
void thread3() {
    bool value = flag.test();  // 只读取，不修改
    flag.wait(false);          // 等待值变化
    flag.notify_one();         // 唤醒等待线程
}
```

## 原子智能指针 (C++20)

```cpp
#include <memory>

// C++20: std::atomic<std::shared_ptr<T>>
std::atomic<std::shared_ptr<int>> ptr;

void writer() {
    auto new_ptr = std::make_shared<int>(42);
    ptr.store(new_ptr);  // 原子存储
}

void reader() {
    auto p = ptr.load();  // 原子加载
    if (p) {
        std::cout << *p << "\n";
    }
}
```

## 性能考虑

### 何时使用原子操作

```cpp
// 适合原子操作的场景
// 1. 简单计数器
std::atomic<int> request_count{0};

// 2. 标志位
std::atomic<bool> shutdown{false};

// 3. 单生产者单消费者索引
std::atomic<size_t> head{0};
std::atomic<size_t> tail{0};

// 不适合原子操作的场景
// 1. 需要保护多个变量
// 2. 复杂的数据结构更新
// 3. 需要条件等待
```

### 避免伪共享

```cpp
// 伪共享：不同线程修改同一缓存行的不同变量
struct BadLayout {
    std::atomic<int> counter1;  // 可能在同一缓存行
    std::atomic<int> counter2;
};

// 解决：使用 alignas 填充
struct GoodLayout {
    alignas(64) std::atomic<int> counter1;  // 64 字节对齐
    alignas(64) std::atomic<int> counter2;
};

// C++17: hardware_destructive_interference_size
struct ModernLayout {
    alignas(std::hardware_destructive_interference_size) 
    std::atomic<int> counter1;
    
    alignas(std::hardware_destructive_interference_size) 
    std::atomic<int> counter2;
};
```

```{warning}
原子操作不是银弹：
1. 无锁算法很难正确实现
2. 需要处理 ABA 问题
3. 在某些架构上可能不是真正无锁
4. 使用 `is_lock_free()` 检查是否真的无锁
```
