# 异步编程与 Future

## std::async

`std::async` 是最简单的异步编程方式。

```cpp
#include <future>
#include <iostream>

int compute(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return x * x;
}

int main() {
    // 启动异步任务
    std::future<int> future = std::async(std::launch::async, compute, 42);
    
    // 做其他工作...
    std::cout << "Computing...\n";
    
    // 获取结果（阻塞）
    int result = future.get();
    std::cout << "Result: " << result << "\n";
}
```

## 启动策略

```cpp
// std::launch::async - 在新线程中立即执行
auto f1 = std::async(std::launch::async, task);

// std::launch::deferred - 延迟执行，在 get() 或 wait() 时执行
auto f2 = std::async(std::launch::deferred, task);

// 默认策略 - 由实现决定
auto f3 = std::async(task);  // async | deferred

// 检查是否已准备好
if (f1.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    auto result = f1.get();
}
```

## std::future

### 获取结果

```cpp
std::future<int> f = std::async(compute, 42);

// get() - 阻塞等待结果
int result = f.get();  // 只能调用一次！

// wait() - 只等待，不获取结果
f.wait();

// wait_for() - 带超时等待
auto status = f.wait_for(std::chrono::seconds(1));
switch (status) {
    case std::future_status::ready:
        // 任务完成
        break;
    case std::future_status::timeout:
        // 超时
        break;
    case std::future_status::deferred:
        // 任务被延迟（deferred launch）
        break;
}
```

### 异常传播

```cpp
int risky_task() {
    throw std::runtime_error("Something went wrong");
}

auto future = std::async(risky_task);
try {
    int result = future.get();  // 异常会在这里重新抛出
} catch (const std::exception& e) {
    std::cerr << "Caught: " << e.what() << "\n";
}
```

## std::promise

`std::promise` 可以手动设置 future 的值。

```cpp
void worker(std::promise<int> promise) {
    try {
        int result = compute();
        promise.set_value(result);
    } catch (...) {
        promise.set_exception(std::current_exception());
    }
}

int main() {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();
    
    std::thread t(worker, std::move(promise));
    
    int result = future.get();
    t.join();
}
```

### 一次性通知

```cpp
std::promise<void> ready_promise;
std::future<void> ready_future = ready_promise.get_future();

void worker() {
    ready_future.wait();  // 等待信号
    // 开始工作
}

int main() {
    std::thread t(worker);
    
    // 准备工作...
    ready_promise.set_value();  // 发送信号
    
    t.join();
}
```

## std::shared_future

`std::shared_future` 可以被多个线程共享。

```cpp
std::promise<int> promise;
std::shared_future<int> sf = promise.get_future().share();

// 多个线程可以等待同一个结果
auto worker = [sf]() {
    int result = sf.get();  // 每个线程都可以调用 get()
    std::cout << "Got: " << result << "\n";
};

std::thread t1(worker);
std::thread t2(worker);
std::thread t3(worker);

promise.set_value(42);

t1.join(); t2.join(); t3.join();
```

## std::packaged_task

`std::packaged_task` 将函数包装为可调用对象，关联一个 future。

```cpp
#include <future>
#include <functional>

int compute(int x, int y) {
    return x + y;
}

int main() {
    // 包装函数
    std::packaged_task<int(int, int)> task(compute);
    
    // 获取关联的 future
    std::future<int> future = task.get_future();
    
    // 在另一个线程执行
    std::thread t(std::move(task), 2, 3);
    
    // 获取结果
    int result = future.get();  // 5
    
    t.join();
}
```

### 任务队列示例

```cpp
class TaskQueue {
    std::queue<std::packaged_task<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool stop_ = false;
    std::thread worker_;

public:
    TaskQueue() : worker_([this] { worker_loop(); }) {}
    
    ~TaskQueue() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cv_.notify_one();
        worker_.join();
    }

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) 
        -> std::future<std::invoke_result_t<F, Args...>> 
    {
        using ReturnType = std::invoke_result_t<F, Args...>;
        
        auto task = std::packaged_task<ReturnType()>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        auto future = task.get_future();
        
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push(std::packaged_task<void()>(std::move(task)));
        }
        cv_.notify_one();
        
        return future;
    }

private:
    void worker_loop() {
        while (true) {
            std::packaged_task<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                
                if (stop_ && tasks_.empty()) break;
                
                task = std::move(tasks_.front());
                tasks_.pop();
            }
            task();
        }
    }
};
```

## 并行算法 (C++17)

```cpp
#include <algorithm>
#include <execution>
#include <vector>

std::vector<int> vec(1000000);

// 顺序执行
std::sort(std::execution::seq, vec.begin(), vec.end());

// 并行执行
std::sort(std::execution::par, vec.begin(), vec.end());

// 并行且向量化
std::sort(std::execution::par_unseq, vec.begin(), vec.end());

// 其他并行算法
std::for_each(std::execution::par, vec.begin(), vec.end(), 
              [](int& x) { x *= 2; });

int sum = std::reduce(std::execution::par, vec.begin(), vec.end(), 0);

std::transform(std::execution::par,
               vec1.begin(), vec1.end(),
               vec2.begin(),
               result.begin(),
               std::plus<>{});
```

## 最佳实践

### 1. 避免阻塞主线程

```cpp
// 不好：阻塞主线程
auto f = std::async(std::launch::async, long_task);
f.get();  // 阻塞

// 好：在需要结果时才等待
auto f = std::async(std::launch::async, long_task);
// 做其他工作...
if (need_result) {
    auto result = f.get();
}
```

### 2. 处理 future 析构

```cpp
// 注意：async 返回的 future 析构时会等待任务完成
void dangerous() {
    std::async(std::launch::async, long_task);
    // future 在这里析构，会阻塞！
}

void better() {
    auto f = std::async(std::launch::async, long_task);
    // 保持 future 存活
}
```

### 3. 使用 when_all / when_any (第三方库)

```cpp
// 标准库没有，但可以自己实现
template<typename... Futures>
auto when_all(Futures&&... futures) {
    return std::make_tuple(futures.get()...);
}

// 或使用第三方库如 folly, boost
```

```{warning}
`std::async` 的默认启动策略可能导致任务永不执行（如果 future 从未被等待）。
推荐明确指定 `std::launch::async` 或 `std::launch::deferred`。
```
