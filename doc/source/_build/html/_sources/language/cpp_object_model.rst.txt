###################
C++ Object Model
###################


Overview
===================

1. 每一个 class 产生出一堆指向 virtual functions 的指针，放在表格中， 这个表格称为虚函数表

2. 每一个 class object 被安插一个指针，指向相关的 virtual table, 这个指针称之为 vptr


虚拟继承
===================

base class 只会存在一个实例

