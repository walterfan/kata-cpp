# TCP 与 UDP

## TCP vs UDP

| 特性 | TCP | UDP |
|------|-----|-----|
| 连接 | 面向连接 | 无连接 |
| 可靠性 | 可靠，有序 | 不可靠 |
| 速度 | 较慢 | 较快 |
| 头部开销 | 20+ 字节 | 8 字节 |
| 用途 | HTTP, FTP, SSH | DNS, VoIP, 游戏 |

## UDP 服务器

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class UdpServer {
    int sock_;
    
public:
    UdpServer(int port) {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("socket() failed");
        }
        
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        
        if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(sock_);
            throw std::runtime_error("bind() failed");
        }
    }
    
    ~UdpServer() {
        close(sock_);
    }
    
    void run() {
        char buffer[1024];
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        while (true) {
            // 接收数据
            ssize_t n = recvfrom(sock_, buffer, sizeof(buffer) - 1, 0,
                                 (sockaddr*)&client_addr, &client_len);
            if (n > 0) {
                buffer[n] = '\0';
                
                // 发送响应
                sendto(sock_, buffer, n, 0,
                       (sockaddr*)&client_addr, client_len);
            }
        }
    }
};
```

## UDP 客户端

```cpp
class UdpClient {
    int sock_;
    sockaddr_in server_addr_;
    
public:
    UdpClient(const char* host, int port) {
        sock_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("socket() failed");
        }
        
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port);
        inet_pton(AF_INET, host, &server_addr_.sin_addr);
    }
    
    ~UdpClient() {
        close(sock_);
    }
    
    void send(const std::string& msg) {
        sendto(sock_, msg.c_str(), msg.size(), 0,
               (sockaddr*)&server_addr_, sizeof(server_addr_));
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

## 多播 (Multicast)

```cpp
// 加入多播组
struct ip_mreq mreq{};
inet_pton(AF_INET, "239.0.0.1", &mreq.imr_multiaddr);
mreq.imr_interface.s_addr = INADDR_ANY;
setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

// 离开多播组
setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

// 设置 TTL
int ttl = 2;
setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

// 禁用回环
int loop = 0;
setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
```

## 广播 (Broadcast)

```cpp
// 启用广播
int opt = 1;
setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));

// 发送广播
sockaddr_in addr{};
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
addr.sin_addr.s_addr = INADDR_BROADCAST;  // 255.255.255.255

sendto(sock, data, len, 0, (sockaddr*)&addr, sizeof(addr));
```

## TCP 粘包问题

TCP 是流协议，没有消息边界，需要自己处理分包。

### 定长消息

```cpp
struct Message {
    uint32_t type;
    char data[256];
};

// 发送
Message msg;
// 填充 msg...
send(sock, &msg, sizeof(msg), 0);

// 接收
Message msg;
size_t total = 0;
while (total < sizeof(msg)) {
    ssize_t n = recv(sock, (char*)&msg + total, sizeof(msg) - total, 0);
    if (n <= 0) break;
    total += n;
}
```

### 长度前缀

```cpp
// 发送
void send_message(int sock, const std::string& data) {
    uint32_t len = htonl(data.size());
    send(sock, &len, sizeof(len), 0);
    send(sock, data.c_str(), data.size(), 0);
}

// 接收
std::string recv_message(int sock) {
    uint32_t len;
    recv(sock, &len, sizeof(len), MSG_WAITALL);
    len = ntohl(len);
    
    std::string data(len, '\0');
    recv(sock, data.data(), len, MSG_WAITALL);
    return data;
}
```

### 分隔符

```cpp
// 接收直到遇到分隔符（如 \r\n）
std::string recv_line(int sock) {
    std::string line;
    char c;
    while (recv(sock, &c, 1, 0) == 1) {
        if (c == '\n') break;
        if (c != '\r') line += c;
    }
    return line;
}
```

## 高效数据传输

### scatter/gather I/O

```cpp
#include <sys/uio.h>

// 分散读取
char header[10];
char body[100];
iovec iov[2] = {
    {header, sizeof(header)},
    {body, sizeof(body)}
};
ssize_t n = readv(sock, iov, 2);

// 聚集写入
ssize_t n = writev(sock, iov, 2);
```

### sendfile (零拷贝)

```cpp
#include <sys/sendfile.h>

// 直接从文件发送到 socket
int file_fd = open("data.bin", O_RDONLY);
off_t offset = 0;
ssize_t n = sendfile(sock, file_fd, &offset, file_size);
close(file_fd);
```

## 错误处理

```cpp
#include <cerrno>
#include <cstring>

void handle_error(const char* msg) {
    std::cerr << msg << ": " << strerror(errno) << "\n";
}

// 常见错误码
// EAGAIN / EWOULDBLOCK - 非阻塞操作无数据
// EINTR - 被信号中断
// EPIPE - 对方关闭连接
// ECONNRESET - 连接被重置
// ECONNREFUSED - 连接被拒绝
// ETIMEDOUT - 连接超时

// 忽略 SIGPIPE
signal(SIGPIPE, SIG_IGN);
// 或使用 MSG_NOSIGNAL
send(sock, data, len, MSG_NOSIGNAL);
```

```{tip}
TCP/UDP 编程要点：
1. TCP 需要处理粘包
2. UDP 需要处理丢包和乱序
3. 使用非阻塞 I/O + I/O 多路复用处理高并发
4. 考虑使用现有库（Boost.Asio, libuv）
```
