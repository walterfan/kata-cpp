# 现代 C++ 实战开发指南

本文档使用 Sphinx 构建，专注于 C++ 程序员易错和易忽略的知识点，尤其是现代 C++ 引入的特性。

## 内容概览

1. **基础知识与陷阱** - 现代 C++ 核心概念，常见陷阱与解决方案
2. **并发编程** - std::thread、async、mutex、atomic、协程详解
3. **性能调优** - 性能分析、优化技巧、基准测试
4. **内存管理** - 智能指针、RAII、内存布局、泄漏检测
5. **网络编程** - Socket、TCP/UDP、HTTP 客户端、异步 I/O
6. **常用库** - GoogleTest、JsonCpp、libcurl、Boost、OpenSSL
7. **速查表** - 语法、STL、命令速查

## 快速开始

### 安装依赖

```bash
uv sync
```

### 构建文档

```bash
# 构建 HTML
make html

# 或使用快速构建
make quick

# 实时预览（自动重建）
make livehtml
```

### 查看文档

```bash
# 启动本地服务器
make serve

# 然后访问 http://localhost:8000
```

## 目录结构

```
doc/
├── source/
│   ├── conf.py           # Sphinx 配置
│   ├── index.rst         # 主页
│   ├── 1.basic/          # 基础知识
│   ├── 2.concurrency/    # 并发编程
│   ├── 3.performance/    # 性能调优
│   ├── 4.memory/         # 内存管理
│   ├── 5.network/        # 网络编程
│   ├── 6.library/        # 常用库
│   ├── 7.cheatsheet/     # 速查表
│   └── _static/          # 静态文件
├── Makefile
├── requirements.txt
└── README.md
```

## 贡献

欢迎提交 Issue 和 Pull Request！

## 许可证

CC BY-NC-ND 4.0
