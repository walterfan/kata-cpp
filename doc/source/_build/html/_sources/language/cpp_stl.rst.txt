#############
C++ STL
#############

.. contents::
    :local:

容器库 contrainers library
===========================

序列容器，关联容器，窗口适配器

放在容器中的对象要被复制，移动，交换，所以它们要实现

.. code-block:: c++

    class T {
        public:
            T();
            T(const T& t);
            ~T();
            T& operator=(const T& t);
            bool operator <(const T& t);//for sort and map/set contrainer
    }

* array
* vector
* stack
* query
* dqueue
* list
* forward_list
* set
* unordered_set
* map
* unordered_map


迭代器库 iterators library
===========================
iterator

迭代器分类
-------------------------------
输入迭代器 input iterators

输出迭代器 output iterators

正向迭代器 forward iterators

双向迭代器 brdirection iterators

随机访问迭代器 random access  iterators



迭代器适配器
-------------------------------
1. 反向适配器 revers_iterator


2. 插入适配器 insert_iterator


3. 移动适配器 move_iterator


算法库 algorithms library
===========================
* algorithm


1. 非变化序列运算
-----------------------------
它不会改变原始序列中的元素的值和顺序

inner_product
accumulate
find
count
mismatch
search
equal

2. 可变序列元素值运算
-----------------------------
会改变原始序列中的元素的值

swap
copy
transform
replace
remove
revsrse
rotate
fill
shufle

3. 可变序列元素顺序运算
-----------------------------
会改变原始序列中的元素的顺序

sort
stable_sort
binary_search
merge
min
max


数值库 numberics library
===========================
* complex
* cmath
* valarray
* numberic
* random
* ratio
* cfenv


函数对象
==========================

也称防函数， 这是一种重载了函数调用运算符 `operator()()` 的类对象, 它提供了一种比原生指针更加高效的，
将函数作为实参传入另一个函数的方式

例如

.. code-block:: c++

    class Volume
    {
    public:
        double operator()(double x, double y, double z) {
            return x * y * z;
        }
    };

    Volume volume;
    double room{ volume(10, 20, 30);};



Lambda 表达式
==========================

.. code-block:: c++

    [](double value) { return value*value*value; }

智能指针
==========================

指针的模板类。在很多情况下,它们没有任何差别。但是,有些情况下,它们还是有两点

差别:
* 智能指针只能用来保存堆上分配的内存的地址。
* 不能像对原生指针那样, 对智能指针进行一些自增或自减这样的算术运算。


unique_ptr
-------------------------

`unique_ptr<>` 对象就像一个指向类型T的指针, 而且 `unique_ptr<T>` 是排他的,

这意味着不可能有其他的 unique_ptr<T> 指向同一个地址。

一个 unique_ptr<> 对象完全拥有它所指向的内容。不能指定或复制一个 uniqueptr<>对象可以通过使用
一个定义在 utility头文件中的 `std:move()` 函数来移出一个 unique_ptr<T对象存储的地址。

在进行这个操作之后, 之前的 `unique_ptr<t>` 变为无效。当需要独占一个对象的所有权时, 可以使用 `unique_ptr<T>`


shared_ptr
--------------------------

`shared_ptr<T>` 对象就像一个指向类型T的指针, 和 `unique_ptr<T>` 不同的是,多个 `shared_ptr<T>` 可以指向同一个地址,
因此 `sharedptr<T>` 允许共享一个对象的所有权。引用计数保存了指向给定地址的 `shared_ptr<T>` 的数量.

* 每当一个新的 `shared_ptr<T>` 指针指向一个特定堆地址时,引用计数就会加 1。
* 当一个 `shared_ptr<T>` 被释放或者指向了新的地址时,引用计数就会减1。
* 当没有 `shared ptr<T>` 指向这个地址时,引用计数将会变为0,在堆上为这个对象分配的内存就会自动释放。

所有指向同一个地址的 `shared_ptr<T>` 都能得到引用计数的值。

weak_ptr
--------------------------

`weak_ptr<T>` 可以从一个 `shared_ptr<T>` 对象创建,它们指向相同的地址。

创建一个 `weak_ptr<T>` 不会增加 `shared_ptr<T>` 对象的引用计数,所以它会阻止指向的对象销毁。

当最后一个 `shared ptr<T>` 引用被释放或重新指向一个不同的地址时, 它们所指向对象的内存将被释放,即使相关的 `weak_ptr<T>` 可能仍然存在


使用 `weak_ptr<T>` 对象的主要原因是, 我们可能在不经意间创建一个循环引用。

循环引用从概念上说就是一个 `shared_ptr<T>` 对象 pA, 指向了另一个 `shared ptr<T>` 对象 pB, pB 反过来变如此。
在这种情况下,两个对象都不能被释放。在实际中,循环引用的发生要比这个复杂。

设计 `weak_ptr<T>` 就是为了避免循环引用的问题。通过使用`weak_ptr<T>` 指向一个`shared_ptr<>`
所指向的对象,就可以避免循环引用,稍后会对此做一些解释。

当最后一个 `shared_ptr<>` 对象析构时, 它所指向的对象也析构了, 这个时候任何和 `shared ptr<>`关联的 `weak_ptr<t>`
对象都指向一个无效的对象。