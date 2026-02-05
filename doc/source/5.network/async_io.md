# 异步 I/O

## I/O 多路复用

### select

```cpp
#include <sys/select.h>

void select_example(std::vector<int>& sockets) {
    fd_set read_fds;
    int max_fd = 0;
    
    FD_ZERO(&read_fds);
    for (int sock : sockets) {
        FD_SET(sock, &read_fds);
        max_fd = std::max(max_fd, sock);
    }
    
    timeval timeout = {5, 0};  // 5 秒超时
    int ready = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
    
    if (ready > 0) {
        for (int sock : sockets) {
            if (FD_ISSET(sock, &read_fds)) {
                // sock 可读
                handle_socket(sock);
            }
        }
    }
}
```

### poll

```cpp
#include <poll.h>

void poll_example(std::vector<int>& sockets) {
    std::vector<pollfd> fds;
    for (int sock : sockets) {
        fds.push_back({sock, POLLIN, 0});
    }
    
    int ready = poll(fds.data(), fds.size(), 5000);  // 5000ms 超时
    
    if (ready > 0) {
        for (const auto& pfd : fds) {
            if (pfd.revents & POLLIN) {
                handle_socket(pfd.fd);
            }
            if (pfd.revents & (POLLERR | POLLHUP)) {
                // 错误或断开
            }
        }
    }
}
```

### epoll (Linux)

```cpp
#include <sys/epoll.h>

class Epoll {
    int epfd_;
    
public:
    Epoll() {
        epfd_ = epoll_create1(0);
        if (epfd_ < 0) {
            throw std::system_error(errno, std::system_category());
        }
    }
    
    ~Epoll() {
        close(epfd_);
    }
    
    void add(int fd, uint32_t events = EPOLLIN) {
        epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
            throw std::system_error(errno, std::system_category());
        }
    }
    
    void modify(int fd, uint32_t events) {
        epoll_event ev;
        ev.events = events;
        ev.data.fd = fd;
        epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
    }
    
    void remove(int fd) {
        epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
    }
    
    int wait(epoll_event* events, int max_events, int timeout_ms) {
        return epoll_wait(epfd_, events, max_events, timeout_ms);
    }
};

// 事件循环
void event_loop() {
    Epoll ep;
    ep.add(server_fd, EPOLLIN);
    
    epoll_event events[64];
    while (running) {
        int n = ep.wait(events, 64, 1000);
        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == server_fd) {
                // 新连接
                int client = accept(server_fd, ...);
                ep.add(client, EPOLLIN | EPOLLET);  // 边缘触发
            } else {
                handle_client(events[i].data.fd);
            }
        }
    }
}
```

## Boost.Asio

### 同步操作

```cpp
#include <boost/asio.hpp>

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

void sync_client() {
    asio::io_context io;
    tcp::socket socket(io);
    
    // 连接
    tcp::resolver resolver(io);
    auto endpoints = resolver.resolve("example.com", "80");
    asio::connect(socket, endpoints);
    
    // 发送
    std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    asio::write(socket, asio::buffer(request));
    
    // 接收
    asio::streambuf response;
    asio::read_until(socket, response, "\r\n");
}
```

### 异步操作

```cpp
class AsyncClient {
    asio::io_context& io_;
    tcp::socket socket_;
    asio::streambuf buffer_;
    
public:
    AsyncClient(asio::io_context& io) : io_(io), socket_(io) {}
    
    void connect(const std::string& host, const std::string& port) {
        tcp::resolver resolver(io_);
        auto endpoints = resolver.resolve(host, port);
        
        asio::async_connect(socket_, endpoints,
            [this](boost::system::error_code ec, tcp::endpoint) {
                if (!ec) {
                    do_write();
                }
            });
    }
    
private:
    void do_write() {
        std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        asio::async_write(socket_, asio::buffer(request),
            [this](boost::system::error_code ec, size_t) {
                if (!ec) {
                    do_read();
                }
            });
    }
    
    void do_read() {
        asio::async_read_until(socket_, buffer_, "\r\n",
            [this](boost::system::error_code ec, size_t) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    std::cout << line << "\n";
                }
            });
    }
};

// 使用
asio::io_context io;
AsyncClient client(io);
client.connect("example.com", "80");
io.run();  // 事件循环
```

### 协程风格 (C++20)

```cpp
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/use_awaitable.hpp>

asio::awaitable<void> async_client() {
    auto executor = co_await asio::this_coro::executor;
    tcp::socket socket(executor);
    
    tcp::resolver resolver(executor);
    auto endpoints = co_await resolver.async_resolve(
        "example.com", "80", asio::use_awaitable);
    
    co_await asio::async_connect(socket, endpoints, asio::use_awaitable);
    
    std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    co_await asio::async_write(socket, asio::buffer(request), 
                               asio::use_awaitable);
    
    asio::streambuf buffer;
    co_await asio::async_read_until(socket, buffer, "\r\n",
                                    asio::use_awaitable);
}

// 使用
asio::io_context io;
asio::co_spawn(io, async_client(), asio::detached);
io.run();
```

## libuv

```cpp
#include <uv.h>

void on_connect(uv_connect_t* req, int status) {
    if (status < 0) {
        fprintf(stderr, "Connect error: %s\n", uv_strerror(status));
        return;
    }
    // 连接成功
}

void libuv_example() {
    uv_loop_t* loop = uv_default_loop();
    
    uv_tcp_t socket;
    uv_tcp_init(loop, &socket);
    
    struct sockaddr_in dest;
    uv_ip4_addr("127.0.0.1", 8080, &dest);
    
    uv_connect_t connect_req;
    uv_tcp_connect(&connect_req, &socket, 
                   (const struct sockaddr*)&dest, on_connect);
    
    uv_run(loop, UV_RUN_DEFAULT);
    uv_loop_close(loop);
}
```

## Reactor vs Proactor

### Reactor 模式

```
1. 注册事件
2. 等待事件就绪
3. 分发事件
4. 处理程序同步处理

特点：事件处理程序负责 I/O 操作
示例：epoll, select, libevent
```

### Proactor 模式

```
1. 发起异步 I/O 操作
2. 系统完成 I/O
3. 通知完成
4. 处理程序处理结果

特点：系统完成 I/O，处理程序只处理结果
示例：Windows IOCP, Boost.Asio (Windows)
```

```{tip}
异步 I/O 选择建议：
- **Linux 高性能**: epoll + 非阻塞 I/O
- **跨平台**: Boost.Asio 或 libuv
- **简单场景**: select/poll
- **现代 C++**: Asio + C++20 协程
```
