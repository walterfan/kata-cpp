#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>


using namespace std;
using namespace testing;

struct BandwidthInfo {
    BandwidthInfo(): mMinBitrate(0)
        , mMaxBitrate(0)
        , mCurrentBitrate(0)
        , mAllocatedBitrate(0) {}

    BandwidthInfo(uint32_t minBw, uint32_t maxBw, uint32_t currentBw)
        : mMinBitrate(minBw)
        , mMaxBitrate(maxBw)
        , mCurrentBitrate(currentBw)
        , mAllocatedBitrate(0) {}

    uint32_t mMinBitrate;
    uint32_t mMaxBitrate;
    uint32_t mCurrentBitrate;
    uint32_t mAllocatedBitrate;
};

void allocate(const std::shared_ptr<BandwidthInfo>& bwInfo, uint32_t value) {
    bwInfo->mAllocatedBitrate = value;
}


TEST(StructTest, testInit)
{
    auto bwInfo = std::make_shared<BandwidthInfo>(BandwidthInfo({0, 0, 0}));
    ASSERT_EQ(bwInfo->mMinBitrate, 0);
    allocate(bwInfo, 10);
    cout << "mAllocatedBitrate=" << bwInfo->mAllocatedBitrate << endl;
}


