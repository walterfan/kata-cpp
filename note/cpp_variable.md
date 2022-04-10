# 回顾 C++ 变量

## 变量的存储期
### 静态存储期

```
extern uint32_t year;
uint32_t year = 2022;
static uint32_t year = 2022;
```
### 线程局部存储期


``
thread_local unsigned uint32_t year = 2022
```

### 自动存储期

也称栈空间分配的变量

### 动态存储期

也称堆空间分配的变量

# 变量的所有权

* 全局所有权