# GoogleTest

GoogleTest 是 Google 开源的 C++ 测试框架。

## 安装

```bash
# Ubuntu
sudo apt install libgtest-dev

# macOS
brew install googletest

# 使用 CMake FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
```

## 基本测试

```cpp
#include <gtest/gtest.h>

// 简单测试
TEST(MathTest, Addition) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_NE(1 + 1, 3);
}

// 浮点数比较
TEST(MathTest, FloatingPoint) {
    EXPECT_FLOAT_EQ(1.0f / 3.0f, 0.333333f);
    EXPECT_NEAR(1.0 / 3.0, 0.333333, 0.0001);
}

// 字符串比较
TEST(StringTest, Compare) {
    std::string s = "hello";
    EXPECT_STREQ(s.c_str(), "hello");
    EXPECT_STRNE(s.c_str(), "world");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

## 断言类型

```cpp
// EXPECT_* - 失败后继续执行
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);
EXPECT_EQ(val1, val2);
EXPECT_NE(val1, val2);
EXPECT_LT(val1, val2);
EXPECT_LE(val1, val2);
EXPECT_GT(val1, val2);
EXPECT_GE(val1, val2);

// ASSERT_* - 失败后立即返回
ASSERT_TRUE(condition);
ASSERT_EQ(val1, val2);
// ... 同上

// 异常测试
EXPECT_THROW(statement, ExceptionType);
EXPECT_ANY_THROW(statement);
EXPECT_NO_THROW(statement);

// 死亡测试
EXPECT_DEATH(statement, regex);
EXPECT_EXIT(statement, predicate, regex);
```

## 测试夹具 (Test Fixture)

```cpp
class StackTest : public ::testing::Test {
protected:
    void SetUp() override {
        stack.push(1);
        stack.push(2);
    }
    
    void TearDown() override {
        // 清理
    }
    
    std::stack<int> stack;
};

TEST_F(StackTest, Pop) {
    EXPECT_EQ(stack.top(), 2);
    stack.pop();
    EXPECT_EQ(stack.top(), 1);
}

TEST_F(StackTest, Size) {
    EXPECT_EQ(stack.size(), 2);
}
```

## 参数化测试

```cpp
// 值参数化
class AddTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {};

TEST_P(AddTest, Addition) {
    auto [a, b, expected] = GetParam();
    EXPECT_EQ(a + b, expected);
}

INSTANTIATE_TEST_SUITE_P(
    MathTests,
    AddTest,
    ::testing::Values(
        std::make_tuple(1, 2, 3),
        std::make_tuple(0, 0, 0),
        std::make_tuple(-1, 1, 0)
    )
);

// 类型参数化
template<typename T>
class TypedTest : public ::testing::Test {};

using MyTypes = ::testing::Types<int, float, double>;
TYPED_TEST_SUITE(TypedTest, MyTypes);

TYPED_TEST(TypedTest, Works) {
    TypeParam value = 1;
    EXPECT_EQ(value, 1);
}
```

## Mock (GoogleMock)

```cpp
#include <gmock/gmock.h>

class Database {
public:
    virtual ~Database() = default;
    virtual bool connect(const std::string& url) = 0;
    virtual std::string query(const std::string& sql) = 0;
};

class MockDatabase : public Database {
public:
    MOCK_METHOD(bool, connect, (const std::string& url), (override));
    MOCK_METHOD(std::string, query, (const std::string& sql), (override));
};

TEST(ServiceTest, UsesDatabase) {
    MockDatabase db;
    
    // 设置期望
    EXPECT_CALL(db, connect("localhost"))
        .Times(1)
        .WillOnce(::testing::Return(true));
    
    EXPECT_CALL(db, query(::testing::HasSubstr("SELECT")))
        .WillOnce(::testing::Return("result"));
    
    // 测试代码
    Service service(&db);
    service.run();
}
```

## 高级特性

### 测试过滤

```bash
# 运行特定测试
./test --gtest_filter=MathTest.*
./test --gtest_filter=*Addition*
./test --gtest_filter=-*Slow*  # 排除

# 列出所有测试
./test --gtest_list_tests
```

### 输出格式

```bash
# XML 输出
./test --gtest_output=xml:result.xml

# JSON 输出 (需要 1.10+)
./test --gtest_output=json:result.json
```

### 重复运行

```bash
# 重复运行检测不稳定测试
./test --gtest_repeat=100
./test --gtest_shuffle
```

## CMake 集成

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(MyProject)

enable_testing()

add_executable(my_tests test.cpp)
target_link_libraries(my_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(my_tests)
```

```{tip}
GoogleTest 最佳实践：
1. 测试名称要清晰描述测试内容
2. 每个测试只测试一件事
3. 使用测试夹具减少重复
4. 使用 Mock 隔离依赖
5. CI 中启用 shuffle 检测测试间依赖
```
