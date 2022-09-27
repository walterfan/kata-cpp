#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <iterator>
#include "test_class.h"

using namespace std;
using namespace testing;



TEST(OptionalTest, testcase1)
{
    auto s0 =  std::nullopt;
    auto s1 = std::make_optional<Student>("1", "Alice");
    auto s2 = std::make_optional<Student>("2", "Bob");
    auto s3 = std::make_optional<Student>("1", "Alice");

    ASSERT_NE(s0, s1);
    ASSERT_NE(s1, s2);
    ASSERT_EQ(s1, s3);

}