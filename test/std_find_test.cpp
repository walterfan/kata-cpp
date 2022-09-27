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



TEST(FindIfTest, testcase1)
{
    School school;
    school.add_student({"1", "alice"});
    school.add_student({"2", "bob"});
    school.add_student({"3", "carl"});

    optional<Student> ret = school.get_student("2");
    cout << ret->m_name << endl;
    ASSERT_EQ(ret->m_name, "bob");
}