# 内存分配

## 栈 vs 堆

```cpp
void stack_example() {
    int x = 42;              // 栈：自动分配，自动释放
    int arr[100];            // 栈：固定大小数组
    std::array<int, 100> a;  // 栈：std::array
}

void heap_example() {
    int* p = new int(42);    // 堆：手动分配
    delete p;                 // 必须手动释放
    
    int* arr = new int[100]; // 堆：动态数组
    delete[] arr;            // 注意 delete[]
    
    auto vec = std::vector<int>(100);  // 堆：内部使用堆
}
```

### 栈 vs 堆比较

| 特性 | 栈 | 堆 |
|------|----|----|
| 分配速度 | 非常快（移动栈指针） | 较慢（需要查找空闲块） |
| 大小限制 | 有限（通常 1-8 MB） | 几乎无限（受系统限制） |
| 生命周期 | 自动（作用域结束） | 手动（或智能指针） |
| 碎片化 | 无 | 可能 |

## new 和 delete

```cpp
// 单个对象
int* p = new int(42);
delete p;

// 数组
int* arr = new int[100];
delete[] arr;  // 必须用 delete[]

// 带初始化的数组 (C++11)
int* arr2 = new int[5]{1, 2, 3, 4, 5};
delete[] arr2;

// placement new：在指定位置构造
char buffer[sizeof(Widget)];
Widget* w = new(buffer) Widget();  // 在 buffer 中构造
w->~Widget();  // 需要手动调用析构函数
```

### new 的变体

```cpp
// 可能抛 std::bad_alloc
int* p1 = new int;

// 失败时返回 nullptr
int* p2 = new(std::nothrow) int;
if (!p2) { /* 分配失败 */ }

// 对齐分配 (C++17)
struct alignas(64) CacheAligned { int data; };
auto* p3 = new CacheAligned();  // 自动 64 字节对齐
```

## 自定义分配器

### 简单内存池

```cpp
template<typename T, size_t PoolSize = 1024>
class PoolAllocator {
    union Node {
        T data;
        Node* next;
    };
    
    Node pool_[PoolSize];
    Node* free_list_;

public:
    PoolAllocator() {
        // 初始化空闲链表
        for (size_t i = 0; i < PoolSize - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[PoolSize - 1].next = nullptr;
        free_list_ = &pool_[0];
    }

    T* allocate() {
        if (!free_list_) return nullptr;
        Node* node = free_list_;
        free_list_ = free_list_->next;
        return &node->data;
    }

    void deallocate(T* ptr) {
        Node* node = reinterpret_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
    }
};
```

### STL 兼容分配器

```cpp
template<typename T>
class CustomAllocator {
public:
    using value_type = T;
    
    CustomAllocator() noexcept = default;
    
    template<typename U>
    CustomAllocator(const CustomAllocator<U>&) noexcept {}
    
    T* allocate(size_t n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }
    
    void deallocate(T* p, size_t) noexcept {
        ::operator delete(p);
    }
};

// 使用
std::vector<int, CustomAllocator<int>> vec;
```

### C++17 pmr (Polymorphic Memory Resources)

```cpp
#include <memory_resource>

// 使用单调缓冲区
char buffer[10000];
std::pmr::monotonic_buffer_resource mbr(buffer, sizeof(buffer));
std::pmr::vector<int> vec(&mbr);

// 同步池资源
std::pmr::synchronized_pool_resource pool;
std::pmr::vector<std::pmr::string> strings(&pool);

// 不同步池资源（单线程更快）
std::pmr::unsynchronized_pool_resource unsync_pool;
```

## 内存对齐

```cpp
// 检查对齐
std::cout << alignof(int);     // 通常 4
std::cout << alignof(double);  // 通常 8

// 指定对齐
struct alignas(16) Vector4 {
    float x, y, z, w;
};

// C++17 对齐分配
void* p = std::aligned_alloc(64, 1024);  // 64 字节对齐
std::free(p);

// C++17 operator new 支持对齐
struct alignas(64) CacheLine {
    int data[16];
};
auto* cl = new CacheLine();  // 自动 64 字节对齐
delete cl;
```

## 小对象优化 (Small Object Optimization)

```cpp
// std::string 的 SSO
std::string s1 = "hello";  // 小字符串存储在栈上
std::string s2(100, 'x');   // 大字符串使用堆

// 自定义 SSO
template<typename T, size_t N = 16>
class SmallVector {
    union {
        T inline_data_[N];
        T* heap_data_;
    };
    size_t size_ = 0;
    size_t capacity_ = N;
    bool on_heap_ = false;

public:
    T* data() {
        return on_heap_ ? heap_data_ : inline_data_;
    }
    
    void push_back(const T& value) {
        if (size_ >= capacity_) {
            grow();
        }
        data()[size_++] = value;
    }
    
    // ... 其他方法
};
```

## 内存映射文件

```cpp
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

class MappedFile {
    void* data_ = nullptr;
    size_t size_ = 0;
    int fd_ = -1;

public:
    MappedFile(const char* path) {
        fd_ = open(path, O_RDONLY);
        if (fd_ < 0) throw std::runtime_error("Cannot open file");
        
        // 获取文件大小
        struct stat sb;
        fstat(fd_, &sb);
        size_ = sb.st_size;
        
        // 映射
        data_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd_, 0);
        if (data_ == MAP_FAILED) {
            close(fd_);
            throw std::runtime_error("mmap failed");
        }
    }
    
    ~MappedFile() {
        if (data_) munmap(data_, size_);
        if (fd_ >= 0) close(fd_);
    }
    
    const void* data() const { return data_; }
    size_t size() const { return size_; }
};
```

## 内存分配策略

### Arena 分配器

```cpp
class Arena {
    std::vector<char*> blocks_;
    char* current_;
    size_t remaining_;
    static constexpr size_t kBlockSize = 4096;

public:
    Arena() : current_(nullptr), remaining_(0) {}
    
    ~Arena() {
        for (char* block : blocks_) {
            delete[] block;
        }
    }

    void* allocate(size_t size) {
        // 对齐
        size = (size + 7) & ~7;
        
        if (remaining_ < size) {
            // 分配新块
            size_t block_size = std::max(size, kBlockSize);
            current_ = new char[block_size];
            blocks_.push_back(current_);
            remaining_ = block_size;
        }
        
        void* result = current_;
        current_ += size;
        remaining_ -= size;
        return result;
    }
    
    // 不支持单独释放，统一释放
};
```

```{warning}
内存分配注意事项：
1. 栈上分配最快，优先使用
2. 避免频繁的小块分配（考虑内存池）
3. 注意对齐要求
4. 使用智能指针避免泄漏
5. 大块内存考虑内存映射
```
