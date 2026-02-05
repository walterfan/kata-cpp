# RAII 原则

## 什么是 RAII

RAII (Resource Acquisition Is Initialization) 是 C++ 最重要的编程范式之一：
- **获取资源时初始化对象**
- **释放资源时销毁对象**

```cpp
// 经典 RAII 示例：文件句柄
class File {
    FILE* handle_;
public:
    File(const char* path, const char* mode) 
        : handle_(fopen(path, mode)) {
        if (!handle_) throw std::runtime_error("Cannot open file");
    }
    
    ~File() {
        if (handle_) fclose(handle_);
    }
    
    // 禁止拷贝
    File(const File&) = delete;
    File& operator=(const File&) = delete;
    
    // 允许移动
    File(File&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    
    FILE* get() const { return handle_; }
};

void process_file() {
    File f("data.txt", "r");  // 获取资源
    // 使用文件...
}  // 自动释放，即使发生异常
```

## RAII 的优势

### 1. 异常安全

```cpp
// 没有 RAII：需要手动清理
void dangerous() {
    int* data = new int[100];
    if (error_condition) {
        delete[] data;  // 容易遗忘
        return;
    }
    process(data);
    delete[] data;
}

// 使用 RAII：自动清理
void safe() {
    auto data = std::make_unique<int[]>(100);
    if (error_condition) {
        return;  // 自动释放
    }
    process(data.get());
}  // 自动释放
```

### 2. 代码简洁

```cpp
// 没有 RAII：繁琐的清理代码
void messy() {
    Resource* r1 = acquire_r1();
    if (!r1) return;
    
    Resource* r2 = acquire_r2();
    if (!r2) {
        release_r1(r1);
        return;
    }
    
    Resource* r3 = acquire_r3();
    if (!r3) {
        release_r2(r2);
        release_r1(r1);
        return;
    }
    
    // 使用资源...
    
    release_r3(r3);
    release_r2(r2);
    release_r1(r1);
}

// 使用 RAII：简洁清晰
void clean() {
    auto r1 = RAII_Resource1();
    auto r2 = RAII_Resource2();
    auto r3 = RAII_Resource3();
    // 使用资源...
}  // 自动按逆序释放
```

## 常见 RAII 类

### 标准库 RAII 类

```cpp
// 智能指针
std::unique_ptr<T>
std::shared_ptr<T>

// 文件流
std::ifstream, std::ofstream, std::fstream

// 锁
std::lock_guard<std::mutex>
std::unique_lock<std::mutex>
std::scoped_lock<>

// 其他
std::string      // 管理字符数组
std::vector<T>   // 管理动态数组
```

### 自定义 RAII 类

```cpp
// 互斥锁守卫
class MutexGuard {
    std::mutex& mutex_;
public:
    explicit MutexGuard(std::mutex& m) : mutex_(m) {
        mutex_.lock();
    }
    ~MutexGuard() {
        mutex_.unlock();
    }
    MutexGuard(const MutexGuard&) = delete;
    MutexGuard& operator=(const MutexGuard&) = delete;
};

// 数据库事务
class Transaction {
    Database& db_;
    bool committed_ = false;
public:
    explicit Transaction(Database& db) : db_(db) {
        db_.begin_transaction();
    }
    ~Transaction() {
        if (!committed_) {
            db_.rollback();  // 未提交则回滚
        }
    }
    void commit() {
        db_.commit();
        committed_ = true;
    }
};

void update_data(Database& db) {
    Transaction txn(db);
    db.execute("UPDATE ...");
    db.execute("INSERT ...");
    txn.commit();  // 成功则提交
}  // 异常时自动回滚
```

## Scope Guard

通用的 RAII 辅助类，在作用域结束时执行指定操作。

```cpp
template<typename F>
class ScopeGuard {
    F func_;
    bool active_;
public:
    explicit ScopeGuard(F f) : func_(std::move(f)), active_(true) {}
    
    ~ScopeGuard() {
        if (active_) func_();
    }
    
    void dismiss() { active_ = false; }
    
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
    
    ScopeGuard(ScopeGuard&& other) noexcept
        : func_(std::move(other.func_)), active_(other.active_) {
        other.active_ = false;
    }
};

template<typename F>
ScopeGuard<F> make_scope_guard(F f) {
    return ScopeGuard<F>(std::move(f));
}

// 使用
void example() {
    auto* resource = acquire_resource();
    auto guard = make_scope_guard([&]{ release_resource(resource); });
    
    // 使用资源...
    
    if (success) {
        guard.dismiss();  // 成功时不释放
    }
}  // 失败时自动释放
```

### C++20 defer 宏风格

```cpp
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define defer ScopeGuard CONCAT(_defer_, __LINE__) = [&]()

void example() {
    FILE* f = fopen("file.txt", "r");
    defer { fclose(f); };
    
    // 使用文件...
}  // 自动关闭
```

## RAII 与异常

### 异常安全等级

```cpp
// 基本保证：发生异常后，对象仍处于有效状态
class BasicGuarantee {
    std::vector<int> data_;
public:
    void add(int x) {
        data_.push_back(x);  // 可能抛异常
        // 但 data_ 仍然有效
    }
};

// 强保证：发生异常后，状态回滚到操作前
class StrongGuarantee {
    std::vector<int> data_;
public:
    void replace(const std::vector<int>& new_data) {
        auto copy = new_data;  // 可能抛异常
        std::swap(data_, copy);  // noexcept
    }
};

// 不抛保证：操作永不抛异常
class NoThrowGuarantee {
    int value_;
public:
    int get() const noexcept { return value_; }
    void set(int v) noexcept { value_ = v; }
};
```

### 析构函数不应抛异常

```cpp
class Bad {
public:
    ~Bad() {
        throw std::runtime_error("error");  // 非常危险！
    }
};

class Good {
public:
    ~Good() noexcept {
        try {
            cleanup();
        } catch (...) {
            // 记录日志但不传播
        }
    }
};
```

## RAII 最佳实践

### 1. 构造时获取，析构时释放

```cpp
class Resource {
    void* handle_;
public:
    Resource() : handle_(acquire()) {
        if (!handle_) throw std::runtime_error("Failed to acquire");
    }
    ~Resource() {
        if (handle_) release(handle_);
    }
};
```

### 2. 遵循 Rule of Zero/Five

```cpp
// Rule of Zero：优先使用现有 RAII 类
class ModernClass {
    std::string name_;
    std::unique_ptr<Resource> resource_;
    // 不需要自定义析构函数
};

// Rule of Five：如果需要自定义析构，考虑所有五个
class CustomClass {
public:
    CustomClass();
    ~CustomClass();
    CustomClass(const CustomClass&);
    CustomClass& operator=(const CustomClass&);
    CustomClass(CustomClass&&) noexcept;
    CustomClass& operator=(CustomClass&&) noexcept;
};
```

### 3. 使用 noexcept

```cpp
class SafeClass {
public:
    ~SafeClass() noexcept;  // 析构函数应该是 noexcept
    SafeClass(SafeClass&&) noexcept;  // 移动操作应该是 noexcept
    SafeClass& operator=(SafeClass&&) noexcept;
};
```

```{tip}
RAII 核心思想：
- 让编译器帮你管理资源生命周期
- 构造时获取，析构时释放
- 析构函数不抛异常
- 优先使用标准库的 RAII 类
```
