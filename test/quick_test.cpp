#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>

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

int MockFailHandler::sHandleCount = 0;

TEST(QuickTest, testCrash)
{
    MockFailHandler* pFailHandler = nullptr;

    pFailHandler = new MockFailHandler();
    delete pFailHandler;
    pFailHandler->handleMaasFail2();
    //pFailHandler->handleMaasFail1();
}