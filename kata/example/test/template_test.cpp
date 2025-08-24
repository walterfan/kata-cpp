#include "gtest/gtest.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>
#include <map>
#include "test_class.h"

using namespace std;
using namespace testing;

template<typename T>
class Metrics {
public:
    Metrics(const string& name): mMetricName(name) {}
    void setMetricName(const string& name);
    void addMetricValue(T metricValue);
    void clearMetrics();
    void dump();
private:
    string mMetricName;
    vector<T> mMetricValues;

};

template<typename T>
void Metrics<T>::setMetricName(const string& name) {
    mMetricName = name;
}

template<typename T>
void Metrics<T>::addMetricValue(T metricValue) {
    mMetricValues.push_back(metricValue);
}

template<typename T>
void Metrics<T>::clearMetrics() {
    mMetricValues.clear();
}

template<typename T>
void Metrics<T>::dump() {
    cout << mMetricName << "'s type: " << typeid(T).name() << endl;
}

TEST(MetricTest, testAddMetric)
{
    //https://en.cppreference.com/w/cpp/types/type_index
    cout << "type name refer to https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangling-type "<< endl;
    Metrics<uint32_t> metrics("keyFrames");
    metrics.dump();


    Metrics<string> strMetrics("keyFrames");
    strMetrics.dump();

    Metrics<float> floatMetrics("keyFrames");
    floatMetrics.dump();

    Metrics<uint64_t> intMetrics("keyFrames");
    intMetrics.dump();
}

