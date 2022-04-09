C 是一门古老的语言， C++在为C 引入了面向对象和泛型，也引入了许多的复杂性，例如多重继承，模板的特化，等等。同时 C++ 本身除了 STL 标准库，缺少高质量的并发和网络软件包，每个C++程序员或多或少都造过大大小小的轮子。

例如我就写过很多 string 处理的函数, 比如 Trim, LowerCase, UpperCase, 等等，还有基于引用计数的智能指针，包装 pthread 函数的线程类，林林总总，各种轮子都要自己和团队手工打造。有时有点成就感，可是显然也拖慢了开发速度，自己的轮子虽然够用，可是并非考虑周全，异常保护不够，经常会挖坑，也让别人踩坑。

所以近年来，逐渐在项目中引入 boost 库来提高开发效率，也从 C++ 98 向 C++11, 14, 17, 乃至 C++ 20 过渡。

在 C++ 11、14、17、20 中我们可以看到很多 boost library 的影子， 例如  boost::shared_ptr,  boost::thread , boost::random, 等等。

# C++ 11 新特性

## 改进的对象构造

* 继承基类的构造函数
* 默认的成员值
* 委托构造函数
* overwrite 关键字
* final 关键字
* 统一的初始化 {}


## 其他语言增强
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

## 新的关键字
* auto 自动类型推导
* constexpr: 表达式常量
* decltype: 声明类型
* nullptr: 空指针
* thread_local: 线程局部存储

## 对 C++ 库的增强
* stl 容器的列表初始化
* 随机库
* 正则表达式库
* 无序容器(hash)
* 附加的算法
* 元组模板
* 其他的新类型: chrono(时间测量), ratio(有理数分数), complex(复数)

作为一个 Java/C++ 都时常用到的老程序员，委托构造函数，智能指针，lambda 等许多东西都不新鲜，唯独需要重点提到的是右值。

C++ 本来就已经很复杂了，为什么又要弄出来右值这个东西来烧脑呢？ 原因还是在于对于压榨出高性能的需求，就象Linux 系统命令 `cp` 和 `mv`, 原先的值拷贝好比 `cp`, 现在的右值用于移动语义好比 `mv`。

什么是右值，因为左值通常出现在赋值运算符的左边（也可能在右边），而右值一般出现在赋值运算符的右边。

一句话，有名字的是左值，没名字的右值，就因为右值没有名字，它们都是一个临时的变量，对于它们的复制可以简化为移动，反正临时的无名变量没人会用到。

最常用的例子是在函数中返回一个对象, C++11 之前如果对象比较大，最好用指针，而C++11之后适用移动语义，下面这样写就挺好。
```
Command createCommand() {
  Command command;
  command.setName("doSomeThing");
  return command;
}
```
# 例子代码

现代C++类的默认成员函数不仅仅只有构造，析构，拷贝构造函数和赋值运算符，还要加上移动构造函数和移动赋值运算符。例如：

```
class Command
{
public:
    Command();
    Command(std::string name);

    Command(const Command& other); //拷贝构造函数
    Command& operator=(const Command& other);//赋值运算符

    Command(Command&& other); //移动构造函数
    Command& operator=(Command&& other); //移动赋值运算符

    virtual ~Command();

    void setName(const std::string& name);
    void setParameter(const std::string& name, const std::string& value);
    void setData(const uint8_t* pData, size_t length);

    friend std::ostream& operator<<(std::ostream&, const Command& obj);

private:
    std::string m_name = "";
    std::map<std::string, std::string> m_parameters;
    size_t m_length = 0;
    uint8_t* m_data = nullptr;
};
```

具体实现为

```
Command::Command():Command("") {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;    
}

Command::Command(string name):m_name(name) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;    
}

Command::Command(const Command& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". copy construct: " << m_name << "@" <<this;
    m_name = other.m_name;
    for (const auto& kv : other.m_parameters) {
        BOOST_LOG_TRIVIAL(trace) << kv.first << " has value " << kv.second;
        m_parameters.insert(kv);
    }
    std::copy(other.m_data, other.m_data + m_length, m_data);
}

Command& Command::operator=(const Command& other) {
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". operator =: " << other.m_name;
    if (this != &other)
    {
        this->m_name = other.m_name;
        for (const auto& kv : other.m_parameters) {
            std::cout << kv.first << " has value " << kv.second << std::endl;
            m_parameters.insert(kv);
        }
        // Free the existing resource.
        if(m_data) {
            delete[] m_data;
        }
        m_length = other.m_length;
        m_data = new uint8_t[m_length];
        std::copy(other.m_data, other.m_data + m_length, m_data);
    }
    return *this;
}

Command::Command(Command&& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move copy construct: " << other.m_name << "@" <<this;
    m_name = std::move(other.m_name);
    m_parameters = std::move(other.m_parameters);

    // Copy the data pointer and its length from the source object.
    m_data = other.m_data;
    m_length = other.m_length;

    // Release the data pointer from the source object so that
    // the destructor does not free the memory multiple times.
    other.m_data = nullptr;
    other.m_length = 0;

}

Command& Command::operator=(Command&& other) 
{
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move operator ==" << other.m_name;
   if (this != &other)
   {
      m_name = std::move(other.m_name);
      m_parameters = std::move(other.m_parameters);
      // Free the existing resource.
      delete[] m_data;

      // Copy the data pointer and its length from the
      // source object.
      m_data = other.m_data;
      m_length = other.m_length;

      // Release the data pointer from the source object so that
      // the destructor does not free the memory multiple times.
      other.m_data = nullptr;
      other.m_length = 0;
   }
   return *this;
}

Command::~Command() {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". destruct: " << m_name ;
}


void Command::setName(const string& name) {
    m_name = name;
}
void Command::setParameter(const string& name, const string& value) {
    m_parameters[name] = value;
}

void Command::setData(const uint8_t* pData, size_t length) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". setData: " << m_name;
	if(nullptr != m_data) {
        delete[] m_data;
    }
	
    m_length = length;
	m_data = new uint8_t[length];
    std::copy(pData, pData + length, m_data);
}

ostream& operator<<(ostream& os, const Command& obj)
{
    os << obj.m_name;
    os << ": ";
    //for (map<string, string>::iterator it = obj.m_parameters.begin(); it != obj.m_parameters.end(); ++it)
    for (const auto& kv : obj.m_parameters)
    {
        os << kv.first << "=" << kv.second << endl;
    }
    for(size_t i=0; i< obj.m_length; ++i) {
        os << *(obj.m_data + i);
    }
    return os;
}

```

* 头文件
[run_example.h](https://github.com/walterfan/snippets/blob/master/cpp/exam/run_example.h)

```
#ifndef RUN_EXAMPLE_H_
#define RUN_EXAMPLE_H_

#include <stdio.h>
#include <stdint.h>

#include <string>
#include <map>
#include <iostream>
#include <memory>

#include <unordered_map>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/core/noncopyable.hpp>
#include <boost/assert.hpp>

//old function pointer
typedef int (*exam_func_ptr)(int argc, char** argv);
//new function object
typedef std::function<int(int, char**)> exam_func_t;


template<typename T, typename... Ts>
std::unique_ptr<T> my_make_unique(Ts&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}


class Command
{
public:
    Command();
    Command(std::string name);

    Command(const Command& other);
    Command& operator=(const Command& other);

    Command(Command&& other);
    Command& operator=(Command&& other);

    virutal ~Command();

    void setName(const std::string& name);
    void setParameter(const std::string& name, const std::string& value);
    void setData(const uint8_t* pData, size_t length);

    friend std::ostream& operator<<(std::ostream&, const Command& obj);

private:
    std::string m_name = "";
    std::map<std::string, std::string> m_parameters;
    size_t m_length = 0;
    uint8_t* m_data = nullptr;
};

class ExampleRunner: boost::noncopyable {
public:
    ExampleRunner();
    //no need by noncopyable
    //ExampleRunner(const ExampleRunner& rhs);
    //ExampleRunner& operator=(const ExampleRunner& rhs);

    virtual ~ExampleRunner();

        void init();

    size_t size() const;

        void register_example(const std::string& name, const exam_func_t &exam);

        int execute_example(const std::string& name, int argc, char** argv) const;
private:
    volatile uint32_t m_example_count = 0;
    std::unordered_map<std::string, exam_func_t> m_func_examples;        
};


#endif
```

* 实现文件
[run_example.cpp](https://github.com/walterfan/snippets/blob/master/cpp/exam/run_example.cpp)

```
#include "run_example.h"

using namespace std;
namespace po = boost::program_options;

extern int function_demo(int argc, char** argv);
extern int lambda_demo(int argc, char* argv[]);
extern int rvalue_demo(int argc, char* argv[]);
extern int smart_ptr_demo(int argc, char** argv);

const int CASE_COUNT =4;

const char* usage = R"name(please specify example name:
    function_demo
    or lambda_demo
    or rvalue_demo
    or smart_ptr_demo
)name";


//C++11: delegate constructor
Command::Command():Command("") {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;    
}

Command::Command(string name):m_name(name) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". construct: " << m_name << "@" <<this;    
}

Command::Command(const Command& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". copy construct: " << m_name << "@" <<this;
    m_name = other.m_name;
    for (const auto& kv : other.m_parameters) {
        BOOST_LOG_TRIVIAL(trace) << kv.first << " has value " << kv.second;
        m_parameters.insert(kv);
    }
    std::copy(other.m_data, other.m_data + m_length, m_data);
}

Command& Command::operator=(const Command& other) {
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". operator =: " << other.m_name;
    if (this != &other)
    {
        this->m_name = other.m_name;
        for (const auto& kv : other.m_parameters) {
            std::cout << kv.first << " has value " << kv.second << std::endl;
            m_parameters.insert(kv);
        }
        // Free the existing resource.
        if(m_data) {
            delete[] m_data;
        }
        m_length = other.m_length;
        m_data = new uint8_t[m_length];
        std::copy(other.m_data, other.m_data + m_length, m_data);
    }
    return *this;
}

Command::Command(Command&& other) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move copy construct: " << other.m_name << "@" <<this;
    m_name = std::move(other.m_name);
    m_parameters = std::move(other.m_parameters);

    // Copy the data pointer and its length from the source object.
    m_data = other.m_data;
    m_length = other.m_length;

    // Release the data pointer from the source object so that
    // the destructor does not free the memory multiple times.
    other.m_data = nullptr;
    other.m_length = 0;

}

Command& Command::operator=(Command&& other) 
{
	BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< " move operator ==" << other.m_name;
   if (this != &other)
   {
      m_name = std::move(other.m_name);
      m_parameters = std::move(other.m_parameters);
      // Free the existing resource.
      delete[] m_data;

      // Copy the data pointer and its length from the
      // source object.
      m_data = other.m_data;
      m_length = other.m_length;

      // Release the data pointer from the source object so that
      // the destructor does not free the memory multiple times.
      other.m_data = nullptr;
      other.m_length = 0;
   }
   return *this;
}

Command::~Command() {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". destruct: " << m_name ;
}


void Command::setName(const string& name) {
    m_name = name;
}
void Command::setParameter(const string& name, const string& value) {
    m_parameters[name] = value;
}

void Command::setData(const uint8_t* pData, size_t length) {
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__<< ". setData: " << m_name;
	if(nullptr != m_data) {
        delete[] m_data;
    }
	
    m_length = length;
	m_data = new uint8_t[length];
    std::copy(pData, pData + length, m_data);
}

ostream& operator<<(ostream& os, const Command& obj)
{
    os << obj.m_name;
    os << ": ";
    //for (map<string, string>::iterator it = obj.m_parameters.begin(); it != obj.m_parameters.end(); ++it)
    for (const auto& kv : obj.m_parameters)
    {
        os << kv.first << "=" << kv.second << endl;
    }
    for(size_t i=0; i< obj.m_length; ++i) {
        os << *(obj.m_data + i);
    }
    return os;
}


ExampleRunner::ExampleRunner(): m_example_count(0),m_func_examples() {
    BOOST_LOG_TRIVIAL(trace)<<"* ExampleRunner construct: " ;    
}

ExampleRunner::~ExampleRunner() {
    BOOST_LOG_TRIVIAL(trace)<<"* ExampleRunner destruct: ";
}

void ExampleRunner::init() {
    register_example("function_demo", function_demo);
    register_example("smart_ptr_demo", smart_ptr_demo);
    register_example("lambda_demo", lambda_demo);
	register_example("rvalue_demo", rvalue_demo);
}

void ExampleRunner::register_example(const string& name, const exam_func_t &exam)
{
    m_example_count++;
    m_func_examples[name] = exam;
}

int ExampleRunner::execute_example(const string& name, int argc, char** argv) const
{
    auto it = m_func_examples.find(name);
    if(it != m_func_examples.end()) {
        BOOST_LOG_TRIVIAL(trace) << "execute "<< it->first;
        exam_func_t func = it->second;
        return func(argc, argv);
    }
    BOOST_LOG_TRIVIAL(trace) << "not registered "<< name;
    return -1;
}


size_t ExampleRunner::size() const {
    return m_func_examples.size();
}

int main(int argc, char** argv)
{
    unique_ptr<ExampleRunner> runner = my_make_unique<ExampleRunner>();
    runner->init();
    BOOST_ASSERT_MSG(runner->size()==CASE_COUNT, "example count should be 2");
    //c++11 R"raw string"
    po::options_description desc("Allowed options:");
    
    desc.add_options()
        ("help,h", "produce help message")
        ("name,n", po::value<string>(), usage)
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        BOOST_LOG_TRIVIAL(trace) << desc << "\n";
        return 1;
    }

    if (vm.count("name")) {
        BOOST_LOG_TRIVIAL(trace) << "* example name is "<< vm["name"].as<string>() << ".";
        runner->execute_example(vm["name"].as<string>(), argc, argv);
    } else {
        BOOST_LOG_TRIVIAL(trace) << "example name was not set.";
        BOOST_LOG_TRIVIAL(trace) << desc ;
    }

    return 0;
}

```

# 关于智能指针和移动拷贝的例子
* [boost_memory.cpp](https://github.com/walterfan/snippets/blob/master/cpp/exam/boost_memory.cpp)
```
#include "run_example.h"

using namespace std;


int rvalue_demo(int argc, char* argv[])
{
    //move constructor
    Command cmd1(Command("c1"));
    //copy constructor
    Command cmd2 = cmd1;

    shared_ptr<Command> sharedPtr = make_shared<Command>("update");
    sharedPtr->setParameter("user", "alice");
    uint8_t nBytes[3]   = { 0x00,0x01,0x02 };
    sharedPtr->setData(nBytes, 3);
    
    shared_ptr<Command> sharedPtr2 = sharedPtr;
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << ". sharedPtr=" << sharedPtr.get() << ", "<< sharedPtr.use_count() <<", command="<< *sharedPtr;
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << ". sharedPtr2=" << sharedPtr2.get() << ", "<< sharedPtr2.use_count() <<", command="<< *sharedPtr2;

    return 0;
}

int smart_ptr_demo(int argc, char* argv[])
{

    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << " -- unique_ptr --";
    //unique_ptr<Command> ptr = unique_ptr<Command>(new Command());
    unique_ptr<Command> uniquePtr = my_make_unique<Command>();
    uniquePtr->setName("create");
    uniquePtr->setParameter("user", "walter");
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << ". unique pointer=" << uniquePtr.get() <<", command="<< *uniquePtr;

    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << ". -- shared_ptr --";

    shared_ptr<Command> sharedPtr = make_shared<Command>();
    sharedPtr->setName("retrieve");
    sharedPtr->setParameter("user", "walter");
    BOOST_LOG_TRIVIAL(trace) << "line " <<__LINE__ << ". shared pointer=" << sharedPtr.get() << ", "<< sharedPtr.use_count() <<", command="<< *sharedPtr;


    return 0;
}

```

## 运行结果

```
./bin/run_example -n rvalue_demo
[2020-10-07 13:59:40.303527] [0x0000000107d9edc0] [trace]   * ExampleRunner construct:
[2020-10-07 13:59:40.304168] [0x0000000107d9edc0] [trace]   * example name is rvalue_demo.
[2020-10-07 13:59:40.304179] [0x0000000107d9edc0] [trace]   execute rvalue_demo
[2020-10-07 13:59:40.304198] [0x0000000107d9edc0] [trace]   line 27. construct: c1@0x7ffeed644df0
[2020-10-07 13:59:40.304204] [0x0000000107d9edc0] [trace]   line 61 move copy construct: c1@0x7ffeed644db0
[2020-10-07 13:59:40.304210] [0x0000000107d9edc0] [trace]   line 100. destruct:
[2020-10-07 13:59:40.304234] [0x0000000107d9edc0] [trace]   line 31. copy construct: @0x7ffeed644df0
[2020-10-07 13:59:40.304255] [0x0000000107d9edc0] [trace]   line 27. construct: update@0x7faa5c407408
[2020-10-07 13:59:40.304264] [0x0000000107d9edc0] [trace]   line 112. setData: update
[2020-10-07 13:59:40.304281] [0x0000000107d9edc0] [trace]   line 19. sharedPtr=0x7faa5c407408, 2, command=update: user=alice

[2020-10-07 13:59:40.304289] [0x0000000107d9edc0] [trace]   line 20. sharedPtr2=0x7faa5c407408, 2, command=update: user=alice

[2020-10-07 13:59:40.304295] [0x0000000107d9edc0] [trace]   line 100. destruct: update
[2020-10-07 13:59:40.304315] [0x0000000107d9edc0] [trace]   line 100. destruct: c1
[2020-10-07 13:59:40.304319] [0x0000000107d9edc0] [trace]   line 100. destruct: c1
[2020-10-07 13:59:40.304327] [0x0000000107d9edc0] [trace]   * ExampleRunner destruct:
```

# 参考资料

* https://www.modernescpp.com/index.php/what-is-modern-c
* https://github.com/lefticus/cppbestpractices
* https://github.com/isocpp/CppCoreGuidelines
