#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include "test_class.h"

using namespace std;
using namespace testing;

class MockFailHandler {
public:
    static int sHandleCount;
    virtual void handleMaasFail1() {
        ++sHandleCount;
    };
    void handleMaasFail2() {
        ++sHandleCount;
    };
    virtual ~MockFailHandler() = default;
};


struct StudentComparator {
    bool operator()(const Student& lhs, const Student& rhs) const {
        return std::tie(lhs.m_id, lhs.m_name) <
               std::tie(rhs.m_id, rhs.m_name);
    }
};


int MockFailHandler::sHandleCount = 0;

TEST(QuickTest, testCrash)
{
    MockFailHandler* pFailHandler = nullptr;

    pFailHandler = new MockFailHandler();
    delete pFailHandler;
    pFailHandler->handleMaasFail2();
    //pFailHandler->handleMaasFail1();
}


TEST(QuickTest, testFormat)
{
    uint8_t a = 205;
    cout << "a=" << static_cast<char>(a) << ", "<< static_cast<int>(a) <<" --> 0x" << std::hex << static_cast<int>(a) <<endl;
}

TEST(QuickTest, testComparator)
{
    Student s1("1", "alice");
    Student s2("2", "bob");
    Student s3("1", "alice");

    StudentComparator cmp;
    ASSERT_TRUE(cmp(s1, s2));
    ASSERT_FALSE(cmp(s1, s3));


}

