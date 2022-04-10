Resource management 
====================================


Resource management rule summary:
-------------------------------------------

* R.1: Manage resources automatically using resource handles and RAII (Resource Acquisition Is Initialization)
* R.2: In interfaces, use raw pointers to denote individual objects (only)
* R.3: A raw pointer (a T*) is non-owning
* R.4: A raw reference (a T&) is non-owning
* R.5: Prefer scoped objects, don't heap-allocate unnecessarily
* R.6: Avoid non-const global variables

Allocation and deallocation rule summary:
-------------------------------------------

* R.10: Avoid malloc() and free()
* R.11: Avoid calling new and delete explicitly
* R.12: Immediately give the result of an explicit resource allocation to a manager object
* R.13: Perform at most one explicit resource allocation in a single expression statement
* R.14: Avoid [] parameters, prefer span
* R.15: Always overload matched allocation/deallocation pairs

Smart pointer rule summary:
-------------------------------------------

* R.20: Use unique_ptr or shared_ptr to represent ownership
* R.21: Prefer unique_ptr over shared_ptr unless you need to share ownership
* R.22: Use make_shared() to make shared_ptrs
* R.23: Use make_unique() to make unique_ptrs
* R.24: Use std::weak_ptr to break cycles of shared_ptrs
* R.30: Take smart pointers as parameters only to explicitly express lifetime semantics
* R.31: If you have non-std smart pointers, follow the basic pattern from std
* R.32: Take a unique_ptr<widget> parameter to express that a function assumes ownership of a widget
* R.33: Take a unique_ptr<widget>& parameter to express that a function reseats the widget
* R.34: Take a shared_ptr<widget> parameter to express that a function is part owner
* R.35: Take a shared_ptr<widget>& parameter to express that a function might reseat the shared pointer
* R.36: Take a const shared_ptr<widget>& parameter to express that it might retain a reference count to the object ???
* R.37: Do not pass a pointer or reference obtained from an aliased smart pointer


FAQ
-----------------


什么是完美转发
~~~~~~~~~~~~~~~~~

std::forward通常是用于完美转发的，它会将输入的参数原封不动地传递到下一个函数中，这个“原封不动”指的是，如果输入的参数是左值，那么传递给下一个函数的参数的也是左值；如果输入的参数是右值，那么传递给下一个函数的参数的也是右值。一个经典的完美转发的场景是：

.. code-block:: c++

    template <class... Args>
    void forward(Args&&... args) {
        f(std::forward<Args>(args)...);
    }


* [C++ 核心指南](http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)

* 规则22：用 make_shared() 来构建 shared_ptr 智能指针

Shared_ptr 中有两个指针，一个指针指向其中的动态变量，一个指针指向引用计数
` shared_ptr<X> p1 { new X{2} } ` 会做两次创建
1）创建 class X
2）创建 引用计数变量

.. code-block:: c++


    shared_ptr<X> p1 { new X{2} }; // bad
    auto p = make_shared<X>(2);    // good


为什么呢？
看看boost 的源码

.. code-block:: c++

    template< class T > struct sp_if_not_array
    {
        typedef boost::shared_ptr< T > type;
    };
    // Variadic templates, rvalue reference 可变参数模板, 右值引用
    template< class T, class... Args > typename boost::detail::sp_if_not_array< T >::type make_shared( Args && ... args )
    {
        boost::shared_ptr< T > pt( static_cast< T* >( 0 ), BOOST_SP_MSD( T ) 
    );

        boost::detail::sp_ms_deleter< T > * pd = static_cast<boost::detail::sp_ms_deleter< T > *>( pt._internal_get_untyped_deleter() );

        void * pv = pd->address();

        ::new( pv ) T( boost::detail::sp_forward<Args>( args )... );
        pd->set_initialized();

        T * pt2 = static_cast< T* >( pv );

        boost::detail::sp_enable_shared_from_this( &pt, pt2, pt2 );
        return boost::shared_ptr< T >( pt, pt2 );
    }
