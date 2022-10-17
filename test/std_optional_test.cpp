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
    ASSERT_EQ(s1.value(), s3.value());

}



TEST(OptionalTest, testcase2)
{


    auto age0 = std::optional<uint8_t>(22);
    auto age1 = std::optional<uint8_t>(24);
    auto age2 = std::optional<uint8_t>(22);
    std::optional<uint8_t> age3;

    ASSERT_NE(age0, age1);
    ASSERT_EQ(age0, age2);

    auto s1 = age0.has_value()? Student("a", "b", age0.value()):Student("a", "b");

    ASSERT_EQ(s1.get_age(), 22);
    ASSERT_FALSE(age3.has_value());


}