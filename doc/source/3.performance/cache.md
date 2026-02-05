# 缓存优化

## CPU 缓存基础

### 缓存层次

```
CPU Register (< 1ns)
    ↓
L1 Cache (1-3ns, 32-64 KB)
    ↓
L2 Cache (3-10ns, 256KB-1MB)
    ↓
L3 Cache (10-30ns, 4-50MB)
    ↓
Main Memory (50-100ns)
    ↓
SSD/HDD (10,000+ ns)
```

### 缓存行 (Cache Line)

```cpp
// 典型缓存行大小：64 字节
// 访问一个字节会加载整个缓存行

struct Data {
    int a;  // 4 bytes
    int b;  // 4 bytes
    int c;  // 4 bytes
    int d;  // 4 bytes
};  // 16 bytes，适合一个缓存行

// 访问 a 会同时加载 b, c, d 到缓存
```

## 数据局部性

### 空间局部性

```cpp
// 好：顺序访问
int sum = 0;
for (int i = 0; i < n; ++i) {
    sum += data[i];  // 连续内存访问
}

// 差：跳跃访问
int sum = 0;
for (int i = 0; i < n; i += 64) {  // 每 64 字节访问一次
    sum += data[i];  // 缓存利用率低
}
```

### 时间局部性

```cpp
// 好：重复使用最近访问的数据
for (int i = 0; i < 1000; ++i) {
    result += process(hot_data);  // hot_data 保持在缓存中
}

// 差：访问大量不同数据
for (int i = 0; i < 1000000; ++i) {
    result += cold_data[i];  // 不断换出缓存
}
```

## 矩阵访问优化

### 行优先 vs 列优先

```cpp
const int N = 1000;
int matrix[N][N];

// 差：列优先（跳跃访问）- ~10x 慢
for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
        process(matrix[i][j]);  // 每次跳 N*sizeof(int) 字节
    }
}

// 好：行优先（顺序访问）
for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
        process(matrix[i][j]);  // 顺序访问
    }
}
```

### 分块优化 (Tiling)

```cpp
// 矩阵乘法：C = A * B
const int N = 1000;
const int BLOCK = 32;  // 适合 L1 缓存的块大小

// 差：朴素实现
for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
        for (int k = 0; k < N; ++k) {
            C[i][j] += A[i][k] * B[k][j];
        }
    }
}

// 好：分块实现
for (int i = 0; i < N; i += BLOCK) {
    for (int j = 0; j < N; j += BLOCK) {
        for (int k = 0; k < N; k += BLOCK) {
            // 处理一个小块
            for (int ii = i; ii < std::min(i + BLOCK, N); ++ii) {
                for (int jj = j; jj < std::min(j + BLOCK, N); ++jj) {
                    for (int kk = k; kk < std::min(k + BLOCK, N); ++kk) {
                        C[ii][jj] += A[ii][kk] * B[kk][jj];
                    }
                }
            }
        }
    }
}
```

## 数据结构优化

### Array of Structures vs Structure of Arrays

```cpp
// AoS (Array of Structures) - 面向对象风格
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float mass;
};
std::vector<Particle> particles;

// 如果只需要更新位置，需要加载不需要的数据
for (auto& p : particles) {
    p.x += p.vx;  // 也会加载 y, z, vy, vz, mass
}

// SoA (Structure of Arrays) - 数据导向风格
struct ParticleSystem {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;
};

// 更好的缓存利用
for (size_t i = 0; i < n; ++i) {
    system.x[i] += system.vx[i];  // 只加载需要的数据
}
```

### 热冷数据分离

```cpp
// 差：热冷数据混合
struct Record {
    int id;          // 热数据，经常访问
    int count;       // 热数据
    std::string name;        // 冷数据
    std::string description; // 冷数据
    std::vector<int> tags;   // 冷数据
};

// 好：分离热冷数据
struct HotData {
    int id;
    int count;
};

struct ColdData {
    std::string name;
    std::string description;
    std::vector<int> tags;
};

struct Record {
    HotData hot;
    ColdData* cold;  // 指向冷数据
};
```

## 伪共享 (False Sharing)

### 问题

```cpp
// 两个线程修改同一缓存行的不同变量
struct Counters {
    int counter1;  // 线程1 修改
    int counter2;  // 线程2 修改
    // 在同一缓存行，导致缓存行反复失效
};
```

### 解决方案

```cpp
// 方案1：缓存行填充
struct Counters {
    alignas(64) int counter1;
    alignas(64) int counter2;
};

// 方案2：使用 hardware_destructive_interference_size (C++17)
struct Counters {
    alignas(std::hardware_destructive_interference_size) int counter1;
    alignas(std::hardware_destructive_interference_size) int counter2;
};

// 方案3：包装类
template<typename T>
struct CacheAligned {
    alignas(64) T value;
};

CacheAligned<int> counter1;
CacheAligned<int> counter2;
```

## 预取 (Prefetch)

```cpp
// 手动预取
#include <xmmintrin.h>

void process_array(int* data, size_t n) {
    for (size_t i = 0; i < n; i += 16) {
        // 预取后面的数据
        _mm_prefetch(&data[i + 64], _MM_HINT_T0);
        
        // 处理当前数据
        for (size_t j = i; j < i + 16 && j < n; ++j) {
            process(data[j]);
        }
    }
}

// 预取提示
// _MM_HINT_T0: 预取到所有缓存级别
// _MM_HINT_T1: 预取到 L2 及以上
// _MM_HINT_T2: 预取到 L3 及以上
// _MM_HINT_NTA: 非临时数据，可能绕过缓存
```

## 缓存友好的数据结构

### 缓存友好的链表

```cpp
// 传统链表：每个节点可能在不同缓存行
struct Node {
    int data;
    Node* next;
};

// 缓存友好版本：预分配块
template<typename T, size_t BlockSize = 64>
class CacheFriendlyList {
    struct Block {
        T data[BlockSize];
        Block* next;
        size_t used;
    };
    Block* head;
};
```

### B+ 树（数据库常用）

```cpp
// B+ 树节点设计为缓存行大小的整数倍
// 每个节点包含多个键，提高缓存命中率
template<typename K, typename V, int ORDER = 16>  // ORDER 选择使节点适合缓存
struct BPlusNode {
    K keys[ORDER - 1];
    union {
        BPlusNode* children[ORDER];
        V* values[ORDER];
    };
    int count;
    bool is_leaf;
};
```

## 测量缓存性能

```bash
# perf 测量缓存事件
perf stat -e cache-misses,cache-references,L1-dcache-load-misses ./program

# Valgrind cachegrind
valgrind --tool=cachegrind ./program
cg_annotate cachegrind.out.<pid>
```

```{tip}
缓存优化总结：
1. 顺序访问优于随机访问
2. 小数据结构优于大数据结构
3. 热冷数据分离
4. 避免伪共享
5. 考虑使用 SoA 布局
6. 使用分块处理大数据
```
