#include "gtest/gtest.h"
#include <stdint.h>
#include <string>
#include <iostream>

#include <stdint.h>
#include <time.h>

using namespace std;
using namespace testing;

struct NtpTime {
    uint32_t mMsw; // the most significant word
    uint32_t mLsw; // the least significant word

    NtpTime(uint32_t msw, uint32_t lsw)
    : mMsw(msw)
    , mLsw(lsw) {}

    uint64_t getTime() const {
        return ((uint64_t)mMsw << 32ull) | mLsw;
    }

    static NtpTime now() {
        struct timespec tp;
        clock_gettime(_CLOCK_REALTIME, &tp);
        uint32_t msw = tp.tv_sec;
        uint32_t lsw = uint32_t((tp.tv_nsec << 32ull) / 1000000000ull);
        return NtpTime(msw, lsw);
    }

    static NtpTime from(uint64_t ntp, bool netByteOrder=false) {
        uint32_t msw = ntp >> 32;
        uint32_t lsw = ntp & 0xFFFFFFFF;
        if(netByteOrder) {
            msw = htonl(msw);
            lsw = htonl(lsw);
        }
        return NtpTime(msw, lsw);
    }

    uint32_t getMid32bits() const {
        return (((mMsw & 0xFFFF) << 16ull) | ((mLsw >> 16ull) & 0xFFFF));
    }
};

class NtpTimeTest : public testing::Test
{
public:
    NtpTimeTest() {

    }
    virtual ~NtpTimeTest() { }

    virtual void SetUp() {
        mSendNtpTime = NtpTime::now().getTime();
        mReceiveNtpTime = NtpTime::from(mSendNtpTime).getTime();
    }
    virtual void TearDown()  {}


protected:
    uint64_t mSendNtpTime = 0;
    uint64_t mReceiveNtpTime = 0;


};


TEST_F(NtpTimeTest, testInitByBrace)
{
    NtpTime time1 = NtpTime::now();
    uint64_t ntp1 = time1.getTime();

    NtpTime time2 = NtpTime::from(ntp1);

    ASSERT_EQ(time1.mLsw, time2.mLsw);
    ASSERT_EQ(time1.mMsw, time2.mMsw);
}


TEST_F(NtpTimeTest, distance)
{
    NtpTime time1 = NtpTime::now();

    auto duration = 2'002'002llu;
    usleep(duration);

    NtpTime time2 = NtpTime::now();


    auto distance = time2.getTime() -  time1.getTime();
    auto distance1 = distance >> 16;
    auto distance2 = ((distance >> 32) << 16) + ((distance & 0xFFFFFFFF) >> 16);

    cout <<  time2.getTime()  << "-" << time1.getTime()
         << "=" << (distance >> 32) << " sec "
         << ", frac=" << (distance & 0xFFFFFFFF)
         << ", distance1=" << distance1
         << ", distance2=" << distance2 << " , and "
        << time2.getMid32bits() << "-"
        << time1.getMid32bits()
         << endl;
}

