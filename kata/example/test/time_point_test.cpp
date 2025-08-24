#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <unistd.h>



using report_time_t = std::chrono::time_point<std::chrono::steady_clock>;

class CountdownClock {
public:
    CountdownClock() {
        mStart = std::chrono::steady_clock::now();
    }
    ~CountdownClock() {
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
};


TEST(TimePointTest, duration) {
    CountdownClock cc;
    cc.begin();
    usleep(50000);
    cc.end();
    ASSERT_GE(cc.durationMs() , 50);

}