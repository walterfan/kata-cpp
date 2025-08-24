#include <iostream>
#include <vector>

using namespace std;

class LinearRegression {
private:
    vector<double> x_values;
    vector<double> y_values;
    double slope;
    double y_intercept;

public:
    void set_data(vector<double> x, vector<double> y) {
        x_values = x;
        y_values = y;
    }

    void fit() {
        int n = x_values.size();
        double sum_x = 0.0;
        double sum_y = 0.0;
        double sum_xy = 0.0;
        double sum_x_squared = 0.0;

        for (int i = 0; i < n; i++) {
            sum_x += x_values[i];
            sum_y += y_values[i];
            sum_xy += x_values[i] * y_values[i];
            sum_x_squared += x_values[i] * x_values[i];
        }

        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x_squared - sum_x * sum_x);
        y_intercept = (sum_y - slope * sum_x) / n;
    }

    double predict(double x) {
        cout << "slope=" << slope << endl;
        return slope * x + y_intercept;
    }
};

int main() {
    vector<double> x = {1.0, 2.0, 3.0, 4.0, 5.0};
    vector<double> y = {6000, 4000, 3000, 2000, 5000};

    LinearRegression lr;
    lr.set_data(x, y);
    lr.fit();

    double x_new = 6.0;
    double y_predicted = lr.predict(x_new);

    cout << "Predicted value for x = " << x_new << " is " << y_predicted << endl;

    return 0;
}
