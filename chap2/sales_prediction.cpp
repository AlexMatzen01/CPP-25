// sales_prediction accepts no arguments
// it calculates the midwest sales prediction based on company sales
// it outputs the original company sales and the predicted midwest sales

#include <iostream>
#include <iomanip>
#include <cmath>

void printSales(double sales, double prediction) {
    double salesInMillions = sales / 1000000;
    double predictionInMillions = prediction / 1000000;

    // Determine decimal places for sales
    int salesDecimals = 0;
    if (std::fmod(salesInMillions, 1.0) != 0.0) {
        salesDecimals = (std::fmod(salesInMillions * 10, 1.0) == 0.0) ? 1 : 3;
    }

    // Determine decimal places for prediction
    int predDecimals = 0;
    if (std::fmod(predictionInMillions, 1.0) != 0.0) {
        predDecimals = (std::fmod(predictionInMillions * 10, 1.0) == 0.0) ? 1 : 3;
    }

    std::cout << "If total sales are " << std::fixed << std::setprecision(salesDecimals) << salesInMillions
              << " million the Midwest division is estimated to generate $"
              << std::fixed << std::setprecision(predDecimals) << predictionInMillions << " million.\n";
}

int main() {
    const double MIDWEST_SHARE = 0.58;
    double companySales1 = 8600000.0;
    double companySales2 = 10000000.0;
    double midwest1 = MIDWEST_SHARE * companySales1;
    double midwest2 = MIDWEST_SHARE * companySales2;

    printSales(companySales1, midwest1);
    printSales(companySales2, midwest2);

    return 0;
}
