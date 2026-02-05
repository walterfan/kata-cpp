# 智能指针

## unique_ptr

`std::unique_ptr` 表示独占所有权，是最常用的智能指针。

```cpp
#include <memory>

// 创建
auto ptr = std::make_unique<int>(42);  // C++14 推荐
std::unique_ptr<int> ptr2(new int(42)); // 也可以

// 使用
std::cout << *ptr << "\n";  // 42

// 移动所有权
auto ptr3 = std::move(ptr);  // ptr 变为 nullptr

// 释放并获取原始指针
int* raw = ptr3.release();
delete raw;

// 重置
ptr3.reset(new int(100));  // 释放旧对象，管理新对象
ptr3.reset();  // 释放并置空
```

### 数组支持

```cpp
// 管理数组
auto arr = std::make_unique<int[]>(100);
arr[0] = 42;

// 自定义删除器
auto file_ptr = std::unique_ptr<FILE, decltype(&fclose)>(
    fopen("file.txt", "r"), 
    &fclose
);
```

### 函数参数传递

```cpp
// 转移所有权：按值传递 unique_ptr
void take_ownership(std::unique_ptr<Widget> widget) {
    // 函数拥有 widget
}

auto w = std::make_unique<Widget>();
take_ownership(std::move(w));  // 必须显式 move

// 不转移所有权：传递引用或原始指针
void use_widget(const Widget& widget);  // 只读
void use_widget(Widget& widget);        // 可修改
void use_widget(Widget* widget);        // 可选参数（可为 null）
```

## shared_ptr

`std::shared_ptr` 表示共享所有权，使用引用计数。

```cpp
#include <memory>

// 创建
auto sp = std::make_shared<int>(42);  // 推荐：一次分配

// 拷贝（增加引用计数）
auto sp2 = sp;
std::cout << sp.use_count();  // 2

// 比较
if (sp == sp2) { /* 指向同一对象 */ }

// 检查
if (sp) { /* 非空 */ }
```

### 自定义删除器

```cpp
// 自定义删除逻辑
auto sp = std::shared_ptr<int>(
    new int(42),
    [](int* p) { 
        std::cout << "Deleting " << *p << "\n";
        delete p;
    }
);

// 管理非内存资源
auto socket = std::shared_ptr<int>(
    new int(create_socket()),
    [](int* fd) { close(*fd); delete fd; }
);
```

### 陷阱：循环引用

```cpp
struct Node {
    std::shared_ptr<Node> next;  // 循环引用！
};

auto a = std::make_shared<Node>();
auto b = std::make_shared<Node>();
a->next = b;
b->next = a;  // 引用计数永不为 0，内存泄漏！

// 解决方案：使用 weak_ptr
struct SafeNode {
    std::shared_ptr<SafeNode> next;
    std::weak_ptr<SafeNode> prev;  // 弱引用
};
```

### 陷阱：从 this 创建 shared_ptr

```cpp
class Bad {
public:
    std::shared_ptr<Bad> get_shared() {
        return std::shared_ptr<Bad>(this);  // 危险！
    }
};

// 正确做法：继承 enable_shared_from_this
class Good : public std::enable_shared_from_this<Good> {
public:
    std::shared_ptr<Good> get_shared() {
        return shared_from_this();
    }
};

auto p = std::make_shared<Good>();
auto p2 = p->get_shared();  // 安全
```

## weak_ptr

`std::weak_ptr` 是对 `shared_ptr` 管理对象的弱引用，不增加引用计数。

```cpp
std::shared_ptr<int> sp = std::make_shared<int>(42);
std::weak_ptr<int> wp = sp;

std::cout << wp.use_count();  // 1（不增加）

// 检查对象是否存在
if (!wp.expired()) {
    // 对象可能在下一刻被销毁
}

// 安全访问：提升为 shared_ptr
if (auto locked = wp.lock()) {
    std::cout << *locked;
}

// 对象销毁后
sp.reset();
std::cout << wp.expired();  // true
auto locked = wp.lock();    // 返回空 shared_ptr
```

### 用途：缓存

```cpp
class ResourceCache {
    std::unordered_map<std::string, std::weak_ptr<Resource>> cache_;

public:
    std::shared_ptr<Resource> get(const std::string& key) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            if (auto sp = it->second.lock()) {
                return sp;  // 缓存命中
            }
            cache_.erase(it);  // 清理过期条目
        }
        
        auto resource = std::make_shared<Resource>(key);
        cache_[key] = resource;
        return resource;
    }
};
```

## make_shared vs new

```cpp
// 推荐：make_shared
auto sp1 = std::make_shared<Widget>(arg1, arg2);

// 不推荐：使用 new
std::shared_ptr<Widget> sp2(new Widget(arg1, arg2));
```

**make_shared 优点：**

1. **单次分配**：对象和控制块一起分配
2. **异常安全**：避免内存泄漏
3. **代码简洁**：类型只写一次

```cpp
// 异常安全问题
void dangerous(std::shared_ptr<A> a, std::shared_ptr<B> b);

dangerous(
    std::shared_ptr<A>(new A()),  // 可能泄漏！
    std::shared_ptr<B>(new B())   // 如果这里抛异常
);

// 安全版本
dangerous(
    std::make_shared<A>(),
    std::make_shared<B>()
);
```

**make_shared 缺点：**

1. 不能使用自定义删除器
2. 控制块和对象一起释放（weak_ptr 延长控制块生命周期）

## 性能考虑

### shared_ptr 开销

```cpp
// shared_ptr 开销
// - 两个指针（对象 + 控制块）
// - 引用计数原子操作

// 在性能关键路径避免频繁拷贝
void process(const std::shared_ptr<Data>& data);  // 传引用
void process(Data* data);  // 或传原始指针

// 不要这样
void process(std::shared_ptr<Data> data);  // 拷贝增加引用计数
```

### unique_ptr 开销

```cpp
// unique_ptr 通常与原始指针大小相同
static_assert(sizeof(std::unique_ptr<int>) == sizeof(int*));

// 除非使用非空删除器
auto up = std::unique_ptr<int, void(*)(int*)>(
    new int, [](int* p) { delete p; }
);
// 需要存储函数指针，更大
```

## 智能指针选择指南

```cpp
// 默认选择：unique_ptr
auto widget = std::make_unique<Widget>();

// 需要共享时：shared_ptr
auto shared = std::make_shared<Widget>();

// 打破循环/观察者：weak_ptr
std::weak_ptr<Widget> observer = shared;

// 与 C API 交互：原始指针
void c_api(Widget* ptr);
c_api(widget.get());
```

```{tip}
智能指针使用原则：
1. 优先使用 `unique_ptr`
2. 只在需要共享所有权时使用 `shared_ptr`
3. 使用 `weak_ptr` 打破循环引用
4. 使用 `make_unique` 和 `make_shared`
5. 传参时优先传引用或原始指针
```
