#include "gtest/gtest.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
using namespace testing;

struct Packet
{
    uint32_t type;
    uint32_t length;
    std::vector<char> values;
};


inline std::ostream& operator<<(std::ostream& o, Packet& packet) {
    o << "type=" << packet.type;
    o << ", length=" <<packet.length;
    o << ", values=";
    for(const auto& value: packet.values) {
        o << value << " ";
    }

    return o;
}


class InitializationTest : public testing::Test
{
public:
    InitializationTest(): m_strFeature("PacketTest"), m_strTestName("") {

    }
    virtual ~InitializationTest() { }

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


TEST_F(InitializationTest, testInitByBrace)
{
    RecordTestCase("an object", "initialize it by brace", "the object was initialized");

    Packet p0{1, 2, {'1', '2'}};
    cout << p0 << endl;
    ASSERT_EQ(p0.type, 1);
    ASSERT_EQ(p0.length, 2);
    ASSERT_EQ(p0.values.size(), 2);


}

