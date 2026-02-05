# OpenSSL

OpenSSL 是最常用的加密库，提供 SSL/TLS 和加密算法支持。

## 安装

```bash
# Ubuntu
sudo apt install libssl-dev

# macOS
brew install openssl

# 编译
g++ -lssl -lcrypto program.cpp
```

## 初始化

```cpp
#include <openssl/ssl.h>
#include <openssl/err.h>

// OpenSSL 1.1.0+ 自动初始化，但显式初始化更安全
void init_openssl() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
    ERR_free_strings();
}
```

## 哈希函数

```cpp
#include <openssl/evp.h>

std::string sha256(const std::string& data) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, data.c_str(), data.size());
    
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len;
    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);
    
    // 转换为十六进制字符串
    std::stringstream ss;
    for (unsigned int i = 0; i < len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << (int)hash[i];
    }
    return ss.str();
}

// 也支持 MD5, SHA1, SHA512 等
std::string md5(const std::string& data) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
    // ... 同上
}
```

## HMAC

```cpp
#include <openssl/hmac.h>

std::string hmac_sha256(const std::string& key, const std::string& data) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len;
    
    HMAC(EVP_sha256(), 
         key.c_str(), key.size(),
         (unsigned char*)data.c_str(), data.size(),
         hash, &len);
    
    std::stringstream ss;
    for (unsigned int i = 0; i < len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << (int)hash[i];
    }
    return ss.str();
}
```

## AES 加密

```cpp
#include <openssl/evp.h>
#include <openssl/rand.h>

class AES256 {
public:
    static std::vector<unsigned char> encrypt(
        const std::string& plaintext,
        const unsigned char* key,
        const unsigned char* iv) 
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
        
        std::vector<unsigned char> ciphertext(
            plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc())
        );
        int len;
        
        EVP_EncryptUpdate(ctx, ciphertext.data(), &len,
                          (unsigned char*)plaintext.c_str(), 
                          plaintext.size());
        int ciphertext_len = len;
        
        EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
        ciphertext_len += len;
        
        ciphertext.resize(ciphertext_len);
        EVP_CIPHER_CTX_free(ctx);
        
        return ciphertext;
    }
    
    static std::string decrypt(
        const std::vector<unsigned char>& ciphertext,
        const unsigned char* key,
        const unsigned char* iv) 
    {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv);
        
        std::vector<unsigned char> plaintext(ciphertext.size());
        int len;
        
        EVP_DecryptUpdate(ctx, plaintext.data(), &len,
                          ciphertext.data(), ciphertext.size());
        int plaintext_len = len;
        
        EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
        plaintext_len += len;
        
        EVP_CIPHER_CTX_free(ctx);
        
        return std::string(plaintext.begin(), 
                          plaintext.begin() + plaintext_len);
    }
    
    static void generate_key_iv(unsigned char* key, unsigned char* iv) {
        RAND_bytes(key, 32);  // 256 bits
        RAND_bytes(iv, 16);   // 128 bits
    }
};
```

## RSA 加密

```cpp
#include <openssl/rsa.h>
#include <openssl/pem.h>

class RSACrypto {
public:
    // 生成密钥对
    static void generate_keys(const std::string& pub_file, 
                              const std::string& priv_file) {
        EVP_PKEY* pkey = EVP_PKEY_new();
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
        
        EVP_PKEY_keygen_init(ctx);
        EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
        EVP_PKEY_keygen(ctx, &pkey);
        
        // 保存公钥
        FILE* pub = fopen(pub_file.c_str(), "wb");
        PEM_write_PUBKEY(pub, pkey);
        fclose(pub);
        
        // 保存私钥
        FILE* priv = fopen(priv_file.c_str(), "wb");
        PEM_write_PrivateKey(priv, pkey, nullptr, nullptr, 0, nullptr, nullptr);
        fclose(priv);
        
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
    }
    
    // 公钥加密
    static std::vector<unsigned char> encrypt(
        const std::string& plaintext,
        const std::string& pub_key_file) 
    {
        FILE* f = fopen(pub_key_file.c_str(), "rb");
        EVP_PKEY* pkey = PEM_read_PUBKEY(f, nullptr, nullptr, nullptr);
        fclose(f);
        
        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        EVP_PKEY_encrypt_init(ctx);
        
        size_t outlen;
        EVP_PKEY_encrypt(ctx, nullptr, &outlen,
                         (unsigned char*)plaintext.c_str(), 
                         plaintext.size());
        
        std::vector<unsigned char> ciphertext(outlen);
        EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen,
                         (unsigned char*)plaintext.c_str(), 
                         plaintext.size());
        
        ciphertext.resize(outlen);
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        
        return ciphertext;
    }
};
```

## SSL/TLS 客户端

```cpp
#include <openssl/ssl.h>

class SSLClient {
    SSL_CTX* ctx_;
    SSL* ssl_;
    int sock_;
    
public:
    SSLClient() {
        ctx_ = SSL_CTX_new(TLS_client_method());
        SSL_CTX_set_verify(ctx_, SSL_VERIFY_PEER, nullptr);
        SSL_CTX_set_default_verify_paths(ctx_);
    }
    
    ~SSLClient() {
        if (ssl_) SSL_free(ssl_);
        if (ctx_) SSL_CTX_free(ctx_);
        if (sock_ >= 0) close(sock_);
    }
    
    bool connect(const std::string& host, int port) {
        // 创建 TCP 连接
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        // 解析主机名...
        
        if (::connect(sock_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            return false;
        }
        
        // 建立 SSL 连接
        ssl_ = SSL_new(ctx_);
        SSL_set_fd(ssl_, sock_);
        SSL_set_tlsext_host_name(ssl_, host.c_str());  // SNI
        
        return SSL_connect(ssl_) == 1;
    }
    
    int send(const std::string& data) {
        return SSL_write(ssl_, data.c_str(), data.size());
    }
    
    std::string recv() {
        char buffer[4096];
        int n = SSL_read(ssl_, buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            return std::string(buffer);
        }
        return "";
    }
};
```

## Base64 编码

```cpp
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

std::string base64_encode(const std::string& data) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);
    
    BIO_write(bio, data.c_str(), data.size());
    BIO_flush(bio);
    
    BUF_MEM* buffer;
    BIO_get_mem_ptr(bio, &buffer);
    
    std::string result(buffer->data, buffer->length);
    BIO_free_all(bio);
    
    return result;
}
```

```{tip}
OpenSSL 使用要点：
1. 检查所有返回值和错误
2. 正确释放资源
3. 使用安全的随机数生成器
4. 保持 OpenSSL 版本更新
5. 生产环境验证证书
```
