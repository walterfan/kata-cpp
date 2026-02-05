# 编译器优化

## 优化级别

### GCC/Clang 优化选项

```bash
-O0  # 无优化（默认），最快编译
-O1  # 基本优化
-O2  # 推荐的优化级别
-O3  # 激进优化，可能增加代码大小
-Os  # 优化代码大小
-Ofast  # -O3 + 不严格遵循标准的优化
```

### 常用编译选项

```bash
# 推荐的生产环境编译选项
g++ -O2 -march=native -flto -DNDEBUG program.cpp

# 调试时保留调试信息
g++ -O2 -g program.cpp

# 性能分析
g++ -O2 -g -fno-omit-frame-pointer program.cpp
```

## 链接时优化 (LTO)

```bash
# GCC
g++ -O2 -flto program.cpp -o program

# Clang (ThinLTO - 更快的增量编译)
clang++ -O2 -flto=thin program.cpp -o program
```

LTO 允许编译器跨编译单元优化，包括：
- 跨文件内联
- 死代码消除
- 全局优化

## 查看编译器优化

### 生成汇编代码

```bash
# 生成汇编
g++ -O2 -S program.cpp

# 带源码注释
g++ -O2 -S -fverbose-asm program.cpp

# 使用 Compiler Explorer (godbolt.org)
```

### 编译器报告

```bash
# GCC 优化报告
g++ -O2 -fopt-info-vec -fopt-info-inline program.cpp

# Clang 优化报告
clang++ -O2 -Rpass=inline -Rpass-missed=inline program.cpp
```

## 帮助编译器优化

### restrict 指针

```cpp
// 告诉编译器指针不会别名
void add_arrays(float* __restrict a, 
                float* __restrict b,
                float* __restrict result, 
                int n) {
    for (int i = 0; i < n; ++i) {
        result[i] = a[i] + b[i];
    }
}
```

### 循环优化提示

```cpp
// OpenMP SIMD
#pragma omp simd
for (int i = 0; i < n; ++i) {
    result[i] = a[i] + b[i];
}

// GCC 循环展开
#pragma GCC unroll 4
for (int i = 0; i < n; ++i) {
    process(i);
}

// 告诉编译器循环至少执行多少次
void process(int* data, int n) {
    __builtin_assume(n >= 1000);
    for (int i = 0; i < n; ++i) {
        // 编译器可以更积极优化
    }
}
```

### [[likely]] 和 [[unlikely]] (C++20)

```cpp
if (value > 0) [[likely]] {
    process(value);
} else [[unlikely]] {
    handle_error();
}

// 或者 GCC 内置函数
if (__builtin_expect(value > 0, 1)) {
    process(value);
}
```

### 内联提示

```cpp
// 建议内联
inline int fast_function(int x) { return x * 2; }

// 强制内联 (GCC/Clang)
[[gnu::always_inline]] inline int very_fast(int x) { return x * 2; }

// 禁止内联
[[gnu::noinline]] void debug_function() { }
```

## 目标架构优化

```bash
# 为当前 CPU 优化
g++ -O2 -march=native program.cpp

# 指定架构
g++ -O2 -march=skylake program.cpp

# 启用 AVX2
g++ -O2 -mavx2 program.cpp

# 查看支持的架构
gcc --target-help
```

## Profile-Guided Optimization (PGO)

```bash
# 第一步：生成插桩程序
g++ -O2 -fprofile-generate program.cpp -o program

# 第二步：运行程序收集数据
./program  # 使用典型工作负载

# 第三步：使用收集的数据重新编译
g++ -O2 -fprofile-use program.cpp -o program_optimized
```

## 常见优化

### 死代码消除

```cpp
void example() {
    int x = 10;
    int y = 20;
    int z = x + y;  // 如果 z 从未使用，会被消除
    
    if (false) {
        // 这段代码会被消除
    }
}
```

### 常量传播

```cpp
int calculate() {
    int a = 5;
    int b = 10;
    return a * b;  // 编译器直接计算为 50
}
```

### 循环不变量外提

```cpp
// 优化前
for (int i = 0; i < n; ++i) {
    result[i] = data[i] * expensive_constant();  // 每次调用
}

// 优化后（编译器自动）
auto c = expensive_constant();
for (int i = 0; i < n; ++i) {
    result[i] = data[i] * c;
}
```

### 循环展开

```cpp
// 优化前
for (int i = 0; i < 4; ++i) {
    data[i] = i;
}

// 优化后
data[0] = 0;
data[1] = 1;
data[2] = 2;
data[3] = 3;
```

### 向量化

```cpp
// 编译器可能将此向量化
void add(float* a, float* b, float* c, int n) {
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}

// 查看向量化报告
// g++ -O2 -fopt-info-vec program.cpp
```

## 编译时计算

```cpp
// constexpr 在编译时计算
constexpr int factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}

// 编译时已知结果
constexpr int result = factorial(10);  // 3628800

// C++20 consteval 强制编译时计算
consteval int must_be_compile_time(int n) {
    return n * 2;
}
```

```{note}
编译器优化要点：
1. 使用 -O2 作为基准优化级别
2. 考虑 LTO 进行跨模块优化
3. 为特定架构使用 -march=native
4. 用 PGO 获得更好的分支预测
5. 使用 Compiler Explorer 验证优化
```
