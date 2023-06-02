#include <cstdint>
#include <gtest/gtest.h>
#include <cmath>
#include <iostream>
#include <string>
#include <tuple>


static constexpr uint32_t MAX_RTX_BANDWIDTH_PERCENT = 25;
static constexpr float MAX_RTX_LOSS_RATIO = (float)MAX_RTX_BANDWIDTH_PERCENT/100;


using namespace std;

static uint32_t calculateMediaBitrate(uint32_t totalBitrate, float lossRatio) {
    float rtxLossRatio = lossRatio * std::pow(1 + lossRatio, 3);
    rtxLossRatio = std::min(rtxLossRatio, MAX_RTX_LOSS_RATIO);
    cout << "rtxLossRatio=" << rtxLossRatio << endl;
    return uint32_t(totalBitrate / (1 + rtxLossRatio));
}


class QosParameterizedTestFixture
    : public testing::TestWithParam<std::tuple<uint32_t, float>> {
protected:

};

TEST_P(QosParameterizedTestFixture, OddYearsAreNotQoss) {
    uint32_t totalBR = std::get<0>(GetParam());
    float lossRatio = std::get<1>(GetParam());
    uint32_t mediaBR = calculateMediaBitrate(totalBR, lossRatio);
    std::cout << std::dec;
    std::cout << "totalBR=" << totalBR <<", lossRatio=" << lossRatio << ",mediaBR=" << mediaBR << endl;
}


const QosTestParameters testCases[] = {
    { 1, "foo" },
    { 2, "bar" },
    { 0, "baz" },
    // Add more test cases as needed
};

// Step 5: Instantiate the test suite
INSTANTIATE_TEST_SUITE_P(MyTestSuite, QosParameterizedTestFixture, ::testing::ValuesIn(testCases));


INSTANTIATE_TEST_CASE_P(
        QosTests,
        QosParameterizedTestFixture,
        ::testing::Combine(
    ::testing::Values(900'000, 1200'000),
    ::testing::Values(0.05, 0.1, 0.2)));
