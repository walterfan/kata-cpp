#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <set>

using namespace std;
using namespace testing;

struct User {
    User(string id, string name):m_id(id), m_name(name) {}
    string m_id;
    string m_name;
    const std::string& get_id() const
    {
        return m_id;
    }
};

struct UserCmparator {
    bool operator()(const User& lhs, const User& rhs) const {
        return lhs.get_id() != rhs.get_id();
    }
};



TEST(SetTest, insert){
    set<User,UserCmparator> user_set;
    auto ret1 = user_set.insert(User("1", "alice"));
    auto ret2 = user_set.insert(User("1", "bob"));
    auto ret3 = user_set.insert(User("3", "carl"));

    for(const auto& u: user_set) {
        std::cout << u.m_id << ", " << u.m_name<< endl;
    };


    ASSERT_TRUE(ret1.first != user_set.end());
    ASSERT_TRUE(ret1.second);

    ASSERT_TRUE(ret2.first != user_set.end());
    ASSERT_FALSE(ret2.second);

    ASSERT_TRUE(ret3.first != user_set.end());
    ASSERT_TRUE(ret3.second);

    ASSERT_EQ(user_set.size(), 2);

}
