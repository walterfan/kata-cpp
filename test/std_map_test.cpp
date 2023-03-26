#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>
#include "test_class.h"

using namespace std;
using namespace testing;

class StdMapTest : public testing::Test
{
public:
    StdMapTest() {

    }
    virtual ~StdMapTest() { }

    virtual void SetUp() {
        mBwInfoMap.insert({"alice", make_shared<BandwidthInfo>(BandwidthInfo({1000,9000,5000, 6000}))});
    }
    virtual void TearDown()  {
        mBwInfoMap.clear();
    }

    void changeCurrentBw(const string& name, const map<string, shared_ptr<BandwidthInfo>>& map, uint32_t bitrate) {
        const auto& iter = map.find(name);
        if (iter != map.end()) {
            const auto& bwInfo = iter->second;
            if (bwInfo) {
                cout << "changeCurrentBw to " << bitrate << endl;
                bwInfo->mCurrentBitrate = bitrate;
            }
        }
    }

protected:
    map<string, shared_ptr<BandwidthInfo>> mBwInfoMap;


};


TEST_F(StdMapTest, insert){
    ASSERT_EQ(mBwInfoMap.size(), 1);
    changeCurrentBw("alice", mBwInfoMap, 1234);
    const auto& iter = mBwInfoMap.find("alice");
    ASSERT_EQ(iter->second->mCurrentBitrate, 1234);
}
