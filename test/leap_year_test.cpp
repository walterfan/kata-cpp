#include <gtest/gtest.h>

class LeapYearCalendar {
public:
    bool isLeap(int year) {
        if (year % 4 != 0) {
            return false;
        } else if (year % 100 != 0) {
            return true;
        } else if (year % 400 != 0) {
            return false;
        } else {
            return true;
        }
    }
};

class LeapYearParameterizedTestFixture :public ::testing::TestWithParam<int> {
protected:
    LeapYearCalendar leapYearCalendar;
};

TEST_P(LeapYearParameterizedTestFixture, OddYearsAreNotLeapYears) {
    int year = GetParam();
    ASSERT_FALSE(leapYearCalendar.isLeap(year));
}

INSTANTIATE_TEST_CASE_P(
        LeapYearTests,
        LeapYearParameterizedTestFixture,
        ::testing::Values(
                1, 711, 1989, 2013
        ));
