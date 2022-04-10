#################
C++ Thread
#################

Overview
=================

* 当前线程可能随时会被切换出去，或者说被抢占(preempt) 了
* 多线程程序中事件的发生顺序不再有全局统一的先后关系


.. code-block:: c++

    #include <thread>

    void print_num(int n) {
        std::cout << "thread " << n << std::endl;
    }

    void thread_example() {
        std::thread t1;
        t1 = std::thread(print_num, 1);
        t1.join();
    }






pthread library
=====================

11 个接口函数

* thread 线程的创建和等待结束
* mutex 锁的创建，销毁，加锁，解锁
* condition 条件变量的创建，销毁，等待，通知，广播

memory model

memory visibility