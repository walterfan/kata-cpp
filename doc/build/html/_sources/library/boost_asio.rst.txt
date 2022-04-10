###########################
Boost asio module
###########################

Overview
===========================

Programs that use Boost.Asio for asynchronous data processing are based on I/O services and I/O objects. 
I/O services abstract the operating system interfaces that process data asynchronously. 
I/O objects initiate asynchronous operations. 

These two concepts are required to separate tasks cleanly: 

* I/O services look towards the operating system API, and 
* I/O objects look towards tasks developers need to do.


设计思想
===========================



主要结构
===========================




一般流程
===========================