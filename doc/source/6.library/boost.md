# Boost

Boost 是 C++ 最重要的准标准库集合，很多特性后来被纳入标准。

## 概述

Boost 提供了大量高质量的 C++ 库，涵盖以下领域：

| 类别 | 主要库 | 说明 |
|------|--------|------|
| 网络/异步 | [Asio](boost_asio.md), [Beast](boost_beast.md) | 异步 I/O、HTTP/WebSocket |
| 容器/数据 | Container, Multi-index | 高级容器 |
| 并发 | Thread, Fiber, Coroutine | 多线程支持 |
| 文本处理 | Regex, Spirit, Format | 正则、解析器 |
| 文件系统 | Filesystem | 文件操作 (C++17 标准化) |
| 智能指针 | Smart_ptr | 智能指针 (C++11 标准化) |
| 序列化 | Serialization, JSON | 数据序列化 |
| 测试 | Test | 单元测试框架 |

## 安装

```bash
# Ubuntu
sudo apt install libboost-all-dev

# macOS
brew install boost

# 使用 Conan
# conanfile.txt
[requires]
boost/1.83.0
```

## Boost.Filesystem

文件系统操作（C++17 已纳入标准）。

```cpp
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

void filesystem_example() {
    fs::path p("/home/user/file.txt");
    
    // 路径操作
    std::cout << p.filename() << "\n";      // file.txt
    std::cout << p.extension() << "\n";     // .txt
    std::cout << p.parent_path() << "\n";   // /home/user
    
    // 文件操作
    if (fs::exists(p)) {
        std::cout << "Size: " << fs::file_size(p) << "\n";
    }
    
    // 目录遍历
    for (const auto& entry : fs::directory_iterator("/path")) {
        std::cout << entry.path() << "\n";
    }
    
    // 创建目录
    fs::create_directories("/path/to/dir");
}
```

## Boost.Optional

可选值（C++17 std::optional）。

```cpp
#include <boost/optional.hpp>

boost::optional<int> find_value(int key) {
    if (/* found */) return value;
    return boost::none;
}

void use_optional() {
    auto result = find_value(42);
    
    if (result) {
        std::cout << *result << "\n";
    }
    
    // 带默认值
    int value = result.value_or(-1);
}
```

## Boost.Variant

类型安全的联合体（C++17 std::variant）。

```cpp
#include <boost/variant.hpp>

using Value = boost::variant<int, double, std::string>;

void variant_example() {
    Value v = 42;
    v = 3.14;
    v = "hello";
    
    // 访问
    if (boost::get<std::string>(&v)) {
        std::cout << boost::get<std::string>(v) << "\n";
    }
    
    // 访问者模式
    struct Visitor : boost::static_visitor<void> {
        void operator()(int i) const { std::cout << "int: " << i; }
        void operator()(double d) const { std::cout << "double: " << d; }
        void operator()(const std::string& s) const { std::cout << "string: " << s; }
    };
    
    boost::apply_visitor(Visitor(), v);
}
```

## Boost.Regex

正则表达式（C++11 std::regex）。

```cpp
#include <boost/regex.hpp>

void regex_example() {
    boost::regex pattern(R"(\d{3}-\d{4})");
    std::string text = "Phone: 123-4567";
    
    // 搜索
    boost::smatch match;
    if (boost::regex_search(text, match, pattern)) {
        std::cout << "Found: " << match[0] << "\n";
    }
    
    // 替换
    std::string result = boost::regex_replace(text, pattern, "XXX-XXXX");
}
```

## Boost.Lexical_Cast

类型转换。

```cpp
#include <boost/lexical_cast.hpp>

void cast_example() {
    // 字符串到数字
    int i = boost::lexical_cast<int>("42");
    double d = boost::lexical_cast<double>("3.14");
    
    // 数字到字符串
    std::string s = boost::lexical_cast<std::string>(42);
    
    // 错误处理
    try {
        int bad = boost::lexical_cast<int>("not a number");
    } catch (const boost::bad_lexical_cast& e) {
        std::cerr << e.what() << "\n";
    }
}
```

## Boost.Format

格式化字符串。

```cpp
#include <boost/format.hpp>

void format_example() {
    std::string s = (boost::format("Hello, %1%! You have %2% messages.") 
                     % "John" % 5).str();
    
    // 类似 printf
    std::cout << boost::format("%1$+5d %2$10.3f") % 42 % 3.14159;
}
```

## Boost.Program_options

命令行参数解析。

```cpp
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", po::value<std::string>(), "input file")
        ("verbose,v", po::bool_switch()->default_value(false), "verbose mode");
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }
    
    if (vm.count("input")) {
        std::cout << "Input: " << vm["input"].as<std::string>() << "\n";
    }
}
```

## Boost.Test

单元测试框架。

```cpp
#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_addition) {
    BOOST_CHECK(1 + 1 == 2);
    BOOST_CHECK_EQUAL(2 * 2, 4);
    BOOST_CHECK_CLOSE(1.0 / 3.0, 0.333333, 0.001);  // 允许 0.001% 误差
}

BOOST_AUTO_TEST_SUITE(MathTests)

BOOST_AUTO_TEST_CASE(test_multiply) {
    BOOST_REQUIRE(3 * 3 == 9);
}

BOOST_AUTO_TEST_SUITE_END()
```

```{tip}
Boost 使用建议：
1. 优先使用标准库中已有的功能
2. 只引入需要的 Boost 库
3. 考虑 header-only 库减少编译依赖
4. 关注 Boost 库迁移到标准的路径
```

## 相关文档

- [Boost.Asio](boost_asio.md) - 异步 I/O 库详解
- [Boost.Beast](boost_beast.md) - HTTP/WebSocket 库详解
