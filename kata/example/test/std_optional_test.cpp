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

class Foo {
public:
    Foo(int id): mId(id) {}
    int mId;
};

std::ostream& operator<<(std::ostream& os, const Foo& foo) {
    os << foo.mId;
    return os;
}

constexpr const char* NOT_AVAILABLE = "NA";


std::ostream& operator<<(std::ostream& os, std::nullopt_t opt ) {

    if (os.good())
    {
        os << NOT_AVAILABLE;
    }

    return os;
}


template<typename T>
std::ostream& operator<<(std::ostream& os, std::optional<T> const& opt) {

    if (os.good())
    {
        if (!opt)
            os << NOT_AVAILABLE ;
        else
            os << *opt ;
    }

    return os;
}



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

TEST(OptionalTest, printOptional)
{
    auto age0 = std::optional<uint32_t>(22);
    auto age1 = std::optional<Foo>(Foo(1));
    auto age2 = std::optional<Foo>();
    std::optional<string> age3 = std::nullopt;



    cout << "age0=" << age0 << ", age1=" << age1 << ", age2=" << age2 << ", age3=" << age3 << endl;

}