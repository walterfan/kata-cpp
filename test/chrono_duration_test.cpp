#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <unistd.h>



using report_time_t = std::chrono::time_point<std::chrono::steady_clock>;

struct Pomodoro {

    Pomodoro(uint32_t ms) {
        mCheckInterval =  std::chrono::milliseconds(ms);
        mStart = std::chrono::steady_clock::now();
    }
    ~Pomodoro() {
        mEnd = std::chrono::steady_clock::now();
    }

    void begin() {
        mStart = std::chrono::steady_clock::now();
    }

    void end() {
        mEnd = std::chrono::steady_clock::now();
    }

    long durationMs() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mStart).count();
    }

    report_time_t mStart;
    report_time_t mEnd;

    std::chrono::milliseconds mCheckInterval{0};
    std::chrono::milliseconds mReportInterval{0};
};


TEST(ChronoDuationTest, testDuration) {
    Pomodoro pd(2500);
    pd.begin();
    usleep(50000);
    pd.end();
    ASSERT_GE(pd.durationMs() , 50);

    ASSERT_TRUE(pd.mReportInterval == std::chrono::milliseconds(0));
    ASSERT_TRUE(pd.mReportInterval == std::chrono::milliseconds::zero());

}