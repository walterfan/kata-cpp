# Boost.Beast

Boost.Beast 是建立在 Boost.Asio 之上的 HTTP 和 WebSocket 库，提供了现代 C++ 风格的网络编程接口。

## 概述

### 特性

- **HTTP/1.1** 完整支持（请求、响应、流式处理）
- **WebSocket** 完整支持（客户端和服务器）
- **SSL/TLS** 支持（通过 Boost.Asio SSL）
- **零拷贝** 设计，高性能
- **协程支持** (C++20 coroutines)

### 核心组件

| 组件 | 说明 |
|------|------|
| `http::request` | HTTP 请求消息 |
| `http::response` | HTTP 响应消息 |
| `websocket::stream` | WebSocket 流 |
| `flat_buffer` | 动态缓冲区 |
| `tcp_stream` | 带超时的 TCP 流 |

## HTTP 编程

### HTTP 客户端 - 同步

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::string http_get(const std::string& host, const std::string& target) {
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);
    
    // 解析域名
    auto const results = resolver.resolve(host, "80");
    stream.connect(results);
    
    // 设置超时
    stream.expires_after(std::chrono::seconds(30));
    
    // 构建请求
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    
    // 发送请求
    http::write(stream, req);
    
    // 接收响应
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);
    
    // 关闭连接
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
    return res.body();
}
```

### HTTP 客户端 - POST 请求

```cpp
std::string http_post(const std::string& host, 
                      const std::string& target,
                      const std::string& body,
                      const std::string& content_type = "application/json") {
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);
    
    auto const results = resolver.resolve(host, "80");
    stream.connect(results);
    
    // 构建 POST 请求
    http::request<http::string_body> req{http::verb::post, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::content_type, content_type);
    req.body() = body;
    req.prepare_payload();  // 设置 Content-Length
    
    http::write(stream, req);
    
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);
    
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
    return res.body();
}
```

### HTTP 服务器

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

// 处理 HTTP 请求
template<class Body, class Allocator>
http::response<http::string_body>
handle_request(http::request<Body, http::basic_fields<Allocator>>&& req) {
    // 处理不同路径
    if (req.target() == "/") {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "<html><body><h1>Hello, Beast!</h1></body></html>";
        res.prepare_payload();
        return res;
    }
    
    if (req.target() == "/api/hello") {
        http::response<http::string_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "application/json");
        res.keep_alive(req.keep_alive());
        res.body() = R"({"message": "Hello, World!"})";
        res.prepare_payload();
        return res;
    }
    
    // 404
    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/plain");
    res.keep_alive(req.keep_alive());
    res.body() = "Not Found";
    res.prepare_payload();
    return res;
}

// HTTP 会话
class HttpSession : public std::enable_shared_from_this<HttpSession> {
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    
public:
    HttpSession(tcp::socket&& socket)
        : stream_(std::move(socket)) {}
    
    void run() {
        do_read();
    }
    
private:
    void do_read() {
        req_ = {};
        stream_.expires_after(std::chrono::seconds(30));
        
        http::async_read(stream_, buffer_, req_,
            beast::bind_front_handler(
                &HttpSession::on_read,
                shared_from_this()));
    }
    
    void on_read(beast::error_code ec, std::size_t) {
        if (ec == http::error::end_of_stream)
            return do_close();
        
        if (ec)
            return;
        
        auto response = handle_request(std::move(req_));
        auto sp = std::make_shared<http::response<http::string_body>>(std::move(response));
        
        http::async_write(stream_, *sp,
            [self = shared_from_this(), sp](beast::error_code ec, std::size_t) {
                self->on_write(ec, sp->need_eof());
            });
    }
    
    void on_write(beast::error_code ec, bool close) {
        if (ec)
            return;
        
        if (close)
            return do_close();
        
        do_read();
    }
    
    void do_close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }
};
```

## WebSocket 编程

### WebSocket 客户端

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void websocket_client(const std::string& host, const std::string& port) {
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    websocket::stream<tcp::socket> ws(ioc);
    
    // 连接
    auto const results = resolver.resolve(host, port);
    auto ep = net::connect(ws.next_layer(), results);
    
    // WebSocket 握手
    std::string host_port = host + ":" + std::to_string(ep.port());
    ws.handshake(host_port, "/");
    
    // 发送消息
    ws.write(net::buffer(std::string("Hello, WebSocket!")));
    
    // 接收消息
    beast::flat_buffer buffer;
    ws.read(buffer);
    std::cout << beast::make_printable(buffer.data()) << std::endl;
    
    // 关闭连接
    ws.close(websocket::close_code::normal);
}
```

### WebSocket 服务器

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <string>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    
public:
    WebSocketSession(tcp::socket&& socket)
        : ws_(std::move(socket)) {}
    
    void run() {
        // 设置选项
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));
        
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(http::field::server, "Beast WebSocket Server");
            }));
        
        // 接受 WebSocket 握手
        ws_.async_accept(
            beast::bind_front_handler(
                &WebSocketSession::on_accept,
                shared_from_this()));
    }
    
private:
    void on_accept(beast::error_code ec) {
        if (ec)
            return;
        
        do_read();
    }
    
    void do_read() {
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &WebSocketSession::on_read,
                shared_from_this()));
    }
    
    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        if (ec == websocket::error::closed)
            return;
        
        if (ec)
            return;
        
        // Echo 回写
        ws_.text(ws_.got_text());
        ws_.async_write(
            buffer_.data(),
            beast::bind_front_handler(
                &WebSocketSession::on_write,
                shared_from_this()));
    }
    
    void on_write(beast::error_code ec, std::size_t) {
        if (ec)
            return;
        
        buffer_.consume(buffer_.size());
        do_read();
    }
};
```

## 实战：多人聊天服务器

一个完整的 HTTP/WebSocket 多人聊天服务器示例，支持：
- HTTP 静态文件服务
- WebSocket 实时消息推送
- 多用户同时在线聊天
- 用户加入/离开通知

### 项目结构

```
chat_server/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── chat_server.hpp
│   ├── websocket_session.hpp
│   ├── http_session.hpp
│   └── shared_state.hpp
└── www/
    └── index.html
```

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(chat_server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.75 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_executable(chat_server
    src/main.cpp
)

target_link_libraries(chat_server
    Boost::system
    Threads::Threads
)

target_include_directories(chat_server PRIVATE src)
```

### shared_state.hpp - 共享状态管理

```cpp
#ifndef SHARED_STATE_HPP
#define SHARED_STATE_HPP

#include <memory>
#include <mutex>
#include <string>
#include <unordered_set>

class websocket_session;

class shared_state {
    std::string doc_root_;
    std::mutex mutex_;
    std::unordered_set<websocket_session*> sessions_;

public:
    explicit shared_state(std::string doc_root)
        : doc_root_(std::move(doc_root)) {}

    const std::string& doc_root() const noexcept {
        return doc_root_;
    }

    void join(websocket_session* session) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.insert(session);
    }

    void leave(websocket_session* session) {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_.erase(session);
    }

    void broadcast(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto* session : sessions_) {
            session->send(message);
        }
    }

    size_t session_count() const {
        std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(mutex_));
        return sessions_.size();
    }
};

#endif
```

### websocket_session.hpp - WebSocket 会话

```cpp
#ifndef WEBSOCKET_SESSION_HPP
#define WEBSOCKET_SESSION_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <queue>
#include <string>

#include "shared_state.hpp"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class websocket_session : public std::enable_shared_from_this<websocket_session> {
    websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer buffer_;
    std::shared_ptr<shared_state> state_;
    std::queue<std::string> queue_;
    std::string username_;

    void fail(beast::error_code ec, char const* what) {
        if (ec == net::error::operation_aborted ||
            ec == websocket::error::closed)
            return;
        std::cerr << what << ": " << ec.message() << "\n";
    }

    void on_accept(beast::error_code ec) {
        if (ec) return fail(ec, "accept");
        state_->join(this);
        do_read();
    }

    void do_read() {
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &websocket_session::on_read,
                shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t) {
        if (ec == websocket::error::closed) {
            state_->leave(this);
            if (!username_.empty()) {
                state_->broadcast("{\"type\":\"leave\",\"user\":\"" + 
                                  username_ + "\"}");
            }
            return;
        }
        if (ec) return fail(ec, "read");

        std::string msg = beast::buffers_to_string(buffer_.data());
        buffer_.consume(buffer_.size());

        if (username_.empty()) {
            username_ = msg;
            state_->broadcast("{\"type\":\"join\",\"user\":\"" + 
                              username_ + "\"}");
        } else {
            std::string broadcast_msg = "{\"type\":\"message\",\"user\":\"" + 
                                        username_ + "\",\"text\":\"" + msg + "\"}";
            state_->broadcast(broadcast_msg);
        }

        do_read();
    }

    void on_send(std::shared_ptr<std::string const> const& ss) {
        queue_.push(*ss);
        if (queue_.size() > 1) return;

        ws_.async_write(
            net::buffer(queue_.front()),
            beast::bind_front_handler(
                &websocket_session::on_write,
                shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t) {
        if (ec) return fail(ec, "write");
        queue_.pop();
        if (!queue_.empty()) {
            ws_.async_write(
                net::buffer(queue_.front()),
                beast::bind_front_handler(
                    &websocket_session::on_write,
                    shared_from_this()));
        }
    }

public:
    websocket_session(tcp::socket&& socket, 
                      std::shared_ptr<shared_state> const& state)
        : ws_(std::move(socket)), state_(state) {}

    ~websocket_session() {
        state_->leave(this);
    }

    void run() {
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res) {
                res.set(beast::http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) + " chat-server");
            }));

        ws_.async_accept(
            beast::bind_front_handler(
                &websocket_session::on_accept,
                shared_from_this()));
    }

    void send(std::string message) {
        auto ss = std::make_shared<std::string const>(std::move(message));
        net::post(
            ws_.get_executor(),
            beast::bind_front_handler(
                &websocket_session::on_send,
                shared_from_this(),
                ss));
    }
};

#endif
```

### http_session.hpp - HTTP 会话

```cpp
#ifndef HTTP_SESSION_HPP
#define HTTP_SESSION_HPP

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <string>
#include <fstream>
#include <sstream>

#include "shared_state.hpp"
#include "websocket_session.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

beast::string_view mime_type(beast::string_view path) {
    using beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if (pos == beast::string_view::npos)
            return beast::string_view{};
        return path.substr(pos);
    }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    return "application/octet-stream";
}

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

class http_session : public std::enable_shared_from_this<http_session> {
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    std::shared_ptr<shared_state> state_;
    http::request<http::string_body> req_;

    void fail(beast::error_code ec, char const* what) {
        if (ec == net::error::operation_aborted) return;
        std::cerr << what << ": " << ec.message() << "\n";
    }

    void do_read() {
        req_ = {};
        stream_.expires_after(std::chrono::seconds(30));
        http::async_read(stream_, buffer_, req_,
            beast::bind_front_handler(&http_session::on_read, shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t) {
        if (ec == http::error::end_of_stream)
            return do_close();
        if (ec) return fail(ec, "read");

        if (websocket::is_upgrade(req_)) {
            std::make_shared<websocket_session>(
                stream_.release_socket(), state_)->run();
            return;
        }
        handle_request();
    }

    void handle_request() {
        if (req_.method() != http::verb::get) {
            send_response(http::status::bad_request, "Unknown HTTP method");
            return;
        }

        std::string path = state_->doc_root();
        std::string target = std::string(req_.target());
        if (target == "/") target = "/index.html";
        path += target;

        std::string body = read_file(path);
        if (body.empty()) {
            send_response(http::status::not_found, "File not found");
            return;
        }

        http::response<http::string_body> res{http::status::ok, req_.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(target));
        res.keep_alive(req_.keep_alive());
        res.body() = std::move(body);
        res.prepare_payload();
        send_response(std::move(res));
    }

    void send_response(http::status status, beast::string_view message) {
        http::response<http::string_body> res{status, req_.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/plain");
        res.keep_alive(req_.keep_alive());
        res.body() = std::string(message);
        res.prepare_payload();
        send_response(std::move(res));
    }

    void send_response(http::response<http::string_body>&& res) {
        auto sp = std::make_shared<http::response<http::string_body>>(std::move(res));
        http::async_write(stream_, *sp,
            [self = shared_from_this(), sp](beast::error_code ec, std::size_t) {
                self->on_write(ec, sp->need_eof());
            });
    }

    void on_write(beast::error_code ec, bool close) {
        if (ec) return fail(ec, "write");
        if (close) return do_close();
        do_read();
    }

    void do_close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }

public:
    http_session(tcp::socket&& socket, std::shared_ptr<shared_state> const& state)
        : stream_(std::move(socket)), state_(state) {}

    void run() { do_read(); }
};

#endif
```

### main.cpp - 主程序

```cpp
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <thread>
#include <vector>

#include "shared_state.hpp"
#include "http_session.hpp"

namespace beast = boost::beast;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class listener : public std::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::shared_ptr<shared_state> state_;

    void do_accept() {
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(&listener::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket) {
        if (!ec) {
            std::make_shared<http_session>(std::move(socket), state_)->run();
        }
        do_accept();
    }

public:
    listener(net::io_context& ioc, tcp::endpoint endpoint,
             std::shared_ptr<shared_state> const& state)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)), state_(state) {
        beast::error_code ec;
        acceptor_.open(endpoint.protocol(), ec);
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        acceptor_.bind(endpoint, ec);
        acceptor_.listen(net::socket_base::max_listen_connections, ec);
    }

    void run() { do_accept(); }
};

int main(int argc, char* argv[]) {
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(argc > 1 ? std::atoi(argv[1]) : 8080);
    auto const doc_root = std::string(argc > 2 ? argv[2] : "./www");
    auto const threads = std::max<int>(1, argc > 3 ? std::atoi(argv[3]) : 4);

    std::cout << "Chat server starting on port " << port << "...\n";

    net::io_context ioc{threads};
    auto state = std::make_shared<shared_state>(doc_root);

    std::make_shared<listener>(ioc, tcp::endpoint{address, port}, state)->run();

    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait([&ioc](beast::error_code const&, int) {
        std::cout << "\nShutting down...\n";
        ioc.stop();
    });

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i) {
        v.emplace_back([&ioc] { ioc.run(); });
    }
    ioc.run();

    for (auto& t : v) t.join();
    return 0;
}
```

### www/index.html - 聊天页面

```html
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WebSocket Chat</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        .chat-container {
            background: white;
            border-radius: 16px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            width: 100%;
            max-width: 500px;
            overflow: hidden;
        }
        .chat-header {
            background: #667eea;
            color: white;
            padding: 20px;
            text-align: center;
        }
        .chat-header h1 { font-size: 1.5em; }
        .chat-header p { opacity: 0.8; font-size: 0.9em; margin-top: 5px; }
        #login-form, #chat-area { padding: 20px; }
        #chat-area { display: none; }
        .input-group { display: flex; gap: 10px; margin-top: 15px; }
        input[type="text"] {
            flex: 1;
            padding: 12px 16px;
            border: 2px solid #e0e0e0;
            border-radius: 8px;
            font-size: 16px;
        }
        input[type="text"]:focus { outline: none; border-color: #667eea; }
        button {
            padding: 12px 24px;
            background: #667eea;
            color: white;
            border: none;
            border-radius: 8px;
            cursor: pointer;
            font-size: 16px;
        }
        button:hover { background: #5a6fd6; }
        #messages {
            height: 350px;
            overflow-y: auto;
            border: 1px solid #e0e0e0;
            border-radius: 8px;
            padding: 15px;
            background: #f9f9f9;
        }
        .message {
            margin-bottom: 12px;
            padding: 10px 14px;
            border-radius: 12px;
            max-width: 85%;
            word-wrap: break-word;
        }
        .message.own { background: #667eea; color: white; margin-left: auto; }
        .message.other { background: white; border: 1px solid #e0e0e0; }
        .message .user { font-weight: bold; font-size: 0.85em; margin-bottom: 4px; }
        .message.own .user { color: rgba(255,255,255,0.8); }
        .message.other .user { color: #667eea; }
        .message.system {
            background: #fff3cd;
            color: #856404;
            text-align: center;
            font-style: italic;
            max-width: 100%;
        }
    </style>
</head>
<body>
    <div class="chat-container">
        <div class="chat-header">
            <h1>WebSocket Chat</h1>
            <p>Real-time multi-user chat powered by Boost.Beast</p>
        </div>
        <div id="login-form">
            <p>Enter your username to join:</p>
            <div class="input-group">
                <input type="text" id="username" placeholder="Username" maxlength="20">
                <button onclick="joinChat()">Join</button>
            </div>
        </div>
        <div id="chat-area">
            <div id="messages"></div>
            <div class="input-group">
                <input type="text" id="message" placeholder="Type a message..."
                       onkeypress="if(event.key==='Enter')sendMessage()">
                <button onclick="sendMessage()">Send</button>
            </div>
        </div>
    </div>
    <script>
        let ws, username;
        function joinChat() {
            username = document.getElementById('username').value.trim();
            if (!username) { alert('Please enter a username'); return; }
            const protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
            ws = new WebSocket(`${protocol}//${location.host}/`);
            ws.onopen = () => {
                ws.send(username);
                document.getElementById('login-form').style.display = 'none';
                document.getElementById('chat-area').style.display = 'block';
                document.getElementById('message').focus();
            };
            ws.onmessage = (e) => addMessage(JSON.parse(e.data));
            ws.onclose = () => addMessage({ type: 'system', text: 'Disconnected' });
        }
        function sendMessage() {
            const input = document.getElementById('message');
            const text = input.value.trim();
            if (text && ws && ws.readyState === WebSocket.OPEN) {
                ws.send(text);
                input.value = '';
            }
        }
        function addMessage(data) {
            const messages = document.getElementById('messages');
            const div = document.createElement('div');
            div.className = 'message';
            if (data.type === 'join') {
                div.className += ' system';
                div.textContent = `${data.user} joined`;
            } else if (data.type === 'leave') {
                div.className += ' system';
                div.textContent = `${data.user} left`;
            } else if (data.type === 'system') {
                div.className += ' system';
                div.textContent = data.text;
            } else {
                div.className += data.user === username ? ' own' : ' other';
                div.innerHTML = `<div class="user">${data.user}</div><div>${data.text}</div>`;
            }
            messages.appendChild(div);
            messages.scrollTop = messages.scrollHeight;
        }
        document.getElementById('username').addEventListener('keypress', (e) => {
            if (e.key === 'Enter') joinChat();
        });
    </script>
</body>
</html>
```

### 编译和运行

```bash
# 编译
mkdir build && cd build
cmake ..
make

# 运行
./chat_server 8080 ../www 4

# 访问 http://localhost:8080
```

### 架构说明

```
┌─────────────────────────────────────┐
│           listener                   │
│    (接受 TCP 连接)                   │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│         http_session                 │
│  (处理 HTTP 请求，检测 WS 升级)      │
└──────────────┬──────────────────────┘
               │
    ┌──────────┼──────────┐
    │          │          │
┌───▼───┐ ┌───▼───┐ ┌────▼────┐
│ WS    │ │Static │ │  404    │
│Session│ │Files  │ │Response │
└───┬───┘ └───────┘ └─────────┘
    │
┌───▼───────────────┐
│   shared_state    │
│ (广播消息到所有    │
│  WebSocket 连接)  │
└───────────────────┘
```

```{tip}
**Beast 使用建议：**
1. 使用 `tcp_stream` 而非原始 socket（提供超时支持）
2. 正确管理对象生命周期（使用 `shared_from_this()`）
3. WebSocket 握手前验证升级请求
4. 生产环境添加 SSL/TLS 支持
```

## 相关文档

- [Boost.Asio](boost_asio.md) - Asio 异步 I/O 库
- [Boost 库概览](boost.md) - 回到 Boost 主页
