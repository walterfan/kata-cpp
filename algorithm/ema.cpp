#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Function to calculate the exponential moving average of a stream of data
double exponentialMovingAverage(const vector<double>& data, int period) {
    if (period <= 0 || period > data.size()) {
        throw invalid_argument("Invalid period");
    }

    double multiplier = 2.0 / (period + 1);
    double ema = data[0];

    for (int i = 1; i < data.size(); i++) {
        ema = (data[i] - ema) * multiplier + ema;
    }

    return ema;
}

// Function to judge the trend based on the exponential moving average
string judgeTrend(const vector<double>& data, int period) {
    double ema = exponentialMovingAverage(data, period);
    double current = data.back();

    if (current > ema) {
        return "Upward trend";
    } else if (current < ema) {
        return "Downward trend";
    } else {
        return "No trend";
    }
}

int main() {
    vector<double> data = {6, 5.2, 4.3, 3.5, 2.7, 1.9, 2.1, 12.2, 11.3, 10.5, 7.6};
    int period = 3;

    for (auto& value : data) {
        cout << "Value: " << value << ", Trend: " << judgeTrend(data, period) << endl;
    }

    return 0;
}
