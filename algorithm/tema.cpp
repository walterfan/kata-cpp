#include <iostream>
#include <vector>
#include <numeric>
#include <cmath>

using namespace std;

vector<double> EMA(const vector<double>& data, int period) {
    vector<double> weights(period);
    double alpha = 2.0 / (period + 1.0);
    double denominator = accumulate(weights.begin(), weights.end(), 0.0);
    for (int i = 0; i < period; ++i) {
        weights[i] = pow(1.0 - alpha, i);
    }
    vector<double> ema(data.size() - period + 1);
    for (int i = period - 1; i < data.size(); ++i) {
        ema[i - period + 1] = inner_product(data.begin() + i - period + 1, data.begin() + i + 1, weights.begin(), 0.0) / denominator;
    }
    return ema;
}

vector<double> TEMA(const vector<double>& data, int period) {
    vector<double> e1 = EMA(data, period);
    vector<double> e2 = EMA(e1, period);
    vector<double> e3 = EMA(e2, period);
    vector<double> tema(data.size() - 3 * period + 2);
    for (int i = 3 * period - 3; i < data.size(); ++i) {
        tema[i - 3 * period + 2] = 3.0 * e1[i - 2 * period + 1] - 3.0 * e2[i - period + 1] + e3[i];
    }
    return tema;
}

void trend_analysis(const vector<double>& data, const vector<double>& tema_values) {
    for (int i = 0; i < data.size(); ++i) {
        if (tema_values[i] > data[i]) {
            cout << tema_values[i] << " > "<< data[i] << " --> Bullish trend at index " << i << endl;
        } else if (tema_values[i] < data[i]) {
            cout << tema_values[i] << " < "<< data[i] << " --> Bearish trend at index " << i << endl;
        } else {
            cout << tema_values[i] << " == "<< data[i] << " --> No clear trend at index " << i << endl;
        }
    }
}

int main() {
    vector<double> data = {10, 15, 13, 12, 11, 9, 13, 15, 17, 19, 20, 18, 16, 15, 14, 13, 12, 11, 10, 9};
    int period = 5;
    vector<double> tema_values = TEMA(data, period);
    trend_analysis(data, tema_values);
    return 0;
}
