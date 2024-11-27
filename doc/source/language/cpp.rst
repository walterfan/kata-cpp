#######
C++
#######


.. include:: ../links.ref
.. include:: ../tags.ref
.. include:: ../abbrs.ref

============ ==========================
**Abstract** C++
**Authors**  Walter Fan
**Status**   v1
**Updated**  |date|
============ ==========================

.. |date| date::

.. contents::
   :local:

C++ 新特性
==================================

C++ 11 新特性
-------------------------------

改进的对象构造
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* 继承基类的构造函数
* 默认的成员值
* 委托构造函数
* overwrite 关键字
* final 关键字
* 统一的初始化 {}


其他语言增强
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* 基于范围的 for 循环
* long long int 类型 (64位)
* lambda 函数
* 移动语义和右值引用
* 强类型枚举: enum 增加了类型检查
* 智能指针
* 原始字符串字面量
* 静态断言
* 可变参数模板
* 改善了右尖括号的处理

新的关键字
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* auto 自动类型推导
* constexpr: 表达式常量
* decltype: 声明类型
* nullptr: 空指针
* thread_local: 线程局部存储

对 C++ 库的增强
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* stl 容器的列表初始化
* 随机库
* 正则表达式库
* 无序容器(hash)
* 附加的算法
* 元组模板
* 其他的新类型: chrono(时间测量), ratio(有理数分数), complex(复数)


C++ 服务器端编程
---------------------------

* 事件驱动
* 多路复用
* 多线程并发



C++ 十大面试题
==================================

1. C++中 指针与引用的区别
-------------------------------------------------------------


数组名会自动转换为指向数组第一个元素的指针

- 指向 const 对象的指针: 不允许用指针操作来改变其所批的 const 值

.. code-block:: c++

    const double pi = 3.14;
    double * ptr = &pi; //.. error
    const doube * ptr = &pi; // correct
    *ptr = 3.14159; // error

- const 指针: 指针本身的值不能修改

.. code-block:: c++

    int errorCode = 0;
    int errorCode2 = -1;
    int * const ptrErr = &errorCode;
    ptrErr = &errorCode2; //error

2. 一个空的 C++ class , 编译器会为它默认添加哪些东西
-------------------------------------------------------------

参见 https://en.cppreference.com/w/cpp/language/rule_of_three

默认构造函数, 拷贝构造函数, 析构函数 赋值操作符以及取值函数

C++ 11 之前, 一般要求实现这3个函数

* user-defined destructor 自定义的析构
* a user-defined copy constructor 自定义的拷贝构造函数
* a user-defined copy assignment operator, 自定义的赋值操作符

C++ 11 之后添加了两个方法:
* 移动构造 move constructor
* 移动赋值操作符 the move assignment operator


.. code-block:: c++

    class User
    {
    Public
        User();
        User(const User&);
        ~User();
        User& operator=(const User&)
        User* operator&();
        Const User* operator&() const;

        //c++ 11, move constructor
        User(User&& other);
        User& operator=(User&& other);


    }

explicit 构造函数是用来防止隐式转换的


3. 封装, 访问控制与友元
-------------------------------------------------------------

public, private, protected, friend


4. C++ 虚函数, 虚继承与虚函数表
-------------------------------------------------------------

虚函数是通过虚函数表实现的, 如果一个类含有虚函数, 系统会为这个类分配一个指针成员指向一张虚函数表, 表中每一项指向一个虚函数的地址, 实现上就是一个函数指针的数组

在类对象的内存布局中, 首先是该类的 vtbl 指针, 然后者对象数据, 在通过对象指针 调用一个虚函数时, 编译器生成的代码将先获取对象类的 vtbl 指针, 然后调用  vtbl 中对应的项, 从而实现多态性


构造函数为什么不能是虚函数呢？

    首先需要了解 vptr指针和虚函数表的概念, 以及这两者的关联。

    vptr指针指向虚函数表, 执行虚函数的时候, 会调用vptr指针指向的虚函数的地址。

    当定义一个对象的时候, 首先会分配对象内存空间, 然后调用构造函数来初始化对象。vptr变量是在构造函数中进行初始化的。又因为执行虚函数需要通过vptr指针来调用。如果可以定义构造函数为虚函数, 那么就会陷入先有鸡还是先有蛋的循环讨论中。



基类的析构函数为什么必须是虚函数呢？

    我们都知道, 想要回收一个对象申请的资源, 那么就需要调用析构函数。虽然我们没有显示地调用析构函数, 但是编译器都会默认地为我们执行析构函数。

    那么当我们执行 `BaseClass *base = new BaseClass();` 当我们执行 `delete base` 时, 会调用析构函数为我们释放资源。而 我们执行 `BaseClass *sub = new SubClass();` 如果 `BaseClass` 基类的析构函数不是虚函数的时候, `delete sub` 对象的时候, 只会释放 `BaseClass` 基类申请的资源, 而不是释放 `SubClass` 派生类的资源。原因如下：

    基类指针指向了派生类对象, 而基类中的析构函数是非virtual的, 而虚构函数是动态绑定的基础。现在析构函数不是virtual的, 因此不会发生动态绑定, 而是静态绑定, 指针的静态类型为基类指针, 因此在delete的时候只会调用基类的析构函数, 而不会调用派生类的析构函数。这样, 在派生类中申请的资源就不会得到释放, 就会造成内存泄漏, 这是相当危险的：如果系统中有大量的派生类对象被这样创建和销毁, 就会有内存不断的泄漏, 久而久之, 系统就会因为缺少内存而崩溃。

    当然, 如果在派生类中没有动态申请有资源的时候, 是不会造成内存泄漏的。而当派生类对象的析构函数中有内存需要回收, 并且在编程过程中采用了基类指针指向派生类对象, 如为了实现多态, 并且通过基类指针将对象销毁, 这时, 就会因为基类的析构函数为非虚函数而不触发动态绑定, 从而没有调用派生类的析构函数而导致内存泄漏。

    因此, 为了防止这种情况下的内存泄漏的发生, 最后将基类的析构函数写成virtual虚析构函数。

5. C++ 显示显式类型转换的好处和分类
-------------------------------------------------------------

* static_cast 完成相关类型之间的转换
* reinterpret_cast 处理互不相关类型之间的转换
* dynamic_cast 将基类类型转换为子类的类型, 它涉及运行时检查
* const_cast 转换掉表达式的 const 性质

6. 如果避免多重继承中的在派生层次中出现多次
-------------------------------------------------------------

虚继承,
比如 istream 继承自 ios, ostream 也继承自 ios, 而 iostream 继承自 istream 和 ostream,
所以一个 iostream 对象可能会包含两个 ios 对象, 所以要用到虚继承。

.. code-block:: c++

    class istream: public virtual ios { ... }
    class ostream: public virtual ios { ... }

7. 为什么提倡用 make_shared, 而不是直接用 shared_ptr<T>(new T)
--------------------------------------------------------------------------

1. 类型名只需写一遍, 避免冗余

2. 避免可能造成的内存泄漏, new 出来的对象异常没有放到 shared_ptr 中
例如 effective mordern c++ 条款 21 中举的例子

3. 性能和效率更高, 只要分配一次单块内存来存储对象和 shared_ptr 控制块


8. 什么是奇异模板
-------------------------------------------------------------------------
在 C++ 中，"奇异模板" (Curiously Recurring Template Pattern, 简称 CRTP) 是一种常用的模板编程技巧，
主要用于实现静态多态 (static polymorphism) 或为派生类提供通用的功能。它的名字来源于其递归的结构：一个类使用自己作为模板参数传递给基类。

CRTP 的基本结构

CRTP 的核心是定义一个基类模板，并让派生类作为模板参数传递给这个基类。

基本代码示例

.. code-block:: c++

    #include <iostream>

    // 基类模板
    template <typename Derived>
    class Base {
    public:
        void interface() {
            // 调用派生类的方法
            static_cast<Derived*>(this)->implementation();
        }

        // 默认实现 (可选) 
        void implementation() {
            std::cout << "Base implementation\n";
        }
    };

    // 派生类
    class Derived : public Base<Derived> {
    public:
        void implementation() {
            std::cout << "Derived implementation\n";
        }
    };

    int main() {
        Derived d;
        d.interface();  // 输出 "Derived implementation"
        return 0;
    }

关键点

 1. 模板参数传递派生类：
 * 基类模板 Base 的模板参数是派生类 Derived。
 * 在 Base 中，通过将 this 指针静态转换为 Derived* 类型，调用派生类中的方法。
 2. 静态多态：
 * 在编译期决定调用的是哪一个类的方法，避免了运行时的虚函数开销。
 * 不同于动态多态 (使用虚函数和 vtable) ，静态多态在编译期直接展开，性能更高。
 3. 递归的奇异性：
 * 派生类引用基类模板，并将自身作为模板参数传递，因此称为"奇异"。

使用场景

1. 实现静态多态

CRTP 通常用于替代虚函数的运行时多态，尤其是性能敏感的场景。

代码示例

.. code-block:: c++

    #include <iostream>

    template <typename Derived>
    class Shape {
    public:
        void draw() {
            static_cast<Derived*>(this)->draw_impl();
        }
    };

    class Circle : public Shape<Circle> {
    public:
        void draw_impl() {
            std::cout << "Drawing Circle\n";
        }
    };

    class Square : public Shape<Square> {
    public:
        void draw_impl() {
            std::cout << "Drawing Square\n";
        }
    };

    int main() {
        Circle c;
        Square s;
        c.draw(); // 输出 "Drawing Circle"
        s.draw(); // 输出 "Drawing Square"
        return 0;
    }

2. 代码复用

CRTP 可以用于基类提供一部分通用功能，而派生类专注于实现特定逻辑。

示例：统计派生类的实例数量

.. code-block:: c++

    #include <iostream>

    template <typename Derived>
    class Counter {
    private:
        static int count;
    public:
        Counter() { ++count; }
        ~Counter() { --count; }

        static int getCount() { return count; }
    };

    // 静态成员变量定义
    template <typename Derived>
    int Counter<Derived>::count = 0;

    class MyClass : public Counter<MyClass> {};
    class AnotherClass : public Counter<AnotherClass> {};

    int main() {
        MyClass a, b;
        AnotherClass c;

        std::cout << "MyClass instances: " << MyClass::getCount() << "\n"; // 输出 2
        std::cout << "AnotherClass instances: " << AnotherClass::getCount() << "\n"; // 输出 1

        return 0;
    }

3. 控制派生类的行为

基类模板可以对派生类施加一定的约束或统一接口。

优缺点

优点

1. 静态多态：
* 提高性能，无需虚函数和运行时多态的开销。
2. 代码复用：
* 基类模板可以封装通用逻辑，派生类只需专注于具体实现。
3. 灵活性：
* 在基类模板中可以为派生类提供默认实现，也可以强制派生类实现特定行为。

缺点

1. 可读性降低：
* 对新手来说，递归的模板结构可能不容易理解。
2. 类型安全性：
* 如果使用不当 (例如 static_cast 错误) ，可能引发未定义行为。
3. 编译时间开销：
* 模板会增加编译时间，因为 CRTP 是基于模板元编程的。

总结

CRTP 是 C++ 模板编程中的一个强大工具，主要用于静态多态和代码复用。它通过将派生类作为模板参数传递给基类，实现了在编译期确定的多态行为，在高性能场景中尤为常见。

9. 什么是万能引用和完美转发
-------------------------------------------------------------------------
万能引用就是：即可以绑定到左值引用也可以绑定到右值引用, 并且还会保持左右值的const属性的函数模板参数。形如这样的参数 T&& 就是万能引用


10.   为什么要用 extern c
-------------------------------------------------------------
由于gcc和g++生成符号表的方式不同, 导致在C++项目中如果使用gcc编译的C模块, 会出现链接错误。因为链接器会去C模块的 (.o) 文件中查找, __Z3addii这样的符号。显然在C模块的目标文件(.o)中, 不存在__Z3addii这样的符号, 它有的只是_add。


通过extern "C", 告诉g++编译器, 不要对这些函数进行Name mangling, 按照C编译器的方式去生成符号表符号。这样在main.c的目标文件(.o)中, 参数列表为两个int类型的add函数名称为_add。链接器可以正确找到util.o中的add函数 (他们都是_add) 。注意参数列表为两个double类型的add函数名称还是__Z3adddd。

11.  锁的类型和用法
-------------------------------------------------------------

互斥锁 (mutex) 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
可以避免多个线程在某一时刻同时操作一个共享资源, 标准C++库提供了std::unique_lock类模板, 实现了互斥锁的RAII惯用语法：
eg: `std::unique_lock<std::mutex> lk(mtx_sync_);`

条件锁 (condition_variable)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
条件锁就是所谓的条件变量, 某一个线程因为某个条件未满足时可以使用条件变量使该程序处于阻塞状态。一旦条件满足了, 即可唤醒该线程(常和互斥锁配合使用), 唤醒后, 需要检查变量, 避免虚假唤醒。
eg:

.. code-block:: c++

    //线程1：
    // wait ack
    {
        std::unique_lock<std::mutex> lk(mtx_sync_);
        if (!cv_sync_.wait_for(lk, 1000ms, [this](){return sync_; })) // wait for 1s
        {
            // wait failed
            printf("wait for notify timeout [%d].\n", cnt);
            return false;
        }
        else
        {
            return true;
        }
    }

    //线程2：
    {
        std::unique_lock<std::mutex> lk(mtx_sync_);
        sync_ = true;
    }
    // 通知前解锁, 可以避免唤醒线程后由于互斥锁的关系又进入了阻塞阶段
    cv_sync_.notify_one();


自旋锁 (不推荐使用)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
自旋锁是一种基础的同步原语, 用于保障对共享数据的互斥访问。与互斥锁的相比, 在获取锁失败的时候不会使得线程阻塞而是一直自旋尝试获取锁。当线程等待自旋锁的时候, CPU不能做其他事情, 而是一直处于轮询忙等的状态。自旋锁主要适用于被持有时间短, 线程不希望在重新调度上花过多时间的情况。实际上许多其他类型的锁在底层使用了自旋锁实现, 例如多数互斥锁在试图获取锁的时候会先自旋一小段时间, 然后才会休眠。如果在持锁时间很长的场景下使用自旋锁, 则会导致CPU在这个线程的时间片用尽之前一直消耗在无意义的忙等上, 造成计算资源的浪费。

.. code-block:: c++

    // 用户空间用 atomic_flag 实现自旋互斥
    #include <thread>
    #include <vector>
    #include <iostream>
    #include <atomic>

    std::atomic_flag lock = ATOMIC_FLAG_INIT;

    void f(int n)
    {
        for (int cnt = 0; cnt < 100; ++cnt) {
            while (lock.test_and_set(std::memory_order_acquire))  // 获得锁
                ; // 自旋
            std::cout << "Output from thread " << n << '\n';
            lock.clear(std::memory_order_release);               // 释放锁
        }
    }

    int main()
    {
        std::vector<std::thread> v;
        for (int n = 0; n < 10; ++n) {
            v.emplace_back(f, n);
        }
        for (auto& t : v) {
            t.join();
        }
    }

递归锁 (recursive_mutex)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
recursive_mutex 类是同步原语, 能用于保护共享数据免受从个多线程同时访问。

recursive_mutex 提供排他性递归所有权语义：

调用方线程在从它成功调用 lock 或 try_lock 开始的时期里占有 recursive_mutex 。此时期间, 线程可以进行对 lock 或 try_lock 的附加调用。所有权的时期在线程调用 unlock 匹配次数时结束。
线程占有 recursive_mutex 时, 若其他所有线程试图要求 recursive_mutex 的所有权, 则它们将阻塞 (对于调用 lock ) 或收到 false 返回值 (对于调用 try_lock ) 。
可锁定 recursive_mutex 次数的最大值是未指定的, 但抵达该数后, 对 lock 的调用将抛出 std::system_error 而对 try_lock 的调用将返回 false 。
若 recursive_mutex 在仍为某线程占有时被销毁, 则程序行为未定义。 recursive_mutex 类满足互斥 (Mutex) 和标准布局类型 (StandardLayoutType) 的所有要求。


12.  为接收只读字符串的函数选择什么形参类型?
----------------------------------------------------
const char* 或者 const std::string& 都不如 std::string_view, 它是前两者的完美替代
1) std::string_view 不会产生开销, 它从不复制字符串
2) std::string_view 与 std::string 有几乎相同的接口, 只不过没有 c_str(), 但是有 data() 方法


