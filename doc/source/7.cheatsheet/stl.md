# STL 容器速查

## 序列容器

| 容器 | 特点 | 随机访问 | 插入/删除 |
|------|------|----------|-----------|
| vector | 动态数组 | O(1) | 尾部 O(1)，其他 O(n) |
| deque | 双端队列 | O(1) | 头尾 O(1)，其他 O(n) |
| list | 双向链表 | O(n) | 任意位置 O(1) |
| forward_list | 单向链表 | O(n) | 头部/迭代器后 O(1) |
| array | 固定数组 | O(1) | 不支持 |

```cpp
// vector
std::vector<int> v = {1, 2, 3};
v.push_back(4);
v.pop_back();
v.reserve(100);
v.resize(10);
v.shrink_to_fit();

// deque
std::deque<int> d = {1, 2, 3};
d.push_front(0);
d.push_back(4);

// list
std::list<int> l = {1, 2, 3};
l.push_front(0);
l.sort();
l.unique();
l.merge(other);
```

## 关联容器

| 容器 | 底层结构 | 查找 | 插入 | 有序 |
|------|----------|------|------|------|
| set | 红黑树 | O(log n) | O(log n) | 是 |
| map | 红黑树 | O(log n) | O(log n) | 是 |
| multiset | 红黑树 | O(log n) | O(log n) | 是 |
| multimap | 红黑树 | O(log n) | O(log n) | 是 |
| unordered_set | 哈希表 | O(1) 平均 | O(1) 平均 | 否 |
| unordered_map | 哈希表 | O(1) 平均 | O(1) 平均 | 否 |

```cpp
// map
std::map<std::string, int> m;
m["key"] = 42;
m.insert({"key2", 100});
m.emplace("key3", 200);

if (auto it = m.find("key"); it != m.end()) {
    std::cout << it->second;
}

// C++17: insert_or_assign, try_emplace
m.insert_or_assign("key", 50);
m.try_emplace("key", 60);  // 只在不存在时插入

// unordered_map
std::unordered_map<std::string, int> um;
um.reserve(1000);  // 预分配桶
```

## 容器适配器

```cpp
// stack (LIFO)
std::stack<int> s;
s.push(1);
s.top();
s.pop();

// queue (FIFO)
std::queue<int> q;
q.push(1);
q.front();
q.back();
q.pop();

// priority_queue (最大堆)
std::priority_queue<int> pq;
pq.push(1);
pq.top();  // 最大元素
pq.pop();

// 最小堆
std::priority_queue<int, std::vector<int>, std::greater<int>> min_pq;
```

## 常用算法

```cpp
#include <algorithm>

std::vector<int> v = {3, 1, 4, 1, 5};

// 排序
std::sort(v.begin(), v.end());
std::sort(v.begin(), v.end(), std::greater<>{});  // 降序
std::stable_sort(v.begin(), v.end());

// 查找
auto it = std::find(v.begin(), v.end(), 4);
auto it = std::find_if(v.begin(), v.end(), [](int x) { return x > 3; });
bool found = std::binary_search(v.begin(), v.end(), 4);

// 变换
std::transform(v.begin(), v.end(), v.begin(), [](int x) { return x * 2; });

// 过滤
auto new_end = std::remove_if(v.begin(), v.end(), [](int x) { return x < 3; });
v.erase(new_end, v.end());

// 聚合
int sum = std::accumulate(v.begin(), v.end(), 0);
int product = std::accumulate(v.begin(), v.end(), 1, std::multiplies<>{});

// 其他
std::reverse(v.begin(), v.end());
std::shuffle(v.begin(), v.end(), std::random_device{});
auto [min, max] = std::minmax_element(v.begin(), v.end());
```

## C++17/20 新算法

```cpp
// C++17 并行算法
#include <execution>
std::sort(std::execution::par, v.begin(), v.end());

// C++20 Ranges
#include <ranges>
auto result = v 
    | std::views::filter([](int x) { return x > 0; })
    | std::views::transform([](int x) { return x * 2; })
    | std::views::take(5);
```

## 字符串

```cpp
std::string s = "hello";

// 操作
s += " world";
s.append("!");
s.insert(0, "Hi ");
s.erase(0, 3);
s.replace(0, 5, "Hello");

// 查找
size_t pos = s.find("world");
size_t pos = s.rfind("l");
size_t pos = s.find_first_of("aeiou");

// 子串
std::string sub = s.substr(0, 5);

// 转换
int n = std::stoi("42");
double d = std::stod("3.14");
std::string ns = std::to_string(42);

// string_view (C++17)
std::string_view sv = s;  // 无拷贝
```

## 迭代器

```cpp
// 迭代器类型
iterator                // 可修改
const_iterator          // 只读
reverse_iterator        // 反向
const_reverse_iterator  // 反向只读

// 迭代器函数
std::advance(it, n);    // 移动 n 步
std::next(it, n);       // 返回向前 n 步的迭代器
std::prev(it, n);       // 返回向后 n 步的迭代器
std::distance(first, last);  // 计算距离
```
