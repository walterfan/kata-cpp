# 性能分析工具

## 基本原则

1. **先测量，后优化**：不要凭直觉优化
2. **关注热点**：80% 的时间花在 20% 的代码上
3. **理解瓶颈**：CPU、内存、I/O、网络

## perf (Linux)

### 基本用法

```bash
# CPU 采样分析
perf record -g ./your_program
perf report

# 统计事件
perf stat ./your_program

# 实时查看
perf top -p <pid>
```

### 火焰图

```bash
# 记录
perf record -F 99 -g ./your_program

# 生成火焰图（需要 FlameGraph 工具）
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg
```

### 常用事件

```bash
# CPU 周期
perf stat -e cycles,instructions ./program

# 缓存
perf stat -e cache-misses,cache-references ./program

# 分支预测
perf stat -e branch-misses,branches ./program

# 内存
perf stat -e LLC-load-misses,LLC-loads ./program
```

## gprof

### 使用步骤

```bash
# 1. 编译时加入分析选项
g++ -pg -g program.cpp -o program

# 2. 运行程序
./program

# 3. 生成分析报告
gprof program gmon.out > analysis.txt
```

### 输出解读

```
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  ms/call  ms/call  name    
 30.00      0.30     0.30     1000     0.30     0.30  hot_function
 20.00      0.50     0.20     5000     0.04     0.04  another_function
```

## Valgrind (callgrind)

```bash
# 运行分析
valgrind --tool=callgrind ./your_program

# 查看结果
callgrind_annotate callgrind.out.<pid>

# 可视化（使用 KCachegrind）
kcachegrind callgrind.out.<pid>
```

## Google Performance Tools (gperftools)

### CPU Profiler

```cpp
#include <gperftools/profiler.h>

int main() {
    ProfilerStart("profile.out");
    
    // 你的代码
    
    ProfilerStop();
}
```

```bash
# 编译
g++ -lprofiler program.cpp -o program

# 运行
./program

# 分析
pprof --text ./program profile.out
pprof --web ./program profile.out  # 在浏览器中查看
```

### Heap Profiler

```cpp
#include <gperftools/heap-profiler.h>

int main() {
    HeapProfilerStart("heap_profile");
    
    // 你的代码
    
    HeapProfilerStop();
}
```

## Intel VTune

```bash
# 收集数据
vtune -collect hotspots ./your_program

# 分析
vtune-gui
```

## 代码级分析

### 手动计时

```cpp
#include <chrono>

class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    ~Timer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end - start_
        ).count();
        std::cout << "Elapsed: " << duration << " us\n";
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

void function_to_measure() {
    Timer timer;  // 自动计时
    // 代码
}
```

### RAII 计时器

```cpp
template<typename Func>
auto measure_time(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    
    if constexpr (std::is_void_v<std::invoke_result_t<Func>>) {
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    } else {
        auto result = func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::make_pair(
            result,
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
        );
    }
}

// 使用
auto duration = measure_time([]{ expensive_function(); });
std::cout << "Time: " << duration.count() << " ns\n";
```

## 内存分析

### Valgrind Massif

```bash
# 运行
valgrind --tool=massif ./your_program

# 查看
ms_print massif.out.<pid>
```

### AddressSanitizer

```bash
# 编译
g++ -fsanitize=address -g program.cpp

# 运行（自动检测内存问题）
./a.out
```

### LeakSanitizer

```bash
# 通常与 ASan 一起使用
g++ -fsanitize=address -g program.cpp

# 设置环境变量获取更多信息
ASAN_OPTIONS=detect_leaks=1 ./a.out
```

## 分析最佳实践

### 1. 选择正确的构建配置

```bash
# Release 构建 + 调试符号
g++ -O2 -g program.cpp

# 对于 perf/gprof，避免帧指针优化
g++ -O2 -g -fno-omit-frame-pointer program.cpp
```

### 2. 热身运行

```cpp
// 第一次运行可能因为冷缓存而较慢
for (int i = 0; i < 3; ++i) {
    run_function();  // 热身
}

auto start = std::chrono::high_resolution_clock::now();
for (int i = 0; i < 1000; ++i) {
    run_function();  // 测量
}
auto end = std::chrono::high_resolution_clock::now();
```

### 3. 避免测量中的优化

```cpp
// 编译器可能优化掉无副作用的代码
volatile int result = 0;
for (int i = 0; i < 1000; ++i) {
    result = compute();  // volatile 防止优化
}

// 或使用 benchmark 库的 DoNotOptimize
```

```{tip}
工具选择指南：
- **快速 CPU 分析**：perf
- **详细调用图**：callgrind + KCachegrind
- **内存问题**：AddressSanitizer
- **生产环境**：gperftools（低开销）
- **Windows**：Visual Studio Profiler 或 VTune
```
