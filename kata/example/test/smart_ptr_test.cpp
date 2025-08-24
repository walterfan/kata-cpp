#include "gtest/gtest.h"

#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace testing;

class Packet: public std::enable_shared_from_this<Packet>
{
public:
    Packet() = default;
    Packet(uint32_t type, const string& name):mType(type), mName(name) {};
    Packet(uint32_t type, const string& name, std::vector<string> values):mType(type), mName(name), mValues(values) {};

    uint32_t mType = 0;
    string mName;
    std::vector<string> mValues;
};


class SmartPtrTest : public testing::Test
{
public:
    SmartPtrTest(): m_strFeature("PacketTest"), m_strTestName("") {

    }
    virtual ~SmartPtrTest() { }

    virtual void SetUp() {


    }
    virtual void TearDown()  {}

    virtual void RecordTestCase(string given, string when, string then, string scenario="", string checkpoints="") {
        string strClassName(::testing::UnitTest::GetInstance()->current_test_info()->test_case_name());
        string strFuncName(::testing::UnitTest::GetInstance()->current_test_info()->name());
        string strTestName = strClassName + "." + strFuncName;

        RecordProperty("feature", m_strFeature);
        RecordProperty("scenario", scenario.empty()?strTestName:scenario);
        RecordProperty("given", given);
        RecordProperty("when", when);
        RecordProperty("then", then);
        RecordProperty("checkpoints", checkpoints);
    }
protected:
    string m_strFeature;
    string m_strTestName;

};


TEST_F(SmartPtrTest, testWeakPtr)
{
    std::weak_ptr<Packet> packetPtr;

    cout << "packetPtr used_count=" << packetPtr.use_count() << endl;

    if(packetPtr.lock()) {
        cout << "packetPtr is false" << endl;
    }

}


