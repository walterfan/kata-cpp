# JsonCpp

JsonCpp 是常用的 C++ JSON 解析库。

## 安装

```bash
# Ubuntu
sudo apt install libjsoncpp-dev

# macOS
brew install jsoncpp

# 编译
g++ -ljsoncpp program.cpp
```

## 解析 JSON

```cpp
#include <json/json.h>
#include <fstream>

// 从字符串解析
void parse_from_string() {
    std::string json_str = R"({
        "name": "John",
        "age": 30,
        "active": true,
        "scores": [90, 85, 92]
    })";
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream stream(json_str);
    
    if (Json::parseFromStream(builder, stream, &root, &errors)) {
        std::string name = root["name"].asString();
        int age = root["age"].asInt();
        bool active = root["active"].asBool();
        
        for (const auto& score : root["scores"]) {
            std::cout << score.asInt() << "\n";
        }
    }
}

// 从文件解析
void parse_from_file() {
    std::ifstream file("data.json");
    Json::Value root;
    file >> root;
    
    // 使用 root...
}
```

## 创建 JSON

```cpp
#include <json/json.h>

Json::Value create_json() {
    Json::Value root;
    
    // 基本类型
    root["name"] = "John";
    root["age"] = 30;
    root["active"] = true;
    root["salary"] = 50000.50;
    root["nullable"] = Json::nullValue;
    
    // 数组
    Json::Value scores(Json::arrayValue);
    scores.append(90);
    scores.append(85);
    scores.append(92);
    root["scores"] = scores;
    
    // 嵌套对象
    Json::Value address;
    address["city"] = "New York";
    address["zip"] = "10001";
    root["address"] = address;
    
    return root;
}
```

## 输出 JSON

```cpp
void output_json(const Json::Value& root) {
    // 紧凑格式
    Json::FastWriter fast_writer;
    std::string compact = fast_writer.write(root);
    
    // 格式化输出
    Json::StyledWriter styled_writer;
    std::string pretty = styled_writer.write(root);
    
    // 使用 StreamWriter (推荐)
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "  ";
    std::string output = Json::writeString(builder, root);
    
    // 写入文件
    std::ofstream file("output.json");
    file << root;
}
```

## 访问值

```cpp
void access_values(const Json::Value& root) {
    // 安全访问（带默认值）
    std::string name = root.get("name", "unknown").asString();
    int age = root.get("age", 0).asInt();
    
    // 检查存在性
    if (root.isMember("email")) {
        std::string email = root["email"].asString();
    }
    
    // 检查类型
    if (root["age"].isInt()) {
        int age = root["age"].asInt();
    }
    
    // 遍历对象
    for (const auto& key : root.getMemberNames()) {
        std::cout << key << ": " << root[key] << "\n";
    }
    
    // 遍历数组
    for (Json::ArrayIndex i = 0; i < root["scores"].size(); ++i) {
        std::cout << root["scores"][i].asInt() << "\n";
    }
}
```

## 错误处理

```cpp
bool safe_parse(const std::string& json_str, Json::Value& root) {
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream stream(json_str);
    
    if (!Json::parseFromStream(builder, stream, &root, &errors)) {
        std::cerr << "Parse error: " << errors << "\n";
        return false;
    }
    return true;
}
```

## 替代库：nlohmann/json

nlohmann/json 是现代 C++ 风格的 JSON 库，更易用。

```cpp
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void nlohmann_example() {
    // 解析
    json j = json::parse(R"({"name": "John", "age": 30})");
    
    // 访问
    std::string name = j["name"];
    int age = j["age"];
    
    // 创建
    json j2 = {
        {"name", "Jane"},
        {"age", 25},
        {"scores", {90, 85, 92}}
    };
    
    // 序列化
    std::string output = j2.dump(2);  // 缩进 2 空格
    
    // 迭代
    for (auto& [key, value] : j.items()) {
        std::cout << key << ": " << value << "\n";
    }
}
```

```{tip}
JSON 库选择：
- **JsonCpp**: 稳定成熟，广泛使用
- **nlohmann/json**: 现代 C++，API 更友好
- **RapidJSON**: 高性能，适合大文件
- **simdjson**: 极高性能，使用 SIMD
```
