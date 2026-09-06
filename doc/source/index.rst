.. 现代 C++ 实战开发指南 documentation master file

现代 C++ 实战开发指南
=======================================

.. image:: https://isocpp.org/assets/images/cpp_logo.png
   :alt: C++ Logo
   :width: 150px
   :align: center

.. include:: links.ref
.. include:: tags.ref
.. include:: abbrs.ref

============= ==============================================================
**Abstract**  现代 C++ 实战开发指南 - 专注于易错和易忽略的知识点
**Authors**   Walter Fan
**Category**  learning note
**Status**    WIP
**Updated**   |date|
**License**   |CC-BY-NC-ND|
============= ==============================================================

欢迎阅读 **现代 C++ 实战开发指南**！本文档专注于 C++ 开发中程序员 **易错** 和 **易忽略** 的知识点，
以及 C++ 相比其他语言 **特殊** 的地方，尤其是 **现代 C++ (C++11/14/17/20/23)** 引入的特性。

目标读者
--------

- 有一定编程基础，想深入学习 C++ 的开发者
- 从 Python/Java/Go 等语言转向 C++ 的程序员
- 希望避免常见陷阱，写出高质量 C++ 代码的工程师
- 想要掌握现代 C++ 特性的 legacy 代码维护者

文档特点
--------

- **实用导向**：每个知识点都有实际应用场景
- **避坑指南**：重点讲解易错、易混淆的概念
- **最佳实践**：提供业界推荐的编码方式
- **代码示例**：可直接编译运行的完整代码
- **对比分析**：与其他语言特性的对比说明

.. toctree::
   :maxdepth: 2
   :caption: 目录
   :numbered:

   1.basic/index
   2.concurrency/index
   3.performance/index
   4.memory/index
   5.network/index
   6.library/index
   7.cheatsheet/index
   references

.. toctree::
   :maxdepth: 2
   :caption: 补充笔记

   language/index
   library/index
   tool/index

快速导航
--------

.. grid:: 2
   :gutter: 3

   .. grid-item-card:: 🚀 基础知识与陷阱
      :link: 1.basic/index
      :link-type: doc

      现代 C++ 核心概念，常见陷阱与解决方案，从其他语言迁移的注意事项，C++11/14/17/20/23 新特性速览。

   .. grid-item-card:: ⚡ 并发编程
      :link: 2.concurrency/index
      :link-type: doc

      std::thread、std::async、mutex、condition_variable、atomic、协程详解，并发模式与最佳实践。

   .. grid-item-card:: 📊 性能调优
      :link: 3.performance/index
      :link-type: doc

      perf/gprof 性能分析、基准测试、编译器优化、CPU cache、SIMD 向量化。

   .. grid-item-card:: 🧠 内存管理
      :link: 4.memory/index
      :link-type: doc

      智能指针、RAII、内存布局、自定义分配器、内存泄漏排查、valgrind/ASan 工具。

   .. grid-item-card:: 🌐 网络编程
      :link: 5.network/index
      :link-type: doc

      Socket 编程、TCP/UDP、HTTP/HTTPS、WebSocket、异步 I/O 模型。

   .. grid-item-card:: 📦 常用库
      :link: 6.library/index
      :link-type: doc

      GoogleTest、JsonCpp、libcurl、Boost、OpenSSL 等常用库详解。

经典资源
--------

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - 资源名称
     - 说明
   * - `C++ Reference <https://en.cppreference.com/>`_
     - 最权威的 C++ 在线参考文档
   * - `ISO C++ <https://isocpp.org/>`_
     - C++ 标准委员会官方网站
   * - `Compiler Explorer <https://godbolt.org/>`_
     - 在线查看编译器生成的汇编代码
   * - `Quick Bench <https://quick-bench.com/>`_
     - 在线 C++ 性能基准测试


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
