# 优化技巧

## 算法优化

### 选择正确的数据结构

```cpp
// 需要频繁查找：使用哈希表
std::unordered_map<int, int> fast_lookup;  // O(1) 平均

// 需要有序遍历：使用红黑树
std::map<int, int> ordered;  // O(log n)

// 需要随机访问：使用 vector
std::vector<int> random_access;  // O(1)

// 需要频繁首尾插入：使用 deque
std::deque<int> double_ended;  // O(1)
```

### 避免不必要的工作

```cpp
// 差：每次循环都计算 size()
for (size_t i = 0; i < vec.size(); ++i) { }

// 好：缓存 size
const size_t n = vec.size();
for (size_t i = 0; i < n; ++i) { }

// 更好：范围 for（编译器可优化）
for (auto& item : vec) { }
```

## 内存优化

### 避免不必要的拷贝

```cpp
// 差：拷贝字符串
void process(std::string s) { }

// 好：const 引用
void process(const std::string& s) { }

// 更好：string_view (C++17)
void process(std::string_view s) { }

// 移动语义
std::vector<std::string> create_data() {
    std::vector<std::string> result;
    // ...
    return result;  // RVO 或移动
}
```

### 预分配内存

```cpp
std::vector<int> vec;
vec.reserve(10000);  // 预分配，避免多次重新分配

for (int i = 0; i < 10000; ++i) {
    vec.push_back(i);  // 不会触发重新分配
}
```

### 使用 emplace

```cpp
std::vector<std::pair<int, std::string>> vec;

// 差：创建临时对象再拷贝/移动
vec.push_back(std::make_pair(1, "hello"));

// 好：原地构造
vec.emplace_back(1, "hello");
```

### 内存池

```cpp
// 简单的对象池
template<typename T, size_t BlockSize = 4096>
class ObjectPool {
public:
    T* allocate() {
        if (free_list_) {
            T* result = free_list_;
            free_list_ = *reinterpret_cast<T**>(free_list_);
            return result;
        }
        if (current_block_ >= BlockSize) {
            blocks_.push_back(std::make_unique<T[]>(BlockSize));
            current_block_ = 0;
        }
        return &blocks_.back()[current_block_++];
    }

    void deallocate(T* ptr) {
        *reinterpret_cast<T**>(ptr) = free_list_;
        free_list_ = ptr;
    }

private:
    std::vector<std::unique_ptr<T[]>> blocks_;
    size_t current_block_ = BlockSize;
    T* free_list_ = nullptr;
};
```

## CPU 优化

### 分支预测

```cpp
// 差：难以预测的分支
if (random() % 2) {
    do_something();
} else {
    do_other();
}

// 好：使用 [[likely]]/[[unlikely]] (C++20)
if (condition) [[likely]] {
    common_case();
} else [[unlikely]] {
    rare_case();
}

// 好：使用查找表代替条件
int lookup_table[] = {value_for_0, value_for_1, value_for_2, ...};
result = lookup_table[index];
```

### 循环展开

```cpp
// 手动展开
for (int i = 0; i < n; i += 4) {
    process(data[i]);
    process(data[i+1]);
    process(data[i+2]);
    process(data[i+3]);
}

// 或让编译器做（-funroll-loops）
#pragma unroll 4
for (int i = 0; i < n; ++i) {
    process(data[i]);
}
```

### SIMD 向量化

```cpp
#include <immintrin.h>

// AVX2 示例：同时处理 8 个 float
void add_arrays_simd(float* a, float* b, float* result, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
    // 处理剩余元素
    for (; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}

// 或使用自动向量化（编译器优化）
// g++ -O3 -march=native -ftree-vectorize
void add_arrays(float* __restrict a, float* __restrict b, 
                float* __restrict result, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        result[i] = a[i] + b[i];  // 编译器会向量化
    }
}
```

## 缓存优化

### 数据局部性

```cpp
// 差：列优先访问（跳跃访问）
for (int j = 0; j < cols; ++j) {
    for (int i = 0; i < rows; ++i) {
        process(matrix[i][j]);  // 缓存不友好
    }
}

// 好：行优先访问（顺序访问）
for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
        process(matrix[i][j]);  // 缓存友好
    }
}
```

### 结构体布局

```cpp
// 差：大量填充
struct Bad {
    char a;     // 1 byte
    // 7 bytes padding
    double b;   // 8 bytes
    char c;     // 1 byte
    // 7 bytes padding
    double d;   // 8 bytes
};  // 32 bytes

// 好：紧凑布局
struct Good {
    double b;   // 8 bytes
    double d;   // 8 bytes
    char a;     // 1 byte
    char c;     // 1 byte
    // 6 bytes padding
};  // 24 bytes
```

### 避免伪共享

```cpp
// 差：不同线程修改同一缓存行的不同变量
struct Counters {
    int counter1;  // 可能在同一缓存行
    int counter2;
};

// 好：缓存行对齐
struct alignas(64) Counter {
    int value;
};

struct AlignedCounters {
    Counter counter1;
    Counter counter2;
};
```

## 编译器优化提示

### restrict 指针

```cpp
// 告诉编译器指针不会别名
void copy(int* __restrict dest, const int* __restrict src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dest[i] = src[i];
    }
}
```

### 内联提示

```cpp
// 建议内联
inline int fast_function(int x) { return x * 2; }

// 强制内联（编译器特定）
__attribute__((always_inline)) int very_fast(int x) { return x * 2; }

// 禁止内联
__attribute__((noinline)) void debug_function() { }
```

### constexpr 编译期计算

```cpp
// 在编译期计算
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

constexpr int result = factorial(10);  // 编译时计算
```

## 常见反模式

```cpp
// 反模式1：过早优化
// 不要优化未经测量的代码

// 反模式2：微优化
// 不要优化不在热点的代码

// 反模式3：破坏可读性
// 优化不应使代码难以维护

// 反模式4：假设而非测量
// 总是用 profiler 验证优化效果
```

```{warning}
优化要点：
1. 先让代码正确，再优化
2. 用 profiler 找到热点
3. 优化算法 > 优化代码 > 优化微指令
4. 测量优化效果
```
