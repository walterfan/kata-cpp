#include <iostream>
#include <vector>

using namespace std;

// Function to calculate the Simple Moving Average (SMA)
double calculateSMA(vector<double> data, int n) {
    double sum = 0;
    for (int i = 0; i < n; i++) {
        sum += data[data.size() - i - 1];
    }
    return sum / n;
}

// Function to calculate the Triple Moving Average (TMA)
double calculateTMA(vector<double> data, int n1, int n2, int n3) {
    double sma1 = calculateSMA(data, n1);
    double sma2 = calculateSMA(data, n2);
    double sma3 = calculateSMA(data, n3);
    return (sma1 + 2 * sma2 + sma3) / 4;
}

// Function to judge the trend using crossover and TMA
void judgeTrend(vector<double> prices, int n1, int n2, int n3) {
    double tma = calculateTMA(prices, n1, n2, n3);
    double sma1 = calculateSMA(prices, n1);
    double sma2 = calculateSMA(prices, n2);

    if (sma1 > sma2 && prices.back() > tma) {
        cout << "The trend is UP" << endl;
    } else if (sma1 < sma2 && prices.back() < tma) {
        cout << "The trend is DOWN" << endl;
    } else {
        cout << "The trend is FLAT" << endl;
    }
}

// Main function to test the trend judging function
int main() {
    // Example data
    vector<double> prices = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200, 190, 180, 170, 160, 150};

    // Judge the trend using crossover and TMA with parameters (5, 10, 15)
    judgeTrend(prices, 5, 10, 15);

    return 0;
}
