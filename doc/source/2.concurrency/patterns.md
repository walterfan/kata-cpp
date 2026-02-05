# 并发模式

## 线程池

```cpp
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop_(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this] { worker_loop(); });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (auto& worker : workers_) {
            worker.join();
        }
    }

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>> 
    {
        using return_type = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

        std::future<return_type> result = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (stop_) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }
            tasks_.emplace([task]() { (*task)(); });
        }
        condition_.notify_one();
        return result;
    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                condition_.wait(lock, [this] { 
                    return stop_ || !tasks_.empty(); 
                });
                if (stop_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};

// 使用示例
int main() {
    ThreadPool pool(4);
    
    auto result1 = pool.enqueue([](int x) { return x * x; }, 42);
    auto result2 = pool.enqueue([]() { return "hello"; });
    
    std::cout << result1.get() << "\n";  // 1764
    std::cout << result2.get() << "\n";  // hello
}
```

## 生产者-消费者模式

```cpp
template<typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(size_t capacity) 
        : capacity_(capacity), closed_(false) {}

    bool push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this] { 
            return queue_.size() < capacity_ || closed_; 
        });
        if (closed_) return false;
        queue_.push(std::move(item));
        not_empty_.notify_one();
        return true;
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this] { 
            return !queue_.empty() || closed_; 
        });
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return item;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        not_full_.notify_all();
        not_empty_.notify_all();
    }

private:
    std::queue<T> queue_;
    size_t capacity_;
    bool closed_;
    std::mutex mutex_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};

// 使用
void producer(BoundedQueue<int>& q) {
    for (int i = 0; i < 100; ++i) {
        q.push(i);
    }
}

void consumer(BoundedQueue<int>& q) {
    while (auto item = q.pop()) {
        std::cout << *item << "\n";
    }
}
```

## 读写锁模式

```cpp
#include <shared_mutex>

class ThreadSafeMap {
public:
    void insert(const std::string& key, int value) {
        std::unique_lock lock(mutex_);  // 独占锁
        data_[key] = value;
    }

    std::optional<int> find(const std::string& key) const {
        std::shared_lock lock(mutex_);  // 共享锁
        auto it = data_.find(key);
        if (it != data_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    bool update(const std::string& key, int value) {
        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
        if (it != data_.end()) {
            it->second = value;
            return true;
        }
        return false;
    }

private:
    mutable std::shared_mutex mutex_;
    std::map<std::string, int> data_;
};
```

## 双缓冲模式

```cpp
template<typename T>
class DoubleBuffer {
public:
    DoubleBuffer() : current_(&buffer1_), next_(&buffer2_) {}

    // 写入下一个缓冲区
    T& get_next() { return *next_; }
    
    // 交换缓冲区（需要同步）
    void swap() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::swap(current_, next_);
    }
    
    // 读取当前缓冲区
    const T& get_current() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return *current_;
    }

private:
    T buffer1_, buffer2_;
    T* current_;
    T* next_;
    mutable std::mutex mutex_;
};

// 无锁版本（使用原子指针）
template<typename T>
class LockFreeDoubleBuffer {
public:
    LockFreeDoubleBuffer() {
        current_.store(&buffer1_);
    }

    // 写入非当前缓冲区
    T& get_back() {
        return current_.load() == &buffer1_ ? buffer2_ : buffer1_;
    }

    void swap() {
        T* expected = current_.load();
        T* next = (expected == &buffer1_) ? &buffer2_ : &buffer1_;
        current_.compare_exchange_strong(expected, next);
    }

    const T& get_current() const {
        return *current_.load();
    }

private:
    T buffer1_, buffer2_;
    std::atomic<T*> current_;
};
```

## Active Object 模式

```cpp
class ActiveObject {
public:
    ActiveObject() : done_(false) {
        thread_ = std::thread([this] { run(); });
    }

    ~ActiveObject() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            done_ = true;
        }
        cv_.notify_one();
        thread_.join();
    }

    template<typename F>
    auto execute(F&& f) -> std::future<decltype(f())> {
        using ReturnType = decltype(f());
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::forward<F>(f)
        );
        auto future = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push([task]() { (*task)(); });
        }
        cv_.notify_one();

        return future;
    }

private:
    void run() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { 
                    return done_ || !tasks_.empty(); 
                });
                if (done_ && tasks_.empty()) return;
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }

    std::thread thread_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool done_;
};

// 使用
ActiveObject obj;
auto f1 = obj.execute([]{ return 42; });
auto f2 = obj.execute([]{ return std::string("hello"); });
std::cout << f1.get() << ", " << f2.get() << "\n";
```

## Monitor 模式

```cpp
template<typename T>
class Monitor {
public:
    template<typename F>
    auto with_lock(F&& f) -> decltype(f(std::declval<T&>())) {
        std::lock_guard<std::mutex> lock(mutex_);
        return f(data_);
    }

    template<typename F>
    auto with_lock(F&& f) const -> decltype(f(std::declval<const T&>())) {
        std::lock_guard<std::mutex> lock(mutex_);
        return f(data_);
    }

private:
    T data_;
    mutable std::mutex mutex_;
};

// 使用
Monitor<std::vector<int>> vec;

vec.with_lock([](auto& v) { v.push_back(42); });
auto size = vec.with_lock([](const auto& v) { return v.size(); });
```

## Fork-Join 模式

```cpp
template<typename Iterator, typename T, typename BinaryOp>
T parallel_reduce(Iterator first, Iterator last, T init, BinaryOp op) {
    const size_t length = std::distance(first, last);
    const size_t min_per_thread = 1000;
    
    if (length < min_per_thread) {
        return std::accumulate(first, last, init, op);
    }

    Iterator mid = first + length / 2;
    
    auto left_future = std::async(std::launch::async,
        parallel_reduce<Iterator, T, BinaryOp>,
        first, mid, init, op
    );
    
    T right_result = parallel_reduce(mid, last, init, op);
    T left_result = left_future.get();
    
    return op(left_result, right_result);
}

// 使用
std::vector<int> vec(1000000);
std::iota(vec.begin(), vec.end(), 1);

auto sum = parallel_reduce(vec.begin(), vec.end(), 0, std::plus<>{});
```

```{tip}
选择并发模式时考虑：
1. **线程池**：适合大量短任务
2. **生产者-消费者**：适合数据流处理
3. **读写锁**：适合读多写少场景
4. **Active Object**：适合需要串行化的异步调用
5. **Fork-Join**：适合分治算法
```
