# 条件变量

## 基本用法

条件变量用于线程间同步，允许线程等待某个条件成立。

```cpp
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> data_queue;
bool finished = false;

void producer() {
    for (int i = 0; i < 10; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            data_queue.push(i);
        }
        cv.notify_one();  // 通知一个等待的线程
    }
    
    {
        std::lock_guard<std::mutex> lock(mtx);
        finished = true;
    }
    cv.notify_all();  // 通知所有等待的线程
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        
        // 等待条件：队列非空或已完成
        cv.wait(lock, [] { 
            return !data_queue.empty() || finished; 
        });
        
        while (!data_queue.empty()) {
            int value = data_queue.front();
            data_queue.pop();
            lock.unlock();  // 处理数据时不持有锁
            std::cout << "Consumed: " << value << "\n";
            lock.lock();
        }
        
        if (finished && data_queue.empty()) {
            break;
        }
    }
}
```

## wait 的变体

### wait with predicate

```cpp
std::unique_lock<std::mutex> lock(mtx);

// 基本 wait - 必须处理虚假唤醒
while (!condition) {
    cv.wait(lock);
}

// 带谓词的 wait - 自动处理虚假唤醒（推荐）
cv.wait(lock, []{ return condition; });
// 等价于：
// while (!condition) cv.wait(lock);
```

### wait_for

```cpp
std::unique_lock<std::mutex> lock(mtx);

// 等待指定时间
auto status = cv.wait_for(lock, std::chrono::seconds(1));
if (status == std::cv_status::timeout) {
    // 超时
}

// 带谓词的 wait_for
bool success = cv.wait_for(lock, std::chrono::seconds(1), 
                           []{ return condition; });
if (!success) {
    // 超时且条件仍不满足
}
```

### wait_until

```cpp
std::unique_lock<std::mutex> lock(mtx);

auto deadline = std::chrono::steady_clock::now() + 
                std::chrono::seconds(5);

bool success = cv.wait_until(lock, deadline, []{ return condition; });
```

## 生产者-消费者队列

```cpp
template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    size_t max_size_;

public:
    explicit ThreadSafeQueue(size_t max_size = 100) 
        : max_size_(max_size) {}

    void push(T value) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { 
            return queue_.size() < max_size_; 
        });
        queue_.push(std::move(value));
        lock.unlock();
        not_empty_.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { 
            return !queue_.empty(); 
        });
        T value = std::move(queue_.front());
        queue_.pop();
        lock.unlock();
        not_full_.notify_one();
        return value;
    }

    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        T value = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};
```

## 常见陷阱

### 1. 忘记使用 unique_lock

```cpp
// 错误：lock_guard 不支持 wait
std::lock_guard<std::mutex> lock(mtx);  // 错误！
cv.wait(lock);  // 编译错误

// 正确：使用 unique_lock
std::unique_lock<std::mutex> lock(mtx);
cv.wait(lock);  // OK
```

### 2. 虚假唤醒

```cpp
// 错误：不处理虚假唤醒
cv.wait(lock);
// 可能在条件不满足时被唤醒！

// 正确：使用带谓词的 wait
cv.wait(lock, []{ return condition; });

// 或手动循环检查
while (!condition) {
    cv.wait(lock);
}
```

### 3. 丢失唤醒

```cpp
// 可能丢失唤醒的场景
void thread1() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock);  // 如果 thread2 先执行，这里会永远等待
}

void thread2() {
    cv.notify_one();  // 如果此时没有线程在等待，通知会丢失
}

// 解决方案：使用条件变量总是配合条件使用
std::atomic<bool> ready{false};

void thread1() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, []{ return ready.load(); });  // 先检查条件
}

void thread2() {
    ready = true;
    cv.notify_one();
}
```

### 4. 通知时持有锁

```cpp
// 不推荐：持有锁时通知
{
    std::lock_guard<std::mutex> lock(mtx);
    data_ready = true;
    cv.notify_one();  // 唤醒的线程会立即阻塞在锁上
}

// 推荐：释放锁后通知
{
    std::lock_guard<std::mutex> lock(mtx);
    data_ready = true;
}
cv.notify_one();  // 唤醒的线程可以立即获取锁
```

## std::condition_variable_any

```cpp
// 可以与任何满足 BasicLockable 的锁配合使用
std::condition_variable_any cv_any;
std::shared_mutex sm;

void reader() {
    std::shared_lock<std::shared_mutex> lock(sm);
    cv_any.wait(lock, []{ return condition; });
    // 读取数据
}
```

## C++20: std::counting_semaphore

```cpp
#include <semaphore>

// 信号量：限制并发访问数量
std::counting_semaphore<10> sem(10);  // 最多 10 个并发

void worker() {
    sem.acquire();  // 获取许可（阻塞）
    // 访问受限资源
    sem.release();  // 释放许可
}

// 二元信号量
std::binary_semaphore bin_sem(1);

void mutex_like() {
    bin_sem.acquire();  // 类似 lock
    // 临界区
    bin_sem.release();  // 类似 unlock
}
```

## C++20: std::latch 和 std::barrier

```cpp
#include <latch>
#include <barrier>

// Latch：一次性屏障
void latch_example() {
    std::latch start_latch(1);
    std::latch done_latch(3);

    auto worker = [&](int id) {
        start_latch.wait();  // 等待开始信号
        // 工作
        done_latch.count_down();  // 完成
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);

    start_latch.count_down();  // 发送开始信号
    done_latch.wait();  // 等待所有工作完成

    t1.join(); t2.join(); t3.join();
}

// Barrier：可重用屏障
void barrier_example() {
    auto on_completion = []() noexcept {
        std::cout << "All threads reached barrier\n";
    };
    
    std::barrier sync_point(3, on_completion);

    auto worker = [&](int id) {
        for (int i = 0; i < 3; ++i) {
            // 阶段 i 的工作
            sync_point.arrive_and_wait();  // 等待其他线程
        }
    };

    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    std::thread t3(worker, 3);

    t1.join(); t2.join(); t3.join();
}
```

```{tip}
条件变量使用要点：
1. 总是配合谓词使用，处理虚假唤醒
2. 使用 `std::unique_lock`，不是 `std::lock_guard`
3. 考虑在释放锁后发送通知
4. C++20 提供了更简洁的同步原语
```
