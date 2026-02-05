# 协程 (C++20)

## 协程基础概念

协程是可以暂停和恢复执行的函数。C++20 提供了协程的底层支持。

### 协程关键字

- `co_await` - 暂停并等待异步操作完成
- `co_yield` - 暂停并产生一个值
- `co_return` - 完成协程并返回值

```cpp
#include <coroutine>

// 一个简单的生成器
generator<int> iota(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;  // 产生值并暂停
    }
}

// 一个异步任务
task<int> async_compute() {
    int result = co_await async_read();  // 暂停等待
    co_return result * 2;
}
```

## 实现简单的 Generator

```cpp
#include <coroutine>
#include <optional>

template<typename T>
class generator {
public:
    struct promise_type {
        T current_value;
        
        generator get_return_object() {
            return generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
        
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        std::suspend_always yield_value(T value) {
            current_value = std::move(value);
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    generator(handle_type h) : handle_(h) {}
    generator(generator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    ~generator() {
        if (handle_) handle_.destroy();
    }

    // 迭代器支持
    struct iterator {
        handle_type handle;
        
        iterator& operator++() {
            handle.resume();
            return *this;
        }
        
        T& operator*() {
            return handle.promise().current_value;
        }
        
        bool operator==(std::default_sentinel_t) const {
            return handle.done();
        }
    };

    iterator begin() {
        handle_.resume();
        return {handle_};
    }
    
    std::default_sentinel_t end() { return {}; }

private:
    handle_type handle_;
};

// 使用示例
generator<int> range(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

int main() {
    for (int n : range(1, 10)) {
        std::cout << n << " ";  // 1 2 3 4 5 6 7 8 9
    }
}
```

## 实现简单的 Task

```cpp
#include <coroutine>

template<typename T>
class task {
public:
    struct promise_type {
        T result;
        std::exception_ptr exception;
        
        task get_return_object() {
            return task{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
        
        std::suspend_never initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T value) {
            result = std::move(value);
        }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    task(handle_type h) : handle_(h) {}
    task(task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    ~task() {
        if (handle_) handle_.destroy();
    }

    T get() {
        if (handle_.promise().exception) {
            std::rethrow_exception(handle_.promise().exception);
        }
        return std::move(handle_.promise().result);
    }

    // co_await 支持
    bool await_ready() const noexcept { return handle_.done(); }
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    T await_resume() { return get(); }

private:
    handle_type handle_;
};

// 使用示例
task<int> compute_async() {
    co_return 42;
}

task<int> main_task() {
    int result = co_await compute_async();
    co_return result * 2;
}
```

## 自定义 Awaitable

```cpp
struct timer_awaitable {
    std::chrono::milliseconds duration;
    
    bool await_ready() const noexcept { return false; }
    
    void await_suspend(std::coroutine_handle<> handle) const {
        // 在另一个线程中等待并恢复
        std::thread([=] {
            std::this_thread::sleep_for(duration);
            handle.resume();
        }).detach();
    }
    
    void await_resume() const noexcept {}
};

auto sleep_for(std::chrono::milliseconds ms) {
    return timer_awaitable{ms};
}

task<void> delayed_task() {
    std::cout << "Starting...\n";
    co_await sleep_for(std::chrono::seconds(1));
    std::cout << "Done!\n";
}
```

## 协程与异步 I/O

```cpp
// 概念性示例：与异步 I/O 集成
class async_read_awaitable {
    int fd;
    char* buffer;
    size_t size;
    
public:
    async_read_awaitable(int fd, char* buf, size_t sz)
        : fd(fd), buffer(buf), size(sz) {}
    
    bool await_ready() const noexcept { return false; }
    
    void await_suspend(std::coroutine_handle<> handle) {
        // 注册到事件循环（如 epoll、io_uring）
        // 当读取完成时调用 handle.resume()
    }
    
    ssize_t await_resume() {
        // 返回读取的字节数
        return bytes_read;
    }
};

task<std::string> async_read_file(const std::string& path) {
    int fd = open(path.c_str(), O_RDONLY);
    std::string content;
    char buffer[4096];
    
    while (true) {
        ssize_t n = co_await async_read_awaitable(fd, buffer, sizeof(buffer));
        if (n <= 0) break;
        content.append(buffer, n);
    }
    
    close(fd);
    co_return content;
}
```

## 协程使用库

由于 C++20 只提供了协程的底层机制，推荐使用成熟的协程库：

### cppcoro

```cpp
#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>

cppcoro::task<int> compute_a() { co_return 1; }
cppcoro::task<int> compute_b() { co_return 2; }

cppcoro::task<int> main_task() {
    auto [a, b] = co_await cppcoro::when_all(
        compute_a(),
        compute_b()
    );
    co_return a + b;
}

int main() {
    int result = cppcoro::sync_wait(main_task());
}
```

### folly coro

```cpp
#include <folly/experimental/coro/Task.h>
#include <folly/experimental/coro/BlockingWait.h>

folly::coro::Task<int> async_compute() {
    co_return 42;
}

int main() {
    int result = folly::coro::blockingWait(async_compute());
}
```

## 协程最佳实践

### 1. 避免在协程中使用引用参数

```cpp
// 危险：引用可能在协程恢复前失效
task<void> dangerous(const std::string& s) {
    co_await some_async_op();
    std::cout << s;  // s 可能已无效
}

// 安全：按值传递
task<void> safe(std::string s) {
    co_await some_async_op();
    std::cout << s;  // OK
}
```

### 2. 小心生命周期

```cpp
// 危险：lambda 捕获悬空
void dangerous() {
    int value = 42;
    auto coro = [&]() -> task<void> {
        co_await some_async_op();
        std::cout << value;  // value 可能已销毁
    }();
}

// 安全：值捕获或确保生命周期
void safe() {
    int value = 42;
    auto coro = [value]() -> task<void> {
        co_await some_async_op();
        std::cout << value;  // OK
    }();
}
```

### 3. 使用 RAII 管理协程句柄

```cpp
class coroutine_owner {
    std::coroutine_handle<> handle_;
public:
    explicit coroutine_owner(std::coroutine_handle<> h) : handle_(h) {}
    ~coroutine_owner() {
        if (handle_) handle_.destroy();
    }
    // 移动语义...
};
```

```{note}
C++23 引入了 `std::generator`，提供了标准的生成器实现。
协程仍在快速发展中，建议关注最新的标准和库。
```
