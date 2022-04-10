###############
Boost
###############


.. toctree::
   :maxdepth: 1
   :caption: programming language:

   boost_asio

.. contents::
    :local:



代码结构
============================

* algorithm 算法库
* align 内存对齐库
* asio 异步并发库
* atomic 原子操作库
* beast 高级网络通信库
* bind - bind 表达式库


安装编译
============================

.. code-block:: bash

    ./bootstrap.sh
    ./b2 install
    ls -l /usr/local/include /usr/local/lib


模块
============================

atomic
----------------------------


thread
----------------------------


asio
----------------------------

同步操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

我们的程序将至少具有一个 I/O 执行上下文，例如 boost::asio::io_context 对象，`boost::asio::thread_pool 对象或 boost::asio::system_context`。
此 I/O 执行上下文表示您的程序与操作系统的 I/O 服务的链接。

1. 您的程序通过调用I/O 对象来启动连接操作：

.. code-block::

   socket.connect（server_endpoint）;

2. I/O 对象将请求转发到 I/O 执行上下文。

3. I/O 执行上下文在操作系统上调用以执行连接操作。

4. 操作系统将操作结果返回到 I/O 执行上下文。

5. I/O 执行上下文将由操作导致的任何错误转换为 boost::system::error_code 类型的对象。 可以将error_code与特定值进行比较，或将其作为布尔值进行测试（错误结果表示未发生错误）。 然后将结果转发回I/O 对象。

6. 如果操作失败，则I/O 对象将引发类型为 boost::system::system_error 的异常。 如果启动操作的代码改为编写为：

.. code-block::

   boost::system::error_code ec;
   socket.connect（server_endpoint，ec）;


异步操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~


1.您的程序通过调用 I/O 对象来启动连接操作：

.. code-block::

   socket.async_connect（server_endpoint，your_completion_handler）;

其中 your_completion_handler 是具有签名的函数或函数对象：
无效 your_completion_handler（const boost::system::error_code＆ec ;
所需的确切签名取决于正在执行的异步操作。 参考文档指出了每种操作的适当形式。

2. I/O 对象将请求转发到 I/O 执行上下文。

3. I/O 执行上下文向操作系统发出信号，表明它应该启动异步连接。

4. 操作系统通过将结果放在队列中来指示连接操作已完成，该队列准备由 I/O 执行上下文拾取。

5. 使用 io_context 作为 I/O 执行上下文时，您的程序必须调用 io_context::run()（或类似的  io_context 成员函数之一）才能检索结果。 在未完成的异步操作完成时，对 io_context::run() 的调用会阻塞，因此通常在开始第一个异步操作后立即调用它。

6. 在调用 io_context::run() 的同时， I/O 执行上下文使操作结果出队，将其转换为 error_code ，然后将其传递给完成处理程序。

示例
-------------------------

* chart server

https://www.boost.org/doc/libs/1_74_0/doc/html/boost_asio/example/cpp11/chat/chat_server.cpp

* chart client

https://www.boost.org/doc/libs/1_74_0/doc/html/boost_asio/example/cpp11/chat/chat_client.cpp

* chart message

https://www.boost.org/doc/libs/1_74_0/doc/html/boost_asio/example/cpp11/chat/chat_message.hpp