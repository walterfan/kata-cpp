# Socket 基础

## Socket 概念

Socket 是网络通信的端点，提供了进程间通信的机制。

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// 创建 Socket
int sock = socket(AF_INET,      // IPv4
                  SOCK_STREAM,   // TCP
                  0);            // 协议（0 = 自动选择）

// Socket 类型
// SOCK_STREAM - TCP，面向连接，可靠
// SOCK_DGRAM  - UDP，无连接，不可靠
// SOCK_RAW    - 原始套接字
```

## TCP 服务器

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

class TcpServer {
    int server_fd_;
    
public:
    TcpServer(int port) {
        // 1. 创建 socket
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ < 0) {
            throw std::runtime_error("socket() failed");
        }
        
        // 设置 SO_REUSEADDR
        int opt = 1;
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        
        // 2. 绑定地址
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        if (bind(server_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(server_fd_);
            throw std::runtime_error("bind() failed");
        }
        
        // 3. 监听
        if (listen(server_fd_, 10) < 0) {
            close(server_fd_);
            throw std::runtime_error("listen() failed");
        }
    }
    
    ~TcpServer() {
        close(server_fd_);
    }
    
    void run() {
        while (true) {
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            
            // 4. 接受连接
            int client_fd = accept(server_fd_, 
                                   (sockaddr*)&client_addr, 
                                   &client_len);
            if (client_fd < 0) continue;
            
            // 处理客户端
            handle_client(client_fd);
            close(client_fd);
        }
    }
    
private:
    void handle_client(int client_fd) {
        char buffer[1024];
        ssize_t n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            std::cout << "Received: " << buffer << "\n";
            
            const char* response = "Hello from server!";
            send(client_fd, response, strlen(response), 0);
        }
    }
};
```

## TCP 客户端

```cpp
class TcpClient {
    int sock_;
    
public:
    TcpClient(const char* host, int port) {
        // 1. 创建 socket
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("socket() failed");
        }
        
        // 2. 连接服务器
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, host, &addr.sin_addr);
        
        if (connect(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock_);
            throw std::runtime_error("connect() failed");
        }
    }
    
    ~TcpClient() {
        close(sock_);
    }
    
    void send_message(const std::string& msg) {
        send(sock_, msg.c_str(), msg.size(), 0);
    }
    
    std::string receive() {
        char buffer[1024];
        ssize_t n = recv(sock_, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            return std::string(buffer);
        }
        return "";
    }
};
```

## 非阻塞 Socket

```cpp
#include <fcntl.h>

// 设置非阻塞
int flags = fcntl(sock, F_GETFL, 0);
fcntl(sock, F_SETFL, flags | O_NONBLOCK);

// 非阻塞 connect
int result = connect(sock, (sockaddr*)&addr, sizeof(addr));
if (result < 0 && errno == EINPROGRESS) {
    // 连接进行中，使用 select/poll/epoll 等待
}

// 非阻塞 recv
ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // 没有数据可读，稍后重试
    }
}
```

## Socket 选项

```cpp
// 地址重用
int opt = 1;
setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

// 保持连接
setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

// 发送/接收超时
struct timeval tv;
tv.tv_sec = 5;
tv.tv_usec = 0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

// 发送/接收缓冲区大小
int bufsize = 65536;
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &bufsize, sizeof(bufsize));
setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));

// TCP_NODELAY (禁用 Nagle 算法)
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
```

## RAII Socket 封装

```cpp
class Socket {
    int fd_ = -1;
    
public:
    Socket() = default;
    
    explicit Socket(int domain, int type, int protocol = 0) {
        fd_ = socket(domain, type, protocol);
        if (fd_ < 0) {
            throw std::system_error(errno, std::system_category());
        }
    }
    
    ~Socket() {
        if (fd_ >= 0) {
            close(fd_);
        }
    }
    
    // 禁止拷贝
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    
    // 允许移动
    Socket(Socket&& other) noexcept : fd_(other.fd_) {
        other.fd_ = -1;
    }
    
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            if (fd_ >= 0) close(fd_);
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }
    
    int get() const { return fd_; }
    int release() { 
        int tmp = fd_; 
        fd_ = -1; 
        return tmp; 
    }
};
```

## 地址解析

```cpp
#include <netdb.h>

// 域名解析
std::vector<std::string> resolve_hostname(const char* hostname) {
    std::vector<std::string> addresses;
    
    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;      // IPv4 或 IPv6
    hints.ai_socktype = SOCK_STREAM;
    
    addrinfo* result;
    int ret = getaddrinfo(hostname, nullptr, &hints, &result);
    if (ret != 0) {
        throw std::runtime_error(gai_strerror(ret));
    }
    
    for (addrinfo* p = result; p != nullptr; p = p->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        if (p->ai_family == AF_INET) {
            auto* addr = (sockaddr_in*)p->ai_addr;
            inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
        } else {
            auto* addr = (sockaddr_in6*)p->ai_addr;
            inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
        }
        addresses.push_back(ip);
    }
    
    freeaddrinfo(result);
    return addresses;
}
```

```{tip}
Socket 编程要点：
1. 总是检查返回值和 errno
2. 使用 RAII 管理文件描述符
3. 设置合适的 Socket 选项
4. 处理信号（如 SIGPIPE）
5. 考虑超时和重试
```
