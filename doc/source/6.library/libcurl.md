# libcurl

libcurl 是功能强大的传输库，支持多种协议（HTTP, HTTPS, FTP 等）。

## 安装

```bash
# Ubuntu
sudo apt install libcurl4-openssl-dev

# macOS
brew install curl

# 编译
g++ -lcurl program.cpp
```

## HTTP GET

```cpp
#include <curl/curl.h>
#include <string>

size_t write_callback(char* ptr, size_t size, size_t nmemb, 
                      std::string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

std::string http_get(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
        }
        
        curl_easy_cleanup(curl);
    }
    
    return response;
}
```

## HTTP POST

```cpp
std::string http_post(const std::string& url, 
                      const std::string& data,
                      const std::string& content_type = "application/json") {
    CURL* curl = curl_easy_init();
    std::string response;
    
    if (curl) {
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, 
            ("Content-Type: " + content_type).c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    
    return response;
}
```

## 文件下载

```cpp
size_t file_write_callback(char* ptr, size_t size, size_t nmemb, 
                           FILE* file) {
    return fwrite(ptr, size, nmemb, file);
}

bool download_file(const std::string& url, const std::string& filename) {
    CURL* curl = curl_easy_init();
    FILE* file = fopen(filename.c_str(), "wb");
    
    if (curl && file) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, file_write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        
        // 显示进度
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        
        CURLcode res = curl_easy_perform(curl);
        
        fclose(file);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    return false;
}
```

## 文件上传

```cpp
bool upload_file(const std::string& url, const std::string& filename) {
    CURL* curl = curl_easy_init();
    
    if (curl) {
        curl_mime* form = curl_mime_init(curl);
        curl_mimepart* field = curl_mime_addpart(form);
        curl_mime_name(field, "file");
        curl_mime_filedata(field, filename.c_str());
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
        
        CURLcode res = curl_easy_perform(curl);
        
        curl_mime_free(form);
        curl_easy_cleanup(curl);
        
        return res == CURLE_OK;
    }
    
    return false;
}
```

## HTTPS 与证书

```cpp
void https_request(const std::string& url) {
    CURL* curl = curl_easy_init();
    
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // SSL 选项
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
        
        // 指定 CA 证书
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/path/to/ca-bundle.crt");
        
        // 客户端证书
        curl_easy_setopt(curl, CURLOPT_SSLCERT, "/path/to/client.crt");
        curl_easy_setopt(curl, CURLOPT_SSLKEY, "/path/to/client.key");
        
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}
```

## RAII 封装

```cpp
class Curl {
    CURL* handle_;
    struct curl_slist* headers_ = nullptr;
    std::string response_;
    
public:
    Curl() : handle_(curl_easy_init()) {
        if (!handle_) throw std::runtime_error("curl_easy_init failed");
        curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(handle_, CURLOPT_WRITEDATA, &response_);
    }
    
    ~Curl() {
        if (headers_) curl_slist_free_all(headers_);
        curl_easy_cleanup(handle_);
    }
    
    Curl& url(const std::string& url) {
        curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
        return *this;
    }
    
    Curl& header(const std::string& header) {
        headers_ = curl_slist_append(headers_, header.c_str());
        curl_easy_setopt(handle_, CURLOPT_HTTPHEADER, headers_);
        return *this;
    }
    
    Curl& post(const std::string& data) {
        curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, data.c_str());
        return *this;
    }
    
    Curl& timeout(long seconds) {
        curl_easy_setopt(handle_, CURLOPT_TIMEOUT, seconds);
        return *this;
    }
    
    std::string perform() {
        response_.clear();
        CURLcode res = curl_easy_perform(handle_);
        if (res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        }
        return response_;
    }
    
    long status_code() const {
        long code;
        curl_easy_getinfo(handle_, CURLINFO_RESPONSE_CODE, &code);
        return code;
    }
    
private:
    static size_t write_callback(char* ptr, size_t size, size_t nmemb, 
                                 std::string* data) {
        data->append(ptr, size * nmemb);
        return size * nmemb;
    }
};

// 使用
auto response = Curl()
    .url("https://api.example.com/data")
    .header("Content-Type: application/json")
    .header("Authorization: Bearer token")
    .post(R"({"key": "value"})")
    .timeout(30)
    .perform();
```

```{tip}
libcurl 使用要点：
1. 调用 `curl_global_init()` 初始化（程序启动时一次）
2. 使用 RAII 确保资源释放
3. 检查所有返回值
4. 生产环境验证 SSL 证书
5. 考虑使用连接池提高性能
```
