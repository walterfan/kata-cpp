#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include "test_class.h"

using namespace std;
using namespace testing;

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


