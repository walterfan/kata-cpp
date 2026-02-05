# HTTP 客户端

## 使用 libcurl

libcurl 是最常用的 HTTP 客户端库。

### 安装

```bash
# Ubuntu
sudo apt install libcurl4-openssl-dev

# macOS
brew install curl

# 编译
g++ -lcurl program.cpp
```

### 简单 GET 请求

```cpp
#include <curl/curl.h>
#include <string>

// 回调函数
static size_t write_callback(char* ptr, size_t size, size_t nmemb, 
                             std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string http_get(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl_easy_init failed");
    }
    
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);  // 跟随重定向
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);        // 超时 30 秒
    
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    curl_easy_cleanup(curl);
    return response;
}
```

### POST 请求

```cpp
std::string http_post(const std::string& url, const std::string& data) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("curl_easy_init failed");
    }
    
    std::string response;
    
    // 设置请求头
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }
    
    return response;
}
```

### RAII 封装

```cpp
class CurlHandle {
    CURL* handle_;
    
public:
    CurlHandle() : handle_(curl_easy_init()) {
        if (!handle_) {
            throw std::runtime_error("curl_easy_init failed");
        }
    }
    
    ~CurlHandle() {
        curl_easy_cleanup(handle_);
    }
    
    CurlHandle(const CurlHandle&) = delete;
    CurlHandle& operator=(const CurlHandle&) = delete;
    
    CURL* get() const { return handle_; }
    
    template<typename T>
    CurlHandle& setopt(CURLoption option, T value) {
        curl_easy_setopt(handle_, option, value);
        return *this;
    }
    
    void perform() {
        CURLcode res = curl_easy_perform(handle_);
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }
    }
};

// 全局初始化
class CurlGlobal {
public:
    CurlGlobal() { curl_global_init(CURL_GLOBAL_ALL); }
    ~CurlGlobal() { curl_global_cleanup(); }
};
static CurlGlobal curl_global;
```

### 异步请求（多线程）

```cpp
#include <curl/curl.h>
#include <future>
#include <vector>

class AsyncHttpClient {
public:
    std::future<std::string> get_async(const std::string& url) {
        return std::async(std::launch::async, [url]() {
            return http_get(url);
        });
    }
    
    std::vector<std::string> get_multiple(
        const std::vector<std::string>& urls) 
    {
        std::vector<std::future<std::string>> futures;
        for (const auto& url : urls) {
            futures.push_back(get_async(url));
        }
        
        std::vector<std::string> results;
        for (auto& f : futures) {
            results.push_back(f.get());
        }
        return results;
    }
};
```

### curl_multi 接口

```cpp
// 更高效的异步方式
void multi_request() {
    CURLM* multi = curl_multi_init();
    
    // 添加多个请求
    CURL* curl1 = curl_easy_init();
    curl_easy_setopt(curl1, CURLOPT_URL, "http://example.com/1");
    curl_multi_add_handle(multi, curl1);
    
    CURL* curl2 = curl_easy_init();
    curl_easy_setopt(curl2, CURLOPT_URL, "http://example.com/2");
    curl_multi_add_handle(multi, curl2);
    
    // 执行
    int running = 0;
    do {
        curl_multi_perform(multi, &running);
        if (running) {
            curl_multi_wait(multi, nullptr, 0, 1000, nullptr);
        }
    } while (running);
    
    // 清理
    curl_multi_remove_handle(multi, curl1);
    curl_multi_remove_handle(multi, curl2);
    curl_easy_cleanup(curl1);
    curl_easy_cleanup(curl2);
    curl_multi_cleanup(multi);
}
```

## 使用 Boost.Beast

Boost.Beast 提供现代 C++ 风格的 HTTP 支持。

```cpp
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

std::string beast_http_get(const std::string& host, 
                           const std::string& path) {
    net::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);
    
    // 解析主机名
    auto results = resolver.resolve(host, "80");
    
    // 连接
    stream.connect(results);
    
    // 发送请求
    http::request<http::string_body> req{http::verb::get, path, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "Beast");
    http::write(stream, req);
    
    // 读取响应
    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);
    
    // 关闭连接
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
    return res.body();
}
```

## 使用 httplib（头文件库）

```cpp
// https://github.com/yhirose/cpp-httplib
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

void httplib_example() {
    httplib::Client cli("https://api.example.com");
    
    // GET
    auto res = cli.Get("/path");
    if (res && res->status == 200) {
        std::cout << res->body << "\n";
    }
    
    // POST JSON
    httplib::Headers headers = {
        {"Content-Type", "application/json"}
    };
    auto res2 = cli.Post("/api", headers, R"({"key": "value"})", 
                         "application/json");
}
```

```{tip}
HTTP 客户端库选择：
- **libcurl**: 功能全面，跨平台，C 接口
- **Boost.Beast**: 现代 C++，与 Asio 集成
- **cpp-httplib**: 单头文件，简单易用
- **cpr**: libcurl 的 C++ 封装
```
