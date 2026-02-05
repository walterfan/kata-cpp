# 基准测试

## Google Benchmark

### 安装

```bash
# Ubuntu
sudo apt install libbenchmark-dev

# 或从源码编译
git clone https://github.com/google/benchmark.git
cd benchmark
cmake -E make_directory build
cmake -E chdir build cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build build --config Release
sudo cmake --install build
```

### 基本用法

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string s("hello");
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StringCreation);

static void BM_StringCopy(benchmark::State& state) {
    std::string x = "hello";
    for (auto _ : state) {
        std::string copy(x);
        benchmark::DoNotOptimize(copy);
    }
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN();
```

```bash
# 编译
g++ -O2 benchmark.cpp -lbenchmark -lpthread -o benchmark

# 运行
./benchmark
```

### 参数化基准测试

```cpp
static void BM_VectorPushBack(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < state.range(0); ++i) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_VectorPushBack)->Range(8, 8192);

// 或使用 Args
BENCHMARK(BM_VectorPushBack)->Args({100})->Args({1000})->Args({10000});

// 范围乘数
BENCHMARK(BM_VectorPushBack)->RangeMultiplier(2)->Range(8, 8<<10);
```

### 比较不同实现

```cpp
static void BM_SortStd(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();  // 暂停计时
        std::iota(v.begin(), v.end(), 0);
        std::random_shuffle(v.begin(), v.end());
        state.ResumeTiming();  // 恢复计时
        
        std::sort(v.begin(), v.end());
    }
}
BENCHMARK(BM_SortStd)->Range(1000, 100000);

static void BM_SortStable(benchmark::State& state) {
    std::vector<int> v(state.range(0));
    for (auto _ : state) {
        state.PauseTiming();
        std::iota(v.begin(), v.end(), 0);
        std::random_shuffle(v.begin(), v.end());
        state.ResumeTiming();
        
        std::stable_sort(v.begin(), v.end());
    }
}
BENCHMARK(BM_SortStable)->Range(1000, 100000);
```

### 复杂度分析

```cpp
static void BM_Complexity(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v(state.range(0));
        std::sort(v.begin(), v.end());
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Complexity)->Range(8, 8<<10)->Complexity(benchmark::oNLogN);
```

### 自定义计数器

```cpp
static void BM_WithCounters(benchmark::State& state) {
    int items_processed = 0;
    for (auto _ : state) {
        // 工作
        items_processed += 100;
    }
    state.counters["Items"] = items_processed;
    state.counters["ItemsPerSecond"] = 
        benchmark::Counter(items_processed, benchmark::Counter::kIsRate);
}
```

## 手动基准测试

```cpp
#include <chrono>
#include <iostream>

template<typename Func>
void benchmark(const char* name, int iterations, Func func) {
    // 热身
    for (int i = 0; i < 10; ++i) {
        func();
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end - start
    ).count();
    
    std::cout << name << ": " 
              << duration / iterations << " ns/op, "
              << iterations * 1e9 / duration << " ops/sec\n";
}

// 使用
benchmark("vector push_back", 10000, [](){
    std::vector<int> v;
    for (int i = 0; i < 1000; ++i) {
        v.push_back(i);
    }
});
```

## Quick Bench（在线工具）

访问 https://quick-bench.com/ 进行快速基准测试比较。

```cpp
// 在线粘贴代码比较
static void StringConcat(benchmark::State& state) {
    std::string a = "hello";
    std::string b = "world";
    for (auto _ : state) {
        std::string c = a + b;
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(StringConcat);

static void StringAppend(benchmark::State& state) {
    for (auto _ : state) {
        std::string c = "hello";
        c += "world";
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(StringAppend);
```

## 基准测试最佳实践

### 1. 防止编译器优化掉测试代码

```cpp
int result;
for (auto _ : state) {
    result = compute();
    benchmark::DoNotOptimize(result);  // 防止优化
}

// 或使用 ClobberMemory
for (auto _ : state) {
    compute();
    benchmark::ClobberMemory();  // 刷新内存
}
```

### 2. 考虑缓存效应

```cpp
static void BM_CacheWarm(benchmark::State& state) {
    std::vector<int> data(state.range(0));
    
    // 预热缓存
    std::fill(data.begin(), data.end(), 1);
    
    for (auto _ : state) {
        int sum = 0;
        for (int x : data) sum += x;
        benchmark::DoNotOptimize(sum);
    }
}
```

### 3. 多次运行取平均

```bash
# 运行多次取平均
./benchmark --benchmark_repetitions=10
```

### 4. 排除初始化时间

```cpp
static void BM_ExcludeSetup(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        auto data = create_test_data();  // 不计入时间
        state.ResumeTiming();
        
        process(data);  // 只测量这部分
    }
}
```

### 5. 使用真实数据

```cpp
// 使用真实场景的数据，而不是简单的测试数据
static void BM_RealWorld(benchmark::State& state) {
    auto real_data = load_real_dataset();
    
    for (auto _ : state) {
        process(real_data);
    }
}
```

```{tip}
基准测试注意事项：
1. 确保测量的是你关心的操作
2. 考虑缓存、分支预测等因素
3. 在接近生产环境的配置下测试
4. 多次运行，关注方差
5. 比较相对性能，而非绝对数字
```
