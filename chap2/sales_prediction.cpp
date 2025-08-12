// sales_prediction accepts no arguments
// it calculates the midwest sales prediction based on company sales
// it outputs the original company sales and the predicted midwest sales

#include <iostream>
#include <iomanip>

int main() {
    const double MIDWEST_SHARE = 0.58;
    double companySales1 = 8600000.0;
    double companySales2 = 10000000.0;
    double midwest1 = MIDWEST_SHARE * companySales1;
    double midwest2 = MIDWEST_SHARE * companySales2;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Company sales: $" << companySales1 << "\tMidwest prediction: $" << midwest1 << '\n';
    std::cout << "Company sales: $" << companySales2 << "\tMidwest prediction: $" << midwest2 << '\n';
    return 0;
}
