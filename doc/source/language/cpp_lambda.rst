#####################
C++ Lambda
#####################

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

Overview
========================================
lambda 以方括号开始 [], 这个称为 lambda 引入符, 其中可以包含在当前作用域中捕捉到的闭包变量
其后是小括号(), 其中包含参数，这个可以没有
其后是大括号 {}， 其中包含 lambda 表达式



基本形式：

.. code-block::

    [闭包](参数){语句组}

例1:

.. code-block:: c++

    auto f1 = [](int a , int b) { return a+b; }
    auto f2 = [](int a, int b) -> int {return a +b; };


例2:

.. code-block:: c++

    double pi = 3.14;
    auto cal_area = [pi](double r) {return pi * pow(r, 2.0); };
    BOOST_LOG_TRIVIAL(trace) << "area: " << cal_area(10);


可以采用两种方式来捕捉所在作用域内的所有变量

* `[=]` 通过值捕捉所有变量
* `[&]` 通过引用捕捉所有亦是


还可以用如下方式捕捉指定的变量

* `[&x]` 通过引用捕捉变量 x
* `[x]` 通过值捕捉变量 x
* `[=， &x， &y]` 通过值捕捉所有变量，除了 x 和 y 是通过引用捕捉的
* `[this]` 捕捉当前对象，在 lambda 表达式体内，即使没有使用 `this->`， 也可以访问这个对象的成员
* `[*this]` 通过当前对象的副本