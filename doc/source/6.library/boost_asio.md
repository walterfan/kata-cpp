# Boost.Asio

Boost.Asio 是一个跨平台的 C++ 异步 I/O 库，提供了一致的异步模型用于网络编程和其他底层 I/O 操作。

## 概述

### 核心概念

| 概念 | 说明 |
|------|------|
| `io_context` | I/O 服务，事件循环的核心 |
| `executor` | 执行器，控制异步操作的执行方式 |
| `buffer` | 缓冲区，数据传输的载体 |
| `strand` | 串行化执行器，保证回调顺序执行 |
| `completion handler` | 完成处理器，异步操作完成时的回调 |

### 异步模型

Asio 支持多种异步模型：

- **回调风格** - 传统的异步回调
- **Future 风格** - 使用 `std::future` 获取结果
- **协程风格** - C++20 协程 (`co_await`)
- **Stackful 协程** - `boost::asio::spawn`

## 基础用法

### io_context 事件循环

```cpp
#include <boost/asio.hpp>
#include <iostream>

namespace asio = boost::asio;

int main() {
    asio::io_context io;
    
    // 添加一些工作
    asio::post(io, []() {
        std::cout << "Task 1\n";
    });
    
    asio::post(io, []() {
        std::cout << "Task 2\n";
    });
    
    // 运行事件循环
    io.run();  // 阻塞直到所有工作完成
    
    return 0;
}
```

### 定时器

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <chrono>

namespace asio = boost::asio;

// 同步定时器
void sync_timer() {
    asio::io_context io;
    asio::steady_timer timer(io, std::chrono::seconds(2));
    
    std::cout << "Waiting...\n";
    timer.wait();  // 阻塞 2 秒
    std::cout << "Done!\n";
}

// 异步定时器
void async_timer() {
    asio::io_context io;
    asio::steady_timer timer(io, std::chrono::seconds(2));
    
    timer.async_wait([](const boost::system::error_code& ec) {
        if (!ec) {
            std::cout << "Timer expired!\n";
        }
    });
    
    std::cout << "Timer started...\n";
    io.run();  // 运行事件循环
}

// 周期性定时器
class PeriodicTimer {
    asio::steady_timer timer_;
    int count_ = 0;
    
public:
    PeriodicTimer(asio::io_context& io)
        : timer_(io, std::chrono::seconds(1)) {
        wait();
    }
    
    void wait() {
        timer_.async_wait([this](const boost::system::error_code& ec) {
            if (!ec) {
                std::cout << "Tick " << ++count_ << "\n";
                if (count_ < 5) {
                    timer_.expires_after(std::chrono::seconds(1));
                    wait();
                }
            }
        });
    }
};
```

## TCP 网络编程

### TCP 客户端

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// 同步 TCP 客户端
void sync_tcp_client() {
    asio::io_context io;
    tcp::resolver resolver(io);
    tcp::socket socket(io);
    
    // 解析地址
    auto endpoints = resolver.resolve("example.com", "80");
    
    // 连接
    asio::connect(socket, endpoints);
    
    // 发送请求
    std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    asio::write(socket, asio::buffer(request));
    
    // 读取响应
    std::string response;
    boost::system::error_code ec;
    asio::read(socket, asio::dynamic_buffer(response), ec);
    
    if (ec != asio::error::eof) {
        throw boost::system::system_error(ec);
    }
    
    std::cout << response << "\n";
}

// 异步 TCP 客户端
class AsyncTcpClient {
    tcp::resolver resolver_;
    tcp::socket socket_;
    std::string request_;
    std::string response_;
    
public:
    AsyncTcpClient(asio::io_context& io, 
                   const std::string& host, 
                   const std::string& port)
        : resolver_(io), socket_(io) {
        
        request_ = "GET / HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
        
        resolver_.async_resolve(host, port,
            [this](const boost::system::error_code& ec,
                   tcp::resolver::results_type results) {
                if (!ec) {
                    do_connect(results);
                }
            });
    }
    
private:
    void do_connect(tcp::resolver::results_type results) {
        asio::async_connect(socket_, results,
            [this](const boost::system::error_code& ec, 
                   const tcp::endpoint&) {
                if (!ec) {
                    do_write();
                }
            });
    }
    
    void do_write() {
        asio::async_write(socket_, asio::buffer(request_),
            [this](const boost::system::error_code& ec, std::size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }
    
    void do_read() {
        asio::async_read(socket_, asio::dynamic_buffer(response_),
            [this](const boost::system::error_code& ec, std::size_t) {
                if (ec == asio::error::eof) {
                    std::cout << response_ << "\n";
                }
            });
    }
};
```

### TCP 服务器

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <string>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// 处理单个连接的会话
class Session : public std::enable_shared_from_this<Session> {
    tcp::socket socket_;
    std::array<char, 1024> buffer_;
    
public:
    Session(tcp::socket socket)
        : socket_(std::move(socket)) {}
    
    void start() {
        do_read();
    }
    
private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(asio::buffer(buffer_),
            [this, self](const boost::system::error_code& ec, 
                         std::size_t length) {
                if (!ec) {
                    // Echo 回写
                    do_write(length);
                }
            });
    }
    
    void do_write(std::size_t length) {
        auto self = shared_from_this();
        asio::async_write(socket_, asio::buffer(buffer_, length),
            [this, self](const boost::system::error_code& ec, std::size_t) {
                if (!ec) {
                    do_read();  // 继续读取
                }
            });
    }
};

// TCP 服务器
class TcpServer {
    tcp::acceptor acceptor_;
    
public:
    TcpServer(asio::io_context& io, unsigned short port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
    
private:
    void do_accept() {
        acceptor_.async_accept(
            [this](const boost::system::error_code& ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();  // 继续接受连接
            });
    }
};

int main() {
    asio::io_context io;
    TcpServer server(io, 8080);
    
    std::cout << "Server running on port 8080...\n";
    io.run();
    
    return 0;
}
```

## UDP 网络编程

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <array>

namespace asio = boost::asio;
using udp = asio::ip::udp;

// UDP 服务器
class UdpServer {
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> recv_buffer_;
    
public:
    UdpServer(asio::io_context& io, unsigned short port)
        : socket_(io, udp::endpoint(udp::v4(), port)) {
        do_receive();
    }
    
private:
    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(recv_buffer_), remote_endpoint_,
            [this](const boost::system::error_code& ec, std::size_t bytes) {
                if (!ec && bytes > 0) {
                    std::string msg(recv_buffer_.data(), bytes);
                    std::cout << "Received: " << msg << "\n";
                    
                    // Echo 回复
                    do_send(bytes);
                } else {
                    do_receive();
                }
            });
    }
    
    void do_send(std::size_t length) {
        socket_.async_send_to(
            asio::buffer(recv_buffer_, length), remote_endpoint_,
            [this](const boost::system::error_code& ec, std::size_t) {
                do_receive();
            });
    }
};

// UDP 客户端
void udp_client() {
    asio::io_context io;
    udp::socket socket(io, udp::endpoint(udp::v4(), 0));
    
    udp::resolver resolver(io);
    udp::endpoint endpoint = *resolver.resolve("localhost", "8080").begin();
    
    std::string message = "Hello, UDP!";
    socket.send_to(asio::buffer(message), endpoint);
    
    std::array<char, 1024> recv_buffer;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(asio::buffer(recv_buffer), sender_endpoint);
    
    std::cout << "Response: " << std::string(recv_buffer.data(), len) << "\n";
}
```

## Strand - 同步执行

当多个线程运行 `io_context::run()` 时，使用 `strand` 保证回调串行执行。

```cpp
#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>

namespace asio = boost::asio;

class Worker {
    asio::io_context& io_;
    asio::strand<asio::io_context::executor_type> strand_;
    int counter_ = 0;
    
public:
    Worker(asio::io_context& io)
        : io_(io), strand_(asio::make_strand(io)) {}
    
    void post_work() {
        // 使用 strand 保证这些操作串行执行
        asio::post(strand_, [this]() {
            ++counter_;
            std::cout << "Counter: " << counter_ 
                      << " (thread: " << std::this_thread::get_id() << ")\n";
        });
    }
    
    int get_counter() const { return counter_; }
};

int main() {
    asio::io_context io;
    Worker worker(io);
    
    // 提交多个任务
    for (int i = 0; i < 10; ++i) {
        worker.post_work();
    }
    
    // 多线程运行
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&io]() { io.run(); });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Final counter: " << worker.get_counter() << "\n";
    
    return 0;
}
```

## C++20 协程支持

```cpp
#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <iostream>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

// 协程风格的 Echo 服务器
asio::awaitable<void> echo_session(tcp::socket socket) {
    try {
        std::array<char, 1024> buffer;
        for (;;) {
            std::size_t n = co_await socket.async_read_some(
                asio::buffer(buffer), asio::use_awaitable);
            
            co_await asio::async_write(socket, 
                asio::buffer(buffer, n), asio::use_awaitable);
        }
    } catch (std::exception& e) {
        std::cout << "Session error: " << e.what() << "\n";
    }
}

asio::awaitable<void> listener(tcp::acceptor acceptor) {
    for (;;) {
        tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
        
        asio::co_spawn(acceptor.get_executor(),
            echo_session(std::move(socket)),
            asio::detached);
    }
}

int main() {
    asio::io_context io;
    
    tcp::acceptor acceptor(io, {tcp::v4(), 8080});
    
    asio::co_spawn(io, listener(std::move(acceptor)), asio::detached);
    
    std::cout << "Server running on port 8080...\n";
    io.run();
    
    return 0;
}
```

## SSL/TLS 支持

```cpp
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

namespace asio = boost::asio;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

void https_client() {
    asio::io_context io;
    ssl::context ctx(ssl::context::tlsv12_client);
    
    // 加载系统证书
    ctx.set_default_verify_paths();
    ctx.set_verify_mode(ssl::verify_peer);
    
    tcp::resolver resolver(io);
    ssl::stream<tcp::socket> socket(io, ctx);
    
    // 设置 SNI
    SSL_set_tlsext_host_name(socket.native_handle(), "example.com");
    
    // 连接
    auto endpoints = resolver.resolve("example.com", "443");
    asio::connect(socket.lowest_layer(), endpoints);
    
    // SSL 握手
    socket.handshake(ssl::stream_base::client);
    
    // 发送请求
    std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    asio::write(socket, asio::buffer(request));
    
    // 读取响应
    std::string response;
    boost::system::error_code ec;
    asio::read(socket, asio::dynamic_buffer(response), ec);
    
    std::cout << response << "\n";
    
    // 关闭
    socket.shutdown(ec);
}
```

## 最佳实践

```{tip}
**Asio 使用建议：**

1. **避免阻塞操作** - 在异步处理器中避免耗时操作
2. **使用 strand** - 多线程环境下保护共享数据
3. **正确管理生命周期** - 使用 `shared_from_this()` 保证对象存活
4. **处理所有错误** - 检查每个操作的 error_code
5. **使用协程** - C++20 协程让代码更清晰
```

## 相关文档

- [Boost.Beast](boost_beast.md) - 基于 Asio 的 HTTP/WebSocket 库
- [Boost 库概览](boost.md) - 回到 Boost 主页
