#include "gtest/gtest.h"
#include <cstddef>
#include <cmath>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include "test_class.h"

using namespace std;
using namespace testing;

static constexpr uint32_t MAX_RTX_BANDWIDTH_PERCENT = 25;
static constexpr float MAX_RTX_LOSS_RATIO = (float)MAX_RTX_BANDWIDTH_PERCENT/100;

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

static uint32_t calculateMediaBitrate(uint32_t totalBitrate, float lossRatio) {
    float rtxLossRatio = lossRatio * std::pow(1 + lossRatio, 3);
    rtxLossRatio = std::min(rtxLossRatio, MAX_RTX_LOSS_RATIO);
    cout << "rtxLossRatio=" << rtxLossRatio << endl;
    return totalBitrate / (1 + rtxLossRatio);
}

class QuickTestFixture : public ::testing::Test {
    // Fixture members and methods
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

TEST(QuickTest, testMapErase) {
    std::map<uint32_t, uint32_t> packetsMap;
    packetsMap[111] = 5;
    packetsMap[222] = 15;
    packetsMap[333] = 25;
    auto ret1 = packetsMap.erase(444);
    auto ret2 = packetsMap.erase(222);
    cout << "ret1=" << ret1 << ", ret2=" << ret2 << ", map size=" << packetsMap.size() << endl;
}

TEST(QuickTest, testCalculateMediaBitrate) {
    uint32_t totalBR = 1200'000;
    float lossRatio = 0.2;
    uint32_t mediaBR = calculateMediaBitrate(totalBR, lossRatio);
    cout << "mediaBR=" << mediaBR << endl;
    ASSERT_EQ(mediaBR, 960'000);
}